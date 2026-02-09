// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: session-awareness-pkg-duration
// Purpose: Session duration tracking and categorization
// Biblical: Lamentations 3:22-23 - "His compassions fail not"
// Authors: Nova Dawn
// Version: 2.0.0
// Created: 2025-12-09
//
// ═══════════════════════════════════════════════════════════════════════════

package duration

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"encoding/json"
	"fmt"
	"os"
	"time"

	"cws.studio/skills/session-awareness/scripts/pkg/config"
)

// CurrentSession represents the current session metadata
type CurrentSession struct {
	SessionID   string    `json:"session_id"`
	StartTime   time.Time `json:"start_time"`
	Compactions int       `json:"compactions,omitempty"`
}

// DurationResult holds the result of a duration check
type DurationResult struct {
	SessionID    string
	StartTime    time.Time
	Elapsed      time.Duration
	CategoryName string
	CategoryDesc string
	Guidance     string
	Minutes      int
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// --- Helpers ---

// LoadCurrentSession reads the current session file
func LoadCurrentSession(cfg *config.Config) (*CurrentSession, error) {
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

// FormatDuration returns human-readable duration
func FormatDuration(d time.Duration) string {
	hours := int(d.Hours())
	minutes := int(d.Minutes()) % 60

	if hours > 0 {
		return fmt.Sprintf("%dh %dm", hours, minutes)
	}
	return fmt.Sprintf("%d minutes", minutes)
}

// --- Core Operations ---

// Check performs a duration check on the current session
func Check(cfg *config.Config) (*DurationResult, error) {
	session, err := LoadCurrentSession(cfg)
	if err != nil {
		return nil, err
	}

	elapsed := time.Since(session.StartTime)
	minutes := int(elapsed.Minutes())
	category := cfg.GetDurationCategory(minutes)

	return &DurationResult{
		SessionID:    session.SessionID,
		StartTime:    session.StartTime,
		Elapsed:      elapsed,
		CategoryName: category.Name,
		CategoryDesc: category.Description,
		Guidance:     category.Guidance,
		Minutes:      minutes,
	}, nil
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
