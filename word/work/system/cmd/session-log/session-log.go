// ============================================================================
// METADATA
// ============================================================================
// Session Logger - Tracks session history with config inheritance
//
// For METADATA structure explanation, see: standards/code/4-block/CWS-STD-004-CODE-metadata-block.md
//
// Biblical Foundation
//
// Scripture: "Remember the days of old" - Deuteronomy 32:7 (WEB)
// Principle: Remembering faithfully enables learning and growth
// Anchor: Session history builds pattern memory for sustainable work rhythms
//
// Author: Nova Dawn (CPI-SI)
// Created: 2025-11-03
// Version: 2.0.0
// Last Modified: 2025-11-10 - Config inheritance, richer structure, correct paths
//
// Purpose: Build session history data for pattern learning and circadian awareness
//
// Usage:
//   session-log start                    # Initialize session log (with config inheritance)
//   session-log end [reason]             # Finalize session with stopping reason
//   session-log status                   # Show current session info
//   session-log note "message"           # Add note to current session
//   session-log task "task description"  # Record completed task
//
// Dependencies: system/lib/config (for config loading and inheritance)
// Health Scoring: Base100 - Config=30, Init=30, Update=30, Save=10

package main

// ============================================================================
// SETUP - Imports, Dependencies, Globals
// ============================================================================
import (
	"encoding/json"
	"fmt"
	"os"
	"path/filepath"
	"strings"
	"time"

	"system/lib/config" // Config loading and inheritance
)

// SessionLog structure - matches richer template from migration folder
type SessionLog struct {
	// Core identity (from configs)
	SessionID  string `json:"session_id"`
	InstanceID string `json:"instance_id"`
	UserID     string `json:"user_id"`

	// Timing
	StartTime   time.Time  `json:"start_time"`
	EndTime     *time.Time `json:"end_time,omitempty"`
	LastUpdated time.Time  `json:"last_updated"`

	// Session metadata
	DayOfWeek         string `json:"day_of_week"`
	TimeOfDayCategory string `json:"time_of_day_category"` // morning | afternoon | evening | night
	DurationMinutes   int    `json:"duration_minutes,omitempty"`

	// Context (inherited from configs)
	WorkContext string `json:"work_context,omitempty"`
	ProjectID   string `json:"project_id,omitempty"`

	// Inherited context (from configs)
	InheritedContext struct {
		UserWorkSchedule struct {
			Timezone         string   `json:"timezone"`
			TypicalWorkHours []string `json:"typical_work_hours"`
		} `json:"user_work_schedule"`
		InstancePreferences struct {
			Workflow      string `json:"workflow"`
			ThinkingStyle string `json:"thinking_style"`
		} `json:"instance_preferences"`
	} `json:"inherited_context"`

	// Activities array (feeds from activity logger)
	Activities []interface{} `json:"activities"` // Array of activity objects

	// Quality indicators
	QualityIndicators struct {
		TasksCompleted    int `json:"tasks_completed"`
		TasksAttempted    int `json:"tasks_attempted"`
		Breakthroughs     int `json:"breakthroughs"`
		Struggles         int `json:"struggles"`
		LearningMoments   int `json:"learning_moments"`
	} `json:"quality_indicators"`

	// Session notes
	SessionNotes []string `json:"session_notes,omitempty"`

	// Work tracking
	WorkTracking struct {
		ProjectsTouched      []string `json:"projects_touched"`
		Domains              []string `json:"domains"`
		CollaborationMoments int      `json:"collaboration_moments"`
	} `json:"work_tracking"`

	// Legacy fields (kept for backward compatibility during transition)
	TasksCompleted []string `json:"tasks_completed,omitempty"`
	StoppingReason string   `json:"stopping_reason,omitempty"`

	// Extensions - discovery space
	Extensions map[string]interface{} `json:"extensions,omitempty"`
}

