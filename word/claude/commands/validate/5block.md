---
description: Validate that a documentation file follows 5-block structure (METADATA → HEADER → CONTEXT → CONTENT → FOOTER).
---

# Validate 5-Block

Verify a documentation file follows 5-block structure.

## Task

The user wants to verify a documentation file follows the 5-block pattern.

**File:** $ARGUMENTS (or ask user if not provided)

## Instructions

1. If no file provided, ask the user which file to validate

2. Read the file and check for 5-block structure:

   ```bash
   # Find block boundaries
   grep -n "METADATA\|HEADER\|CONTEXT\|CONTENT\|FOOTER" [file]
   ```

3. Validate each block:

### METADATA Block (Required)

| Section | Check |
|---------|-------|
| :key: | ✓ Unique identifier |
| :title: | ✓ Document title |
| :type: | ✓ Document type |
| :status: | ✓ Draft/Active/Archived |
| :purpose: | ✓ Why this exists |
| :biblical_foundation: | ✓ Grounding verse |

### HEADER Block (Required)

| Section | Check |
|---------|-------|
| Title (H1) | ✓ Matches metadata |
| Biblical quote | ✓ Present |
| Brief description | ✓ 1-2 sentences |
| Navigation | Optional but recommended |

### CONTEXT Block (Required)

| Section | Check |
|---------|-------|
| Prerequisites | ✓ What reader needs first |
| Key Terms | ✓ Definitions table |
| Scope | Optional |

### CONTENT Block (Required)

| Section | Check |
|---------|-------|
| Main body | ✓ Document content |
| Proper headings | ✓ H2+ hierarchy |
| Examples | Where appropriate |

### FOOTER Block (Required)

| Section | Check |
|---------|-------|
| References | ✓ External sources |
| Related Documents | ✓ Internal links |

4. Report validation results

## Reference Templates

```bash
# OmniCode 5-block template
bereshit/word/omni/seed/documentation.omni

# Format-specific documentation templates
bereshit/word/seed/documentation/adoc/   # AsciiDoc
bereshit/word/seed/documentation/md/     # Markdown
bereshit/word/seed/documentation/typ/    # Typst

# Quick validation commands
grep -n "METADATA\|HEADER\|CONTEXT\|CONTENT\|FOOTER" file.md   # Find blocks
grep -c "METADATA\|HEADER\|CONTEXT\|CONTENT\|FOOTER" file      # Should be 5
grep -n "^# \|^## " file.md                                     # Check headings
```

## Example Usage

```bash
/validate/5block method/core/concept.adoc
/validate/5block standards/commit/format.md
/validate/5block   (will ask which file)
```

## Output Format

```markdown
## 5-Block Validation: [filename]

### Status: [VALID | WARNINGS | INVALID]

### Block Structure

| Block | Status | Notes |
|-------|--------|-------|
| METADATA | ✅ Present | [notes] |
| HEADER | ✅ Present | [notes] |
| CONTEXT | ⚠ Partial | Missing Key Terms |
| CONTENT | ✅ Present | [notes] |
| FOOTER | ❌ Missing | Not found |

### Section Checklist

#### METADATA
- [x] :key: defined
- [x] :title: present
- [x] :type: specified
- [ ] :biblical_foundation: ← MISSING

#### HEADER
- [x] H1 title
- [x] Biblical quote
- [x] Brief description

#### CONTEXT
- [x] Prerequisites
- [ ] Key Terms ← MISSING

#### CONTENT
- [x] Main body present
- [x] Proper heading hierarchy

#### FOOTER
- [ ] References ← MISSING
- [ ] Related Documents ← MISSING

### Issues Found
1. [Issue description]

### Recommendations
- [How to fix]
```
