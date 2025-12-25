# Chronological Layer - Calendar

**Purpose:** Structural anchor for dates, anchored to celestial reality

## What Belongs Here

### Calendar Data (EXISTING)
Located in `chronological/calendar/`:
- Yearly metadata files (`2025.json`, `2026.json`)
- Monthly files (`YYYY/MM-month-name.json`)
- Date definitions with weekday, week number, month, day
- Holiday markers

### Structure
```
chronological/
└── calendar/
    ├── base/
    │   ├── 2025.json
    │   ├── 2025/
    │   │   ├── 01-january.json
    │   │   ├── 02-february.json
    │   │   └── ... (12 months)
    │   └── 2026/
    │       └── ... (same structure)
```

### What Calendar Provides
- **Reference anchor** for all temporal queries
- **Date structure** (year, month, week, day)
- **Weekday context** (Monday-Sunday)
- **Holiday information** (dates set apart)
- **Weekend markers** (cultural rest patterns)

## Relationship to Celestial

Calendar structure SHOULD anchor to celestial reality:
- Months → lunar cycles (approximately)
- Years → solar cycles (earth's orbit)
- Days → sun's day/night cycle

## Current Status

✅ **Calendar data exists** - 2025-2026 populated
🟡 **Schema needed** - No formal schema defined
🟡 **Celestial link needed** - Not yet anchored to solar/lunar data

## Next Steps

1. Define `calendar.schema.json`
2. Link to celestial layer (reference sunrise/sunset for each date)
3. Document holiday policy and how they're determined
4. Establish update process for future years

---

*Status: Data exists, needs schema formalization*
