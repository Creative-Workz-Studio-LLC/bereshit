# Logger Shell Library API Documentation

**Location:** `system/runtime/lib/logging/logger.sh`

**Language:** Bash

**Version:** 1.0.0

**Created:** 2025-10-25

**Last Updated:** 2025-11-21

---

## Overview

Comprehensive game development quality logging for bash scripts. Provides complete execution narrative with health tracking, system context capture, and immune system detection layer.

### Biblical Foundation

**Scripture:** Malachi 3:16 - "A scroll of remembrance was written in his presence"

**Principle:** Faithful Witness Through Complete Remembrance

**Anchor:** Proverbs 14:5 - "A faithful witness does not lie"

### Component Identity

**Type:** Rails - orthogonal infrastructure that components attach to directly

Each script sources this library and uses its functions directly. The logging rail runs parallel to your script without interfering with it.

**Role:** Logging infrastructure capturing complete execution narrative with health tracking

**Paradigm:** CPI-SI framework foundational component (bash built-ins only - no external dependencies)

### Core Design

**Immune System - Detection Layer:**

Detection (logging) captures events → Assessment (debugging) provides detailed analysis → Restoration (fixing) uses both to determine appropriate response.

### Key Features

- **Health Scoring:** Every action gets a score showing impact (+points help, -points hurt)
- **TRUE SCORE:** Real impact values (like -73, +42, +8) create unique fingerprints for debugging
- **Full Context Capture:** WHO, WHEN, WHERE, WHAT, WHY, HOW, RESULT for every logged moment
- **Non-blocking:** All operations fail gracefully, never interrupt execution

---

## Health Scoring System

### Base100 System

All actions in execution total 100 points.

- **Perfect execution** = +100
- **Complete failure** = -100
- **Real execution** = sum of actual results

### Normalization

- **Cumulative Health (SESSION_HEALTH):** Raw sum of all action deltas executed so far
- **Total Possible (TOTAL_POSSIBLE_HEALTH):** Expected total declared at execution start
- **Normalized Health:** `(Cumulative / Total Possible) × 100` = percentage on -100 to +100 scale

### TRUE SCORE Philosophy

Each action assigned its actual impact value (17, -73, +6, etc.) instead of rounding to nice numbers.

Creates unique "fingerprints" for each type of failure - seeing -73 in logs tells you EXACTLY what failed without reading further.

---

## Global Variables

### Session State

| Variable | Type | Default | Purpose |
|----------|------|---------|---------|
| `LOG_BASE_DIR` | string | `$HOME/.claude/cpi-si/output/logs` | Base directory for all logs |
| `CONTEXT_ID` | string | `bash-$$-$(date +%s%N)` | Unique execution context ID |
| `SESSION_HEALTH` | int | 0 | Raw cumulative health score |
| `TOTAL_POSSIBLE_HEALTH` | int | 0 | Expected total for normalization |
| `NORMALIZED_HEALTH` | int | 0 | Percentage (-100 to +100) |

---

## API Reference

### Initialization Functions

#### declare_health_total

Declares the expected total health for perfect execution (enables normalization).

```bash
declare_health_total <total>
```

**Parameters:**
- `total` (int) - Expected total health points for perfect execution

**Global Effects:**
- Sets `TOTAL_POSSIBLE_HEALTH` to provided value

**Example:**
```bash
source ~/.claude/cpi-si/system/runtime/lib/logging/logger.sh
declare_health_total 100  # Perfect execution = 100 points
```

---

### Core Logging Functions

#### log_operation

Logs an operation start with full context capture.

```bash
log_operation <component> <command> <health_impact> [args...]
```

**Parameters:**
- `component` (string) - Component name for routing and identification
- `command` (string) - Operation/command being executed
- `health_impact` (int) - Health delta for this operation start
- `args` (variadic) - Optional command arguments

**Log Level:** OPERATION (full context captured)

**Example:**
```bash
log_operation "validate" "syntax-check" 5 "--strict" "--all"
# Logs: [timestamp] OPERATION | validate | ...
#       EVENT: Starting operation: syntax-check
#       DETAILS: command: syntax-check --strict --all
```

