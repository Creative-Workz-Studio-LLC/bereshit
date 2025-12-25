---
description: Show how OmniCode elements map to a target format. Uses format-lookup skill for quick lookups, Phoebe (format-bridge) for deep analysis.
---

# Format Mapping

Show how OmniCode maps to a target format.

## Task

The user wants to understand how OmniCode elements translate to a specific format.

**Arguments:** $ARGUMENTS
- Format: `[format]` for quick lookup
- Format: `[file] --[format]` for file-specific analysis
- No args: ask user

## Instructions

### Quick Lookup (just format name)

1. Use **format-lookup skill** reference tables:

   For `adoc`:
   ```
   grounded in: scripture → :biblical_foundation:
   exists to: purpose → :purpose:
   etc.
   ```

   For `go`:
   ```
   grounded in: scripture → // Biblical: comment
   exists to: purpose → package doc first line
   etc.
   ```

2. Show the mapping table for requested format

### Deep Analysis (file + format)

1. Spawn **Phoebe** (format-bridge agent) with:
   ```
   Analyze format mapping for: [file]
   Target format: [format]

   1. Read the OmniCode file
   2. For each element, show the format equivalent
   3. Identify any gaps (unmapped elements)
   4. Show what the transpiled output would look like
   ```

2. Return comprehensive mapping analysis

## Example Usage

```
/format-map adoc                    # Quick AsciiDoc lookup
/format-map go                      # Quick Go lookup
/format-map root.omni --adoc        # Deep analysis of specific file
/format-map                         # Will ask what format
```

## Supported Formats

| Format | Status | Quick Lookup | Deep Analysis |
|--------|--------|--------------|---------------|
| adoc | Complete | Yes | Yes |
| go | Complete | Yes | Yes |
| md | Planned | Partial | Yes |
| c | Planned | Partial | Yes |
| rs | Planned | Partial | Yes |

## Output Format

### Quick Lookup

```markdown
## OmniCode → [Format] Mapping

### METADATA
| OmniCode | [Format] |
|----------|----------|
| `grounded in:` | [equivalent] |
| `serves as` | [equivalent] |
...

### BLOCKS
| Block | [Format] |
|-------|----------|
| METADATA | [how expressed] |
...
```

### Deep Analysis

```markdown
## Format Analysis: [file] → [format]

### Element-by-Element Mapping
[detailed mapping table]

### Gaps Found
[unmapped elements if any]

### Transpiled Preview
[what output would look like]
```
