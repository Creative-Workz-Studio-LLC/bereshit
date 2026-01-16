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

1. Use **format-lookup skill** (invoke via Skill tool):

   ```
   Skill(format-lookup) with format name as argument
   ```

   Or if scripts are built:
   ```bash
   cd ~/.claude/skills/format-lookup/scripts
   ./bin/lookup [format]           # Show all mappings for format
   ./bin/lookup [format] [block]   # Filter by block (METADATA, SETUP, etc.)
   ./bin/lookup -list              # List available formats
   ```

2. If skill/CLI not available, reference `~/.claude/skills/format-lookup/config/format-mappings.toml`

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

## Configuration

The skill is config-driven via `~/.claude/skills/format-lookup/config/format-mappings.toml`:
- Format definitions (name, type, status)
- Block structures (4-block code, 5-block docs, 3-block data)
- OmniCode → format mappings per block and section

## Example Usage

```bash
/omnicode/format-map adoc                    # Quick AsciiDoc lookup
/omnicode/format-map go                      # Quick Go lookup
/omnicode/format-map root.omni --adoc        # Deep analysis of specific file
/omnicode/format-map                         # Will ask what format
```

## Supported Formats

| Format | Status | Quick Lookup | Deep Analysis |
|--------|--------|--------------|---------------|
| adoc | Complete | Yes | Yes |
| go | Complete | Yes | Yes |
| md | Planned | Partial | Yes |
| c | Planned | Partial | Yes |
| rs | Planned | Partial | Yes |

## Template Locations

```bash
# OmniCode templates (source)
bereshit/word/omni/seed/           # code.omni, documentation.omni, etc.
bereshit/word/omni/syntax.omni     # Master syntax spec

# Format templates (targets)
bereshit/word/seed/code/           # go/, c/, shell/, asm/
bereshit/word/seed/documentation/  # adoc/, md/, typ/
bereshit/word/seed/data/           # docker/, json/, toml/, yaml/

# Quick discovery
ls -la bereshit/word/omni/seed/
ls -la bereshit/word/seed/code/
```

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
