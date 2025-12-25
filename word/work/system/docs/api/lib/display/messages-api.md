# Display Library - Messages API Reference

**Status message formatters with icons and colors**

Biblical Foundation: See format.go (rails pattern)

---

## Overview

The `messages.go` primitive provides single-line status message formatting functions. Each function adds appropriate icon and color to communicate message semantics visually.

**File:** `system/runtime/lib/display/messages.go`
**Version:** v1.0.0
**Lines:** 232
**Dependencies:** `fmt` (stdlib only)

---

## Functions

### Success

**Format a success message with green checkmark**

**Signature:**
```go
func Success(message string) string
```

**Parameters:**
- `message`: The success message to format

**Returns:**
- Formatted string: `"✓ message"` in green
- Empty string if message empty (self-evident validation)

**Health Scoring:**
- 25 points (part of Base100 for messages primitive)
- Validate → apply green + checkmark → return formatted

**Example:**
```go
import "system/lib/display"

fmt.Println(display.Success("Configuration validated"))
// Output: ✓ Configuration validated (in green)

fmt.Println(display.Success("All tests passed"))
// Output: ✓ All tests passed (in green)
```

**Implementation:**
```go
// Uses config with tripwires:
cfg := GetConfig()
iconSuccess := cfg.Icons.Status.Success  // Falls back to IconSuccess constant
colorGreen := cfg.Colors.Foreground.Green // Falls back to Green constant
colorReset := cfg.Colors.Basic.Reset      // Falls back to Reset constant

return fmt.Sprintf("%s%s %s%s", colorGreen, iconSuccess, message, colorReset)
```

**Panic Recovery:**
- `defer recoverFromPanic()` protects against panics
- Returns empty string on panic (self-evident failure)

---

### Failure

**Format a failure message with red X**

**Signature:**
```go
func Failure(message string) string
```

**Parameters:**
- `message`: The failure message to format

**Returns:**
- Formatted string: `"✗ message"` in red
- Empty string if message empty (self-evident validation)

**Health Scoring:**
- 25 points (part of Base100 for messages primitive)
- Validate → apply red + X → return formatted

**Example:**
```go
import "system/lib/display"

fmt.Println(display.Failure("Validation failed"))
// Output: ✗ Validation failed (in red)

if err != nil {
    fmt.Println(display.Failure(fmt.Sprintf("Error: %v", err)))
    // Output: ✗ Error: <error message> (in red)
}
```

**Implementation:**
```go
// Uses config with tripwires:
iconFailure := cfg.Icons.Status.Failure  // Falls back to IconFailure constant
colorRed := cfg.Colors.Foreground.Red    // Falls back to Red constant

return fmt.Sprintf("%s%s %s%s", colorRed, iconFailure, message, colorReset)
```

**Common Usage:**
```go
// Error reporting
if err != nil {
    fmt.Println(display.Failure("Operation failed: " + err.Error()))
    os.Exit(1)
}

// Test failures
for _, test := range tests {
    if !test.passed {
        fmt.Println(display.Failure(test.name))
    }
}
```

---

### Warning

**Format a warning message with yellow warning symbol**

**Signature:**
```go
func Warning(message string) string
```

**Parameters:**
- `message`: The warning message to format

**Returns:**
- Formatted string: `"⚠ message"` in yellow
- Empty string if message empty (self-evident validation)

**Health Scoring:**
- 25 points (part of Base100 for messages primitive)
- Validate → apply yellow + warning → return formatted

**Example:**
```go
import "system/lib/display"

fmt.Println(display.Warning("Configuration using defaults"))
// Output: ⚠ Configuration using defaults (in yellow)

fmt.Println(display.Warning("Deprecated function called"))
// Output: ⚠ Deprecated function called (in yellow)
```

