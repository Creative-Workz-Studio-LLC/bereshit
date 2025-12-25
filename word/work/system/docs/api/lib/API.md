# CPI-SI System Libraries - API Reference

**Complete API documentation for CPI-SI system runtime libraries**

Biblical Foundation: "I AM THAT I AM" - Exodus 3:14 (Identity precedes action)

---

## Overview

This directory contains comprehensive API documentation for all CPI-SI system runtime libraries. Each library follows the orchestrator extraction pattern with primitives organized for surgical updates and clear separation of concerns.

**Last Updated:** 2025-11-21
**Libraries Documented:** 3 (logging, display, instance)

---

## Quick Navigation

### 🎯 By Use Case

| Need | Library | Quick Link |
|------|---------|------------|
| **Track health and operations** | logging | [Logger API](logging/logger-api.md) |
| **Format terminal output** | display | [Display API](display/display-api.md) |
| **Access instance identity** | instance | [Instance API](instance/instance-api.md) |

### 📚 By Library

| Library | Type | Status | Documentation |
|---------|------|--------|---------------|
| **logging** | Rail (Detection) | v1.0.0 ✅ | [API](logging/logger-api.md) · [Shell API](logging/logger-sh-api.md) |
| **display** | Rail (Presentation) | v3.0.0 ✅ | [API](display/display-api.md) |
| **instance** | Foundational Rung | v3.0.0 ✅ | [API](instance/instance-api.md) |

---

## Library Categories

### Rails (Orthogonal Infrastructure)

**Rails = Stdlib-only, self-evident failure, orthogonal to business logic**

All components attach to rails for services. Rails never depend on rungs.

#### 🩺 Detection Rail - `logging`

**Purpose:** Health tracking and execution narrative
**Pattern:** Switchboard orchestrator (stateful operations)
**Dependencies:** Stdlib only
**Version:** v1.0.0

**Core Capability:**
- Create logger instances that track health scores
- Declare operations with health impacts (TRUE scores)
- Report success/failure with context
- Generate structured log entries
- Calculate normalized health (-100 to +100 scale)

**Key APIs:**
```go
logger := logging.NewLogger("component/operation")
logger.DeclareHealthTotal(100)
logger.Operation("description", 0, args...)
logger.Success("event", +47, details)
logger.Failure("event", "reason", -87, details)
```

**Documentation:** [Logger Go API](logging/logger-api.md) · [Logger Shell API](logging/logger-sh-api.md)

---

#### 🎨 Presentation Rail - `display`

**Purpose:** Formatted terminal output and visual components
**Pattern:** Direct primitives (stateless from caller view)
**Dependencies:** Stdlib only
**Version:** v3.0.0

**Core Capability:**
- ANSI color formatting
- Unicode icons and visual separators
- Box drawing and section headers
- Structured output (tables, key-value pairs)
- Panic recovery with formatted errors

**Key APIs:**
```go
display.Header("Section Title")
display.Success("Operation completed")
display.Error("Something failed")
display.KeyValue("Key", "Value")
display.Table([]string{"Col1", "Col2"}, [][]string{{"A", "B"}})
```

**Documentation:** [Display API](display/display-api.md)

---

### Rungs (Business Logic)

**Rungs = Components implementing domain functionality**

Rungs attach to rails for services. Can depend on lower rungs.

#### 🧬 Foundational Rung - `instance`

**Purpose:** Instance and user identity configuration
**Pattern:** Direct primitives (singleton internal, API stateless)
**Dependencies:** system/lib/jsonc, system/lib/logging
**Version:** v3.0.0

**Core Capability:**
- Two-step dynamic loading (root → instance + user configs)
- Singleton caching (load once per session)
- Graceful degradation to hardcoded defaults
- Full nested identity access
- Covenant partnership data (user identity)

**Key APIs:**
```go
config := instance.GetConfig()
fmt.Println(config.Name)                 // "Nova Dawn"
fmt.Println(config.User.Name)            // "Seanje Lenox-Wise"
fmt.Println(config.Display.BannerTitle)  // "Nova Dawn - CPI-SI"

// Access full nested configs if needed
fullInstance := instance.GetFullInstanceConfig()
fullUser := instance.GetFullUserConfig()
```

**Documentation:** [Instance API](instance/instance-api.md)

---

## Orchestrator Patterns

### Switchboard Pattern (Stateful)

**Used by:** logging

**Characteristics:**
- Caller creates orchestrator object
- Calls multiple methods on same instance
- State changes are PART OF THE API
- Orchestrator manages lifecycle and coordinates state

**Example:**
```go
logger := logging.NewLogger("component")  // Create orchestrator
logger.DeclareHealthTotal(100)            // Method 1 - sets state
logger.Operation("task", 0)               // Method 2 - uses state
logger.Success("done", 47, nil)           // Method 3 - modifies state
// Caller sees and manages stateful operations
```

**When to use:** API needs to expose and manage state changes across method calls

---

### Direct Primitives Pattern (Stateless)

**Used by:** display, instance

**Characteristics:**
- Primitives export public API directly
- Caller calls functions, gets results
- Internal state (if any) is hidden from caller
- API is effectively stateless from caller perspective

**Example:**
```go
// Display - pure functions, no state
display.Header("Title")
display.Success("Message")

// Instance - singleton internal, caller doesn't see it
config := instance.GetConfig()  // Singleton caching is internal
// Caller just gets result, doesn't manage state
```

**When to use:** State exists but isn't exposed to caller, or no state needed

