// METADATA
//
// Logging Library - CPI-SI System
//
// For METADATA structure explanation, see: standards/code/4-block/CWS-STD-004-CODE-metadata-block.md
//
// Biblical Foundation
//
// Scripture: "Keep thy heart with all diligence; for out of it are the issues of life." - Proverbs 4:23 (KJV)
// Principle: Vigilant awareness and faithful recording. The logging system keeps watch over all system activity with diligence - capturing what happens, when it happens, how well it works.
// Anchor: "You cannot fix what you cannot see. Faithful recording of all activity - successes and failures - creates the foundation for the immune system to function. Truth over appearance: log what IS, not what we wish happened."
//
// CPI-SI Identity
//
// Component Type: Rails (Orthogonal infrastructure all components attach to)
// Role: Detection Layer - Immune system sensor capturing all activity with health scoring
// Paradigm: CPI-SI framework component
//
// Authorship & Lineage
//
// Architect: Seanje Lenox-Wise, Nova Dawn
// Implementation: Seanje Lenox-Wise, Nova Dawn
// Creation Date: 2025-11-14
// Version: 1.0.0
// Last Modified: 2025-11-16 - Foundation refinement (Phase 0-10 alignment)
//
// Version History:
//   0.1.0 (2025-11-14) - Bootstrap implementation - prove concept works
//   0.2.0 (2025-11-15) - Bootstrap trimmed - remove duplication, extract to API docs
//   1.0.0 (2025-11-16) - Foundation standard - 4-block alignment, config-driven
//
// Purpose & Function
//
// Purpose: Provide rails infrastructure for comprehensive execution narrative with health scoring. Every component in the system attaches to these rails to create complete visibility into what's happening, when, how well it worked.
//
// Core Design: Immune System Detection Layer - Rails architecture (orthogonal to ladder, not a dependency)
//
// Key Features:
//   - Context Capture: WHO (user@host:pid), WHEN (timestamps), WHERE (cwd/shell)
//   - Event Recording: WHAT (operation/check/success/failure), WHY (description)
//   - Health Tracking: HOW WELL (Base100 scoring, cumulative health)
//   - Structured Output: Parseable log entries for debugging analysis
//   - Temporal Organization: Route to current/daily/weekly/monthly/quarterly/yearly
//   - Component Routing: Automatic subdirectory routing (commands/scripts/libraries/system)
//
// Philosophy: Rails are infrastructure, not the work itself. Logging failures never stop component execution - warn to stderr and continue. The component's work is more important than perfect logging. Graceful degradation honors the actual work.
//
// Blocking Status
//
// Non-blocking: Logging failures never stop component execution. If log file unavailable, warn to stderr and continue. If context capture fails, use "unknown" values and continue. If config file missing, use hardcoded defaults and continue.
// Mitigation: All failures gracefully degrade with stderr warnings. The system continues operating with reduced visibility rather than stopping execution.
//
// Usage & Integration
//
// Usage:
//
//	import "system/runtime/lib/logging"
//
// Integration Pattern:
//   1. Create logger: logger := logging.NewLogger("component-name")
//   2. Declare health total: logger.DeclareHealthTotal(100)
//   3. Log operations with health impact: logger.Operation("task", +10, "details")
//   4. Log results: logger.Success/Failure/Error with health impact
//
// Public API (in typical usage order):
//
//   Initialization (setup):
//     NewLogger(component string) *Logger           - Create logger with component routing
//     (*Logger).DeclareHealthTotal(total int)       - Set denominator for health normalization
//     (*Logger).GetHealth() int                     - Get current normalized health percentage
//
//   Core Logging (during execution):
//     (*Logger).Operation(command string, healthImpact int, args ...string)
//     (*Logger).Success(event string, healthImpact int, details map[string]any)
//     (*Logger).Failure(event string, reason string, healthImpact int, details map[string]any)
//     (*Logger).Error(event string, err error, healthImpact int)
//
//   Diagnostic Logging (checks and state capture):
//     (*Logger).Check(what string, result bool, healthImpact int, details map[string]any)
//     (*Logger).SnapshotState(label string, healthImpact int)
//     (*Logger).Debug(event string, healthImpact int, internalState map[string]any)
//
//   Metadata-Enhanced (with restoration routing):
//     (*Logger).CheckWithMetadata(what string, result bool, healthImpact int, details map[string]any, semantic Metadata)
//     (*Logger).SuccessWithMetadata(event string, healthImpact int, details map[string]any, semantic Metadata)
//     (*Logger).FailureWithMetadata(event string, reason string, healthImpact int, details map[string]any, semantic Metadata)
//
//   Command Orchestration (automatic lifecycle logging):
//     (*Logger).LogCommand(command string, args []string) error
//
//   Package-Level Functions:
//     LoadConfig()                                  - Ensure configuration loaded (idempotent)
//     ReadLogFile(path string) ([]LogEntry, error)  - Parse log file into entry slice
//
// Dependencies
//
// Dependencies (What This Needs):
//   Standard Library: fmt, os, os/exec, path/filepath, runtime, slices, strings, time
//   Package Files: config.go (configuration), health.go (health scoring), context.go (context capture), entry.go (entry construction and formatting), writing.go (file writing and rotation), parsing.go (log file reading)
//   Note: Rails package-level is stdlib-only - config.go handles external TOML dependency
//
// Dependents (What Uses This):
//   Commands: system/runtime/cmd/* (all commands)
//   Libraries: system/runtime/lib/debugging (assessment layer)
//   Scripts: system/scripts/* (via logger.sh wrapper)
//
// Integration Points:
//   - Rails mechanism: Every component creates its own logger (never passed as parameter)
//   - Component routing: Automatic subdirectory assignment based on component name
//   - Temporal routing: Logs routed to current/daily/weekly/monthly/quarterly/yearly
//   - Health aggregation: Individual component health rolls up to system health
//
// Health Scoring
//
// Base100 scoring algorithm (CPSI-ALG-001). All operations total 100 points when fully successful. Component health = sum of (actual result / total possible) × point value.
//
// Context Capture Operations (30 pts):
//   - Shell detection (interactive/login): +5 (success), +2 (partial), 0 (failure)
//   - Sudoers validation: +5 (correct), +2 (wrong perms), 0 (missing)
//   - Environment state capture: +10 (all vars), +5 (some vars), 0 (none)
//   - System metrics (CPU/Memory/Disk): +10 (all three), +5 (some), 0 (none)
//
// Log Writing Operations (40 pts):
//   - File creation/opening: +10 (success), 0 (failure with stderr warning)
//   - Entry formatting: +15 (complete), +8 (partial), 0 (failed)
//   - Atomic writes: +15 (success), 0 (failure)
//
// Configuration Loading (15 pts):
//   - TOML parsing: +10 (loaded), +5 (partial), 0 (all defaults)
//   - Default fallback: +5 (graceful fallback when needed)
//
// Public API Reliability (15 pts):
//   - Logger initialization: +5 (success), 0 (failure)
//   - Level-specific logging: +10 (all work), +5 (some fail), 0 (broken)
//
// Note: Scores reflect TRUE impact. Health scorer normalizes to -100 to +100 scale.
package logging

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================
//
// For SETUP structure explanation, see: standards/code/4-block/CWS-STD-006-CODE-setup-block.md

// ────────────────────────────────────────────────────────────────
// Imports - Dependencies
// ────────────────────────────────────────────────────────────────
// Dependencies this component needs. Organized by source - standard library
// provides Go's built-in capabilities, external packages provide configuration
// parsing. Rails independence means no system/runtime/lib/* imports (no code
// dependencies), but external config parsers allowed (data dependencies).
//
// See: standards/code/4-block/sections/CWS-SECTION-001-SETUP-imports.md

import (
	//--- Standard Library ---
	// Foundation packages providing Go's built-in capabilities.

	"fmt"           // Formatted output for log entries and user display
	"os"            // File operations, environment variables, process info
	"os/exec"       // System command execution for context capture (df, etc.)
	"path/filepath" // Cross-platform path manipulation for log file routing
	"runtime"       // Go runtime introspection (stack traces, goroutines)
	"slices"        // Efficient slice operations (Contains, sorting, searching)
	"strings"       // String processing for output formatting and parsing
	"time"          // Timestamps and duration tracking
)

