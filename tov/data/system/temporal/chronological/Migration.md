# Chronological Data Migration

This folder contains the new structure for calendar/chronological data in the temporal awareness system.

## Purpose

The migration folder allows us to:

1. **Build new structure safely** - Without touching existing calendar data
2. **Validate new design** - Prove the architecture works before switching
3. **Migrate systematically** - Move data from old to new structure methodically
4. **Maintain rollback option** - Keep old structure until new is proven
5. **Compare approaches** - See old vs new side-by-side

## Why Migration?

The existing `calendar/` folder has data but doesn't follow the proven patterns we established with:

- Config inheritance architecture
- Data → Template → Schema flow
- Proper year/week organization
- Schema validation
- Biblical grounding
- Extension objects for growth

This migration brings chronological data into alignment with those patterns.

## Folder Structure

```
chronological/
├── calendar/          # OLD STRUCTURE - Do not modify
│   ├── personal/
│   └── instances/
│
└── migration/         # NEW STRUCTURE - Being built here
    ├── templates/
    │   ├── weekly-log.template.jsonc
    │   ├── monthly-summary.template.jsonc
    │   ├── yearly-overview.template.jsonc
    │   └── README.md
    ├── personal/
    │   ├── instances/{instance-id}/{year}/
    │   │   ├── {year}-W{week}.jsonc       # Weekly logs
    │   │   ├── {year}-{month}.jsonc       # Monthly summaries
    │   │   └── {year}.jsonc               # Yearly overview
    │   └── users/{user-id}/{year}/
    │       ├── {year}-W{week}.jsonc       # Weekly logs
    │       ├── {year}-{month}.jsonc       # Monthly summaries
    │       └── {year}.jsonc               # Yearly overview
    ├── shared/
    │   ├── user-to-instance/{partnership-id}/{year}/
    │   ├── user-to-user/{partnership-id}/{year}/
    │   └── instance-to-instance/{partnership-id}/{year}/
    ├── projects/
    │   └── {project-id}/
    │       └── timeline.jsonc
    ├── milestones/
    │   └── {milestone-id}.jsonc
    ├── calendar/
    │   ├── base/
    │   │   ├── 2025.jsonc                 # Complete 2025 calendar
    │   │   └── 2026.jsonc                 # Complete 2026 calendar
    │   ├── 2025/                          # Month-by-month for 2025
    │   │   ├── 01-january.jsonc
    │   │   ├── 02-february.jsonc
    │   │   └── ... (all 12 months)
    │   └── 2026/                          # Month-by-month for 2026
    │       └── ... (all 12 months)
    └── CALENDAR-SYSTEM.jsonc              # System coordination doc
```

## New Structure Features

### 1. Multiple Calendar Types

The system provides several calendar views:

**Personal Calendars** - Individual time tracking:
- Weekly logs (detailed daily tracking)
- Monthly summaries (aggregated patterns)
- Yearly overviews (annual trajectory)

**Shared Calendars** - Collaborative work:
- User-to-Instance (e.g., Seanje ⊗ Nova)
- User-to-User (human partnerships)
- Instance-to-Instance (CPI-SI collaborations)

**Project Calendars** - Project-centric timelines with phases and milestones

**Milestone Markers** - Significant temporal events and achievements

**Base Calendars** - Reference calendars with all dates, weekdays, weeks, holidays

### 2. Identity-Based Organization

**Users** (humans):
```
personal/users/{user-id}/{year}/
├── {year}-W{week}.jsonc     # Weekly logs
├── {year}-{month}.jsonc     # Monthly summaries
└── {year}.jsonc             # Yearly overview
```

**Instances** (CPI-SI):
```
personal/instances/{instance-id}/{year}/
├── {year}-W{week}.jsonc     # Weekly logs
├── {year}-{month}.jsonc     # Monthly summaries
└── {year}.jsonc             # Yearly overview
```

Note: Instances are under `personal/` to prevent clinical thinking - instance calendars represent MY personal work and growth.

### 3. Year Folders

