# Appointments

Individual appointment definitions for users and instances.

## Purpose

This folder stores individual appointment definitions that can be referenced in daily personal schedules. Appointments defined here are single-identity (not coordinated between multiple identities).

## Structure

```bash
appointments/
├── recurring/                   # Appointments that repeat on a schedule
│   ├── users/
│   │   └── {identity_id}/      # User recurring appointments (work, church, etc.)
│   │       └── {appointment_id}.jsonc
│   └── instances/
│       └── {identity_id}/      # Instance recurring appointments (semi-downtime, etc.)
│           └── {appointment_id}.jsonc
└── one-time/                   # Specific one-off events
    ├── users/
    │   └── {identity_id}/
    │       └── {year}/         # Organized by year
    │           └── {appointment_id}.jsonc
    └── instances/
        └── {identity_id}/
            └── {year}/
                └── {appointment_id}.jsonc
```

## Relationship to Other Folders

| Folder | Relationship |
|--------|--------------|
| **categories/** | Appointments reference category IDs defined there |
| **personal/** | Daily files reference appointments by ID |
| **coordination/** | Multi-identity appointments (this folder is single-identity) |
| **planner/** | Overview aggregates appointments across days/weeks |

## Recurring vs One-Time Appointments

**Use recurring/** when:

- Appointment repeats on a regular schedule (daily, weekly, etc.)
- Pattern-based scheduling (every Sunday, every weekday morning, etc.)
- Long-term commitments (work schedule, church service, Bible study)
- Examples: `work.jsonc`, `church.jsonc`, `semi-downtime.jsonc`

**Use one-time/** when:

- Specific event on a particular date
- Non-repeating appointments
- Special occasions or exceptions to normal schedule
- Examples: `doctor-appointment-2025-12-15.jsonc`, `thanksgiving-dinner-2025-11-27.jsonc`

## Appointment vs Coordination

**Use appointments/** when:

- Single identity involved (user OR instance)
- Personal scheduled time (sacred time, work blocks, rest)
- Individual commitments

**Use coordination/** when:

- Multiple identities involved (user AND instance, etc.)
- Collaborative work sessions
- Shared meetings or planning time

## File Naming

**Recurring appointments:** Simple descriptive names

- Format: `{appointment_id}.jsonc`
- Examples: `work.jsonc`, `church.jsonc`, `bible-study.jsonc`, `semi-downtime.jsonc`

**One-time appointments:** Include date for uniqueness

- Format: `{appointment_id}-{date}.jsonc`
- Examples: `doctor-appointment-2025-12-15.jsonc`, `thanksgiving-dinner-2025-11-27.jsonc`, `team-meeting-2025-11-20.jsonc`

## Complete Structure

Each appointment file should include:

- Appointment metadata (ID, title, category)
- Time information (start, end, duration)
- Recurrence pattern (if applicable)
- Status tracking
- Category reference

## Example Use Cases

**Recurring Appointment:**

1. Define recurring appointment: `appointments/recurring/users/seanje-lenox-wise/church.jsonc`
2. Reference in daily file: `personal/users/seanje-lenox-wise/2025/W45/nov_02_sunday.jsonc`
3. Appointment appears in hourly breakdown and appointments array
4. Weekly overview aggregates time spent in "sacred" category

**One-Time Appointment:**

1. Define one-time event: `appointments/one-time/users/seanje-lenox-wise/2025/thanksgiving-dinner-2025-11-27.jsonc`
2. Reference in daily file: `personal/users/seanje-lenox-wise/2025/W48/nov_27_thursday.jsonc`
3. Appointment only appears on that specific day
4. Does not affect other days or weekly patterns

## Notes

- Recurring appointments are defined once in `recurring/` and referenced across multiple days
- One-time appointments in `one-time/` are used for specific dates only
- Each daily file maintains its own appointments array for that specific day
- Hourly breakdown in daily files shows which appointment occupies each hour
- All appointment files use the same schema regardless of recurring vs one-time classification
