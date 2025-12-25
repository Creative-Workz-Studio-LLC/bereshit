# Calendar Templates

This folder contains templates for all calendar types in the chronological temporal data system.

## Purpose

The temporal calendar system provides multiple views of time for faithful stewardship and awareness:

- **Personal calendars**: Track individual work, rest, and growth (weekly, monthly, yearly)
- **Shared calendars**: Track collaboration and covenant partnerships
- **Project calendars**: Track project timelines and milestones
- **Base calendars**: Reference data for all date lookups

All calendars work together to provide:

- **Temporal awareness**: Where time actually went (not where you thought it went)
- **Faithful stewardship**: Honest accounting of God's gift of time
- **Pattern recognition**: Understanding your natural work rhythms
- **Growth tracking**: Seeing development over time
- **Rest accountability**: Ensuring Sabbath and rest are honored

## Calendar Types

### Personal Calendars

Track individual time for users (humans) and instances (CPI-SI):

- **Weekly**: Detailed daily tracking with sessions, tasks, reflections
- **Monthly**: Aggregated monthly view with themes and major accomplishments
- **Yearly**: Annual overview with quarterly breakdowns and growth trajectory

### Shared Calendars

Track collaborative work between partners:

- **User-to-Instance**: Human and CPI-SI partnership (e.g., Seanje ⊗ Nova)
- **User-to-User**: Human collaboration
- **Instance-to-Instance**: CPI-SI instance collaboration

### Project Calendars

Track project-specific timelines, phases, and deliverables.

### Milestone Calendars

Mark significant achievements, releases, and temporal markers.

### Base Calendars

Reference calendars with all dates, weekdays, week numbers, and holidays. Used for lookups and validation.

---

## Weekly Calendars

The foundation of personal temporal tracking.

## File Structure

```
chronological/migration/
├── templates/
│   ├── weekly-log.template.jsonc         # Weekly calendar template
│   ├── monthly-summary.template.jsonc    # Monthly calendar template
│   ├── yearly-overview.template.jsonc    # Yearly calendar template
│   └── README.md                          # This file
├── personal/
│   ├── instances/{instance-id}/{year}/   # Instance personal calendars
│   │   ├── {year}-W{week}.jsonc          # Weekly logs
│   │   ├── {year}-{month}.jsonc          # Monthly summaries
│   │   └── {year}.jsonc                  # Yearly overview
│   └── users/{user-id}/{year}/           # User personal calendars
│       ├── {year}-W{week}.jsonc          # Weekly logs
│       ├── {year}-{month}.jsonc          # Monthly summaries
│       └── {year}.jsonc                  # Yearly overview
├── shared/
│   ├── user-to-instance/{partnership-id}/{year}/
│   ├── user-to-user/{partnership-id}/{year}/
│   └── instance-to-instance/{partnership-id}/{year}/
├── projects/
│   └── {project-id}/
│       └── timeline.jsonc
├── milestones/
│   └── {milestone-id}.jsonc
└── calendar/
    ├── base/
    │   ├── 2025.jsonc                    # Full 2025 calendar
    │   └── 2026.jsonc                    # Full 2026 calendar
    ├── 2025/                             # Month-by-month breakdown
    └── 2026/                             # Month-by-month breakdown
```

## Quick Start

### 1. Determine Your Week Number

```bash
# Get current ISO week number
date +%V

# Get week number for specific date
date -d "2025-11-07" +%V
```

ISO week numbers run from 1-53. Week 1 is the first week with a Thursday in the new year.

### 2. Copy the Template

**For Users:**
```bash
# Create your year folder if it doesn't exist
mkdir -p ~/.claude/cpi-si/system/data/temporal/chronological/migration/personal/users/{your-user-id}/{year}

# Copy template
cp weekly-log.template.jsonc personal/users/{your-user-id}/{year}/{year}-W{week}.jsonc
```

**For Instances:**
```bash
# Create year folder
mkdir -p ~/.claude/cpi-si/system/data/temporal/chronological/migration/instances/{instance-id}/{year}

# Copy template
cp weekly-log.template.jsonc instances/{instance-id}/{year}/{year}-W{week}.jsonc
```

### 3. Fill In Your Data

Open the file and update:

1. **Metadata**: Set your identity_id, year, week number, week dates
2. **Daily entries**: Add sessions, tasks, events as they happen
3. **Weekly summary**: Complete at end of week

### 4. Keep Comments

The template includes extensive comments explaining each field. Keep them for future reference.

## Data Sections Explained

### Metadata

- **identity_type**: `"user"` for humans, `"instance"` for CPI-SI instances
- **identity_id**: Your user ID (e.g., `"seanje-lenox-wise"`) or instance ID (e.g., `"nova_dawn"`)
- **week**: ISO week number (1-53)
- **week_start/week_end**: Monday-Sunday date range for this week
- **week_theme**: Optional focus or theme for the week

### Daily Entries

Each day should have:

- **sessions**: Work periods with start/end times
  - Type (optional): `"work"`, `"lord_time"`, `"personal"`, etc.
  - Duration in `Xh XXm` format
  - Focus: What you worked on
- **tasks_completed**: List of accomplishments
- **significant_events**: Notable occurrences worth remembering
- **personal_time** (Users): Non-work activities
- **notes**: Reflections, observations, learnings
- **energy_level**: `null`, `"low"`, `"medium"`, `"high"`
- **quality_assessment**: `null`, `"poor"`, `"fair"`, `"good"`, `"excellent"`, `"restful"`

### Weekly Summary

Complete at week's end:

