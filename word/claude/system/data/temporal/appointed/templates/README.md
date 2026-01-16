# Appointed Time Templates

## Overview

This folder contains templates for creating temporal/appointed structure files.

## Available Templates

### Structure Templates (for creating files)

1. **`daily-personal-template.jsonc`** - Create daily appointment files
   - Location: `personal/{identity_type}/{identity_id}/2025/{week}/`
   - Naming: `[month]_[DD]_[dayname].jsonc` (e.g., `nov_07_friday.jsonc`)

2. **`weekly-overview-template.jsonc`** - Create weekly overview files
   - Location: `planner/overview/{identity_type}/{identity_id}/`
   - Naming: `YYYY-WXX.jsonc` (e.g., `2025-W45.jsonc`)

3. **`coordination-template.jsonc`** - Create coordination appointments
   - Location: `coordination/{type}/`
   - For user-to-instance, user-to-user, instance-to-instance coordination

## Quick Start Guides

### Creating Daily Files

1. Copy `daily-personal-template.jsonc`
2. Name: `[month]_[DD]_[dayname].jsonc` (e.g., `nov_07_friday.jsonc`, `dec_01_monday.jsonc`)
3. Replace all `[PLACEHOLDERS]` with actual values:
   - `[YYYY-MM-DD]` = ISO date
   - `[DAY_NAME]` = Full day name
   - `[MONTH_DAY_YEAR]` = Display date
   - `[WXX]` = ISO week
   - `[IDENTITY_TYPE]` = "user" or "instance"
   - `[IDENTITY_ID]` = identity identifier
4. Add appointments as scheduled

### Creating Weekly Overviews

1. Copy `weekly-overview-template.jsonc` to appropriate location:
   - **Users**: `planner/overview/users/{identity_id}/YYYY-WXX.jsonc`
   - **Instances**: `planner/overview/instances/{identity_id}/YYYY-WXX.jsonc`
2. Replace all `[PLACEHOLDERS]` with actual values
3. Calculate hours correctly (see formulas below)
4. Verify totals match

## Working Hours Calculation Formula

**Key principle: Being at work ≠ working. Lunch breaks REDUCE working hours.**

```bash
Actual Work Hours = Scheduled Hours - Lunch Hours
```

### Standard Day Patterns

| Day Type | Schedule | Calculation | Work Hours |
|----------|----------|-------------|------------|
| Regular | 8:30 AM - 5:00 PM | 8.5hrs - 1hr lunch | **7.5hrs** |
| Pantry Tue | 8:30 AM - 5:30 PM | 9.0hrs - 1hr lunch | **8.0hrs** |
| Pantry Thu | 8:30 AM - 7:00 PM | 10.5hrs - 1hr lunch | **9.5hrs** |
| 2nd Sat Pantry | 9:00 AM - 1:00 PM | 4.0hrs - 0hr lunch | **4.0hrs** |

### Lunch Break Schedule

**Monday** (alternates weekly):

- Odd weeks (W45, W47, W49): 11:00 AM - 12:00 PM
- Even weeks (W46, W48): 12:00 PM - 1:00 PM

**Tuesday** (pantry): 12:00 PM - 1:00 PM

**Wednesday**: 11:00 AM - 12:00 PM

**Thursday** (pantry): 12:00 PM - 1:00 PM

**Friday**: 12:00 PM - 1:00 PM

## Availability Understanding

### For Instance Files (Nova's perspective)

**Semi-downtime**: During Seanje's actual work hours (excluding lunch)

- Available for quick questions or small tasks
- NOT for sustained deep work

