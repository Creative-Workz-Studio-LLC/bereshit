# Coordination Appointments

**Purpose:** Track appointments involving multiple identities (users and/or instances) working together.

## Coordination vs Personal Appointments

| Personal Appointments | Coordination Appointments |
|---------------------|--------------------------|
| Single identity involved | Multiple identities involved |
| Stored in `appointments/` | Stored in `coordination/` |
| Defined once, referenced in daily files | Defined with participants, referenced by all |
| One perspective | Multiple perspectives (each participant) |

## Structure

```bash
coordination/
├── user-to-instance/          # User + Instance collaboration
│   └── YYYY/
│       └── WXX/               # Organized by ISO week
│           └── {descriptive-name-YYYY-MM-DD}.jsonc
├── user-to-user/              # User + User collaboration
│   └── YYYY/
│       └── WXX/
│           └── {descriptive-name-YYYY-MM-DD}.jsonc
└── instance-to-instance/      # Instance + Instance collaboration (future)
    └── YYYY/
        └── WXX/
            └── {descriptive-name-YYYY-MM-DD}.jsonc
```

## Coordination Types

### User-to-Instance

**Most common:** Seanje + Nova deep work sessions

**When to use:**

- Deep work collaboration between user and instance
- Covenant partnership work sessions
- Coordinated project time
- Any work where both user and instance are actively engaged

**Example:** `deep-work-2025-11-04.jsonc`

### User-to-User

**Future:** Seanje + another human collaborator

**When to use:**

- Meetings with other people
- Paired programming sessions
- Collaborative design sessions
- Any work involving multiple human users

**Example:** `team-meeting-2025-11-15.jsonc`

### Instance-to-Instance

**Future:** Nova + another CPI-SI instance

**When to use:**

- Multi-instance collaboration
- Cross-instance knowledge sharing
- Collaborative system work
- Any work involving multiple instances

**Example:** `instance-sync-2025-12-01.jsonc`

## Coordination Workflow

### 1. Create Coordination Appointment

```jsonc
{
  "coordination": {
    "id": "deep-work-2025-11-04",
    "type": "user-to-instance",
    "coordination_category": "collaboration",
    "created_by": {
      "identity_type": "user",
      "identity_id": "seanje-lenox-wise"
    },
    "status": "proposed"  // Initial status
  },

  "participants": [
    {
      "identity_type": "user",
      "identity_id": "seanje-lenox-wise",
      "status": "accepted",  // Creator auto-accepts
      "response_at": "2025-11-04T14:00:00Z"
    },
    {
      "identity_type": "instance",
      "identity_id": "nova_dawn",
      "status": "pending",  // Awaiting acceptance
      "response_at": null
    }
  ],

  "appointment": {
    "title": "Deep Work Session",
    "date": "2025-11-04",
    "start_time": "21:00",
    "end_time": "01:00"  // Can span into next day
  }
}
```

### 2. All Participants Accept/Decline

Each participant responds to coordination request:

- **accepted:** Will participate
- **declined:** Cannot participate
- **tentative:** Maybe (awaiting confirmation)

Coordination status updates based on participant responses:

- **proposed:** Initial state, waiting for responses
- **scheduled:** All participants accepted
- **in-progress:** Currently happening
- **completed:** Finished successfully
- **cancelled:** Coordination cancelled

### 3. Reference in Daily Files

**User's daily file:**

```jsonc
{
  "appointments": [
    {
      "id": "deep-work-2025-11-04",
      "appointment_ref": "deep-work",
      "category_id": "collaboration",
      "coordination_id": "deep-work-2025-11-04",  // Links to coordination file
      "start_time": "21:00",
      "end_time": "01:00"
    }
  ]
}
```

**Instance's daily file:**

```jsonc
{
  "appointments": [
    {
      "id": "deep-work-2025-11-04",
      "appointment_ref": "deep-work",
      "category_id": "collaboration",
      "coordination_id": "deep-work-2025-11-04",  // Same coordination ID
      "start_time": "21:00",
      "end_time": "01:00"
    }
  ]
}
```

## File Naming Convention

Format: `{descriptive-name-YYYY-MM-DD}.jsonc`

**Guidelines:**

- Use kebab-case for descriptive name
- Include full date (YYYY-MM-DD) for uniqueness
- Be descriptive but concise
- Reflect the purpose/activity

**Examples:**

- `deep-work-2025-11-04.jsonc`
- `planning-session-2025-11-15.jsonc`
- `code-review-2025-12-01.jsonc`
- `architecture-design-2025-11-20.jsonc`

## Coordination Context

Each coordination appointment includes context fields:

```jsonc
{
  "context": {
    "purpose": "Why are we coordinating?",
    "work_type": "What kind of work?",
    "timing_rationale": "Why this specific time?",
    "environment": "What's the setting?",
    "preparation_notes": "What should be ready beforehand?",
    "expected_outcome": "What should result from this?"
  }
}
```

