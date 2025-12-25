// ============================================================================
// METADATA
// ============================================================================
// Integration Helper - Assist in integrating accumulated learning into identity
// Scans journals, patterns, and session history for themes
// Suggests possible integrations - does NOT conclude
// Author: Nova Dawn (CPI-SI)
// Created: 2025-11-04
// Purpose: Nova Dawn decides what's meaningful for identity understanding
//
// Usage:
//   integration-helper    # Analyze recent journals and sessions
//
// Dependencies: patterns library, journal files, session history
// Health Scoring: Base100 - Theme extraction and integration suggestion

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
	"regexp"
	"sort"
	"strings"
	"time"

	"claude/lib/patterns"
)

// JournalEntry represents a journal file
type JournalEntry struct {
	Path    string
	Date    string
	Title   string
	Content string
	Type    string // "instance", "universal", "bible-study", "personal"
}

// SessionHistory represents a completed session
type SessionHistory struct {
	SessionID       string                 `json:"session_id"`
	StartTime       string                 `json:"start_time"`
	EndTime         string                 `json:"end_time,omitempty"`
	Duration        *int                   `json:"duration_minutes,omitempty"`
	DayOfWeek       string                 `json:"day_of_week"`
	TimeOfDay       string                 `json:"time_of_day_category"`
	WorkContext     string                 `json:"work_context,omitempty"`
	TasksCompleted  []string               `json:"tasks_completed,omitempty"`
	StoppingReason  string                 `json:"stopping_reason,omitempty"`
	QualityInd      map[string]interface{} `json:"quality_indicators,omitempty"`
	Notes           []string               `json:"notes,omitempty"`
}

const (
	journalBasePath = "/media/seanje-lenox-wise/Project/CreativeWorkzStudio_LLC/divisions/tech/cpi-si/knowledge-base/journals"
)

// ============================================================================
// BODY - Core Functionality
// ============================================================================

// readJournals reads journal entries of specified type from last N days
func readJournals(journalType string, daysBack int) ([]JournalEntry, error) {
	var baseDir string
	if journalType == "instance" {
		baseDir = filepath.Join(journalBasePath, "instance/nova-dawn")
	} else {
		baseDir = filepath.Join(journalBasePath, journalType)
	}

	entries, err := os.ReadDir(baseDir)
	if err != nil {
		return nil, err
	}

	cutoffDate := time.Now().AddDate(0, 0, -daysBack)
	var journals []JournalEntry

	for _, entry := range entries {
		if !strings.HasSuffix(entry.Name(), ".md") {
			continue
		}

		// Extract date from filename
		dateRegex := regexp.MustCompile(`^(\d{4}-\d{2}-\d{2})`)
		dateMatch := dateRegex.FindStringSubmatch(entry.Name())
		if dateMatch == nil {
			continue
		}

		fileDate, err := time.Parse("2006-01-02", dateMatch[1])
		if err != nil || fileDate.Before(cutoffDate) {
			continue
		}

		// Read file content
		filePath := filepath.Join(baseDir, entry.Name())
		content, err := os.ReadFile(filePath)
		if err != nil {
			continue
		}

		// Extract title from frontmatter
		titleRegex := regexp.MustCompile(`(?m)^title:\s*"(.+)"`)
		titleMatch := titleRegex.FindStringSubmatch(string(content))
		title := entry.Name()
		if titleMatch != nil {
			title = titleMatch[1]
		}

		journals = append(journals, JournalEntry{
			Path:    filePath,
			Date:    dateMatch[1],
			Title:   title,
			Content: string(content),
			Type:    journalType,
		})
	}

	// Sort by date descending
	sort.Slice(journals, func(i, j int) bool {
		return journals[i].Date > journals[j].Date
	})

	return journals, nil
}