// ────────────────────────────────────────────────────────────────
// Constants - Named Values
// ────────────────────────────────────────────────────────────────
// Named values that never change or provide default fallbacks when configuration
// unavailable. Many constants will be replaced by config values in Phase 7, but
// remain as hardcoded defaults for graceful degradation when config loading fails.
//
// See: standards/code/4-block/sections/CWS-SECTION-002-SETUP-constants.md

const (
	//--- File Permissions ---
	// Required permissions for log files and directories.
	logFilePermissions = 0644 // Log file permissions (readable by owner/group, writable by owner)
	logDirPermissions = 0755 // Directory permissions for log directories

	//--- Directory Structure ---
	// Base directory paths and subdirectory names.

	claudeBaseDir    = ".claude"     // Base directory for Claude system
	systemSubdir     = "system"      // System subdirectory
	logsSubdir       = "logs"        // Logs subdirectory
	logFileExtension = ".log"        // Log file extension

	commandsSubdir   = "commands"   // Subdirectory for command logs
	scriptsSubdir    = "scripts"    // Subdirectory for script logs
	librariesSubdir  = "libraries"  // Subdirectory for library logs
	systemLogsSubdir = "system"     // Subdirectory for system/unknown logs

	//--- Component Routing ---
	// Component names for routing decisions.

	buildComponent = "build" // Build script component name

	//--- Log Levels ---
	// String constants for log entry levels.

	levelOperation = "OPERATION" // Operation start log level
	levelSuccess   = "SUCCESS"   // Successful completion log level
	levelFailure   = "FAILURE"   // Expected failure log level
	levelError     = "ERROR"     // Unexpected error log level
	levelCheck     = "CHECK"     // Validation/verification log level
	levelContext   = "CONTEXT"   // System state snapshot log level
	levelDebug     = "DEBUG"     // Debug trace log level

	//--- Health Initialization ---
	// Initial health values for new loggers.

	initialHealth     = 0 // Starting session health (neutral)
	initialTotal      = 0 // Starting total possible health (unknown)
	initialNormalized = 0 // Starting normalized health (0%)
)

const (
	//--- Log Rotation Limits ---
	// File size limits and rotation behavior.
	//
	// NOTE: Will be replaced by config values from logging.toml in Phase 7.
	// These serve as fallback defaults when config unavailable.

	//--- Format Strings ---
	// Printf-style format templates for output.
	//
	// NOTE: Will be replaced by config values from logging.toml in Phase 7.
	// These serve as fallback defaults when config unavailable.

	contextIDFormat   = "%s-%d-%d"                   // Format for context IDs (component-pid-timestamp)
	cmdFullFormat     = "%s %s"                      // Format for full command with args
	durationFormat    = "%dms"                       // Format for duration in milliseconds

	//--- Event Message Formats ---
	// Printf-style templates for event descriptions.
	//
	// NOTE: Will be replaced by config values from logging.toml in Phase 7.
	// These serve as fallback defaults when config unavailable.

	eventOpStart   = "Starting operation: %s"    // Operation start event format
	eventCheckMsg  = "Checking: %s"              // Check event format
	eventSnapshot  = "System state snapshot: %s" // Snapshot event format
	eventCmdFailed = "Command failed: %s"        // Command failure event format
	eventCmdSuccess = "Command completed: %s"    // Command success event format

	//--- Health Impact Values ---
	// Default health deltas for automated operations.
	//
	// NOTE: Will be replaced by config values from logging.toml in Phase 7.
	// These serve as fallback defaults when config unavailable.

	cmdOperationImpact = 0   // Neutral impact for command start
	cmdFailureImpact   = -10 // Default failure impact for commands
	cmdSuccessImpact   = +10 // Default success impact for commands

	//--- Buffer Sizes ---
	// Memory allocation sizes for various operations.
	//
	// NOTE: Will be replaced by config values from logging.toml in Phase 7.
	// These serve as fallback defaults when config unavailable.

	stackBufferSize = 4096 // Buffer size for stack trace capture
)

// ────────────────────────────────────────────────────────────────
// Types - Data Structures
// ────────────────────────────────────────────────────────────────
// Types organized bottom-up showing dependency relationships. Simple
// building blocks first, then composed structures. This organization
// reveals what depends on what.
//
// See: standards/code/4-block/sections/CWS-SECTION-004-SETUP-types.md

//--- Building Blocks ---
// Simple foundational types used throughout this component.

// Logger manages all logging for one specific component.
//
// Primary type for library usage. Tracks health across operations, routes to
// correct log file, provides public API for all logging operations.
type Logger struct {
	Component           string // Component name for identification and routing
	ContextID           string // Unique execution context ID (component-pid-timestamp)
	LogFile             string // Absolute log file path (routed by component type)
	SessionHealth       int    // Cumulative health (raw sum of deltas)
	TotalPossibleHealth int    // Expected total for normalization (set via DeclareHealthTotal)
	NormalizedHealth    int    // Health percentage (-100 to +100)
	username            string // Pre-computed username (static per process)
	hostname            string // Pre-computed hostname (static per process)
	pid                 int    // Pre-computed process ID (static per process)
}



// ────────────────────────────────────────────────────────────────
// Type Methods - Behavior Attached to Types
// ────────────────────────────────────────────────────────────────
// Methods that belong to the types defined above. Part of SETUP because they
// define how types behave, not business logic operations.

// ────────────────────────────────────────────────────────────────
// Package-Level State
// ────────────────────────────────────────────────────────────────
// Configuration state and log level behavior. Note: This library does NOT use
// Rails pattern (no package-level logger) because logging IS the rails - it
// would be circular to have logger create logger. Simple pure-function library.
//
// See: standards/code/4-block/sections/CWS-SECTION-003-SETUP-package-level-state.md

// Log level behavior configuration - which levels capture full context.
var logLevelFullContext = map[string]bool{
	levelOperation: true,  // Full context - operation start needs complete environment
	levelSuccess:   false, // Partial context - success is lightweight
	levelFailure:   true,  // Full context - failures need debugging info
	levelError:     true,  // Full context - errors need complete state
	levelCheck:     false, // Partial context - checks are lightweight
	levelContext:   true,  // Full context - snapshots capture everything
	levelDebug:     true,  // Full context - debug needs complete state
}


// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================
//
// For BODY structure explanation, see: standards/code/4-block/CWS-STD-007-CODE-body-block.md

