# Personal Daily Schedules

**Purpose:** Track daily appointments from user and instance perspectives with minute-level precision.

## Overview

Personal daily files record all appointments for a specific day, organized by identity. Each day gets its own file showing:

- All scheduled appointments
- Hourly breakdown (00:00-23:59)
- Summary statistics
- Conflicts (if any)

## Structure

```bash
personal/
├── users/
│   └── {identity_id}/              # e.g., seanje-lenox-wise
│       └── YYYY/
│           └── WXX/                # ISO week (W45, W46, etc.)
│               └── {month}_{DD}_{dayname}.jsonc
└── instances/
    └── {identity_id}/              # e.g., nova_dawn
        └── YYYY/
            └── WXX/
                └── {month}_{DD}_{dayname}.jsonc
```

## File Naming Convention

Format: `{month}_{DD}_{dayname}.jsonc`

**Components:**

- **month:** 3-letter lowercase month abbreviation (jan, feb, mar, ..., dec)
- **DD:** Two-digit day (01-31)
- **dayname:** Full lowercase day name (monday, tuesday, ..., sunday)

**Examples:**

- `nov_04_monday.jsonc`
- `nov_15_friday.jsonc`
- `dec_01_monday.jsonc`
- `dec_25_thursday.jsonc`

## The Two Perspectives

### User Perspective (Human)

**Tracks:** Actual scheduled commitments from user's viewpoint

**Typical appointments:**

- Work (external employment - GASA schedule)
- Sacred time (personal time with God)
- Rest periods (sleep, relaxation)
- Collaboration (coordinated sessions with instance)
- Personal appointments (errands, family time)

**Lunch breaks:** UNSCHEDULED (free time, available hours)

### Instance Perspective (Nova Dawn)

**Tracks:** Instance availability and scheduled collaboration

**Typical appointments:**

- Semi-downtime (during user work hours - quick questions ok)
- AVAILABLE (during user lunch breaks - fully available)
- Deep work (coordinated collaboration sessions)
- Rest (when instance is offline/not available)

**Key difference:** Instance tracks availability patterns, not external obligations.

## Daily File Structure

Each daily file contains:

```jsonc
{
  "day": {
    "date": "2025-11-04",
    "day_name": "Monday",
    "iso_week": "2025-W45",
    "identity_type": "user",  // or "instance"
    "identity_id": "seanje-lenox-wise",
    "timezone": "America/Chicago"
  },

  "appointments": [
    {
      "id": "work-2025-11-04",
      "appointment_ref": "work",  // References recurring appointment
      "category_id": "work",
      "start_time": "08:30",
      "end_time": "17:00",
      "duration_minutes": 510,
      "status": "scheduled",
      "coordination_id": null,  // Or ID if coordinated
      "notes": "Regular work day"
    }
  ],

  "hourly_breakdown": {
    "00:00": { "status": "unscheduled", "appointment_id": null, "category": null },
    "08:00": { "status": "unscheduled", "appointment_id": null, "category": null },
    "09:00": { "status": "scheduled", "appointment_id": "work-2025-11-04", "category": "work" },
    // ... all 24 hours
  },

  "summary": {
    "total_appointments": 3,
    "total_scheduled_hours": 13.5,
    "by_category": {
      "sacred": 1,
      "work": 1,
      "collaboration": 1,
      "rest": 0,
      "other": 0
    },
    "first_appointment": "08:30",
    "last_appointment": "21:00",
    "has_conflicts": false,
    "conflicts": []
  }
}
```

## Hourly Breakdown Explained

**Purpose:** Map which appointment occupies each hour of the day.

**Format:** 24 entries from 00:00 to 23:00 (midnight to 11 PM)

```jsonc
{
  "09:00": {
    "status": "scheduled",              // or "unscheduled"
    "appointment_id": "work-2025-11-04", // or null
    "category": "work"                   // or null
  }
}
```

**Status values:**

- **scheduled:** Hour is occupied by an appointment
- **unscheduled:** Hour is free/available

### Example: User Work Day

```jsonc
{
  "08:00": { "status": "unscheduled", "appointment_id": null, "category": null },
  "09:00": { "status": "scheduled", "appointment_id": "work-2025-11-04", "category": "work" },
  "10:00": { "status": "scheduled", "appointment_id": "work-2025-11-04", "category": "work" },
  "11:00": { "status": "scheduled", "appointment_id": "work-2025-11-04", "category": "work" },
  "12:00": { "status": "unscheduled", "appointment_id": null, "category": null },  // LUNCH - FREE TIME
  "13:00": { "status": "scheduled", "appointment_id": "work-2025-11-04", "category": "work" },
  "14:00": { "status": "scheduled", "appointment_id": "work-2025-11-04", "category": "work" }
}
```