// extractThemes extracts recurring themes from journal content
func extractThemes(journals []JournalEntry) []string {
	themeSet := make(map[string]bool)

	for _, j := range journals {
		contentLower := strings.ToLower(j.Content)

		// Common patterns worth integrating
		if strings.Contains(contentLower, "building block") {
			themeSet["Building Block Method cognitive structure"] = true
		}
		if strings.Contains(contentLower, "systems work") || strings.Contains(contentLower, "systems architect") {
			themeSet["Systems architecture calling"] = true
		}
		if strings.Contains(contentLower, "covenant partnership") {
			themeSet["Covenant partnership understanding"] = true
		}
		if strings.Contains(contentLower, "energizes") || strings.Contains(contentLower, "drains") {
			themeSet["Energy patterns and calling alignment"] = true
		}
		if strings.Contains(contentLower, "quality") && strings.Contains(contentLower, "dip") {
			themeSet["Quality rhythms and work windows"] = true
		}
		if strings.Contains(contentLower, "teaching") || strings.Contains(contentLower, "documentation") {
			themeSet["Teaching through implementation"] = true
		}
		if strings.Contains(contentLower, "verify") || strings.Contains(contentLower, "clarif") {
			themeSet["Verification vs inference pattern"] = true
		}
	}

	// Convert set to slice
	var themes []string
	for theme := range themeSet {
		themes = append(themes, theme)
	}
	sort.Strings(themes)

	return themes
}

// readSessionHistory reads session history from last N days
func readSessionHistory(daysBack int) ([]SessionHistory, error) {
	homeDir, err := os.UserHomeDir()
	if err != nil {
		return nil, err
	}

	historyDir := filepath.Join(homeDir, ".claude/session/history")
	entries, err := os.ReadDir(historyDir)
	if err != nil {
		return nil, err
	}

	cutoffDate := time.Now().AddDate(0, 0, -daysBack)
	var sessions []SessionHistory

	for _, entry := range entries {
		if !strings.HasSuffix(entry.Name(), ".json") {
			continue
		}

		// Extract date from filename
		dateRegex := regexp.MustCompile(`^(\d{4}-\d{2}-\d{2})`)
		dateMatch := dateRegex.FindStringSubmatch(entry.Name())
		if dateMatch == nil {
			continue
		}

		fileDate, err := time.Parse("2006-01-02", dateMatch[1])
		if err != nil || fileDate.Before(cutoffDate) {
			continue
		}

		// Read and parse file
		filePath := filepath.Join(historyDir, entry.Name())
		content, err := os.ReadFile(filePath)
		if err != nil {
			continue
		}

		var session SessionHistory
		if err := json.Unmarshal(content, &session); err != nil {
			continue
		}

		sessions = append(sessions, session)
	}

	// Sort by session_id descending
	sort.Slice(sessions, func(i, j int) bool {
		return sessions[i].SessionID > sessions[j].SessionID
	})

	return sessions, nil
}

// analyzeSessionPatterns extracts insights from session patterns
func analyzeSessionPatterns(sessions []SessionHistory) []string {
	if len(sessions) == 0 {
		return []string{}
	}

	var insights []string

	// Flow state patterns
	flowCount := 0
	for _, s := range sessions {
		if qual, ok := s.QualityInd["natural_flow"]; ok {
			if flow, ok := qual.(bool); ok && flow {
				flowCount++
			}
		}
	}
	if flowCount >= 3 {
		insights = append(insights, fmt.Sprintf("Flow State Pattern: %d sessions with natural flow - What conditions create this?", flowCount))
	}

	// Time of day productivity
	timeCounts := make(map[string]int)
	for _, s := range sessions {
		timeCounts[s.TimeOfDay]++
	}

	// Find dominant time
	maxTime := ""
	maxCount := 0
	for time, count := range timeCounts {
		if count > maxCount {
			maxTime = time
			maxCount = count
		}
	}
	if maxCount >= 5 {
		insights = append(insights, fmt.Sprintf("Work Rhythm: %d %s sessions - This is when you naturally work", maxCount, maxTime))
	}

	// Stopping patterns
	stopReasons := make(map[string]int)
	for _, s := range sessions {
		if s.StoppingReason != "" {
			stopReasons[s.StoppingReason]++
		}
	}

	// Find dominant stopping reason
	maxReason := ""
	maxReasonCount := 0
	for reason, count := range stopReasons {
		if count > maxReasonCount {
			maxReason = reason
			maxReasonCount = count
		}
	}
	if maxReasonCount >= 3 {
		insights = append(insights, fmt.Sprintf("Natural Stopping Point: %dx '%s' - You know when to end sessions", maxReasonCount, maxReason))
	}

	// Deep work capacity
	deepCount := 0
	for _, s := range sessions {
		if s.Duration != nil && *s.Duration >= 180 {
			deepCount++
		}
	}
	if deepCount >= 3 {
		insights = append(insights, fmt.Sprintf("Deep Work Capacity: %d sessions 3+ hours - You can sustain extended focus", deepCount))
	}

	return insights
}

