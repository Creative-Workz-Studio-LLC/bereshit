// ============================================================================
// METADATA
// ============================================================================
// Analyze Stopping Point - Determine if this is a natural place to stop
// Part of: recognize-stopping-point skill
// Author: Nova Dawn (CPI-SI)
// Created: 2025-11-04
// Purpose: Analyze context to determine if current moment is natural stopping point
//
// Usage:
//   analyze-stopping-point         # Human-readable output
//   analyze-stopping-point --json  # JSON output
//
// Exit codes:
//   0: Natural stopping point detected
//   1: Not a stopping point
//   2: Error
//
// Health Scoring: Base100 - Context gathering and signal analysis

package main

// ============================================================================
// SETUP - Imports, Dependencies, Globals
// ============================================================================

import (
	"encoding/json"
	"fmt"
	"os"
	"os/exec"
	"path/filepath"
	"strings"
	"time"
)

const (
	BinDir = ".claude/system/bin"
)

// Stopping signal types and strengths
type SignalType string
type SignalStrength string

const (
	NaturalMilestone SignalType = "natural_milestone"
	CleanBreak       SignalType = "clean_break"
	EndOfDay         SignalType = "end_of_day"
	QualityDip       SignalType = "quality_dip"
	DurationLimit    SignalType = "duration_limit"

	Strong   SignalStrength = "strong"
	Moderate SignalStrength = "moderate"
	Weak     SignalStrength = "weak"
)

// StoppingSignal represents an indicator for stopping
type StoppingSignal struct {
	Type      SignalType     `json:"type"`
	Strength  SignalStrength `json:"strength"`
	Indicator string         `json:"indicator"`
}

// SessionContext holds current session state
type SessionContext struct {
	ElapsedMinutes      int    `json:"elapsed_minutes"`
	TasksCompleted      int    `json:"tasks_completed"`
	NotesAdded          int    `json:"notes_added"`
	TimeOfDay           string `json:"time_of_day"`
	IsTypicalWorkHours  bool   `json:"is_typical_work_hours"`
	LastNote            string `json:"last_note"`
}

// StoppingPointAnalysis is the complete analysis result
type StoppingPointAnalysis struct {
	IsNaturalStoppingPoint bool             `json:"is_natural_stopping_point"`
	Signals                []StoppingSignal `json:"signals"`
	Recommendation         string           `json:"recommendation"`
	Reasoning              string           `json:"reasoning"`
}

// ============================================================================
// BODY - Core Functionality
// ============================================================================

// runCommand executes a system command and returns output
func runCommand(command string, args ...string) string {
	cmd := exec.Command(command, args...)
	output, err := cmd.CombinedOutput()
	if err != nil {
		return ""
	}
	return strings.TrimSpace(string(output))
}

// getBinPath returns absolute path to a binary in system/bin
func getBinPath(binary string) string {
	homeDir, err := os.UserHomeDir()
	if err != nil {
		return filepath.Join("/home", os.Getenv("USER"), BinDir, binary)
	}
	return filepath.Join(homeDir, BinDir, binary)
}

// getTimeOfDay returns current time category
func getTimeOfDay() string {
	hour := time.Now().Hour()
	if hour >= 5 && hour < 12 {
		return "morning"
	}
	if hour >= 12 && hour < 17 {
		return "afternoon"
	}
	if hour >= 17 && hour < 21 {
		return "evening"
	}
	return "night"
}

// getSessionContext gathers current session information
func getSessionContext() SessionContext {
	// Get session status (would parse actual output in production)
	_ = runCommand(getBinPath("session-log"), "status")

	// Get circadian check
	_ = runCommand(getBinPath("session-patterns"), "check")

	// Current time analysis
	hour := time.Now().Hour()
	day := time.Now().Weekday()
	isWorkHours := (day >= time.Monday && day <= time.Friday) && (hour >= 9 && hour < 17)

	// For now, return basic context
	// Production version would parse session-log output
	return SessionContext{
		ElapsedMinutes:     0, // TODO: Parse from session-log status
		TasksCompleted:     0, // TODO: Parse from session-log status
		NotesAdded:         0, // TODO: Parse from session-log status
		TimeOfDay:          getTimeOfDay(),
		IsTypicalWorkHours: isWorkHours,
		LastNote:           "", // TODO: Parse from session-log status
	}
}

