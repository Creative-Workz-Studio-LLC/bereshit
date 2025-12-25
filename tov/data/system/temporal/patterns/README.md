# Patterns Layer - Learned Behavior

**Purpose:** What actually happens over time (learned from session data)

## What Belongs Here

### Aggregated Pattern Data

- Work rhythm patterns (when work typically happens)
- Session duration patterns (quick check vs deep work)
- Time-of-day quality indicators (when is focus best?)
- Natural stopping points (how sessions typically end)
- Circadian patterns (learned work/rest cycles)

### Pattern Types

**1. Temporal Patterns**

- What times of day are typical work periods?
- How long do sessions typically last?
- When do sessions naturally end?

**2. Quality Patterns**

- Which times produce best work?
- What session lengths are most productive?
- Which stopping reasons correlate with satisfaction?

**3. Behavioral Patterns**

- Does actual behavior match configured preferences?
- Are there consistent exceptions to appointed times?
- Do work patterns change over time?

## Relationship to Other Layers

### Validates Configuration

```bash
CPI-SI Config says: "Seanje works better at night"
Pattern data shows: 85% of deep-work sessions happen after 8 PM
Result: Config validated
```

### Informs Appointed Times

```bash
Planner says: Available 7-10 PM weeknights
Pattern data shows: Actual sessions cluster 8-9 PM
Result: Refinement opportunity (narrow window)
```

### Feeds Back to Config

```bash
Pattern discovered: Consistently productive 6-8 AM on weekends
Config currently: No morning preference noted
Result: Update config with discovered preference
```

## Data Sources

Patterns are **aggregated from session history:**

- Session timing data
- Duration tracking
- Quality indicators
- Stopping reasons
- Work context

**NOT manually created** - Patterns emerge from actual data.

## Current Status

🟢 **Schema system complete** - 14 pattern schemas defined and validated
🟢 **Migration workspace ready** - All pattern types designed in `migration/`
🟡 **Awaiting migration approval** - Ready to move from migration to live
🟡 **Aggregation system needed** - Transform session logs → pattern files
🟡 **Manual data entry** - Automated aggregation not yet built

## Completed Work

1. ✅ **Schema definition** - All pattern types have JSON schemas
2. ✅ **Structure design** - User/Instance/Discovered pattern organization
3. ✅ **Template creation** - Valid templates for all pattern types
4. ✅ **Data validation** - All migration data validates against schemas

## Next Steps

1. **User review and migration approval** - Verify structure serves purpose
2. **Execute migration** - Move validated patterns from migration/ to live
3. **Build aggregation system** - Automated session logs → patterns
4. **Implement validation queries** - Compare actual vs configured preferences
5. **Establish feedback loop** - Pattern insights → config updates

---

*Status: Schema validation complete - Ready for migration approval and deployment*