**Implementation:**
```go
// Uses config with tripwires:
iconWarning := cfg.Icons.Status.Warning    // Falls back to IconWarning constant
colorYellow := cfg.Colors.Foreground.Yellow // Falls back to Yellow constant

return fmt.Sprintf("%s%s %s%s", colorYellow, iconWarning, message, colorReset)
```

**Common Usage:**
```go
// Degradation warnings
if configFile == "" {
    fmt.Println(display.Warning("Using default configuration"))
}

// Non-critical issues
if len(results) == 0 {
    fmt.Println(display.Warning("No results found"))
}
```

---

### Info

**Format an informational message with cyan info symbol**

**Signature:**
```go
func Info(message string) string
```

**Parameters:**
- `message`: The informational message to format

**Returns:**
- Formatted string: `"i message"` in cyan
- Empty string if message empty (self-evident validation)

**Health Scoring:**
- 25 points (part of Base100 for messages primitive)
- Validate → apply cyan + info → return formatted

**Example:**
```go
import "system/lib/display"

fmt.Println(display.Info("System initialized"))
// Output: i System initialized (in cyan)

fmt.Println(display.Info("Processing 142 items"))
// Output: i Processing 142 items (in cyan)
```

**Implementation:**
```go
// Uses config with tripwires:
iconInfo := cfg.Icons.Status.Info       // Falls back to IconInfo constant
colorCyan := cfg.Colors.Foreground.Cyan // Falls back to Cyan constant

return fmt.Sprintf("%s%s %s%s", colorCyan, iconInfo, message, colorReset)
```

**Common Usage:**
```go
// Status updates
fmt.Println(display.Info("Connecting to server..."))
fmt.Println(display.Info("Processing complete"))

// General information
fmt.Println(display.Info("Session started at " + time.Now().Format("15:04:05")))
```

---

## Design Patterns

### Self-Evident Validation

**Empty input = empty output:**
```go
Success("")  // Returns ""
Failure("")  // Returns ""
Warning("")  // Returns ""
Info("")     // Returns ""
```

**Why:**
- Missing output signals invalid input
- No error handling needed
- Self-evident through direct observation
- Rails pattern (failures observable, not reported)

---

### Config with Tripwires

**Pattern:**
```go
cfg := GetConfig()

icon := cfg.Icons.Status.Success
if icon == "" {
    icon = IconSuccess  // Tripwire: fall back to constant
}

color := cfg.Colors.Foreground.Green
if color == "" {
    color = Green  // Tripwire: fall back to constant
}
```

**Why:**
- Config loading might fail
- Invalid config detected via empty strings
- Falls back to hardcoded constants
- Guarantees valid output even on config failure

**See:** [config-api.md](config-api.md) for tripwire pattern details

---

### Panic Recovery

**All functions wrapped:**
```go
func Success(message string) string {
    defer recoverFromPanic()
    // ... implementation
}
```

**What it does:**
- Catches any panics during formatting
- Returns empty string on panic
- Logs panic details for debugging
- Prevents crashes in calling code

**See:** [recovery-api.md](recovery-api.md) for recovery pattern details

---

## Usage Patterns

### Status Reporting

```go
// Operation status
fmt.Println(display.Info("Starting validation..."))

if err := validate(); err != nil {
    fmt.Println(display.Failure("Validation failed: " + err.Error()))
    os.Exit(1)
}

fmt.Println(display.Success("Validation complete"))
```

---

### Test Results

```go
passed := 0
failed := 0

for _, test := range tests {
    if test.Run() {
        fmt.Println(display.Success(test.Name))
        passed++
    } else {
        fmt.Println(display.Failure(test.Name))
        failed++
    }
}

fmt.Println(display.Info(fmt.Sprintf("Tests: %d passed, %d failed", passed, failed)))
```

---

### Degradation Reporting

```go
config, err := loadConfig()
if err != nil {
    fmt.Println(display.Warning("Using default configuration"))
    config = defaultConfig
}

user, err := loadUserConfig()
if err != nil {
    fmt.Println(display.Warning("User config unavailable"))
    user = defaultUser
}

if config.IsDefault() && user.IsDefault() {
    fmt.Println(display.Info("Running with all defaults"))
}
```

