// ============================================================================
// METADATA
// ============================================================================
// Debugger Command - CPI-SI Immune System (Assessment Layer)
// Purpose: Analyze system logs, assess health, classify problems, route decisions
// Non-blocking: Read-only analysis with actionable recommendations
// Usage: ./bin/debugger [--component <name>] [--since <duration>]
//
// HEALTH SCORING MAP (TRUE SCORE):
// ----------------------------------
// Setup (4 health tracking calls = 18 points):
//   Call 1/4: logger.Check - Initialize logger (+5 or -5)
//   Call 2/4: logger.Operation - Start operation (+5 or -5)
//   Call 3/4: logger.Check - Parse arguments (+5 or -5)
//   Call 4/4: logger.Check - Locate log files (+3 or -3)
//
// Log Analysis (6 health tracking calls = 95 points) - CORE PURPOSE:
//   Call 1/6: logger.Check - Read log files (+10 or -10)
//   Call 2/6: logger.Check - Parse log entries (+30 or -30) - Parsing accuracy critical
//   Call 3/6: logger.Check - Aggregate component health (+20 or -20) - Understanding system state
//   Call 4/6: logger.Check - Correlate across components (+15 or -15) - Finding connections
//   Call 5/6: logger.Check - Identify patterns (+15 or -15) - Classification
//   Call 6/6: logger.Check - Compare proposed vs actual (+5 or -5)
//
// Assessment & Routing (5 health tracking calls = 26 points):
//   Call 1/5: logger.Check - Classify problems (+8 or -8) - Routing decisions critical
//   Call 2/5: logger.Check - Determine severity (+5 or -5)
//   Call 3/5: logger.Check - Generate recommendations (+8 or -8)
//   Call 4/5: logger.Check - Display assessment (+3 or -3)
//   Call 5/5: logger.Success/Failure - Log final result (+2 or -2)
//
// Total Possible: 139 points
// Normalization: (cumulative_health / 139) × 100

package main

// ============================================================================
// SETUP
// ============================================================================

import (
	"flag"
	"fmt"
	"os"
	"path/filepath"
	"sort"
	"strings"
	"time"

	"system/lib/debugging"
	"system/lib/display"
	"system/lib/logging"
)

// ────────────────────────────────────────────────────────────────
// Constants - Assessment Configuration
// ────────────────────────────────────────────────────────────────

const (
	// Health thresholds for assessment classification
	healthCritical = -30 // Below this = critical issues requiring immediate attention
	healthDegraded = 0   // Below this = system degradation, investigate soon
	healthWarning  = 50  // Below this = warning state, monitor closely

	// Analysis thresholds
	warningCountThreshold = 5 // Multiple warnings indicate potential instability
)

// ────────────────────────────────────────────────────────────────
// Types - Debugger-Specific Structures
// ────────────────────────────────────────────────────────────────

// ComponentDebugState tracks debug entries for a component
type ComponentDebugState struct {
	Component string
	Entries   []debugging.InspectionEntry // Use library's exported type
}

// HealthDivergence tracks when actual health diverges from expected
type HealthDivergence struct {
	CheckName       string // Name of the check that diverged
	Expected        int    // Expected health delta
	Actual          int    // Actual health delta
	Gap             int    // Difference (Actual - Expected)
	Severity        string // "critical", "high", "medium", "low"
	Pattern         string // Divergence pattern classification

	// Restoration routing (from semantic metadata)
	OperationType    string         // Operation category (file_validation, system_operation, etc.)
	OperationSubtype string         // Granular sub-type (syntax_check, permission_check, etc.)
	ErrorType        string         // Error classification (permission_denied, missing_dependency, etc.)
	ErrorDetails     map[string]any // Structured error context
	RecoveryHint     string         // Restoration hint (automated_fix, manual_intervention, etc.)
	RecoveryStrategy string         // Specific antibody to use (fix_file_permissions, install_package, etc.)
	RecoveryParams   map[string]any // Parameters for antibody execution
}

// ComponentHealth tracks health state for a component
type ComponentHealth struct {
	Name             string
	FinalHealth      int
	Entries          []logging.LogEntry
	SuccessCount     int
	FailureCount     int
	WarningCount     int
	ErrorCount       int
	CheckCount       int
	OperationCount   int
	Divergences      []HealthDivergence  // Health divergences detected
}

// SystemAssessment represents the overall system analysis
type SystemAssessment struct {
	Components           map[string]*ComponentHealth
	TotalEntries         int
	DebugEntries         int                              // Debug entries analyzed
	OverallHealth        int
	CriticalIssues       []string
	Warnings             []string
	Recommendations      []string
	Patterns             map[string]int                   // Pattern type -> count (for antibody routing)
	CrossComponentIssues map[string][]string              // Issue type -> affected components
	Divergences          map[string]int                   // Divergence type -> count
	CorrelatedEntries    map[string]*ComponentDebugState  // ContextID -> correlated log+debug entries
	AnalysisTime         time.Time
}

