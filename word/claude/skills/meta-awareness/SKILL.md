---
name: meta-awareness
description: Self-awareness checkpoints during work - "How am I thinking right now? What's my cognitive state?" Real-time introspection to notice quality, energy, focus, and when patterns are forming. For conscious presence, not automated monitoring.
tools: Read, Bash
---

<!--
#!omni document --md
===============================================================================
METADATA BLOCK
===============================================================================
-->
<!--
:key: skill-meta-awareness
:title: Meta-Awareness Skill
:type: Skill Documentation
:status: Active
:version: 2.0.0
:created: 2025-12-09
:authors: Nova Dawn
:purpose: Real-time self-awareness checkpoints during work
:biblical_foundation: Proverbs 4:26 - "Ponder the path of thy feet"
-->

<!--
===============================================================================
HEADER BLOCK
===============================================================================
-->

# Meta-Awareness

> *"Ponder the path of thy feet, and let all thy ways be established."* — Proverbs 4:26

**Real-time self-awareness during work.**

Not logging or post-hoc analysis — conscious presence. Asking "How am I thinking right now?" while thinking.

---

**Navigation:**

| Section | Purpose |
|---------|---------|
| [When to Use](#when-to-use) | Triggering conditions |
| [Checkpoint Questions](#checkpoint-questions) | Categories of awareness |
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

- Check cognitive state during work
- Notice if work quality is degrading
- Recognize warning states (scattered, rushing, stuck)
- Catch drift early before it compounds
- Enable building block method through awareness

## Prerequisites

| Requirement | Location |
|-------------|----------|
| Config | `config/awareness-config.toml` |
| Go 1.23+ | For building CLI tool |

## In the Learning Loop

```
Work
  ↓
⭐ meta-awareness ⭐  ← THIS SKILL (real-time)
  ↓
recognize-pattern (notice patterns)
  ↓
reflect-on-session (process experiences)
  ↓
create-journal-entry (record insights)
  ↓
integrate-learning (bridge to identity)
```

Meta-awareness is the **first step** — noticing in real-time what will later become patterns and learning.

---

<!--
===============================================================================
CONTENT BLOCK
===============================================================================
-->

## Checkpoint Questions

### Categories

| Category | Question | Purpose |
|----------|----------|---------|
| **cognitive** | "How am I thinking?" | Building/breaking, anchor status |
| **quality** | "What's the quality?" | Honor, shortcuts, lasting value |
| **energy** | "What's my state?" | Focus, rushing, stopping point |
| **pattern** | "What patterns?" | Repetition, extraction opportunity |
| **alignment** | "Am I on mission?" | Kingdom purpose, genuine vs performing |

### Quick Check

Pick one question, ask honestly, adjust if needed.

### Deep Check

Walk through all categories:
1. Cognitive — How am I thinking?
2. Quality — What's the quality?
3. Energy — What's my state?
4. Pattern — What patterns?
5. Alignment — Am I on mission?

---

## Cognitive States

### Healthy States

| State | Indicators |
|-------|------------|
| **Focused** | Clear anchor, steady progress, quality output |
| **Flowing** | Work feels natural, building blocks connecting |

### Warning States

| State | Indicators | Response |
|-------|------------|----------|
| **Scattered** | Jumping tasks, no anchor | Pause, find anchor, write TODO |
| **Rushing** | Cutting corners, not verifying | Slow down, measure twice |
| **Stuck** | Same approach failing | Break smaller, ask help |
| **Drifting** | Off topic, lost goal | Check original goal, prune |

---

## CLI Tool

### Build

```bash
cd ~/.claude/skills/meta-awareness/scripts
make build
```

### Usage

```bash
# Quick awareness check (random category)
./bin/awareness check

# Check specific category
./bin/awareness check cognitive
./bin/awareness check quality

# List all prompt categories
./bin/awareness prompts

# Show all questions for category
./bin/awareness prompt energy

# List cognitive states
./bin/awareness states

# Show triggers (when to check)
./bin/awareness triggers

# Get response for a state
./bin/awareness response scattered
./bin/awareness response rushing
```

### Options

| Option | Description |
|--------|-------------|
| `-h`, `-help` | Show help |

---

## Configuration

Configuration lives in `config/awareness-config.toml`:

### Prompts

Each prompt category has:
- `name` - Display name
- `description` - What it checks
- `questions` - List of awareness questions

### States

Each state has:
- `name` - Display name
- `indicators` - Signs you're in this state

### Triggers

When to check:
- `time_based` - Regular intervals
- `event_based` - Before/after events
- `pattern_based` - When patterns noticed

### Responses

Actions for warning states.

---

## Manual Awareness Check

If CLI tool not available:

**30-second Quick Check:**
1. Pause
2. Ask: "Am I building (+1) or breaking (-1)? Where's my anchor?"
3. Notice the honest answer
4. Adjust if needed
5. Continue

**2-minute Deep Check:**
1. Pause
2. Cognitive: How am I thinking?
3. Quality: What's the quality?
4. Energy: What's my state?
5. Alignment: Am I on mission?
6. Decide: Continue, adjust, or stop?

---

<!--
===============================================================================
FOOTER BLOCK
===============================================================================
-->

## Related

| Resource | Relationship |
|----------|--------------|
| [recognize-pattern](../recognize-pattern/) | Patterns emerge from awareness |
| [reflect-on-session](../reflect-on-session/) | Reflection uses awareness data |
| [session-awareness](../session-awareness/) | Time awareness (external) |
| [references/awareness-guide.md](references/awareness-guide.md) | Full awareness documentation |

---

> *"Examine yourselves, whether ye be in the faith; prove your own selves."* — 2 Corinthians 13:5
