# Inspector Library API Documentation

**Location:** `system/runtime/lib/debugging/inspector.go`

**Package:** `debugging`

**Version:** 1.0.0

**Created:** 2025-10-26

**Last Modified:** 2025-10-26

---

## Overview

State inspection infrastructure for CPI-SI runtime system. Provides deep execution visibility capturing HOW code executes (variable states, execution paths, performance metrics), complementing logging's WHAT happened narrative.

### Biblical Foundation

**Scripture:** Proverbs 25:2 - "It is the glory of God to conceal a matter; to search out a matter is the glory of kings"

**Principle:** Deep Understanding Through Careful Examination

**Anchor:** Proverbs 4:7 - "Wisdom is the principal thing; therefore get wisdom: and with all thy getting get understanding"

### Component Identity

**Type:** Rails - orthogonal infrastructure that components attach to directly

Each component creates its own inspector independently without passing through function calls or dependency injection. The debugging rail runs parallel to your code without interfering with it.

**Role:** State inspection infrastructure capturing execution details at critical moments

**Paradigm:** CPI-SI framework foundational component, parallel to logging rail

### Core Design

**Immune System - Assessment Support Layer:**

Detection (logging) captures events → Assessment (debugging) provides detailed state → Restoration uses both to determine appropriate response.

**Separation of Concerns:**
- **Logging Rail:** WHAT happened (narrative, health trajectory, story)
- **Debugging Rail:** HOW it happened (variable states, execution paths, performance)

Both rails are orthogonal - components attach to both independently without creating dependencies.

### Key Features

- **Dynamic Activation:** Enable/disable at runtime without recompilation
- **Non-blocking:** All operations fail gracefully, never interrupt execution
- **Output Separation:** Debug files stored separately from logs
- **Rail Correlation:** Shared context IDs enable cross-rail matching with logs
- **Pre-computed Values:** Username, hostname, PID captured once for fast inspection
- **State Capture:** Variable snapshots, expected vs actual comparisons
- **Execution Analysis:** Performance timing, call stacks, execution flow
- **System State:** Memory snapshots, full system context

### Philosophy

**Core Principle:** Logs tell the story, inspection shows the state. Narrative + Detail = Complete Understanding.

Debugging captures what logs cannot - the exact execution state at critical moments. When disabled, overhead is a single boolean check per call.

---

## Type Definitions

### Inspector State

#### Inspector

Manages inspection lifecycle and state for a single component.

**Fields:**
- `component` (string) - Component name for identification and correlation
- `enabled` (bool) - Whether inspection is active (false = no-ops, true = capture)
- `outputFile` (*os.File) - Debug file handle (nil when disabled)
- `contextID` (string) - Correlation ID shared with logger
- `startTime` (time.Time) - Inspector creation time for elapsed tracking
- `username` (string) - Pre-computed username (static per process)
- `hostname` (string) - Pre-computed hostname (static per process)
- `pid` (int) - Pre-computed process ID (static per process)
- `userHost` (string) - Pre-computed user@host:pid (static per process)
- `homeDir` (string) - Pre-computed home directory (static per process)
- `componentDir` (string) - Pre-computed debug directory path (static per process)
- `goVersion` (string) - Pre-computed Go runtime version (static per process)
- `numCPU` (int) - Pre-computed CPU count (static per process)

**Purpose:** Dynamic inspection control without recompiling. When disabled, inspection calls become instant no-ops (boolean check). When enabled, all output routes to same file with consistent correlation.

**Usage:**
```go
inspector := debugging.NewInspector("component-name", contextID)
inspector.Enable()
inspector.Snapshot("state-label", vars)
inspector.Disable()
```

---

### Inspection Entry

#### inspectionEntry (internal)

Internal container for one inspection moment.

**Fields:**
- `Timestamp` (time.Time) - Inspection moment
- `Type` (string) - Entry type constant (entrySnapshot, entryDivergence, etc.)
- `Label` (string) - User-provided inspection label
- `CallSite` (string) - Code location (file:line)
- `Data` (map[string]any) - Captured state

**Purpose:** Ensures every inspection entry has all required correlation points before writing. Internal use only.

#### InspectionEntry (exported)

Public version of inspectionEntry for reading debug files.

**Fields:**
- `Timestamp` (time.Time) - Inspection moment
- `Type` (string) - Entry type string ("SNAPSHOT", "DIVERGENCE", "TIMING", etc.)
- `Label` (string) - User-provided inspection label
- `CallSite` (string) - Code location (file:line)
- `Data` (map[string]any) - Captured state