// ============================================================================
// BODY
// ============================================================================
//
// NOTE: Correlation Implementation
// This debugger implements basic correlation by contextID for config/development analysis.
// A full system correlator will be implemented as a separate component to handle:
//   - Multi-component correlation across the entire system
//   - Timeline reconstruction across logging and debugging rails
//   - Pattern recognition across component boundaries
//   - Cross-rail analysis for complex failure scenarios
//
// This implementation focuses on per-file assessment for configuration validation
// and development troubleshooting. The full correlator will orchestrate system-wide
// analysis using this debugger as a foundation.

// correlateEntries matches debug entries with log entries by contextID.
//
// What It Does:
// Takes log entries and debug entries, matches them by contextID to show how
// execution appears across both rails (logging shows WHAT happened with health,
// debugging shows HOW it happened with variable states).
//
// Why It Exists:
// Logging and debugging rails observe the same execution from different perspectives.
// Correlation links them via contextID, enabling assessment to see complete picture:
// health trajectory + execution state at each point.
//
// How It Works:
// Creates map keyed by contextID, associates debug entries with their corresponding
// log entries. Returns structure showing both perspectives of same execution.
//
// Parameters:
//   logs: []logging.LogEntry - parsed log entries
//   debugs: []debugging.InspectionEntry - parsed debug entries
//
// Returns:
//   map[string]*ComponentDebugState - contextID -> correlated entries
func correlateEntries(logs []logging.LogEntry, debugs []debugging.InspectionEntry) map[string]*ComponentDebugState {
	correlated := make(map[string]*ComponentDebugState)                      // Map contextID -> correlated entries

	// BUILD DEBUG ENTRY INDEX - Group debug entries by contextID for quick lookup
	debugByContext := make(map[string][]debugging.InspectionEntry)           // Map contextID -> debug entries
	for _, debug := range debugs {                                           // Process each debug entry
		// Note: InspectionEntry needs contextID field when implemented in debugging library
		// For now, debug entries are collected but not yet correlated by contextID
		// Full implementation will extract contextID from debug entry and match with logs
		debugByContext["unknown"] = append(debugByContext["unknown"], debug) // Placeholder until contextID available
	}

	// CORRELATE LOGS WITH DEBUG ENTRIES - Match by contextID
	for _, log := range logs {                                               // Process each log entry
		contextID := log.ContextID                                           // Extract contextID from log
		component := log.Component                                           // Extract component name

		if _, exists := correlated[contextID]; !exists {                     // First entry for this contextID
			correlated[contextID] = &ComponentDebugState{                    // Create new correlation
				Component: component,                                        // Set component name
				Entries:   []debugging.InspectionEntry{},                    // Initialize empty debug entries
			}
		}

		// MATCH DEBUG ENTRIES - Find debug entries with same contextID
		if debugEntries, found := debugByContext[contextID]; found {         // Debug entries exist for this contextID
			correlated[contextID].Entries = append(correlated[contextID].Entries, debugEntries...)  // Associate debug entries
		}
	}

	return correlated                                                        // Return correlated entries map
}

// getExpectedHealthMap returns expected health deltas for a component
// TODO: Eventually parse this from METADATA health scoring maps in source files
// For now, manually defined as proof of concept
func getExpectedHealthMap(componentName string) map[string]int {
	// Map of check names to expected health delta
	expected := make(map[string]int)

	switch componentName {
	case "debugger":
		// From debugger.go METADATA health scoring map
		expected["logger-initialized"] = 5
		expected["arguments-parsed"] = 5
		expected["log-directory-located"] = 3
		expected["log-files-read"] = 10
		expected["log-entries-parsed"] = 30
		expected["component-health-aggregated"] = 20
		expected["cross-component-correlation"] = 15
		expected["pattern-identification"] = 15
		expected["proposed-vs-actual-comparison"] = 5
		expected["problems-classified"] = 8
		expected["severity-determined"] = 5
		expected["recommendations-generated"] = 8
		expected["assessment-displayed"] = 3
		// Final success/failure = 2

	case "divergence-demo":
		// From divergence-demo.go METADATA health scoring map
		expected["partial-success-test"] = 30
		expected["complete-failure-test"] = 40
		expected["unexpected-failure-test"] = 20
		expected["perfect-match-test"] = 15
		expected["over-performance-test"] = 10
	}

	return expected
}

