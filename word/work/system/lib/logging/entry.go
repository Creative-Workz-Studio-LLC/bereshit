// ============================================================================
// METADATA
// ============================================================================
// Entry Construction & Formatting - Logging Library
//
// Biblical Foundation
//
// Scripture: "A word fitly spoken is like apples of gold in pictures of silver" (Proverbs 25:11, KJV)
// Principle: Proper formatting creates clear communication. Well-structured entries enable understanding and discernment.
// Anchor: Log entries are communication - from execution moment to future debugging. Structure serves comprehension.
//
// CPI-SI Identity
//
// Component Type: Entry formatting module within Rails infrastructure
// Role: Build and format log entries for output (Detection layer of immune system)
// Paradigm: CPI-SI framework component
//
// Authorship & Lineage
//
// Architect: Seanje Lenox-Wise, Nova Dawn
// Implementation: Nova Dawn
// Creation Date: 2025-11-18
// Version: 1.0.0
// Last Modified: 2025-11-18 - Extracted from monolithic logger.go
//
// Purpose & Function
//
// Purpose: Construct and format log entries according to documented standard. Converts LogEntry structures into human-readable formatted text for file output.
//
// Core Design: Structured formatting with consistent indentation, headers, and sections. CONTEXT → EVENT → DETAILS → INTERACTIONS hierarchy.
//
// Key Features:
//   - Base entry creation with common fields
//   - Full entry formatting with all sections
//   - Field writing helpers (writeField, writeDetailValue)
//   - Map/list section helpers (writeMapSection, writeListSection)
//   - Health indicator and delta formatting
//   - User identifier formatting (user@host:pid)
//
// Blocking Status
//
// Non-blocking: Entry formatting always succeeds. If formatting fails, returns basic entry with error noted.
// Mitigation: All operations have sensible defaults and graceful degradation.
//
// Usage & Integration
//
// Usage:
//
//	import "system/runtime/lib/logging"
//
// Integration Pattern:
//   1. Logger calls createBaseEntry() to build LogEntry structure
//   2. formatEntry() converts LogEntry to formatted string
//   3. Helper functions (writeField, writeMapSection, etc.) build sections
//   4. Output goes to writeEntry() for file writing
//
// Public API:
//
//   createBaseEntry(context, healthImpact) LogEntry - Build entry with common fields (Logger method)
//   formatEntry(entry) string - Convert entry to formatted text (Logger method)
//
// Dependencies
//
// Dependencies (What This Needs):
//   Standard Library: fmt, strings, time
//   Package Files: context.go (SystemContext type), health.go (getHealthIndicator, getHealthBar)
//
// Dependents (What Uses This):
//   Internal: logger.go (all logging methods use createBaseEntry and formatEntry)
//
// Health Scoring
//
// Base100 scoring algorithm (CPSI-ALG-001).
//
// Entry Formatting Operations (20 pts):
//   - Base entry creation: +5 (all fields populated correctly)
//   - Entry formatting: +10 (valid structure with all sections)
//   - Field writing: +3 (proper indentation and format)
//   - Section writing: +2 (correct header and content)
//
// Note: This module's health is about formatting correctness, not log content quality.

package logging

// ============================================================================
// SETUP
// ============================================================================

// Imports

import (
	"fmt"     // String formatting for entry output
	"strings" // String manipulation for building entries
	"time"    // Timestamp handling
)

// Constants

const (
	//--- Format Strings ---
	// Entry section headers and formatting.

	timestampFormat    = "2006-01-02 15:04:05.000"   // Standard log timestamp format (microsecond precision)
	contextHeader      = "  CONTEXT:\n"              // Header for context section
	eventHeader        = "  EVENT: "                 // Prefix for event description
	detailsHeader      = "  DETAILS:\n"              // Header for details section
	interactionsHeader = "  INTERACTIONS:\n"         // Header for interactions section
	entrySeparator     = "---"                       // Separator between log entries
)

// Types

// Interactions captures what else was happening when this operation ran (optional).
//
// Used by LogEntry for complex scenario tracking. Records concurrent operations,
// dependencies, and state changes to enable debugging of race conditions and
// unexpected interactions.
type Interactions struct {
	Concurrent   []string          // Operations running simultaneously (race condition tracking)
	Dependencies map[string]string // Requirements and provisions (dependency analysis)
	StateChanges map[string]string // Before/after values (mutation tracking)
}

