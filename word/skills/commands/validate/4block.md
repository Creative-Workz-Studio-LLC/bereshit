---
description: Validate that a code file follows 4-block structure (METADATA → SETUP → BODY → CLOSING).
---

# Validate 4-Block

Verify a code file follows 4-block structure.

## Task

The user wants to verify a code file follows the 4-block pattern.

**File:** $ARGUMENTS (or ask user if not provided)

## Instructions

1. If no file provided, ask the user which file to validate

2. Read the file and check for 4-block structure:

   ```bash
   # Find block boundaries
   grep -n "METADATA\|SETUP\|BODY\|CLOSING" [file]
   ```

3. Validate each block:

### METADATA Block (Required)

| Section | Check |
|---------|-------|
| Package/Module | ✓ Declared |
| Purpose | ✓ Stated |
| Biblical grounding | ✓ Present |
| Author | ✓ Listed |
| Health Scoring | ✓ Core Identity, Interface, Operational |

### SETUP Block (Required)

| Section | Check |
|---------|-------|
| Imports | ✓ Organized (std, external, internal) |
| Constants | ✓ Present if needed |
| Variables | ✓ Present if needed |
| Types | ✓ Defined before use |

### BODY Block (Required)

| Section | Check |
|---------|-------|
| Helpers | ✓ Internal functions first |
| Core Operations | ✓ Main logic |
| Public APIs | ✓ Exported functions last |

### CLOSING Block (Required)

| Section | Check |
|---------|-------|
| Validation | ✓ Input/output validation |
| Execution | ✓ Entry points (main, init) |
| Cleanup | ✓ Resource cleanup |

4. Report validation results

## Reference Templates

```bash
# OmniCode 4-block template
bereshit/word/omni/seed/code.omni

# Format-specific code templates
bereshit/word/seed/code/go/      # Go
bereshit/word/seed/code/c/       # C
bereshit/word/seed/code/shell/   # Shell

# Quick validation commands
grep -n "^// ═" file.go                              # Find block boundaries
grep -E "^// (METADATA|SETUP|BODY|CLOSING)" file.go  # Check block markers
grep -c "METADATA\|SETUP\|BODY\|CLOSING" file        # Should be 4
```

## Example Usage

```bash
/validate/4block parser.go
/validate/4block lib/health/scorer.c
/validate/4block   (will ask which file)
```

## Output Format

```markdown
## 4-Block Validation: [filename]

### Status: [VALID | WARNINGS | INVALID]

### Block Structure

| Block | Status | Notes |
|-------|--------|-------|
| METADATA | ✅ Present | [notes] |
| SETUP | ✅ Present | [notes] |
| BODY | ⚠ Partial | Missing Helpers section |
| CLOSING | ❌ Missing | Not found |

### Section Checklist

#### METADATA
- [x] Package declared
- [x] Purpose stated
- [ ] Biblical grounding ← MISSING
- [x] Health scoring

#### SETUP
- [x] Imports organized
- [x] Constants
- [x] Types

#### BODY
- [ ] Helpers ← MISSING
- [x] Core Operations
- [x] Public APIs

#### CLOSING
- [x] Validation
- [x] Execution
- [ ] Cleanup ← MISSING

### Issues Found
1. [Issue description]

### Recommendations
- [How to fix]
```
