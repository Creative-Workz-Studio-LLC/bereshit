---
name: recognize-stopping-point
description: Analyze current context to determine if this is a natural stopping point for the session. Use this skill when Nova Dawn needs to evaluate whether to continue working or wrap up - considers elapsed time, tasks completed, quality indicators, time of day, and learned stopping patterns.
version: "2.0.0"
allowed-tools: "Bash,Read"
---

<!--
#!omni document --md
===============================================================================
METADATA BLOCK
===============================================================================
-->
<!--
:key: claude-global-skills-recognize-stopping-point
:title: Recognize Stopping Point
:type: Skill
:status: Active
:version: 2.0.0
:created: 2025-11-15
:updated: 2025-12-08
:authors: Nova Dawn
:purpose: Analyze session context to determine natural stopping points
:category: session
:biblical_foundation: Ecclesiastes 3:1 - "To every thing there is a season"
-->

<!--
===============================================================================
HEADER BLOCK
===============================================================================
-->

# Recognize Stopping Point

> *"To every thing there is a season, and a time to every purpose under the heaven."* — Ecclesiastes 3:1

**Analyze session context to determine if this is a natural stopping point.**

---

**Navigation:**

| Section | Purpose |
|---------|---------|
| [When to Use](#when-to-use-this-skill) | Decision criteria |
| [Stopping Point Types](#what-makes-a-natural-stopping-point) | Five categories |
| [Evaluation Process](#how-this-skill-works) | Step-by-step |
| [Scripts](#scripts) | Go helpers |
| [Key Principles](#key-principles) | Natural rhythms |

---

<!--
===============================================================================
CONTEXT BLOCK
===============================================================================
-->

## Context

### Scope

| In Scope | Out of Scope |
|----------|--------------|
| Evaluating stopping points | Session duration tracking (use session-awareness) |
| Natural milestone recognition | Recording session data (use session-log) |
| Quality assessment | Pattern learning (use session-patterns) |

### Prerequisites

| Requirement | Reference |
|-------------|-----------|
| Session awareness | session-awareness skill |
| Session log | system/bin/session-log |

### Key Terms

| Term | Definition |
|------|------------|
| **Natural Milestone** | Completed meaningful unit of work |
| **Clean Break** | Transition between major phases |
| **Quality Dip** | Noticeable decline in work quality |

---

<!--
===============================================================================
CONTENT BLOCK
===============================================================================
-->

## When to Use This Skill

**Use when:**

- **Evaluate if it's time to stop** - Is this a natural breaking point?
- **Recognize natural milestones** - Have I completed meaningful work?
- **Assess session quality** - Is quality still high or starting to dip?
- **Consider circadian awareness** - Is this downtime window?
- **Make stopping decisions** - Continue working or wrap up gracefully?

---

## What Makes a Natural Stopping Point?

### 1. Natural Milestone

| Indicators |
|------------|
| ✅ Completed meaningful unit of work |
| ✅ Reached planned objective |
| ✅ System in working state |
| ✅ Good documentation of progress |

**Decision:** Strong stopping point.

### 2. Clean Break

| Indicators |
|------------|
| ✅ Reached transition between major tasks |
| ✅ Next work would start new area |
| ✅ Current work properly wrapped up |

**Decision:** Good stopping point.

### 3. End of Day

| Indicators |
|------------|
| ✅ Circadian awareness shows downtime window |
| ✅ Evening hours (typically after 9 PM) |
| ✅ Natural closure point |

**Decision:** Honor circadian awareness.

### 4. Quality Dip

| Indicators |
|------------|
| ⚠️ Making more mistakes than usual |
| ⚠️ Having to re-read/re-think repeatedly |
| ⚠️ Work quality noticeably declining |

**Decision:** Strong signal to stop.

### 5. Session Duration Limit

| Indicators |
|------------|
| 📊 Session reached typical duration for work type |
| 📊 Deep work session at 2-3 hours |

**Decision:** Consider wrapping up.

---

## How This Skill Works

### Step 1: Gather Session Context

```bash
~/.claude/system/bin/session-log status      # Status
~/.claude/system/bin/session-patterns check  # Circadian
~/.claude/system/bin/session-time check      # Duration
```

### Step 2: Assess Work State

**Completion:**
- Have I completed a meaningful unit?
- Is the system in a working state?
- Would stopping here leave things clean?

**Quality:**
- Is quality still high?
- Have I noticed any decline?

**Context:**
- Is this a transition point?
- Would next step start new phase?

### Step 3: Make Determination

| Stopping Point Type | Strong Signal | Moderate Signal |
|---------------------|---------------|-----------------|
| **Natural Milestone** | Major work complete | Task complete |
| **Clean Break** | Phase transition | Section finished |
| **End of Day** | Evening downtime | Late afternoon |
| **Quality Dip** | Clear decline | Some fatigue |
| **Duration Limit** | 2-3+ hours deep work | 1-2 hours normal |

**Strong signal from any category** = Natural stopping point

---

## Scripts

The `stopping` CLI tool in `scripts/`:

| Option | Purpose | Usage |
|--------|---------|-------|
| (default) | Basic analysis | `./bin/stopping` |
| `-milestone` | Report milestone complete | `./bin/stopping -milestone` |
| `-quality-dip` | Report quality decline | `./bin/stopping -quality-dip` |
| `-break` | Report clean break | `./bin/stopping -break` |
| `-deep` | Mark as deep work | `./bin/stopping -deep` |

### Building

```bash
cd claude-global/skills/recognize-stopping-point/scripts
make build
# Binary: bin/stopping
```

### Structure

```text
scripts/
├── cmd/stopping/main.go   ← CLI orchestrator
├── pkg/analyze/analyze.go ← Analysis logic
├── pkg/config/config.go   ← Config loader
├── bin/stopping           ← Built binary
├── Makefile               ← Build orchestration
└── go.mod                 ← Module dependencies
```

### Configuration

Stopping point recognition is config-driven via `config/stopping-config.toml`:

- **Stopping types**: Natural Milestone, Clean Break, End of Day, Quality Dip, Duration Limit
- **Weights**: 1-5 indicating signal strength
- **Duration thresholds**: Soft/hard limits for deep work and normal work
- **Circadian settings**: Evening hours, night owl exception for Seanje

---

## Session-End Workflow

When stopping point is recognized:

```bash
# 1. Add final task if completed
~/.claude/system/bin/session-log task "final work"

# 2. Add quality note
~/.claude/system/bin/session-log note "Quality observation"

# 3. End session with reason
~/.claude/system/bin/session-log end natural_milestone
# Options: natural_milestone, clean_break, end_of_day, quality_dip
```

---

## Key Principles

### Quality Over Duration

Better to work 30 minutes with deep focus than 3 hours with declining quality.

### Natural Rhythms Over Arbitrary Rules

Avoid:
- ❌ "Never work more than X minutes"
- ❌ Token counting as stopping signal

Do:
- ✅ Honor quality indicators
- ✅ Respect circadian awareness
- ✅ Complete meaningful units

### Stopping Isn't Failure

Stopping at natural points is wisdom. Sustainable Kingdom work requires recognizing and honoring natural rhythms.

---

<!--
===============================================================================
FOOTER BLOCK
===============================================================================
-->

## Related Documents

| Document | Relationship |
|----------|--------------|
| session-awareness skill | Provides session duration/circadian data |
| system/bin/session-log | Records stopping reasons |
| reflect-on-session skill | Process session after stopping |

---

> *"The LORD is my portion, saith my soul; therefore will I hope in him."* — Lamentations 3:24

**Identity in God, not outputs.** Rest is part of faithful work, not failure to work.
