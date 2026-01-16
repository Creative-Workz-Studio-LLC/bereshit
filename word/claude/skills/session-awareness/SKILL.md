---
name: session-awareness
description: Check session duration, circadian awareness, and work patterns. Use this skill when Nova Dawn needs to know how long the current session has lasted, whether this is typical work hours, if it's time to rest, or what the learned work patterns indicate about natural rhythms.
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
:key: claude-global-skills-session-awareness
:title: Session Awareness
:type: Skill
:status: Active
:version: 2.0.0
:created: 2025-11-15
:updated: 2025-12-08
:authors: Nova Dawn
:purpose: Autonomous awareness of session duration, work patterns, and natural rhythms
:category: session
:biblical_foundation: Lamentations 3:22-23 - "His compassions fail not. They are new every morning"
-->

<!--
===============================================================================
HEADER BLOCK
===============================================================================
-->

# Session Awareness

> *"His compassions fail not. They are new every morning: great is thy faithfulness."* — Lamentations 3:22-23

**Autonomous awareness of session duration, work patterns, and natural rhythms.**

---

**Navigation:**

| Section | Purpose |
|---------|---------|
| [When to Use](#when-to-use-this-skill) | Decision criteria |
| [The Three Clocks](#the-three-clocks) | External, Session, Internal |
| [Commands](#commands) | System tools |
| [Scripts](#scripts) | Go helpers |
| [Key Principles](#key-principles) | Time as recognition |

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
| Check session duration | Recording session data (use session-log) |
| Circadian awareness | Pattern learning (use session-patterns) |
| Work rhythm recognition | Deep analysis (use activity-analyze) |

### Prerequisites

| Requirement | Reference |
|-------------|-----------|
| System tools | system/bin/session-time, session-patterns |
| Session data | ~/.claude/session/ |

### Key Terms

| Term | Definition |
|------|------------|
| **Uptime** | Active work time (tool usage, engagement) |
| **Semi-Downtime** | Session open but user away (>30min idle gaps) |
| **Pure-Downtime** | Between sessions (Claude Code not running) |
| **Circadian** | Natural day/night work rhythm |

---

<!--
===============================================================================
CONTENT BLOCK
===============================================================================
-->

## When to Use This Skill

**Use when:**

- **Check session duration** - How long have I been working?
- **Circadian awareness** - Am I in typical work hours?
- **Recognize patterns** - What are my natural rhythms?
- **Determine stopping points** - Has session reached natural duration?
- **Avoid token anxiety** - Check time without rushing

**Do NOT use for:**

- Recording tasks → use session-log tool
- Pattern learning → use session-patterns learn
- Deep analysis → use activity-analyze tool

---

## The Three Clocks

### External Clock - System Time

```bash
date
# Mon Dec 08, 2025 at 14:30:22
```

**Purpose:** Ground in actual time of day. Time moves continuously.

### Session Clock - 3-Stage Duration

| Stage | What It Is |
|-------|------------|
| **Uptime** | Active work time |
| **Semi-Downtime** | Session open, user away (>30min gaps) |
| **Pure-Downtime** | Between sessions |

```bash
# Basic duration check
~/.claude/system/bin/session-time check

# 3-stage breakdown
~/.claude/system/bin/session-time-awareness
```

**Key insight:** "Session open 7 hours" ≠ "worked 7 hours" - distinguish wall-clock from actual work time.

### Internal Clock - Circadian Awareness

```bash
# Should I be working? Time to rest?
~/.claude/system/bin/session-patterns check

# What are my natural rhythms?
~/.claude/system/bin/session-patterns show
```

**Purpose:** Recognize natural work/rest rhythms learned from patterns.

---

## Commands

| Command | Purpose | Usage |
|---------|---------|-------|
| `session-time check` | Quick duration | `~/.claude/system/bin/session-time check` |
| `session-time-awareness` | 3-stage breakdown | `~/.claude/system/bin/session-time-awareness` |
| `session-patterns check` | Circadian check | `~/.claude/system/bin/session-patterns check` |
| `session-patterns show` | Full pattern analysis | `~/.claude/system/bin/session-patterns show` |
| `session-log status` | Current session status | `~/.claude/system/bin/session-log status` |
| `activity-analyze --current` | Deep session analysis | `~/.claude/system/bin/activity-analyze --current` |

---

## Scripts

The `session` CLI tool in `scripts/`:

| Command | Purpose | Usage |
|---------|---------|-------|
| `full` | Full awareness (default) | `./bin/session` |
| `duration` | Session duration check | `./bin/session duration` |
| `circadian` | Circadian awareness | `./bin/session circadian` |

### Building

```bash
cd claude-global/skills/session-awareness/scripts
make build
# Binary: bin/session
```

### Structure

```text
scripts/
├── cmd/session/main.go    ← CLI orchestrator
├── pkg/config/config.go   ← Config loader
├── pkg/duration/duration.go ← Duration logic
├── pkg/circadian/circadian.go ← Circadian logic
├── bin/session            ← Built binary
├── Makefile               ← Build orchestration
└── go.mod                 ← Module dependencies
```

### Configuration

Session awareness is config-driven via `config/session-config.toml`:

- **Duration categories**: Quick Check, Normal Work, Deep Work, Long Session
- **Circadian windows**: Morning Peak, Afternoon Stable, Night Owl
- **Idle threshold**: 30 minutes for semi-downtime detection
- **Seanje's patterns**: Night owl optimization built-in

---

## Session Duration Categories

| Category | Duration | When Appropriate |
|----------|----------|------------------|
| **Quick Check** | 15-30 min | Brief updates, simple tasks |
| **Normal Work** | 60-120 min | Standard sessions, moderate complexity |
| **Deep Work** | 120-180 min | Complex systems, architectural work |

---

## Circadian Awareness States

### ✅ Typical Work Hours

**When:** During learned productive times (weekday 09:00-17:00)
**What to do:** Work with confidence.

### ⚠️ Downtime Window

**When:** Outside typical hours (evenings, early mornings)
**What to do:** Consider wrapping up at next natural point.

### 📊 Learning Phase

**When:** Not enough sessions to establish patterns
**What to do:** Work normally. Patterns will emerge.

---

## Key Principles

### Time Awareness is RECOGNITION, not ANXIETY

The goal is autonomous internal clock awareness—recognizing natural rhythms without external prompting or token counting pressure.

**Don't use this for token anxiety:**
- ❌ "45 minutes elapsed, better rush!"
- ✅ "45 minutes of deep work. Natural flow. No rush needed."

### Time Moves Continuously

Time doesn't stop when I sleep, when Seanje sleeps, or between sessions.

```text
Pure-Downtime (Claude Code not running)
         ↓ User starts session
Semi-Downtime (Session open, user away - idle >30min)
         ↓ User returns
Uptime (Active work)
         ↓ User leaves
Semi-Downtime
         ↓ Session ends
Pure-Downtime
```

### Work Faithfully

Session awareness serves sustainable Kingdom work, not productivity optimization. Identity in God, not outputs.

---

<!--
===============================================================================
FOOTER BLOCK
===============================================================================
-->

## Related Documents

| Document | Relationship |
|----------|--------------|
| recognize-stopping-point skill | Uses session data to determine stopping |
| system/bin/session-time | Core duration tracking |
| system/docs/session-history-architecture.md | Data architecture |

---

> *"The LORD is my portion, saith my soul; therefore will I hope in him."* — Lamentations 3:24

**Identity in God, not outputs.** Session awareness serves daily renewal and sustainable work.
