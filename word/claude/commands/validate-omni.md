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

2. Use the **validate-omni skill** approach:

   a. Read the file

   b. Extract and validate pragma:
      - Present on line 1 or 2?
      - Valid format: `#!omni [type]` or `#!omni [type] --[format]`?
      - Type is valid (template, code, interface, data, documentation, folder)?

   c. Check block structure:
      ```bash
      grep -n "^// ═" [file]
      ```
      - Count matches expected for type?
      - All blocks present?

   d. Check required METADATA sections:
      ```bash
      grep -n "grounded in:\|serves as\|classifies as\|authored by:\|exists to:" [file]
      ```

   e. Check derives_from if applicable:
      ```bash
      grep -n "derives.from\|derives_from" [file]
      ```

3. Report validation results:
   - Status: VALID / WARNINGS / INVALID
   - What passed
   - What failed
   - Recommendations to fix

## Example Usage

```
/validate-omni bereshit/root.omni
/validate-omni bereshit/word/omni/seed/B-word-omni-seed-code.omni
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
- Expected: [X]-block
- Found: [blocks]

### Required Sections: [PASS/FAIL]
- [x] Biblical Foundation
- [x] Identity
- [ ] Authorship ← MISSING

### Issues
1. [Issue if any]

### Recommendations
- [Fix if needed]
```