**Purpose:** Returned by ReadDebugFile() for assessment tools to analyze debug output.

---

## Constants

### Entry Type Identifiers

**State Capture:**
- `entrySnapshot` - "SNAPSHOT" - Simple state capture at execution point
- `entryExpectedState` - "EXPECTED_STATE" - Expected vs actual state comparison matches
- `entryDivergence` - "DIVERGENCE" - State diverged from expected
- `entryConditional` - "CONDITIONAL" - Conditional state capture (when condition true)

**Execution Analysis:**
- `entryTiming` - "TIMING" - Performance timing within expected bounds
- `entrySlowTiming` - "SLOW_TIMING" - Performance exceeded expected duration
- `entryCounter` - "COUNTER" - Execution count matches expected
- `entryCountDivergence` - "COUNT_DIVERGENCE" - Count diverged from expected
- `entryCallStack` - "CALLSTACK" - Call stack trace capture
- `entryCheckpoint` - "CHECKPOINT" - Execution checkpoint marker
- `entryFlow` - "FLOW" - Execution flow matches expected path
- `entryUnexpectedFlow` - "UNEXPECTED_FLOW" - Flow diverged from expected path

**System State:**
- `entryMemory` - "MEMORY" - Memory state capture
- `entrySystemContext` - "SYSTEM_CONTEXT" - Full system state capture

### File Structure Configuration

**Directory Structure:**
- `claudeBaseDir` - ".claude" - Base Claude directory
- `systemSubdir` - "system" - System subdirectory
- `debugSubdir` - "debug" - Debug subdirectory (parallels logs/)
- `debugFileExtension` - ".debug" - Debug file extension
- `debugDirPermissions` - 0755 - Directory permissions
- `debugFilePermissions` - 0644 - File permissions

**Format Configuration:**
- `timestampFormat` - "2006-01-02 15:04:05.000" - Entry timestamp (millisecond precision)
- `timestampFormatHeader` - "2006-01-02 15:04:05" - Session header timestamp
- `entrySeparator` - "---\n" - Entry separator (matches logging)
- `contextIDFormat` - "%s-%d-%d" - Context ID format (component-pid-nanotime)

### System Context Configuration

**Environment Variables:**
- `envUser` - "USER" - Username environment variable
- `envHome` - "HOME" - Home directory
- `envShell` - "SHELL" - Shell binary path

**Proc Filesystem Paths (Linux):**
- `procLoadAvg` - "/proc/loadavg" - CPU load averages
- `procMemInfo` - "/proc/meminfo" - Memory information

**Memory Configuration:**
- `bytesToMbDivisor` - 1048576 (1024 * 1024) - Bytes to MB conversion
- `defaultCallStackDepth` - 10 - Default call stack capture depth

---

## API Reference

### Inspector Lifecycle

#### NewInspector(component string, contextID ...string) *Inspector

Creates a new inspector instance for a component.

**Parameters:**
- `component` - Component name for identification and debug file routing
- `contextID` (optional) - Correlation ID shared with logger for rail correlation. If omitted, generates own ID using component-pid-nanotime format.

**Returns:**
- `*Inspector` with pre-computed values, disabled by default (call Enable() to start)

**Examples:**
```go
// Create with generated contextID
inspector := debugging.NewInspector("validate")

// Create with logger's contextID for rail correlation
inspector := debugging.NewInspector("validate", logger.ContextID)
```

**Purpose:** Pre-computing unchanging values once (username, hostname, PID, paths, Go version, CPU count) makes every inspection call fast - no repeated environment lookups during execution.

---

#### (*Inspector).Enable() error

Activates debug output for this inspector.

**Returns:**
- `error` if directory creation or file opening fails, nil on success or if already enabled

**Behavior:**
- Creates debug directory at ~/.claude/system/debug/\<component\>/
- Opens output file with timestamp
- Writes session header with context information
- File remains open for append until Disable called

**Example:**
```go
if err := inspector.Enable(); err != nil {
    log.Printf("Warning: inspection disabled: %v", err)
}
// Continues even if enabling fails - inspection is non-blocking
```

**Purpose:** Separating creation from activation allows inspectors to exist without overhead - disabled inspection is single boolean check per method call.

---

#### (*Inspector).Disable()

Disables debug output and closes file.

**Behavior:**
- Closes output file
- Sets enabled flag to false
- Future inspection calls become instant no-ops

**Example:**
```go
defer inspector.Disable()  // Ensure cleanup
```

---

#### (*Inspector).IsEnabled() bool

Checks whether inspection is currently enabled.

**Returns:**
- `bool` - true if enabled, false if disabled