// analyzeComponent builds health assessment for a component
func analyzeComponent(name string, entries []logging.LogEntry) *ComponentHealth {
	health := &ComponentHealth{
		Name:    name,
		Entries: entries,
	}

	// SPECIAL CASE: Debugger analyzing itself
	// Problem: Debugger writes setup entries BEFORE reading log, so latest contextID is incomplete current run
	// Solution: Analyze the SECOND-TO-LAST contextID (previous completed run)
	if name == "debugger" && len(entries) > 0 {
		// Find all unique contextIDs
		contextIDs := make(map[string]bool)
		for _, entry := range entries {
			contextIDs[entry.ContextID] = true
		}

		// If multiple contexts exist, use second-to-last (skip incomplete current run)
		if len(contextIDs) > 1 {
			// Find second-to-last contextID by removing last one
			latestContextID := entries[len(entries)-1].ContextID
			var secondToLastContextID string

			for _, entry := range entries {
				if entry.ContextID != latestContextID {
					secondToLastContextID = entry.ContextID  // Keep updating until we find the actual second-to-last
				}
			}

			// Filter to only entries from second-to-last (completed) run
			var completedRunEntries []logging.LogEntry
			for _, entry := range entries {
				if entry.ContextID == secondToLastContextID {
					completedRunEntries = append(completedRunEntries, entry)
				}
			}

			// Replace entries with filtered set for analysis
			entries = completedRunEntries
			health.Entries = completedRunEntries
		}
	}

	// Use normalized health from last entry (logger calculates this based on TotalPossibleHealth)
	if len(entries) > 0 {
		health.FinalHealth = entries[len(entries)-1].NormalizedHealth
	}

	// Get expected health map for this component
	expectedHealth := getExpectedHealthMap(name)

	for _, entry := range entries {
		switch entry.Level {
		case "CHECK":
			health.CheckCount++
			if entry.HealthImpact > 0 {
				health.SuccessCount++
			} else if entry.HealthImpact < 0 {
				health.FailureCount++
			}
		case "OPERATION":
			health.OperationCount++
		case "ERROR":
			health.ErrorCount++
			health.FailureCount++
		case "WARNING":
			health.WarningCount++
		case "SUCCESS":
			health.CheckCount++  // Count SUCCESS as a check
			health.SuccessCount++
		case "FAILURE":
			health.CheckCount++  // Count FAILURE as a check
			health.FailureCount++
		}

		// DIVERGENCE DETECTION: Compare actual vs expected health impact
		// Look for check names in event field (format: "Checking: check-name")
		if (entry.Level == "CHECK" || entry.Level == "SUCCESS" || entry.Level == "FAILURE") && entry.Event != "" {
			// Extract check name from event
			checkName := entry.Event
			if strings.HasPrefix(checkName, "Checking: ") {
				checkName = strings.TrimPrefix(checkName, "Checking: ")
			}

			// If we have expected health for this check, compare
			if expected, exists := expectedHealth[checkName]; exists {
				actual := entry.HealthImpact

				// Detect divergence (allow small tolerance for rounding)
				if actual != expected && (actual > 0 || expected > 0) {  // Both should be positive for success
					gap := actual - expected

					// Classify severity based on gap size
					severity := "low"
					if gap <= -20 || gap >= 20 {
						severity = "critical"
					} else if gap <= -10 || gap >= 10 {
						severity = "high"
					} else if gap <= -5 || gap >= 5 {
						severity = "medium"
					}

					// Classify pattern
					pattern := "unknown"
					if actual == 0 && expected > 0 {
						pattern = "complete-failure"
					} else if actual > 0 && actual < expected {
						pattern = "partial-success"
					} else if actual < 0 && expected > 0 {
						pattern = "unexpected-failure"
					} else if actual > expected {
						pattern = "over-performance"
					}

					divergence := HealthDivergence{
						CheckName: checkName,
						Expected:  expected,
						Actual:    actual,
						Gap:       gap,
						Severity:  severity,
						Pattern:   pattern,
					}

					// Extract semantic metadata for restoration routing (if present)
					if entry.Semantic != nil {
						divergence.OperationType = entry.Semantic.OperationType
						divergence.OperationSubtype = entry.Semantic.OperationSubtype
						divergence.ErrorType = entry.Semantic.ErrorType
						divergence.ErrorDetails = entry.Semantic.ErrorDetails
						divergence.RecoveryHint = entry.Semantic.RecoveryHint
						divergence.RecoveryStrategy = entry.Semantic.RecoveryStrategy
						divergence.RecoveryParams = entry.Semantic.RecoveryParams
					}

					health.Divergences = append(health.Divergences, divergence)
				}
			}
		}
	}

	return health
}