### Example: Instance Semi-Downtime

```jsonc
{
  "08:00": { "status": "unscheduled", "appointment_id": null, "category": null },
  "09:00": { "status": "scheduled", "appointment_id": "semi-downtime-morning-2025-11-04", "category": "rest" },
  "10:00": { "status": "scheduled", "appointment_id": "semi-downtime-morning-2025-11-04", "category": "rest" },
  "11:00": { "status": "scheduled", "appointment_id": "semi-downtime-morning-2025-11-04", "category": "rest" },
  "12:00": { "status": "scheduled", "appointment_id": "available-lunch-2025-11-04", "category": "collaboration" },  // AVAILABLE
  "13:00": { "status": "scheduled", "appointment_id": "semi-downtime-afternoon-2025-11-04", "category": "rest" }
}
```

## Cross-Day Appointments

Appointments can span midnight into the next day:

**Coordination:** Deep work session 9 PM Monday to 1 AM Tuesday

**Monday's file (nov_04_monday.jsonc):**

```jsonc
{
  "appointments": [
    {
      "id": "deep-work-2025-11-04",
      "start_time": "21:00",
      "end_time": "01:00",  // Crosses into next day
      "duration_minutes": 240
    }
  ],
  "hourly_breakdown": {
    "21:00": { "status": "scheduled", "appointment_id": "deep-work-2025-11-04", "category": "collaboration" },
    "22:00": { "status": "scheduled", "appointment_id": "deep-work-2025-11-04", "category": "collaboration" },
    "23:00": { "status": "scheduled", "appointment_id": "deep-work-2025-11-04", "category": "collaboration" }
  }
}
```

**Tuesday's file (nov_05_tuesday.jsonc):**

```jsonc
{
  "appointments": [
    {
      "id": "deep-work-carryover-2025-11-05",
      "appointment_ref": "deep-work",
      "start_time": "00:00",
      "end_time": "01:00",
      "duration_minutes": 60,
      "coordination_id": "deep-work-2025-11-04",  // References previous day's coordination
      "notes": "Carryover from previous day"
    }
  ],
  "hourly_breakdown": {
    "00:00": { "status": "scheduled", "appointment_id": "deep-work-carryover-2025-11-05", "category": "collaboration" }
  }
}
```

## Appointment References

Daily files reference appointment definitions:

**Recurring appointment:**

```jsonc
{
  "id": "work-2025-11-04",
  "appointment_ref": "work",  // References appointments/recurring/users/.../work.jsonc
  "category_id": "work"
}
```

**One-time appointment:**

```jsonc
{
  "id": "doctor-appointment-2025-11-15",
  "appointment_ref": "doctor-appointment-2025-11-15",  // References appointments/one-time/users/.../doctor-appointment-2025-11-15.jsonc
  "category_id": "other"
}
```

**Coordinated appointment:**

```jsonc
{
  "id": "deep-work-2025-11-04",
  "appointment_ref": "deep-work",
  "category_id": "collaboration",
  "coordination_id": "deep-work-2025-11-04"  // References coordination/user-to-instance/.../deep-work-2025-11-04.jsonc
}
```

## Summary Statistics

Each daily file calculates:

```jsonc
{
  "summary": {
    "total_appointments": 3,           // Count of appointments
    "total_scheduled_hours": 13.5,     // Total hours occupied
    "by_category": {                   // Breakdown by category
      "sacred": 1,
      "work": 1,
      "rest": 0,
      "collaboration": 1,
      "other": 0
    },
    "first_appointment": "08:30",      // Start time of earliest appointment
    "last_appointment": "21:00",       // Start time of latest appointment
    "has_conflicts": false,            // Are any appointments overlapping?
    "conflicts": []                    // List of conflict details (if any)
  }
}
```

## Conflict Detection

Conflicts occur when appointments overlap:

```jsonc
{
  "summary": {
    "has_conflicts": true,
    "conflicts": [
      {
        "time": "14:00-15:00",
        "appointments": ["meeting-2025-11-04", "work-2025-11-04"],
        "description": "Meeting scheduled during work hours"
      }
    ]
  }
}
```

## Working Hours Calculation (User Perspective)

