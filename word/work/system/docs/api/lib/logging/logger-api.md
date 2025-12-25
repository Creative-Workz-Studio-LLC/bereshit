# Logger Library API Documentation

**Location:** `system/runtime/lib/logging/logger.go`

**Package:** `logging`

**Version:** 1.0.0

**Created:** 2025-10-25

**Last Modified:** 2025-10-25

---

## Overview

Comprehensive game development quality logging for CPI-SI runtime system. Provides complete execution narrative with health tracking, system context capture, and immune system detection layer.

### Biblical Foundation

**Scripture:** Malachi 3:16 - "A scroll of remembrance was written in his presence"

**Principle:** Faithful Witness Through Complete Remembrance

**Anchor:** Proverbs 14:5 - "A faithful witness does not lie"

### Component Identity

**Type:** Rails - orthogonal infrastructure that components attach to directly

Each component creates its own logger independently without passing through function calls or dependency injection. The logging rail runs parallel to your code without interfering with it.

**Role:** Logging infrastructure capturing complete execution narrative with health tracking

**Paradigm:** CPI-SI framework foundational component (no external dependencies, standard library only)

### Core Design

**Immune System - Detection Layer:**

Detection (logging) captures events → Assessment (debugging) provides detailed analysis → Restoration (fixing) uses both to determine appropriate response.

### Key Features

- **Health Scoring:** Every action gets a score showing impact (+points help, -points hurt)
- **TRUE SCORE:** Real impact values (like -73, +42, +8) create unique fingerprints for debugging
- **Full Context Capture:** WHO, WHEN, WHERE, WHAT, WHY, HOW, RESULT for every logged moment
- **Non-blocking:** All operations fail gracefully, never interrupt execution

### Philosophy

**Core Principle:** Logs are your time machine back to the unreproducible moment.

Every action has value, every moment recorded with complete truthful witness.

---

## Health Scoring System

### Base100 System

All actions in execution total 100 points.

- **Perfect execution** = +100
- **Complete failure** = -100
- **Real execution** = sum of actual results

### Normalization

- **Cumulative Health:** Raw sum of all action deltas executed so far
- **Total Possible Health:** Expected total declared at execution start
- **Normalized Health:** `(Cumulative / Total Possible) × 100` = percentage on -100 to +100 scale

### TRUE SCORE Philosophy

Each action assigned its actual impact value (17, -73, +6, etc.) instead of rounding to nice numbers.

Creates unique "fingerprints" for each type of failure - seeing -73 in logs tells you EXACTLY what failed without reading further.

**Examples:**
- Catastrophic: -100 (would brick sudo entirely - system unusable)
- Critical: -73 (bootloader corruption - requires immediate intervention)
- Severe: -48 (major service down - functionality lost)
- Moderate: -27 (recoverable critical error - degraded operation)
- Minor: -6 (cosmetic issue - annoying but functional)

**Example - Build script with 4 binaries (Total = 100):**
```
Setup: +10 (source logging +3, create bin +2, init context +5)
Per binary: +20 each (find source +4, compile +8, write +2, executable +1, etc.)
Verification: +10 (count +3, log +2, display +5)
Total: 10 + (4 × 20) + 10 = 100
```

---

## Type Definitions

### Context Building Blocks

#### ShellContext

Captures which shell is running and how it's running.

**Fields:**
- `Type` (string) - Shell program (bash, zsh, sh, etc.)
- `Interactive` (bool) - Terminal with prompts (true) vs script execution (false)
- `Login` (bool) - Full profile loaded (true) vs lightweight sub-shell (false)

**Methods:**
- `Format() string` - Converts to human-readable string for logs (e.g., "bash (interactive, login)")

**Purpose:** When debugging failures, shell configuration explains behavior. Interactive bash shows prompts, non-interactive runs scripts, login shells load different configuration.

**How Captured:** Type from $SHELL, Interactive from stdin terminal check, Login from $0 and $SHLVL.

**Usage:** Automatically captured by `CaptureContext()` - not created directly by users.

---

#### SudoersContext

Captures whether passwordless sudo is configured correctly.

