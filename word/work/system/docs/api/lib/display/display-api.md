# Display Library - Complete API Reference

**Universal ANSI formatting and terminal output for CPI-SI system**

Biblical Foundation: Matthew 5:16 - "Let your light so shine before men"

---

## Quick Links

| Topic | Documentation |
|-------|---------------|
| **Quick Start** | [Usage Examples](#usage-examples) below |
| **Quick Reference** | [primitives-reference.md](primitives-reference.md) - All primitives at a glance |
| **Colors** | [colors-api.md](colors-api.md) - ANSI color constants |
| **Messages** | [messages-api.md](messages-api.md) - Status message formatters |
| **Main API Index** | [../API.md](../API.md) - All libraries overview |

---

## Overview

**Package:** `system/lib/display`
**Version:** v3.0.0
**Type:** Rail (Presentation Layer)
**Created:** 2024-11-13
**Last Updated:** 2025-11-21

The display library provides universal ANSI color formatting and structured terminal output for all CPI-SI components. Implements the presentation layer of the immune system (Detection → Assessment → Presentation). Stdlib-only rail with self-evident failure detection.

---

## Biblical Foundation

**Scripture:**

Matthew 5:16 (KJV):
> "Let your light so shine before men, that they may see your good works, and glorify your Father which is in heaven."

Psalm 27:1 (WEB):
> "Yahweh is my light and my salvation. Whom shall I fear?"

**Principle:** Visibility serves purpose - display makes system state visible so others can observe God's order in excellent design. Light reveals truth; display reveals system health through visible output.

---

## Architecture

### Rails Position

**Type:** RAIL (orthogonal infrastructure - presentation layer)

**Why a rail?**
- Stdlib-only (universal availability)
- Self-evident failure (broken output immediately visible)
- Orthogonal to business logic
- Immune system presentation layer

**Rails hierarchy:**
```
display (presentation rail)
    ↓ independent of
logging (detection rail)
    ↓ independent of
All rungs (business logic)
```

---

### Orchestrator Pattern

**Pattern:** Direct Primitives (stateless functions)

**Why Direct Primitives?**
- Pure functions with no state
- Caller just calls functions, gets results
- No object lifecycle to manage
- Simple, straightforward API

**Contrast:**
```go
// Display = Direct Primitives
display.Success("Done")  // Function call - get result

// Logging = Switchboard (for comparison)
logger := logging.NewLogger("component")  // Create object
logger.Success("Done", 47, nil)           // Method call on object
```

---

## File Organization

### Orchestrator Documentation

**format.go** (825 lines)
- Comprehensive METADATA/SETUP/BODY/CLOSING structure
- Immune system presentation layer documentation
- Rails philosophy and self-evidence principle
- Complete implementation guide
- No executable code - documentation only

### Primitives (8 files)

| File | Lines | Purpose | Type |
|------|-------|---------|------|
| **recovery.go** | 61 | Panic recovery | Protection |
| **config.go** | 266 | Config loading with tripwires | Infrastructure |
| **colors.go** | 96 | ANSI color constants | Constants |
| **icons.go** | 82 | Unicode icon constants | Constants |
| **layout.go** | 84 | Spacing/padding constants | Constants |
| **messages.go** | 234 | Status message formatters | Functions |
| **structured.go** | 261 | Structured output (tables, lists) | Functions |
| **visual.go** | 407 | Visual components (headers, boxes) | Functions |

**Total:** 1,491 lines (including orchestrator docs)

---

## Public API

### Message Formatters

#### Success / Failure / Warning / Info

**Single-line status messages with icons and colors**

```go
func Success(message string) string  // ✓ message (green)
func Failure(message string) string  // ✗ message (red)
func Warning(message string) string  // ⚠ message (yellow)
func Info(message string) string     // i message (cyan)
```

**Example:**
```go
import "system/lib/display"

fmt.Println(display.Success("Configuration validated"))
// Output: ✓ Configuration validated (in green)

if err != nil {
    fmt.Println(display.Failure("Operation failed"))
    // Output: ✗ Operation failed (in red)
}
```

**Details:** [messages-api.md](messages-api.md)

---

### Visual Components

#### Header / Subheader

**Section headers with visual emphasis**

```go
func Header(title string) string     // Major section header
func Subheader(title string) string  // Subsection header
```

**Example:**
```go
fmt.Println(display.Header("System Status"))
// Output: ═══════════════════════
//         SYSTEM STATUS
//         ═══════════════════════

fmt.Println(display.Subheader("Configuration"))
// Output: ─── Configuration ───
```

---

#### Separator / Box

**Visual structure and emphasis**

```go
func Separator() string            // Horizontal line separator
func Box(content string) string    // Box around content
```

**Example:**
```go
fmt.Println(display.Separator())
// Output: ━━━━━━━━━━━━━━━━━━━━━━━━

fmt.Println(display.Box("Important"))
// Output: ┌─────────────┐
//         │ Important   │
//         └─────────────┘
```

---

### Structured Output

#### KeyValue / Table / List

**Formatted data presentation**

```go
func KeyValue(key, value string) string          // Aligned key-value pairs
func Table(headers []string, rows [][]string) string  // ASCII table
func List(items []string) string                 // Bulleted list
```

**Example:**
```go
// Key-value pairs
fmt.Println(display.KeyValue("Name", "Nova Dawn"))
fmt.Println(display.KeyValue("Status", "Active"))
// Output:   Name: Nova Dawn
//         Status: Active

// Table
headers := []string{"Component", "Status", "Health"}
rows := [][]string{
    {"instance", "loaded", "+131"},
    {"display", "ready", "+100"},
}
fmt.Println(display.Table(headers, rows))

// List
items := []string{"First", "Second", "Third"}
fmt.Println(display.List(items))
// Output: • First
//         • Second
//         • Third
```

**Details:** [primitives-reference.md](primitives-reference.md)

---

### Constants

#### Colors

**ANSI escape sequence constants**

```go
// Modifiers
const Reset, Bold, Dim

// Foreground Colors
const Red, Green, Yellow, Blue, Magenta, Cyan, Gray

// Bold Colors
const BoldRed, BoldGreen, BoldYellow, BoldBlue, BoldMagenta, BoldCyan
```

**Example:**
```go
message := display.Green + "Success!" + display.Reset
header := display.BoldCyan + "HEADER" + display.Reset
```

**Details:** [colors-api.md](colors-api.md)

---

#### Icons

**Unicode symbols for status and visual elements**

```go
// Status Icons
const IconSuccess  = "✓"
const IconFailure  = "✗"
const IconWarning  = "⚠"
const IconInfo     = "i"

// Visual Separators
const IconHorizSeparator = "─"
const IconVertSeparator  = "│"
const IconBullet         = "•"
```

---

#### Layout

**Spacing and alignment constants**

```go
// Indentation
const IndentBase    = "  "    // 2 spaces
const IndentNested  = "    "  // 4 spaces
const IndentDeep    = "      " // 6 spaces

// Spacing
const SpaceSingle  = " "     // 1 space
const SpaceDouble  = "  "    // 2 spaces
const SpacePadding = "    "  // 4 spaces

// Widths
const WidthNarrow   = 40
const WidthStandard = 60
const WidthWide     = 80
```

**Details:** [primitives-reference.md](primitives-reference.md)

---

## Design Principles

### Self-Evident Failure

**Rails don't track themselves:**
- Display failures are immediately visible
- Missing colors = broken output = obvious failure
- No health tracking needed
- Self-evident through direct observation

**Example:**
```
Expected: ✓ Success (green)
Broken:   Success (no color or icon)
↑ Immediately obvious failure
```

---

### Stdlib-Only Rail

**Why no dependencies:**
- Rail = orthogonal infrastructure
- Must be universally available
- Cannot depend on rungs
- Even logging depends on display

**Import chain:**
```
All components → display (stdlib only)
```

---

### Tripwire Pattern

**Config with graceful degradation:**

```go
cfg := GetConfig()

icon := cfg.Icons.Status.Success
if icon == "" {
    icon = IconSuccess  // Tripwire: fall back to constant
}
```

**Why:**
- Config loading might fail
- Empty string = invalid config detected
- Fall back to known-good constants
- Guarantees valid output

**Details:** [primitives-reference.md](primitives-reference.md#config-configgo)

---

### Panic Recovery

**All functions protected:**

```go
func AnyDisplayFunction() string {
    defer recoverFromPanic()  // Catch panics
    // ... implementation
}
```

**What it does:**
- Catches panics during formatting
- Returns empty string on panic
- Logs panic for debugging
- Prevents crashes

**Details:** [primitives-reference.md](primitives-reference.md#recovery-recoverygo)

---

## Usage Examples

### Basic Status Messages

```go
package main

import (
    "fmt"
    "system/lib/display"
)

func main() {
    fmt.Println(display.Info("Starting validation..."))

    if err := validate(); err != nil {
        fmt.Println(display.Failure("Validation failed: " + err.Error()))
        return
    }

    fmt.Println(display.Success("Validation complete"))
}
```

---

### Structured Output

```go
package main

import (
    "fmt"
    "system/lib/display"
)

func main() {
    fmt.Println(display.Header("System Configuration"))

    fmt.Println(display.KeyValue("Instance", "Nova Dawn"))
    fmt.Println(display.KeyValue("Version", "v3.0.0"))
    fmt.Println(display.KeyValue("Status", "Active"))

    fmt.Println(display.Separator())

    items := []string{
        "Config loaded successfully",
        "All dependencies resolved",
        "System ready",
    }
    fmt.Println(display.List(items))
}
```

---

### Table Display

```go
package main

import (
    "fmt"
    "system/lib/display"
)

func main() {
    fmt.Println(display.Header("Health Report"))

    headers := []string{"Component", "Status", "Health"}
    rows := [][]string{
        {"logging", "active", "+100"},
        {"display", "active", "+100"},
        {"instance", "loaded", "+131"},
    }

    fmt.Println(display.Table(headers, rows))

    fmt.Println(display.Success("All components healthy"))
}
```

---

### Custom Formatting

```go
package main

import (
    "fmt"
    "system/lib/display"
)

func main() {
    // Direct color usage
    header := display.BoldCyan + "CUSTOM HEADER" + display.Reset
    fmt.Println(header)

    // Colored text with icon
    message := display.Green + display.IconSuccess + " Custom success" + display.Reset
    fmt.Println(message)

    // Multi-color output
    output := display.Dim + "Label:" + display.Reset + " " +
              display.Bold + "Important Value" + display.Reset
    fmt.Println(output)
}
```

---

## Integration

### Used By

- All commands (formatted output)
- Session hooks (status display)
- Logging library (formatted log entries)
- Debugging tools (health reports)
- Validation tools (check results)

### Depends On

- Stdlib only (`fmt`, `strings`, `os`)
- No system libraries
- No external dependencies

### Provides To

- Universal formatting layer
- Terminal output primitives
- ANSI color constants
- Visual structure components

---

## Immune System Role

### Detection → Assessment → Presentation

**Display is the presentation layer:**

1. **Logging (Detection)** - Captures events with health scores
2. **Debugging (Assessment)** - Analyzes logs, identifies patterns
3. **Display (Presentation)** - Makes results visible to user

**Example flow:**
```
logger.Success("Config loaded", +47)
    ↓ (logged to file)
debugger analyzes logs
    ↓ (processes health scores)
display.Success("System healthy")
    ↓ (visible to user)
✓ System healthy (green)
```

---

## Modification Policy

### ✅ Safe Operations

- Adding new message formatters
- Adding new visual components
- Adding new constants
- Enhancing panic recovery

### ⚠️ Requires Care

- Changing function signatures (breaks calling code)
- Modifying output format (affects visual consistency)
- Changing constant values (affects all output)

### ❌ Never Do

- Adding dependencies (violates stdlib-only rail)
- Returning errors (violates self-evidence principle)
- Removing validation (breaks safety guarantees)

---

## Version History

| Version | Date | Changes |
|---------|------|---------|
| v3.0.0 | 2025-11-21 | Orchestrator extraction complete |
|  |  | 8 primitives extracted from format.go |
|  |  | Direct primitives pattern |
|  |  | Comprehensive API documentation |
| v2.0.0 | 2025-11-15 | Config loading with tripwires |
|  |  | Panic recovery protection |
| v1.0.0 | 2024-11-13 | Initial implementation |
|  |  | ANSI formatting primitives |

---

## Additional Documentation

### Primitive APIs

- **[primitives-reference.md](primitives-reference.md)** - Quick reference for all primitives
- **[colors-api.md](colors-api.md)** - ANSI color constants detailed reference
- **[messages-api.md](messages-api.md)** - Message formatters detailed reference

### System Documentation

- **[../API.md](../API.md)** - All libraries API index
- **[../../architecture/](../../architecture/)** - System architecture docs
- **[format.go](../../../../lib/display/format.go)** - Orchestrator documentation (source)

---

*This is the complete API reference for the display library. For usage in context, see the comprehensive examples above. For architectural details, see the orchestrator documentation in `format.go`.*