Data organized by year for clean archival:
- `2024/` contains all weeks from 2024
- `2025/` contains all weeks from 2025
- Easy to archive old years
- Scales naturally over time

### 4. ISO Week Numbering

Files named `{year}-W{week}.jsonc`:
- `2025-W45.jsonc` = Week 45 of 2025
- Weeks run Monday-Sunday (ISO 8601 standard)
- Unambiguous dating
- Aligns with international standard
- Easy to calculate and reference

### 5. Comprehensive Tracking

**Weekly calendars** (most detailed):
- Daily entries with sessions, tasks, events
- Personal time and activities
- Notes and reflections
- Energy and quality assessment
- Weekly summaries with accomplishments, challenges, patterns

**Monthly summaries** (aggregated patterns):
- Monthly theme and focus areas
- Major accomplishments across 4-5 weeks
- Challenges and how addressed
- Patterns observed
- Growth trajectory
- Planning for next month

**Yearly overviews** (highest level):
- Year theme and annual focus
- Quarterly breakdowns
- Major milestones
- Growth across the year
- Lessons learned
- Vision for next year

**Shared calendars** (partnership view):
- Same time period from collaboration perspective
- What we accomplished together
- Partnership dynamics and growth

**Project timelines** (project-centric):
- Phases, deliverables, milestones
- Resource allocation
- Cross-references to personal calendars

**Milestones** (temporal markers):
- Releases, achievements, significant events
- Reference points across all calendar types

**Base calendars** (reference data):
- All 365 days with weekdays, week numbers, holidays
- Organized by month, then by week
- Used for lookups, validation, calculations

### 6. Schema Validation

All calendar files validate against their respective schemas:

```
~/.claude/cpi-si/system/config/schemas/temporal/chronological/
├── weekly-log.schema.json
├── monthly-summary.schema.json
├── yearly-overview.schema.json
├── shared-calendar.schema.json
├── project-timeline.schema.json
└── milestone.schema.json
```

Ensures:
- Required fields present
- Proper date/time formats
- Valid enum values
- Consistent structure across all calendar types

### 7. Biblical Foundation

Every calendar file grounds temporal awareness in Scripture:

**Ecclesiastes 3:1** - "To every thing there is a season, and a time to every purpose under heaven"

Time is God's gift. Calendars track faithful stewardship, not just productivity metrics.

## Current Status

### ✅ Completed

- [x] Complete folder structure created
- [x] All calendar types implemented:
  - Personal calendars (weekly, monthly, yearly)
  - Shared calendars (user-to-instance, user-to-user, instance-to-instance)
  - Project timelines
  - Milestones
  - Base calendars (2025, 2026)
- [x] All templates created with full documentation
- [x] Schemas created for all calendar types
- [x] Example data:
  - Weekly: Nova Dawn (instance) and Seanje (user) for W45, 2025
  - Shared: Seanje ⊗ Nova for W45, 2025
- [x] Base calendars properly organized:
  - All 365 days in chronological order
  - Organized by month, then by week (Monday-Sunday)
  - Complete months metadata
  - 24 monthly files (12 for 2025, 12 for 2026)
  - 2 base files (2025.jsonc, 2026.jsonc)
- [x] Templates README with comprehensive usage guide
- [x] Migration README (this file) - updated
- [x] CALENDAR-SYSTEM.jsonc coordination documentation

### 🚧 In Progress

- [ ] Create monthly and yearly calendar templates
- [ ] Create schemas for monthly, yearly, shared, project, and milestone calendars
- [ ] Review complete structure with Seanje
- [ ] Validate against existing calendar data

### 📋 Pending

- [ ] Create migration script to move old data to new structure
- [ ] Test migration with sample weeks
- [ ] Complete migration of all historical data
- [ ] Remove old calendar folder after migration complete
- [ ] Update system references to point to new structure
- [ ] Document migration completion

## Migration Process

### Phase 1: Build (Current)

Build new structure following proven patterns:
1. Create actual data files (examples)
2. Build template from proven examples
3. Create schema validating actual structure
4. Document usage thoroughly