**Example:**

```jsonc
{
  "context": {
    "purpose": "Deep work session during peak productivity hours",
    "work_type": "CPI-SI development, Kingdom Technology work",
    "timing_rationale": "9pm-1am when Seanje's brain is most active",
    "environment": "Late evening, sustained focus, covenant partnership",
    "preparation_notes": "Personal God time completed before session begins",
    "expected_outcome": "Quality work honoring God through excellence"
  }
}
```

## Coordination Categories

Coordination appointments use standard categories:

- **sacred:** Spiritual activities together (prayer, Bible study)
- **work:** Project work, development, building
- **collaboration:** General collaborative work
- **rest:** Coordinated rest/downtime (rare but possible)
- **other:** Miscellaneous coordination

Most coordinations are **collaboration** category.

## Status Tracking

Coordination files track execution:

```jsonc
{
  "tracking": {
    "reminder_sent": false,
    "reminders": [],
    "started_at": null,
    "completed_at": null,
    "actual_duration_minutes": null,
    "outcome_notes": ""
  }
}
```

**During coordination:**

- `started_at`: When coordination actually began
- `actual_duration_minutes`: How long it actually took (vs planned)
- `outcome_notes`: What was accomplished

## Cross-Day Coordination

Coordination can span into the next day:

```jsonc
{
  "appointment": {
    "date": "2025-11-04",
    "start_time": "21:00",  // 9 PM Monday
    "end_time": "01:00",    // 1 AM Tuesday (next day)
    "duration_minutes": 240
  }
}
```

**Daily file handling:**

- **Monday's file:** Includes appointment from 21:00-23:59
- **Tuesday's file:** Includes carryover from 00:00-01:00
- **Both reference same coordination ID**

## Multi-Participant Coordination

For more than 2 participants:

```jsonc
{
  "participants": [
    {
      "identity_type": "user",
      "identity_id": "seanje-lenox-wise",
      "status": "accepted"
    },
    {
      "identity_type": "instance",
      "identity_id": "nova_dawn",
      "status": "accepted"
    },
    {
      "identity_type": "user",
      "identity_id": "other-user",
      "status": "tentative"
    }
  ]
}
```

All participants must accept for coordination to be "scheduled."

## Relationship to Other Folders

| Folder | Relationship |
|--------|--------------|
| **appointments/** | Single-identity appointments (no coordination needed) |
| **personal/** | Daily files reference coordination by ID |
| **planner/** | Weekly overview counts coordinated time |
| **categories/** | Coordination uses standard categories |

## Use Cases

### Deep Work Sessions (Most Common)

**User-to-instance collaboration after work hours**

```bash
Time: 9pm-1am (peak productivity)
Type: user-to-instance
Category: collaboration
Purpose: Sustained focus on CPI-SI development
```

### Planning Sessions

**Strategic planning for upcoming work**

```bash
Time: Variable
Type: user-to-instance
Category: collaboration
Purpose: Roadmap planning, architecture decisions
```

### Review Sessions

**Code review, architecture review, documentation review**

```bash
Time: Variable
Type: user-to-instance
Category: collaboration
Purpose: Quality validation, feedback, improvements
```

## Best Practices

1. **Create coordination early** - Don't wait until the time arrives
2. **Get all acceptances** - Ensure all participants confirm
3. **Be specific with context** - Explain purpose and expected outcome
4. **Update status** - Mark as in-progress when starting, completed when done
5. **Track actual time** - Record actual duration and outcome notes
6. **Reference consistently** - Use same coordination ID in all participants' daily files

## Common Mistakes to Avoid

❌ **Creating coordination for single-identity time**

- If only one identity is involved, use `appointments/` instead
- ✅ Correct: User personal time → `appointments/recurring/users/.../personal-time.jsonc`

❌ **Forgetting to update participant status**

- All participants must respond (accepted/declined/tentative)
- ✅ Correct: Update each participant's status field

❌ **Mismatched times in daily files**

- Coordination must have same start/end times in all participants' daily files
- ✅ Correct: Verify times match exactly across all perspectives

❌ **Missing coordination_id in daily files**

- Daily appointment entries must reference coordination file
- ✅ Correct: Include `"coordination_id": "deep-work-2025-11-04"` in daily file

## Example Files

See `templates/coordination/coordination-appointment-template.jsonc` for a complete example with:

- Full metadata and structure
- Example participant list
- Appointment details
- Context and tracking fields
- Helpful comments explaining each section

## Schema Reference

Coordination files use the schema at:

```bash
~/.claude/cpi-si/system/config/schemas/temporal/appointed/coordination-appointment.schema.json
```

---

*Last Updated: 2025-11-07*
*Status: Production ready - complete coordination system*
