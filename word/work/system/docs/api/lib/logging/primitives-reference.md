# Logging Library - Primitives Quick Reference

**Complete reference for all logging primitives**

---

## Overview

The logging library uses a **switchboard orchestrator pattern** where `logger.go` coordinates 6 primitives that handle specific aspects of log management. Unlike display (direct primitives), logging exposes stateful operations through the Logger object.

**Pattern:** Switchboard - Logger object coordinates primitive operations
**Total Lines:** ~1,850 (6 primitives + orchestrator)

---

## Config (`config.go`)

**Configuration loading and validation**

### GetConfig()

**Load logging configuration with graceful degradation**

```go
func GetConfig() Config
```

**Returns:**
- `Config` struct with log file settings, format preferences, health thresholds
- Falls back to hardcoded defaults on load failure

**Config structure:**
```go
type Config struct {
    LogDirectory   string  // Where to write logs
    LogFilePrefix  string  // Filename prefix
    MaxFileSize    int64   // Rotation threshold (bytes)
    RetentionDays  int     // How long to keep logs
    IncludeContext bool    // Include command args in logs
    HealthThresholds struct {
        Excellent int  // +80 to +100
        Good      int  // +40 to +79
        Fair      int  // +1 to +39
        // ... etc
    }
}
```

**Why needed:**
- Centralized logging configuration
- Consistent log file locations
- Configurable health thresholds
- Graceful degradation if config fails

---

## Health (`health.go`)

**Health score calculations and normalization**

### CalculateNormalizedHealth()

**Normalize TRUE scores to -100 to +100 display scale**

```go
func CalculateNormalizedHealth(declaredTotal int, sessionHealth int) int
```

**Parameters:**
- `declaredTotal`: Expected health for perfect execution (e.g., 47, 131)
- `sessionHealth`: Actual accumulated health (sum of TRUE scores)

**Returns:**
- Normalized health score (-100 to +100)

**Algorithm:**
```
If sessionHealth >= 0:
    normalized = (sessionHealth / declaredTotal) * 100
    clamped to [0, 100]

If sessionHealth < 0:
    normalized = (sessionHealth / abs(declaredTotal)) * 100
    clamped to [-100, 0]
```

**Example:**
```go
// Perfect execution
normalized := CalculateNormalizedHealth(47, 47)  // Returns +100

// Partial failure
normalized := CalculateNormalizedHealth(131, 89)  // Returns +67

// Complete failure
normalized := CalculateNormalizedHealth(52, -87)  // Returns -100 (clamped)
```

---

### GetHealthIndicator()

**Visual health indicator for display**

```go
func GetHealthIndicator(normalizedHealth int) string
```

**Returns:**
- Emoji representing health level
- 💚 Excellent (+80 to +100)
- 💙 Good (+40 to +79)
- 💛 Fair (+1 to +39)
- ⚪ Neutral (0)
- 🧡 Concerning (-1 to -39)
- ❤️ Poor (-40 to -79)
- 💀 Critical (-80 to -100)

**Example:**
```go
health := 47
indicator := GetHealthIndicator(health)  // Returns "💙" (good)
```

---

## Context (`context.go`)

**Execution context capture and formatting**

### captureContext()

**Capture command arguments and environment**

```go
func captureContext() []string
```

**Returns:**
- Array of command-line arguments
- Empty if context capture disabled in config

**Used for:**
- Recording what command was run
- Debugging execution environment
- Reproducing issues

**Example log entry:**
```
[2025-11-21T16:43:05Z] instance/loading/loadRootConfig
  CONTEXT: ["/path/to/binary", "arg1", "arg2"]
  OPERATION: Load root instance config
```

---

### formatContext()

**Format context for log entry**

```go
func formatContext(args []string) string
```

**Returns:**
- Formatted context string for log file
- Empty if no context to format

---

## Entry (`entry.go`)

**Log entry construction and formatting**

### LogEntry

**Structure representing a single log entry**

```go
type LogEntry struct {
    Timestamp   time.Time
    Component   string
    Event       string
    Result      string // "OPERATION", "SUCCESS", "FAILURE"
    HealthImpact int
    Details     map[string]any
    Context     []string
}
```

---

### formatLogEntry()

**Format log entry for file writing**

```go
func formatLogEntry(entry LogEntry) string
```

**Returns:**
- Formatted multi-line log entry
- Includes timestamp, component, result, details

**Example output:**
```
[2025-11-21T16:43:05Z] instance/loading/loadRootConfig
  OPERATION: Load root instance config (0)
  CONTEXT: ["/usr/bin/session-start"]
```

```
[2025-11-21T16:43:06Z] instance/loading/loadRootConfig
  SUCCESS: Root config loaded successfully (+52)
  DETAILS: {
    "path": "/home/user/.claude/instance.jsonc",
    "config_type": "bootstrap pointer to full configs"
  }
```

---

## Writing (`writing.go`)

**File writing and log management**

### writeToLogFile()

**Write formatted entry to log file**

```go
func writeToLogFile(component string, formatted string) error
```

**What it does:**
- Creates log directory if needed
- Opens log file for component/date
- Appends formatted entry
- Handles file permissions

**Log file location:**
```
~/.claude/cpi-si/system/runtime/logs/{category}/{component}/YYYY-MM-DD.log
```

