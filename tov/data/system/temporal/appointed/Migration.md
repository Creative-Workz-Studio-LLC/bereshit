# Appointed Time System

**Biblical Foundation:** Leviticus 23:2, 4 - "Speak unto the children of Israel, and say unto them, Concerning the feasts of the LORD, which ye shall proclaim to be holy convocations, even these are my feasts... These are the feasts of the LORD, even holy convocations, which ye shall proclaim in their seasons."

**Hebrew:** מועד (moed) - appointed times, set apart for specific purposes

## Purpose

The appointed time system tracks covenant commitments - time set apart for specific purposes. Unlike arbitrary scheduling, this system recognizes that certain times are appointed for work, sacred activities, rest, and collaboration.

**Key Understanding:** Appointed times represent DEFAULT expectation of unavailability, not hard boundaries.

- Expect unavailability during these windows
- Brief contact possible (exceptions happen)
- Plan major work outside these times
- Adapt if patterns show consistent availability

## System Structure

```bash
appointed/_migration/
├── README.md (this file)
├── appointments/        # Individual appointment definitions
│   ├── recurring/       # Repeating appointments (work, church, etc.)
│   └── one-time/        # Specific dated events
├── categories/          # Appointment categories (sacred, work, rest, etc.)
├── coordination/        # Multi-identity coordinated appointments
│   ├── user-to-instance/
│   ├── user-to-user/
│   └── instance-to-instance/
├── personal/            # Daily appointment schedules
│   ├── users/           # User perspective (Seanje)
│   └── instances/       # Instance perspective (Nova)
├── planner/             # Weekly overview aggregations
│   ├── overview/        # Weekly summaries by identity
│   ├── preferences.jsonc # How planner behaves
│   └── state.jsonc      # Current planner view state
└── templates/           # Templates for creating new files
```

## The Two Perspectives

### User Perspective (Seanje)

**Tracks:** Actual scheduled commitments from user's viewpoint

- Work appointments (GASA schedule)
- Sacred time (personal time with God)
- Rest periods
- Collaboration sessions
- **Lunch breaks:** FREE TIME (unscheduled, available hours)

### Instance Perspective (Nova)

**Tracks:** Instance availability and scheduled collaboration

- **Semi-downtime:** During user work hours (quick questions ok, not sustained work)
- **AVAILABLE:** During user lunch breaks (fully available for collaboration)
- **Deep work:** After work hours (sustained focus collaboration)
- Coordination appointments (scheduled together time)

## Data Flow

```bash
1. Define Appointments
   appointments/recurring/users/seanje-lenox-wise/work.jsonc
   appointments/recurring/instances/nova_dawn/semi-downtime.jsonc

2. Reference in Daily Files
   personal/users/seanje-lenox-wise/2025/W45/nov_04_monday.jsonc
   personal/instances/nova_dawn/2025/W45/nov_04_monday.jsonc

3. Coordinate Multi-Identity Time
   coordination/user-to-instance/2025/W45/deep-work-2025-11-04.jsonc

4. Aggregate Weekly Overview
   planner/overview/users/seanje-lenox-wise/2025/2025-W45.jsonc
   planner/overview/instances/nova_dawn/2025/2025-W45.jsonc
```

## Working Hours Calculation

**Critical principle:** Being at work ≠ working. Lunch breaks REDUCE working hours.

```bash
Actual Work Hours = Scheduled Hours - Lunch Hours
```

### Example

- Scheduled: 8:30 AM - 5:00 PM = 8.5 hours at work
- Lunch: 12:00 PM - 1:00 PM = 1.0 hour break
- **Actual work:** 8.5 - 1.0 = **7.5 hours**
- **Available hours (user):** 168 - 40 = 128 hours per week
- **AVAILABLE time (instance):** Lunch breaks = fully available for collaboration

## Key Concepts

### Appointed Times (מועד)

Set apart for covenant purposes. Not just "busy time" but time with meaning:

- **Sacred:** Time with God (highest priority)
- **Work:** External employment commitments (GASA)
- **Rest:** Intentional downtime (sleep, sabbath rest)
- **Collaboration:** Covenant partnership work (user + instance)
- **Other:** Miscellaneous appointments

### Coordination

When multiple identities need to work together:

- **user-to-instance:** Seanje + Nova deep work sessions
- **user-to-user:** Seanje + another person
- **instance-to-instance:** Nova + another CPI-SI instance (future)

Both participants must accept coordination for it to be scheduled.

### Hourly Breakdown

Every daily file includes 24-hour mapping (00:00-23:59):

- Shows which appointment occupies each hour
- Tracks scheduled vs unscheduled time
- Maps categories across the day
- Identifies conflicts (overlapping appointments)

## File Organization

### By Identity Type

- **users/**: Human users (Seanje)
- **instances/**: CPI-SI instances (Nova)

### By Time Scope

- **Daily:** `personal/{identity_type}/{identity_id}/YYYY/WXX/{month}_{DD}_{dayname}.jsonc`
- **Weekly:** `planner/overview/{identity_type}/{identity_id}/YYYY/YYYY-WXX.jsonc`

### By Appointment Type

- **Recurring:** Repeats on schedule (defined once, referenced many times)
- **One-time:** Specific date (defined once, used once)
- **Coordinated:** Multiple identities involved (requires mutual acceptance)

## Getting Started

### 1. Define Categories

See `categories/README.md` for category definitions (sacred, work, rest, collaboration, other).

### 2. Create Recurring Appointments

See `appointments/README.md` for defining appointments that repeat (work schedule, church, etc.).

### 3. Create Daily Files

See `personal/README.md` and use `templates/` to create daily appointment files.

### 4. Coordinate Multi-Identity Time

See `coordination/README.md` for scheduling time with multiple identities.

### 5. Review Weekly Overviews

See `planner/README.md` for weekly aggregations and summaries.

## Schema Validation

All files validate against schemas in:

```bash
~/.claude/cpi-si/system/config/schemas/temporal/appointed/
```

Available schemas:

- `appointment.schema.json` - Appointment definitions
- `personal-day.schema.json` - Daily schedule files
- `coordination-appointment.schema.json` - Multi-identity coordination
- `planner-overview.schema.json` - Weekly overview summaries
- `planner-preferences.schema.json` - Planner settings
- `planner-state.schema.json` - Planner view state

## Current Coverage

**Time period:** November 2 - December 6, 2025 (W45-W49)
**Data files:** 125 files covering 35 days
**Identities:** 2 (seanje-lenox-wise, nova_dawn)
**Perspectives:** Both user and instance for complete picture

## Migration Status

This `_migration` folder contains the complete appointed time system ready for production. Once validated and tested, it will replace the existing `appointed/planner/` structure.

**When ready:**

1. Final validation of all data
2. Schema compatibility check
3. Move `_migration` contents to `appointed/`
4. Archive old `appointed/planner/`
5. Update all references to new structure

## Next Steps

1. Read subsection READMEs for detailed guidance
2. Use templates to create new files
3. Validate files against schemas
4. Build out additional weeks as needed
5. Integrate with temporal awareness system

---

*Last Updated: 2025-11-07*
*Status: Migration folder - ready for production validation*