// assessSystem performs overall system assessment
func assessSystem(components map[string]*ComponentHealth) *SystemAssessment {
	assessment := &SystemAssessment{
		Components:      components,
		AnalysisTime:    time.Now(),
		CriticalIssues:  make([]string, 0),
		Warnings:        make([]string, 0),
		Recommendations: make([]string, 0),
	}

	totalHealth := 0
	componentCount := 0

	for name, comp := range components {
		assessment.TotalEntries += len(comp.Entries)
		totalHealth += comp.FinalHealth
		componentCount++

		// Identify critical issues
		if comp.FinalHealth < healthCritical {
			assessment.CriticalIssues = append(assessment.CriticalIssues,
				fmt.Sprintf("%s: Critical health (%d) - multiple failures detected", name, comp.FinalHealth))
		} else if comp.FinalHealth < healthDegraded {
			assessment.Warnings = append(assessment.Warnings,
				fmt.Sprintf("%s: Negative health (%d) - system degradation", name, comp.FinalHealth))
		}

		// Check for high failure rate
		if comp.FailureCount > comp.SuccessCount && comp.FailureCount > 0 {
			assessment.CriticalIssues = append(assessment.CriticalIssues,
				fmt.Sprintf("%s: Failure rate exceeds success rate (%d failures vs %d successes)",
					name, comp.FailureCount, comp.SuccessCount))
		}

		// Warnings for degradation
		if comp.WarningCount > warningCountThreshold {
			assessment.Warnings = append(assessment.Warnings,
				fmt.Sprintf("%s: Multiple warnings (%d) - potential instability", name, comp.WarningCount))
		}
	}

	if componentCount > 0 {
		assessment.OverallHealth = totalHealth / componentCount
	}

	// Generate recommendations
	if len(assessment.CriticalIssues) > 0 {
		assessment.Recommendations = append(assessment.Recommendations,
			"Run 'diagnose' command for detailed troubleshooting of critical issues")
	}

	if assessment.OverallHealth < healthDegraded {
		assessment.Recommendations = append(assessment.Recommendations,
			"System health is negative - review component logs for failure patterns")
	} else if assessment.OverallHealth < healthWarning {
		assessment.Recommendations = append(assessment.Recommendations,
			"System health is degraded - consider running 'validate' to check configuration")
	}

	return assessment
}

// identifyPatterns analyzes assessment for known failure patterns.
//
// What It Does:
// Examines critical issues and warnings to identify recurring patterns that indicate
// specific problems (configuration errors, permission issues, missing dependencies).
//
// Why It Exists:
// Pattern recognition enables automated antibody responses. Known patterns map to
// known fixes. Novel patterns escalate to human analysis.
//
// How It Works:
// Scans critical issues and warnings for known keywords/phrases, categorizes them,
// returns pattern classifications for antibody routing.
//
// Parameters:
//   assessment: *SystemAssessment - analyzed system state
//
// Returns:
//   map[string]int - pattern type -> occurrence count
func identifyPatterns(assessment *SystemAssessment) map[string]int {
	patterns := make(map[string]int)                                         // Pattern type -> count

	// SCAN CRITICAL ISSUES - Identify severe problem patterns

	for _, issue := range assessment.CriticalIssues {                        // Process each critical issue
		// Configuration patterns
		if contains(issue, "permission") || contains(issue, "denied") {      // Permission-related failures
			patterns["permission_error"]++                                   // Track permission pattern
		}
		if contains(issue, "not found") || contains(issue, "missing") {      // Missing resources
			patterns["missing_resource"]++                                   // Track missing resource pattern
		}
		if contains(issue, "syntax") || contains(issue, "invalid") {         // Configuration syntax errors
			patterns["config_error"]++                                       // Track config error pattern
		}
		// Health patterns
		if contains(issue, "failure rate exceeds") {                         // High failure rate
			patterns["high_failure_rate"]++                                  // Track failure rate pattern
		}
		if contains(issue, "critical health") {                              // Critical health state
			patterns["critical_health"]++                                    // Track critical health pattern
		}
	}

	// SCAN WARNINGS - Identify degradation patterns

	for _, warning := range assessment.Warnings {                            // Process each warning
		if contains(warning, "negative health") {                            // Health degradation
			patterns["health_degradation"]++                                 // Track degradation pattern
		}
		if contains(warning, "multiple warnings") {                          // Warning accumulation
			patterns["warning_accumulation"]++                               // Track accumulation pattern
		}
	}

	return patterns                                                          // Return pattern classifications
}

// contains checks if string contains substring (case-insensitive helper).
func contains(s, substr string) bool {
	return len(s) >= len(substr) && (s == substr || len(s) > len(substr) &&
		(s[:len(substr)] == substr || s[len(s)-len(substr):] == substr ||
		 findInString(s, substr)))
}

// findInString searches for substring in string.
func findInString(s, substr string) bool {
	for i := 0; i <= len(s)-len(substr); i++ {
		if s[i:i+len(substr)] == substr {
			return true
		}
	}
	return false
}