**Categories:**
- `commands/` - Command executables
- `libraries/` - Library components
- `scripts/` - Shell scripts
- `system/` - System-level operations

**Example paths:**
```
logs/libraries/instance/2025-11-21.log
logs/libraries/logging/2025-11-21.log
logs/libraries/display/2025-11-21.log
logs/commands/validate/2025-11-21.log
```

---

### ensureLogDirectory()

**Ensure log directory exists**

```go
func ensureLogDirectory(dirPath string) error
```

**Creates:**
- Full directory path
- Proper permissions (0755)
- Parent directories as needed

---

## Parsing (`parsing.go`)

**Log file parsing and entry extraction**

### parseLogFile()

**Parse log file into structured entries**

```go
func parseLogFile(filePath string) ([]LogEntry, error)
```

**Returns:**
- Array of parsed LogEntry structs
- Error if file doesn't exist or is malformed

**Used by:**
- Debugger (health assessment)
- Log analysis tools
- Health reporting

---

### parseLogEntry()

**Parse single log entry from text**

```go
func parseLogEntry(text string) LogEntry
```

**Handles:**
- Timestamp extraction
- Component identification
- Result parsing (OPERATION/SUCCESS/FAILURE)
- Health impact extraction
- Details JSON parsing

**Example:**
```go
text := `[2025-11-21T16:43:05Z] instance/loading/loadRootConfig
  SUCCESS: Root config loaded (+52)
  DETAILS: {"path": "/home/user/.claude/instance.jsonc"}`

entry := parseLogEntry(text)
// entry.HealthImpact = 52
// entry.Result = "SUCCESS"
```

---

## Orchestrator Integration

### How Primitives Work Together

**Logger.go (Switchboard) coordinates:**

1. **Config** - Loads settings at initialization
2. **Context** - Captures execution environment
3. **Entry** - Constructs structured log entries
4. **Health** - Calculates normalized scores
5. **Writing** - Persists entries to disk
6. **Parsing** - Reads entries for analysis

**Example flow:**
```
Logger.Operation()
    ↓
Context.captureContext()        // Get command args
Entry.createEntry()              // Build LogEntry struct
Entry.formatLogEntry()           // Format for file
Writing.writeToLogFile()         // Write to disk

Later:

Debugger needs analysis
    ↓
Parsing.parseLogFile()          // Read from disk
Entry.parseLogEntry()            // Parse each entry
Health.CalculateNormalizedHealth()  // Score health
Display results
```

---

## Switchboard vs Direct Primitives

### Why Switchboard for Logging?

**Logging = Stateful operations:**
```go
logger := logging.NewLogger("component")
logger.DeclareHealthTotal(100)           // Sets state
logger.Operation("task", 0)               // Uses state
logger.Success("done", 47, nil)           // Modifies state
```

**State exposed to caller:**
- SessionHealth accumulates
- DeclaredTotal remembered
- Operations tracked
- Caller sees stateful API

**Contrast with Display (Direct Primitives):**
```go
display.Success("message")  // Stateless - just format and return
```

---

## Common Patterns

### Basic Logging

```go
import "system/lib/logging"

logger := logging.NewLogger("mycomponent/operation")
logger.DeclareHealthTotal(47)
logger.Operation("Do important work", 0)

if err := doWork(); err != nil {
    logger.Failure("Work failed", err.Error(), -87, nil)
    return err
}

logger.Success("Work completed", 47, map[string]any{
    "items_processed": 142,
    "duration_ms": 523,
})
```

---

### Health Calculation

```go
import "system/lib/logging"

// After operations complete
normalized := logging.CalculateNormalizedHealth(
    declaredTotal,   // e.g., 100
    sessionHealth,   // e.g., 67
)

indicator := logging.GetHealthIndicator(normalized)
fmt.Printf("%s Health: %d\n", indicator, normalized)
// Output: 💙 Health: 67
```

---

### Log Parsing

```go
import "system/lib/logging"

entries, err := logging.ParseLogFile("path/to/log.log")
if err != nil {
    return err
}

for _, entry := range entries {
    fmt.Printf("%s: %s (%d)\n",
        entry.Component,
        entry.Event,
        entry.HealthImpact)
}
```

---

## Quick Import Reference

```go
import "system/lib/logging"

// Create logger (switchboard)
logger := logging.NewLogger("component/operation")
logger.DeclareHealthTotal(100)

// Log operations
logger.Operation("description", 0, "arg1", "arg2")
logger.Success("event", +47, details)
logger.Failure("event", "reason", -87, details)

// Health calculations
normalized := logging.CalculateNormalizedHealth(total, actual)
indicator := logging.GetHealthIndicator(normalized)

// Log parsing (for tools)
entries, _ := logging.ParseLogFile(filePath)
```

---

## Version History

| Version | Date | Changes |
|---------|------|---------|
| v1.0.0 | 2025-11-21 | Switchboard orchestrator with 6 primitives |
|  |  | config, health, context, entry, writing, parsing |
|  |  | Comprehensive logging infrastructure |

---

*For detailed Go API documentation, see [logger-api.md](logger-api.md). For Shell API, see [logger-sh-api.md](logger-sh-api.md). For complete usage guide, see [../API.md](../API.md).*