// LogEntry is one complete log entry - everything about one moment.
//
// Final composition combining all pieces: context, event, details, health,
// interactions. This is what gets written to log files and parsed by debugging.
type LogEntry struct {
	Timestamp        time.Time      // Exact moment (microsecond precision)
	Level            string         // Entry type (OPERATION, SUCCESS, FAILURE, ERROR, CHECK, CONTEXT, DEBUG)
	Component        string         // Logging component name
	User             string         // WHO identifier (user@host:pid format)
	ContextID        string         // Execution context ID (links related entries: component-pid-timestamp)
	Context          *SystemContext // Full environment snapshot (nil for lightweight entries)
	Event            string         // Human description of occurrence
	Details          map[string]any // Structured data (command, exit_code, duration, stdout, stderr)
	Interactions     *Interactions  // Optional complexity tracking
	Semantic         *Metadata      // Optional restoration routing metadata
	RawHealth        int            // Cumulative health (sum of all deltas)
	NormalizedHealth int            // Health percentage (-100 to +100)
	HealthImpact     int            // This event's delta (Δ)
}

// Metadata captures semantic information for restoration routing (optional).
//
// Used by LogEntry.Semantic field. Provides structured error classification,
// recovery hints, and state contracts for the restoration layer (future).
type Metadata struct {
	// Operation classification
	OperationType    string         // Primary category (file_validation, system_operation, etc.)
	OperationSubtype string         // Granular sub-type (syntax_check, permission_check, etc.)

	// Error information (only for failures)
	ErrorType    string         // Error classification (permission_denied, file_not_found, etc.)
	ErrorDetails map[string]any // Structured error context

	// Recovery routing
	RecoveryHint     string         // Hint for restoration routing (automated_fix, manual_intervention, etc.)
	RecoveryStrategy string         // Specific antibody to use (fix_file_permissions, install_package, etc.)
	RecoveryParams   map[string]any // Parameters for antibody execution

	// State contracts (inspector usage)
	Expected map[string]any // Expected state
	Actual   map[string]any // Actual state
}

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// Helpers - Foundation Functions
// ────────────────────────────────────────────────────────────────

// formatDeltaSign formats health delta with appropriate sign prefix.
//
// Returns formatted string like "+10" or "-5" for visual clarity in logs.
func formatDeltaSign(delta int) string {
	if delta > 0 {           // Positive delta
		return fmt.Sprintf("+%d", delta) // Add explicit + sign
	}
	return fmt.Sprintf("%d", delta) // Negative already has - sign
}

// formatUserIdentifier formats user identity as user@host:pid.
//
// Creates standardized WHO identifier for log entries.
func formatUserIdentifier(context *SystemContext) string {
	return fmt.Sprintf("%s@%s:%d", context.User, context.Host, context.PID)
}

// writeField writes a single key-value pair with consistent 4-space indentation.
func writeField(builder *strings.Builder, key string, value string) {
	fmt.Fprintf(builder, "    %s: %s\n", key, value) // Write with 4-space indent
}

// writeDetailValue writes a detail entry, handling both single-line and multiline values.
func writeDetailValue(builder *strings.Builder, key string, value any) {
	// Check if value is multiline string (contains newlines)
	if str, ok := value.(string); ok && strings.Contains(str, "\n") { // Multiline value detected
		fmt.Fprintf(builder, "    %s: |\n", key)                 // Write key with "|" indicator
		for line := range strings.SplitSeq(str, "\n") {          // Iterate through lines using iterator
			fmt.Fprintf(builder, "      %s\n", line)             // Write line with 6-space indent
		}
	} else { // Single-line value
		fmt.Fprintf(builder, "    %s: %v\n", key, value) // Write directly (4-space indent)
	}
}

// writeMapKeyValues writes all key-value pairs from a map with consistent indentation.
func writeMapKeyValues(builder *strings.Builder, data map[string]string) {
	for key, value := range data {                      // Iterate through all map entries
		fmt.Fprintf(builder, "      %s: %s\n", key, value) // Write with 6-space indent
	}
}

// writeMapSection writes a map section with header if data exists.
func writeMapSection(builder *strings.Builder, sectionName string, data map[string]string) {
	if len(data) == 0 {                             // No data to write
		return                                      // Skip entire section
	}
	fmt.Fprintf(builder, "    %s:\n", sectionName)  // Write section header (4-space indent)
	writeMapKeyValues(builder, data)                // Write all key-value pairs (6-space indent)
}