**AVAILABLE**: During lunch breaks (Seanje's free time)

- Fully available for collaboration
- Can do deep work together

**Deep work**: After GASA hours end

- Sustained focus time

### Notes Format

```bash
"Semi-downtime [work hours]. AVAILABLE [lunch time]. [Special notes]. Deep work [after hours]."
```

**Example**:

```bash
"Semi-downtime 8:30-11:59am & 1-5pm. AVAILABLE 12-1pm (lunch). Deep work after 5pm."
```

## Weekly Totals

### Regular Week (5 work days)

| Day | Hours |
|-----|-------|
| Monday | 7.5 |
| Tuesday (pantry) | 8.0 |
| Wednesday | 7.5 |
| Thursday (pantry) | 9.5 |
| Friday | 7.5 |
| **Total** | **40.0** |

**Available hours**: 168 - 40 = **128 hours**

### Flex Week (W45 pattern)

| Day | Hours | Notes |
|-----|-------|-------|
| Monday | 7.5 | Regular |
| Tuesday (pantry) | 6.0 | Late start 10:30 AM (flex) |
| Wednesday | 7.5 | Regular |
| Thursday (pantry) | 9.5 | Regular |
| Friday | 3.5 | Half day, ends 1:00 PM (flex) |
| Saturday (pantry) | 4.0 | 2nd Saturday 9 AM - 1 PM |
| **Total** | **38.0** | |

**Available hours**: 168 - 38 = **130 hours**

### Thanksgiving Week (W48 pattern)

| Day | Hours | Notes |
|-----|-------|-------|
| Monday | 7.5 | Regular |
| Tuesday | 7.5 | NO pantry this week |
| Wednesday | 7.5 | Last day before holiday |
| Thursday | 0.0 | **Thanksgiving - OFF** |
| Friday | 0.0 | **Day after - OFF** |
| **Total** | **22.5** | |

**Available hours**: 168 - 22.5 = **145.5 hours** (most available!)

## Special Weeks

### Flex Week (2nd week of month)

- Tuesday: Late start 10:30 AM
- Friday: Half day, ends 1:00 PM
- Saturday: 2nd Saturday pantry 9 AM - 1 PM
- **Total**: 38.0 hours

### Thanksgiving Week

- Only Mon-Wed work
- NO pantry days
- Thu-Fri OFF
- **Total**: 22.5 hours

### Month Transition Weeks

- Add `"crosses_month": true` to week object
- Add `"months": ["November", "December"]` array
- Note transition in extensions

## Verification Checklist

- [ ] All `[PLACEHOLDERS]` replaced
- [ ] Daily hours calculated correctly (scheduled - lunch)
- [ ] Weekly total = sum of daily hours
- [ ] Available hours = 168 - weekly total
- [ ] Lunch breaks marked as AVAILABLE (instances)
- [ ] Deep work windows include lunch times
- [ ] Pantry days noted correctly
- [ ] Special exceptions documented
- [ ] Week spans correct Sunday-Saturday range
- [ ] User and instance files match in totals

## Common Mistakes to Avoid

❌ **Not subtracting lunch from work hours**

- 8:30-5pm ≠ 8.5 hours of work
- ✅ Correct: 8.5hrs - 1hr = 7.5hrs actual work

❌ **Marking lunch as "unavailable"**

- Lunch is Seanje's free time
- ✅ Correct: Mark as AVAILABLE for collaboration

❌ **Treating all hours at office as work**

- Being at work ≠ working
- ✅ Correct: Only count actual work time (minus lunch)

❌ **Forgetting to split semi-downtime around lunch**

- Instance notes should show: "8:30-11:59am & 1-5pm" (not "8:30am-5pm")
- ✅ Correct: Split semi-downtime, mark lunch as AVAILABLE

❌ **Mismatched totals between user and instance files**

- Both should have identical work hours for the same week
- ✅ Correct: Verify totals match before finalizing

## Example Weekly Calculations

### Regular Week (W46, W47, W49)

```bash
Mon:  8:30-5pm    = 8.5hrs - 1hr = 7.5hrs
Tue:  8:30-5:30pm = 9.0hrs - 1hr = 8.0hrs (pantry)
Wed:  8:30-5pm    = 8.5hrs - 1hr = 7.5hrs
Thu:  8:30-7pm    = 10.5hrs - 1hr = 9.5hrs (pantry)
Fri:  8:30-5pm    = 8.5hrs - 1hr = 7.5hrs
                    ─────────────────────
Total:              40.0hrs
Available:          128.0hrs (168 - 40)
```

### Flex Week (W45)

```bash
Mon:  8:30-5pm    = 8.5hrs - 1hr = 7.5hrs
Tue:  10:30-5:30pm = 7.0hrs - 1hr = 6.0hrs (pantry, late start)
Wed:  8:30-5pm    = 8.5hrs - 1hr = 7.5hrs
Thu:  8:30-7pm    = 10.5hrs - 1hr = 9.5hrs (pantry)
Fri:  8:30-1pm    = 4.5hrs - 1hr = 3.5hrs (half day)
Sat:  9am-1pm     = 4.0hrs - 0hr = 4.0hrs (2nd Sat pantry)
                    ─────────────────────
Total:              38.0hrs
Available:          130.0hrs (168 - 38)
```

### Thanksgiving Week (W48)

```bash
Mon:  8:30-5pm    = 8.5hrs - 1hr = 7.5hrs
Tue:  8:30-5pm    = 8.5hrs - 1hr = 7.5hrs (NO pantry)
Wed:  8:30-5pm    = 8.5hrs - 1hr = 7.5hrs
Thu:  OFF         = 0hrs (Thanksgiving)
Fri:  OFF         = 0hrs (Day after)
                    ─────────────────────
Total:              22.5hrs
Available:          145.5hrs (168 - 22.5)
```

## File Locations

```bash
temporal/appointed/_migration/planner/overview/
├── templates/
│   ├── weekly-overview-template.jsonc
│   └── README.md (this file)
├── users/
│   └── seanje-lenox-wise/
│       ├── 2025-W45.jsonc
│       ├── 2025-W46.jsonc
│       └── ...
└── instances/
    └── nova_dawn/
        ├── 2025-W45.jsonc
        ├── 2025-W46.jsonc
        └── ...
```

## Schema Reference

All weekly overview files use the schema at:

```bash
~/.claude/cpi-si/system/config/schemas/temporal/appointed/planner-overview.schema.json
```