**Example:**
```go
if inspector.IsEnabled() {
    // Perform expensive state capture only when inspection active
}
```

---

#### (*Inspector).Close()

Closes debug output file and performs cleanup.

**Behavior:**
- Alias for Disable()
- Safe to call multiple times

**Example:**
```go
defer inspector.Close()
```

---

### State Capture Methods

#### (*Inspector).Snapshot(label string, vars map[string]any)

Captures current variable state at this execution point.

**Purpose:** Point-in-time variable values

**Parameters:**
- `label` - Human-readable identifier for this snapshot
- `vars` - Map of variable names to values

**Example:**
```go
inspector.Snapshot("after-init", map[string]any{
    "config_loaded": true,
    "retry_count": retries,
})
```

---

#### (*Inspector).ExpectedState(label string, expected, actual any, vars map[string]any)

Captures expected vs actual state comparison.

**Purpose:** Detect state divergence from expected values

**Behavior:**
- Writes `EXPECTED_STATE` entry when values match
- Writes `DIVERGENCE` entry when values don't match
- Adds `expected`, `actual`, and `matches` fields to vars automatically

**Parameters:**
- `label` - Human-readable identifier for this check
- `expected` - Expected value
- `actual` - Actual value observed
- `vars` - Optional additional context (can be nil)

**Example:**
```go
inspector.ExpectedState("cache-lookup", expectedValue, actualValue, map[string]any{
    "cache_key": key,
})
```

---

#### (*Inspector).ConditionalSnapshot(label string, condition bool, vars map[string]any)

Captures state only when condition is true.

**Purpose:** Capture state for rare conditions without overhead when false

**Behavior:**
- If condition is false: instant return (no capture)
- If condition is true: writes `CONDITIONAL` entry with state

**Parameters:**
- `label` - Human-readable identifier
- `condition` - Only capture when true
- `vars` - Variables to capture

**Example:**
```go
inspector.ConditionalSnapshot("rare-condition", retries > 10, map[string]any{
    "retry_count": retries,
    "last_error": err,
})
```

---

### Execution Analysis Methods

#### (*Inspector).Timing(label string, duration, expected time.Duration)

Captures performance metrics with expected vs actual duration comparison.

**Purpose:** Detect performance regressions and slow operations

**Behavior:**
- Writes `TIMING` entry when duration ≤ expected
- Writes `SLOW_TIMING` entry when duration > expected
- Captures duration, expected, variance, and within_expected flag

**Parameters:**
- `label` - Human-readable identifier for timed operation
- `duration` - Actual time.Duration the operation took
- `expected` - Expected time.Duration threshold

**Example:**
```go
start := time.Now()
result := complexOperation()
inspector.Timing("complex-op", time.Since(start), 100*time.Millisecond)
```

---

#### (*Inspector).Counter(label string, count, expected int)

Tracks execution counts with expected vs actual comparison.

**Purpose:** Detect execution count divergence - loops, retries, iterations

**Behavior:**
- Writes `COUNTER` entry when count == expected
- Writes `COUNT_DIVERGENCE` entry when count != expected
- Captures count, expected, variance, and matches flag

**Parameters:**
- `label` - Human-readable identifier for counted operation
- `count` - Actual execution count
- `expected` - Expected execution count

**Example:**
```go
retries := 0
for !success && retries < maxRetries {
    success = attemptOperation()
    retries++
}
inspector.Counter("operation-retries", retries, 3)
```

---

#### (*Inspector).CallStack(label string, depth int)

Captures the current call stack trace.

**Purpose:** Understand "how did we get here?" - trace execution path

**Behavior:**
- Walks stack frame by frame using runtime.Caller()
- Captures function name, file, and line for each frame
- If depth ≤ 0, uses defaultCallStackDepth (10)
- Formats as "fn@file:line <- caller <- ..." chain

**Parameters:**
- `label` - Human-readable identifier for this stack capture
- `depth` - How many stack frames to capture (0 uses default)

**Example:**
```go
inspector.CallStack("unexpected-nil", 10)  // Capture 10 frames
```

---

#### (*Inspector).Checkpoint(label string, vars map[string]any)

Marks an important execution waypoint with state capture.

**Purpose:** Create named waypoints for tracking execution progress

**Behavior:**
- Writes `CHECKPOINT` entry
- Acts as execution marker showing actual path taken
- Different from snapshots - marks significant progress points

**Parameters:**
- `label` - Waypoint identifier (e.g., "init-complete", "pre-commit")
- `vars` - State at this checkpoint (can be nil for simple markers)

