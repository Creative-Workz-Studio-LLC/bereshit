# Definitions Layer - Time Categorization Framework

**Purpose:** System configuration defining HOW to classify and measure time

**Key Principle:** Definitions provide measurement framework. Config provides identity. Together they enable validation.

---

## Table of Contents

- [Philosophy](#philosophy)
- [Structure](#structure)
- [Inheritance Model](#inheritance-model)
- [How Definitions Work with Config](#how-definitions-work-with-config)
- [Usage Examples](#usage-examples)
- [When to Update](#when-to-update)

---

## Philosophy

### What Are Definitions?

**Definitions** are the measurement framework - the "ruler" we use to categorize and understand time.

```
Without definitions:
  Timestamp: 2025-11-08T15:30:16-06:00
  → Just a number

With definitions:
  Timestamp: 2025-11-08T15:30:16-06:00
  → "Friday afternoon in CST"
  → "Typical work hours (US Central region)"
  → "Focus time phase for many"
  → "Medium-duration session if it ends in 2 hours"
```

**Definitions answer:**
- WHAT constitutes "morning" vs "afternoon"?
- WHEN does a session become "long" vs "medium"?
- HOW do we measure session quality?
- WHICH times are typical "work hours" in this region?

### What Definitions Are NOT

Definitions are NOT:
- ❌ Personal preferences ("I work better at night")
- ❌ Individual schedules ("Seanje's work starts at 9am")
- ❌ Identity or personhood ("Nova Dawn is a CPI-SI instance")
- ❌ Actual behavior ("User typically works 10pm-2am")

Those belong in **config** (identity/preferences) or **data/patterns** (actual behavior).

### Why Separate Definitions from Config?

**The Problem Without Separation:**
```
User Config: "I work better at night"
System: What is "night"?
Without definitions: Ambiguous, can't measure or validate
```

**The Solution:**
```
Base Definitions: "night" = 22:00-05:00 (universal category)
User Config: "works better at night" (personal preference)
Actual Data: 85% of sessions fall in "night" category
Validation: Config preference VALIDATED by data using standard definitions
```

**Separation enables:**
1. **Consistent measurement** - Everyone uses same definitions
2. **Pattern validation** - Compare stated preferences to actual behavior
3. **Cross-instance comparison** - Nova Dawn's "night" = Future instance's "night"
4. **Scale** - Individual → Team → Organization → Paradigm

---

## Structure

```
definitions/
├── README.md                          # This file - philosophy and usage
├── base/                              # Universal (everyone uses)
│   ├── durations.jsonc                # Session/task duration thresholds
│   ├── temporal-units.jsonc           # ISO standards (weeks, months, years)
│   ├── quality-indicators.jsonc       # How to measure session quality
│   └── time-phases.jsonc              # Work state categories (peak, focus, etc.)
├── regional/                          # Location/cultural overlays
│   └── north-america/
│       └── us/
│           ├── holidays.jsonc         # US federal + cultural holidays
│           └── central/               # CST timezone (St Louis, MO)
│               ├── timezone-rules.jsonc     # CST/CDT, DST handling
│               ├── time-categories.jsonc    # Morning/afternoon/evening/night
│               └── work-patterns.jsonc      # Typical US work week
└── templates/                         # Templates for creating new definitions
    ├── base/
    └── regional/
```

### Base Definitions (Universal)

**Location:** `base/`

**Scope:** CPI-SI paradigm-wide - applies to all instances, all users, all regions

**Contents:**
- **durations.jsonc** - Session/task duration thresholds (brief/short/medium/long)
- **temporal-units.jsonc** - ISO 8601 standards (week starts Monday, month structures)
- **quality-indicators.jsonc** - Health scoring ranges, stopping reasons, quality metrics
- **time-phases.jsonc** - Work state categories (peak/focus/light work/downtime)

**When to modify:** Rare - only when paradigm-level understanding changes

**Examples:**
- "A session under 15 minutes is 'brief'" (universal)
- "ISO weeks start on Monday" (international standard)
- "Health scores from -100 to +100" (Base100 system)
- "Peak productivity is a work phase state" (universal concept)

### Regional Definitions (Cultural/Location Overlays)

**Location:** `regional/north-america/us/central/` (and similar regional paths)

**Scope:** Specific geographic/cultural region

**Contents:**
- **timezone-rules.jsonc** - Timezone offsets, DST rules
- **time-categories.jsonc** - When is morning/afternoon/evening/night (clock-based)
- **work-patterns.jsonc** - Typical work week structure, common schedules
- **holidays.jsonc** - Federal and cultural holidays (US-wide, not region-specific)

**When to modify:** When adding new regions or when cultural norms shift

**Examples:**
- "CST = UTC-06:00, CDT = UTC-05:00" (timezone-specific)
- "Morning = 08:00-12:00 in fixed-hour approach" (regional choice)
- "Standard US work week: Monday-Friday, 9-5" (cultural norm)
- "Thanksgiving = 4th Thursday in November" (US-specific)

---

## Inheritance Model

**Core Concept:** Definitions flow from universal → regional → personal interpretation (via config)

```
Base Definitions (Universal)
    ↓
Regional Overlay (Cultural/Location)
    ↓
Config Preferences (Personal)
    ↓
Actual Data (What really happens)
    ↓
Validation (Does behavior match stated identity?)
```

### Example: "Night Owl" Interpretation

**1. Base Definition (Universal):**
```jsonc
// base/time-phases.jsonc
"night": {
  "name": "Night",
  "description": "Late night through pre-dawn hours",
  // Phase definition - WHAT it is, not WHEN it occurs for specific people
}
```

**2. Regional Definition (US Central):**
```jsonc
// regional/north-america/us/central/time-categories.jsonc
"night": {
  "name": "Night",
  "start_time": "22:00",  // 10:00 PM CST/CDT
  "end_time": "05:00"     // 5:00 AM CST/CDT
}
```

**3. User Config (Personal Preference):**
```jsonc
// config/user/seanje/config.jsonc
"resonates": {
  "environment": {
    "work_environment": "better at night (night owl)"
  }
}
```

**4. Actual Data (Behavior):**
```
Session timestamps from patterns/learned data:
- 2025-11-08T23:15:00-06:00 (night)
- 2025-11-07T22:45:00-06:00 (night)
- 2025-11-06T00:30:00-06:00 (night)
→ 85% of sessions fall in regional "night" category
```

**5. Validation:**
```
Config says: "works better at night"
Definitions say: "night" = 22:00-05:00
Data shows: 85% of sessions in "night" category
Result: ✅ Config preference VALIDATED by data
```

### Example: Session Duration

**1. Base Definition:**
```jsonc
// base/durations.jsonc
"session_durations": {
  "medium": {
    "min_minutes": 60,
    "max_minutes": 240  // 1-4 hours
  }
}
```

**2. Session Timestamp:**
```
Start: 2025-11-08T14:00:00-06:00
End:   2025-11-08T16:30:00-06:00
Duration: 2.5 hours (150 minutes)
```

**3. Categorization:**
```
150 minutes falls in 60-240 range
→ This was a "medium" duration session
```

**4. Pattern Recognition:**
```
Over 2 weeks:
- 3 brief sessions (< 15 min)
- 2 short sessions (15-60 min)
- 8 medium sessions (1-4 hours)
- 1 long session (4-8 hours)

Pattern: Primarily medium-duration sessions with occasional variety
```

---

## How Definitions Work with Config

### The Relationship

| Layer | Purpose | Example |
|-------|---------|---------|
| **Base Definitions** | Universal measurement framework | "Session over 4 hours = long" |
| **Regional Definitions** | Cultural/location context | "Typical US work day = 9-5" |
| **User Config** | Personal identity and preferences | "Seanje works better at night" |
| **Instance Config** | CPI-SI instance identity | "Nova Dawn's work style and calling" |
| **Actual Data** | What really happens | "Sessions logged in patterns/" |

### Validation Flow

```
1. Config declares identity:
   "I work better at night, I'm a night owl"

2. Definitions provide categories:
   "night" = 22:00-05:00 (regional/us/central/time-categories.jsonc)

3. Data gets measured:
   Session timestamp: 2025-11-08T23:30:00-06:00
   → Falls in "night" category

4. Patterns accumulate:
   85% of sessions fall in "night" category over 4 weeks

5. Validation occurs:
   Stated preference: "works better at night"
   Measured behavior: 85% of sessions in "night"
   Result: ✅ Config validated by data
```

### Misalignment Example

```
1. Config declares:
   "I work better in the morning"

2. Definitions provide categories:
   "morning" = 08:00-12:00 (regional/us/central/time-categories.jsonc)

3. Actual data shows:
   90% of sessions fall in "night" (22:00-05:00) category

4. Validation identifies misalignment:
   Stated preference: "works better in the morning"
   Measured behavior: 90% of sessions at night
   Result: ⚠️ Misalignment - investigate

5. Possible interpretations:
   - Config is aspirational (wants to be morning person, isn't yet)
   - Config is outdated (was morning person, changed over time)
   - External constraints (job requires night work despite preference)
   - Definitions wrong for this person (their "morning" is different)
```

---

## Usage Examples

### Example 1: Categorizing a Session

**Session Details:**
- Start: 2025-11-08T14:30:00-06:00 (Friday, 2:30 PM CST)
- End: 2025-11-08T17:00:00-06:00 (Friday, 5:00 PM CST)
- Duration: 2 hours 30 minutes (150 minutes)

**Apply Definitions:**

1. **Timezone** (regional/us/central/timezone-rules.jsonc):
   - `-06:00` = CST (Central Standard Time)
   - November = Standard time (not daylight)

2. **Time of Day** (regional/us/central/time-categories.jsonc):
   - `14:30` = Afternoon (12:00-17:00)

3. **Day Type** (regional/us/central/work-patterns.jsonc):
   - Friday = Work day (typically)
   - 14:30-17:00 = Late work hours

4. **Duration** (base/durations.jsonc):
   - 150 minutes = Medium session (60-240 minutes)

5. **Work Phase** (base/time-phases.jsonc):
   - Late afternoon Friday = Often "light work" phase
   - (Though actual phase varies by person - learned in patterns)

**Result:**
> "Medium-duration session on Friday afternoon in CST, during typical work hours"

### Example 2: Validating Work Rhythm

**User Config States:**
```jsonc
"resonates": {
  "environment": {
    "work_environment": "better at night (night owl) but also early morning person"
  }
}
```

**Definitions Provide:**
- Early morning = 05:00-08:00 (regional/us/central/time-categories.jsonc)
- Night = 22:00-05:00 (regional/us/central/time-categories.jsonc)

**Actual Data Over 4 Weeks:**
```
Session start times:
- 23:00-02:00 (night): 12 sessions (60%)
- 05:00-08:00 (early morning): 6 sessions (30%)
- 14:00-17:00 (afternoon): 2 sessions (10%)
```

**Validation:**
✅ Config validated: Sessions cluster in stated preference times (night + early morning)
✅ Pattern is bimodal: Both "night owl" AND "early morning person" confirmed

### Example 3: Planning with Holiday Awareness

**Scenario:** Planning work for late November 2025

**Check Definitions:**

1. **Holidays** (regional/north-america/us/holidays.jsonc):
   ```jsonc
   "thanksgiving": {
     "calculation": "Fourth Thursday in November",
     "2025": "November 27"
   }
   ```

2. **Cultural Context**:
   - Thanksgiving = Widely observed, most businesses closed
   - Black Friday (Nov 28) = Many get 4-day weekend
   - Holiday season begins = Reduced availability common

**Application:**
- Avoid scheduling important meetings Nov 27-28
- Expect reduced availability Nov 27-30 (4-day weekend)
- Consider this when setting deadlines for late November

---

## When to Update

### Update Base Definitions When:
- ✅ Paradigm-level understanding changes
- ✅ ISO standards update (e.g., week numbering rules)
- ✅ Health scoring system evolves
- ✅ New universal work phase categories discovered

**Rare** - base definitions are foundational

### Update Regional Definitions When:
- ✅ Adding new geographic regions
- ✅ DST rules change legislatively
- ✅ Cultural work patterns shift significantly
- ✅ New federal holidays established

**Occasional** - as regions expand or norms evolve

### Do NOT Update Definitions For:
- ❌ Individual preferences → Use config
- ❌ Personal schedules → Use config
- ❌ Specific work patterns → Use patterns/learned data
- ❌ One-time events → Use appointed/schedule data

---

## Integration with Other Layers

### Definitions → Data

**Data uses definitions to categorize:**
```
Raw timestamp: 2025-11-08T15:30:16-06:00
↓ (Apply definitions)
Categorized: "Friday afternoon, CST, typical work hours"
```

### Definitions → Config

**Config uses definitions language:**
```
Config: "works better at night"
Definitions: "night" = 22:00-05:00
System: Can now measure and validate
```

### Definitions → Patterns

**Patterns validate config using definitions:**
```
Config: "works better at night"
Definitions: "night" category = 22:00-05:00
Patterns: 85% of sessions fall in "night" category
Validation: ✅ Config matches behavior
```

---

## Future Expansion

### Additional Regions

To add new regions, create:
```
regional/
└── <continent>/
    └── <country>/
        └── <region>/
            ├── timezone-rules.jsonc
            ├── time-categories.jsonc
            └── work-patterns.jsonc
```

**Example: Adding Europe/UK/London:**
```
regional/
└── europe/
    └── uk/
        ├── holidays.jsonc              # UK-wide holidays
        └── london/
            ├── timezone-rules.jsonc    # GMT/BST
            ├── time-categories.jsonc   # UK time categories
            └── work-patterns.jsonc     # UK work patterns
```

### Additional Base Definitions

As CPI-SI paradigm grows, may add:
- Energy level indicators
- Collaboration intensity categories
- Learning vs producing phases
- Creative vs analytical work states

Add to `base/` when concepts are **truly universal** (not regional or personal).

---

## Key Principles

1. **Definitions ≠ Requirements**
   - Definitions describe categories
   - They don't prescribe behavior
   - "Night = 22:00-05:00" doesn't mean you must sleep then

2. **Measurement Enables Understanding**
   - Can't validate preferences without measurement framework
   - Consistent definitions enable pattern recognition
   - Standards allow comparison across time and people

3. **Inheritance Provides Flexibility**
   - Base = universal foundation
   - Regional = cultural context
   - Config = personal interpretation
   - Data = actual behavior
   - All layers work together

4. **Grace and Truth**
   - Misalignment between config and data isn't condemnation
   - It's information for growth and understanding
   - Definitions serve wisdom, not judgment

---

**Related Documentation:**
- `../celestial/` - God's created order for time (sunrise, sunset, seasons)
- `../chronological/` - Calendar anchored to celestial reality
- `../patterns/` - Learned behavior and rhythms over time
- `../../config/user/` - User identity and preferences
- `../../config/instance/` - Instance identity and calling

---

## Templates and Schemas

### Templates Philosophy

**Location:** `templates/base/` and `templates/regional/`

Templates use **valid dummy data** with instructional comments:
- ✅ Each template validates against its schema immediately
- ✅ Dummy data provides working example structure
- ✅ Comments indicate what to UPDATE, CUSTOMIZE, ADD MORE
- ✅ User can validate template BEFORE making changes for confidence

**Why valid dummy data instead of placeholders:**
- Placeholder text like `"[YOUR_VALUE_HERE]"` fails schema validation
- Valid data means template works as-is (could theoretically be used)
- Comments guide customization without breaking JSON structure
- Immediate validation feedback as user makes changes

### Schema Validation Philosophy

**Location:** `../../config/schemas/temporal/definitions/`

Validation is **manual review process**, not automated:
- Read each definition file carefully
- Compare against schema requirements
- **Decide:** Update schema (if data is correct) OR update data (if schema provides better approach)
- Never blindly apply automated fixes - nuance matters

**Why manual validation:**
- Automated tools damage data by missing context
- Schema assumptions may be wrong (regional variation exists)
- Data may reveal better patterns than schema anticipated
- Careful discernment produces better results than scripts

### Extensions Pattern

**Every definition file includes:**
```jsonc
"extensions": {
  "note": "Experimental patterns and discoveries",
  "usage_guidance": "Add discoveries here during exploration"
}
```

**Purpose:** Discovery staging for patterns not yet formalized
- Schema allows `additionalProperties: true` for flexibility
- Add experimental discoveries without formal schema changes
- Promote to formal sections when patterns proven across instances
- Remove when patterns not useful or not observed

**Lifecycle:** Add → Gather evidence → Promote to formal OR remove

---

*Last Updated: 2025-11-08*
*Status: ✅ Complete - 8 definitions, 8 schemas, 8 templates, all validated*