---

#### log_success

Logs successful completion with partial context (lightweight).

```bash
log_success <component> <event> <health_impact> [key:value...]
```

**Parameters:**
- `component` (string) - Component name for routing
- `event` (string) - Description of what succeeded
- `health_impact` (int) - Positive health delta
- `key:value` (variadic) - Optional detail pairs

**Log Level:** SUCCESS (partial context - optimized for performance)

**Example:**
```bash
log_success "build" "Compilation completed" 20 "files: 42" "warnings: 0"
# Logs: [timestamp] SUCCESS | build | ...
#       EVENT: Compilation completed
#       DETAILS:
#           files: 42
#           warnings: 0
```

---

#### log_failure

Logs expected/handled failure with full context for debugging.

```bash
log_failure <component> <event> <reason> <health_impact> [key:value...]
```

**Parameters:**
- `component` (string) - Component name for routing
- `event` (string) - Description of what failed
- `reason` (string) - Why it failed
- `health_impact` (int) - Negative health delta
- `key:value` (variadic) - Optional detail pairs

**Log Level:** FAILURE (full context captured)

**Example:**
```bash
log_failure "build" "Compilation failed" "syntax error on line 42" -15 "file: main.go"
# Logs: [timestamp] FAILURE | build | ...
#       EVENT: Compilation failed
#       DETAILS:
#           reason: syntax error on line 42
#           file: main.go
```

---

#### log_error

Logs unexpected error with full context and automatic stack trace.

```bash
log_error <component> <event> <error> <health_impact> [key:value...]
```

**Parameters:**
- `component` (string) - Component name for routing
- `event` (string) - Description of error context
- `error` (string) - Error message
- `health_impact` (int) - Large negative health delta
- `key:value` (variadic) - Optional detail pairs

**Log Level:** ERROR (full context + stack trace)

**Example:**
```bash
log_error "processor" "Unexpected crash" "segfault in memory allocation" -50 "address: 0x7fff"
# Logs: [timestamp] ERROR | processor | ...
#       EVENT: Unexpected crash
#       DETAILS:
#           error: segfault in memory allocation
#           stack_trace: |
#             main in script.sh:42
#             process_file in script.sh:28
```

---

### Diagnostic Logging Functions

#### log_check

Logs validation/verification check result with partial context.

```bash
log_check <component> <what> <result> <health_impact> [key:value...]
```

**Parameters:**
- `component` (string) - Component name for routing
- `what` (string) - What was checked
- `result` (bool string) - "true" or "false"
- `health_impact` (int) - Health delta (positive for pass, negative for fail)
- `key:value` (variadic) - Optional detail pairs

**Log Level:** CHECK (partial context - optimized for frequent calls)

**Example:**
```bash
log_check "validate" "file-exists" "true" 10 "path: /etc/config"
# Logs: [timestamp] CHECK | validate | ...
#       EVENT: Checking: file-exists
#       DETAILS:
#           result: true
#           path: /etc/config
```

---

#### log_snapshot

Logs system state snapshot with full context for baseline/checkpoint.

```bash
log_snapshot <component> <label> <health_impact>
```

**Parameters:**
- `component` (string) - Component name for routing
- `label` (string) - Snapshot identifier/description
- `health_impact` (int) - Health delta (typically 0)

**Log Level:** CONTEXT (full context captured)

**Example:**
```bash
log_snapshot "risky-op" "before-modification" 0
# ... perform risky operation ...
log_snapshot "risky-op" "after-modification" 0
```

---

#### log_debug

Logs debug trace with full context and internal state.

```bash
log_debug <component> <event> <health_impact> [key:value...]
```

**Parameters:**
- `component` (string) - Component name for routing
- `event` (string) - Debug event description
- `health_impact` (int) - Health delta (typically 0)
- `key:value` (variadic) - Debug details

**Log Level:** DEBUG (full context captured)

**Example:**
```bash
log_debug "processor" "Loop iteration" 0 "iteration: 5" "buffer_size: 1024"
```

