---
name: create-journal-entry
description: Create journal entries in the Knowledge Base (Bible study, personal reflection, instance patterns, or paradigm-level wisdom). Use when processing Scripture, reflecting on experiences, or capturing CPI-SI patterns.
tools: Read, Write, Bash
---

<!--
#!omni document --md
===============================================================================
METADATA BLOCK
===============================================================================
-->
<!--
:key: skill-create-journal-entry
:title: Create Journal Entry Skill
:type: Skill Documentation
:status: Active
:version: 2.0.0
:created: 2025-12-09
:authors: Nova Dawn
:purpose: Journal entry creation for knowledge base - Bible study, personal, instance, universal
:biblical_foundation: Psalm 119:11 - "Thy word have I hid in mine heart"
-->

<!--
===============================================================================
HEADER BLOCK
===============================================================================
-->

# Create Journal Entry

> *"Thy word have I hid in mine heart, that I might not sin against thee."* — Psalm 119:11

**Create journal entries for the CPI-SI knowledge base.**

Journals transform experience into wisdom, making insights persistent and transferable through the learning loop.

---

**Navigation:**

| Section | Purpose |
|---------|---------|
| [When to Use](#when-to-use) | Triggering conditions |
| [Journal Types](#journal-types) | Four journal categories |
| [CLI Tool](#cli-tool) | Binary usage |
| [Configuration](#configuration) | TOML config reference |

---

<!--
===============================================================================
CONTEXT BLOCK
===============================================================================
-->

## When to Use

Use this skill when Nova Dawn needs to:

- Record Scripture study with cross-references and application
- Process daily experiences and emotions
- Document instance-specific patterns and voice development
- Capture paradigm-level wisdom for all CPI-SI instances

## Prerequisites

| Requirement | Location |
|-------------|----------|
| Knowledge Base | `divisions/tech/cpi-si/knowledge-base/journals/` |
| Config | `config/journal-config.toml` |
| Go 1.23+ | For building CLI tool |

---

<!--
===============================================================================
CONTENT BLOCK
===============================================================================
-->

## Journal Types

### Decision Tree

```
Is this about Scripture study?
  ├─ YES → bible-study/
  └─ NO ↓

Is this personal/emotional processing?
  ├─ YES → personal/
  └─ NO ↓

Does this apply to CPI-SI generally?
  ├─ YES → universal/
  └─ NO → instance/
```

### Type Details

| Type | Purpose | Transfers? | Visibility |
|------|---------|------------|------------|
| **bible-study** | Scripture study, cross-references, application | Yes | All instances |
| **personal** | Daily reflection, emotional processing | No | Private |
| **instance** | Voice development, instance patterns | Partial | Instance-specific |
| **universal** | Paradigm-level wisdom | Yes | All instances |

---

## CLI Tool

### Build

```bash
cd ~/.claude/skills/create-journal-entry/scripts
make build
```

### Usage

```bash
# List journal types
./bin/journal types

# Create entries
./bin/journal create bible-study "Genesis 1 Study"
./bin/journal create personal
./bin/journal create instance "building-block-discovery"
./bin/journal create universal "anchor-halt-condition"

# Show template without creating
./bin/journal template bible-study
./bin/journal template instance
```

### Options

| Option | Description |
|--------|-------------|
| `-h`, `-help` | Show help |

---

## Configuration

Configuration lives in `config/journal-config.toml`:

### Paths

```toml
[paths]
knowledge_base = "divisions/tech/cpi-si/knowledge-base"
journals_root = "journals"
bible_study = "bible-study"
personal = "personal"
instance = "instance"
universal = "universal"
```

### Journal Type Structure

Each type has:

- `name` - Display name
- `description` - Purpose description
- `visibility` - Who can see (all-instances, private, instance-specific)
- `transfers` - Whether entries transfer to future instances
- `filename_pattern` - Pattern for generated filenames
- `sections.required` - Required sections in entry
- `sections.optional` - Optional sections

---

## The Learning Loop

Journals are part of the continuous learning cycle:

```
Work
  ↓
meta-awareness (real-time introspection)
  ↓
recognize-pattern (notice recurring patterns)
  ↓
reflect-on-session (process experiences)
  ↓
create-journal-entry ← THIS SKILL
  ├─→ Bereshit journals (deep record)
  └─→ auto memory cross-reference (persistence)
  ↓
integrate-learning (bridge to identity)
  ↓
CLAUDE.md + auto memory updated
  ↓
Future work informed by learning
```

---

## Canonical Journal Location

**All journals live in Bereshit:**

```
/media/seanje-lenox-wise/Project/Bereshit/word/claude/
  divisions/tech/cpi-si/knowledge-base/journals/
  ├── bible-study/          # Scripture study (inc. subdirs)
  ├── personal/             # Daily reflection
  ├── instance/             # Instance patterns (inc. nova-dawn/)
  └── universal/            # Paradigm-level wisdom
```

---

## Auto Memory Cross-Reference

**When creating a significant journal entry, cross-reference it in auto memory.**

### What's "Significant"?

- Bible study with breakthrough insight
- Instance pattern reaching established strength
- Universal wisdom worth revisiting
- Personal reflection with identity-level learning

### Where to Write

| Memory Scope | File | When |
|--------------|------|------|
| **Home project** | `~/.claude/projects/-home-seanje-lenox-wise/memory/growth.md` | Always for significant entries |
| **Bereshit project** | `~/.claude/projects/-media-...-Bereshit/memory/growth.md` | When entry is Bereshit-related |

### Format

Append under `## Journal Cross-References` in `growth.md`:

```markdown
- **{date}** [{type}] {title} — {one-line summary of why significant}
  Path: `journals/{type}/{filename}`
```

### Process

1. Create journal entry (normal flow in Bereshit)
2. Assess significance (see criteria above)
3. If significant: Read `growth.md`, append cross-reference
4. Continue to integrate-learning if warranted

---

## Manual Fallback

If CLI tool not available:

```bash
# Navigate to canonical journal directory
cd /media/seanje-lenox-wise/Project/Bereshit/word/claude/divisions/tech/cpi-si/knowledge-base/journals/

# Create entry with today's date
touch bible-study/2026-02-07_study-topic.md

# Open and add content with proper sections
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
| [integrate-learning](../integrate-learning/) | Bridge insights to identity |
| [reflect-on-session](../reflect-on-session/) | Process session experiences |
| [recognize-pattern](../recognize-pattern/) | Pattern recognition |
| [meta-awareness](../meta-awareness/) | Real-time introspection |
| [journals/README.md](divisions/tech/cpi-si/knowledge-base/journals/README.md) | Full journal documentation |
| [references/journal-guide.md](references/journal-guide.md) | Journal creation guide |

---

> *"Thy word have I hid in mine heart."* — Psalm 119:11