**Fields:**
- `Installed` (bool) - File exists at `/etc/sudoers.d/90-cpi-si-safe-operations`
- `Valid` (bool) - Correct 0440 permissions
- `Permissions` (string) - Actual permissions (octal string like "0440")

**Methods:**
- `ToMap() map[string]string` - Converts to map for log display

**Purpose:** Many operations need passwordless sudo. Missing file or wrong permissions cause confusing permission errors. Capturing sudoers state shows immediately if sudo configuration caused failure.

**How Captured:** Direct filesystem check (no sudoers library dependency to avoid circular dependency).

**Usage:** Automatically captured by `CaptureContext()` - not created directly by users.

---

#### SystemMetrics

Captures how busy the computer is at this exact moment.

**Fields:**
- `Load` (string) - CPU load averages (1min, 5min, 15min from /proc/loadavg) (e.g., "0.5, 0.6, 0.7")
- `Memory` (string) - RAM usage (used/total MB from /proc/meminfo) (e.g., "2048MB / 8192MB")
- `Disk` (string) - Disk space (used/total with % from df command) (e.g., "50GB / 100GB (50%)")

**Methods:**
- `ToMap() map[string]string` - Converts to map for log display

**Purpose:** When operations fail or slow, need to know if computer itself was struggling. High CPU, low memory, or full disk show resource constraints.

**How Captured:** Reads from /proc filesystem and df command.

**Usage:** Automatically captured by `CaptureContext()` - not created directly by users.

---

### Composed Context

#### SystemContext

Complete system state combining all building blocks.

**Fields:**
- `User` (string) - Username ($USER environment variable)
- `Host` (string) - Computer hostname
- `PID` (int) - Process ID
- `Shell` (ShellContext) - Shell configuration
- `CWD` (string) - Current working directory
- `EnvState` (map[string]string) - Important environment variables (CPI_SI_*, DEBIAN_FRONTEND, etc.)
- `Sudoers` (SudoersContext) - Passwordless sudo configuration state
- `System` (SystemMetrics) - CPU/memory/disk resource snapshot

**Purpose:** Complete environment picture answering WHO (user, host, PID), WHERE (directory, shell), and WHY (environment, sudo, resources). Everything needed to recreate and understand any logged moment.

**How It Works:** Captures all pieces simultaneously for consistent snapshot. Everything frozen at exact logging moment.

**Usage:** Automatically built by `CaptureContext()` for log entries needing full context (OPERATION, FAILURE, ERROR, DEBUG, CONTEXT). Lightweight entries (CHECK, SUCCESS) skip to save space.

---

### Entry Components

#### Interactions

Captures what else was happening when this operation ran (optional complexity tracking).

**Fields:**
- `Concurrent` ([]string) - Operations running simultaneously (race condition tracking)
- `Dependencies` (map[string]string) - Requirements and provisions (dependency analysis)
- `StateChanges` (map[string]string) - Before/after values (mutation tracking)

**Purpose:** For complex scenarios beyond basic logging - race conditions, dependency failures, state corruption.

**Usage:** Most operations leave this nil. Use only when debugging complex scenarios requiring interaction tracking.

---

#### Metadata

Semantic information for restoration routing (Detection → Assessment → Restoration).

**Fields:**

*Operation Classification:*
- `OperationType` (string) - Primary category (file_validation, system_operation, etc.)
- `OperationSubtype` (string) - Granular sub-type (syntax_check, permission_check, etc.)

*Error Information (failures only):*
- `ErrorType` (string) - Error classification (permission_denied, file_not_found, etc.)
- `ErrorDetails` (map[string]any) - Structured error context

*Recovery Routing:*
- `RecoveryHint` (string) - Hint for restoration routing (automated_fix, manual_intervention, etc.)
- `RecoveryStrategy` (string) - Specific antibody to use (fix_file_permissions, install_package, etc.)
- `RecoveryParams` (map[string]any) - Parameters for antibody execution

*State Contracts (inspector usage):*
- `Expected` (map[string]any) - Expected state
- `Actual` (map[string]any) - Actual state observed

**Purpose:** Bridges detection (logging) and restoration (fixing). Enables debugger to route failures to appropriate restoration antibodies.

**Usage:** Use `WithMetadata` variants of logging methods when automated fixes are possible. Optional - backward compatible with standard logging.

