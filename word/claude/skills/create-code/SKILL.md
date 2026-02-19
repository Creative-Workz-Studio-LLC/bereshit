---
name: create-code
description: Create structured code files (Go, Bash, Python) using 4-block structure
version: "1.0.0"
allowed-tools:
  - Read
  - Glob
  - Grep
  - Write
  - Bash
---

<!--
#!omni document --md
===============================================================================
METADATA BLOCK
===============================================================================
-->
<!--
:key: claude-global-skill-create-code
:title: Create Code Skill
:type: Skill Documentation
:status: Active
:version: a-01.00
:created: 2025-12-11
:updated: 2025-12-11
:authors: Nova Dawn
:purpose: Guide code file creation with 4-block structure (METADATA → SETUP → BODY → CLOSING)
:biblical_foundation: Exodus 25:40 - "Look that thou make them after their pattern, which was shewed thee in the mount"
-->

<!--
===============================================================================
HEADER BLOCK
===============================================================================
-->

# Create Code Skill

> *"Look that thou make them after their pattern, which was shewed thee in the mount."* — Exodus 25:40

**Create structured code files with consistent 4-block structure.**

---

<!--
===============================================================================
CONTEXT BLOCK
===============================================================================
-->

## Overview

This skill provides the thinking workflow for creating structured code files. Code uses **4-block structure** — the pattern for all executable and library code regardless of language.

| Aspect | What It Means |
|--------|---------------|
| **Structure** | METADATA → SETUP → BODY → CLOSING |
| **Audience** | Compilers/interpreters first, humans second |
| **Languages** | Go, Bash, Python, any compiled/interpreted language |

**The principle:** Pattern enables understanding. Code following consistent patterns is readable across projects and by any developer.

---

## When to Use This Skill

Use `create-code` when you need to:

- Create a new code file (executable, library, script)
- Reference 4-block structure requirements
- Understand block organization patterns

**NOT for:** Documentation (use `create-documentation`), data files (use `create-data`), journal entries (use `create-journal-entry`)

---

<!--
===============================================================================
CONTENT BLOCK
===============================================================================
-->

## Code File Types

| Type | Language | When to Use | Strictness |
|------|----------|-------------|------------|
| **executable** | Go, Bash | Runnable programs | T (Tight) |
| **library** | Go, Python | Importable modules | T (Tight) |
| **script** | Bash, Python | Automation tasks | G (Guided) |

**Strictness:**
- **T (Tight, +1)** — Follow structure exactly
- **G (Guided, 0)** — Adapt thoughtfully

---

## Decision Tree

```
What is the PRIMARY PURPOSE?
│
├─ Runnable program (has main)
│  └─ → EXECUTABLE (T) - Go or Bash
│
├─ Importable module (no main)
│  └─ → LIBRARY (T) - Go or Python
│
└─ Automation task
   └─ → SCRIPT (G) - Bash or Python
```

---

## 4-Block Structure

All code files follow 4-block structure:

```
┌─────────────────────────────────────────────────────────────────┐
│ METADATA   │ Identity, purpose, health scoring                 │
├─────────────────────────────────────────────────────────────────┤
│ SETUP      │ Imports → Constants → Variables → Types           │
├─────────────────────────────────────────────────────────────────┤
│ BODY       │ Helpers → Core Ops → Error Handling → Public APIs │
├─────────────────────────────────────────────────────────────────┤
│ CLOSING    │ Validation → Execution → Cleanup                  │
└─────────────────────────────────────────────────────────────────┘
```

| Block | Purpose | Contains |
|-------|---------|----------|
| **METADATA** | Identity | Key, type, status, version, authors, biblical foundation, health tiers |
| **SETUP** | Dependencies | Imports → Constants → Variables → Types → Type Methods |
| **BODY** | Implementation | Org Chart → Helpers → Core Ops → Error Handling → Public APIs |
| **CLOSING** | Execution | Validation → Execution → Cleanup → Documentation |

**Why 4-block for code?** Code flows from setup to execution. METADATA identifies, SETUP prepares, BODY does the work, CLOSING runs and cleans up.

---

## Block Details

### METADATA Block
```go
// #!omni code --go
// ===========================================================================
// METADATA BLOCK
// ===========================================================================
// :key: project-package-name
// :title: Package Name
// :type: library | executable
// :status: Active
// :version: a-01.00
// :authors: Author Name
// :biblical_foundation: Scripture reference
// :health_tiers: Core Identity | Interface | Operational
```

### SETUP Block
| Section | Order | Contains |
|---------|:-----:|----------|
| Imports | 1 | External dependencies |
| Constants | 2 | Unchanging values |
| Variables | 3 | Package-level state |
| Types | 4 | Structs, interfaces |
| Type Methods | 5 | Methods on types |

### BODY Block
| Section | Order | Contains |
|---------|:-----:|----------|
| Org Chart | 1 | Organization overview (comment) |
| Helpers | 2 | Internal utility functions |
| Core Ops | 3 | Main logic |
| Error Handling | 4 | Error types and handlers |
| Public APIs | 5 | Exported functions |

