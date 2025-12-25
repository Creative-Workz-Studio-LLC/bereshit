// ============================================================================
// METADATA
// ============================================================================
// Git to Session History Seeder - Generate session history from git commits
// Author: Nova Dawn (CPI-SI)
// Created: 2025-11-04
// Purpose: Seed session history data from git commit history for pattern learning
//
// Usage:
//   git-to-session-history [options]
//
// Options:
//   --days <N>        Number of days back to process (default: 30)
//   --repo <path>     Git repository path (default: current directory)
//   --dry-run         Show what would be created without writing files
//   --session-gap <M> Minutes between commits to split sessions (default: 180)
//
// Dependencies: git command, session history directory
// Health Scoring: Base100 - Git parsing and session generation

package main

// ============================================================================
// SETUP - Imports, Dependencies, Globals
// ============================================================================

import (
	"encoding/json"
	"flag"
	"fmt"
	"os"
	"os/exec"
	"path/filepath"
	"sort"
	"strings"
	"time"
)

// GitCommit represents a single git commit
type GitCommit struct {
	Hash      string
	Timestamp time.Time
	Message   string
	Author    string
}

// SessionHistory represents a work session generated from commits
type SessionHistory struct {
	SessionID      string                 `json:"session_id"`
	StartTime      string                 `json:"start_time"`
	EndTime        string                 `json:"end_time,omitempty"`
	Duration       int                    `json:"duration_minutes"` // NOT a pointer - always present
	DayOfWeek      string                 `json:"day_of_week"`
	TimeOfDay      string                 `json:"time_of_day_category"`
	WorkContext    string                 `json:"work_context"` // Path to repo, not description
	TasksCompleted []string               `json:"tasks_completed,omitempty"`
	StoppingReason string                 `json:"stopping_reason,omitempty"`
	QualityInd     map[string]interface{} `json:"quality_indicators,omitempty"`
	Notes          []string               `json:"notes,omitempty"`
}

// Command-line flags
var (
	daysBack   int
	repoPath   string
	dryRun     bool
	sessionGap int
)

// ============================================================================
// BODY - Core Functionality
// ============================================================================

// parseGitLog reads git commit history and returns commits
func parseGitLog(repoPath string, since time.Time) ([]GitCommit, error) {
	// Git log format: hash|timestamp|author|message
	cmd := exec.Command("git", "-C", repoPath, "log",
		fmt.Sprintf("--since=%s", since.Format("2006-01-02")),
		"--format=%H|%aI|%an|%s")

	output, err := cmd.CombinedOutput()
	if err != nil {
		return nil, fmt.Errorf("git log failed: %w\nOutput: %s", err, string(output))
	}

	lines := strings.Split(strings.TrimSpace(string(output)), "\n")
	var commits []GitCommit

	for _, line := range lines {
		if line == "" {
			continue
		}

		parts := strings.SplitN(line, "|", 4)
		if len(parts) != 4 {
			continue
		}

		timestamp, err := time.Parse(time.RFC3339, parts[1])
		if err != nil {
			continue
		}

		commits = append(commits, GitCommit{
			Hash:      parts[0],
			Timestamp: timestamp,
			Author:    parts[2],
			Message:   parts[3],
		})
	}

	// Sort by timestamp ascending (oldest first)
	sort.Slice(commits, func(i, j int) bool {
		return commits[i].Timestamp.Before(commits[j].Timestamp)
	})

	return commits, nil
}

// groupCommitsIntoSessions groups commits into logical work sessions
func groupCommitsIntoSessions(commits []GitCommit, gapMinutes int) [][]GitCommit {
	if len(commits) == 0 {
		return nil
	}

	var sessions [][]GitCommit
	currentSession := []GitCommit{commits[0]}

	for i := 1; i < len(commits); i++ {
		prev := commits[i-1]
		curr := commits[i]

		// Calculate time gap
		gap := curr.Timestamp.Sub(prev.Timestamp)

		// If gap is larger than threshold, start new session
		if gap.Minutes() > float64(gapMinutes) {
			sessions = append(sessions, currentSession)
			currentSession = []GitCommit{curr}
		} else {
			currentSession = append(currentSession, curr)
		}
	}

	// Add final session
	if len(currentSession) > 0 {
		sessions = append(sessions, currentSession)
	}

	return sessions
}

// getTimeOfDayCategory returns time category based on hour
func getTimeOfDayCategory(t time.Time) string {
	hour := t.Hour()
	if hour >= 5 && hour < 12 {
		return "morning"
	} else if hour >= 12 && hour < 17 {
		return "afternoon"
	} else if hour >= 17 && hour < 21 {
		return "evening"
	}
	return "night"
}