---

### Complete Entry

#### LogEntry

One complete log entry - everything about one moment.

**Fields:**
- `Timestamp` (time.Time) - Exact moment (microsecond precision)
- `Level` (string) - Entry type (OPERATION, SUCCESS, FAILURE, ERROR, CHECK, CONTEXT, DEBUG)
- `Component` (string) - Logging component name (e.g., "validate", "build")
- `User` (string) - WHO identifier (user@host:pid format)
- `ContextID` (string) - Execution context ID (links related entries: component-pid-timestamp)
- `Context` (*SystemContext) - Full environment snapshot (nil for lightweight entries)
- `Event` (string) - Human description of occurrence
- `Details` (map[string]any) - Structured data (command, exit_code, duration, stdout, stderr)
- `Interactions` (*Interactions) - Optional complexity tracking
- `Semantic` (*Metadata) - Optional restoration routing metadata
- `RawHealth` (int) - Cumulative health (sum of all deltas)
- `NormalizedHealth` (int) - Health percentage (-100 to +100)
- `HealthImpact` (int) - This event's delta (Δ)

**Purpose:** Complete record answering WHO, WHEN, WHERE, WHAT, WHY, HOW, RESULT. Time machine to past moments enabling complete understanding and reproduction.

**Usage:** Created automatically by API functions (Operation, Success, Failure). Read when debugging to understand logged moments.

---

### Logger State

#### Logger

Manages all logging for one specific component.

**Fields:**
- `Component` (string) - Component name for identification and routing
- `ContextID` (string) - Unique execution context ID (component-pid-timestamp)
- `LogFile` (string) - Absolute log file path (routed by component type)
- `SessionHealth` (int) - Cumulative health (raw sum of deltas)
- `TotalPossibleHealth` (int) - Expected total for normalization
- `NormalizedHealth` (int) - Health percentage (-100 to +100)
- `username` (string) - Pre-computed username (captured once)
- `hostname` (string) - Pre-computed hostname (captured once)
- `pid` (int) - Pre-computed process ID (captured once)

**Purpose:** Control center for one component's logging. Tracks log file location, execution health, context ID, and pre-computed correlation points. Provides logging API methods.

**Usage:**
```go
logger := logging.NewLogger("component-name")
logger.DeclareHealthTotal(100)  // Optional - enables normalization
logger.Operation("task-name", +10, "arg1", "arg2")
logger.Success("Task completed", +15, details)
```

**State Isolation:** Each component gets isolated Logger - multiple can run concurrently without interfering.

---

### Health Visualization

#### HealthRange

One health range with its emoji and description.

**Fields:**
- `Threshold` (int) - Minimum health value for this range (e.g., 90 means "90 to 100")
- `Emoji` (string) - Visual indicator emoji (e.g., "💚" for excellent)
- `Description` (string) - Human-readable health state (e.g., "Excellent - all systems healthy")

**Purpose:** Data-driven health visualization. 21 ranges from -100 to +100 defined as table data (see `healthRanges` variable).

**Usage:** Not created directly - defined in `healthRanges` variable. Used automatically when displaying health scores.

---

## Constants

### Log Format Configuration

- `timestampFormat` - Standard log timestamp format with millisecond precision
- `contextHeader`, `eventHeader`, `detailsHeader`, `interactionsHeader` - Section headers
- `entrySeparator` - Entry separator line ("---")
- `logFilePermissions` - Log file permissions (0644)
- `warnLogOpenFailed`, `warnLogWriteFailed` - Warning message templates

### Context Capture Configuration

**File Paths:**
- `sudoersFilePath` - `/etc/sudoers.d/90-cpi-si-safe-operations`
- `sudoersValidPerms` - `0440`
- `procLoadAvgPath` - `/proc/loadavg`
- `procMeminfoPath` - `/proc/meminfo`

**Environment:**
- `frameworkEnvPrefix` - `"CPI_SI_"`
- `shellLvlEnvVar` - `"SHLVL"`
- `shellArgEnvVar` - `"0"`
- `loginShellPrefix` - `"-"`
- `loginShellLevel` - `"1"`

