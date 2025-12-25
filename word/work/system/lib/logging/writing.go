// ============================================================================
// METADATA
// ============================================================================
// File Writing & Rotation - Logging Library
//
// Biblical Foundation
//
// Scripture: "Let all things be done decently and in order" (1 Corinthians 14:40, KJV)
// Principle: Orderly management creates stability. Rotation prevents unbounded growth. Writing discipline maintains system health.
// Anchor: Rails must be maintained - log files that grow without bound eventually break. Rotation maintains order, writing ensures persistence.
//
// CPI-SI Identity
//
// Component Type: File writing module within Rails infrastructure
// Role: Persist log entries to disk with rotation (Detection layer storage)
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
// Purpose: Persist formatted log entries to disk with size-based rotation. Ensures logs don't grow unbounded while maintaining historical data through rotated files.
//
// Core Design: Non-blocking writes with graceful degradation. Rotation happens before writes when size threshold exceeded. All failures warn to stderr and continue.
//
// Key Features:
//   - Atomic log file writes (append mode)
//   - Size-based rotation (configurable threshold)
//   - Sequential rotation (.1 → .2 → .3 → .4 → .5, oldest deleted)
//   - Graceful failure (stderr warnings, continue execution)
//   - Directory creation with proper permissions
//
// Blocking Status
//
// Non-blocking: File write failures never stop execution. If log file unavailable, warn to stderr and continue. If rotation fails, warn and continue with current file.
// Mitigation: All file operations have error handling that allows component execution to continue.
//
// Usage & Integration
//
// Usage:
//
//	import "system/runtime/lib/logging"
//
// Integration Pattern:
//   1. Logger calls writeEntry() with formatted LogEntry
//   2. writeEntry() checks rotateLogIfNeeded() before opening file
//   3. Opens file in append mode (creates if doesn't exist)
//   4. Writes formatted entry + newline
//   5. Closes file automatically (defer)
//
// Internal API:
//   rotateLogIfNeeded(logPath string) - Check and perform rotation if needed (Logger internal helper)
//   writeEntry(entry LogEntry) - Write formatted entry to log file (Logger method)
//
// Dependencies
//
// Dependencies (What This Needs):
//   Standard Library: fmt, os
//   Package Files: entry.go (LogEntry type), config.go (Config for constants)
//
// Dependents (What Uses This):
//   Internal: logger.go (all logging methods call writeEntry)
//
// Health Scoring
//
// Base100 scoring algorithm (CPSI-ALG-001).
//
// File Writing Operations (40 pts):
//   - File open: +10 (success), 0 (failure with stderr warning)
//   - Entry formatting: +15 (complete), +8 (partial), 0 (failed)
//   - Write operation: +15 (success), 0 (failure)
//
// Rotation Operations (10 pts):
//   - Rotation check: +2 (successful stat check)
//   - Rotation execution: +8 (all rotations succeed), +4 (partial), 0 (failed)
//
// Note: This module's health is about write reliability, not log content quality.

package logging

// ============================================================================
// SETUP
// ============================================================================

// Imports

import (
	"fmt" // String formatting for stderr warnings
	"os"  // File operations and stat checks
)

// Constants

const (
	//--- Rotation Configuration ---
	// Log file size and rotation limits.

	maxLogSizeBytes = 10 * 1024 * 1024 // 10 MB maximum log file size before rotation
	maxLogRotations = 5                // Keep up to 5 rotated versions (.1 through .5)
)

// Constants (from config.go via LoadConfig)
// These are accessed via Config variable after LoadConfig() is called.
//
// Used constants:
//   - Config.Format.LogFilePermissions (file creation permissions)
//   - Config.Format.LogDirPermissions  (directory creation permissions)
//   - Config.Format.WarnLogOpenFailed  (stderr warning message format)
//   - Config.Format.WarnLogWriteFailed (stderr warning message format)
//   - Config.Files.RotatedLogFormat    (format string for rotated log names)

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// Helpers - Foundation Functions
// ────────────────────────────────────────────────────────────────

