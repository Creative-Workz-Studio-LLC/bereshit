---
name: create-data
description: Create structured data files (configs, settings, glossary entries) using 3-block structure
---

<!--
#!omni document --md
===============================================================================
METADATA BLOCK
===============================================================================
-->
<!--
:key: claude-global-skill-create-data
:title: Create Data Skill
:type: Skill Documentation
:status: Active
:version: a-01.00
:created: 2025-12-11
:updated: 2025-12-11
:authors: Nova Dawn
:purpose: Guide data file creation with 3-block structure (METADATA → CONTENT → CLOSING)
:biblical_foundation: Colossians 2:5 - "For though I be absent in the flesh, yet am I with you in the spirit, joying and beholding your order"
-->

<!--
===============================================================================
HEADER BLOCK
===============================================================================
-->

# Create Data Skill

> *"For though I be absent in the flesh, yet am I with you in the spirit, joying and beholding your order."* — Colossians 2:5

**Create structured data files with consistent 3-block structure.**

---

<!--
===============================================================================
CONTEXT BLOCK
===============================================================================
-->

## Overview

This skill provides the thinking workflow for creating structured data files. Data files use **3-block structure** — simpler than documentation (5-block) or code (4-block) because data serves machines primarily.

| Aspect | What It Means |
|--------|---------------|
| **Structure** | METADATA → CONTENT → CLOSING |
| **Audience** | Machines first, humans second |
| **Formats** | JSONC, JSON, YAML, TOML |

**The principle:** Order enables function. Structured data with consistent patterns enables reliable parsing and validation.

---

## When to Use This Skill

Use `create-data` when you need to:

- Create a new config file (settings, preferences)
- Create a glossary entry (jsonc + adoc pair)
- Create structured data (lookup tables, mappings)
- Reference 3-block structure requirements

**NOT for:** Documentation (use `create-documentation`), code files (use `create-code`), journal entries (use `create-journal-entry`)

---

<!--
===============================================================================
CONTENT BLOCK
===============================================================================
-->

## Data File Types

| Type | Format | When to Use | Strictness |
|------|--------|-------------|------------|
| **config** | JSONC, TOML | Settings, preferences, options | T (Tight) |
| **glossary** | JSONC + ADoc | Vocabulary definitions | T (Tight) |
| **mapping** | JSON, YAML | Lookup tables, translations | G (Guided) |
| **manifest** | JSON | Package/project metadata | T (Tight) |

**Strictness:**
- **T (Tight, +1)** — Follow structure exactly
- **G (Guided, 0)** — Adapt thoughtfully

---

## Decision Tree

```
What is the PRIMARY PURPOSE?
│
├─ Settings/preferences
│  └─ → CONFIG (T) - JSONC or TOML
│
├─ Vocabulary definition
│  └─ → GLOSSARY (T) - JSONC + ADoc pair
│
├─ Lookup/translation table
│  └─ → MAPPING (G) - JSON or YAML
│
└─ Package/project metadata
   └─ → MANIFEST (T) - JSON
```

---

## 3-Block Structure

All data files follow 3-block structure:

```
┌─────────────────────────────────────────────────────────────────┐
│ METADATA   │ Identity and context for the data                 │
├─────────────────────────────────────────────────────────────────┤
│ CONTENT    │ The actual data                                   │
├─────────────────────────────────────────────────────────────────┤
│ CLOSING    │ Validation, checksums, notes (optional)           │
└─────────────────────────────────────────────────────────────────┘
```

| Block | Purpose | Contains |
|-------|---------|----------|
| **METADATA** | Identity | Key, type, status, version, dates, authors, biblical foundation |
| **CONTENT** | The data | Structured data appropriate to the format |
| **CLOSING** | Validation | Schema reference, checksums, end marker (often minimal) |

**Why 3-block for data?** Data serves machines primarily. No HEADER (no human "cover" needed). No CONTEXT (data is self-describing). Simpler structure for simpler purpose.

---

## Creation Process (Block-by-Block)

[IMPORTANT]
====
**Work ONE BLOCK at a time.** Not wholesale. Each block is a unit of work.
====

| Step | Block | What To Do |
|:----:|-------|------------|
| 1 | — | **Determine Type** — Walk the decision tree |
| 2 | — | **Copy Template** — `cp bereshit/word/seed/data/[format]/[type].jsonc path/to/new.jsonc` |
| 3 | — | **Change Pragma** — `#!omni template` → `#!omni data` |
| 4 | METADATA | Fill identity fields |
| 5 | CONTENT | Fill the actual data |
| 6 | CLOSING | Add validation notes if needed |
| 7 | — | **Verify** — All blocks present, valid syntax |