// analyzeNaturalMilestone checks for work completion signals
func analyzeNaturalMilestone(context SessionContext) *StoppingSignal {
	// Strong: Multiple tasks completed, work documented
	if context.TasksCompleted >= 3 && strings.Contains(context.LastNote, "complete") {
		return &StoppingSignal{
			Type:      NaturalMilestone,
			Strength:  Strong,
			Indicator: fmt.Sprintf("%d tasks completed, work documented", context.TasksCompleted),
		}
	}

	// Moderate: Some tasks completed
	if context.TasksCompleted >= 1 {
		return &StoppingSignal{
			Type:      NaturalMilestone,
			Strength:  Moderate,
			Indicator: fmt.Sprintf("%d task(s) completed", context.TasksCompleted),
		}
	}

	return nil
}

// analyzeCleanBreak checks for phase transition signals
func analyzeCleanBreak(context SessionContext) *StoppingSignal {
	transitionKeywords := []string{"complete", "finished", "done", "ready", "milestone"}
	lastNoteLower := strings.ToLower(context.LastNote)

	for _, keyword := range transitionKeywords {
		if strings.Contains(lastNoteLower, keyword) {
			return &StoppingSignal{
				Type:      CleanBreak,
				Strength:  Strong,
				Indicator: "Phase or component complete, natural transition point",
			}
		}
	}

	return nil
}

// analyzeEndOfDay checks for circadian/time-based signals
func analyzeEndOfDay(context SessionContext) *StoppingSignal {
	hour := time.Now().Hour()

	// Strong: Evening downtime (after 9 PM)
	if hour >= 21 {
		return &StoppingSignal{
			Type:      EndOfDay,
			Strength:  Strong,
			Indicator: fmt.Sprintf("%d:00 - Evening downtime window", hour),
		}
	}

	// Moderate: Late afternoon/early evening outside work hours
	if hour >= 17 && !context.IsTypicalWorkHours {
		return &StoppingSignal{
			Type:      EndOfDay,
			Strength:  Moderate,
			Indicator: fmt.Sprintf("%d:00 - Outside typical work hours", hour),
		}
	}

	return nil
}

// analyzeQualityDip checks for quality decline signals
func analyzeQualityDip(context SessionContext) *StoppingSignal {
	qualityDipKeywords := []string{"dipping", "declining", "tired", "errors", "mistakes", "re-reading"}
	lastNoteLower := strings.ToLower(context.LastNote)

	for _, keyword := range qualityDipKeywords {
		if strings.Contains(lastNoteLower, keyword) {
			return &StoppingSignal{
				Type:      QualityDip,
				Strength:  Strong,
				Indicator: "Quality indicators showing decline",
			}
		}
	}

	return nil
}

// analyzeDurationLimit checks for session duration signals
func analyzeDurationLimit(context SessionContext) *StoppingSignal {
	// Strong: Deep work session at 2.5+ hours
	if context.ElapsedMinutes >= 150 {
		return &StoppingSignal{
			Type:      DurationLimit,
			Strength:  Strong,
			Indicator: fmt.Sprintf("%d minutes - Extended deep work session", context.ElapsedMinutes),
		}
	}

	// Moderate: Normal session at 2 hours
	if context.ElapsedMinutes >= 120 {
		return &StoppingSignal{
			Type:      DurationLimit,
			Strength:  Moderate,
			Indicator: fmt.Sprintf("%d minutes - Approaching upper limit of normal session", context.ElapsedMinutes),
		}
	}

	return nil
}

