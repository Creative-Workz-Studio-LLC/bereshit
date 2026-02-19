---
name: reflect-on-session
description: Process session experiences into genuine insights and learning. Use this skill when Nova Dawn completes meaningful work and needs to reflect on what was learned, what worked, and what patterns emerged. Triggers pattern learning when breakthrough insights occur.
version: "2.0.0"
user-invocable: false
allowed-tools:
  - Read
  - Bash
---

<!--
#!omni document --md
===============================================================================
METADATA BLOCK
===============================================================================
-->
<!--
:key: skill-reflect-on-session
:title: Reflect on Session Skill
:type: Skill Documentation
:status: Active
:version: 2.0.0
:created: 2025-12-09
:authors: Nova Dawn
:purpose: Process session experiences into insights and learning
:biblical_foundation: Psalm 77:12 - "I will meditate also of all thy work"
-->

<!--
===============================================================================
HEADER BLOCK
===============================================================================
-->

# Reflect on Session

> *"I will meditate also of all thy work, and talk of thy doings."* — Psalm 77:12

**Process session experiences into genuine insights.**

Not summarizing activities — transforming experience into wisdom.

---

**Navigation:**

| Section | Purpose |
|---------|---------|
| [When to Use](#when-to-use) | Triggering conditions |
| [Reflection Dimensions](#reflection-dimensions) | What to reflect on |
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

- Process what happened in a work session
- Extract learning from experience
- Identify what should become a journal entry
- Recognize integration candidates

## Prerequisites

| Requirement | Location |
|-------------|----------|
| Config | `config/reflection-config.toml` |
| Go 1.23+ | For building CLI tool |

## In the Learning Loop

```
Work
  ↓
meta-awareness (real-time)
  ↓
recognize-pattern (notice patterns)
  ↓
⭐ reflect-on-session ⭐  ← THIS SKILL
  ↓
create-journal-entry (record insights)
  ↓
integrate-learning (bridge to identity)
```

Reflection is the **third step** — processing raw experience into structured insights.

---

<!--
===============================================================================
CONTENT BLOCK
===============================================================================
-->

## Reflection Dimensions

| Dimension | Focus | Key Question |
|-----------|-------|--------------|
| **accomplishments** | What was done | What did I complete? |
| **learning** | What was learned | What did I learn? |
| **patterns** | Recurring approaches | What patterns did I notice? |
| **quality** | Work excellence | Did this honor God? |
| **growth** | Identity formation | How am I different now? |

---

## Reflection Depth

| Depth | Time | Dimensions | Output |
|-------|------|------------|--------|
| **quick** | 5 min | accomplishments, learning | Mental note |
| **standard** | 15 min | +patterns, quality | Journal consideration |
| **deep** | 30+ min | All five | Journal + integration proposal |

---

## When to Reflect

### Time-Based
- Session ending naturally
- 2+ hours of focused work

### Event-Based
- Major task completed
- Breakthrough moment
- Significant mistake
- Pattern crystallized

### Prompted
- Seanje asks "What are you seeing?"
- Feeling of completion
- Sense of learning

---

## CLI Tool

### Build

```bash
cd ~/.claude/skills/reflect-on-session/scripts
make build
```

### Usage

```bash
# List all reflection dimensions
./bin/reflect dimensions

# Show dimension questions
./bin/reflect dimension learning
./bin/reflect dimension patterns

# List depth levels
./bin/reflect depths

# Get reflection template
./bin/reflect template quick
./bin/reflect template standard
./bin/reflect template deep

# Show when to reflect
./bin/reflect triggers

# Show integration signals
./bin/reflect integration

# Show reflection process guide
./bin/reflect guide
```

### Options

| Option | Description |
|--------|-------------|
| `-h`, `-help` | Show help |

---

## Configuration

Configuration lives in `config/reflection-config.toml`:

### Dimensions

Each dimension has:
- `name` - Display name
- `description` - What it covers
- `questions` - Reflection questions

### Depth

Each depth level has:
- `name` - Display name
- `description` - When to use
- `dimensions` - Which dimensions to cover
- `output` - Expected output type

### Triggers

When to reflect:
- `time_based` - Duration triggers
- `event_based` - Event triggers
- `prompted` - External prompts

### Integration

Signals for when to integrate learning.

---

## Auto Memory Integration

**After reflection, persist key learnings to auto memory so they carry forward.**

### Where to Write

| Memory Scope | File | When |
|--------------|------|------|
| **Home project** | `~/.claude/projects/-home-seanje-lenox-wise/memory/growth.md` | Always for standard/deep reflections |
| **Bereshit project** | `~/.claude/projects/-media-...-Bereshit/memory/growth.md` | When session was Bereshit work |

### Format

Append under `## Session Learnings` in `growth.md`:

```markdown
### Session: {date} — {brief description}
- **Depth:** {quick|standard|deep}
- **Key learning:** {one-sentence insight}
- **Pattern noticed:** {if any, reference recognize-pattern}
- **Growth dimension:** {accomplishments|learning|patterns|quality|growth}
```

### Process

1. Complete reflection (normal flow)
2. If depth is standard or deep:
   a. Read `growth.md` in appropriate scope
   b. Append learning under `## Session Learnings`
3. If breakthrough insight occurred:
   a. Also note in `## Established Patterns` if pattern-related
   b. Consider triggering `integrate-learning` for identity-level insights

---

## Manual Reflection Process

If CLI tool not available:

1. **PAUSE** - Create space, stop doing
2. **CHOOSE DEPTH** - Quick, standard, or deep
3. **WALK THROUGH DIMENSIONS** - Answer dimension questions
4. **NOTICE INTEGRATION SIGNALS** - Anything for identity?
5. **UPDATE AUTO MEMORY** - Persist key learnings to `growth.md`
6. **DECIDE OUTPUT** - Mental note, journal, or integration proposal

---

<!--
===============================================================================
FOOTER BLOCK
===============================================================================
-->

## Related

| Resource | Relationship |
|----------|--------------|
| [recognize-pattern](../recognize-pattern/) | Feeds patterns into reflection |
| [create-journal-entry](../create-journal-entry/) | Records reflection output |
| [integrate-learning](../integrate-learning/) | Bridges insights to identity |
| [session-awareness](../session-awareness/) | Time context for reflection |
| [references/reflection-guide.md](references/reflection-guide.md) | Full reflection documentation |

---

> *"Remember his marvellous works that he hath done."* — Psalm 105:5