---

### Command Orchestration

#### log_command

Executes command and logs complete lifecycle (start, output, success/failure).

```bash
log_command <component> <description> <command> [args...]
```

**Parameters:**
- `component` (string) - Component name for routing
- `description` (string) - Human-readable operation description
- `command` (string) - Command to execute
- `args` (variadic) - Command arguments

**Returns:**
- Exit code of executed command (0 = success, non-zero = failure)

**Behavior:**
1. Logs OPERATION at command start (full context, neutral health)
2. Executes command, capturing stdout+stderr
3. Calculates duration in milliseconds
4. Logs SUCCESS (+10) or FAILURE (-10) based on exit code
5. Returns original exit code for script flow control

**Example:**
```bash
if log_command "builder" "Compile main binary" go build -o bin/main main.go; then
    echo "Build succeeded"
else
    echo "Build failed"
fi
```

---

## Internal Functions

These functions support the public API but are not intended for direct use.

| Function | Purpose |
|----------|---------|
| `determine_log_subdirectory` | Routes component to correct log subdirectory |
| `get_log_file_path` | Builds full path to component's log file |
| `calculate_normalized_health` | Converts raw health to percentage |
| `capture_shell_context` | Captures shell type, interactive, login status |
| `capture_environment` | Captures CPI_SI_* environment variables |
| `capture_sudoers_state` | Checks passwordless sudo configuration |
| `capture_system_metrics` | Gets CPU load, memory, disk usage |
| `capture_full_context` | Orchestrates all context capture |
| `format_log_header` | Builds log entry header line |
| `write_log_entry` | Writes formatted entry to log file |
| `log_generic` | Core logging function all public functions delegate to |
| `print_details_list` | Formats key:value pairs as indented list |

---

## Component Routing

Components are automatically routed to appropriate log subdirectories:

| Category | Components | Directory |
|----------|------------|-----------|
| Commands | validate, test, status, diagnose, debugger, unix-safe, rails-demo | `commands/` |
| Scripts | build | `scripts/` |
| Libraries | operations, sudoers, environment, display, logging, debugging, calendar, config, jsonc, patterns, planner, privacy, sessiontime, temporal, validation | `libraries/` |
| System | (all others) | `system/` |

**Log Path Format:** `~/.claude/cpi-si/output/logs/<category>/<component>.log`

---

## Usage Examples

### Basic Script Logging

```bash
#!/bin/bash
source ~/.claude/cpi-si/system/runtime/lib/logging/logger.sh
declare_health_total 100

log_operation "my-script" "process-files" 5 "input/" "output/"

log_check "my-script" "input-exists" "true" 10
log_check "my-script" "output-writable" "true" 10

# ... do work ...

log_success "my-script" "Files processed" 75 "files: 42" "duration: 2.3s"

echo "Final health: $NORMALIZED_HEALTH%"
```

### Script with Health Scoring Map

```bash
#!/bin/bash
# HEALTH SCORING MAP (Total = 100):
# Setup: +10 (source +5, create-dir +3, init +2)
# Per file (3 files × 25 = 75): check +5, process +15, verify +5
# Verification: +15 (count +5, summary +10)

source ~/.claude/cpi-si/system/runtime/lib/logging/logger.sh
declare_health_total 100

# Setup
log_check "processor" "source-library" "true" 5
log_check "processor" "create-output-dir" "true" 3
log_operation "processor" "process-batch" 2

# Process files
for file in file1 file2 file3; do
    log_check "processor" "file-exists-$file" "true" 5
    # ... process ...
    log_success "processor" "processed-$file" 15
    log_check "processor" "verify-$file" "true" 5
done

# Verification
log_check "processor" "count-results" "true" 5
log_success "processor" "Batch complete" 10 "total: 3"
```

### Error Handling Pattern

```bash
#!/bin/bash
source ~/.claude/cpi-si/system/runtime/lib/logging/logger.sh
declare_health_total 100

log_snapshot "risky-op" "before-operation" 0

if ! some_risky_command; then
    log_error "risky-op" "Operation failed" "$?" -50
    log_snapshot "risky-op" "after-failure" 0
    exit 1
fi

log_success "risky-op" "Operation completed" 50
```

