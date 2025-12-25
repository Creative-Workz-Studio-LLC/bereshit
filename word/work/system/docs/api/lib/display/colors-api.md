# Display Library - Colors API Reference

**ANSI escape sequence constants for terminal formatting**

Biblical Foundation: See format.go (rails pattern)

---

## Overview

The `colors.go` primitive provides ANSI escape sequence constants for colored terminal output. These are the fundamental building blocks for all colored text in the CPI-SI system.

**File:** `system/runtime/lib/display/colors.go`
**Version:** v1.0.0
**Lines:** 121
**Dependencies:** None (pure constants, stdlib only)

---

## Constants

### Text Modifiers

#### Reset

**Clears all formatting, returning terminal to default state**

```go
const Reset = "\033[0m"
```

**Usage:**
- Applied at end of every formatted string to prevent color bleeding
- Critical for maintaining terminal state between messages

**Example:**
```go
fmt.Println(Green + "Success" + Reset)
// Ensures terminal returns to default after green text
```

---

#### Bold

**Increases text weight for emphasis**

```go
const Bold = "\033[1m"
```

**Usage:**
- Headers and subheaders
- Table headers
- Important text requiring emphasis

**Example:**
```go
fmt.Println(Bold + "IMPORTANT" + Reset)
```

---

#### Dim

**Reduces text intensity for de-emphasized content**

```go
const Dim = "\033[2m"
```

