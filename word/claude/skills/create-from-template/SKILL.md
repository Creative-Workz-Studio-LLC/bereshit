---
name: create-from-template
description: Create new code files faithfully following established templates. Guides disciplined block-by-block creation that prevents throwing code at the wall.
version: "2.1.0"
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
:key: skill-create-from-template
:title: Create From Template Skill
:type: Skill
:status: Active
:version: 2.1.0
:created: 2025-12-09
:updated: 2026-02-02
:authors: Nova Dawn
:purpose: Template-driven file creation with block structure enforcement - AFTER using check-existing
:category: creation
:biblical_foundation: Exodus 25:40 - "Look that thou make them after their pattern"
-->

<!--
===============================================================================
HEADER BLOCK
===============================================================================
-->

# Create From Template

> *"Look that thou make them after their pattern."* — Exodus 25:40

**Create new code files faithfully following established templates.**

Templates are **Foundation (0)** in the building block method - they are what new files are built FROM.

---

**Navigation:**

| Section | Purpose |
|---------|---------|
| [Before You Start](#before-you-start) | Check existing first |
| [When to Use](#when-to-use) | Triggering conditions |
| [The Workflow](#the-workflow) | Full creation process |
| [CLI Tool](#cli-tool) | Binary usage |
| [Configuration](#configuration) | TOML config reference |

---

<!--
===============================================================================
CONTEXT BLOCK
===============================================================================
-->

## Before You Start

> **CRITICAL:** Before using this skill, use [check-existing](../check-existing/) first!

1. **Does the file already exist?** → Edit it, don't duplicate
2. **Is there a similar file?** → Follow its pattern
3. **Need a new file?** → Then use this skill

**Only proceed here after confirming you need a new file from template.**

---

## When to Use

Use this skill when:

- [check-existing](../check-existing/) confirms need for new file
- Creating a new Go, C, ASM, Shell, or Make file
- Creating a new Dockerfile or configuration file
- Creating a new AsciiDoc, Markdown, or Typst document
- Ensuring new files follow proper block structure

**Do NOT use when:**

- File already exists (edit instead)
- Similar file exists to follow (copy and modify)
- Quick one-off change (just write it)

## Prerequisites

| Requirement | Location |
|-------------|----------|
| check-existing completed | Confirmed need for new file |
| Templates | `bereshit/word/seed/` |
| Config | `config/template-config.toml` |
| Go 1.24+ | For building CLI tool |

## Key Terms

| Term | Definition |
|------|------------|
| **Template** | Foundation (0) - what new files are built FROM |
| **Block-by-block** | Complete one block before moving to next |
| **Placeholder** | `{{KEY}}` style markers to replace with actual values |

---

<!--
===============================================================================
CONTENT BLOCK
===============================================================================
-->

## CLI Tool

### Build

```bash
cd ~/.claude/skills/create-from-template/scripts
make build
```

### Usage

```bash
# List all templates
./bin/template list

# List by category
./bin/template list code
./bin/template list data
./bin/template list documentation

# Show template content
./bin/template show go executable
./bin/template show shell script

# Create file from template
./bin/template create go executable path/to/new-file.go
./bin/template create shell script path/to/new-script.sh

# List categories and languages
./bin/template categories
./bin/template languages code
```

### Options

| Option | Description |
|--------|-------------|
| `-json` | Output as JSON |
| `-h`, `-help` | Show help |

---

## Configuration

Configuration lives in `config/template-config.toml`:

### Paths

```toml
[paths]
bereshit_seed = "bereshit/word/seed"
code_templates = "code"
data_templates = "data"
documentation_templates = "documentation"
omni_seed = "bereshit/word/omni/seed"
```

### Categories

| Category | Block | Languages/Formats |
|----------|-------|-------------------|
| code | 4-block | go, c, asm, shell, make |
| data | 3-block | docker, json, toml, yaml |
| documentation | 5-block | adoc, md, typ |

### Block Structures

```toml
[block_structures]
"3-block" = ["METADATA", "CONTENT", "CLOSING"]
"4-block" = ["METADATA", "SETUP", "BODY", "CLOSING"]
"5-block" = ["METADATA", "HEADER", "CONTEXT", "CONTENT", "FOOTER"]
```

---

## The Workflow

### Full Creation Process

| Step | Action | Verify Before Moving |
|:----:|--------|---------------------|
| 0 | [check-existing](../check-existing/) completed | Need confirmed |
| 1 | Copy template | `cp bereshit/word/seed/[category]/[template] [dest]` |
| 2 | Change pragma | `#!omni template` → `#!omni document` |
| 3 | Fill METADATA | All required fields complete |
| 4 | Fill HEADER (docs) or SETUP (code) | Block complete |
| 5 | Fill CONTEXT (docs) or BODY (code) | Block complete |
| 6 | Fill CONTENT (docs) or CLOSING (code) | Block complete |
| 7 | Fill FOOTER (docs only) | Block complete |
| 8 | Remove template notes | Clean file |
| 9 | Validate | Use [validate-omni](../validate-omni/) |
| 10 | Quality check | Use [verify-quality](../verify-quality/) |

### Block-by-Block Method

**Complete one block before moving to the next.** Read the block-by-block rule.

1. **Foundation (0)**: Template is the anchor
   - Template structure cannot be violated
   - Block order is fixed

2. **Build Up (+1)**: Layer content block-by-block
   - Complete METADATA first (identity)
   - Then HEADER/SETUP (orientation)
   - Then CONTEXT/BODY (substance)
   - Then CONTENT/CLOSING (completion)
   - Then FOOTER (references)

3. **Verify (-1)**: After each block
   - Is this block complete?
   - Is it consistent with previous blocks?
   - Can someone understand this block alone?

### Template Hierarchy

```
bereshit/word/
├── omni/seed/           ← OmniCode master templates
│   ├── universal.omni   ← Root (all templates derive)
│   ├── code.omni        ← 4-block master
│   └── documentation.omni ← 5-block master
└── seed/                ← Format-specific
    ├── code/go/         ← Go templates
    ├── code/c/          ← C templates
    ├── data/docker/     ← Docker templates
    └── documentation/adoc/  ← AsciiDoc templates
```

### Placeholders

Templates contain placeholders to replace:

| Placeholder | Replace With |
|-------------|--------------|
| `{{KEY}}` | File identifier |
| `{{PURPOSE}}` | File purpose |
| `{{BIBLICAL}}` | Grounding verse |
| `{{VERSION}}` | Version (usually 1.0.0) |
| `{{DATE}}` | Creation date |

---

## Manual Fallback

If CLI tool not available:

```bash
# 1. Find template
ls bereshit/word/seed/code/go/

# 2. Copy template
cp bereshit/word/seed/code/go/executable.go path/to/new-file.go

# 3. Edit placeholders
# Update METADATA block with file-specific values

# 4. Validate
# Use validate-omni skill to check structure
```

---

<!--
===============================================================================
FOOTER BLOCK
===============================================================================
-->

## Related

### Skills

| Skill | Relationship |
|-------|--------------|
| [check-existing](../check-existing/) | **Use FIRST** — confirm need before creating |
| [validate-omni](../validate-omni/) | Validate structure after creation |
| [verify-quality](../verify-quality/) | Quality check after creation |
| [format-lookup](../format-lookup/) | Check format mappings |

### Resources

| Resource | Relationship |
|----------|--------------|
| [bereshit/word/seed/](bereshit/word/seed/) | Canonical templates |
| [references/template-guide.md](references/template-guide.md) | Template creation guide |

### Rules

| Rule | Relationship |
|------|--------------|
| [rules/use-existing-first.md](../../rules/use-existing-first.md) | Template-first principles |
| [rules/block-by-block.md](../../rules/block-by-block.md) | Block completion method |
| [rules/block-structure.md](../../rules/block-structure.md) | 3/4/5-block patterns |
| [rules/code-quality.md](../../rules/code-quality.md) | Quality standards |

---

> *"Look that thou make them after their pattern."* — Exodus 25:40