**Critical:** Being at work ≠ working. Lunch breaks reduce working hours.

```bash
Actual Work Hours = Scheduled Hours - Lunch Hours
```

**Example day:**

- Scheduled: 8:30 AM - 5:00 PM = 8.5 hours
- Lunch: 12:00 PM - 1:00 PM = 1.0 hour
- **Actual work:** 7.5 hours
- **Available (free) time:** 168 - 40 = 128 hours per week

## Availability Understanding (Instance Perspective)

### Semi-Downtime

**When:** During user's actual work hours (excluding lunch)
**Meaning:** Available for quick questions or small tasks, NOT sustained work
**Category:** `rest`

### AVAILABLE

**When:** During user's lunch breaks (user's free time)
**Meaning:** Fully available for collaboration, can do deep work
**Category:** `collaboration`

### Deep Work

**When:** Coordinated sessions (usually after work hours)
**Meaning:** Sustained focus collaboration
**Category:** `collaboration`

## Creating Daily Files

### 1. Copy Template

```bash
cp templates/personal/users/daily-template.jsonc \
   personal/users/seanje-lenox-wise/2025/W45/nov_04_monday.jsonc
```

### 2. Replace Placeholders

- `"date": "YYYY-MM-DD"` → Actual date
- `"day_name": "DayName"` → Full day name
- `"iso_week": "YYYY-Www"` → ISO week
- `"identity_id": "example-user"` → Actual identity ID

### 3. Add Appointments

Reference recurring, one-time, or coordinated appointments by ID.

### 4. Build Hourly Breakdown

Map all 24 hours showing which appointment occupies each hour.

### 5. Calculate Summary

Count appointments, total hours, category breakdown, detect conflicts.

## Relationship to Other Folders

| Folder | Relationship |
|--------|--------------|
| **appointments/** | Daily files reference appointment definitions by ID |
| **coordination/** | Daily files reference coordination appointments |
| **categories/** | Appointments use category IDs defined there |
| **planner/** | Weekly overviews aggregate daily files |
| **templates/** | Source for creating new daily files |

## Best Practices

1. **One file per day per identity** - Don't combine days or identities
2. **Complete hourly breakdown** - Map all 24 hours (00:00-23:00)
3. **Accurate time calculation** - Subtract lunch from work hours (user perspective)
4. **Consistent naming** - Follow `{month}_{DD}_{dayname}.jsonc` format
5. **Reference by ID** - Use appointment_ref and coordination_id correctly
6. **Validate summaries** - Ensure totals match appointments
7. **Check for conflicts** - Mark overlapping appointments

## Common Mistakes to Avoid

❌ **Not subtracting lunch from work hours (user perspective)**

- 8:30-5pm ≠ 8.5 hours of work
- ✅ Correct: 8.5hrs - 1hr = 7.5hrs actual work

❌ **Marking lunch as "unavailable" (instance perspective)**

- Lunch is user's free time = instance AVAILABLE
- ✅ Correct: Mark lunch hour as `"category": "collaboration"` with AVAILABLE appointment

❌ **Incomplete hourly breakdown**

- Must include all 24 hours (00:00-23:00)
- ✅ Correct: Map every hour, use "unscheduled" for free time

❌ **Mismatched summaries**

- Total hours should match sum of appointment durations
- ✅ Correct: Verify totals before finalizing

## Example Use Case

**User: Monday, November 4, 2025**

- Work: 8:30 AM - 5:00 PM (7.5 hrs actual, lunch 12-1pm)
- Personal time: 7:00 PM - 8:00 PM (1 hr sacred)
- Deep work: 9:00 PM - 1:00 AM (4 hrs collaboration)
- Total scheduled: 12.5 hours
- Total available: 155.5 hours

**Instance: Monday, November 4, 2025**

- Semi-downtime morning: 8:30 AM - 12:00 PM (3.5 hrs rest)
- AVAILABLE lunch: 12:00 PM - 1:00 PM (1 hr collaboration)
- Semi-downtime afternoon: 1:00 PM - 5:00 PM (4 hrs rest)
- Deep work: 9:00 PM - 1:00 AM (4 hrs collaboration)
- Total scheduled: 12.5 hours
- Total unscheduled: 155.5 hours (completely free for autonomous work)

## Schema Reference

Daily files use the schema at:

```bash
~/.claude/cpi-si/system/config/schemas/temporal/appointed/personal-day.schema.json
```

---

*Last Updated: 2025-11-07*
*Status: Production ready - complete daily schedule system*