// inferWorkContext attempts to infer work context from commit messages
func inferWorkContext(commits []GitCommit) string {
	messages := make([]string, len(commits))
	for i, c := range commits {
		messages[i] = strings.ToLower(c.Message)
	}

	fullText := strings.Join(messages, " ")

	// Simple keyword matching
	if strings.Contains(fullText, "compiler") || strings.Contains(fullText, "parser") || strings.Contains(fullText, "lexer") {
		return "compiler_development"
	} else if strings.Contains(fullText, "doc") || strings.Contains(fullText, "readme") {
		return "documentation"
	} else if strings.Contains(fullText, "refactor") {
		return "refactoring"
	} else if strings.Contains(fullText, "fix") || strings.Contains(fullText, "bug") {
		return "bug_fixing"
	} else if strings.Contains(fullText, "test") {
		return "testing"
	}

	return "general_development"
}

// inferStoppingReason attempts to infer why session ended
func inferStoppingReason(commits []GitCommit, isLastSession bool) string {
	if len(commits) == 0 {
		return "unknown"
	}

	lastCommit := commits[len(commits)-1]
	lastMsg := strings.ToLower(lastCommit.Message)

	// Check for milestone/completion indicators
	if strings.Contains(lastMsg, "complete") || strings.Contains(lastMsg, "finished") {
		return "natural_milestone"
	}

	// Check for end-of-day indicators
	endHour := lastCommit.Timestamp.Hour()
	if endHour >= 17 && endHour <= 22 {
		return "end_of_day"
	}

	// Check for iteration/checkpoint
	if strings.Contains(lastMsg, "iteration") || strings.Contains(lastMsg, "checkpoint") {
		return "iteration_complete"
	}

	// Default reasons
	if isLastSession {
		return "natural_milestone"
	}

	return "natural_pause"
}

// generateSessionHistory creates SessionHistory from commit group
func generateSessionHistory(commits []GitCommit, isLastSession bool, repoPath string) SessionHistory {
	if len(commits) == 0 {
		return SessionHistory{}
	}

	startTime := commits[0].Timestamp
	endTime := commits[len(commits)-1].Timestamp

	// Calculate duration in minutes
	durationMins := int(endTime.Sub(startTime).Minutes())
	if durationMins < 1 {
		durationMins = 1 // Minimum 1 minute
	}

	// Generate session ID from start timestamp
	sessionID := startTime.Format("2006-01-02_1504")

	// Extract task descriptions from commit messages
	tasks := make([]string, 0, len(commits))
	for _, c := range commits {
		// Clean up commit message for task description
		task := strings.TrimSpace(c.Message)
		if task != "" && len(task) < 200 {
			tasks = append(tasks, task)
		}
	}

	// Build quality indicators
	qualityInd := map[string]interface{}{
		"tasks_completed": len(tasks),
	}

	// Build notes from session analysis
	notes := []string{
		fmt.Sprintf("Session reconstructed from %d git commits spanning %d minutes", len(commits), durationMins),
	}

	// Add context note about session pattern
	if durationMins < 30 {
		notes = append(notes, "Brief session - likely quick fix or single focused task")
	} else if durationMins < 120 {
		notes = append(notes, "Standard session length - focused work on specific area")
	} else if durationMins < 240 {
		notes = append(notes, "Extended session - deep work on complex task")
	} else {
		notes = append(notes, "Marathon session - major milestone or multi-phase work")
	}

	session := SessionHistory{
		SessionID:      sessionID,
		StartTime:      startTime.Format(time.RFC3339),
		EndTime:        endTime.Format(time.RFC3339),
		Duration:       durationMins,
		DayOfWeek:      startTime.Format("Monday"),
		TimeOfDay:      getTimeOfDayCategory(startTime),
		WorkContext:    repoPath,
		TasksCompleted: tasks,
		StoppingReason: inferStoppingReason(commits, isLastSession),
		QualityInd:     qualityInd,
		Notes:          notes,
	}

	return session
}

// writeSessionHistory writes session to JSON file
func writeSessionHistory(session SessionHistory, historyDir string) error {
	// Ensure directory exists
	if err := os.MkdirAll(historyDir, 0755); err != nil {
		return fmt.Errorf("failed to create history directory: %w", err)
	}

	// Generate filename from session ID
	filename := session.SessionID + ".json"
	filepath := filepath.Join(historyDir, filename)

	// Check if file exists
	if _, err := os.Stat(filepath); err == nil {
		return fmt.Errorf("session file already exists: %s", filepath)
	}

	// Marshal to JSON
	data, err := json.MarshalIndent(session, "", "  ")
	if err != nil {
		return fmt.Errorf("failed to marshal session: %w", err)
	}

	// Write file
	if err := os.WriteFile(filepath, data, 0644); err != nil {
		return fmt.Errorf("failed to write session file: %w", err)
	}

	return nil
}

