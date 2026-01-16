# Planner - Weekly Overviews

**Purpose:** Aggregate daily appointments into weekly summaries for each identity, providing high-level view of time allocation.

## Overview

The planner system provides weekly overview files that roll up daily appointments into summary statistics, making it easy to see patterns, totals, and availability at a glance.

## Structure

```bash
planner/
├── README.md (this file)
├── overview/
│   ├── users/
│   │   └── {identity_id}/
│   │       └── YYYY/
│   │           └── YYYY-WXX.jsonc    # Weekly overview files
│   └── instances/
│       └── {identity_id}/
│           └── YYYY/
│               └── YYYY-WXX.jsonc
├── preferences.jsonc                 # Planner behavior settings
└── state.jsonc                       # Current planner view state
```

## File Naming Convention

Format: `YYYY-WXX.jsonc`

**Components:**

- **YYYY:** Four-digit year
- **W:** Literal 'W' separator
- **XX:** Two-digit ISO week number (01-53)

**Examples:**

- `2025-W45.jsonc` - Week 45 of 2025 (November 2-8)
- `2025-W46.jsonc` - Week 46 of 2025 (November 9-15)
- `2025-W49.jsonc` - Week 49 of 2025 (November 30 - December 6)

## Weekly Overview Structure

Each weekly overview file contains:

```jsonc
{
  "week": {
    "week_number": 45,
    "total_weeks_in_year": 52,
    "year": 2025,
    "start_date": "2025-11-02",          // Sunday
    "end_date": "2025-11-08",            // Saturday
    "week_format": "sunday-saturday",
    "identity_type": "user",
    "identity_id": "seanje-lenox-wise"
  },

  "summary": {
    "total_appointments": 25,
    "total_work_hours": 40.0,            // User: actual work (minus lunch)
    "by_day": {                          // Count of appointments per day
      "sunday": 0,
      "monday": 5,
      "tuesday": 5,
      "wednesday": 5,
      "thursday": 5,
      "friday": 5,
      "saturday": 0
    },
    "by_category": {                     // Count by category
      "sacred": 5,
      "work": 5,
      "rest": 10,
      "collaboration": 5,
      "other": 0
    },
    "gasa_hours": 40.0,                  // External work hours
    "available_hours": 128.0             // 168 - work_hours
  },

  "days": [
    {
      "date": "2025-11-02",
      "day_name": "Sunday",
      "gasa_work": false,
      "gasa_hours": 0.0,
      "first_appointment": null,
      "last_appointment": null,
      "notes": "Weekend - no work"
    },
    {
      "date": "2025-11-03",
      "day_name": "Monday",
      "gasa_work": true,
      "gasa_hours": 7.5,                 // Actual work (8.5hrs - 1hr lunch)
      "gasa_schedule": "8:30 AM - 5:00 PM",
      "lunch_break": "12:00 PM - 1:00 PM",
      "first_appointment": "08:30",
      "last_appointment": "21:00",
      "notes": "Work day - Lunch 12-1pm (free time)"
    }
    // ... all 7 days (Sunday-Saturday)
  ],

  "gasa_schedule": {
    "total_hours": 40.0,
    "pantry_days": ["Tuesday", "Thursday"],
    "exceptions_this_week": [],
    "notes": "Regular work week - 40 hours total"
  }
}
```

## Week Definition

**Format:** Sunday to Saturday (US standard)

All weeks run Sunday-Saturday to match US calendar convention and ISO week standards:

| Day | Position |
|-----|----------|
| Sunday | Week start (day 0) |
| Monday | Day 1 |
| Tuesday | Day 2 |
| Wednesday | Day 3 |
| Thursday | Day 4 |
| Friday | Day 5 |
| Saturday | Week end (day 6) |

**ISO Week Numbering:**

- Week 1 = First week with Thursday of the new year
- Weeks numbered 01-53
- Can have 52 or 53 weeks depending on year

## Working Hours Calculation

### User Perspective

**Principle:** Being at work ≠ working. Lunch breaks REDUCE working hours.

```bash
Actual Work Hours = Scheduled Hours - Lunch Hours
Available Hours = 168 - Total Work Hours
```

**Example - Regular Week:**