**Format Strings:**
- `permissionsFormat` - `"%04o"` (octal file permissions)
- `loadAvgFormat` - `"%s, %s, %s"` (CPU load averages)
- `memoryUsageFormat` - `"%dMB / %dMB"` (memory usage)
- `diskUsageFormat` - `"%s / %s (%s)"` (disk usage with percentage)

**Metrics:**
- `kbToMbDivisor` - `1024` (KB to MB conversion)
- `unknownValue` - `"unknown"` (graceful failure return)

**Commands:**
- `dfCommand` - `"df"` (disk free)
- `dfHumanFlag` - `"-h"` (human-readable flag)

### Logger Creation Configuration

**Directory Structure:**
- `claudeBaseDir` - `".claude"`
- `systemSubdir` - `"system"`
- `logsSubdir` - `"logs"`
- `logFileExtension` - `".log"`
- `logDirPermissions` - `0755`

**Subdirectories:**
- `commandsSubdir` - `"commands"` (command logs)
- `scriptsSubdir` - `"scripts"` (script logs)
- `librariesSubdir` - `"libraries"` (library logs)
- `systemLogsSubdir` - `"system"` (system/unknown logs)

**Routing:**
- `buildComponent` - `"build"` (build script component name)

**Log Rotation:**
- `maxLogSizeMB` - `10` (max size before rotating)
- `maxLogSizeBytes` - `10485760` (10MB in bytes)
- `maxLogRotations` - `5` (keep .1, .2, .3, .4, .5)
- `rotatedLogFormat` - `"%s.%d"` (path.1, path.2, etc.)

**Format Strings:**
- `contextIDFormat` - `"%s-%d-%d"` (component-pid-timestamp)

**Initial Values:**
- `initialHealth` - `0` (neutral start)
- `initialTotal` - `0` (unknown)
- `initialNormalized` - `0` (0%)

### Public API Configuration

**Log Levels:**
- `levelOperation` - `"OPERATION"` (operation start)
- `levelSuccess` - `"SUCCESS"` (successful completion)
- `levelFailure` - `"FAILURE"` (expected failure)
- `levelError` - `"ERROR"` (unexpected error)
- `levelCheck` - `"CHECK"` (validation/verification)
- `levelContext` - `"CONTEXT"` (system state snapshot)
- `levelDebug` - `"DEBUG"` (debug trace)

**Event Message Formats:**
- `eventOpStart` - `"Starting operation: %s"`
- `eventCheckMsg` - `"Checking: %s"`
- `eventSnapshot` - `"System state snapshot: %s"`
- `eventCmdFailed` - `"Command failed: %s"`
- `eventCmdSuccess` - `"Command completed: %s"`

**Stack Trace:**
- `stackBufferSize` - `4096` (buffer size for stack trace capture)

**LogCommand Health Impacts:**
- `cmdOperationImpact` - `0` (neutral for command start)
- `cmdFailureImpact` - `-10` (default failure impact)
- `cmdSuccessImpact` - `+10` (default success impact)

**Format Strings:**
- `cmdFullFormat` - `"%s %s"` (command with args)
- `durationFormat` - `"%dms"` (duration in milliseconds)

---

## Variables

### Log Level Behavior Configuration

**`logLevelFullContext` (map[string]bool):**

Controls which log levels get full SystemContext vs lightweight/partial context.

```go
{
    "OPERATION": true,   // Full context - operation start needs complete environment
    "SUCCESS":   false,  // Partial context - success is lightweight
    "FAILURE":   true,   // Full context - failures need debugging info
    "ERROR":     true,   // Full context - errors need complete state
    "CHECK":     false,  // Partial context - checks are lightweight
    "CONTEXT":   true,   // Full context - snapshots capture everything
    "DEBUG":     true,   // Full context - debug needs complete state
}
```

### Component Routing Lists

**`commandComponents` ([]string):**

Components that route to `commands/` subdirectory:
```go
{"validate", "test", "status", "diagnose"}
```

**`libraryComponents` ([]string):**

Components that route to `libraries/` subdirectory:
```go
{"operations", "sudoers", "environment", "display"}
```

**Routing Logic:**
- If component in `commandComponents` → route to `commands/`
- If component in `libraryComponents` → route to `libraries/`
- If component is `"build"` → route to `scripts/`
- Otherwise → route to `system/`