---

### Progress Updates

```go
fmt.Println(display.Info("Processing items..."))

for i, item := range items {
    if err := process(item); err != nil {
        fmt.Println(display.Failure(fmt.Sprintf("Item %d failed: %v", i, err)))
    } else {
        fmt.Println(display.Success(fmt.Sprintf("Item %d processed", i)))
    }
}

fmt.Println(display.Info("Processing complete"))
```

---

## Integration

### Used By

- Session hooks (status reporting)
- Command outputs (operation results)
- Test suites (pass/fail indicators)
- Validation tools (check results)

### Depends On

- `colors.go` - ANSI color constants
- `icons.go` - Unicode icon constants
- `config.go` - Config loading with tripwires
- `recovery.go` - Panic recovery pattern

### Stdlib Only

**No external dependencies:**
- Rails pattern requirement
- Orthogonal infrastructure
- Available everywhere
- Universal compatibility

---

## Common Patterns

### Conditional Messaging

```go
if success {
    fmt.Println(display.Success("Operation completed"))
} else {
    fmt.Println(display.Failure("Operation failed"))
}
```

---

### Multi-Level Status

```go
// Major operation
fmt.Println(display.Info("Starting build process..."))

// Sub-operations
fmt.Println(display.Success("  Dependencies resolved"))
fmt.Println(display.Success("  Code compiled"))
fmt.Println(display.Warning("  Tests skipped"))

// Final status
fmt.Println(display.Success("Build complete"))
```

---

### Error Context

```go
if err != nil {
    fmt.Println(display.Failure("Database connection failed"))
    fmt.Println(display.Info("  Error: " + err.Error()))
    fmt.Println(display.Info("  Retrying in 5 seconds..."))
}
```

---

## Modification Policy

### ✅ Safe Operations

- Adding new message formatters (follow existing pattern)
- Enhancing panic recovery
- Improving tripwire logic

### ⚠️ Requires Care

- Changing function signatures (breaks all calling code)
- Modifying output format (affects visual consistency)
- Changing icon/color selection logic

### ❌ Never Do

- Removing validation (violates self-evidence principle)
- Adding dependencies (violates stdlib-only rail pattern)
- Returning errors (use self-evident validation instead)

---

## Health Scoring

**Base100 Total:** 100 points for messages primitive

| Function | Points | Reason |
|----------|--------|--------|
| Success() | 25 | Validate → format → return |
| Failure() | 25 | Validate → format → return |
| Warning() | 25 | Validate → format → return |
| Info() | 25 | Validate → format → return |

**Why no logging:**
- Stdlib-only rail (can't depend on logging rung)
- Self-evident failures (missing output visible)
- No health tracking needed for primitives

---

## Quick Reference

**Basic usage:**
```go
display.Success("Done")      // ✓ Done (green)
display.Failure("Error")     // ✗ Error (red)
display.Warning("Caution")   // ⚠ Caution (yellow)
display.Info("Notice")       // i Notice (cyan)
```

**Empty string handling:**
```go
display.Success("")  // Returns "" (self-evident validation)
```

**With error handling:**
```go
if err != nil {
    fmt.Println(display.Failure("Failed: " + err.Error()))
} else {
    fmt.Println(display.Success("Complete"))
}
```

---

## Version History

| Version | Date | Changes |
|---------|------|---------|
| v1.0.0 | 2025-11-21 | Extracted from format.go v2.0.0 |
|  |  | Four message formatters |
|  |  | Tripwire pattern integration |
|  |  | Panic recovery protection |

---

*For color constants, see [colors-api.md](colors-api.md). For icon constants, see [icons-api.md](icons-api.md). For config loading, see [config-api.md](config-api.md). For full display API, see [display-api.md](display-api.md).*