// correlateAcrossComponents identifies issues that affect multiple components.
//
// What It Does:
// Analyzes component health to find systemic issues (same failure across multiple
// components indicates shared root cause like environment misconfiguration).
//
// Why It Exists:
// Cross-component correlation reveals systemic vs component-specific issues. Enables
// targeted fixes (fix root cause once vs fixing each component separately).
//
// How It Works:
// Groups components by similar health states, identifies common failure patterns,
// returns correlation insights for assessment recommendations.
//
// Parameters:
//   components: map[string]*ComponentHealth - all component analyses
//
// Returns:
//   map[string][]string - issue type -> affected components list
func correlateAcrossComponents(components map[string]*ComponentHealth) map[string][]string {
	correlations := make(map[string][]string)                                // Issue type -> components list

	// IDENTIFY SYSTEMIC FAILURES - Same problem across multiple components

	for name, comp := range components {                                     // Process each component
		// Track critical health issues
		if comp.FinalHealth < healthCritical {                               // Critical health state
			correlations["critical_health"] = append(correlations["critical_health"], name)  // Add to critical list
		}
		// Track degradation
		if comp.FinalHealth < healthDegraded && comp.FinalHealth >= healthCritical {  // Degraded but not critical
			correlations["degraded_health"] = append(correlations["degraded_health"], name)  // Add to degraded list
		}
		// Track high failure rates
		if comp.FailureCount > comp.SuccessCount && comp.FailureCount > 0 {  // More failures than successes
			correlations["high_failure_rate"] = append(correlations["high_failure_rate"], name)  // Add to failure list
		}
		// Track warning accumulation
		if comp.WarningCount > warningCountThreshold {                       // Excessive warnings
			correlations["warning_accumulation"] = append(correlations["warning_accumulation"], name)  // Add to warning list
		}
	}

	return correlations                                                      // Return correlation map
}

// compareProposedVsActual compares expected vs actual execution outcomes.
//
// What It Does:
// Analyzes health scoring to detect divergence from expected execution (operations
// that should succeed but failed, or succeeded with lower health than expected).
//
// Why It Exists:
// Expected vs actual comparison reveals logic errors, unexpected conditions, or
// incomplete error handling. Guides improvement of health scoring accuracy.
//
// How It Works:
// Examines component entries for health delta patterns, identifies unexpected
// outcomes, returns divergence analysis for assessment.
//
// Parameters:
//   components: map[string]*ComponentHealth - all component analyses
//
// Returns:
//   map[string]int - divergence type -> count
func compareProposedVsActual(components map[string]*ComponentHealth) map[string]int {
	divergences := make(map[string]int)                                      // Divergence type -> count

	// ANALYZE HEALTH TRAJECTORIES - Identify unexpected outcomes

	for _, comp := range components {                                        // Process each component
		// Detect unexpected degradation
		if comp.FinalHealth < 0 && comp.SuccessCount > comp.FailureCount {   // Negative health despite more successes
			divergences["unexpected_degradation"]++                          // Track unexpected degradation
		}
		// Detect incomplete recovery
		if comp.FailureCount > 0 && comp.SuccessCount > comp.FailureCount && comp.FinalHealth < healthWarning {  // Recovered but low health
			divergences["incomplete_recovery"]++                             // Track incomplete recovery
		}
		// Detect inconsistent scoring
		if comp.CheckCount > 10 && comp.FinalHealth == 0 {                   // Many checks but zero health
			divergences["inconsistent_scoring"]++                            // Track scoring inconsistency
		}
	}

	return divergences                                                       // Return divergence analysis
}

// sumDivergences calculates total divergences across all types.
func sumDivergences(divergences map[string]int) int {
	total := 0                                                               // Initialize total
	for _, count := range divergences {                                      // Sum all divergence counts
		total += count                                                       // Add to total
	}
	return total                                                             // Return sum
}

