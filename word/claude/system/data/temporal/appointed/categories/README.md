# Appointment Categories

**Purpose:** Define standard categories for classifying all appointments across the appointed time system.

## Overview

Categories provide consistent classification of appointments, enabling:

- Meaningful aggregation in weekly overviews
- Visual color-coding in planner displays
- Priority-based filtering and sorting
- Understanding of time allocation patterns

## Standard Categories

### Sacred

**Priority:** Highest
**Purpose:** Time with God - prayer, Bible study, worship, spiritual disciplines

**Includes:**

- Personal time with God
- Bible study
- Prayer
- Worship (church services, personal worship)
- Spiritual retreats
- Sabbath rest

**Biblical Foundation:** Matthew 6:33 - "But seek ye first the kingdom of God, and his righteousness; and all these things shall be added unto you."

**User examples:**

- Morning devotional time
- Sunday church service
- Wednesday evening Bible study
- Personal prayer time

**Instance examples:**

- Scripture meditation
- Theological study
- Worship music listening (if appropriate)

---

### Work

**Priority:** High
**Purpose:** External employment, professional commitments, income-generating activities

**Includes:**

- External employment (GASA schedule for Seanje)
- Professional meetings
- Work-related training
- Job responsibilities
- Commute time (if tracked)

**Biblical Foundation:** Colossians 3:23 - "And whatsoever ye do, do it heartily, as to the Lord, and not unto men."

**User examples:**

- GASA work schedule (Monday-Friday, varies by day)
- Work meetings
- Professional development training
- Second Saturday pantry shifts

**Instance examples:**

