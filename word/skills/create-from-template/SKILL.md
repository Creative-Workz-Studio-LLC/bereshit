---
name: create-from-template
description: Create new code files faithfully following established templates. Guides disciplined block-by-block creation that prevents throwing code at the wall.
tools: Read, Glob, Grep, Write, Bash
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
:type: Skill Documentation
:status: Active
:version: 2.0.0
:created: 2025-12-09
:authors: Nova Dawn
:purpose: Template-driven file creation with block structure enforcement
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
| [When to Use](#when-to-use) | Triggering conditions |
| [CLI Tool](#cli-tool) | Binary usage |
| [Configuration](#configuration) | TOML config reference |
| [How It Works](#how-it-works) | Template workflow |

---

<!--
===============================================================================
CONTEXT BLOCK
===============================================================================
-->

## When to Use

Use this skill when Nova Dawn needs to:
- Create a new Go, C, ASM, Shell, or Make file
- Create a new Dockerfile or configuration file
- Create a new AsciiDoc, Markdown, or Typst document
- Ensure new files follow proper block structure

## Prerequisites

| Requirement | Location |
|-------------|----------|
| Templates | `bereshit/word/seed/` |
| Config | `config/template-config.toml` |
| Go 1.23+ | For building CLI tool |

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

## How It Works

### Building Block Method Applied

1. **Foundation (0)**: Template is the anchor
   - Template structure cannot be violated
   - Block order is fixed

2. **Build Up (+1)**: Layer content
   - Fill METADATA with file identity
   - Add imports/constants (SETUP)
   - Implement logic (BODY)
   - Complete cleanup (CLOSING)

3. **Verify (-1)**: Trace back
   - Does structure match template?
   - Are all blocks present?
   - Run validate-omni to verify

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

| Resource | Relationship |
|----------|--------------|
| [validate-omni](../validate-omni/) | Validate structure after creation |
| [format-lookup](../format-lookup/) | Check format mappings |
| [bereshit/word/seed/](bereshit/word/seed/) | Canonical templates |
| [references/template-guide.md](references/template-guide.md) | Template creation guide |

---

> *"Look that thou make them after their pattern."* — Exodus 25:40