---

## API Reference

### Public API

#### Initialization Functions

##### NewLogger

Creates a logger instance with routed log file for the specified component.

```go
func NewLogger(component string) *Logger
```

**Parameters:**
- `component` (string) - Component name used for log file routing and identification

**Returns:**
- `*Logger` - Configured logger instance with automatic file routing

**Behavior:**
- Routes to `commands/` for: validate, test, status, diagnose, debugger, unix-safe, rails-demo
- Routes to `libraries/` for: operations, sudoers, environment, display, logging, debugging, calendar, config, jsonc, patterns, planner, privacy, sessiontime, temporal, validation
- Routes to `scripts/` for: build
- Routes to `system/` for: all other components

**Example:**
```go
logger := logging.NewLogger("validate")
logger.DeclareHealthTotal(100)
// Ready for logging operations
```

---

##### DeclareHealthTotal

Declares the expected total health for perfect execution (enables normalization).

```go
func (l *Logger) DeclareHealthTotal(total int)
```

**Parameters:**
- `total` (int) - Expected total health points for perfect execution

**Behavior:**
- Sets denominator for health percentage calculation
- Without this call, normalized health equals raw health (clamped to ±100)
- Call once at operation start before logging begins

**Example:**
```go
logger := logging.NewLogger("build")
logger.DeclareHealthTotal(100)  // Perfect execution = 100 points
```

---

##### GetHealth

Returns the current normalized health percentage.

```go
func (l *Logger) GetHealth() int
```

**Returns:**
- `int` - Normalized health percentage (-100 to +100)

**Example:**
```go
health := logger.GetHealth()
if health < 50 {
    fmt.Println("System health is degraded")
}
```

---

#### Core Logging Functions

##### Operation

Logs an operation start with command details.

```go
func (l *Logger) Operation(command string, healthImpact int, args ...string)
```

**Parameters:**
- `command` (string) - Operation/command name
- `healthImpact` (int) - Health delta for this operation start
- `args` (...string) - Optional command arguments

**Log Level:** OPERATION (full context captured)

**Example:**
```go
logger.Operation("compile", +5, "main.go", "-o", "binary")
```

---

##### Success

Logs successful completion of an operation.

```go
func (l *Logger) Success(event string, healthImpact int, details map[string]any)
```

**Parameters:**
- `event` (string) - Description of what succeeded
- `healthImpact` (int) - Positive health delta
- `details` (map[string]any) - Structured details (can be nil)

**Log Level:** SUCCESS (lightweight context)

**Example:**
```go
logger.Success("Build completed", +20, map[string]any{
    "duration": "1.2s",
    "output":   "/bin/validate",
})
```

---

##### Failure

Logs expected/handled failure with reason.

```go
func (l *Logger) Failure(event string, reason string, healthImpact int, details map[string]any)
```

**Parameters:**
- `event` (string) - Description of what failed
- `reason` (string) - Why it failed
- `healthImpact` (int) - Negative health delta
- `details` (map[string]any) - Structured details (can be nil)

**Log Level:** FAILURE (full context captured)

**Example:**
```go
logger.Failure("Compilation failed", "syntax error on line 42", -15, map[string]any{
    "file":     "main.go",
    "exit_code": 1,
})
```

---

##### Error

Logs unexpected error with stack trace.

```go
func (l *Logger) Error(event string, err error, healthImpact int)
```

**Parameters:**
- `event` (string) - Description of error context
- `err` (error) - The Go error object
- `healthImpact` (int) - Negative health delta

**Log Level:** ERROR (full context + stack trace)

**Example:**
```go
if err != nil {
    logger.Error("Failed to read config", err, -25)
}
```

---

#### Diagnostic Logging Functions

##### Check

Logs validation/verification check result.

```go
func (l *Logger) Check(what string, result bool, healthImpact int, details map[string]any)
```

**Parameters:**
- `what` (string) - What was checked
- `result` (bool) - Pass (true) or fail (false)
- `healthImpact` (int) - Health delta (typically positive for pass, negative for fail)
- `details` (map[string]any) - Structured details (can be nil)

**Log Level:** CHECK (lightweight context)