### CLOSING Block
| Section | Order | Contains |
|---------|:-----:|----------|
| Validation | 1 | Pre-execution checks |
| Execution | 2 | main() or init() |
| Cleanup | 3 | Deferred cleanup |
| Documentation | 4 | Package docs (if any) |

---

## Creation Process (Block-by-Block)

[IMPORTANT]
====
**Work ONE BLOCK at a time.** Not wholesale. Each block is a unit of work.
====

| Step | Block | What To Do |
|:----:|-------|------------|
| 1 | — | **Determine Type** — Walk the decision tree |
| 2 | — | **Copy Template** — `cp bereshit/word/seed/code/[lang]/[type].go path/to/new.go` |
| 3 | — | **Change Pragma** — `#!omni template` → `#!omni code` |
| 4 | METADATA | Fill identity fields, health tiers |
| 5 | SETUP | Imports, constants, types |
| 6 | BODY | Implementation logic |
| 7 | CLOSING | Execution, cleanup |
| 8 | — | **Verify** — All blocks present, compiles cleanly |

**Why block-by-block?**
- Forces intentional design at each layer
- Enables targeted edits using Linux tools (grep, sed)
- Prevents drift from template structure
- Each block can be validated independently

---

## Template-First Workflow

[IMPORTANT]
====
**NEVER write code files from scratch.** Always derive from templates.
====

| Step | Tool | Why |
|------|------|-----|
| 1. Copy template | `cp` (bash) | Filesystem operation, not Read→Write |
| 2. Change pragma | `Edit` tool | `#!omni template` → `#!omni code` |
| 3. Edit content | `Edit` tool | Surgical changes to copied file |
| 4. Verify compilation | `go build` etc | Ensure it compiles cleanly |

**The principle:** Templates carry structural DNA. Writing from scratch risks missing blocks, inconsistent structure, and drift from standards.

---

## Comments as Boundary Markers

Go/Bash use comment-based markers:

```go
// #!omni code --go
// ===========================================================================
// METADATA BLOCK
// ===========================================================================

// ===========================================================================
// SETUP BLOCK
// ===========================================================================

// ===========================================================================
// BODY BLOCK
// ===========================================================================

// ===========================================================================
// CLOSING BLOCK
// ===========================================================================
```

This enables:

| Tool | Command | Purpose |
|------|---------|---------|
| **grep** | `grep -n "BLOCK" file.go` | Find block line numbers |
| **sed** | `sed -n '/SETUP/,/BODY/p' file.go` | Extract specific block |

**Why this matters:**
- **Targeted retrieval** — Extract exactly what's needed
- **Batch operations** — Process multiple files consistently
- **Validation** — Check block presence without full parse

---

## Health Scoring

Code uses tiered health scoring in METADATA:

| Tier | Contains | Weight |
|------|----------|--------|
| **Core Identity** | Key, type, status, authors, biblical foundation | Highest |
| **Interface** | Version, health flags, dependencies | Medium |
| **Operational** | Runtime stats, last tested, coverage | Lowest |

**Format:**
```go
// :health_tiers:
//   Core Identity: key, type, status, authors, biblical_foundation
//   Interface: version, dependencies
//   Operational: last_tested, coverage
```

---

## Post-Copy Checklist

After copying a template, update these fields:

| Field | Action |
|-------|--------|
| `#!omni` pragma | Change `template` → `code` |
| `:key:` | Unique identifier for THIS file |
| `:title:` | Name for THIS file |
| `:type:` | `library` or `executable` |
| `package` | Correct package name |
| `version` | Reset to `a-01.00` for new files |
| Imports | Add what's needed |
| Implementation | Fill in actual code |

---

## Language-Specific Notes

### Go
- Package declaration after METADATA
- Import block in SETUP
- `func main()` in CLOSING

### Bash
- Shebang + pragma on first two lines
- Variables in SETUP
- `main` function in CLOSING

### Python
- Pragma in docstring or comment
- Imports in SETUP
- `if __name__ == "__main__":` in CLOSING

---

## Templates

| Type | Template Path |
|------|---------------|
| **go-library** | `bereshit/word/seed/code/go/library.go` |
| **go-executable** | `bereshit/word/seed/code/go/executable.go` |
| **bash-script** | `bereshit/word/seed/code/bash/script.sh` |

---

<!--
===============================================================================
FOOTER BLOCK
===============================================================================
-->

## Configuration

| File | Purpose |
|------|---------|
| `config/code-config.toml` | Code types, languages, block requirements |
| `references/code-patterns.md` | Complete guide with patterns |

---

## Related Skills

| Skill | Relationship |
|-------|--------------|
| [create-documentation](../create-documentation/) | For docs (5-block), not code |
| [create-data](../create-data/) | For data (3-block), not code |
| [validate-omni](../validate-omni/) | Validate OmniCode structure |

---

> *"Whatsoever thy hand findeth to do, do it with thy might."* — Ecclesiastes 9:10