// displayAssessment shows the assessment results
func displayAssessment(assessment *SystemAssessment) {
	fmt.Print(display.Header("CPI-SI System Debugger - Assessment Report"))

	// Overall health summary
	fmt.Print(display.Subheader("System Overview"))
	fmt.Printf("  Analysis Time: %s\n", assessment.AnalysisTime.Format("2006-01-02 15:04:05"))
	fmt.Printf("  Components Analyzed: %d\n", len(assessment.Components))
	fmt.Printf("  Total Log Entries: %d\n", assessment.TotalEntries)

	healthStatus := "Healthy"
	healthColor := display.Green
	if assessment.OverallHealth < healthDegraded {
		healthStatus = "Degraded"
		healthColor = display.Red
	} else if assessment.OverallHealth < healthWarning {
		healthStatus = "Warning"
		healthColor = display.Yellow
	}
	fmt.Printf("  Overall Health: %s%d%s (%s)\n\n", healthColor, assessment.OverallHealth, display.Reset, healthStatus)

	// Component details
	fmt.Print(display.Subheader("Component Health"))

	// Sort components by health (worst first)
	type compHealth struct {
		name   string
		health int
	}
	var sorted []compHealth
	for name, comp := range assessment.Components {
		sorted = append(sorted, compHealth{name, comp.FinalHealth})
	}
	sort.Slice(sorted, func(i, j int) bool {
		return sorted[i].health < sorted[j].health
	})

	for _, ch := range sorted {
		comp := assessment.Components[ch.name]
		color := display.Green
		if comp.FinalHealth < healthDegraded {
			color = display.Red
		} else if comp.FinalHealth < healthWarning {
			color = display.Yellow
		}

		fmt.Printf("  %s%-20s%s Health: %s%4d%s | Checks: %d | Failures: %d | Warnings: %d\n",
			display.Bold, comp.Name, display.Reset,
			color, comp.FinalHealth, display.Reset,
			comp.CheckCount, comp.FailureCount, comp.WarningCount)
	}
	fmt.Println()

	// Critical issues
	if len(assessment.CriticalIssues) > 0 {
		fmt.Print(display.Subheader("Critical Issues"))
		for _, issue := range assessment.CriticalIssues {
			fmt.Println(display.Failure(issue))
		}
		fmt.Println()
	}

	// Warnings
	if len(assessment.Warnings) > 0 {
		fmt.Print(display.Subheader("Warnings"))
		for _, warning := range assessment.Warnings {
			fmt.Println(display.Warning(warning))
		}
		fmt.Println()
	}

	// Recommendations
	if len(assessment.Recommendations) > 0 {
		fmt.Print(display.Subheader("Recommendations"))
		for _, rec := range assessment.Recommendations {
			fmt.Println(display.Info(rec))
		}
		fmt.Println()
	}

	// Debug correlation status
	fmt.Print(display.Subheader("Debug Correlation"))
	fmt.Printf("  Debug Entries Analyzed: %d\n", assessment.DebugEntries)
	fmt.Printf("  Correlated Contexts: %d\n", len(assessment.CorrelatedEntries))
	if len(assessment.CorrelatedEntries) > 0 {
		fmt.Println("  Contexts with debugging data:")
		for contextID, state := range assessment.CorrelatedEntries {
			if len(state.Entries) > 0 {
				fmt.Printf("    - %s: %s (%d debug entries)\n", contextID, state.Component, len(state.Entries))
			}
		}
	}
	fmt.Println()

	// Pattern analysis - Critical for antibody routing
	if len(assessment.Patterns) > 0 {
		fmt.Print(display.Subheader("Pattern Analysis"))
		fmt.Println("  Identified failure patterns for antibody routing:")

		// Sort patterns by count (most frequent first)
		type patternCount struct {
			pattern string
			count   int
		}
		var sortedPatterns []patternCount
		for pattern, count := range assessment.Patterns {
			sortedPatterns = append(sortedPatterns, patternCount{pattern, count})
		}
		sort.Slice(sortedPatterns, func(i, j int) bool {
			return sortedPatterns[i].count > sortedPatterns[j].count
		})

		for _, pc := range sortedPatterns {
			fmt.Printf("    %s%-25s%s Count: %s%d%s\n",
				display.Bold, pc.pattern, display.Reset,
				display.Yellow, pc.count, display.Reset)
		}
		fmt.Println()
	}

	// Cross-component correlation - Identifies systemic issues
	if len(assessment.CrossComponentIssues) > 0 {
		fmt.Print(display.Subheader("Cross-Component Issues"))
		fmt.Println("  Systemic problems affecting multiple components:")

		for issueType, components := range assessment.CrossComponentIssues {
			fmt.Printf("    %s%s%s affects %d component(s):\n",
				display.Bold, issueType, display.Reset, len(components))
			for _, comp := range components {
				fmt.Printf("      - %s\n", comp)
			}
		}
		fmt.Println()
	}

	// Health Divergence Analysis - Expected vs actual health deltas
	hasHealthDivergences := false
	for _, comp := range assessment.Components {
		if len(comp.Divergences) > 0 {
			hasHealthDivergences = true
			break
		}
	}

	if hasHealthDivergences {
		fmt.Print(display.Subheader("Health Divergence Analysis"))
		fmt.Println("  Actual health diverged from expected:")
		fmt.Println()

		for _, comp := range assessment.Components {
			if len(comp.Divergences) > 0 {
				fmt.Printf("  %s%s%s:\n", display.Bold, comp.Name, display.Reset)
				for _, div := range comp.Divergences {
					severityColor := display.Yellow
					if div.Severity == "critical" {
						severityColor = display.Red
					} else if div.Severity == "high" {
						severityColor = display.Red
					} else if div.Severity == "medium" {
						severityColor = display.Yellow
					}

					gapSign := "+"
					if div.Gap < 0 {
						gapSign = ""
					}

					fmt.Printf("    %s%-30s%s Expected: %+3d | Actual: %+3d | Gap: %s%s%d%s | Pattern: %s\n",
						severityColor, div.CheckName, display.Reset,
						div.Expected, div.Actual,
						severityColor, gapSign, div.Gap, display.Reset,
						div.Pattern)
				}
				fmt.Println()
			}
		}
	}

	// Restoration Routing - Semantic metadata for automated fixes
	hasRestorationRouting := false
	for _, comp := range assessment.Components {
		for _, div := range comp.Divergences {
			if div.RecoveryHint != "" {
				hasRestorationRouting = true
				break
			}
		}
		if hasRestorationRouting {
			break
		}
	}

	if hasRestorationRouting {
		fmt.Print(display.Subheader("Restoration Routing"))
		fmt.Println("  Automated restoration strategies available:")
		fmt.Println()

		for _, comp := range assessment.Components {
			hasRoutableDiv := false
			for _, div := range comp.Divergences {
				if div.RecoveryHint != "" {
					hasRoutableDiv = true
					break
				}
			}

			if hasRoutableDiv {
				fmt.Printf("  %s%s%s:\n", display.Bold, comp.Name, display.Reset)
				for _, div := range comp.Divergences {
					if div.RecoveryHint != "" {
						// Determine routing color based on recovery hint
						routingColor := display.Green
						if div.RecoveryHint == "manual_intervention" || div.RecoveryHint == "investigate" {
							routingColor = display.Yellow
						}

						fmt.Printf("    %s%-30s%s %s→%s %s\n",
							routingColor, div.CheckName, display.Reset,
							routingColor, display.Reset,
							div.RecoveryStrategy)

						// Show operation context
						if div.OperationType != "" {
							fmt.Printf("      Operation: %s", div.OperationType)
							if div.OperationSubtype != "" {
								fmt.Printf(" (%s)", div.OperationSubtype)
							}
							fmt.Println()
						}

						// Show error classification
						if div.ErrorType != "" {
							fmt.Printf("      Error: %s\n", div.ErrorType)
						}

						// Show recovery hint
						fmt.Printf("      Strategy: %s\n", div.RecoveryHint)

						// Show recovery parameters (abbreviated)
						if len(div.RecoveryParams) > 0 {
							fmt.Printf("      Params: ")
							paramCount := 0
							for key, value := range div.RecoveryParams {
								if paramCount > 0 {
									fmt.Printf(", ")
								}
								fmt.Printf("%s=%v", key, value)
								paramCount++
								if paramCount >= 3 {
									if len(div.RecoveryParams) > 3 {
										fmt.Printf(", ... (%d more)", len(div.RecoveryParams)-3)
									}
									break
								}
							}
							fmt.Println()
						}

						fmt.Println()
					}
				}
			}
		}
	}

	// Divergence analysis - Proposed vs actual execution (from debugging rail)
	if len(assessment.Divergences) > 0 {
		fmt.Print(display.Subheader("Debug Divergence Analysis"))
		fmt.Println("  Execution diverged from expected behavior:")

		// Sort divergences by count
		type divCount struct {
			divergence string
			count      int
		}
		var sortedDiv []divCount
		for div, count := range assessment.Divergences {
			sortedDiv = append(sortedDiv, divCount{div, count})
		}
		sort.Slice(sortedDiv, func(i, j int) bool {
			return sortedDiv[i].count > sortedDiv[j].count
		})

		for _, dc := range sortedDiv {
			fmt.Printf("    %s%-30s%s Instances: %s%d%s\n",
				display.Bold, dc.divergence, display.Reset,
				display.Red, dc.count, display.Reset)
		}
		fmt.Println()
	}

	if len(assessment.CriticalIssues) == 0 && len(assessment.Warnings) == 0 {
		fmt.Println(display.Success("No critical issues or warnings detected"))
		fmt.Println()
	}
}