**Example:**
```go
logger.Check("sudoers-permissions", true, +10, map[string]any{
    "expected": "0440",
    "actual":   "0440",
})
```

---

##### SnapshotState

Logs system state snapshot at a point in time.

```go
func (l *Logger) SnapshotState(label string, healthImpact int)
```

**Parameters:**
- `label` (string) - Snapshot identifier/description
- `healthImpact` (int) - Health delta (typically 0 for neutral snapshots)

**Log Level:** CONTEXT (full context captured)

**Example:**
```go
logger.SnapshotState("before-risky-operation", 0)
// ... perform operation ...
logger.SnapshotState("after-risky-operation", 0)
```

---

##### Debug

Logs debug trace with internal state.

```go
func (l *Logger) Debug(event string, healthImpact int, internalState map[string]any)
```

**Parameters:**
- `event` (string) - Debug event description
- `healthImpact` (int) - Health delta (typically 0)
- `internalState` (map[string]any) - Internal state to capture

**Log Level:** DEBUG (full context captured)

**Example:**
```go
logger.Debug("Processing iteration", 0, map[string]any{
    "iteration": i,
    "buffer_size": len(buffer),
})
```

---

#### Metadata-Enhanced Functions

These variants include semantic metadata for restoration routing (Detection → Assessment → Restoration pipeline).

##### CheckWithMetadata

Check with semantic metadata for automated restoration routing.

```go
func (l *Logger) CheckWithMetadata(what string, result bool, healthImpact int, details map[string]any, semantic Metadata)
```

**Parameters:**
- Same as Check, plus:
- `semantic` (Metadata) - Restoration routing metadata

**Example:**
```go
logger.CheckWithMetadata("file-permissions", false, -10,
    map[string]any{"path": "/etc/config", "actual": "0777"},
    Metadata{
        OperationType:    "file_validation",
        OperationSubtype: "permission_check",
        ErrorType:        "permission_mismatch",
        RecoveryHint:     "automated_fix",
        RecoveryStrategy: "fix_file_permissions",
        RecoveryParams:   map[string]any{"target_perms": "0644"},
    })
```

---

##### SuccessWithMetadata

Success with semantic metadata for pattern tracking.

```go
func (l *Logger) SuccessWithMetadata(event string, healthImpact int, details map[string]any, semantic Metadata)
```

---

##### FailureWithMetadata

Failure with semantic metadata for automated restoration routing.

```go
func (l *Logger) FailureWithMetadata(event string, reason string, healthImpact int, details map[string]any, semantic Metadata)
```

---

#### Command Orchestration Functions

##### LogCommand

Executes command and logs complete lifecycle (start, output, success/failure).

```go
func (l *Logger) LogCommand(command string, args []string) error
```

**Parameters:**
- `command` (string) - Command to execute
- `args` ([]string) - Command arguments

**Returns:**
- `error` - nil on success, error on failure

**Behavior:**
1. Logs OPERATION at command start
2. Executes command via exec.Command
3. Captures stdout, stderr, exit code, duration
4. Logs SUCCESS or FAILURE based on exit code

**Example:**
```go
err := logger.LogCommand("go", []string{"build", "-o", "binary", "main.go"})
if err != nil {
    // Command failed - already logged
}
```

---

#### Package-Level Functions

##### LoadConfig

Ensures configuration is loaded from logging.toml (idempotent).

```go
func LoadConfig()
```

**Behavior:**
- Called automatically by NewLogger
- Safe to call multiple times (loads once)
- Sets ConfigLoaded flag and populates Config struct

---

##### ReadLogFile

Parses log file into LogEntry structures for analysis.

```go
func ReadLogFile(path string) ([]LogEntry, error)
```

**Parameters:**
- `path` (string) - Absolute path to log file

**Returns:**
- `[]LogEntry` - Slice of parsed log entries
- `error` - nil on success, error on parse failure

**Behavior:**
- State machine parser recognizes entry boundaries
- Extracts timestamp, level, component, context ID, health values
- Parses EVENT, DETAILS, CONTEXT, INTERACTIONS sections
- Returns partial data + error if parse fails mid-file