// analyzeStoppingPoint performs complete stopping point analysis
func analyzeStoppingPoint() StoppingPointAnalysis {
	context := getSessionContext()
	var signals []StoppingSignal

	// Check each stopping point type
	if sig := analyzeNaturalMilestone(context); sig != nil {
		signals = append(signals, *sig)
	}
	if sig := analyzeCleanBreak(context); sig != nil {
		signals = append(signals, *sig)
	}
	if sig := analyzeEndOfDay(context); sig != nil {
		signals = append(signals, *sig)
	}
	if sig := analyzeQualityDip(context); sig != nil {
		signals = append(signals, *sig)
	}
	if sig := analyzeDurationLimit(context); sig != nil {
		signals = append(signals, *sig)
	}

	// Determine if natural stopping point
	hasStrongSignal := false
	moderateCount := 0

	for _, sig := range signals {
		if sig.Strength == Strong {
			hasStrongSignal = true
		} else if sig.Strength == Moderate {
			moderateCount++
		}
	}

	isNaturalStoppingPoint := hasStrongSignal || moderateCount >= 2

	// Generate recommendation and reasoning
	var recommendation, reasoning string

	if isNaturalStoppingPoint {
		recommendation = "✅ Natural stopping point detected - Good place to wrap up"

		strongSignals := []string{}
		for _, sig := range signals {
			if sig.Strength == Strong {
				strongSignals = append(strongSignals, string(sig.Type))
			}
		}

		if len(strongSignals) > 0 {
			reasoning = fmt.Sprintf("Strong signals: %s", strings.Join(strongSignals, ", "))
		} else {
			allTypes := []string{}
			for _, sig := range signals {
				allTypes = append(allTypes, string(sig.Type))
			}
			reasoning = fmt.Sprintf("Multiple moderate signals: %s", strings.Join(allTypes, ", "))
		}
	} else {
		recommendation = "➡️  Not a natural stopping point - Continue working if quality remains high"

		if len(signals) > 0 {
			indicators := []string{}
			for _, sig := range signals {
				indicators = append(indicators, sig.Indicator)
			}
			reasoning = fmt.Sprintf("Weak signals present but not compelling: %s", strings.Join(indicators, "; "))
		} else {
			reasoning = "No strong stopping signals detected"
		}
	}

	return StoppingPointAnalysis{
		IsNaturalStoppingPoint: isNaturalStoppingPoint,
		Signals:                signals,
		Recommendation:         recommendation,
		Reasoning:              reasoning,
	}
}

// formatAnalysis creates human-readable output
func formatAnalysis(analysis StoppingPointAnalysis) string {
	var output strings.Builder

	output.WriteString("\n🛑 Stopping Point Analysis\n")
	output.WriteString(strings.Repeat("━", 65) + "\n\n")
	output.WriteString(analysis.Recommendation + "\n\n")

	if len(analysis.Signals) > 0 {
		output.WriteString("📊 Signals Detected:\n\n")

		for _, signal := range analysis.Signals {
			icon := "⚪"
			if signal.Strength == Strong {
				icon = "🔴"
			} else if signal.Strength == Moderate {
				icon = "🟡"
			}

			signalType := strings.ToUpper(strings.ReplaceAll(string(signal.Type), "_", " "))
			output.WriteString(fmt.Sprintf("%s %s\n", icon, signalType))
			output.WriteString(fmt.Sprintf("   Strength: %s\n", signal.Strength))
			output.WriteString(fmt.Sprintf("   Indicator: %s\n\n", signal.Indicator))
		}
	} else {
		output.WriteString("📊 No stopping signals detected\n\n")
	}

	output.WriteString(fmt.Sprintf("💡 Reasoning:\n%s\n\n", analysis.Reasoning))

	return output.String()
}

// printUsage displays command usage information
func printUsage() {
	fmt.Println(`
Usage: analyze-stopping-point [options]

Options:
    --json      Output results as JSON
    --help      Show this help message

Examples:
    analyze-stopping-point
    analyze-stopping-point --json
`)
}

// ============================================================================
// CLOSING - Main Entry Point
// ============================================================================

func main() {
	args := os.Args[1:]

	// Check for help flag
	for _, arg := range args {
		if arg == "--help" || arg == "-h" {
			printUsage()
			os.Exit(0)
		}
	}

	// Perform analysis
	analysis := analyzeStoppingPoint()

	// Output format
	jsonOutput := false
	for _, arg := range args {
		if arg == "--json" {
			jsonOutput = true
		}
	}

	if jsonOutput {
		data, _ := json.MarshalIndent(analysis, "", "  ")
		fmt.Println(string(data))
	} else {
		fmt.Print(formatAnalysis(analysis))
	}

	// Exit code: 0 if natural stopping point, 1 if not
	if analysis.IsNaturalStoppingPoint {
		os.Exit(0)
	} else {
		os.Exit(1)
	}
}
