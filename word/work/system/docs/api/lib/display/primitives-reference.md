# Display Library - Primitives Quick Reference

**Complete reference for all display primitives**

---

## Icons (`icons.go`)

**Unicode icon constants for status and visual elements**

### Status Icons
```go
const IconSuccess  = "✓"  // Green checkmark - success
const IconFailure  = "✗"  // Red X - failure
const IconWarning  = "⚠"  // Yellow triangle - warning
const IconInfo     = "i"  // Cyan circle - information
```

### Visual Separators
```go
const IconHorizSeparator = "─"  // Horizontal line
const IconVertSeparator  = "│"  // Vertical line
const IconBullet         = "•"  // List bullet point
```

---

## Layout (`layout.go`)

**Spacing and padding constants for consistent formatting**

### Indentation
```go
const IndentBase    = "  "    // 2 spaces - base indentation
const IndentNested  = "    "  // 4 spaces - nested indentation
const IndentDeep    = "      " // 6 spaces - deeply nested
```

### Spacing
```go
const SpaceSingle  = " "     // 1 space - minimal separation
const SpaceDouble  = "  "    // 2 spaces - standard separation
const SpacePadding = "    "  // 4 spaces - visual padding
```

### Line Widths
```go
const WidthNarrow   = 40  // Narrow column width
const WidthStandard = 60  // Standard text width
const WidthWide     = 80  // Wide terminal width
```

---

## Recovery (`recovery.go`)

**Panic recovery for robust error handling**

### recoverFromPanic()

**Internal function protecting all display functions**

```go
func recoverFromPanic()
```

**What it does:**
- Recovers from any panic during formatting
- Logs panic details for debugging
- Prevents crashes in calling code
- Returns empty string on panic (self-evident failure)

**Usage pattern:**
```go
func AnyDisplayFunction() string {
    defer recoverFromPanic()  // Protect against panics
    // ... implementation
}
```

**Why needed:**
- Config loading might panic (invalid JSON)
- String formatting might panic (nil pointers)
- Rails don't return errors (self-evident failures)
- Graceful degradation on panic

---

## Config (`config.go`)

**Configuration loading with tripwire pattern**

### GetConfig()

**Load display configuration with graceful degradation**

```go
func GetConfig() Config
```

**Returns:**
- `Config` struct with icons, colors, layout settings
- Falls back to hardcoded constants on load failure

**Config structure:**
```go
type Config struct {
    Icons struct {
        Status struct {
            Success string
            Failure string
            Warning string
            Info    string
        }
    }
    Colors struct {
        Basic struct {
            Reset string
            Bold  string
            Dim   string
        }
        Foreground struct {
            Red    string
            Green  string
            Yellow string
            Cyan   string
        }
    }
    Layout struct {
        IndentBase   string
        SpaceDouble  string
        WidthStandard int
    }
}
```

**Tripwire pattern:**
```go
cfg := GetConfig()

icon := cfg.Icons.Status.Success
if icon == "" {
    icon = IconSuccess  // Tripwire: fall back to constant
}
```

**Why tripwires:**
- Config loading might fail
- Invalid config returns empty strings
- Detect failure via empty check
- Fall back to known-good constants
- Guarantees valid output

---

## Visual Components (`visual.go`)

### Header()

**Format section header with emphasized styling**

```go
func Header(title string) string
```

**Example:**
```go
fmt.Println(display.Header("Configuration"))
// Output: ═══════════════════════════
//         CONFIGURATION
//         ═══════════════════════════
```

---

### Subheader()

**Format subsection header with lighter emphasis**

```go
func Subheader(title string) string
```

**Example:**
```go
fmt.Println(display.Subheader("System Settings"))
// Output: ─── System Settings ───
```

---

### Box()

**Draw box around content**

```go
func Box(content string) string
```

**Example:**
```go
fmt.Println(display.Box("Important Message"))
// Output: ┌────────────────────┐
//         │ Important Message  │
//         └────────────────────┘
```

---

### Separator()

**Draw horizontal separator line**

```go
func Separator() string
```

**Example:**
```go
fmt.Println(display.Separator())
// Output: ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
```

---

## Structured Output (`structured.go`)

### KeyValue()

**Format key-value pair with aligned formatting**

```go
func KeyValue(key, value string) string
```

**Example:**
```go
fmt.Println(display.KeyValue("Name", "Nova Dawn"))
fmt.Println(display.KeyValue("Status", "Active"))
// Output:   Name: Nova Dawn
//         Status: Active
```

---

### Table()

**Format data as aligned table**

```go
func Table(headers []string, rows [][]string) string
```

**Example:**
```go
headers := []string{"Name", "Status", "Health"}
rows := [][]string{
    {"instance", "loaded", "+131"},
    {"display", "ready", "+100"},
}

fmt.Println(display.Table(headers, rows))
// Output: ┌──────────┬────────┬────────┐
//         │ Name     │ Status │ Health │
//         ├──────────┼────────┼────────┤
//         │ instance │ loaded │ +131   │
//         │ display  │ ready  │ +100   │
//         └──────────┴────────┴────────┘
```

---

### List()

**Format bulleted list**

```go
func List(items []string) string
```

**Example:**
```go
items := []string{"First item", "Second item", "Third item"}
fmt.Println(display.List(items))
// Output: • First item
//         • Second item
//         • Third item
```

---

## Quick Import Reference

```go
import "system/lib/display"

// Messages
display.Success("Done")
display.Failure("Error")
display.Warning("Caution")
display.Info("Notice")

// Visual
display.Header("Section")
display.Subheader("Subsection")
display.Separator()

// Structured
display.KeyValue("Key", "Value")
display.Table(headers, rows)
display.List(items)

// Direct constants
display.Green + "text" + display.Reset
display.BoldCyan + "HEADER" + display.Reset
```

---

## Version History

| Version | Date | Changes |
|---------|------|---------|
| v1.0.0 | 2025-11-21 | All primitives extracted from format.go |
|  |  | 8 primitives: recovery, config, colors, icons, layout, messages, structured, visual |

---

*For detailed API documentation, see individual primitive docs in this directory. For complete usage guide, see [display-api.md](display-api.md).*