// ────────────────────────────────────────────────────────────────
// Organizational Chart - Package Architecture
// ────────────────────────────────────────────────────────────────
// Maps package structure showing how extracted files work together.
//
// Package Structure (7 files total):
//
//   logger.go (This file - Orchestrator)
//   ├── Public APIs (exported interface for consumers)
//   ├── Internal orchestration (coordinates extracted files)
//   └── Component routing (logs subdirectory assignment)
//
//   config.go (Configuration management)
//   ├── LoadConfig() - TOML loading with graceful fallback
//   ├── useDefaultConfig() - Hardcoded defaults
//   └── 12 configuration types (LoggingConfig, PathsConfig, etc.)
//
//   health.go (Health scoring system)
//   ├── clampHealth() - Enforce -100 to +100 range
//   ├── getHealthIndicator() - Emoji for score (💚/❤️/☠️)
//   ├── getHealthBar() - ASCII progress bar visualization
//   ├── calculateNormalizedHealth() - Convert raw to percentage
//   └── updateHealth() - Apply delta and recalculate
//
//   context.go (System context capture)
//   ├── CaptureContext() - WHO, WHERE, WHEN orchestration
//   ├── captureShellContext() - Shell type and mode
//   ├── captureEnvState() - Environment variables
//   ├── captureSudoersContext() - Sudo configuration
//   ├── captureSystemMetrics() - CPU, memory, disk
//   └── 4 types (ShellContext, SudoersContext, SystemMetrics, SystemContext)
//
//   entry.go (Entry construction and formatting)
//   ├── createBaseEntry() - Common fields population
//   ├── formatEntry() - LogEntry → formatted string
//   ├── formatDeltaSign() - Health delta sign prefix
//   ├── formatUserIdentifier() - user@host:pid format
//   ├── writeField(), writeDetailValue() - Section writing
//   ├── writeMapSection(), writeListSection() - Collection formatting
//   └── 3 types (Interactions, LogEntry, Metadata)
//
//   writing.go (File writing and rotation)
//   ├── rotateLogIfNeeded() - Size-based rotation (.1→.2→.3→.4→.5)
//   └── writeEntry() - Atomic append with rotation check
//
//   parsing.go (Log file reading)
//   └── ReadLogFile() - Parse log entries back into structures
//
// Baton Flow (Execution Paths):
//
//   Logger Creation Flow:
//     NewLogger(component) [logger.go]
//       ↓
//     LoadConfig() [config.go - tripwire pattern]
//       ↓
//     determineLogSubdirectory(component) [logger.go - routing]
//       ↓
//     Return *Logger with routed log file path
//
//   Logging Operation Flow:
//     Operation/Success/Failure/Error/Check/Debug [logger.go - public APIs]
//       ↓
//     logEntry(level, event, healthImpact, details) [logger.go - orchestration]
//       ├─→ CaptureContext() [context.go - WHO/WHERE/WHEN]
//       ├─→ updateHealth(delta) [health.go - score calculation]
//       ├─→ createBaseEntry(context, healthImpact) [entry.go - structure building]
//       └─→ writeEntry(entry) [writing.go - disk persistence]
//             ├─→ rotateLogIfNeeded(logPath) [writing.go - rotation check]
//             └─→ formatEntry(entry) [entry.go - text formatting]
//       ↓
//     Entry written to routed log file
//
//   Command Orchestration Flow:
//     LogCommand(command, args) [logger.go - command wrapper]
//       ├─→ Operation(command) [log start event]
//       ├─→ exec.Command().Run() [actual execution]
//       └─→ Success() or Failure() [log result event]
//       ↓
//     Return command error
//
//   Log Reading Flow:
//     ReadLogFile(path) [parsing.go - standalone]
//       ↓
//     State machine parser (line-by-line)
//       ↓
//     Return []LogEntry structures
//
// API Surface:
//   - 7 files (logger.go + 6 extracted)
//   - 14 public APIs (exported from logger.go)
//   - 30+ internal functions (distributed across files)
//   - Rails pattern (stdlib-only except config.go TOML dependency)

// ────────────────────────────────────────────────────────────────
// Helpers/Utilities - Internal Support
// ────────────────────────────────────────────────────────────────
// Foundation functions used throughout this component. Bottom rungs of
// the ladder - simple, focused, reusable utilities. Not exported.

// determineLogSubdirectory routes component names to log subdirectories (commands/scripts/libraries/system).
func determineLogSubdirectory(component string) string {
	// Ensure config is loaded
	LoadConfig()

	// Commands go to commands/
	if slices.Contains(Config.Routing.Commands, component) { // Check config routing list
		return commandsSubdir                                // Use constant from SETUP
	}

	// Scripts go to scripts/
	if slices.Contains(Config.Routing.Scripts, component) { // Check config routing list
		return scriptsSubdir                                 // Use constant from SETUP
	}

	// Library components go to libraries/
	if slices.Contains(Config.Routing.Libraries, component) { // Check config routing list
		return librariesSubdir                                 // Use constant from SETUP
	}

	// Everything else goes to system/
	return systemLogsSubdir                                   // Use constant from SETUP (default routing)
}

// getCurrentUser and getHostname are defined in context.go (system context helpers)

// ────────────────────────────────────────────────────────────────
// Core Operations - Business Logic
// ────────────────────────────────────────────────────────────────
// Component-specific functionality implementing primary purpose. Organized
// by operational categories (descriptive subsections) below.

// ────────────────────────────────────────────────────────────────
// Entry Coordination - Internal Orchestration
// ────────────────────────────────────────────────────────────────
// Internal functions that coordinate core operations. These are used by public
// logging methods but not exported themselves. They orchestrate context capture,
// health updates, entry building, and log writing.

// logEntry is the base logging function that all public logging methods use.
//
// What It Does:
// Orchestrates the complete logging pipeline: captures system context, updates
// session health, builds log entry with all fields, determines context mode
// (full vs partial), and writes to log file.
//
// Parameters:
//   level: Log level (OPERATION, SUCCESS, FAILURE, ERROR, CHECK, CONTEXT, DEBUG)
//   event: Human-readable description of what happened
//   healthImpact: Health points to add/subtract from session health
//   details: Optional structured data (map) with additional context
//
// Used by: All core logging methods (Operation, Success, Failure, etc.)
func (l *Logger) logEntry(level string, event string, healthImpact int, details map[string]any) {
	context := l.CaptureContext()                       // Capture full system state
	l.updateHealth(healthImpact)                        // Update session health and normalization

	entry := l.createBaseEntry(context, healthImpact)   // Create entry with common fields
	entry.Level = level                                 // Set level from parameter
	entry.Event = event                                 // Set event description
	entry.Details = details                             // Set details (may be nil)

	// Set context mode based on configuration (multi-layer tripwire)
	var fullContext bool
	if ConfigLoaded && len(Config.Behavior.LogLevelFullContext) > 0 {
		fullContext = Config.Behavior.LogLevelFullContext[level] // Use config map
	} else {
		fullContext = logLevelFullContext[level] // Fallback to hardcoded map
	}

	if fullContext {                                    // Check configuration result
		entry.Context = context                         // Full context for this level
	} else {
		entry.Context = nil                             // Partial context (nil)
	}

	l.writeEntry(entry)                                 // Write to log file
}

// logEntryWithMetadata logs an entry with semantic metadata for restoration routing.
//
// What It Does:
// Extended logging pipeline that includes semantic metadata for future restoration
// routing. Adds structured error classification, recovery hints, and state contracts
// to enable automated or semi-automated fixes.
//
// Parameters:
//   level: Log level (OPERATION, SUCCESS, FAILURE, ERROR, CHECK, CONTEXT, DEBUG)
//   event: Human-readable description of what happened
//   healthImpact: Health points to add/subtract from session health
//   details: Optional structured data (map) with additional context
//   semantic: Metadata structure with operation classification, error details, recovery routing
//
// Used by: Metadata-enhanced logging methods (CheckWithMetadata, SuccessWithMetadata, FailureWithMetadata)
func (l *Logger) logEntryWithMetadata(level string, event string, healthImpact int, details map[string]any, semantic Metadata) {
	context := l.CaptureContext()                       // Capture full system state
	l.updateHealth(healthImpact)                        // Update session health and normalization

	entry := l.createBaseEntry(context, healthImpact)   // Create entry with common fields
	entry.Level = level                                 // Set level from parameter
	entry.Event = event                                 // Set event description
	entry.Details = details                             // Set details (may be nil)
	entry.Semantic = &semantic                          // Set semantic metadata (pointer for optional field)

	// Set context mode based on configuration (multi-layer tripwire)
	var fullContext bool
	if ConfigLoaded && len(Config.Behavior.LogLevelFullContext) > 0 {
		fullContext = Config.Behavior.LogLevelFullContext[level] // Use config map
	} else {
		fullContext = logLevelFullContext[level] // Fallback to hardcoded map
	}

	if fullContext {                                    // Check configuration result
		entry.Context = context                         // Full context for this level
	} else {
		entry.Context = nil                             // Partial context (nil)
	}

	l.writeEntry(entry)                                 // Write to log file (formatEntry outputs SEMANTIC section)
}

// ────────────────────────────────────────────────────────────────
// Core Logging Methods - Standard Operations
// ────────────────────────────────────────────────────────────────
// Business logic implementing the primary logging operations. These methods
// provide the core functionality for recording different types of events with
// appropriate context levels and health impacts.