**Example:**
```go
inspector.Checkpoint("pre-transaction", map[string]any{
    "pending_operations": len(ops),
    "transaction_id": txID,
})
```

---

#### (*Inspector).Flow(label string, branch string, expected ...string)

Tracks which execution path was taken with optional expected path comparison.

**Purpose:** Understand control flow decisions - which branch executed?

**Behavior:**
- Writes `FLOW` entry when no expected branch specified or branches match
- Writes `UNEXPECTED_FLOW` entry when execution diverged from expected path
- Captures branch_taken, and optionally expected_branch and matches_expected

**Parameters:**
- `label` - Human-readable identifier for this decision point
- `branch` - Which branch/path was actually taken
- `expected` - Optional variadic - if provided, which branch was expected

**Example:**
```go
if user.IsAdmin() {
    inspector.Flow("auth-check", "admin-path", "user-path")  // Expected user, got admin
    // ... admin logic
}
```

---

### System State Methods

#### (*Inspector).Memory(label string, vars map[string]any)

Captures current memory allocation state for leak detection.

**Purpose:** Track memory growth and detect leaks

**Behavior:**
- Uses runtime.MemStats to get current memory state
- Captures allocated memory, total allocations, system memory, GC runs, goroutines
- Merges with optional user-provided vars

**Parameters:**
- `label` - Human-readable identifier for this memory capture
- `vars` - Optional additional context (can be nil)

**Captured Metrics:**
- `alloc_mb` - Currently allocated memory in MB
- `total_alloc_mb` - Cumulative allocated memory in MB
- `sys_mb` - System memory obtained in MB
- `num_gc` - Number of GC cycles completed
- `goroutines` - Current goroutine count

**Example:**
```go
for i := 0; i < batchSize; i++ {
    processBatch(batches[i])
    if i % 100 == 0 {
        inspector.Memory(fmt.Sprintf("batch-%d", i), map[string]any{"batch_id": i})
    }
}
```

---

#### (*Inspector).SystemContext(label string)

Captures full system environment snapshot matching logging's context capture.

**Purpose:** Record complete system state for environment-dependent bugs

**Behavior:**
- Captures complete system context for correlation with logging rail
- Uses pre-computed values for unchanging data (username, home, Go version, CPU count)
- Gracefully handles failures (never crashes on context capture)

**Captured Context:**
- User context: username, shell, home directory
- Working directory: current working directory
- System load (Linux): 1, 5, 15 minute load averages from /proc/loadavg
- Memory info (Linux): MemTotal and MemAvailable from /proc/meminfo
- Disk usage: Free space on current volume
- Go runtime: Go version, OS, architecture, CPU count

**Parameters:**
- `label` - Human-readable identifier for this context snapshot

**Example:**
```go
inspector.SystemContext("pre-critical-operation")  // Capture environment before risky operation
```

---

### Usage Examples and Integration Patterns

#### Basic Setup

```go
inspector := debugging.NewInspector("my-component")
defer inspector.Close()
inspector.Enable()  // Start capturing
```

#### With Logger Correlation

```go
logger := logging.NewLogger("my-component")
inspector := debugging.NewInspector("my-component", logger.ContextID)
// Now both rails share contextID for correlation
```

#### State Capture Examples

```go
// Simple snapshot
inspector.Snapshot("after-init", map[string]any{
    "config_loaded": true,
    "retry_count": retries,
})

// Expected vs actual
inspector.ExpectedState("cache-lookup", expectedValue, actualValue, map[string]any{
    "cache_key": key,
})

// Conditional capture
inspector.ConditionalSnapshot("rare-condition", condition, vars)
```

#### Execution Analysis Examples

```go
// Performance timing
start := time.Now()
result := expensiveOperation()
inspector.Timing("expensive-op", time.Since(start), 100*time.Millisecond)

// Execution counting
inspector.Counter("retry-attempts", retries, maxRetries)

// Call stack capture
inspector.CallStack("error-path", 10)

// Execution checkpoints
inspector.Checkpoint("pre-transaction", map[string]any{"txid": id})

// Control flow tracking
inspector.Flow("auth-check", actualBranch, expectedBranch)
```

#### System State Examples

```go
// Memory tracking
inspector.Memory("batch-iteration-1000", map[string]any{"batch": i})

// Full system context
inspector.SystemContext("pre-critical-operation")
```

#### Dynamic Control

```go
inspector.Disable()  // Stop capturing (no-op overhead only)
inspector.Enable()   // Resume capturing
```

---

**Document Status:** Complete

**API Version:** 1.0.0

**Last Updated:** 2025-11-13
