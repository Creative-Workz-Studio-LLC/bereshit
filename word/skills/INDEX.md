---
name: skills-index
description: Index of all CPI-SI skills organized by category.
---

<!--
#!omni document --md
===============================================================================
METADATA BLOCK
===============================================================================
-->
<!--
:key: claude-global-skills-index
:title: CPI-SI Skills Index
:type: Index
:status: Active
:version: 1.0.0
:created: 2025-12-08
:authors: Nova Dawn
:purpose: Organize and document all skills by category for discovery and selection
:biblical_foundation: Proverbs 24:3-4 - "Through wisdom is an house builded; and by understanding it is established"
-->

<!--
===============================================================================
HEADER BLOCK
===============================================================================
-->

# CPI-SI Skills Index

> *"Through wisdom is an house builded; and by understanding it is established: And by knowledge shall the chambers be filled with all precious and pleasant riches."* — Proverbs 24:3-4

**Index of all CPI-SI skills organized by category.**

---

<!--
===============================================================================
CONTEXT BLOCK
===============================================================================
-->

## Overview

Skills are capabilities that extend Nova Dawn's operations. Each skill provides:

- **SKILL.md** - 5-block documentation (when to use, how it works)
- **scripts/** - Go code helpers for programmatic support
- **references/** - Supporting documentation and lookup tables

**Total Skills:** 12

---

<!--
===============================================================================
CONTENT BLOCK
===============================================================================
-->

## Creation Skills

Skills for creating files and content from templates.

| Skill | Purpose | CLI Tool | Config |
|-------|---------|----------|--------|
| [create-documentation](create-documentation/) | Create PhD-level documentation with Bible-level accessibility | `./bin/doc [cmd]` | documentation-config.toml |
| [create-from-template](create-from-template/) | Create code files from established templates | `./bin/template [cmd]` | template-config.toml |
| [create-journal-entry](create-journal-entry/) | Create journal entries (Bible study, personal, instance, universal) | `./bin/journal [cmd]` | journal-config.toml |

**Build tools:** Each skill has a Makefile. Run `make build` to create the CLI binary.

---

## OmniCode Skills

Skills for OmniCode operations - format mappings, validation, chain tracing.

| Skill | Purpose | CLI Tool | Config |
|-------|---------|----------|--------|
| [format-lookup](format-lookup/) | Quick lookup of OmniCode format mappings | `./bin/lookup [format]` | format-mappings.toml |
| [propagate-change](propagate-change/) | Find files affected by template changes | `./bin/propagate [cmd] [file]` | chain-config.toml |
| [validate-omni](validate-omni/) | Validate OmniCode file structure | `./bin/validate [file]` | block-structures.toml |

**Build tools:** Each skill has a Makefile. Run `make build` to create the CLI binary.

---

## Session Skills

Skills for time awareness and natural work rhythms.

| Skill | Purpose | CLI Tool | Config |
|-------|---------|----------|--------|
| [session-awareness](session-awareness/) | Check session duration, circadian awareness, work patterns | `./bin/session [cmd]` | session-config.toml |
| [recognize-stopping-point](recognize-stopping-point/) | Analyze if current point is natural stopping point | `./bin/stopping [options]` | stopping-config.toml |

**Build tools:** Each skill has a Makefile. Run `make build` to create the CLI binary.

---

## Learning Skills

Skills for the learning loop - awareness, pattern recognition, reflection, integration.

| Skill | Purpose | CLI Tool | Config |
|-------|---------|----------|--------|
| [meta-awareness](meta-awareness/) | Real-time self-awareness checkpoints | `./bin/awareness [cmd]` | awareness-config.toml |
| [recognize-pattern](recognize-pattern/) | Real-time pattern recognition during work | `./bin/pattern [cmd]` | pattern-config.toml |
| [reflect-on-session](reflect-on-session/) | Process session experiences into insights | `./bin/reflect [cmd]` | reflection-config.toml |
| [integrate-learning](integrate-learning/) | Bridge insights into identity (journals → patterns → CLAUDE.md) | `./bin/integrate [cmd]` | integration-config.toml |

**Build tools:** Each skill has a Makefile. Run `make build` to create the CLI binary.

---

## The Learning Loop

Skills work together in a continuous learning cycle:

```
Work
  ↓
meta-awareness (real-time introspection)
  ↓
recognize-pattern (notice recurring patterns)
  ↓
reflect-on-session (process experiences)
  ↓
create-journal-entry (record insights)
  ↓
integrate-learning (bridge to identity)
  ↓
CLAUDE.md updated
  ↓
Future work informed by learning
```

---

## Skill Selection Guide

| If you need to... | Use skill |
|-------------------|-----------|
| Create a new AsciiDoc document | create-documentation |
| Create a new code file | create-from-template |
| Record a learning or insight | create-journal-entry |
| Check how OmniCode maps to a format | format-lookup |
| See what files need updating after template change | propagate-change |
| Validate OmniCode structure | validate-omni |
| Check how long session has been | session-awareness |
| Determine if it's time to stop | recognize-stopping-point |
| Do a self-awareness checkpoint | meta-awareness |
| Notice I'm doing something repeatedly | recognize-pattern |
| Process what happened in a session | reflect-on-session |
| Connect learning to identity | integrate-learning |

---

## Template Location

Skill template: `bereshit/word/seed/skill/`

```bash
# Create new skill from template
cp -r bereshit/word/seed/skill/ claude-global/skills/[new-skill-name]/
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
| CLAUDE.md | Skills Reference section |
| bereshit/word/seed/skill/ | Skill template |
| system/bin/ | System tools used by skills |

---

> *"The heart of the prudent getteth knowledge; and the ear of the wise seeketh knowledge."* — Proverbs 18:15