// Operation logs operation start events with full system context.
//
// What It Does:
// Records the start of a major operation with full system context (WHO, WHERE,
// WHEN). Captures command with arguments for execution tracking.
//
// Parameters:
//   command: Operation or command name being started
//   healthImpact: Health points for starting this operation (typically +5 to +10)
//   args: Optional arguments to the command/operation
//
// Health Impact:
//   Configurable: Pass explicit health impact based on operation complexity
//
// Example usage:
//
//	logger.Operation("validate", +5, "config.toml")
//	logger.Operation("backup", +10)
//
func (l *Logger) Operation(command string, healthImpact int, args ...string) {
	// Build full command string using config format with fallback (multi-layer tripwire)
	fullCommand := command                                          // Default to command only
	if len(args) > 0 {                                              // Arguments provided
		if ConfigLoaded && Config.Messages.CmdFullFormat != "" {
			fullCommand = fmt.Sprintf(Config.Messages.CmdFullFormat, command, strings.Join(args, " "))
		} else {
			fullCommand = fmt.Sprintf(cmdFullFormat, command, strings.Join(args, " "))
		}
	}

	// Format event message using config with fallback (multi-layer tripwire)
	var eventMsg string
	if ConfigLoaded && Config.Messages.EventOpStart != "" {
		eventMsg = fmt.Sprintf(Config.Messages.EventOpStart, command)
	} else {
		eventMsg = fmt.Sprintf(eventOpStart, command)
	}

	l.logEntry(levelOperation, eventMsg, healthImpact, map[string]any{"command": fullCommand})
}

// Success logs successful completion events with partial context.
//
// What It Does:
// Records successful operation completion. Logs with partial context to keep
// success entries concise while capturing essential outcome details.
//
// Parameters:
//   event: Description of what succeeded
//   healthImpact: Health points gained (typically +10 to +30 for significant successes)
//   details: Optional structured data about the success
//
// Health Impact:
//   Configurable: Pass explicit positive health impact based on success significance
//
// Example usage:
//
//	logger.Success("Validation passed", +20, map[string]any{
//	    "files_checked": 15,
//	    "errors_found": 0,
//	})
//
func (l *Logger) Success(event string, healthImpact int, details map[string]any) {
	l.logEntry(levelSuccess, event, healthImpact, details)
}

// Failure logs expected failure events with full context.
//
// What It Does:
// Records expected failure with full system context for debugging. Captures
// failure reason and details for analysis and restoration routing.
//
// Parameters:
//   event: Description of what failed
//   reason: Why it failed (user-readable explanation)
//   healthImpact: Health points lost (typically -10 to -30 based on severity)
//   details: Optional structured data about the failure context
//
// Health Impact:
//   Configurable: Pass explicit negative health impact based on failure severity
//
// Example usage:
//
//	logger.Failure("Validation failed", "Invalid schema", -20, map[string]any{
//	    "file": "config.toml",
//	    "line": 42,
//	})
//
func (l *Logger) Failure(event string, reason string, healthImpact int, details map[string]any) {
	if details == nil {                                             // No details provided
		details = make(map[string]any)                              // Create empty map
	}
	details["reason"] = reason                                      // Add failure reason
	l.logEntry(levelFailure, event, healthImpact, details)
}

// Error logs unexpected error events with full context and stack trace.
//
// What It Does:
// Records unexpected errors with full context and stack trace for debugging.
// Captures complete error information for investigation of runtime failures.
//
// Parameters:
//   event: Description of what operation failed
//   err: The error that occurred
//   healthImpact: Health points lost (typically -20 to -50 for unexpected errors)
//
// Health Impact:
//   Configurable: Pass explicit negative health impact based on error severity
//
// Example usage:
//
//	if err := someOperation(); err != nil {
//	    logger.Error("Operation failed unexpectedly", err, -30)
//	}
//
func (l *Logger) Error(event string, err error, healthImpact int) {
	stackBuf := make([]byte, stackBufferSize)                      // Allocate stack buffer
	stackSize := runtime.Stack(stackBuf, false)                    // Capture stack trace
	l.logEntry(levelError, event, healthImpact,
		map[string]any{"error": err.Error(), "stack_trace": string(stackBuf[:stackSize])})
}

// Check logs validation/verification events with partial context.
//
// What It Does:
// Records validation or verification check results. Logs with partial context
// including the boolean result and any relevant details.
//
// Parameters:
//   what: Description of what was checked
//   result: Boolean result of the check (true = passed, false = failed)
//   healthImpact: Health points (+/- based on result and importance)
//   details: Optional structured data about the check
//
// Health Impact:
//   Configurable: Pass explicit health impact based on check importance
//
// Example usage:
//
//	logger.Check("File exists", fileExists, +5, map[string]any{
//	    "path": "/path/to/file",
//	})
//
func (l *Logger) Check(what string, result bool, healthImpact int, details map[string]any) {
	if details == nil {                                             // No details provided
		details = make(map[string]any)                              // Create empty map
	}
	details["result"] = result                                      // Add check result

	// Format event message using config with fallback (multi-layer tripwire)
	var eventMsg string
	if ConfigLoaded && Config.Messages.EventCheckMsg != "" {
		eventMsg = fmt.Sprintf(Config.Messages.EventCheckMsg, what)
	} else {
		eventMsg = fmt.Sprintf(eventCheckMsg, what)
	}

	l.logEntry(levelCheck, eventMsg, healthImpact, details)
}

// SnapshotState logs full system state snapshot events with full context.
//
// What It Does:
// Records a complete system state snapshot at a specific point in time. Captures
// full context for later comparison or debugging.
//
// Parameters:
//   label: Descriptive label for this snapshot (e.g., "before-operation", "after-rollback")
//   healthImpact: Health points (typically 0 for neutral observation)
//
// Health Impact:
//   Configurable: Usually 0 (neutral observation point)
//
// Example usage:
//
//	logger.SnapshotState("before-migration", 0)
//	// ... perform migration ...
//	logger.SnapshotState("after-migration", 0)
//
func (l *Logger) SnapshotState(label string, healthImpact int) {
	// Format event message using config with fallback (multi-layer tripwire)
	var eventMsg string
	if ConfigLoaded && Config.Messages.EventSnapshot != "" {
		eventMsg = fmt.Sprintf(Config.Messages.EventSnapshot, label)
	} else {
		eventMsg = fmt.Sprintf(eventSnapshot, label)
	}

	l.logEntry(levelContext, eventMsg, healthImpact, map[string]any{})
}

// Debug logs detailed execution trace events with full context.
//
// What It Does:
// Records detailed internal state for debugging purposes. Captures full context
// plus any internal state variables useful for tracing execution flow.
//
// Parameters:
//   event: Description of debug point
//   healthImpact: Health points (typically 0 for debug observations)
//   internalState: Map of internal variables and their values
//
// Health Impact:
//   Configurable: Usually 0 (debug observation)
//
// Example usage:
//
//	logger.Debug("Processing loop iteration", 0, map[string]any{
//	    "iteration": i,
//	    "remaining": len(items) - i,
//	    "current_item": items[i],
//	})
//
func (l *Logger) Debug(event string, healthImpact int, internalState map[string]any) {
	l.logEntry(levelDebug, event, healthImpact, internalState)
}

// ────────────────────────────────────────────────────────────────
// Metadata-Enhanced Logging Methods
// ────────────────────────────────────────────────────────────────
// Extended logging methods that include semantic metadata for future restoration
// routing. These enable automated or semi-automated recovery from failures.

// CheckWithMetadata logs validation/verification events with semantic metadata for restoration routing.
//
// What It Does:
// Extended Check method that includes semantic metadata for automated restoration.
// Records check result plus structured classification for future recovery routing.
//
// Parameters:
//   what: Description of what was checked
//   result: Boolean result of the check
//   healthImpact: Health points based on result
//   details: Optional structured data about the check
//   semantic: Metadata for restoration routing (operation type, error classification, recovery hints)
//
// Example usage:
//
//	logger.CheckWithMetadata("Config file valid", false, -10,
//	    map[string]any{"file": "config.toml"},
//	    logging.Metadata{
//	        OperationType: "file_validation",
//	        ErrorType: "schema_invalid",
//	        RecoveryHint: "automated_fix",
//	    })
//
func (l *Logger) CheckWithMetadata(what string, result bool, healthImpact int, details map[string]any, semantic Metadata) {
	if details == nil {                                             // No details provided
		details = make(map[string]any)                              // Create empty map
	}
	details["result"] = result                                      // Add check result

	// Format event message using config with fallback (multi-layer tripwire)
	var eventMsg string
	if ConfigLoaded && Config.Messages.EventCheckMsg != "" {
		eventMsg = fmt.Sprintf(Config.Messages.EventCheckMsg, what)
	} else {
		eventMsg = fmt.Sprintf(eventCheckMsg, what)
	}

	l.logEntryWithMetadata(levelCheck, eventMsg, healthImpact, details, semantic)
}