**Example:**
```go
entries, err := logging.ReadLogFile("/home/user/.claude/cpi-si/output/logs/commands/validate.log")
if err != nil {
    log.Printf("Parse warning: %v", err)
}
for _, entry := range entries {
    fmt.Printf("[%s] %s: %s (health: %d%%)\n",
        entry.Timestamp.Format("15:04:05"),
        entry.Level,
        entry.Event,
        entry.NormalizedHealth)
}
```

---

## Usage Examples

### Basic Command Logging

```go
package main

import "system/runtime/lib/logging"

func main() {
    // Initialize logger
    logger := logging.NewLogger("my-command")
    logger.DeclareHealthTotal(100)

    // Log operation start
    logger.Operation("process-files", +5, "input/", "output/")

    // Log checks
    logger.Check("input-exists", true, +10, nil)
    logger.Check("output-writable", true, +10, nil)

    // Log success
    logger.Success("Files processed", +75, map[string]any{
        "files_processed": 42,
        "duration":        "2.3s",
    })

    // Check final health
    health := logger.GetHealth()
    fmt.Printf("Final health: %d%%\n", health)
}
```

### Script with Health Scoring Map

```go
// HEALTH SCORING MAP (Total = 100):
// Setup: +10 (source +5, create-dir +3, init +2)
// Per file (3 files × 25 = 75): check +5, process +15, verify +5
// Verification: +15 (count +5, summary +10)

logger := logging.NewLogger("processor")
logger.DeclareHealthTotal(100)

// Setup phase
logger.Check("source-library", true, +5, nil)
logger.Check("create-output-dir", true, +3, nil)
logger.Operation("process-batch", +2)

// Process each file
for _, file := range files {
    logger.Check("file-exists-"+file, true, +5, nil)
    // ... process ...
    logger.Success("processed-"+file, +15, nil)
    logger.Check("verify-"+file, true, +5, nil)
}

// Verification
logger.Check("count-results", true, +5, nil)
logger.Success("Batch complete", +10, map[string]any{"total": len(files)})
```

### Error Handling Pattern

```go
logger := logging.NewLogger("risky-operation")
logger.DeclareHealthTotal(100)

logger.SnapshotState("before-operation", 0)

result, err := riskyFunction()
if err != nil {
    logger.Error("Operation failed unexpectedly", err, -50)
    logger.SnapshotState("after-failure", 0)
    return err
}

logger.Success("Operation completed", +50, map[string]any{"result": result})
```

---

## Best Practices

### Health Scoring

1. **Plan before coding** - Create HEALTH SCORING MAP in METADATA
2. **Total = 100** - All actions sum to 100 for consistent normalization
3. **TRUE SCORE** - Use actual impact values (-73, +17) not rounded numbers
4. **Balance positive/negative** - Success paths should reach +100, failure paths show magnitude

### Log Levels

| Level | When to Use | Context |
|-------|-------------|---------|
| OPERATION | Starting work | Full |
| SUCCESS | Completed successfully | Lightweight |
| FAILURE | Expected/handled failure | Full |
| ERROR | Unexpected exception | Full + stack |
| CHECK | Validation/verification | Lightweight |
| CONTEXT | State snapshot | Full |
| DEBUG | Development tracing | Full |

### Component Naming

- Use lowercase, hyphenated names: `my-command`, `file-processor`
- Match binary/script names for automatic routing
- Keep names stable - they're part of log file paths

### Performance

- Logger creation is lightweight - create per component
- Context capture happens only for full-context levels
- Rotation happens automatically at 10MB
- No blocking on write failures

---

## Configuration

The logging library loads configuration from `~/.claude/cpi-si/system/config/logging.toml`.

All configuration values have hardcoded fallbacks - the library works without config file.

### Key Configuration Sections

- `[paths]` - Log directory structure
- `[format]` - Timestamp and header formats
- `[behavior]` - Context capture policies
- `[messages]` - Event message templates
- `[health_impacts]` - Default health values
- `[routing]` - Component-to-subdirectory mapping
- `[rotation]` - File size and rotation limits
- `[health.ranges]` - Health visualization thresholds

---

**Document Status:** Complete

**Last Updated:** 2025-11-21

**Version:** 2.0.0 - Full API documentation with all 16 public methods