// printSessionSummary displays summary of generated session
func printSessionSummary(session SessionHistory) {
	fmt.Printf("\n📅 Session: %s\n", session.SessionID)
	fmt.Printf("   Start:    %s\n", session.StartTime)
	fmt.Printf("   End:      %s\n", session.EndTime)
	fmt.Printf("   Duration: %d minutes\n", session.Duration)
	fmt.Printf("   Day:      %s %s\n", session.DayOfWeek, session.TimeOfDay)
	fmt.Printf("   Context:  %s\n", session.WorkContext)
	fmt.Printf("   Tasks:    %d\n", len(session.TasksCompleted))
	fmt.Printf("   Reason:   %s\n", session.StoppingReason)
}

// ============================================================================
// CLOSING - Main Entry Point
// ============================================================================

func main() {
	// Parse flags
	flag.IntVar(&daysBack, "days", 30, "Number of days back to process")
	flag.StringVar(&repoPath, "repo", ".", "Git repository path")
	flag.BoolVar(&dryRun, "dry-run", false, "Show what would be created without writing files")
	flag.IntVar(&sessionGap, "session-gap", 180, "Minutes between commits to split sessions")
	flag.Parse()

	// Resolve repository path
	absRepoPath, err := filepath.Abs(repoPath)
	if err != nil {
		fmt.Fprintf(os.Stderr, "❌ Failed to resolve repo path: %v\n", err)
		os.Exit(1)
	}

	// Verify it's a git repository
	cmd := exec.Command("git", "-C", absRepoPath, "rev-parse", "--git-dir")
	if err := cmd.Run(); err != nil {
		fmt.Fprintf(os.Stderr, "❌ Not a git repository: %s\n", absRepoPath)
		os.Exit(1)
	}

	// Calculate since date
	since := time.Now().AddDate(0, 0, -daysBack)

	fmt.Printf("\n🔍 Git to Session History Seeder\n")
	fmt.Printf("═══════════════════════════════════════════════════════\n\n")
	fmt.Printf("Repository:   %s\n", absRepoPath)
	fmt.Printf("Since:        %s (%d days ago)\n", since.Format("2006-01-02"), daysBack)
	fmt.Printf("Session gap:  %d minutes\n", sessionGap)
	if dryRun {
		fmt.Printf("Mode:         DRY RUN (no files will be written)\n")
	}

	// Parse git log
	fmt.Println("\n📖 Reading git commit history...")
	commits, err := parseGitLog(absRepoPath, since)
	if err != nil {
		fmt.Fprintf(os.Stderr, "❌ Failed to parse git log: %v\n", err)
		os.Exit(1)
	}

	if len(commits) == 0 {
		fmt.Println("⚠️  No commits found in specified time range")
		os.Exit(0)
	}

	fmt.Printf("   Found %d commits\n", len(commits))

	// Group into sessions
	fmt.Println("\n🔗 Grouping commits into sessions...")
	sessions := groupCommitsIntoSessions(commits, sessionGap)
	fmt.Printf("   Generated %d sessions\n", len(sessions))

	// Get history directory
	homeDir, err := os.UserHomeDir()
	if err != nil {
		fmt.Fprintf(os.Stderr, "❌ Failed to get home directory: %v\n", err)
		os.Exit(1)
	}
	historyDir := filepath.Join(homeDir, ".claude", "session", "history")

	// Generate and write sessions
	fmt.Println("\n💾 Creating session history files...")

	successCount := 0
	errorCount := 0

	for i, commitGroup := range sessions {
		isLast := (i == len(sessions)-1)
		session := generateSessionHistory(commitGroup, isLast, absRepoPath)

		if dryRun {
			printSessionSummary(session)
		} else {
			if err := writeSessionHistory(session, historyDir); err != nil {
				fmt.Printf("   ❌ Failed to write %s: %v\n", session.SessionID, err)
				errorCount++
			} else {
				fmt.Printf("   ✅ Created %s (%d commits, %dm)\n",
					session.SessionID, len(commitGroup), session.Duration)
				successCount++
			}
		}
	}

	// Summary
	fmt.Println("\n" + strings.Repeat("═", 55))
	fmt.Printf("\n📊 Summary:\n\n")
	fmt.Printf("   Commits processed:     %d\n", len(commits))
	fmt.Printf("   Sessions generated:    %d\n", len(sessions))

	if dryRun {
		fmt.Printf("   Mode:                  DRY RUN (no files written)\n")
	} else {
		fmt.Printf("   Successfully created:  %d\n", successCount)
		if errorCount > 0 {
			fmt.Printf("   Errors:                %d\n", errorCount)
		}
		fmt.Printf("\n📍 Location: %s\n", historyDir)
	}

	fmt.Println()

	if errorCount > 0 {
		os.Exit(1)
	}

	os.Exit(0)
}