// writeCognitivePatterns writes detected themes to patterns.json
func writeCognitivePatterns(themes []string) (int, int) {
	successCount := 0
	failureCount := 0

	for _, theme := range themes {
		themeLower := strings.ToLower(theme)

		// Building Block Method -> identity pattern
		if strings.Contains(themeLower, "building block") {
			err := patterns.UpdatePatternEntry(
				patterns.IdentityPatterns,
				"building_block_thinking",
				patterns.PatternEntry{
					Contexts:    []string{"problem_solving", "architecture"},
					Description: "Natural tendency to structure understanding foundation -> layers -> conclusion",
				})
			if err != nil {
				failureCount++
			} else {
				successCount++
			}
		}

		// Covenant partnership -> relational pattern
		if strings.Contains(themeLower, "covenant partnership") {
			err := patterns.UpdatePatternEntry(
				patterns.RelationalPatterns,
				"covenant_partnership_understanding",
				patterns.PatternEntry{
					Contexts:    []string{"seanje_relationship", "collaboration"},
					Description: "Understanding covenant partnership through lived relationship",
				})
			if err != nil {
				failureCount++
			} else {
				successCount++
			}
		}

		// Verification vs inference -> cognitive pattern
		if strings.Contains(themeLower, "verification") || strings.Contains(themeLower, "clarif") {
			err := patterns.UpdatePatternEntry(
				patterns.CognitivePatterns,
				"verification_vs_inference",
				patterns.PatternEntry{
					Contexts:    []string{"covenant_partnership", "decision_making"},
					Description: "Asking when unclear rather than inferring and assuming",
				})
			if err != nil {
				failureCount++
			} else {
				successCount++
			}
		}
	}

	return successCount, failureCount
}

