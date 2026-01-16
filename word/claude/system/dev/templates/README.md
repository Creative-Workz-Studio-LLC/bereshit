# CPI-SI Development Templates

**Purpose:** Define structured formats for logging and debugging to avoid hardcoding in runtime code

**Location:** `~/.claude/cpi-si/system/dev/templates/`

---

## Why Templates?

### The Problem

**Before templates:**
- Log format hardcoded in Go runtime code
- Debug format hardcoded in Go runtime code
- Changing format requires code changes, recompilation, redeployment
- Format inconsistencies across components
- Difficult to extend or modify output structure

**Example of hardcoded approach:**
```go
// BAD: Format hardcoded in code
log.Printf("[%s] %s | %s | %s@%s:%d | %s | HEALTH: %d%% ...",
    timestamp, level, component, user, host, pid, contextID, health)
```

### The Solution

**With templates:**
- Format defined in JSONC configuration files
- Runtime reads template and applies it
- Format changes = edit template file, no code changes
- Consistent structure across all components
- Easy to extend with new fields or sections
- Separation of concerns: runtime = logic, templates = presentation

**Example of template-based approach:**
```go
// GOOD: Runtime reads format from template
template := LoadLogTemplate("system/dev/templates/logging/log-format.jsonc")
log := template.Format(entry)
```

---

## Template Structure

```
templates/
├── README.md                           # This file
├── logging/
│   └── log-format.jsonc                # Structured logging format
└── debugging/
    └── debug-format.jsonc              # Structured debugging format
```

---

## Logging Template

**File:** `logging/log-format.jsonc`

**Purpose:** Define structured log entry format for all CPI-SI components

**Key Features:**

| Feature | Description |
|---------|-------------|
| **Entry Structure** | `[timestamp] LEVEL \| component \| identity \| context_id \| health \| content` |
| **Log Levels** | CHECK, OPERATION, CONTEXT, EVENT, ERROR, WARNING, INFO, DEBUG |
| **Health Integration** | Health score with delta, emoji, and visual progress bar |
| **Structured Content** | EVENT and DETAILS sections with proper indentation |
| **Extensible** | Add new levels or fields by updating template only |

**Current Log Levels:**

- **CHECK** - Validation/verification operations
- **OPERATION** - Action being performed
- **CONTEXT** - Context establishment
- **EVENT** - Significant event occurred
- **ERROR** - Error condition
- **WARNING** - Warning condition
- **INFO** - Informational message
- **DEBUG** - Debug-level detail

**Health Emoji Mapping:**

| Score Range | Emoji | Status |
|-------------|-------|--------|
| 90-100 | 💚 | Excellent |
| 70-89 | 💛 | Good |
| 50-69 | 🧡 | Moderate |
| 30-49 | ❤️ | Poor |
| 10-29 | 🩸 | Critical |
| 0-9 | ☠️ | Failed |

**Example Log Entry:**
```
[2025-11-09 06:40:21.190] CHECK | status | seanje-lenox-wise@creativeworkz-nova-dawn:6573 | status-6573-1761710846178910692 | HEALTH: 5% (raw: 10, Δ+10) ☠️ [██████████░░░░░░░░░░]
  EVENT: Checking: logger-initialized
  DETAILS:
    component: status
    result: true
```

---

## Debugging Template

**File:** `debugging/debug-format.jsonc`

**Purpose:** Define structured debug session output for all CPI-SI components

**Key Features:**

| Feature | Description |
|---------|-------------|
| **Session Structure** | Header → Context → Initial State → Events → Final State → Analysis → Footer |
| **Debug Levels** | BASIC (states only), DETAILED (+ events), VERBOSE (+ analysis) |
| **Context Capture** | Complete execution context for reproducibility |
| **Event Timeline** | Chronological event log with state changes |
| **State Comparison** | Before/after snapshots for debugging |

**Debug Levels:**

| Level | Sections | Use Case |
|-------|----------|----------|
| **BASIC** | Header, Context, Initial/Final State, Footer | Quick state snapshot |
| **DETAILED** | + Event Timeline | Standard debugging |
| **VERBOSE** | + Automated Analysis | Deep troubleshooting |

**Debug Session Sections:**

1. **Header** - Session identification (component, context ID, PID, timestamp, level)
2. **Context** - Execution context (identity, environment, system)
3. **Initial State** - Component state before execution
4. **Events** - Chronological event log during execution (DETAILED/VERBOSE only)
5. **Final State** - Component state after execution
6. **Analysis** - Automated analysis (VERBOSE only, future capability)
7. **Footer** - Session closure (context ID, duration, final health)