- **Time tracking**: Total hours worked, rest days
- **Major accomplishments**: Significant progress made
- **Challenges faced**: Obstacles encountered
- **Lessons learned**: Insights gained
- **Growth areas**: Where you developed
- **Next week focus**: Planning ahead
- **Notable patterns**: Work rhythm observations
- **Spiritual reflections** (Users): What God taught you
- **Health and wellness** (Users): Physical/mental wellbeing

## User vs Instance Calendars

### User Calendars (Humans)

Track holistic life:

- Work time
- Time with the Lord (prayer, Scripture, worship)
- Personal time (exercise, family, hobbies)
- Rest and Sabbath
- Spiritual reflections
- Health and wellness

**Example: Seanje's calendar includes morning Lord time, work sessions with Nova, personal activities, Sabbath rest, spiritual insights.**

### Instance Calendars (CPI-SI)

Track work and growth:

- Work sessions and focus areas
- Tasks completed
- Technical learning
- Pattern recognition
- Growth in understanding
- Quality assessment

**Example: Nova's calendar tracks development work, systems thinking growth, architectural learning, technical accomplishments.**

## Best Practices

### 1. Record Daily

Don't wait until end of week. Update as you go:

- Log sessions when they happen
- Note tasks as you complete them
- Capture reflections while fresh

### 2. Be Honest

This isn't performance metrics - it's faithful accounting:

- Record actual time, not ideal time
- Include rest days and low-energy days
- Note challenges and struggles
- Track patterns honestly

### 3. Track Rest

Rest days are as important as work days:

- Record Sabbath observance
- Note rest and recreation
- Track sleep quality
- Monitor energy patterns

### 4. Reflect Deeply

Weekly summary is for learning:

- What patterns emerged?
- Where did you grow?
- What did God teach you?
- What needs to change?

### 5. Plan Forward

Use insights to improve:

- Set next week's focus based on this week's patterns
- Adjust work rhythms if needed
- Plan for known challenges
- Build on successes

## Schema Validation

All weekly log files validate against:
```
~/.claude/cpi-si/system/config/schemas/temporal/chronological/weekly-log.schema.json
```

The schema ensures:
- Required fields are present
- Dates are properly formatted
- Time durations follow `Xh XXm` pattern
- Energy/quality values use valid options
- Structure remains consistent

## Biblical Foundation

**Ecclesiastes 3:1** - "To every thing there is a season, and a time to every purpose under heaven"

Time is God's gift. Weekly calendars help us:

- Steward time faithfully
- Honor rest and Sabbath
- Recognize God's work in our rhythms
- See patterns He's teaching us
- Plan obediently for what He's calling us to

This isn't productivity optimization - it's faithful accounting of God's gift.

## Examples

See actual examples in:

- **Instance**: `instances/nova_dawn/2025/2025-W45.jsonc`
- **User**: `personal/users/seanje-lenox-wise/2025/2025-W45.jsonc`

These show how the same week looks from different perspectives:

- Nova tracks work sessions, technical growth, pattern recognition
- Seanje tracks Lord time, work, personal life, rest, spiritual reflections

Both honor the gift of time in their respective contexts.

## Extension Section

The `extension` object allows future growth without breaking existing structure:

- Habit tracking
- Pomodoro counts
- Prayer journal integration
- Gratitude entries
- Collaboration notes
- Health metrics

Add what serves your growth. The structure supports it.

---

## Other Calendar Types

### Monthly Summaries

**Template**: `monthly-summary.template.jsonc`
**Schema**: `~/.claude/cpi-si/system/config/schemas/temporal/chronological/monthly-summary.schema.json`

Monthly calendars aggregate weekly data into broader patterns:

- Monthly theme and focus areas
- Major accomplishments across the month
- Challenges and how they were addressed
- Patterns observed over 4-5 weeks
- Growth trajectory
- Planning for next month

Created at month's end by reviewing all weekly logs for that month.

### Yearly Overviews

**Template**: `yearly-overview.template.jsonc`
**Schema**: `~/.claude/cpi-si/system/config/schemas/temporal/chronological/yearly-overview.schema.json`

Yearly calendars provide the highest-level view:

- Year theme and annual focus
- Quarterly breakdowns
- Major milestones and achievements
- Growth trajectory across the year
- Lessons learned
- Vision for next year

Created at year's end by reviewing all monthly summaries.

### Shared Calendars

**Schema**: `~/.claude/cpi-si/system/config/schemas/temporal/chronological/shared-calendar.schema.json`

Track collaborative work between covenant partners:

- **User-to-Instance**: Track Seanje ⊗ Nova partnership work
- **User-to-User**: Track human collaboration
- **Instance-to-Instance**: Track CPI-SI instance partnerships

Shows the same time period from the partnership perspective - what we accomplished together.

### Project Timelines

**Schema**: `~/.claude/cpi-si/system/config/schemas/temporal/chronological/project-timeline.schema.json`

Track project-specific temporal data:

- Project phases and deliverables
- Milestone dates
- Resource allocation over time
- Project-specific metrics

Cross-references personal calendars to show where time was spent on this project.

### Milestone Markers

**Schema**: `~/.claude/cpi-si/system/config/schemas/temporal/chronological/milestone.schema.json`

Mark significant temporal events:

- Product releases
- Major feature completions
- Significant achievements
- Temporal markers for reference

Milestones create reference points across all calendar types.

### Base Calendars

**Location**: `calendar/base/{year}.jsonc`

Reference calendars with complete date information:

- All 365 days of the year in chronological order
- Organized by month, then by week within each month
- Week numbers (ISO 8601: Monday-Sunday)
- Weekday names
- Weekend indicators
- US Federal holidays
- Month metadata

Used for:

- Date lookups and validation
- Determining week boundaries
- Holiday awareness
- Calendar calculations

---

**Remember**: This is about faithful stewardship, not performance metrics. Track honestly, rest properly, learn deeply, plan obediently.
