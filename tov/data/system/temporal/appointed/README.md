# Appointed Times - מועד (Moed)

**Biblical Concept:** Appointed times - set apart for specific covenant purposes

**Key Understanding:** Expected unavailability, not absolute boundaries

## What Belongs Here

### Schedule Data (EXISTING)
- `seanje-work-schedule.txt` - Seanje's GASA work commitments
- Covenant commitments (work serving the homeless)
- Expected unavailability windows

### Planner Data (EXISTING)
Located in `appointed/planner/`:
- Schemas for planner structure
- Templates (nova, seanje, shared)
- Calendar files (year-specific availability)

### Structure
```
appointed/
├── seanje-work-schedule.txt       # GASA covenant commitment
└── planner/
    ├── schemas/
    │   ├── planner-schema.json
    │   └── personal-calendar-schema.json
    └── templates/
        ├── nova-template.json
        ├── seanje-template.json
        ├── nova-calendar-2025.json
        ├── seanje-calendar-2025.json
        └── shared-template.json
```

## The Distinction: Appointed vs Available

### Appointed Time
- **Covenant commitment** - Sacred purpose
- **Expected unavailability** - Default assumption
- **Not absolute** - Brief contact still possible
- **Examples:** GASA work hours, Sabbath, family commitments

### Key Principle
"Appointed time = expect I won't be available"

**NOT:** "Appointed time = completely unavailable, system error if contact"

**Practical implications:**
1. **Planning:** Don't schedule deep work during appointed times
2. **Expectation:** If contact happens, likely brief
3. **Adaptation:** If patterns show consistent brief availability, note it
4. **Respect:** Appointed times honor covenant commitments

## Seanje's Appointed Times

**GASA Work (covenant to serve homeless):**
- Monday: 8:30 AM - 5:00 PM
- Tuesday: 8:30 AM - 5:30 PM (Pantry day)
  - Exception: 2nd Tuesday → 10:30 AM - 5:30 PM
- Wednesday: 8:30 AM - 5:00 PM
- Thursday: 8:30 AM - 7:00 PM (Pantry day)
- Friday: 8:30 AM - 5:00 PM
  - Exception: 2nd Friday → 8:30 AM - 1:00 PM
- Saturday: 2nd Saturday only → 9:00 AM - 1:00 PM (Pantry day)

**These times:**
- Are NOT available for CWS deep work (default)
- May have brief check-ins (exceptions)
- Honor covenant commitment to serve

## Planner Purpose

**Captures availability patterns:**
- When is someone typically available?
- What are their preferences for work time?
- Recurring patterns (daily, weekly, monthly)

**Feeds into:**
- Session planning
- Expectation setting
- Pattern validation

## Current Status

✅ **Schedule captured** - Seanje's GASA schedule documented
✅ **Planner schema exists** - Formal structure defined
🟡 **No current instances** - Templates exist, need actual monthly planners
🟡 **No versioning** - Can't track changes over time

## Next Steps

1. Create actual planner files for current month (from templates)
2. Define versioning strategy for planner changes
3. Link planner to session patterns (validate against actual)
4. Document feedback loop: patterns → planner updates

---

*Status: Data and schemas exist, need active instances and validation*