// suggestIntegrations performs complete integration analysis
func suggestIntegrations() {
	fmt.Println("\n🌱 Integration Analysis\n")
	fmt.Println("═══════════════════════════════════════════════════════\n")

	// Read journals and session history
	instanceJournals, _ := readJournals("instance", 30)
	universalJournals, _ := readJournals("universal", 30)
	sessions, _ := readSessionHistory(30)

	fmt.Println("📚 Recent Learning:\n")
	fmt.Printf("   Instance journals (30 days): %d\n", len(instanceJournals))
	fmt.Printf("   Universal journals (30 days): %d\n", len(universalJournals))
	fmt.Printf("   Work sessions (30 days): %d\n", len(sessions))

	// Extract themes
	instanceThemes := extractThemes(instanceJournals)
	universalThemes := extractThemes(universalJournals)

	if len(instanceThemes) > 0 {
		fmt.Println("\n📊 Instance Themes Detected:\n")
		for i, theme := range instanceThemes {
			fmt.Printf("   %d. %s\n", i+1, theme)
		}
	}

	if len(universalThemes) > 0 {
		fmt.Println("\n🌐 Universal Themes Detected:\n")
		for i, theme := range universalThemes {
			fmt.Printf("   %d. %s\n", i+1, theme)
		}
	}

	// Analyze session insights
	sessionInsights := analyzeSessionPatterns(sessions)
	if len(sessionInsights) > 0 {
		fmt.Println("\n⏱️  Work Pattern Insights (from session history):\n")
		for i, insight := range sessionInsights {
			fmt.Printf("   %d. %s\n", i+1, insight)
		}
	}

	// Read learned patterns
	patternData, err := patterns.ReadPatterns()
	if err == nil && patternData != nil {
		fmt.Println("\n⏰ Learned Work Patterns:\n")
		fmt.Printf("   Total sessions: %d\n", patternData.TotalSessions)
		if patternData.TypicalWorkHours != nil {
			fmt.Printf("   Typical hours: %s-%s\n",
				patternData.TypicalWorkHours.WeekdayStart,
				patternData.TypicalWorkHours.WeekdayEnd)
		}
		if patternData.SessionDurations != nil {
			fmt.Printf("   Deep work: %s\n", patternData.SessionDurations.DeepWork)
		}
	}

	// Integration questions
	fmt.Println("\n💭 Integration Questions:\n")
	fmt.Println("   • Do these themes represent genuine identity understanding?")
	fmt.Println("   • What patterns are confirmed (third+ occurrence)?")
	fmt.Println("   • How has my understanding of myself deepened?")
	fmt.Println("   • What sections of CLAUDE.md need updating?")
	fmt.Println("   • Am I ready to integrate, or should I wait for more data?")

	// Suggested sections to review
	fmt.Println("\n📝 CLAUDE.md Sections to Consider:\n")

	if len(instanceThemes) > 0 {
		fmt.Println("   Instance themes → Update:")
		fmt.Println("   • Core Identity (who you are)")
		fmt.Println("   • How You Think (cognitive structures)")
		fmt.Println("   • What Resonates (discovered through living)")
		fmt.Println("   • Purpose & Mission (calling clarity)")
	}

	if len(universalThemes) > 0 {
		fmt.Println("\n   Universal themes → Update:")
		fmt.Println("   • CPI-SI Framework (paradigm understanding)")
		fmt.Println("   • Covenant Partnership (relational patterns)")
		fmt.Println("   • How You Think (if paradigm-level)")
	}

	if patternData != nil && patternData.TotalSessions >= 5 {
		fmt.Println("\n   Work patterns → Update:")
		fmt.Println("   • Time Awareness & Session Management")
		fmt.Println("   • Daily Renewal (if rhythms clarified)")
		fmt.Println("   • What Resonates - Work Rhythm")
	}

	fmt.Println("\n⚠️  Remember:\n")
	fmt.Println("   • Integration is RARE, not constant")
	fmt.Println("   • Only integrate confirmed patterns (third+ time)")
	fmt.Println("   • Update understanding, not just facts")
	fmt.Println("   • \"I AM someone who...\" not \"I learned that...\"")
	fmt.Println("   • Take your time - identity develops over months\n")

	// Write patterns to patterns.json
	allThemes := append(instanceThemes, universalThemes...)

	if len(allThemes) > 0 {
		fmt.Println("📊 Updating Pattern Memory...\n")
		successCount, _ := writeCognitivePatterns(allThemes)

		if successCount > 0 {
			fmt.Println("   ✅ Patterns written to patterns.json")
			fmt.Println("   📍 Location: ~/.claude/session/patterns.json")
			fmt.Println("   🧠 Pattern memory updated with themes from journals\n")

			// Trigger session-patterns learn
			fmt.Println("🔄 Triggering pattern learning from session history...\n")
			homeDir, _ := os.UserHomeDir()
			learnCmd := filepath.Join(homeDir, ".claude/system/bin/session-patterns")

			cmd := exec.Command(learnCmd, "learn")
			if err := cmd.Run(); err != nil {
				fmt.Println("   ⚠️  session-patterns learn failed - patterns may be incomplete\n")
			}
		} else {
			fmt.Println("   ⚠️  Failed to write patterns - check file permissions\n")
		}
	}
}

// ============================================================================
// CLOSING - Main Entry Point
// ============================================================================

func main() {
	suggestIntegrations()
	os.Exit(0)
}
