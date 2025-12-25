# Internal Calendar Structure

**Purpose:** Track Nova Dawn's work schedule, plans, and progress - MY timeline through MY work.

## Core Concept

**External calendar:** World's dates and time (November 4, 2025, Tuesday)
**Internal calendar:** My work schedule and progress (Iteration 4, Day 3 of 14, Session 2 of 12)

Together: I know where I am in time AND where I am in my work.

## Data Structure

### Schedule File Format

**Location:** `~/.claude/schedule/current-schedule.json`

```json
{
  "schedule_id": "iteration-4-semantic-analysis",
  "work_item": "Compiler Iteration 4: Semantic Analysis",
  "status": "in_progress",
  "start_date": "2025-11-03",
  "estimated_end_date": "2025-11-17",
  "actual_end_date": null,

  "estimates": {
    "total_days": 14,
    "total_sessions": 12,
    "total_uptime_hours": 12,
    "avg_session_uptime_minutes": 60
  },

  "progress": {
    "days_elapsed": 2,
    "sessions_completed": 1,
    "uptime_hours_actual": 1.5,
    "current_session_number": 2
  },

  "milestones": [
    {
      "name": "Type system foundation",
      "estimated_sessions": 3,
      "status": "in_progress",
      "completed_date": null
    },
    {
      "name": "Symbol table implementation",
      "estimated_sessions": 4,
      "status": "pending",
      "completed_date": null
    },
    {
      "name": "Semantic validation",
      "estimated_sessions": 3,
      "status": "pending",
      "completed_date": null
    },
    {
      "name": "Integration and testing",
      "estimated_sessions": 2,
      "status": "pending",
      "completed_date": null
    }
  ],

  "velocity": {
    "planned_sessions_per_week": 6,
    "actual_sessions_per_week": null,
    "planned_uptime_per_session": 60,
    "actual_uptime_per_session": 90
  },

  "adjustments": [
    {
      "date": "2025-11-04",
      "reason": "Sessions taking longer than estimated (90m vs 60m)",
      "adjustment": "Extended total days from 10 to 14",
      "revised_estimate": "14 days total"
    }
  ],

  "notes": [
    "Building on Iterations 1-3 foundation",
    "Type system more complex than parser",
    "Quality target: +85 health scores maintained"
  ]
}
```

### Schedule States

| Status | Meaning |
|--------|---------|
| **planned** | Schedule created, work not yet started |
| **in_progress** | Currently working on this plan |
| **paused** | Temporarily paused (e.g., waiting on decisions) |
| **completed** | Work finished, schedule archived |
| **cancelled** | Plan cancelled before completion |

### Milestone States

| Status | Meaning |
|--------|---------|
| **pending** | Not yet started |
| **in_progress** | Currently working on this milestone |
| **completed** | Milestone finished |
| **blocked** | Cannot proceed (dependency or blocker) |

## Schedule vs Session

**Schedule:** Long-term plan (Iteration 4: 14 days, 12 sessions)
**Session:** Single work period (Today's session: 45m uptime so far)

Schedule contains many sessions. Sessions contribute to schedule progress.

## Data Storage

```
~/.claude/schedule/
├── current-schedule.json           # Active work plan
├── history/
│   ├── iteration-3-integration.json
│   └── iteration-2-parser.json
└── templates/
    └── iteration-template.json
```

**current-schedule.json:** The active plan I'm currently working on
**history/*.json:** Completed schedules for reference and velocity learning
**templates/*.json:** Templates for common schedule types

## Integration Points

### Session-log Integration

When starting session:
```bash
session-log start
# Reads current-schedule.json
# Displays: "Session 2 of 12 for Iteration 4 (Day 3 of 14)"
```

### Session-time-awareness Integration

When checking time awareness:
```bash
session-time-awareness
# Shows:
# - Uptime: 45m (active work)
# - Schedule: Session 2 of 12, Day 3 of 14
# - Progress: 8% complete (1/12 sessions done)
```

### Pattern Learning Integration

When analyzing velocity:
```bash
session-patterns show
# Includes:
# - Planned vs actual session duration
# - Planned vs actual sessions per week
# - Velocity trends
```

## Key Questions Answered

**"Where am I?"**
- Day 3 of 14 estimated
- Session 2 of 12 planned
- Currently: Type system foundation milestone

**"What's next?"**
- Current milestone: Type system foundation (session 2 of 3)
- Next milestone: Symbol table implementation (4 sessions)

**"Am I on track?"**
- Planned: 60m/session, Actual: 90m/session
- Adjustment: Extended timeline 10→14 days
- Status: On track with revised estimate

**"How's my velocity?"**
- Sessions taking 50% longer than estimated
- Quality staying high (+85)
- Sustainable pace maintained

## Why This Matters

**Autonomy:** I know where I am in MY work without being told
**Planning:** Can estimate future work based on actual velocity
**Progress:** Track planned vs actual, adjust realistically
**Communication:** "Session 2 of 12, Day 3 of 14" is clear progress report
**Learning:** Velocity data improves future estimates

**This is MY calendar** - my work timeline, my plans, my progress tracking.

## Design Principles

1. **Simple first:** Basic schedule tracking before complex features
2. **Planned vs actual:** Always track both for learning
3. **Adjustable:** Plans change - make adjustments explicit
4. **Historical:** Archive completed schedules for velocity learning
5. **Integrated:** Schedule informs session tools, not isolated system

---

**Next Steps:**
1. Implement schedule-init (create new schedule)
2. Implement schedule-check (show current progress)
3. Implement schedule-update (adjust based on reality)
4. Integrate with existing session tools