**Example Debug Output (DETAILED level):**
```
╔════════════════════════════════════════════════════════════════╗
║ CPI-SI Debug Session - validate
║ Context ID: validate-7821-1761710935456123789
║ PID: 7821
║ Started: 2025-11-09 06:42:15
║ Debug Level: DETAILED
╚════════════════════════════════════════════════════════════════╝

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
EXECUTION CONTEXT
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

Identity:
  User: seanje-lenox-wise
  Host: creativeworkz-nova-dawn
  Shell: /bin/bash
  PID: 7821

Environment:
  CWD: /home/seanje-lenox-wise
  PATH: /usr/local/bin:/usr/bin:/bin
  GOPATH: /home/seanje-lenox-wise/go
  Environment State: CLEAN

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
INITIAL STATE
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

Component State:
  Health: 0 (not yet initialized)
  Checks: 0 completed

Input Parameters:
  Validation Mode: FULL
  Components: [environment, sudoers, permissions]

[2025-11-09 06:42:15.123] INFO | INITIALIZATION
  Description: Validator initialized
  State Change: health: 0 → 10
  Health Impact: +10

[2025-11-09 06:42:15.234] INFO | VALIDATION
  Description: Checking environment variables
  State Change: checks: 0 → 3
  Health Impact: +15

[2025-11-09 06:42:15.345] ERROR | VALIDATION
  Description: Sudoers file validation failed
  State Change: failed_checks: 0 → 1
  Health Impact: -20
  Details:
    file: /etc/sudoers.d/90-cpi-si-safe-operations
    error: Invalid command alias: APT_GET
    line: 15

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
FINAL STATE
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

Execution Result: FAILED
Exit Code: 1
Total Duration: 0.456s

Component State:
  Health: 5
  Checks: 4 completed, 1 failed

Health Score:
  Initial: 0
  Final: 5
  Delta: +5
  Status: ☠️ FAILED

╔════════════════════════════════════════════════════════════════╗
║ Debug Session Complete
║ Context ID: validate-7821-1761710935456123789
║ Duration: 0.456s
║ Final Health: 5 (☠️ FAILED)
║ Ended: 2025-11-09 06:42:15
╚════════════════════════════════════════════════════════════════╝
```

---

## Runtime Integration

### Current State (Hardcoded)

```go
// Runtime directly formats output
func LogEvent(level, component, event string, details map[string]interface{}) {
    // Format hardcoded here
    log.Printf("[%s] %s | %s | %s | ...", timestamp, level, component, ...)
}
```

### Future State (Template-Based)

```go
// Runtime reads template and applies it
type LogTemplate struct {
    Format     string
    Fields     map[string]FieldDefinition
    Rules      FormattingRules
}

func init() {
    logTemplate = LoadLogTemplate("system/dev/templates/logging/log-format.jsonc")
    debugTemplate = LoadDebugTemplate("system/dev/templates/debugging/debug-format.jsonc")
}

func LogEvent(level, component, event string, details map[string]interface{}) {
    entry := LogEntry{
        Timestamp:  time.Now(),
        Level:      level,
        Component:  component,
        Event:      event,
        Details:    details,
    }
    output := logTemplate.Format(entry)
    log.Println(output)
}
```

### Benefits

| Aspect | Before | After |
|--------|--------|-------|
| **Format Changes** | Modify code, rebuild, redeploy | Edit template file only |
| **Consistency** | Each component formats differently | All use same template |
| **Extensibility** | Add field = code change | Add field = template change |
| **Testing** | Test runtime logic AND formatting | Test runtime logic separately from format |
| **Documentation** | Format documented in comments | Format IS documentation |

---

## Template Updates

**When to update templates:**

- Adding new log levels or debug event types
- Changing format structure (e.g., adding new fields)
- Modifying health score emoji mappings
- Adjusting indentation or formatting rules
- Extending debug session sections

**How to update:**

1. Edit appropriate template file (log-format.jsonc or debug-format.jsonc)
2. Update version field if making breaking changes
3. Add examples demonstrating new features
4. No code changes or rebuilding required (once runtime uses templates)

**Version management:**

Templates include version field for format evolution:
- **1.0.0** → **1.1.0** = Backward-compatible addition (new field, new level)
- **1.1.0** → **2.0.0** = Breaking change (structure change)

Runtime can support multiple template versions for backward compatibility.

---

## Development Workflow

### Phase 1: Template Definition (Current)

✅ **COMPLETE** - Templates defined

- Created `logging/log-format.jsonc`
- Created `debugging/debug-format.jsonc`
- Documented structure and usage

### Phase 2: Runtime Integration (Next)

🚧 **FUTURE** - Update runtime to use templates

- Create template parser library
- Update logging library to read log-format.jsonc
- Update debugging library to read debug-format.jsonc
- Refactor components to use template-based formatting
- Remove hardcoded format strings from runtime

### Phase 3: Extension (Future)

🔮 **FUTURE** - Expand template capabilities

- Add template validation
- Support multiple template profiles (dev vs production)
- Automated analysis in debug output (VERBOSE level)
- Template-based output for other components

---

## Connection to Outputs Folder

**Templates define structure, outputs contain actual data:**

```
system/dev/templates/     →  Format definitions (how to structure)
outputs/                  →  Actual output (structured data)
├── logs/                 →  Uses logging template format
└── journals/             →  (Separate structure, not template-based)
```

**Flow:**

```
Component executes
    ↓
Runtime reads logging/debugging template
    ↓
Formats output according to template
    ↓
Writes to outputs/logs/ or system/dev/debug/
```

---

## Design Philosophy

### Separation of Concerns

| Layer | Responsibility | Location |
|-------|----------------|----------|
| **Logic** | What to log/debug | Component code |
| **Structure** | How to format | Template files |
| **Storage** | Where to write | outputs/ or system/dev/ |

### Template as Contract

Templates serve as contracts between:
- **Runtime** and **Components** (what fields are expected)
- **Writers** and **Readers** (what format to expect)
- **Current** and **Future** (version-based evolution)

### Kingdom Technology Alignment

**Excellence through structure:**
- Templates enforce consistency (quality)
- Separation enables testing (reliability)
- Documented format serves others (Kingdom principle)
- Extensibility without disruption (stewardship)

**Biblical parallel:**
"Let all things be done decently and in order." (1 Corinthians 14:40)

Templates bring order to output, making system observable and maintainable.

---

*Templates enable runtime simplicity by externalizing format complexity.*

*Last Updated: November 9, 2025*