**Why block-by-block?**
- Forces intentional design at each layer
- Enables targeted edits using Linux tools (grep, sed)
- Prevents drift from template structure
- Each block can be validated independently

---

## Template-First Workflow

[IMPORTANT]
====
**NEVER write data files from scratch.** Always derive from templates.
====

| Step | Tool | Why |
|------|------|-----|
| 1. Copy template | `cp` (bash) | Filesystem operation, not Read→Write |
| 2. Change pragma | `Edit` tool | `#!omni template` → `#!omni data` |
| 3. Edit content | `Edit` tool | Surgical changes to copied file |
| 4. Verify syntax | linter/parser | Ensure valid JSON/YAML/TOML |

**The principle:** Templates carry structural DNA. Writing from scratch risks missing blocks, inconsistent structure, and drift from standards.

---

## Comments as Boundary Markers

JSONC uses comment-based markers:

```jsonc
// #!omni data --jsonc
// ===========================================================================
// METADATA BLOCK
// ===========================================================================

// ===========================================================================
// CONTENT BLOCK
// ===========================================================================

// ===========================================================================
// CLOSING BLOCK
// ===========================================================================
```

This enables:

| Tool | Command | Purpose |
|------|---------|---------|
| **grep** | `grep -n "BLOCK" file.jsonc` | Find block line numbers |
| **sed** | `sed -n '/METADATA/,/CONTENT/p' file.jsonc` | Extract specific block |

**Why this matters:**
- **Targeted retrieval** — Extract exactly what's needed
- **Batch operations** — Process multiple files consistently
- **Validation** — Check block presence without full parse

---

## Glossary Entry Pattern

Glossary entries use a **pair pattern** — JSONC for machines, ADoc for humans:

```
term/
├── term.jsonc   # Machine-parseable (3-block data)
└── term.adoc    # Human-readable (5-block documentation)
```

**JSONC file (3-block):**
```jsonc
// #!omni data --jsonc
// GLOSS-CAT-NNN: term

{
  // METADATA
  "key": "GLOSS-CAT-NNN",
  "term": "term",
  "category": "category",
  "status": "active",
  "created": "YYYY-MM-DD",
  "authors": ["Author"],

  // CONTENT
  "definition": "...",
  "biblical_foundation": { "verse": "...", "text": "...", "connection": "..." },
  "what_it_is": [...],
  "what_it_is_not": [...],
  "related_terms": [...],

  // CLOSING
  "companion_doc": "term.adoc"
}
```

---

## Post-Copy Checklist

After copying a template, update these fields:

| Field | Action |
|-------|--------|
| `#!omni` pragma | Change `template` → `data` |
| `key` | Unique identifier for THIS data |
| `type`/`category` | Classification |
| `version` | Reset to `a-01.00` for new files |
| `created`/`updated` | Today's date |
| Content | Fill in actual data values |

---

## Format-Specific Notes

### JSONC (JSON with Comments)
- Use `//` for comments
- Trailing commas allowed
- Block markers in comments

### YAML
- Use `#` for comments
- Indentation-sensitive
- Natural for nested data

### TOML
- Use `#` for comments
- Section headers with `[section]`
- Good for flat configs

---

## Templates

| Type | Template Path |
|------|---------------|
| **config** | `bereshit/word/seed/data/jsonc/config.jsonc` |
| **glossary** | `bereshit/word/seed/data/jsonc/glossary-entry.jsonc` |
| **mapping** | `bereshit/word/seed/data/json/mapping.json` |

---

<!--
===============================================================================
FOOTER BLOCK
===============================================================================
-->

## Configuration

| File | Purpose |
|------|---------|
| `config/data-config.toml` | Data types, formats, validation rules |
| `references/data-patterns.md` | Complete guide with patterns |

---

## Related Skills

| Skill | Relationship |
|-------|--------------|
| [create-documentation](../create-documentation/) | For docs (5-block), not data |
| [create-code](../create-code/) | For code (4-block), not data |
| [validate-omni](../validate-omni/) | Validate OmniCode structure |

---

> *"For God is not the author of confusion, but of peace."* — 1 Corinthians 14:33