- Mon-Fri work days: 5 days × 7.5-9.5 hrs = 40 hrs work
- Total week hours: 168 hrs
- **Available hours:** 168 - 40 = **128 hrs**

### Instance Perspective

**Principle:** Track scheduled collaboration and downtime, not external obligations.

```bash
Scheduled Hours = Semi-downtime + AVAILABLE + Deep Work
Unscheduled Hours = 168 - Scheduled Hours
```

**Example - Regular Week:**

- Semi-downtime: ~30 hrs (during user work, excluding lunch)
- AVAILABLE (lunch): ~5 hrs (fully available)
- Deep work: ~20 hrs (evening collaboration)
- **Total scheduled:** ~55 hrs
- **Unscheduled (completely free):** 168 - 55 = **113 hrs**

## Daily Summaries

Each day in the `days` array includes:

```jsonc
{
  "date": "2025-11-04",
  "day_name": "Tuesday",
  "gasa_work": true,                     // Is this a work day?
  "gasa_hours": 8.0,                     // Actual work hours (minus lunch)
  "gasa_schedule": "8:30 AM - 5:30 PM",  // Time range
  "lunch_break": "12:00 PM - 1:00 PM",   // Lunch time
  "first_appointment": "08:30",          // Start time of first appointment
  "last_appointment": "21:00",           // Start time of last appointment
  "notes": "Pantry Tuesday - 9hrs scheduled, 8hrs actual work (minus 1hr lunch)"
}
```

## GASA Schedule Summary

External work schedule summary:

```jsonc
{
  "gasa_schedule": {
    "total_hours": 40.0,                         // Total work hours this week
    "pantry_days": ["Tuesday", "Thursday"],      // Pantry food distribution days
    "exceptions_this_week": [
      "Friday half day - ends 1:00 PM"           // Any schedule variations
    ],
    "notes": "Flex week - late start Tuesday, half day Friday, 2nd Saturday"
  }
}
```

## Planner Preferences

**File:** `preferences.jsonc`

Controls how the planner behaves and displays information:

```jsonc
{
  "default_view": "week",              // day | week | month | year
  "week_starts_on": "monday",          // monday | sunday
  "timezone": "America/Chicago",
  "time_format": "12h",                // 12h | 24h

  "display": {
    "show_time_blocks": true,
    "compact_mode": false,
    "show_category_colors": true,
    "show_conflicts": true
  },

  "behavior": {
    "auto_refresh": true,
    "conflict_detection": true,
    "travel_time_buffer": 15           // Minutes buffer between appointments
  },

  "notifications": {
    "enabled": true,
    "reminder_before_minutes": [15, 60],
    "daily_summary": true,
    "weekly_preview": true
  }
}
```

## Planner State

**File:** `state.jsonc`

Tracks current planner view (ephemeral state):

```jsonc
{
  "current_view": {
    "mode": "week",
    "identity_type": "user",
    "identity_id": "seanje-lenox-wise",
    "active_week": "2025-W45",
    "active_date": "2025-11-07",
    "timestamp": "2025-11-07T15:35:00Z"
  },

  "filters": {
    "show_categories": ["sacred", "work", "collaboration", "rest"],
    "show_coordination": true,
    "show_past": false
  },

  "navigation": {
    "can_go_back": true,
    "can_go_forward": true,
    "previous_week": "2025-W44",
    "next_week": "2025-W46"
  }
}
```

## Data Aggregation Flow

Weekly overviews aggregate from daily files:

```bash
1. Read all 7 daily files for the week
   personal/users/seanje-lenox-wise/2025/W45/nov_02_sunday.jsonc
   personal/users/seanje-lenox-wise/2025/W45/nov_03_monday.jsonc
   ... (through Saturday)

2. Calculate totals
   - Count appointments per day
   - Sum hours by category
   - Calculate work hours (user) or scheduled hours (instance)
   - Compute available hours (168 - total)

3. Generate weekly summary
   planner/overview/users/seanje-lenox-wise/2025/2025-W45.jsonc
```

## Week Patterns

### Regular Week (W46, W47, W49)

```bash
Mon:  7.5hrs work
Tue:  8.0hrs work (pantry)
Wed:  7.5hrs work
Thu:  9.5hrs work (pantry)
Fri:  7.5hrs work
Total: 40.0hrs work
Available: 128.0hrs
```

