---
name: propagate-change
description: Find all files affected by a template change and show what needs updating. Use this skill when Nova Dawn has edited a template and needs to know what downstream files need corresponding updates - traces derives_from chains and format mappings.
version: "2.0.0"
allowed-tools: "Read,Glob,Grep,Bash"
---

<!--
#!omni document --md
===============================================================================
METADATA BLOCK
===============================================================================
-->
<!--
:key: claude-global-skills-propagate-change
:title: Propagate Change
:type: Skill
:status: Active
:version: 2.0.0
:created: 2025-11-15
:updated: 2025-12-08
:authors: Nova Dawn
:purpose: Find downstream files affected by template changes - trace derives_from chains
:category: omnicode
:biblical_foundation: Galatians 5:9 - "A little leaven leaveneth the whole lump"
-->

<!--
===============================================================================
HEADER BLOCK
===============================================================================
-->

# Propagate Change

> *"A little leaven leaveneth the whole lump."* — Galatians 5:9

**When a template changes, find everything that needs to change with it.**

---

**Navigation:**

| Section | Purpose |
|---------|---------|
| [When to Use](#when-to-use-this-skill) | Decision criteria |
| [Propagation Chain](#the-propagation-chain) | How changes flow |
| [Propagation Process](#propagation-process) | Step-by-step |
| [Scripts](#scripts) | Go chain tracers |
| [Common Scenarios](#common-scenarios) | Practical examples |

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
| Finding affected files | Deep structural comparison (use template-chain-analyzer) |
| Tracing derives_from chains | Creating new files (use create-from-template) |
| Impact assessment | Validating single files (use validate-omni) |

### Prerequisites

| Requirement | Reference |
|-------------|-----------|
| OmniCode chain understanding | bereshit/word/omni/seed/ |
| Template locations | bereshit/word/seed/ |

### Key Terms

| Term | Definition |
|------|------------|
| **derives_from** | Chain reference pointing to parent template |
| **Propagation** | How changes flow down the template hierarchy |
| **Cascade** | Full impact of a change through all levels |

---

<!--
===============================================================================
CONTENT BLOCK
===============================================================================
-->

## When to Use This Skill

**Use when:**

- After editing a template, before finishing
- Planning a structural change to understand impact
- Checking what derives from a specific template
- Ensuring chain consistency after updates

**Do NOT use for:**

- Deep structural comparison → use Selah (template-chain-analyzer agent)
- Creating new files → use create-from-template skill
- Validating single files → use validate-omni skill

---

## The Propagation Chain

```text
SYNTAX SPEC (master)
    ↓ changes propagate to
UNIVERSAL TEMPLATE
    ↓ changes propagate to
SPECIALIZED TEMPLATES (code, documentation, interface, folder, data)
    ↓ changes propagate to
FORMAT TEMPLATES (adoc, go, c, etc.)
    ↓ changes propagate to
ACTUAL DOCUMENTS (root.adoc, main.go, etc.)
```

**Direction matters:**

- Changes flow DOWN from syntax spec
- Fixes may need to flow UP to align with spec

---

## Propagation Process

### Step 1: Identify Changed File's Position

```text
What was edited?
    │
    ├── Syntax spec ───────► Everything below needs checking
    │
    ├── Universal template ─► Specialized + Format + Documents
    │
    ├── Specialized template ► Format templates + Documents of this type
    │
    ├── Format template ────► Documents using this format
    │
    └── Actual document ────► Nothing (leaf node)
```

### Step 2: Find Direct Descendants

```bash
# Use propagate tool
cd claude-global/skills/propagate-change/scripts
./bin/propagate descendants TEMPLATE_NAME

# Or bash commands
grep -rl "TEMPLATE_NAME" bereshit/

# Count direct descendants
grep -rl "TEMPLATE_NAME" bereshit/ | wc -l
```

### Step 3: Find Format Mappings

```bash
# Check if changed file has FORMAT DEFINITIONS
grep -q "format definitions:" CHANGED_FILE.omni && echo "Has format definitions"

# Find format templates implementing this
find bereshit/word/seed/ -type f | xargs grep -l "derives.*TEMPLATE_NAME"
```

### Step 4: Build Cascade Report

```bash
# Full cascade discovery with propagate tool
cd claude-global/skills/propagate-change/scripts
./bin/propagate report TEMPLATE_NAME

# JSON output for scripting
./bin/propagate report TEMPLATE_NAME -json

# Or manual commands
echo "=== PROPAGATION REPORT ==="
echo "Changed: TEMPLATE_NAME"
echo ""
echo "=== Direct Descendants (Level 1) ==="
grep -rl "TEMPLATE_NAME" bereshit/ | head -20
```

---

## Scripts

The `propagate` CLI tool in `scripts/`:

| Command | Purpose | Usage |
|---------|---------|-------|
| `trace` | Trace derives_from chain up to root | `./bin/propagate trace FILE.omni` |
| `descendants` | Find files deriving from template | `./bin/propagate descendants TEMPLATE` |
| `report` | Full propagation report (chain + descendants) | `./bin/propagate report FILE.omni` |

### Building

```bash
cd claude-global/skills/propagate-change/scripts
make build
# Binary: bin/propagate
```

### Structure

```text
scripts/
├── cmd/propagate/main.go  ← CLI orchestrator
├── pkg/chain/chain.go     ← Chain tracing logic
├── pkg/config/config.go   ← Config loader
├── bin/propagate          ← Built binary
├── Makefile               ← Build orchestration
└── go.mod                 ← Module dependencies
```

### Configuration

Chain tracing is config-driven via `config/chain-config.toml`:

- **Hierarchy levels**: syntax-spec → universal → specialized → format → document
- **derives_from patterns**: Multiple markers recognized
- **Template locations**: Points to bereshit/word/ structure
- **Propagation rules**: What changes affect what levels

---

## Output Format

```markdown
# Propagation Report: [changed-file]

## Changed File
- Path: [path/to/changed/file]
- Type: [syntax-spec | universal | specialized | format | document]
- Position in chain: [level]

## Direct Descendants (Level 1)

| File | Type | derives_from |
|------|------|--------------|
| path/to/file1 | specialized | this file |
| path/to/file2 | format | this file |

## Total Impact

- Direct descendants: [count]
- Indirect descendants: [count]
- **Total files to review:** [total]

## Recommended Update Order

1. [file] - Update [specific sections]
2. [file] - Verify alignment
```

---

## Common Scenarios

### Scenario 1: Changed Syntax Spec

**Impact:** Everything
**Update order:**

1. Universal template
2. All specialized templates
3. All format templates
4. Spot-check documents

### Scenario 2: Changed Specialized Template

**Impact:** Format templates of that type, documents of that type
**Update order:**

1. Format templates (e.g., adoc, md for documentation)
2. Documents using those templates

### Scenario 3: Changed Format Template

**Impact:** Documents using that format
**Update order:**

1. All documents with derives_from pointing to this template

---

## Chain Reference

```text
bereshit/word/omni/seed/
├── universal.omni              ← Master template
├── code.omni                   ← 4-block code template
├── documentation.omni          ← 5-block docs template
├── interface.omni              ← 3-block interface template
├── folder.omni                 ← 3-block folder template
└── data.omni                   ← Data template

bereshit/word/seed/
├── code/go/                    ← Go format templates
├── documentation/asciidoc/     ← AsciiDoc format templates
└── documentation/markdown/     ← Markdown format templates
```

---

<!--
===============================================================================
FOOTER BLOCK
===============================================================================
-->

## Related Documents

| Document | Relationship |
|----------|--------------|
| bereshit/word/omni/seed/ | OmniCode templates |
| validate-omni skill | Validate before propagating |
| format-lookup skill | Format mappings for propagation |

---

> *"For which of you, intending to build a tower, sitteth not down first, and counteth the cost?"* — Luke 14:28

**Count the cost of change.** Know what will be affected. Then proceed with wisdom.