// writeListSection writes a list section with header if items exist.
func writeListSection(builder *strings.Builder, sectionName string, items []string) {
	if len(items) == 0 {                            // No items to write
		return                                      // Skip entire section
	}
	fmt.Fprintf(builder, "    %s:\n", sectionName)  // Write section header (4-space indent)
	for _, item := range items {                    // Iterate through all items
		fmt.Fprintf(builder, "      - %s\n", item) // Write with list marker (6-space indent)
	}
}

// ────────────────────────────────────────────────────────────────
// Core Operations - Entry Construction
// ────────────────────────────────────────────────────────────────

// createBaseEntry creates a LogEntry with common fields populated.
func (l *Logger) createBaseEntry(context *SystemContext, healthImpact int) LogEntry {
	return LogEntry{
		Timestamp:        time.Now(),                    // Capture current time
		Component:        l.Component,                   // Component name from logger
		User:             formatUserIdentifier(context), // Formatted user@host:pid
		ContextID:        l.ContextID,                   // Unique execution identifier
		RawHealth:        l.SessionHealth,               // Current raw cumulative health
		NormalizedHealth: l.NormalizedHealth,            // Current normalized percentage
		HealthImpact:     healthImpact,                  // Health delta for this event
	}
}

// formatEntry formats a LogEntry according to the documented standard.
func (l *Logger) formatEntry(entry LogEntry) string {
	var builder strings.Builder // Efficient string building

	// First line: Timestamp, Level, Component
	fmt.Fprintf(&builder, "[%s] %s %s\n",
		entry.Timestamp.Format(timestampFormat), // Formatted timestamp
		entry.Level,                              // Log level
		entry.Component,                          // Component name
	)

	// CONTEXT section (if full context captured)
	if entry.Context != nil { // Full context available
		builder.WriteString(contextHeader) // Write section header

		// Basic WHO/WHERE/WHEN fields
		writeField(&builder, "User", entry.User)                                 // user@host:pid
		writeField(&builder, "Context ID", entry.ContextID)                      // Execution context ID
		writeField(&builder, "Shell", entry.Context.Shell.Format())              // Shell description (from context.go)
		writeField(&builder, "CWD", entry.Context.CWD)                           // Current working directory

		// Environment state (if any vars captured)
		writeMapSection(&builder, "Environment", entry.Context.EnvState)         // Automation + framework vars

		// Sudoers configuration
		writeMapSection(&builder, "Sudoers", entry.Context.Sudoers.ToMap())      // Installation + permissions

		// System metrics
		writeMapSection(&builder, "System Metrics", entry.Context.System.ToMap()) // Load, memory, disk
	}

	// EVENT section (always present)
	fmt.Fprintf(&builder, "%s%s\n", eventHeader, entry.Event) // Event description

	// DETAILS section (if any details provided)
	if len(entry.Details) > 0 { // Details exist
		builder.WriteString(detailsHeader) // Write section header
		for key, value := range entry.Details { // Iterate all detail fields
			writeDetailValue(&builder, key, value) // Write each field with proper formatting
		}
	}

	// INTERACTIONS section (if tracking concurrent/dependencies)
	if entry.Interactions != nil { // Interactions tracked
		builder.WriteString(interactionsHeader) // Write section header
		writeListSection(&builder, "Concurrent", entry.Interactions.Concurrent)       // Concurrent operations
		writeMapSection(&builder, "Dependencies", entry.Interactions.Dependencies)    // Dependency relationships
		writeMapSection(&builder, "State Changes", entry.Interactions.StateChanges)   // Before/after values
	}

	// Health scoring (always present)
	healthIndicator := getHealthIndicator(entry.NormalizedHealth) // Get emoji from health.go
	healthBar := getHealthBar(entry.NormalizedHealth)             // Get progress bar from health.go
	delta := formatDeltaSign(entry.HealthImpact)                  // Format delta with sign

	fmt.Fprintf(&builder, "  HEALTH: %s %s (Δ%s, Raw: %d)\n",
		healthIndicator,          // Visual emoji indicator
		healthBar,                // ASCII progress bar
		delta,                    // Delta with sign
		entry.RawHealth,          // Raw cumulative score
	)

	// Entry separator
	fmt.Fprintf(&builder, "%s\n", entrySeparator) // Entry separator line

	return builder.String() // Return complete formatted entry
}

// ============================================================================
// CLOSING
// ============================================================================
// Library module (no entry point). Import: "system/runtime/lib/logging"
//
// ============================================================================
// END CLOSING
// ============================================================================