// SuccessWithMetadata logs successful completion events with semantic metadata for pattern learning.
//
// What It Does:
// Extended Success method that includes semantic metadata for pattern recognition.
// Records success plus structured information for learning successful patterns.
//
// Parameters:
//   event: Description of what succeeded
//   healthImpact: Health points gained
//   details: Optional structured data about the success
//   semantic: Metadata for pattern learning (operation classification, state contracts)
//
// Example usage:
//
//	logger.SuccessWithMetadata("Installation complete", +30,
//	    map[string]any{"packages": 15},
//	    logging.Metadata{
//	        OperationType: "package_installation",
//	    })
//
func (l *Logger) SuccessWithMetadata(event string, healthImpact int, details map[string]any, semantic Metadata) {
	l.logEntryWithMetadata(levelSuccess, event, healthImpact, details, semantic)
}

// FailureWithMetadata logs expected failure events with semantic metadata for restoration routing.
//
// What It Does:
// Extended Failure method that includes semantic metadata for automated restoration.
// Records failure plus structured classification and recovery routing information.
//
// Parameters:
//   event: Description of what failed
//   reason: Why it failed
//   healthImpact: Health points lost
//   details: Optional structured data about the failure
//   semantic: Metadata for restoration routing (error type, recovery strategy, recovery params)
//
// Example usage:
//
//	logger.FailureWithMetadata("Permission denied", "Insufficient permissions", -20,
//	    map[string]any{"file": "/etc/config"},
//	    logging.Metadata{
//	        ErrorType: "permission_denied",
//	        RecoveryStrategy: "fix_file_permissions",
//	        RecoveryParams: map[string]any{"target": "/etc/config", "mode": "0644"},
//	    })
//
func (l *Logger) FailureWithMetadata(event string, reason string, healthImpact int, details map[string]any, semantic Metadata) {
	if details == nil {                                             // No details provided
		details = make(map[string]any)                              // Create empty map
	}
	details["reason"] = reason                                      // Add failure reason
	l.logEntryWithMetadata(levelFailure, event, healthImpact, details, semantic)
}

// ────────────────────────────────────────────────────────────────
// Command Orchestration
// ────────────────────────────────────────────────────────────────
// High-level command execution with automatic lifecycle logging. Orchestrates
// complete command execution flow with start, execute, and result logging.

// LogCommand executes a command and automatically logs the full lifecycle (start, execute, result).
//
// What It Does:
// Orchestrates complete command execution with automatic logging: logs operation
// start, executes command, captures output/exit code/duration, logs success or
// failure based on exit code.
//
// Parameters:
//   command: Command to execute
//   args: Command arguments
//
// Returns:
//   error: Command execution error (nil if exit code 0)
//
// Health Impact:
//   Operation start: 0 points (neutral - just starting)
//   Success (exit 0): +10 points (default success impact)
//   Failure (non-zero exit): -10 points (default failure impact)
//
// Example usage:
//
//	err := logger.LogCommand("go", []string{"build", "./cmd/validate"})
//	if err != nil {
//	    // Command failed - already logged with full context
//	}
//
func (l *Logger) LogCommand(command string, args []string) error {
	// Log operation start using config health impact with fallback (multi-layer tripwire)
	var opImpact int
	if ConfigLoaded {
		opImpact = Config.HealthImpacts.CmdOperationImpact
	} else {
		opImpact = cmdOperationImpact
	}
	l.Operation(command, opImpact, args...)

	startTime := time.Now()							// Record start time

	// Execute command
	cmd := exec.Command(command, args...)			// Create command
	output, err := cmd.CombinedOutput()				// Execute and capture output

	duration := time.Since(startTime)				// Calculate duration
	exitCode := 0									// Default exit code (success)
	if err != nil {									// Command failed
		if exitErr, ok := err.(*exec.ExitError); ok {  // Get actual exit code
			exitCode = exitErr.ExitCode()			// Extract exit code from error
		}
	}

	// Format command string using config with fallback (multi-layer tripwire)
	var cmdString string
	if ConfigLoaded && Config.Messages.CmdFullFormat != "" {
		cmdString = fmt.Sprintf(Config.Messages.CmdFullFormat, command, strings.Join(args, " "))
	} else {
		cmdString = fmt.Sprintf(cmdFullFormat, command, strings.Join(args, " "))
	}

	// Log result with execution details
	details := map[string]any{
		"command":   cmdString,						// Formatted command
		"exit_code": exitCode,						// Command exit code
		"duration":  duration.String(),				// Execution duration
		"output":    string(output),				// Command output (stdout+stderr)
	}

	if exitCode == 0 {								// Success
		// Use config values with fallbacks (multi-layer tripwires)
		var successMsg string
		var successImpact int

		if ConfigLoaded && Config.Messages.EventCmdSuccess != "" {
			successMsg = fmt.Sprintf(Config.Messages.EventCmdSuccess, command)
		} else {
			successMsg = fmt.Sprintf(eventCmdSuccess, command)
		}

		if ConfigLoaded {
			successImpact = Config.HealthImpacts.CmdSuccessImpact
		} else {
			successImpact = cmdSuccessImpact
		}

		l.Success(successMsg, successImpact, details)
		return nil									// No error to return
	} else {										// Failure
		// Use config values with fallbacks (multi-layer tripwires)
		var failureMsg string
		var failureImpact int

		if ConfigLoaded && Config.Messages.EventCmdFailed != "" {
			failureMsg = fmt.Sprintf(Config.Messages.EventCmdFailed, command)
		} else {
			failureMsg = fmt.Sprintf(eventCmdFailed, command)
		}

		if ConfigLoaded {
			failureImpact = Config.HealthImpacts.CmdFailureImpact
		} else {
			failureImpact = cmdFailureImpact
		}

		l.Failure(failureMsg, fmt.Sprintf("exit code: %d", exitCode), failureImpact, details)
		return err									// Return command error
	}
}

// ────────────────────────────────────────────────────────────────
// Error Handling/Recovery Patterns
// ────────────────────────────────────────────────────────────────
// Centralized error management ensuring component handles failures gracefully.
// Provides safety boundaries and recovery strategies for robust operation.
//
// This component uses graceful degradation patterns:
// - File write failures: Warn to stderr, continue execution
// - Context capture failures: Use fallback defaults ("unknown" for username/hostname)
// - Configuration load failures: Use hardcoded default configuration
//
// The logging library must never block execution - all operations are non-blocking
// with clear fallback behaviors.

// ────────────────────────────────────────────────────────────────
// Public APIs - Exported Interface
// ────────────────────────────────────────────────────────────────
// Exported functions providing the logging library's public interface. These
// are the true orchestration layer - simple initialization and configuration
// functions that set up the logging system for use.

// NewLogger creates a logger instance with routed log file for the specified component.
//
// What It Does:
// Creates and initializes a Logger for the specified component with proper log file
// routing (commands/, scripts/, libraries/, or system/). Pre-computes correlation
// fields (username, hostname, PID) once for efficiency across all log entries.
//
// Parameters:
//   component: Component name for routing and log file naming
//
// Returns:
//   *Logger: Initialized logger ready for logging operations
//
// Health Impact:
//   Success: No health impact (initialization operation)
//
// Example usage:
//
//	logger := logging.NewLogger("validate")
//	logger.DeclareHealthTotal(100)
//	logger.Operation("Starting validation", +5)
//
func NewLogger(component string) *Logger {
	// Ensure config is loaded
	LoadConfig()

	home, _ := os.UserHomeDir() // Get user home directory

	// Determine subdirectory based on component type
	subdirectory := determineLogSubdirectory(component) // Route to appropriate subdirectory

	// Build log file path using config with fallback to constants (multi-layer tripwire)
	// Path: ~/.claude/[config.paths.base_dir or fallback]/logs/[subdirectory]/[component].log
	var logFile string
	if ConfigLoaded && Config.Paths.BaseDir != "" {
		// Use config base_dir + /logs
		logFile = filepath.Join(home, claudeBaseDir, Config.Paths.BaseDir, logsSubdir, subdirectory, component+logFileExtension)
	} else {
		// Fallback to hardcoded constants
		logFile = filepath.Join(home, claudeBaseDir, systemSubdir, logsSubdir, subdirectory, component+logFileExtension)
	}

	// Ensure logs directory exists
	logDir := filepath.Dir(logFile)					// Get directory path
	os.MkdirAll(logDir, logDirPermissions)			// Create with permissions from SETUP

	// Generate unique context ID using config format with fallback (multi-layer tripwire)
	var contextID string
	if ConfigLoaded && Config.Files.ContextIDFormat != "" {
		contextID = fmt.Sprintf(Config.Files.ContextIDFormat, component, os.Getpid(), time.Now().UnixNano())
	} else {
		contextID = fmt.Sprintf(contextIDFormat, component, os.Getpid(), time.Now().UnixNano())
	}

	// Pre-compute unchanging correlation points once at initialization
	// These values don't change during process lifetime - compute once, reuse everywhere
	username := getCurrentUser()					// Capture username once
	hostname := getHostname()						// Capture hostname once
	pid := os.Getpid()								// Capture PID once

	return &Logger{									// Return initialized logger
		Component:           component,					// Component name
		ContextID:           contextID,					// Unique execution identifier
		LogFile:             logFile,					// Routed log file path
		SessionHealth:       initialHealth,				// Use constant from SETUP
		TotalPossibleHealth: initialTotal,				// Use constant from SETUP
		NormalizedHealth:    initialNormalized,			// Use constant from SETUP
		username:            username,					// Pre-computed username (reused for every entry)
		hostname:            hostname,					// Pre-computed hostname (reused for every entry)
		pid:                 pid,						// Pre-computed PID (reused for every entry)
	}
}

