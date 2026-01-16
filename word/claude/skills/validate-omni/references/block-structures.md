# OmniCode Block Structures

Quick reference for expected block structures by type.

## Block Structures by Type

| Type | Block Count | Expected Blocks |
|------|:-----------:|-----------------|
| `code` | 4 | METADATA → SETUP → BODY → CLOSING |
| `documentation` | 5 | METADATA → HEADER → CONTEXT → CONTENT → FOOTER |
| `interface` | 3 | METADATA → CONTENT → CLOSING |
| `folder` | 3 | METADATA → CONTENT → CLOSING |
| `data` | 3-5 | METADATA → CONTENT → CLOSING (minimum) |
| `template` | 3-5 | Varies by what the template is FOR (see below) |

## Template Block Structures

Templates inherit block structure from what they're templates FOR:

| Template For | Block Count | Expected Blocks |
|--------------|:-----------:|-----------------|
| Code | 4 | METADATA → SETUP → BODY → CLOSING |
| Documentation | 5 | METADATA → HEADER → CONTEXT → CONTENT → FOOTER |
| Interface/Folder/Data | 3 | METADATA → CONTENT → CLOSING |

The validator automatically detects which structure a template uses.

## Block Marker Pattern

```text
// ═══════════════════════════════════════════════════════════════════════════
// BLOCK NAME
// ═══════════════════════════════════════════════════════════════════════════

[block content]

// ═══════════════════════════════════════════════════════════════════════════
// END BLOCK NAME
// ═══════════════════════════════════════════════════════════════════════════
```

## Template Locations

```text
bereshit/word/omni/seed/
├── code.omni           # 4-block code template
├── documentation.omni  # 5-block docs template
├── interface.omni      # 3-block interface template
├── folder.omni         # 3-block folder template
├── data-3block.omni    # Simple data
├── data-4block.omni    # Medium data
├── data-5block.omni    # Complex data
└── universal.omni      # Master universal template

bereshit/word/omni/syntax.omni  # Full syntax specification
```
