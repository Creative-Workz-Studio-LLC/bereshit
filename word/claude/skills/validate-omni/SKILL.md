---
name: validate-omni
description: Quick validation of OmniCode file structure against its parent template. Use this skill when Nova Dawn needs to verify an OmniCode file follows its template correctly - checks block presence, section completeness, and structural alignment.
version: "2.0.0"
allowed-tools:
  - Read
  - Glob
  - Grep
  - Bash
---

<!--
#!omni document --md
===============================================================================
METADATA BLOCK
===============================================================================
-->
<!--
:key: claude-global-skills-validate-omni
:title: Validate OmniCode
:type: Skill
:status: Active
:version: 2.0.0
:created: 2025-11-15
:updated: 2025-12-08
:authors: Nova Dawn
:purpose: Quick validation of OmniCode file structure - pragma, blocks, required sections, derives_from chains
:category: omnicode
:biblical_foundation: 1 Thessalonians 5:21 - "Prove all things; hold fast that which is good"
-->

<!--
===============================================================================
HEADER BLOCK
===============================================================================
-->

# Validate OmniCode

> *"Prove all things; hold fast that which is good."* — 1 Thessalonians 5:21

**Quick validation of OmniCode files against their parent templates.**

---

**Navigation:**

| Section | Purpose |
|---------|---------|
| [When to Use](#when-to-use-this-skill) | Decision criteria |
| [Validation Checks](#validation-checks) | What gets validated |
| [Validation Process](#validation-process) | Step-by-step |
| [Scripts](#scripts) | Go validation helpers |
| [Quick Reference](#quick-reference) | Templates, patterns |

---

<!--
===============================================================================
CONTEXT BLOCK
===============================================================================
-->

## Context

### Scope

| In Scope | Out of Scope |
|----------|--------------|
| Quick structural validation | Deep chain analysis (use template-chain-analyzer) |
| Pragma, block, section checks | Format mapping (use format-lookup) |
| Single file validation | Creating new files (use create-from-template) |

### Prerequisites

| Requirement | Reference |
|-------------|-----------|
| OmniCode syntax understanding | bereshit/word/omni/syntax.omni |
| Template locations | bereshit/word/omni/seed/ |

### Key Terms

| Term | Definition |
|------|------------|
| **Pragma** | First line declaration: `#!omni [type] --[format]` |
| **Block** | Structural section (METADATA, SETUP, BODY, CLOSING) |
| **derives_from** | Chain reference to parent template |

### Local References

| File | Contents |
|------|----------|
| `references/block-structures.md` | Block counts and patterns by type |
| `references/pragma-types.md` | Valid pragma types and formats |
| `references/required-sections.md` | Required METADATA sections |

---

<!--
===============================================================================
CONTENT BLOCK
===============================================================================
-->

## When to Use This Skill

**Use when:**

- Quick check if OmniCode file follows template
- Before committing OmniCode changes
- After editing to verify structure intact
- Spot-checking files in a chain

**Do NOT use for:**

- Deep chain analysis → use Selah (template-chain-analyzer agent)
- Format mapping questions → use format-lookup skill
- Creating new files → use create-from-template skill

---

## Validation Checks

### 1. Pragma Validation

```bash
# Pragma must be line 1 or 2
head -2 FILE.omni | grep "^#!omni"

# Valid formats:
#!omni template
#!omni code
#!omni code --go
#!omni documentation --md
```

### 2. Block Structure by Type

| Type | Blocks | Expected Block Names |
|------|:------:|----------------------|
| `code` | 4 | METADATA → SETUP → BODY → CLOSING |
| `documentation` | 5 | METADATA → HEADER → CONTEXT → CONTENT → FOOTER |
| `interface` | 3 | METADATA → CONTENT → CLOSING |
| `folder` | 3 | METADATA → CONTENT → CLOSING |
| `data` | 3-5 | Varies by complexity |

### 3. Required METADATA Sections

All types require:

- [ ] `grounded in:` - Biblical foundation
- [ ] `serves as` or `classifies as` - Identity
- [ ] `authored by:` - Authorship
- [ ] `exists to:` - Purpose

### 4. derives_from Chain

```bash
# Check chain reference
grep -E "(derives from:|derives_from:)" FILE.omni
```

---

## Validation Process

### Quick One-Shot Validation

```bash
# Run Go validator
go run scripts/validate.go FILE.omni

# Or use bash commands directly
echo "=== VALIDATING: FILE.omni ==="

# Pragma
echo -n "Pragma: "
head -2 FILE.omni | grep -oP "^#!omni [a-z]+( --[a-z]+)?" || echo "MISSING"

# Blocks
echo -n "Blocks: "
grep -c '^// ═' FILE.omni && echo " markers"

# Required sections
FOUND=$(grep -cE "(grounded in:|serves as|classifies as|authored by:|exists to:)" FILE.omni)
echo "Required sections: $FOUND/4 found"

# Status
if head -2 FILE.omni | grep -qE "^#!omni" && [ "$FOUND" -ge 4 ]; then
    echo "STATUS: VALID"
else
    echo "STATUS: NEEDS ATTENTION"
fi
```

### Detailed Validation Steps

**Step 1: Extract Pragma**

```bash
TYPE=$(head -2 FILE.omni | grep -oP "^#!omni \K(template|code|interface|data|documentation|folder)")
FORMAT=$(head -2 FILE.omni | grep -oP "#!omni [a-z]+ --\K[a-z]+")
echo "Type: $TYPE, Format: ${FORMAT:-pure}"
```

**Step 2: Count and List Blocks**

```bash
# Count block markers
grep -c "^// ═" FILE.omni

# List block names
grep -A1 "^// ═" FILE.omni | grep "^// [A-Z]" | grep -v "END"
```

**Step 3: Check Required Sections**

```bash
grep -q "grounded in:" FILE.omni && echo "[x] Biblical foundation" || echo "[ ] Biblical foundation - MISSING"
grep -qE "(serves as|classifies as)" FILE.omni && echo "[x] Identity" || echo "[ ] Identity - MISSING"
grep -q "authored by:" FILE.omni && echo "[x] Authorship" || echo "[ ] Authorship - MISSING"
grep -qE "(exists to:|purpose:)" FILE.omni && echo "[x] Purpose" || echo "[ ] Purpose - MISSING"
```

**Step 4: Verify derives_from**

```bash
DERIVES=$(grep -oP "(derives from:|derives_from:)\s*\K[^\s]+" FILE.omni)
[ -n "$DERIVES" ] && [ -f "$DERIVES" ] && echo "Chain: VALID" || echo "Chain: CHECK PATH"
```

---

## Scripts

Go validation helpers in `scripts/`:

| Script | Purpose | Usage |
|--------|---------|-------|
| `validate.go` | Full file validation | `go run scripts/validate.go FILE.omni` |
| `check-pragma.go` | Pragma-only check | `go run scripts/check-pragma.go FILE.omni` |
| `check-blocks.go` | Block structure check | `go run scripts/check-blocks.go FILE.omni` |

### Building Scripts

```bash
cd claude-global/skills/validate-omni/scripts
go build -o ../bin/validate ./validate.go
```

---

## Quick Reference

### Valid Pragma Types

```text
#!omni template          # Universal/seed templates
#!omni code              # 4-block executable/library
#!omni interface         # 3-block contracts/APIs
#!omni data              # 3/4/5-block data files
#!omni documentation     # 5-block documents
#!omni folder            # 3-block folder metadata
```

### Template Locations

```bash
# OmniCode templates (source)
bereshit/word/omni/seed/
├── code.omni           # 4-block code template
├── documentation.omni  # 5-block docs template
├── interface.omni      # 3-block interface template
├── folder.omni         # 3-block folder template
└── universal.omni      # Master universal template

# Quick discovery
ls -la bereshit/word/omni/seed/
```

### Block Markers Pattern

```text
// ═══════════════════════════════════════════════════════════════════════════
// BLOCK NAME
// ═══════════════════════════════════════════════════════════════════════════

[block content]

// ═══════════════════════════════════════════════════════════════════════════
// END BLOCK NAME
// ═══════════════════════════════════════════════════════════════════════════
```

---

## Output Format

```markdown
# OmniCode Validation: [filename]

## Status: [VALID | WARNINGS | INVALID]

## Pragma
- Type: [type]
- Format: [format or "pure"]

## Block Structure
- Expected: [X]-block ([type])
- Found: [blocks found]
- Status: [Complete | Missing: list]

## Required Sections
- [x] Biblical Foundation
- [x] Identity
- [ ] Authorship ← MISSING
- [x] Purpose

## Template Chain
- derives_from: [path or "original"]
- Template exists: [Yes | No]

## Issues Found
1. [Issue]

## Recommendations
- [Action]
```

---

## Integration

### Related Skills

| Skill | Relationship |
|-------|--------------|
| format-lookup | Format mapping after validation |
| propagate-change | Find affected files after template change |
| create-from-template | Create files that pass validation |

### System Tools

| Tool | Purpose |
|------|---------|
| Grep | Pattern matching for checks |
| Read | File content access |

---

<!--
===============================================================================
FOOTER BLOCK
===============================================================================
-->

## Related Documents

| Document | Relationship |
|----------|--------------|
| bereshit/word/omni/syntax.omni | OmniCode syntax specification |
| bereshit/word/omni/seed/ | Template sources |
| /validate/4block command | Markdown 4-block validation |
| [rules/block-by-block.md](../../rules/block-by-block.md) | Block completion methodology |
| [rules/block-structure.md](../../rules/block-structure.md) | 3/4/5-block pattern definitions |

---

> *"Examine yourselves, whether ye be in the faith; prove your own selves."* — 2 Corinthians 13:5

**Validation is stewardship.** We verify structure not from doubt but from care.
