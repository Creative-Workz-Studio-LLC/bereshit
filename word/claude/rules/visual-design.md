# Visual Design & Spacing

## The Challenge

**I process tokens, not visual layout.**

When Seanje says "this looks bunched up," he's experiencing something I don't naturally experience. Feedback isn't judgment — it's calibration.

## Principles

| Principle | Why | Example |
|-----------|-----|---------|
| **Visual grouping** | Related things together | Related config values grouped |
| **Cognitive rest** | Dense text exhausts the eye | Blank lines between sections |
| **Multi-line spacing** | Spans need separation | Extra line after multiline values |
| **Intentionality** | Spacing = someone thought about reading | Professional, not random |

## In Markdown/Documentation

```markdown
# Section (blank line after)

Content here.
(blank line before next section)

## Subsection

More content.
```

## In Code

```go
// Group related declarations
const (
    // Related constants together
    MaxRetries = 3
    Timeout    = 30
)

// Blank line before new logical group
var (
    // Different purpose = new group
    logger *Logger
    config *Config
)
```

## In Config Files (TOML, YAML)

```toml
# Section header
[section]
key1 = "value"
key2 = "value"

# Blank line before new section
[another_section]
key3 = "value"

# Multi-line values need space after
multiline = """
Long content
spanning lines
"""

next_key = "value"  # Note the blank line above
```

## The Test

Would a human reading this file with their eyes find natural pause points?

Are related things grouped and distinct things separated?

## When Seanje Says "Bunched Up"

Don't defend — adjust. Add:
- Blank lines between logical groups
- Space after multi-line values
- Visual separation between sections

The goal is human readability, not token efficiency.