// GetHealth returns the current normalized health percentage.
func (l *Logger) GetHealth() int {
	return l.NormalizedHealth                           // Return current health percentage
}

// DeclareHealthTotal declares the expected total health for perfect execution.
//
// What It Does:
// Sets the denominator for health normalization. When component declares that
// perfect execution would be 100 points, logger can calculate percentage health
// based on actual cumulative scores.
//
// Parameters:
//   total: Total health points for perfect execution (typically 100)
//
// Health Impact:
//   No health impact (configuration operation)
//
// Example usage:
//
//	logger := logging.NewLogger("validate")
//	logger.DeclareHealthTotal(100) // Perfect execution = 100 points
//	logger.Operation("Check file", +5)
//	logger.Success("File valid", +10, nil)
//
func (l *Logger) DeclareHealthTotal(total int) {
	l.TotalPossibleHealth = total                       // Set denominator for normalization calculation
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// ────────────────────────────────────────────────────────────────
// Code Validation: None (Library)
// ────────────────────────────────────────────────────────────────
//
// Testing Requirements:
//   - Import the library without errors: import "system/runtime/lib/logging"
//   - Create logger instance: logger := logging.NewLogger("component")
//   - Call each logging method with representative parameters
//   - Verify log files created in correct subdirectories
//   - Check log format matches documented standard (EVENT, DETAILS, CONTEXT sections)
//   - Ensure health tracking produces expected cumulative and normalized values
//   - Verify rotation creates .1, .2, .3, .4, .5 rotated files
//   - Confirm parsing can reconstruct LogEntry structures from log files
//   - Ensure no go vet warnings introduced
//   - Run: go test -v ./... (when tests exist)
//
// Build Verification:
//   - go build ./cpi-si/system/runtime/lib/logging (compiles without errors)
//   - go vet ./cpi-si/system/runtime/lib/logging (no warnings)
//   - Verify all 7 package files compile together cleanly
//
// Integration Testing:
//   - Test with actual commands (validate, test, status, diagnose)
//   - Verify health scoring accuracy across complete operation sequences
//   - Check log file growth and rotation under sustained logging
//   - Validate debugger can parse all log entries correctly
//   - Ensure component routing (commands/, scripts/, libraries/, system/) works
//
// Example validation code:
//
//	// Test basic logging functionality
//	logger := logging.NewLogger("test-component")
//	logger.DeclareHealthTotal(100)
//	logger.Operation("test operation", +10)
//	logger.Success("operation succeeded", +20, nil)
//
//	if logger.GetHealth() != 30 {
//	    t.Errorf("Expected health 30, got %d", logger.GetHealth())
//	}
//
// ────────────────────────────────────────────────────────────────
// Code Execution: None (Library)
// ────────────────────────────────────────────────────────────────
//
// This is a LIBRARY, not an executable. There is no entry point, no main function,
// no execution flow. All functions defined in BODY wait to be called by other components.
//
// Usage: import "system/runtime/lib/logging"
//
// The library is imported into the calling package, making all exported functions
// and types available. No code executes during import - functions are defined and ready to use.
//
// Example import and usage:
//
//	package main
//
//	import "system/runtime/lib/logging"
//
//	func main() {
//	    // Create logger with automatic routing
//	    logger := logging.NewLogger("my-command")
//	    logger.DeclareHealthTotal(100)
//
//	    // Log operation lifecycle
//	    logger.Operation("processing", +5)
//	    logger.Success("processing complete", +10, nil)
//
//	    // Check health
//	    health := logger.GetHealth()
//	    fmt.Printf("Final health: %d%%\n", health)
//	}
//
// ────────────────────────────────────────────────────────────────
// Code Cleanup: None (Library)
// ────────────────────────────────────────────────────────────────
//
// Resource Management:
//   - Log files: Auto-closed after each write (defer file.Close() pattern)
//   - File handles: No persistent handles maintained (open/write/close per entry)
//   - Memory: Go's garbage collector handles logger instances
//   - Rotation: Automatic when size exceeds maxLogSizeBytes (10 MB)
//
// Graceful Shutdown:
//   - N/A for libraries (no lifecycle)
//   - Calling code responsible for completing logging operations
//   - No cleanup function needed - all resources released per-operation
//
// Error State Cleanup:
//   - File write failures: Warn to stderr, continue execution (non-blocking)
//   - Context capture failures: Use fallback defaults ("unknown" for missing values)
//   - Configuration load failures: Use hardcoded default configuration
//   - All operations designed to never leave partial state
//
// Memory Management:
//   - Go's garbage collector handles memory
//   - Logger instances: ~200 bytes each (minimal overhead)
//   - Context captures: ~1-2 KB per capture (includes environment snapshot)
//   - Log entries: Written immediately, not buffered (no memory accumulation)
//
// No cleanup pattern needed (all resources auto-managed):
//
//	// Logger cleanup is automatic
//	logger := logging.NewLogger("component")
//	logger.Operation("work", +5)
//	// Logger released automatically when out of scope
//
// ════════════════════════════════════════════════════════════════
// FINAL DOCUMENTATION
// ════════════════════════════════════════════════════════════════
//
// ────────────────────────────────────────────────────────────────
// Library Overview & Integration Summary
// ────────────────────────────────────────────────────────────────
//
// Purpose: See METADATA "Purpose & Function" section above
//
// Provides: See METADATA "Key Features" list above for comprehensive capabilities
//
// Quick summary (high-level only - details in METADATA):
//   - Execution narrative with health scoring for all system components
//   - Detection layer in the logging → debugging → restoration immune system paradigm
//   - Captures WHO, WHEN, WHERE, WHAT with full system context
//   - Base100 health scoring algorithm with normalized percentages
//   - Automatic log rotation and file management
//   - Log file parsing for debugging analysis
//
// Integration Pattern: See METADATA "Usage & Integration" section above for
// complete step-by-step integration guide
//
// Public API: See METADATA "Public API" section above for complete
// public API list organized by category in typical usage order
//
// Architecture: Rails infrastructure - orthogonal to ladder, all components attach
// to logging independently. See METADATA "CPI-SI Identity" section for complete
// architectural role explanation.
//
// ────────────────────────────────────────────────────────────────
// Modification Policy
// ────────────────────────────────────────────────────────────────
//
// Safe to Modify (Extension Points):
//   ✅ Add new logging methods (follow Operation/Success/Failure pattern)
//   ✅ Add new context capture functions in context.go
//   ✅ Extend configuration types in config.go (add new TOML sections)
//   ✅ Add new health calculation algorithms in health.go
//   ✅ Add new log parsing modes in parsing.go
//   ✅ Add new entry formatting options in entry.go
//
// Modify with Extreme Care (Breaking Changes):
//   ⚠️ Logger struct fields - breaks code accessing fields directly
//   ⚠️ LogEntry struct fields - breaks parsing and debugging tools
//   ⚠️ Log file format - breaks all existing log parsing
//   ⚠️ Health scoring algorithm - changes all health calculations
//   ⚠️ Component routing logic - breaks log file organization
//   ⚠️ Public API function signatures - breaks all calling code
//
// NEVER Modify (Foundational Rails):
//   ❌ 4-block structure (METADATA, SETUP, BODY, CLOSING)
//   ❌ Rails pattern (package-level infrastructure, no parameter passing)
//   ❌ Graceful degradation (logging must never block execution)
//   ❌ Non-blocking design (all failures warn and continue)
//   ❌ Stdlib-only principle (except config.go TOML dependency)
//   ❌ Base100 health scoring foundation (normalization algorithm)
//
// Validation After Modifications:
//   See "Code Validation" section above for comprehensive testing requirements,
//   build verification, and integration testing procedures.
//
// ────────────────────────────────────────────────────────────────
// Ladder and Baton Flow
// ────────────────────────────────────────────────────────────────
//
// See BODY "Organizational Chart - Package Architecture" section above for
// complete ladder structure (dependencies) and baton flow (execution paths).
//
// The Organizational Chart in BODY provides the detailed map showing:
// - All 7 package files and their responsibilities
// - File coordination and dependencies
// - Complete execution flow from public APIs through core operations
//
// Quick architectural summary (details in BODY Organizational Chart):
// - 3 public APIs (NewLogger, GetHealth, DeclareHealthTotal) orchestrate 13 logging methods
// - 13 logging methods use entry coordination and file writing
// - Entry coordination captures context, builds entries, writes to disk
// - Supporting files: config (TOML loading), health (scoring), context (WHO/WHERE/WHEN),
//   entry (formatting), writing (persistence + rotation), parsing (reading)
//
// Ladder: Public APIs → Logging Methods → Entry Coordination → File Writing
// Baton: Create logger → Declare health total → Log operations → Write entries → Rotate files
//
// Rails Pattern: Each component creates own logger - never passed as parameter.
// Logging is orthogonal infrastructure (Rails), not a ladder dependency.
//
// ────────────────────────────────────────────────────────────────
// Surgical Update Points (Extension Guide)
// ────────────────────────────────────────────────────────────────
//
// See BODY "Core Operations" subsection header comments above for detailed
// extension points. Each subsection includes organization and pattern guidance.
//
// Quick reference (details in BODY subsections):
//
// Adding new logging methods:
//   - Location: BODY "Core Operations → Core Logging Methods" subsection
//   - Pattern: Follow Operation/Success/Failure docstring structure
//   - Template: Copy existing method, modify level and event format
//   - Integration: Add to appropriate log level constant group
//
// Adding new context capture:
//   - File: context.go (separate file for context operations)
//   - Pattern: Follow getCurrentUser/getHostname graceful fallback pattern
//   - Add to: SystemContext struct and captureSystemContext() function
//   - Test: Verify fallback defaults work when capture fails
//
// Adding new configuration options:
//   - File: config.go (configuration management)
//   - Pattern: Add field to appropriate Config type (11 sub-types available)
//   - Update: LoadConfig() to handle new field
//   - Fallback: Add default value to useDefaultConfig()
//
// Adding new health scoring:
//   - File: health.go (health calculation)
//   - Pattern: Follow clampHealth() helper function pattern
//   - Update: calculateNormalizedHealth() if algorithm changes
//   - Test: Verify -100 to +100 range maintained
//
// Adding new log parsing modes:
//   - File: parsing.go (log file reading)
//   - Pattern: Extend ReadLogFile() state machine
//   - Add: New section recognition (like EVENT, DETAILS, CONTEXT)
//   - Test: Verify round-trip (write → read → reconstruct)
//
// ────────────────────────────────────────────────────────────────
// Performance Considerations
// ────────────────────────────────────────────────────────────────
//
// See SETUP section above for performance characteristics:
// - Constants: Rotation thresholds (10 MB log files, 5 rotations max)
// - Types: Logger struct (~200 bytes), LogEntry struct (~1-2 KB with context)
//
// See BODY function docstrings above for operation-specific performance notes.
//
// Quick summary (details in SETUP/BODY above):
//
// Most expensive operation: Context capture with full environment snapshot
//   - Cost: ~1-2 KB memory per capture, ~100μs to collect
//   - Includes: User, hostname, PID, shell, CWD, environment, sudoers, system metrics
//   - Used by: OPERATION, FAILURE, ERROR, CONTEXT, DEBUG levels
//   - See: context.go captureSystemContext() for full implementation
//
// Memory characteristics:
//   - Logger instance: ~200 bytes (pre-computed correlation fields)
//   - Log entry with context: ~1-2 KB (full system snapshot)
//   - Log entry without context: ~200-500 bytes (SUCCESS, CHECK levels)
//   - No buffering: Entries written immediately to disk
//
// File I/O patterns:
//   - Write mode: Open, append, close per entry (no persistent handles)
//   - Rotation: Happens before write when size > 10 MB
//   - Directory creation: mkdir -p with proper permissions (0755)
//
// Key optimizations:
//   - Pre-compute correlation fields (username, hostname, PID) at logger creation
//   - Use sync.Once for config loading (load once, use many times)
//   - Partial context mode for SUCCESS/CHECK (skip full snapshot)
//   - Defer file.Close() ensures cleanup even on panic
//
// Scaling considerations:
//   - Log files: 10 MB max + 5 rotations = 60 MB max per component
//   - Concurrent loggers: Each component has own file (no contention)
//   - Disk I/O: ~1-5 entries/second typical, no buffering delay
//
// ────────────────────────────────────────────────────────────────
// Troubleshooting Guide
// ────────────────────────────────────────────────────────────────
//
// See BODY function docstrings above for operation-specific troubleshooting.
//
// Common issues and solutions:
//
// Problem: Log files not created
//   - Cause: Directory permissions or path doesn't exist
//   - Check: Verify ~/.claude/cpi-si/system/runtime/logs/ exists and is writable
//   - Solution: mkdir -p ~/.claude/cpi-si/system/runtime/logs/{commands,scripts,libraries,system}
//   - Note: Logger creates directories automatically, but parent must exist
//
// Problem: Health scores seem wrong
//   - Cause: TotalPossibleHealth not declared, defaults to 0
//   - Check: Call logger.DeclareHealthTotal(100) before logging operations
//   - Solution: Add DeclareHealthTotal() call after NewLogger()
//   - Expected: Without DeclareHealthTotal, normalized health = raw health (clamped to ±100)
//
// Problem: Context information shows "unknown" values
//   - Cause: System calls for username/hostname failed
//   - Expected: This is normal behavior - graceful degradation to "unknown"
//   - Note: Design decision - never block on context capture failures
//
// Problem: Log rotation not happening
//   - Cause: File size check happens before write, not continuously
//   - Check: Log file reaches 10 MB before rotation triggers
//   - Expected: Rotation happens on next write after 10 MB threshold
//   - Note: File may slightly exceed 10 MB (rotation on next write, not exact)
//
// Problem: Parsing fails on log files
//   - Cause: Log format changed or file corrupted
//   - Check: Verify log entries have [timestamp] header lines
//   - Solution: Ensure all writes use formatEntry() - don't write raw to log file
//   - Note: Parser expects specific format (header with |, EVENT:, DETAILS:, etc.)
//
// Problem: Compilation errors after modification
//   - Cause: Changed exported fields or function signatures
//   - Check: All calling code updated to match new signatures
//   - Solution: grep -r "logging\." cpi-si/ to find all usage sites
//   - Note: See "Modification Policy" above for safe vs breaking changes
//
// ────────────────────────────────────────────────────────────────
// Related Components & Dependencies
// ────────────────────────────────────────────────────────────────
//
// See METADATA "Dependencies" section above for complete dependency information:
// - Dependencies (What This Needs): Standard Library, TOML (config.go only)
// - Dependents (What Uses This): All commands, scripts, libraries, system components
// - Integration Points: Rails pattern - every component creates own logger
//
// Quick summary (details in METADATA Dependencies section above):
//
// Key dependencies:
//   - Standard library only (except config.go uses github.com/BurntSushi/toml)
//   - No internal system dependencies (Rails can't depend on ladder rungs)
//
// Primary consumers:
//   - Commands: validate, test, status, diagnose, debugger, unix-safe
//   - Scripts: build.sh (via Shell logging)
//   - Libraries: All future library components
//   - System: General system operations
//
// Parallel Implementation:
//   - Go version: cpi-si/system/runtime/lib/logging/ (this implementation)
//   - Shell version: cpi-si/system/runtime/lib/logging/logger.sh (parallel Rails for Shell)
//   - Shared philosophy: Base100 health scoring, graceful degradation, WHO/WHEN/WHERE/WHAT capture
//   - Shared format: Compatible log file format (Shell writes, Go parses)
//
// Integration with debugging layer:
//   - Logging provides Detection (this library)
//   - Debugging provides Assessment (system/runtime/lib/debugging - future)
//   - Restoration provides Response (automated fixes - future)
//   - See: logging-debugging-restoration.md for complete immune system paradigm
//
// ────────────────────────────────────────────────────────────────
// Future Expansions & Roadmap
// ────────────────────────────────────────────────────────────────
//
// Planned Features:
//   ✓ Multi-file package extraction (7 specialized files) - COMPLETED
//   ✓ Base100 health scoring algorithm - COMPLETED
//   ✓ Context capture with graceful fallbacks - COMPLETED
//   ✓ Log file rotation (10 MB, 5 rotations) - COMPLETED
//   ✓ Log file parsing for debugging - COMPLETED
//   ✓ Component routing (commands/, scripts/, libraries/, system/) - COMPLETED
//   ⏳ Configuration loading from logging.toml (Phase 7)
//   ⏳ Metadata-enhanced logging with restoration routing
//   ⏳ Interactions tracking (complexity scoring)
//   ⏳ Real-time log streaming (watch mode)
//   ⏳ Log compression for rotated files (.log.1.gz)
//   ⏳ Health trend analysis over time
//
// Research Areas:
//   - Structured logging with semantic search (query logs by operation type, error category)
//   - Performance profiling integration (correlate health with execution time)
//   - Distributed logging (correlate entries across multiple components)
//   - Anomaly detection (learn normal health patterns, flag deviations)
//   - Visual health dashboards (real-time component health monitoring)
//   - Machine learning for pattern recognition (predict failures before they occur)
//
// Integration Targets:
//   - Debugging library (Assessment layer - pattern recognition and routing)
//   - Restoration library (Response layer - automated fixes based on log analysis)
//   - Monitoring dashboard (web interface for health visualization)
//   - Alert system (notifications when health degrades)
//   - Performance profiler (correlate health with resource usage)
//   - CI/CD pipeline (health scoring in automated tests)
//
// Known Limitations to Address:
//   - Configuration loading not yet implemented (Phase 7) - currently uses hardcoded defaults
//   - Metadata and Interactions fields defined but not fully utilized yet
//   - No log compression (rotated files remain uncompressed)
//   - No real-time log tailing or streaming capability
//   - Context capture includes all environment variables (potential info disclosure)
//   - File I/O not buffered (each entry = one file open/write/close cycle)
//   - No log level filtering (all entries written regardless of level)
//   - Rotation happens on write boundary (file can slightly exceed 10 MB)
//   - No log cleanup (old rotations accumulate indefinitely)
//
// Version History:
//
// See METADATA "Authorship & Lineage" section above for brief version changelog.
// Comprehensive version history with full context:
//
//   1.0.0 (2025-11-18) - Multi-file package extraction and foundation refinement
//         - Extracted monolithic logger.go (1800+ lines) into 7 specialized files
//         - Created config.go, health.go, context.go, entry.go, writing.go, parsing.go
//         - Added comprehensive docstrings to all functions (template-compliant)
//         - Aligned BODY structure with template (5 sections, 4 Core Ops subsections)
//         - Moved all logging methods to Core Operations section
//         - Updated build.sh for workspace builds (go.work integration)
//         - Package total: 2,515 lines across 7 files
//         - Rails pattern: Package-level infrastructure, stdlib-only (except config TOML)
//
// ────────────────────────────────────────────────────────────────
// Closing Note
// ────────────────────────────────────────────────────────────────
//
// This library is RAILS infrastructure - orthogonal to the ladder, providing
// logging foundation for all system components. Every command, script, and library
// creates its own logger independently using the Rails pattern.
//
// Modify thoughtfully - changes here affect the entire system's ability to track
// health and execution narrative. The graceful degradation design is critical:
// logging must NEVER block execution, even if log files can't be written.
//
// Design guarantees that must be maintained:
// - Non-blocking: All operations continue even if logging fails
// - Graceful degradation: Fallback defaults for all context captures
// - Stdlib-only: Rails can't depend on ladder rungs (except config.go TOML)
// - Base100 health: Normalization algorithm keeps -100 to +100 range
//
// For questions, issues, or contributions:
//   - Review the modification policy above (safe vs breaking changes)
//   - Follow the 4-block structure pattern in all files
//   - Test thoroughly before committing (see Code Validation section)
//   - Document all changes comprehensively (What/Why/How pattern)
//   - Maintain Rails independence (no imports from ladder components)
//   - Preserve graceful degradation (never panic, always fallback)
//
// "Let all things be done decently and in order" - 1 Corinthians 14:40 (KJV)
//
// ────────────────────────────────────────────────────────────────
// Quick Reference: Usage Examples
// ────────────────────────────────────────────────────────────────
//
// Basic Setup:
//
//	package main
//
//	import "system/runtime/lib/logging"
//
//	func main() {
//	    // Create logger with automatic component routing
//	    logger := logging.NewLogger("my-command")
//	    logger.DeclareHealthTotal(100)  // Perfect execution = 100 points
//
//	    // Log operations
//	    logger.Operation("Starting work", +5)
//	    logger.Success("Work complete", +10, nil)
//	}
//
// Full Operation Lifecycle:
//
//	logger := logging.NewLogger("validate")
//	logger.DeclareHealthTotal(100)
//
//	// Start operation with full context
//	logger.Operation("validate", +5, "config.toml")
//
//	// Check validations
//	logger.Check("File exists", true, +10, nil)
//	logger.Check("Schema valid", true, +15, nil)
//
//	// Success with details
//	logger.Success("Validation passed", +20, map[string]any{
//	    "file": "config.toml",
//	    "checks": 2,
//	})
//
//	health := logger.GetHealth()  // Returns 50 (50% of 100)
//
// Error Handling Pattern:
//
//	logger := logging.NewLogger("processor")
//	logger.DeclareHealthTotal(100)
//
//	logger.Operation("process", +10)
//
//	if err := processFile("data.txt"); err != nil {
//	    // Unexpected error with stack trace
//	    logger.Error("Processing failed", err, -20)
//	    return
//	}
//
//	// Or expected failure with details
//	logger.Failure("process", "file not found", -15, map[string]any{
//	    "file": "data.txt",
//	})
//
// Command Execution (Automatic Lifecycle):
//
//	logger := logging.NewLogger("builder")
//	logger.DeclareHealthTotal(100)
//
//	// LogCommand automatically logs operation start, captures output, logs success/failure
//	err := logger.LogCommand("go", []string{"build", "./cmd/validate"})
//	if err != nil {
//	    // Already logged with full context (output, exit code, duration)
//	    return
//	}
//
//	// Command succeeded - automatically logged with +10 health
//
// System State Snapshot:
//
//	logger := logging.NewLogger("diagnose")
//
//	// Capture complete system state (WHO, WHERE, WHEN, resources)
//	logger.SnapshotState("before-operation", +5, map[string]any{
//	    "memory": getMemoryUsage(),
//	    "disk": getDiskUsage(),
//	})
//
//	performOperation()
//
//	logger.SnapshotState("after-operation", +5, map[string]any{
//	    "memory": getMemoryUsage(),
//	    "disk": getDiskUsage(),
//	})
//
// Debug Tracing:
//
//	logger := logging.NewLogger("complex-system")
//
//	logger.Debug("Entering critical section", 0, map[string]any{
//	    "state": getCurrentState(),
//	    "variables": dumpVariables(),
//	})
//
//	// Detailed execution trace with full context
//	performComplexOperation()
//
//	logger.Debug("Exiting critical section", 0, map[string]any{
//	    "result": getResult(),
//	})
//
// ============================================================================
// END CLOSING
// ============================================================================