**Usage:**
- KeyValue() keys (showing they're labels not values)
- Secondary information
- Low-priority content

**Example:**
```go
fmt.Println(Dim + "Label:" + Reset + " Value")
```

---

### Foreground Colors

#### Red

**Indicates errors, failures, and critical issues**

```go
const Red = "\033[31m"
```

**Used by:**
- `Failure()` - Error messages
- Negative `StatusLine()` - Health below zero
- Critical messages

**Example:**
```go
fmt.Println(Red + "✗ Operation failed" + Reset)
```

---

#### Green

**Indicates success, completion, and positive states**

```go
const Green = "\033[32m"
```

**Used by:**
- `Success()` - Success messages
- Positive `StatusLine()` - Health above zero
- Completion indicators

**Example:**
```go
fmt.Println(Green + "✓ Configuration validated" + Reset)
```

---

#### Yellow

**Indicates warnings and caution requiring attention**

```go
const Yellow = "\033[33m"
```

**Used by:**
- `Warning()` - Warning messages
- Non-critical issues
- Review-needed indicators

**Example:**
```go
fmt.Println(Yellow + "⚠ Using default configuration" + Reset)
```

---

#### Blue

**Provides neutral informational coloring**

```go
const Blue = "\033[34m"
```

**Status:** Currently unused - reserved for future info categories

---

#### Magenta

**Provides distinct categorization separate from status**

```go
const Magenta = "\033[35m"
```

**Status:** Currently unused - reserved for special categories

---

#### Cyan

**Indicates general information and structural elements**

```go
const Cyan = "\033[36m"
```

**Used by:**
- `Info()` - Informational messages
- General structural elements

**Example:**
```go
fmt.Println(Cyan + "i System initialized" + Reset)
```

---

#### Gray

**Provides muted text for secondary information**

```go
const Gray = "\033[37m"
```

**Status:** Currently unused - available for low-priority content

---

### Bold Foreground Colors

#### BoldRed

**Emphasizes critical failures and errors demanding attention**

```go
const BoldRed = "\033[1;31m"
```

**Status:** Currently unused - available for critical emphasis

---

#### BoldGreen

**Emphasizes important successes and achievements**

```go
const BoldGreen = "\033[1;32m"
```

**Status:** Currently unused - available for success emphasis

---

#### BoldYellow

**Emphasizes important warnings requiring immediate review**

```go
const BoldYellow = "\033[1;33m"
```

**Status:** Currently unused - available for warning emphasis

---

#### BoldBlue

**Provides emphasized informational hierarchy**

```go
const BoldBlue = "\033[1;34m"
```

**Status:** Currently unused - reserved for emphasized info

---

#### BoldMagenta

**Provides emphasized special categorization**

```go
const BoldMagenta = "\033[1;35m"
```

**Status:** Currently unused - reserved for special emphasis

---

#### BoldCyan

**Emphasizes structural headers and important information**

```go
const BoldCyan = "\033[1;36m"
```

**Used by:**
- `Header()` - Section headers
- `Box()` - Border lines
- Visual structure emphasis

**Example:**
```go
fmt.Println(BoldCyan + "═════════════════" + Reset)
```

---

## ANSI Color Format

### Basic Structure

```
\033[<attributes>m
```

Where attributes are numeric codes:
- `0` = reset all formatting
- `1` = bold text weight
- `2` = dim text intensity
- `31-37` = foreground colors

### Combining Attributes

**Bold + Color:**
```
\033[1;32m = Bold + Green
```

**Multiple effects:**
```
fmt.Println("\033[1;4;32mBold Underline Green\033[0m")
```

---

## Usage Patterns

### Simple Coloring

```go
import "system/lib/display"

// Basic color
message := display.Green + "Success!" + display.Reset
fmt.Println(message)

// Bold emphasis
header := display.Bold + "IMPORTANT" + display.Reset
fmt.Println(header)
```

---

### Combining Colors and Modifiers

```go
// Bold cyan header
header := display.BoldCyan + "Section Header" + display.Reset

// Dim label + normal value
output := display.Dim + "Status:" + display.Reset + " " +
          display.Green + "Active" + display.Reset
```

---

### Error Handling Pattern

```go
if err != nil {
    fmt.Println(display.Red + "✗ Error: " + err.Error() + display.Reset)
} else {
    fmt.Println(display.Green + "✓ Success" + display.Reset)
}
```

---

## Future Enhancements

### Planned Features (Phase 7+)

**256-Color Support:**
```
\033[38;5;Nm  // N = 0-255
```

**RGB True Color:**
```
\033[38;2;R;G;Bm  // R,G,B = 0-255
```

**Configuration Loading:**
- Load from `system/data/config/display/formatting.jsonc`
- Automatic terminal capability detection
- Fallback to 16-color for limited terminals

---

## Design Philosophy

### Stdlib-Only Rail

**Why no dependencies:**
- Rail = orthogonal infrastructure
- Must be available everywhere
- Cannot depend on rungs
- Stdlib-only ensures universal availability

### Self-Evident Failures

**Color not showing?**
- Terminal doesn't support ANSI codes
- Output redirected to non-terminal (file, pipe)
- Self-evident through direct observation

### Constant Safety

**Why constants, not variables:**
- Compile-time guaranteed
- No runtime modification possible
- Thread-safe by design
- Zero overhead

---

## Modification Policy

### ✅ Safe Operations

- Adding new color constants
- Extending palette with bold variants
- Adding comments/documentation

### ⚠️ Requires Care

- Changing existing constant values (affects ALL output)
- Renaming constants (breaks calling code)

### ❌ Never Do

- Removing constants in use
- Changing ANSI format without testing
- Adding dependencies (violates stdlib-only rail pattern)

---

## Terminal Compatibility

### Supported Terminals

**Full ANSI Support:**
- Modern Linux terminals (gnome-terminal, konsole, etc.)
- macOS Terminal.app and iTerm2
- Windows Terminal
- VSCode integrated terminal

**Limited Support:**
- Windows CMD (Win10+ supports ANSI)
- Basic SSH terminals

**No Support:**
- Non-interactive shells (scripts, pipes)
- Very old terminals

### Graceful Degradation

**When colors don't work:**
- Output still readable (icons + text remain)
- Semantic meaning preserved (✓/✗ indicators)
- No crashes or errors

---

## Quick Reference

**Basic usage:**
```go
display.Green + "text" + display.Reset
```

**Bold header:**
```go
display.BoldCyan + "HEADER" + display.Reset
```

**Status indicators:**
```go
display.Red + "✗ " + message + display.Reset      // Failure
display.Green + "✓ " + message + display.Reset    // Success
display.Yellow + "⚠ " + message + display.Reset   // Warning
display.Cyan + "i " + message + display.Reset     // Info
```

**Emphasis:**
```go
display.Bold + "important" + display.Reset
display.Dim + "label:" + display.Reset + " value"
```

---

## Version History

| Version | Date | Changes |
|---------|------|---------|
| v1.0.0 | 2025-11-21 | Extracted from format.go v2.0.0 |
|  |  | Pure constants, no dependencies |
|  |  | Full ANSI 16-color palette |

---

*For usage in context, see [display-api.md](display-api.md). For message formatting using these colors, see [messages-api.md](messages-api.md).*