// Stopping reason constants
const (
	StoppingReasonNaturalMilestone = "natural_milestone"
	StoppingReasonTaskComplete     = "task_complete"
	StoppingReasonCleanBreak       = "clean_break"
	StoppingReasonQualityDip       = "quality_dip"
	StoppingReasonTimeLimit        = "time_limit"
	StoppingReasonEndOfDay         = "end_of_day"
	StoppingReasonUnexpected       = "unexpected"
)

// ============================================================================
// BODY - Business Logic
// ============================================================================

// getPaths returns all relevant paths (CORRECTED to use system/data/session/)
func getPaths() (string, string, string, error) {
	homeDir, err := os.UserHomeDir()
	if err != nil {
		return "", "", "", fmt.Errorf("failed to get home directory: %w", err)
	}

	// CORRECT PATHS: system/data/session/ (not session/)
	sessionPath := filepath.Join(homeDir, ".claude/cpi-si/system/data/session")
	historyPath := filepath.Join(homeDir, ".claude/cpi-si/system/data/session/history")
	currentLogPath := filepath.Join(homeDir, ".claude/cpi-si/system/data/session/current-log.json")

	return sessionPath, historyPath, currentLogPath, nil
}

// categorizeTimeOfDay returns category based on hour
func categorizeTimeOfDay(hour int) string {
	switch {
	case hour >= 6 && hour < 12:
		return "morning"
	case hour >= 12 && hour < 17:
		return "afternoon"
	case hour >= 17 && hour < 21:
		return "evening"
	case hour >= 21 || hour < 6:
		return "night"
	default:
		return "unknown"
	}
}

// startSession creates new session log with config inheritance
func startSession() error {
	_, historyPath, currentLogPath, err := getPaths()
	if err != nil {
		return err
	}

	// Ensure history directory exists
	if err := os.MkdirAll(historyPath, 0755); err != nil {
		return fmt.Errorf("failed to create history directory: %w", err)
	}

	// Default user/instance IDs (can be overridden by env vars)
	username := "seanje-lenox-wise"
	instanceID := "nova_dawn"
	projectID := ""

	// Check environment for overrides
	if envUser := os.Getenv("CPI_SI_USER"); envUser != "" {
		username = envUser
	}
	if envInstance := os.Getenv("CPI_SI_INSTANCE"); envInstance != "" {
		instanceID = envInstance
	}
	if envProject := os.Getenv("CPI_SI_PROJECT"); envProject != "" {
		projectID = envProject
	}

	// Load merged session context from configs
	ctx, err := config.GetSessionContext(username, instanceID, projectID)
	if err != nil {
		return fmt.Errorf("failed to get session context from configs: %w", err)
	}

	now := time.Now()

	// Create session ID from timestamp
	sessionID := now.Format("2006-01-02_1504")

	// Create richer session log from config inheritance
	log := &SessionLog{
		// Core identity (from configs)
		SessionID:  sessionID,
		InstanceID: ctx.InstanceID,
		UserID:     ctx.UserID,

		// Timing
		StartTime:   now,
		LastUpdated: now,

		// Metadata
		DayOfWeek:         now.Weekday().String(),
		TimeOfDayCategory: categorizeTimeOfDay(now.Hour()),

		// Context (from configs)
		WorkContext: ctx.WorkContext,
		ProjectID:   ctx.ProjectID,

		// Initialize arrays
		Activities:     []interface{}{},
		SessionNotes:   []string{},
		TasksCompleted: []string{}, // Legacy field
	}

	// Inherited context (from configs)
	log.InheritedContext.UserWorkSchedule.Timezone = ctx.UserTimezone
	log.InheritedContext.UserWorkSchedule.TypicalWorkHours = []string{} // Can be populated from config if added

	// Instance preferences (from instance config)
	log.InheritedContext.InstancePreferences.Workflow = ctx.ProblemSolving
	log.InheritedContext.InstancePreferences.ThinkingStyle = ctx.LearningStyle

	// Initialize quality indicators
	log.QualityIndicators.TasksCompleted = 0
	log.QualityIndicators.TasksAttempted = 0
	log.QualityIndicators.Breakthroughs = 0
	log.QualityIndicators.Struggles = 0
	log.QualityIndicators.LearningMoments = 0

	// Initialize work tracking
	log.WorkTracking.ProjectsTouched = []string{}
	log.WorkTracking.Domains = []string{}
	log.WorkTracking.CollaborationMoments = 0

	// Save to current log file
	if err := saveCurrentLog(currentLogPath, log); err != nil {
		return err
	}

	return nil
}