**Status**: ✅ Complete

### Phase 2: Validate

Ensure new structure works:
1. Review examples with Seanje
2. Test schema validation
3. Verify all fields are useful
4. Confirm structure supports growth

**Status**: 🚧 Ready for review

### Phase 3: Migrate

Move data systematically:
1. Create migration script
2. Map old structure to new structure
3. Migrate week by week
4. Validate each migrated file
5. Compare old vs new for accuracy

**Status**: 📋 Not started

### Phase 4: Switch

Make new structure official:
1. Update system to use new structure
2. Archive old structure (don't delete yet)
3. Monitor for issues
4. Complete transition

**Status**: 📋 Not started

### Phase 5: Clean Up

Remove old structure safely:
1. Verify new structure working perfectly
2. Confirm all data migrated
3. Remove old calendar folder
4. Update all documentation

**Status**: 📋 Not started

## Examples

See working examples in:

### Personal Calendars

**Instance (Nova Dawn)**:
```
personal/instances/nova_dawn/2025/2025-W45.jsonc
```
Tracks: Work sessions, technical focus, tasks, growth, pattern recognition

**User (Seanje)**:
```
personal/users/seanje-lenox-wise/2025/2025-W45.jsonc
```
Tracks: Lord time, work, personal activities, rest, Sabbath, spiritual reflections

Both track the same week (W45, 2025) from different perspectives.

### Shared Calendar

**Seanje ⊗ Nova Partnership**:
```
shared/user-to-instance/seanje-nova/2025/2025-W45.jsonc
```
Tracks: Collaborative work from partnership perspective - what we accomplished together

### Base Calendars

**Complete year calendars**:
```
calendar/base/2025.jsonc  # All 365 days of 2025
calendar/base/2026.jsonc  # All 365 days of 2026
```
Organized by month, then by week, with complete date information

**Month-by-month breakdown**:
```
calendar/2025/01-january.jsonc ... 12-december.jsonc
calendar/2026/01-january.jsonc ... 12-december.jsonc
```

### System Coordination

**CALENDAR-SYSTEM.jsonc**:
Documents how all calendar types integrate, temporal layers, data flow patterns

## Key Differences from Old Structure

| Aspect | Old Calendar | New Migration |
|--------|-------------|---------------|
| **Calendar types** | Weekly only | Weekly, monthly, yearly, shared, projects, milestones, base |
| **Organization** | Flat or unclear | Year folders, ISO weeks (Monday-Sunday) |
| **Personal structure** | Unclear separation | Instances under personal/ (prevents clinical thinking) |
| **Schema** | None | Full validation for all types |
| **Templates** | None | Comprehensive templates for all types |
| **Documentation** | Minimal | Extensive guides (templates + migration READMEs) |
| **Biblical foundation** | Implicit | Explicit in every file |
| **Extension** | No growth plan | Extension objects for all calendar types |
| **Validation** | Manual | Automatic schema checks |
| **Examples** | Limited | Full working examples (personal, shared, base calendars) |
| **Base calendars** | None | Complete 2025/2026 reference calendars |
| **Coordination** | Unclear | CALENDAR-SYSTEM.jsonc documents integration |

## Next Steps

1. **Review with Seanje** - Ensure structure meets needs
2. **Validate approach** - Confirm this properly serves temporal awareness
3. **Create migration script** - Automate data transfer
4. **Test migration** - Start with sample weeks
5. **Complete migration** - Move all historical data
6. **Switch over** - Make new structure official
7. **Clean up** - Remove old structure

## Notes

- **Don't touch calendar/ folder** - Old structure stays intact until migration complete
- **Build examples first** - Actual data drives template and schema design
- **Follow proven flow** - Data → Template → Schema (not schema first!)
- **Test thoroughly** - Validate before switching
- **Document everything** - Future clarity is worth present effort

---

**Remember**: Migration is about alignment, not just moving files. We're bringing chronological data into harmony with the temporal awareness architecture.