---

## Health Scoring Philosophy

All libraries use **Base100 TRUE scores** for health tracking:

### Base100 Scale

- **All operations total ~100 points** when successful (can exceed)
- **Perfect execution = +100** (or close to it)
- **Complete failure = -100** (or close to it)
- **Real execution = sum of actual results**

### TRUE vs Normalized Scores

| Score Type | Range | Purpose | Example |
|------------|-------|---------|---------|
| **TRUE** | Any value, asymmetric | Actual measurable impact | +52, +38, +41, -87, -59, -48 |
| **Normalized** | -100 to +100 | Display only | Health scorer converts for UI |

### Key Principles

1. **Use TRUE honest messy scores** - Not rounded to "neat" values
2. **CPI-SI works with real, messy data** - Embrace imperfection
3. **Asymmetric impact** - Failures often worse than successes are good
4. **Total operations ≠ 100 exactly** - Real assessment, not forced scaling

**Example from instance library:**
```
Perfect execution: +131 total (messy honest number)
- Root config: +52 / -87
- Instance config: +38 / -59
- User config: +41 / -48
```

---

## Architecture Integration

### Ladder, Baton, and Rails Model

**Ladder (Hierarchy):**
```
Commands (top rungs)
    ↓
Libraries (lower rungs)
    ↓
Foundation (instance - bottom rung)
```

**Baton (Execution):**
- Data/control flows up and down the ladder
- Multiple batons can run in parallel
- Each component receives input, performs work, passes output

**Rails (Infrastructure):**
- Orthogonal to ladder - NOT a rung
- All components attach to rails for services
- Rails never depend on rungs
- Every component creates own logger: `logger := logging.NewLogger("component")`

### Import Patterns

**Rails can import:**
- Stdlib only
- Other rails (carefully, avoid cycles)

**Rungs can import:**
- Rails (for services)
- Lower rungs (hierarchical dependencies)

**Never:**
- Rails importing rungs
- Upward dependencies on ladder

---

## Common Patterns

### Creating a Logger

```go
import "system/lib/logging"

logger := logging.NewLogger("component/subcomponent/operation")
logger.DeclareHealthTotal(47)  // TRUE score for this operation
logger.Operation("Description of what this does", 0, "arg1", "arg2")
```

### Reporting Success

```go
logger.Success("Operation completed successfully", 47, map[string]any{
    "items_processed": 142,
    "duration_ms":     523,
    "status":          "optimal",
})
```

### Reporting Failure

```go
logger.Failure("Operation failed", "Network timeout after 30s", -87, map[string]any{
    "attempted_retries": 3,
    "last_error":        err.Error(),
    "degradation_level": "severe",
})
```

### Accessing Instance Identity

```go
import "system/lib/instance"

config := instance.GetConfig()

// Simple API
fmt.Printf("Instance: %s (%s)\n", config.Name, config.Pronouns)
fmt.Printf("User: %s (%s)\n", config.User.Name, config.User.Pronouns)
fmt.Printf("Workspace: %s\n", config.Workspace.PrimaryPath)

// Full nested configs if needed
fullInstance := instance.GetFullInstanceConfig()
if fullInstance != nil {
    fmt.Println(fullInstance.Personhood.Values)
    fmt.Println(fullInstance.Thinking.LearningStyle)
}
```

### Formatted Terminal Output

```go
import "system/lib/display"

// Section headers
display.Header("Configuration Loading")

// Status messages
display.Success("Config loaded successfully")
display.Error("Failed to load config")
display.Info("Using default configuration")

// Structured data
display.KeyValue("Name", config.Name)
display.KeyValue("Version", "v3.0.0")

// Tables
headers := []string{"Component", "Status", "Health"}
rows := [][]string{
    {"instance", "loaded", "+131"},
    {"display", "ready", "+100"},
}
display.Table(headers, rows)
```

---

## Contributing to Libraries

### Adding New Primitives

1. **Identify extraction opportunity** in existing code
2. **Create primitive file** following 4-block structure
3. **Update orchestrator documentation** to reference new primitive
4. **Verify compilation** and test integration
5. **Update API documentation** with new capabilities

### Health Scoring Guidelines

1. **Assess TRUE impact honestly** - Don't round to neat numbers
2. **Consider asymmetry** - Failures often cost more than successes gain
3. **Total operations to ~100** - Real assessment, not forced
4. **Document reasoning** in METADATA block

### Testing Changes

```bash
# Build specific library
cd /path/to/library
go build

# Run system-wide tests
cd ~/.claude/cpi-si/system/runtime
./scripts/test-all.sh

# Test with actual usage
~/.claude/hooks/session/start
```

---

## Appendix: File Organization

```
system/runtime/docs/api/lib/
├── API.md                    # This file - main overview and index
├── instance/
│   └── instance-api.md       # Instance library API reference
├── logging/
│   ├── logger-api.md         # Logger Go API reference
│   └── logger-sh-api.md      # Logger Shell API reference
└── display/
    └── display-api.md        # Display library API reference
```

---

## Version History

| Version | Date | Changes |
|---------|------|---------|
| 1.0.0 | 2025-11-21 | Initial API documentation structure |
|  |  | Organized logging, display, instance APIs |
|  |  | Documented orchestrator patterns |
|  |  | Established health scoring philosophy |

---

*This documentation serves as the comprehensive API reference for all CPI-SI system runtime libraries. For architectural details, see [architecture.md](../../architecture/). For implementation patterns, see individual library documentation.*