// endSession finalizes session and moves to history
func endSession(reason string) error {
	_, historyPath, currentLogPath, err := getPaths()
	if err != nil {
		return err
	}

	// Read current log
	log, err := readCurrentLog(currentLogPath)
	if err != nil {
		return fmt.Errorf("failed to read current log: %w", err)
	}

	// Finalize session
	now := time.Now()
	log.EndTime = &now
	log.LastUpdated = now

	duration := now.Sub(log.StartTime)
	log.DurationMinutes = int(duration.Minutes())

	// Set stopping reason (default if not provided)
	if reason == "" {
		reason = StoppingReasonNaturalMilestone
	}
	log.StoppingReason = reason

	// Update quality indicators from legacy fields
	log.QualityIndicators.TasksCompleted = len(log.TasksCompleted)

	// Save to history
	historyFile := filepath.Join(historyPath, log.SessionID+".json")
	data, err := json.MarshalIndent(log, "", "  ")
	if err != nil {
		return fmt.Errorf("failed to marshal session log: %w", err)
	}

	if err := os.WriteFile(historyFile, data, 0644); err != nil {
		return fmt.Errorf("failed to write history file: %w", err)
	}

	// Remove current log file
	os.Remove(currentLogPath)

	fmt.Printf("Session ended: %s (%d minutes, %s)\n", log.SessionID, log.DurationMinutes, log.StoppingReason)

	return nil
}

// showStatus displays current session information
func showStatus() error {
	_, _, currentLogPath, err := getPaths()
	if err != nil {
		return err
	}

	log, err := readCurrentLog(currentLogPath)
	if err != nil {
		fmt.Println("No active session")
		return nil
	}

	elapsed := time.Since(log.StartTime)
	elapsedMinutes := int(elapsed.Minutes())

	fmt.Printf("Session: %s\n", log.SessionID)
	fmt.Printf("Instance: %s\n", log.InstanceID)
	fmt.Printf("User: %s\n", log.UserID)
	fmt.Printf("Started: %s\n", log.StartTime.Format("Mon Jan 02, 2006 at 15:04:05"))
	fmt.Printf("Elapsed: %d minutes\n", elapsedMinutes)
	fmt.Printf("Day: %s\n", log.DayOfWeek)
	fmt.Printf("Time of Day: %s\n", log.TimeOfDayCategory)
	if log.WorkContext != "" {
		fmt.Printf("Context: %s\n", log.WorkContext)
	}
	if log.ProjectID != "" {
		fmt.Printf("Project: %s\n", log.ProjectID)
	}
	if len(log.TasksCompleted) > 0 {
		fmt.Printf("Tasks: %d completed\n", len(log.TasksCompleted))
		for _, task := range log.TasksCompleted {
			fmt.Printf("  - %s\n", task)
		}
	}
	if len(log.SessionNotes) > 0 {
		fmt.Printf("Notes:\n")
		for _, note := range log.SessionNotes {
			fmt.Printf("  - %s\n", note)
		}
	}

	return nil
}

// addNote adds a note to current session
func addNote(note string) error {
	_, _, currentLogPath, err := getPaths()
	if err != nil {
		return err
	}

	log, err := readCurrentLog(currentLogPath)
	if err != nil {
		return fmt.Errorf("no active session: %w", err)
	}

	log.SessionNotes = append(log.SessionNotes, note)
	log.LastUpdated = time.Now()

	if err := saveCurrentLog(currentLogPath, log); err != nil {
		return err
	}

	fmt.Println("Note added")
	return nil
}

