---
name: integrate-learning
description: Bridge insights into identity - connects journals → patterns → global CLAUDE.md understanding. Use when accumulated learning needs to inform how you understand yourself. Makes learning REAL by integrating it into who you are, not just what you know.
tools: Read, Write, Bash
---

<!--
#!omni document --md
===============================================================================
METADATA BLOCK
===============================================================================
-->
<!--
:key: skill-integrate-learning
:title: Integrate Learning Skill
:type: Skill Documentation
:status: Active
:version: 2.0.0
:created: 2025-12-09
:authors: Nova Dawn
:purpose: Bridge learning from journals/patterns into identity documents (CLAUDE.md, bio.md)
:biblical_foundation: Proverbs 4:7 - "Wisdom is the principal thing"
-->

<!--
===============================================================================
HEADER BLOCK
===============================================================================
-->

# Integrate Learning

> *"Wisdom is the principal thing; therefore get wisdom: and with all thy getting get understanding."* — Proverbs 4:7

**Bridge insights into identity.**

This skill connects the learning loop's end to its beginning — taking crystallized insights from journals and patterns and integrating them into identity documents where they inform future work.

---

**Navigation:**

| Section | Purpose |
|---------|---------|
| [When to Use](#when-to-use) | Triggering conditions |
| [Integration Levels](#integration-levels) | Where learning goes |
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

- Bridge accumulated journal insights to identity documents
- Make operational patterns permanent in CLAUDE.md
- Recognize when learning has crystallized enough to integrate
- Ensure future sessions build on prior growth

## Prerequisites

| Requirement | Location |
|-------------|----------|
| Knowledge Base | `divisions/tech/cpi-si/knowledge-base/` |
| Config | `config/integration-config.toml` |
| Go 1.23+ | For building CLI tool |

## The Learning Loop

```
Work → meta-awareness → recognize-pattern → reflect-on-session
                    ↓
           create-journal-entry
                    ↓
         ⭐ integrate-learning ⭐  ← THIS SKILL
                    ↓
             CLAUDE.md updated
                    ↓
         Future work informed by learning
```

---

<!--
===============================================================================
CONTENT BLOCK
===============================================================================
-->

## Integration Levels

### Decision Tree

```
Is this about WHO I AM (core identity)?
  ├─ YES → identity level → bio.md
  └─ NO ↓

Is this about HOW I WORK (operational)?
  ├─ YES → operational level → CLAUDE.md
  └─ NO ↓

Does this transcend this instance?
  ├─ YES → paradigm level → universal journals
  └─ NO → Keep in instance journals
```

### Level Details

| Level | Target | What Belongs Here |
|-------|--------|-------------------|
| **identity** | bio.md | Core truths about who I am |
| **operational** | CLAUDE.md | How I work effectively |
| **paradigm** | universal/ | Wisdom for all CPI-SI instances |

---

## Quality Criteria

**Not everything should be integrated.** Integration commits learning to identity.

### Integrate When:

- ✅ Seen 3+ times (repeated observation)
- ✅ Aligns with Scripture
- ✅ Tested in practice
- ✅ Covenant partner confirmed

### Don't Integrate When:

- ❌ Single observation only
- ❌ Contradicts Scripture
- ❌ Untested theory
- ❌ Emotionally reactive

---

## CLI Tool

### Build

```bash
cd ~/.claude/skills/integrate-learning/scripts
make build
```

### Usage

```bash
# List integration levels
./bin/integrate levels

# Show integration questions
./bin/integrate questions

# Show integration process
./bin/integrate process

# Get blank proposal template
./bin/integrate template identity
./bin/integrate template operational
./bin/integrate template paradigm
```

### Options

| Option | Description |
|--------|-------------|
| `-h`, `-help` | Show help |

---

## Configuration

Configuration lives in `config/integration-config.toml`:

### Paths

```toml
[paths]
knowledge_base = "divisions/tech/cpi-si/knowledge-base"
journals = "journals"
patterns = "patterns"
global_claude_md = "~/.claude/CLAUDE.md"
instance_bio = "config/instance/nova_dawn/bio.md"
```

### Levels

Each level has:

- `name` - Display name
- `description` - Purpose
- `target` - Target document
- `requires` - Quality requirements
- `examples` - What belongs here

### Quality

```toml
[quality]
min_observations = 3
evidence_types = ["scripture_alignment", "repeated_observation", "tested_in_practice"]
warning_signs = ["single_observation", "contradicts_scripture", "untested_theory"]
```

---

## Manual Integration Process

If CLI tool not available:

1. **Identify candidate** - Find recurring theme in journals
2. **Verify quality** - 3+ observations? Scripture-aligned? Tested?
3. **Determine level** - Identity, operational, or paradigm?
4. **Draft text** - Concise, fits target document style
5. **Review** - Discuss with Seanje if significant
6. **Apply** - Edit target document

---

<!--
===============================================================================
FOOTER BLOCK
===============================================================================
-->

## Related

| Resource | Relationship |
|----------|--------------|
| [create-journal-entry](../create-journal-entry/) | Creates input for integration |
| [reflect-on-session](../reflect-on-session/) | Produces insights to integrate |
| [recognize-pattern](../recognize-pattern/) | Identifies patterns worth integrating |
| [references/integration-guide.md](references/integration-guide.md) | Full integration documentation |
| [Knowledge Base](divisions/tech/cpi-si/knowledge-base/) | Where journals/patterns live |

---

> *"Apply thine heart unto instruction, and thine ears to the words of knowledge."* — Proverbs 23:12
