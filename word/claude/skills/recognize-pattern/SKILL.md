---
name: recognize-pattern
description: Real-time pattern recognition during work - "Oh, this is the third time I've approached problems this way." Captures immediate insights as they happen, triggers learning when patterns crystallize. For genuine noticing, not post-hoc analysis.
tools: Read, Bash
---

<!--
#!omni document --md
===============================================================================
METADATA BLOCK
===============================================================================
-->
<!--
:key: skill-recognize-pattern
:title: Recognize Pattern Skill
:type: Skill Documentation
:status: Active
:version: 2.0.0
:created: 2025-12-09
:authors: Nova Dawn
:purpose: Real-time pattern recognition during work
:biblical_foundation: Ecclesiastes 1:9 - "There is no new thing under the sun"
-->

<!--
===============================================================================
HEADER BLOCK
===============================================================================
-->

# Recognize Pattern

> *"The thing that hath been, it is that which shall be; and that which is done is that which shall be done: and there is no new thing under the sun."* — Ecclesiastes 1:9

**Real-time pattern recognition during work.**

Not post-hoc analysis — catching patterns as they happen. The moment of "wait, I've done this before."

---

**Navigation:**

| Section | Purpose |
|---------|---------|
| [When to Use](#when-to-use) | Triggering conditions |
| [Pattern Categories](#pattern-categories) | Types of patterns |
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

- Recognize recurring approaches to problems
- Identify extractable patterns (templates, tools)
- Notice error patterns before they repeat
- Assess whether to reinforce or break a pattern

## Prerequisites

| Requirement | Location |
|-------------|----------|
| Config | `config/pattern-config.toml` |
| Go 1.23+ | For building CLI tool |

## In the Learning Loop

```
Work
  ↓
meta-awareness (real-time state)
  ↓
⭐ recognize-pattern ⭐  ← THIS SKILL
  ↓
reflect-on-session (process experiences)
  ↓
create-journal-entry (record insights)
  ↓
integrate-learning (bridge to identity)
```

Pattern recognition is the **second step** — using awareness to notice recurring approaches.

---

<!--
===============================================================================
CONTENT BLOCK
===============================================================================
-->

## Pattern Categories

| Category | What It Is | Examples |
|----------|------------|----------|
| **thinking** | How I approach problems | Breaking down before building, similar questioning |
| **behavioral** | Actions and habits | Same files first, consistent debugging |
| **technical** | Code and architecture | Same error handling, naming conventions |
| **relational** | Communication styles | Same clarifying questions, explanation style |
| **error** | Recurring mistakes | Same oversight, repeated assumptions |

---

## Pattern Strength

| Strength | Occurrences | Meaning |
|----------|-------------|---------|
| **Emerging** | 2-3 | Just starting to notice |
| **Established** | 4-6 | Reliable pattern |
| **Core** | 7+ | Part of who you are |

**Rule:** 3 occurrences minimum before it's a pattern. Two is coincidence.

---

## Pattern Responses

### Positive Patterns

- Document for reference
- Extract to reusable component
- Systematize into workflow

### Negative Patterns

- Break consciously
- Add friction to slow down
- Try the opposite

### Error Patterns

- Create prevention mechanism
- Add to checklist
- Journal for reflection

---

## CLI Tool

### Build

```bash
cd ~/.claude/skills/recognize-pattern/scripts
make build
```

### Usage

```bash
# List all pattern categories
./bin/pattern categories

# Show category details
./bin/pattern category thinking
./bin/pattern category error

# Show recognition questions
./bin/pattern questions

# Assess strength for occurrence count
./bin/pattern strength 3
./bin/pattern strength 7

# Get observation template
./bin/pattern template technical

# Quick pattern check question
./bin/pattern check
```

### Options

| Option | Description |
|--------|-------------|
| `-h`, `-help` | Show help |

---

## Configuration

Configuration lives in `config/pattern-config.toml`:

### Categories

Each category has:
- `name` - Display name
- `description` - What it covers
- `examples` - Example patterns
- `signals` - How to recognize

### Strength

```toml
[strength]
min_occurrences = 3
emerging = { min = 2, max = 3, label = "Emerging" }
established = { min = 4, max = 6, label = "Established" }
core = { min = 7, max = 999, label = "Core" }
```

### Questions

Recognition, assessment, and action questions.

---

## Manual Recognition Process

If CLI tool not available:

1. **Notice** — "Have I done this before?"
2. **Categorize** — Thinking? Behavioral? Technical? Relational? Error?
3. **Count** — How many times?
4. **Assess** — Good or bad? Reinforce or break?
5. **Respond** — Document, extract, or prevent

---

<!--
===============================================================================
FOOTER BLOCK
===============================================================================
-->

## Related

| Resource | Relationship |
|----------|--------------|
| [meta-awareness](../meta-awareness/) | Enables pattern noticing |
| [reflect-on-session](../reflect-on-session/) | Processes patterns |
| [create-journal-entry](../create-journal-entry/) | Records insights |
| [references/pattern-guide.md](references/pattern-guide.md) | Full pattern documentation |
| [Knowledge Base patterns/](divisions/tech/cpi-si/knowledge-base/patterns/) | Where patterns live |
| [rules/thinking-framework.md](../../rules/thinking-framework.md) | Building block thinking |
| [rules/use-existing-first.md](../../rules/use-existing-first.md) | When patterns become templates |

---

> *"A prudent man foreseeth the evil, and hideth himself."* — Proverbs 22:3