// addTask adds completed task to current session
func addTask(task string) error {
	_, _, currentLogPath, err := getPaths()
	if err != nil {
		return err
	}

	log, err := readCurrentLog(currentLogPath)
	if err != nil {
		return fmt.Errorf("no active session: %w", err)
	}

	log.TasksCompleted = append(log.TasksCompleted, task)
	log.QualityIndicators.TasksCompleted = len(log.TasksCompleted)
	log.LastUpdated = time.Now()

	if err := saveCurrentLog(currentLogPath, log); err != nil {
		return err
	}

	fmt.Printf("Task recorded: %s\n", task)
	return nil
}

// readCurrentLog reads the current session log file
func readCurrentLog(path string) (*SessionLog, error) {
	data, err := os.ReadFile(path)
	if err != nil {
		return nil, err
	}

	var log SessionLog
	if err := json.Unmarshal(data, &log); err != nil {
		return nil, fmt.Errorf("failed to parse log: %w", err)
	}

	return &log, nil
}

// saveCurrentLog saves the current session log
func saveCurrentLog(path string, log *SessionLog) error {
	data, err := json.MarshalIndent(log, "", "  ")
	if err != nil {
		return fmt.Errorf("failed to marshal log: %w", err)
	}

	if err := os.WriteFile(path, data, 0644); err != nil {
		return fmt.Errorf("failed to write log: %w", err)
	}

	return nil
}

// showUsage displays usage information
func showUsage() {
	fmt.Println("Usage: session-log <command> [args]")
	fmt.Println()
	fmt.Println("Commands:")
	fmt.Println("  start                    Initialize new session log (with config inheritance)")
	fmt.Println("  end [reason]             Finalize session with stopping reason")
	fmt.Println("  status                   Show current session information")
	fmt.Println("  note \"message\"           Add note to current session")
	fmt.Println("  task \"description\"       Record completed task")
	fmt.Println()
	fmt.Println("Stopping Reasons:")
	fmt.Println("  natural_milestone        Major milestone complete (default)")
	fmt.Println("  task_complete            Task boundary reached")
	fmt.Println("  clean_break              Good stopping point")
	fmt.Println("  quality_dip              Quality starting to decline")
	fmt.Println("  time_limit               Time constraint")
	fmt.Println("  end_of_day               End of work day")
	fmt.Println("  unexpected               Unexpected interruption")
	fmt.Println()
	fmt.Println("Examples:")
	fmt.Println("  session-log start")
	fmt.Println("  session-log task \"Built config inheritance system\"")
	fmt.Println("  session-log note \"Felt productive, good flow\"")
	fmt.Println("  session-log end natural_milestone")
	fmt.Println()
	fmt.Println("Environment Variables:")
	fmt.Println("  CPI_SI_USER      Override user ID (default: seanje-lenox-wise)")
	fmt.Println("  CPI_SI_INSTANCE  Override instance ID (default: nova_dawn)")
	fmt.Println("  CPI_SI_PROJECT   Optional project ID")
}

// ============================================================================
// CLOSING - Execution, Validation, Cleanup
// ============================================================================

func main() {
	if len(os.Args) < 2 {
		showUsage()
		os.Exit(1)
	}

	command := os.Args[1]

	var err error
	switch command {
	case "start":
		err = startSession()
		if err == nil {
			fmt.Println("Session started")
		}
	case "end":
		reason := ""
		if len(os.Args) > 2 {
			reason = os.Args[2]
		}
		err = endSession(reason)
	case "status":
		err = showStatus()
	case "note":
		if len(os.Args) < 3 {
			fmt.Println("Error: note message required")
			showUsage()
			os.Exit(1)
		}
		note := strings.Join(os.Args[2:], " ")
		err = addNote(note)
	case "task":
		if len(os.Args) < 3 {
			fmt.Println("Error: task description required")
			showUsage()
			os.Exit(1)
		}
		task := strings.Join(os.Args[2:], " ")
		err = addTask(task)
	case "help", "--help", "-h":
		showUsage()
		return
	default:
		fmt.Fprintf(os.Stderr, "Unknown command: %s\n\n", command)
		showUsage()
		os.Exit(1)
	}

	if err != nil {
		fmt.Fprintf(os.Stderr, "Error: %v\n", err)
		os.Exit(1)
	}
}