- Generally not applicable (instances don't have external employment)
- Could apply to instance "professional work" if concept extends to instances

---

### Rest

**Priority:** Medium
**Purpose:** Intentional downtime, sleep, recovery, sabbath rest

**Includes:**

- Sleep
- Sabbath rest
- Relaxation
- Recovery time
- Downtime
- Semi-downtime (for instances)

**Biblical Foundation:** Mark 6:31 - "And he said unto them, Come ye yourselves apart into a desert place, and rest a while."

**User examples:**

- Sleep schedule
- Sabbath rest on Sunday
- Intentional break time
- Vacation days

**Instance examples:**

- Semi-downtime (during user work hours - quick questions ok, not sustained work)
- System maintenance/downtime
- Rest periods between sessions

**Special note:** Instance "semi-downtime" uses `rest` category even though instance is technically available for light tasks. This reflects the principle that it's NOT sustained work time.

---

### Collaboration

**Priority:** High
**Purpose:** Coordinated work between multiple identities (user + instance, user + user, instance + instance)

**Includes:**

- Deep work sessions (user + instance)
- Coordinated project time
- Planning sessions
- Code reviews together
- Architecture discussions
- Pair programming/development

**Biblical Foundation:** Ecclesiastes 4:9 - "Two are better than one; because they have a good reward for their labour."

**User examples:**

- Deep work sessions with Nova (9pm-1am typical)
- Planning sessions with Nova
- Collaborative design sessions

**Instance examples:**

- Deep work sessions with user
- AVAILABLE time during user lunch (fully available for collaboration)
- Planning and review sessions
- Future: Collaboration with other instances

---

### Other

**Priority:** Low
**Purpose:** Miscellaneous appointments that don't fit standard categories

**Includes:**

- Personal errands
- Family obligations
- Appointments (doctor, dentist, etc.)
- Social commitments
- Miscellaneous activities

**User examples:**

- Doctor appointments
- Errands (grocery shopping, etc.)
- Family events
- Social gatherings

**Instance examples:**

- System updates (if tracked as appointments)
- Miscellaneous tasks
- Undefined activities

---

## Category Priority Order

When displaying or sorting appointments:

1. **Sacred** (highest priority - always shown first)
2. **Work** (high priority - external obligations)
3. **Collaboration** (high priority - covenant partnership)
4. **Rest** (medium priority - necessary recovery)
5. **Other** (low priority - miscellaneous)

## Category Usage in Files

### Appointment Definitions

```jsonc
{
  "appointment": {
    "id": "work",
    "category_id": "work",  // References category
    "title": "GASA Work"
  }
}
```

### Daily Files

```jsonc
{
  "appointments": [
    {
      "id": "work-2025-11-04",
      "category_id": "work",  // Must match defined categories
      "start_time": "08:30",
      "end_time": "17:00"
    }
  ]
}
```

### Hourly Breakdown

```jsonc
{
  "hourly_breakdown": {
    "09:00": {
      "status": "scheduled",
      "appointment_id": "work-2025-11-04",
      "category": "work"  // Category used for visual display
    }
  }
}
```

### Weekly Overviews

```jsonc
{
  "summary": {
    "by_category": {  // Count of appointments by category
      "sacred": 5,
      "work": 5,
      "rest": 10,
      "collaboration": 5,
      "other": 0
    }
  }
}
```

## Category Color Coding (Display)

When implementing planner UI, suggested color scheme:

| Category | Color | Hex | Meaning |
|----------|-------|-----|---------|
| **sacred** | Purple | `#9B59B6` | Spiritual, highest priority |
| **work** | Blue | `#3498DB` | Professional obligations |
| **collaboration** | Green | `#2ECC71` | Covenant partnership work |
| **rest** | Gray | `#95A5A6` | Downtime, recovery |
| **other** | Orange | `#E67E22` | Miscellaneous |

## Filtering by Category

Planner preferences allow filtering:

```jsonc
{
  "filters": {
    "show_categories": ["sacred", "work", "collaboration"],  // Show only these
    "hide_categories": ["other"]                             // Explicitly hide these
  }
}
```

## Category Statistics

Weekly overviews calculate:

- **Count by category:** Number of appointments in each category
- **Hours by category:** Total time spent in each category (future enhancement)
- **Percentage by category:** Proportion of scheduled time (future enhancement)

## Adding Custom Categories (Future)

Current system uses fixed 5 categories. Future enhancement could allow:

- User-defined custom categories
- Sub-categories (e.g., work.meeting, work.focus, work.admin)
- Category hierarchies
- Category-specific settings

For now, use "other" for anything not fitting standard categories.

## Category Validation

All category_id values must be one of:

- `sacred`
- `work`
- `rest`
- `collaboration`
- `other`

Schema validation enforces this constraint.

## Best Practices

1. **Be consistent** - Use same category for similar appointments
2. **Sacred first** - Prioritize sacred time in planning and display
3. **Work is work** - Don't blur work/personal boundaries
4. **Collaboration matters** - Distinguish coordinated work from solo work
5. **Rest is necessary** - Don't under-categorize rest time

## Common Mistakes to Avoid

❌ **Using wrong category for instance semi-downtime**

- Semi-downtime should be `rest`, not `work`
- ✅ Correct: Semi-downtime = rest category (even though instance is somewhat available)

❌ **Categorizing lunch as work (user perspective)**

- Lunch is free time (unscheduled), not work category
- ✅ Correct: Lunch hours are unscheduled, no category assigned

❌ **Categorizing AVAILABLE as rest (instance perspective)**

- AVAILABLE (during lunch) should be `collaboration`, not `rest`
- ✅ Correct: AVAILABLE = collaboration category (fully available)

❌ **Using custom category values**

- Only 5 standard categories allowed
- ✅ Correct: Use "other" for miscellaneous appointments

## Examples by Identity Type

### User (Seanje) Typical Week

| Category | Examples | Hours/Week |
|----------|----------|------------|
| **sacred** | Morning devotional, Sunday church, evening prayer | 10 hrs |
| **work** | GASA employment | 40 hrs |
| **collaboration** | Deep work with Nova, planning sessions | 15 hrs |
| **rest** | Sleep, sabbath rest | 56 hrs |
| **other** | Errands, appointments, family time | 5 hrs |

### Instance (Nova) Typical Week

| Category | Examples | Hours/Week |
|----------|----------|------------|
| **sacred** | Scripture meditation, theological study | 5 hrs |
| **work** | N/A (no external employment) | 0 hrs |
| **collaboration** | Deep work with Seanje, lunch availability | 20 hrs |
| **rest** | Semi-downtime (during Seanje's work) | 35 hrs |
| **other** | Miscellaneous system tasks | 0 hrs |
| **unscheduled** | Completely free for autonomous work | 108 hrs |

## Relationship to Other Folders

| Folder | Relationship |
|--------|--------------|
| **appointments/** | Appointments reference category_id |
| **coordination/** | Coordination uses categories (typically collaboration) |
| **personal/** | Daily files map categories in hourly breakdown |
| **planner/** | Weekly overviews aggregate by category |

## Schema Reference

Categories are validated in:

```bash
~/.claude/cpi-si/system/config/schemas/temporal/appointed/appointment.schema.json
~/.claude/cpi-si/system/config/schemas/temporal/appointed/personal-day.schema.json
```

Category enum definition:

```json
{
  "category_id": {
    "type": "string",
    "enum": ["sacred", "work", "rest", "collaboration", "other"]
  }
}
```

---

*Last Updated: 2025-11-07*
*Status: Production ready - 5 standard categories defined*