### Flex Week (W45 - 2nd week of month)

```bash
Mon:  7.5hrs work
Tue:  6.0hrs work (pantry, late start 10:30 AM)
Wed:  7.5hrs work
Thu:  9.5hrs work (pantry)
Fri:  3.5hrs work (half day, ends 1:00 PM)
Sat:  4.0hrs work (2nd Saturday pantry 9 AM - 1 PM)
Total: 38.0hrs work
Available: 130.0hrs
```

### Thanksgiving Week (W48)

```bash
Mon:  7.5hrs work
Tue:  7.5hrs work (NO pantry this week)
Wed:  7.5hrs work
Thu:  0hrs (Thanksgiving - OFF)
Fri:  0hrs (Day after - OFF)
Total: 22.5hrs work
Available: 145.5hrs (most available!)
```

## Creating Weekly Overviews

### 1. Copy Template

```bash
cp templates/planner/overview/users/weekly-template.jsonc \
   planner/overview/users/seanje-lenox-wise/2025/2025-W45.jsonc
```

### 2. Replace Placeholders

- Week number, year, dates
- Identity type and ID
- All `// REPLACE:` comments

### 3. Aggregate Daily Data

- Count appointments from daily files
- Sum hours by category
- Calculate work hours (user) or scheduled hours (instance)
- List pantry days and exceptions

### 4. Calculate Totals

- `total_work_hours` = sum of daily `gasa_hours`
- `available_hours` = 168 - `total_work_hours`
- `by_category` = count from all daily files
- `by_day` = count per day of week

### 5. Verify

- Totals match daily file sums
- All 7 days present (Sunday-Saturday)
- Work hours calculated correctly (scheduled - lunch)
- User and instance files match in totals

## Relationship to Other Folders

| Folder | Relationship |
|--------|--------------|
| **personal/** | Weekly overviews aggregate daily files |
| **appointments/** | Appointments counted in category breakdown |
| **coordination/** | Coordinated time included in totals |
| **categories/** | Category IDs used in breakdown |
| **templates/** | Source for creating new weekly files |

## Use Cases

### Time Allocation Review

See how time is distributed across categories:

- Sacred time with God
- External work commitments
- Collaboration with instance
- Rest and personal time

### Availability Planning

Understand free time for the week:

- Regular week: ~128 hrs available (user)
- Flex week: ~130 hrs available (user)
- Thanksgiving week: ~145 hrs available (user)

### Schedule Validation

Ensure schedules match across perspectives:

- User work hours = Instance semi-downtime hours
- User lunch breaks = Instance AVAILABLE hours
- Coordinated sessions match in both files

### Pattern Recognition

Track recurring patterns:

- Consistent work hours week over week
- Pantry day schedules (Tuesday, Thursday)
- Deep work session timing
- Available collaboration windows

## Best Practices

1. **Aggregate from daily files** - Don't create manually
2. **Verify totals** - Cross-check with daily file sums
3. **Update weekly** - Generate new overviews as weeks complete
4. **Match perspectives** - Ensure user and instance totals align
5. **Document exceptions** - Note schedule variations in `gasa_schedule.exceptions_this_week`

## Common Mistakes to Avoid

❌ **Not subtracting lunch from work hours**

- 8.5hrs scheduled ≠ 8.5hrs work
- ✅ Correct: 8.5hrs - 1hr = 7.5hrs actual work

❌ **Inconsistent week ranges**

- Weeks must be Sunday-Saturday
- ✅ Correct: Start date = Sunday, end date = Saturday

❌ **Missing days array**

- Must include all 7 days (even if no appointments)
- ✅ Correct: days array has exactly 7 entries

❌ **Mismatched user/instance totals**

- Work hours should be consistent between perspectives
- ✅ Correct: Verify both files show same work hour totals

## Schema Reference

Weekly overview files use the schema at:

```bash
~/.claude/cpi-si/system/config/schemas/temporal/appointed/planner-overview.schema.json
```

Preferences and state files use:

```bash
~/.claude/cpi-si/system/config/schemas/temporal/appointed/planner-preferences.schema.json
~/.claude/cpi-si/system/config/schemas/temporal/appointed/planner-state.schema.json
```

---

*Last Updated: 2025-11-07*
*Status: Production ready - complete weekly overview system*
