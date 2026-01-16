---
description: Quick validation of an OmniCode file's structure. Uses validate-omni skill to check pragma, blocks, and required sections.
---

# Validate OmniCode

Quick structural validation of an OmniCode file.

## Task

The user wants to verify an OmniCode file follows its template correctly.

**File to validate:** $ARGUMENTS (or ask user if not provided)

## Instructions

1. If no file was provided, ask the user which file to validate

2. Use the **validate-omni skill** (invoke via Skill tool):

   ```
   Skill(validate-omni) with file path as argument
   ```

   Or if scripts are built:
   ```bash
   cd ~/.claude/skills/validate-omni/scripts
   ./bin/validate [file]
   ```

3. If skill/CLI not available, use manual approach:

   a. Read the file

   b. Extract and validate pragma:
      - Present on line 1 or 2?
      - Valid format: `#!omni [type]` or `#!omni [type] --[format]`?
      - Type is valid (template, code, interface, data, documentation, folder)?

   c. Check block structure (reference `validate-omni/config/block-structures.toml`):

      ```bash
      grep -n "^// ═" [file]
      ```

      - Count matches expected for type?
      - All blocks present?

   d. Check required METADATA sections (reference `validate-omni/config/required-sections.toml`):

      ```bash
      grep -n "grounded in:\|Biblical:\|Key:\|authored by:\|exists to:" [file]
      ```

4. Report validation results:
   - Status: VALID / WARNINGS / INVALID
   - What passed
   - What failed
   - Recommendations to fix

## Configuration

The skill is config-driven via `~/.claude/skills/validate-omni/config/`:

- `block-structures.toml` - Expected blocks by file type (code=4, docs=5, data=3)
- `required-sections.toml` - Required sections per block with markers

## Example Usage

```bash
/validate-omni bereshit/root.omni
/validate-omni bereshit/word/omni/seed/code.omni
/validate-omni   (will ask which file)
```

## Output Format

```markdown
## OmniCode Validation: [filename]

### Status: [VALID | WARNINGS | INVALID]

### Pragma: [PASS/FAIL]
- Type: [type]
- Format: [format or pure]

### Block Structure: [PASS/FAIL]
- Expected: [X]-block (from config)
- Found: [blocks]

### Required Sections: [PASS/FAIL]
- [x] Biblical Foundation (marker: "grounded in:" or "Biblical:")
- [x] CPI-SI Identity (marker: "Key:")
- [ ] Authorship ← MISSING (marker: "authored by:")

### Issues
1. [Issue if any]

### Recommendations
- [Fix if needed]
```