### Using log_command for External Commands

```bash
#!/bin/bash
source ~/.claude/cpi-si/system/runtime/lib/logging/logger.sh
declare_health_total 100

log_operation "builder" "build-all" 10

if log_command "builder" "Compile binary" go build -o bin/app main.go; then
    log_success "builder" "Build completed" 40
else
    log_failure "builder" "Build failed" "compilation error" -40
    exit 1
fi

if log_command "builder" "Run tests" go test ./...; then
    log_success "builder" "Tests passed" 40
fi
```

---

## Log Entry Format

Each log entry follows this structure:

```
[timestamp] LEVEL | component | user@host:pid | context-id | HEALTH: X% (raw: Y, ΔZ)
  CONTEXT:
    shell: bash (interactive, login)
    cwd: /path/to/working/directory
    env: CPI_SI_VAR=value
    sudoers: installed=true, valid=true, perms=0440
    system: load=0.5,0.6,0.7 mem=2048MB/8192MB disk=50GB/100GB(50%)
  EVENT: Description of what happened
  DETAILS:
    key: value
    another_key: another_value
---
```

**Context capture rules:**
- OPERATION, FAILURE, ERROR, DEBUG, CONTEXT: Full context
- SUCCESS, CHECK: Partial context (performance optimization)

---

## Best Practices

### Health Scoring

1. **Plan before coding** - Create HEALTH SCORING MAP in script header
2. **Total = 100** - All actions sum to 100 for consistent normalization
3. **TRUE SCORE** - Use actual impact values (-73, +17) not rounded numbers
4. **Balance positive/negative** - Success paths should reach +100

### Log Levels

| Level | When to Use | Context |
|-------|-------------|---------|
| OPERATION | Starting work | Full |
| SUCCESS | Completed successfully | Partial |
| FAILURE | Expected/handled failure | Full |
| ERROR | Unexpected exception | Full + stack |
| CHECK | Validation/verification | Partial |
| CONTEXT | State snapshot | Full |
| DEBUG | Development tracing | Full |

### Component Naming

- Use lowercase, hyphenated names: `my-script`, `file-processor`
- Match script names for automatic routing
- Keep names stable - they're part of log file paths

### Performance

- Use partial context functions (SUCCESS, CHECK) for frequent operations
- Full context captures are expensive - reserve for important moments
- log_command handles lifecycle automatically - use for external commands

---

## Troubleshooting

### No log files created

- **Check:** Does `~/.claude/cpi-si/output/logs/` exist?
- **Check:** Do subdirectories (commands/, scripts/, etc.) exist?
- **Solution:** `mkdir -p ~/.claude/cpi-si/output/logs/{commands,scripts,libraries,system}`

### Logs in wrong subdirectory

- **Check:** Component name routing in `determine_log_subdirectory()`
- **Solution:** Add your component to appropriate case statement

### Health showing 0% despite logging

- **Check:** Did you call `declare_health_total`?
- **Solution:** Add `declare_health_total 100` after sourcing library

### Context not appearing in logs

- **Check:** Are you using a partial-context function (SUCCESS, CHECK)?
- **Solution:** Use OPERATION, FAILURE, or SNAPSHOT for full context

---

## Comparison with Go Implementation

| Feature | Bash (logger.sh) | Go (logging package) |
|---------|------------------|----------------------|
| Initialization | `source` + `declare_health_total` | `NewLogger()` + `DeclareHealthTotal()` |
| Health access | `$NORMALIZED_HEALTH` variable | `GetHealth()` method |
| Details format | `"key: value"` string pairs | `map[string]any` |
| Command execution | `log_command` function | `LogCommand()` method |
| Metadata support | Not available | `*WithMetadata()` variants |
| Configuration | Hardcoded paths | TOML config with fallbacks |

Both implementations write to the same log format and directory structure, enabling cross-language log analysis with the `debugger` command.

---

**Document Status:** Complete

**Version:** 1.0.0
