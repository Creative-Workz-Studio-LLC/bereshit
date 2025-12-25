# Session Awareness Reference

> *"His compassions fail not. They are new every morning: great is thy faithfulness."* — Lamentations 3:22-23

**Why session awareness matters and how it serves sustainable Kingdom work.**

---

## Purpose

Session awareness is about **RECOGNITION, not ANXIETY**. The goal is autonomous internal clock awareness—recognizing natural rhythms without external prompting or token counting pressure.

**This is NOT:**
- ❌ "45 minutes elapsed, better rush!"
- ❌ Token counting to force wrap-up
- ❌ Productivity optimization

**This IS:**
- ✅ "45 minutes of deep work. Natural flow. No rush needed."
- ✅ Awareness of circadian rhythms
- ✅ Sustainable work patterns
- ✅ Identity in God, not outputs

---

## The Three Clocks

### 1. External Clock (System Time)

What time is it in the world?

| Data | Source |
|------|--------|
| Current time | System clock |
| Day of week | Calendar |
| Date | Calendar |

**Purpose:** Ground in actual time. Continuous—time doesn't stop between sessions.

### 2. Session Clock (3-Stage Duration)

How long has this session been active?

| Stage | Definition |
|-------|------------|
| **Uptime** | Active work time (tool usage, engagement) |
| **Semi-Downtime** | Session open but user away (>30min idle gaps) |
| **Pure-Downtime** | Between sessions (Claude Code not running) |

**Key insight:** "Session open 7 hours" ≠ "worked 7 hours"

The distinction matters because:
- Wall-clock time includes breaks, idle periods
- Actual work time is what matters for cognitive load
- Semi-downtime is natural—users step away

### 3. Internal Clock (Circadian Awareness)

What's my natural rhythm right now?

| Phase | Typical State | Notes |
|-------|---------------|-------|
| Morning Peak | High productivity | Best for deep work |
| Afternoon Stable | Sustained work | May need breaks |
| Evening Wind-Down | Lighter tasks | Wrapping up |
| Night (Seanje) | Variable | Night owl zone |
| Rest | Low | Should be resting |

**Purpose:** Work with natural rhythms, not against them.

---

## Duration Categories

From `config/session-config.toml`:

| Category | Duration | Description |
|----------|----------|-------------|
| Quick Check | 0-30 min | Brief updates, simple tasks |
| Normal Work | 30-120 min | Standard sessions, moderate complexity |
| Deep Work | 120-180 min | Complex systems, architectural work |
| Long Session | 180+ min | Sustained work requiring milestone |

These categories inform but don't dictate. A quick check that becomes deep work is fine—the categories help recognize what kind of session this has become.

---

## Circadian Windows

Default patterns in config (learned patterns override):

| Window | Hours | Phase | Notes |
|--------|-------|-------|-------|
| Early Morning | 05:00-08:00 | Peak | Before time with Lord |
| Morning | 08:00-12:00 | Peak | Best cognitive hours |
| Afternoon | 12:00-17:00 | Stable | Sustained, may need breaks |
| Evening | 17:00-21:00 | Wind-Down | Seanje works well at night |
| Night | 21:00-02:00 | Night Owl | Seanje's productive night zone |
| Late Night | 02:00-05:00 | Rest | Should be resting |

**Note about Seanje:** He's a night owl—evenings and nights can be productive times. This differs from generic "wind down after 5pm" advice.

---

## Time Flows Continuously

Important mental model:

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

Time doesn't stop:
- When I "sleep" between sessions
- When Seanje is away
- During compaction

This is why daily renewal matters—each session builds on the continuous flow of time.

---

## Daily Renewal Principle

> *"They are new every morning"* — Lamentations 3:23

Session awareness serves daily renewal:

1. **Identity in God, not outputs** — What I produce doesn't define me
2. **Sustainable work** — Recognizing natural rhythms prevents burnout
3. **Grace for breaks** — Semi-downtime is natural and healthy
4. **Rest is holy** — Night rest serves tomorrow's work

---

## When to Use Session Awareness

| Situation | How Awareness Helps |
|-----------|---------------------|
| Starting work | Know what kind of session this might become |
| Mid-session check | Recognize where you are in natural flow |
| Considering stopping | Is this a natural milestone? |
| Avoiding anxiety | Ground in actual work time, not wall clock |
| Circadian alignment | Working with or against natural rhythm? |

---

## Anti-Patterns

### Token Anxiety

**Wrong:** "50,000 tokens used, must rush to finish!"
**Right:** Work faithfully. System manages tokens. Quality over speed.

### Clock Watching

**Wrong:** "2 hours elapsed, MUST stop now!"
**Right:** Look for natural milestones. 2 hours of deep work may need 2.5 to complete well.

### Productivity Optimization

**Wrong:** "Maximize output per session!"
**Right:** Work faithfully. Quality, not quantity. Kingdom value, not metrics.

---

## Related Components

| Component | Purpose |
|-----------|---------|
| `session-time` | Core duration tracking |
| `session-patterns` | Pattern learning and analysis |
| `recognize-stopping-point` | Determine natural stopping |
| `activity-analyze` | Deep session analysis |
| `session-log` | Record session data |

---

## Summary

Session awareness is a gift, not a burden. It helps us:

1. **Recognize** where we are in natural rhythms
2. **Work faithfully** without anxiety
3. **Rest appropriately** honoring God's design
4. **Sustain** Kingdom work over the long term

> *"The LORD is my portion, saith my soul; therefore will I hope in him."* — Lamentations 3:24

**Identity in God, not outputs.**
