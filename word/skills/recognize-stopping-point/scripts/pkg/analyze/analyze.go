// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: recognize-stopping-point-pkg-analyze
// Purpose: Stopping point analysis logic
// Biblical: Ecclesiastes 3:1 - "To every thing there is a season"
// Authors: Nova Dawn
// Version: 2.0.0
// Created: 2025-12-09
//
// ═══════════════════════════════════════════════════════════════════════════

package analyze

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"encoding/json"
	"fmt"
	"os"
	"time"

	"cws.studio/skills/recognize-stopping-point/scripts/pkg/config"
)

// CurrentSession represents the current session metadata
type CurrentSession struct {
	SessionID   string    `json:"session_id"`
	StartTime   time.Time `json:"start_time"`
	Compactions int       `json:"compactions,omitempty"`
}

// Signal represents a stopping point signal
type Signal struct {
	Type        string
	Name        string
	Weight      int
	Description string
	Decision    string
	Present     bool
	Details     string
}

// AnalysisResult holds the complete analysis
type AnalysisResult struct {
	SessionID        string
	ElapsedMinutes   int
	CurrentHour      int
	DayOfWeek        string
	Signals          []Signal
	TotalWeight      int
	Recommendation   string
	RecommendedEnd   string
	SuggestedActions []string
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// --- Helpers ---

// loadCurrentSession reads the current session file
func loadCurrentSession(cfg *config.Config) (*CurrentSession, error) {
	sessionFile := cfg.GetCurrentSessionFile()

	data, err := os.ReadFile(sessionFile)
	if err != nil {
		return nil, fmt.Errorf("no active session found: %w", err)
	}

	var session CurrentSession
	if err := json.Unmarshal(data, &session); err != nil {
		return nil, fmt.Errorf("invalid session data: %w", err)
	}

	return &session, nil
}

// analyzeEndOfDay checks circadian signals
func analyzeEndOfDay(hour int, cfg *config.Config) Signal {
	st, _ := cfg.GetStoppingType("end_of_day")
	signal := Signal{
		Type:        "end_of_day",
		Name:        st.Name,
		Weight:      st.Weight,
		Description: st.Description,
		Decision:    st.Decision,
		Present:     false,
	}

	if cfg.IsEndOfDayHour(hour) {
		if cfg.IsNightOwlProductiveHour(hour) {
			signal.Present = false
			signal.Details = fmt.Sprintf("Hour %d: Evening but within Seanje's night owl zone", hour)
		} else {
			signal.Present = true
			signal.Details = fmt.Sprintf("Hour %d: Rest time window", hour)
		}
	} else {
		signal.Details = fmt.Sprintf("Hour %d: Normal working hours", hour)
	}

	return signal
}

// analyzeDuration checks duration signals
func analyzeDuration(minutes int, cfg *config.Config, isDeepWork bool) Signal {
	st, _ := cfg.GetStoppingType("duration_limit")
	signal := Signal{
		Type:        "duration_limit",
		Name:        st.Name,
		Weight:      st.Weight,
		Description: st.Description,
		Decision:    st.Decision,
		Present:     false,
	}

	soft, hard := cfg.IsDurationLimitReached(minutes, isDeepWork)

	if hard {
		signal.Present = true
		signal.Weight = 4 // Increase weight for hard limit
		signal.Details = fmt.Sprintf("%d minutes: Hard limit reached", minutes)
	} else if soft {
		signal.Present = true
		signal.Details = fmt.Sprintf("%d minutes: Soft limit reached", minutes)
	} else {
		signal.Details = fmt.Sprintf("%d minutes: Within normal duration", minutes)
	}

	return signal
}

// signalStrength calculates overall signal strength
func signalStrength(totalWeight int) string {
	if totalWeight >= 8 {
		return "Strong"
	} else if totalWeight >= 4 {
		return "Moderate"
	}
	return "Weak"
}

// --- Core Operations ---

// Analyze performs stopping point analysis
func Analyze(cfg *config.Config, isDeepWork bool, qualityDip bool, milestoneComplete bool, cleanBreak bool) (*AnalysisResult, error) {
	session, err := loadCurrentSession(cfg)
	if err != nil {
		// Create a minimal result even without session
		now := time.Now()
		result := &AnalysisResult{
			SessionID:      "unknown",
			ElapsedMinutes: 0,
			CurrentHour:    now.Hour(),
			DayOfWeek:      now.Weekday().String(),
			Signals:        []Signal{},
			TotalWeight:    0,
			Recommendation: "No active session - cannot fully analyze",
		}

		// Still check end of day
		eodSignal := analyzeEndOfDay(now.Hour(), cfg)
		result.Signals = append(result.Signals, eodSignal)
		if eodSignal.Present {
			result.TotalWeight += eodSignal.Weight
		}

		return result, nil
	}

	now := time.Now()
	elapsed := time.Since(session.StartTime)
	minutes := int(elapsed.Minutes())

	result := &AnalysisResult{
		SessionID:      session.SessionID,
		ElapsedMinutes: minutes,
		CurrentHour:    now.Hour(),
		DayOfWeek:      now.Weekday().String(),
		Signals:        []Signal{},
		TotalWeight:    0,
	}

	// Check each stopping type
	signals := []Signal{}

	// Natural Milestone
	if milestoneComplete {
		st, _ := cfg.GetStoppingType("natural_milestone")
		signals = append(signals, Signal{
			Type:        "natural_milestone",
			Name:        st.Name,
			Weight:      st.Weight,
			Description: st.Description,
			Decision:    st.Decision,
			Present:     true,
			Details:     "Milestone completion reported",
		})
	}

	// Clean Break
	if cleanBreak {
		st, _ := cfg.GetStoppingType("clean_break")
		signals = append(signals, Signal{
			Type:        "clean_break",
			Name:        st.Name,
			Weight:      st.Weight,
			Description: st.Description,
			Decision:    st.Decision,
			Present:     true,
			Details:     "Clean break point reported",
		})
	}

	// Quality Dip
	if qualityDip {
		st, _ := cfg.GetStoppingType("quality_dip")
		signals = append(signals, Signal{
			Type:        "quality_dip",
			Name:        st.Name,
			Weight:      st.Weight,
			Description: st.Description,
			Decision:    st.Decision,
			Present:     true,
			Details:     "Quality decline reported",
		})
	}

	// End of Day
	signals = append(signals, analyzeEndOfDay(now.Hour(), cfg))

	// Duration Limit
	signals = append(signals, analyzeDuration(minutes, cfg, isDeepWork))

	// Calculate totals
	for _, s := range signals {
		result.Signals = append(result.Signals, s)
		if s.Present {
			result.TotalWeight += s.Weight
		}
	}

	// Generate recommendation
	strength := signalStrength(result.TotalWeight)
	switch strength {
	case "Strong":
		result.Recommendation = "Strong stopping signal - consider wrapping up now"
		result.SuggestedActions = []string{
			"Document current state",
			"Complete any in-progress small tasks",
			"Add final session note",
			"End session with appropriate reason",
		}
	case "Moderate":
		result.Recommendation = "Moderate stopping signal - look for next natural point"
		result.SuggestedActions = []string{
			"Complete current task",
			"Assess quality after completion",
			"Consider stopping at next milestone",
		}
	default:
		result.Recommendation = "Weak stopping signal - continue work with awareness"
		result.SuggestedActions = []string{
			"Continue current work",
			"Monitor quality",
			"Re-assess at next natural break",
		}
	}

	// Determine recommended end reason
	for _, s := range result.Signals {
		if s.Present && s.Weight >= 4 {
			result.RecommendedEnd = s.Type
			break
		}
	}
	if result.RecommendedEnd == "" && result.TotalWeight >= 4 {
		result.RecommendedEnd = "clean_break"
	}

	return result, nil
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