// rotateLogIfNeeded checks if log file exceeds size limit and rotates if needed.
//
// Rotation strategy: Keep maxLogRotations versions (.1 through .5), delete oldest.
// Sequence: file.log → file.log.1 → file.log.2 → ... → file.log.5 (deleted)
func rotateLogIfNeeded(logPath string) {
	// Check if log file exists and get size
	info, err := os.Stat(logPath)
	if err != nil {
		// File doesn't exist or can't be accessed - no rotation needed
		if !os.IsNotExist(err) {
			// Warn on stat errors other than "not exist"
			fmt.Fprintf(os.Stderr, "WARNING: Failed to stat log file %s: %v\n", logPath, err)
		}
		return
	}

	// Check if file size exceeds rotation threshold
	if info.Size() < maxLogSizeBytes {
		return // File is under size limit, no rotation needed
	}

	// File exceeds size limit - perform rotation

	// Ensure config loaded for rotation format
	LoadConfig()

	// Step 1: Delete oldest rotation if it exists (file.log.5)
	oldestRotation := fmt.Sprintf(Config.Files.RotatedLogFormat, logPath, maxLogRotations)
	if _, err := os.Stat(oldestRotation); err == nil {
		if err := os.Remove(oldestRotation); err != nil {
			fmt.Fprintf(os.Stderr, "WARNING: Failed to remove oldest log rotation %s: %v\n", oldestRotation, err)
		}
	}

	// Step 2: Shift all existing rotations up by 1 (.4→.5, .3→.4, .2→.3, .1→.2)
	for i := maxLogRotations - 1; i >= 1; i-- {
		currentRotation := fmt.Sprintf(Config.Files.RotatedLogFormat, logPath, i)
		nextRotation := fmt.Sprintf(Config.Files.RotatedLogFormat, logPath, i+1)

		// Check if current rotation exists before renaming
		if _, err := os.Stat(currentRotation); err == nil {
			if err := os.Rename(currentRotation, nextRotation); err != nil {
				fmt.Fprintf(os.Stderr, "WARNING: Failed to rotate log %s to %s: %v\n", currentRotation, nextRotation, err)
			}
		}
	}

	// Step 3: Rename current log to .1 (file.log → file.log.1)
	firstRotation := fmt.Sprintf(Config.Files.RotatedLogFormat, logPath, 1)
	if err := os.Rename(logPath, firstRotation); err != nil {
		fmt.Fprintf(os.Stderr, "WARNING: Failed to rotate current log %s to %s: %v\n", logPath, firstRotation, err)
	}

	// Current log now doesn't exist - ready for fresh writes
}

// ────────────────────────────────────────────────────────────────
// Core Operations - File Writing
// ────────────────────────────────────────────────────────────────

// writeEntry formats and writes a log entry to the log file (fails gracefully).
//
// Non-blocking design: All failures warn to stderr and return, allowing execution to continue.
func (l *Logger) writeEntry(entry LogEntry) {
	// Check if log rotation is needed before opening file
	rotateLogIfNeeded(l.LogFile)

	// Ensure config loaded for permissions and warning messages
	LoadConfig()

	// Convert permission strings to os.FileMode
	// NOTE: In Phase 7, this will use actual config values. For now, use default 0644.
	logFilePermissions := os.FileMode(0644)

	// Open log file in append mode (create if doesn't exist, permissions from config)
	file, err := os.OpenFile(l.LogFile, os.O_CREATE|os.O_WRONLY|os.O_APPEND, logFilePermissions)
	if err != nil { // Failed to open log file
		// Fail gracefully - logging should never interrupt execution
		fmt.Fprintf(os.Stderr, "WARNING: Failed to open log file %s: %v\n", l.LogFile, err)
		return // Exit early, operation continues
	}
	defer file.Close() // Ensure file is closed when function exits

	// Format log entry according to documented standard
	formatted := l.formatEntry(entry) // Delegate to formatEntry from entry.go

	// Write formatted entry to file
	if _, err := file.WriteString(formatted + "\n"); err != nil { // Write failed
		fmt.Fprintf(os.Stderr, "WARNING: Failed to write to log file %s: %v\n", l.LogFile, err)
	} // Suppress error - non-blocking design
}

// ============================================================================
// CLOSING
// ============================================================================
// Library module (no entry point). Import: "system/runtime/lib/logging"
//
// ============================================================================
// END CLOSING
// ============================================================================