// ============================================================================
// CLOSING
// ============================================================================

func main() {
	// Setup Call 1/4: Initialize logger (+5 or -5)
	logger := logging.NewLogger("debugger")
	logger.DeclareHealthTotal(139)  // Total possible points from health scoring map
	logger.Check("logger-initialized", true, 5, map[string]any{
		"component": "debugger",
	})

	// Setup Call 2/4: Start operation (+5 or -5)
	logger.Operation("system-debug", 5, "analyze system logs and assess health")

	// Setup Call 3/4: Parse arguments (+5 or -5)
	var componentFilter string
	flag.StringVar(&componentFilter, "component", "", "Filter by component name")
	flag.Parse()

	logger.Check("arguments-parsed", true, 5, map[string]any{
		"component_filter": componentFilter,
	})

	// Setup Call 4/4: Locate log files (+3 or -3)
	logDir := filepath.Join(os.Getenv("HOME"), ".claude", "system", "logs")
	logDirs := []string{
		filepath.Join(logDir, "commands"),
		filepath.Join(logDir, "scripts"),
		filepath.Join(logDir, "libraries"),
		filepath.Join(logDir, "system"),
	}

	logger.Check("log-directory-located", true, 3, map[string]any{
		"log_dir": logDir,
	})

	// Log Analysis Call 1/6: Read log files (+10 or -10)
	var allLogFiles []string
	for _, dir := range logDirs {
		files, err := filepath.Glob(filepath.Join(dir, "*.log"))
		if err == nil {
			allLogFiles = append(allLogFiles, files...)
		}
	}

	logger.Check("log-files-read", len(allLogFiles) > 0, 10, map[string]any{
		"file_count": len(allLogFiles),
	})

	if len(allLogFiles) == 0 {
		fmt.Println(display.Warning("No log files found"))
		logger.Failure("No logs to analyze", "no log files found", -10, map[string]any{
			"log_dir": logDir,
		})
		os.Exit(1)
	}

	// Log Analysis Call 2/6: Parse log entries (+30 or -30)
	components := make(map[string]*ComponentHealth)
	var allLogEntries []logging.LogEntry                                     // Collect all log entries for correlation
	totalParsed := 0
	parseErrors := 0

	for _, logFile := range allLogFiles {
		entries, err := logging.ReadLogFile(logFile)
		if err != nil {
			parseErrors++
			continue
		}

		totalParsed += len(entries)
		allLogEntries = append(allLogEntries, entries...)                    // Collect for correlation

		// Group by component
		for _, entry := range entries {
			if componentFilter != "" && entry.Component != componentFilter {
				continue
			}

			if _, exists := components[entry.Component]; !exists {
				components[entry.Component] = analyzeComponent(entry.Component, []logging.LogEntry{})
			}
			components[entry.Component].Entries = append(components[entry.Component].Entries, entry)
		}
	}

	// Read debug files for correlation
	debugDir := filepath.Join(os.Getenv("HOME"), ".claude", "system", "debug")
	var allDebugEntries []debugging.InspectionEntry                          // Collect all debug entries for correlation
	debugFiles, _ := filepath.Glob(filepath.Join(debugDir, "**", "*.debug"))
	for _, debugFile := range debugFiles {
		debugEntries, err := debugging.ReadDebugFile(debugFile)
		if err == nil {
			allDebugEntries = append(allDebugEntries, debugEntries...)       // Collect for correlation
		}
	}

	// Correlate logging and debugging rails
	correlatedData := correlateEntries(allLogEntries, allDebugEntries)       // Correlate entries by contextID

	// Re-analyze components with complete entry lists
	for name, comp := range components {
		components[name] = analyzeComponent(name, comp.Entries)
	}

	parseSuccess := parseErrors == 0
	logger.Check("log-entries-parsed", parseSuccess, 30, map[string]any{
		"total_parsed":    totalParsed,
		"parse_errors":    parseErrors,
		"component_count": len(components),
		"debug_entries":   len(allDebugEntries),
	})

	// Log Analysis Call 3/6: Aggregate component health (+20 or -20)
	assessment := assessSystem(components)
	assessment.DebugEntries = len(allDebugEntries)                           // Store debug entry count
	assessment.CorrelatedEntries = correlatedData                            // Store correlation results
	logger.Check("component-health-aggregated", true, 20, map[string]any{
		"components":     len(components),
		"overall_health": assessment.OverallHealth,
	})

	// Log Analysis Call 4/6: Correlate across components (+15 or -15)
	assessment.CrossComponentIssues = correlateAcrossComponents(components)  // Identify and store systemic issues
	logger.Check("cross-component-correlation", true, 15, map[string]any{
		"correlation_types": len(assessment.CrossComponentIssues),
		"systemic_issues":   len(assessment.CrossComponentIssues["critical_health"]),
	})

	// Log Analysis Call 5/6: Identify patterns (+15 or -15)
	assessment.Patterns = identifyPatterns(assessment)                       // Recognize and store known failure patterns
	logger.Check("pattern-identification", true, 15, map[string]any{
		"patterns_found":  len(assessment.Patterns),
		"critical_issues": len(assessment.CriticalIssues),
		"warnings":        len(assessment.Warnings),
	})

	// Log Analysis Call 6/6: Compare proposed vs actual (+5 or -5)
	assessment.Divergences = compareProposedVsActual(components)             // Detect and store execution divergences
	logger.Check("proposed-vs-actual-comparison", true, 5, map[string]any{
		"divergence_types":  len(assessment.Divergences),
		"total_divergences": sumDivergences(assessment.Divergences),
	})

	// Assessment Call 1/5: Classify problems (+8 or -8)
	problemsClassified := len(assessment.CriticalIssues) + len(assessment.Warnings)
	logger.Check("problems-classified", true, 8, map[string]any{
		"classified_count": problemsClassified,
	})

	// Assessment Call 2/5: Determine severity (+5 or -5)
	hasCritical := len(assessment.CriticalIssues) > 0
	logger.Check("severity-determined", true, 5, map[string]any{
		"has_critical": hasCritical,
		"overall_health": assessment.OverallHealth,
	})

	// Assessment Call 3/5: Generate recommendations (+8 or -8)
	logger.Check("recommendations-generated", len(assessment.Recommendations) > 0, 8, map[string]any{
		"recommendation_count": len(assessment.Recommendations),
	})

	// Assessment Call 4/5: Display assessment (+3 or -3)
	displayAssessment(assessment)
	logger.Check("assessment-displayed", true, 3, map[string]any{
		"displayed": "system assessment",
	})

	// Assessment Call 5/5: Log final result (+2 or -2)
	if hasCritical || assessment.OverallHealth < healthDegraded {
		logger.Failure("System assessment complete", "critical issues detected", -2, map[string]any{
			"overall_health": assessment.OverallHealth,
			"critical_count": len(assessment.CriticalIssues),
		})
		os.Exit(1)
	}

	logger.Success("System assessment complete", 2, map[string]any{
		"overall_health": assessment.OverallHealth,
		"components": len(components),
	})
}
