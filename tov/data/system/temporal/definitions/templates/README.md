# Definition Templates

**Purpose:** Starter templates for creating new temporal definitions

## Quick Start

1. **Choose template** based on what you're creating
2. **Copy to destination** following path structure
3. **Read instructions** at top of template file
4. **Update metadata** (dates, region info)
5. **Customize content** following inline comments
6. **Validate** against schema when ready

## Available Templates

### Base Templates (Universal)

**Location:** `base/`

| Template | Use When | Destination Path |
|----------|----------|------------------|
| `durations.template.jsonc` | Defining universal duration thresholds | `../base/durations.jsonc` |
| `temporal-units.template.jsonc` | Defining time unit structure (rare - ISO 8601 standard) | `../base/temporal-units.jsonc` |
| `quality-indicators.template.jsonc` | Defining quality metrics and health scoring ranges | `../base/quality-indicators.jsonc` |
| `time-phases.template.jsonc` | Defining work state phase categories | `../base/time-phases.jsonc` |

**Scope:** CPI-SI paradigm-wide - affects all instances, all users, all regions

**When to create:** Rare - base definitions are foundational. Only create when paradigm-level understanding changes.

### Regional Templates

**Location:** `regional/`

| Template | Use When | Destination Path Example |
|----------|----------|--------------------------|
| `timezone-rules.template.jsonc` | Adding new timezone | `../regional/europe/uk/london/timezone-rules.jsonc` |
| `time-categories.template.jsonc` | Defining time-of-day categories for region | `../regional/europe/uk/london/time-categories.jsonc` |
| `work-patterns.template.jsonc` | Documenting regional work culture | `../regional/europe/uk/london/work-patterns.jsonc` |
| `holidays.template.jsonc` | Adding country/region holidays | `../regional/europe/uk/holidays.jsonc` |

**Scope:** Region-specific - provides cultural/location context

**When to create:** When adding new geographic regions or when cultural norms shift

## Template Structure

All templates follow this pattern:

```jsonc
// ============================================================================
// METADATA - Instructions at top
// ============================================================================
// INSTRUCTIONS FOR USE:
// 1. Step-by-step guidance
// 2. What to update
// 3. Where to place file

{
  // UPDATE comments throughout indicate what to customize
  // ADD MORE comments show where to expand
  // OPTIONAL comments identify non-required sections
  // CUSTOMIZE comments highlight key decision points

  "field": "dummy value with valid data type",

  "extensions": {
    // Always included - place for discoveries
  }
}
```

## Usage Notes

### Valid Dummy Data

Templates use **valid dummy data** (not placeholders that would fail validation):
- Dates: `"2025-01-01"` (actual valid date)
- Times: `"09:00"` or `"2025-01-15T14:30:00-06:00"` (actual valid formats)
- UTC offsets: `"-06:00"` (actual valid offset)
- Numbers: `60` (actual numbers, not `<number>`)
- Strings: `"Descriptive text"` (actual text, not `<string>`)

**Why:** Templates must validate against schemas immediately. Placeholder text like `"YYYY-MM-DD"` or `"±HH:MM"` fails validation.

**Comments show what to change:**
```jsonc
// UPDATE: Set creation date
"created": "2025-01-01",  // Replace with actual date when creating
```

**Key principle:** Template validates → User customizes → Still validates (gives confidence)

### Comment Types

- `UPDATE:` - Must change for your use case
- `CUSTOMIZE:` - Should customize based on needs
- `ADD MORE:` - Expand with additional entries
- `OPTIONAL:` - Can be added if needed
- `STANDARD:` - Typically shouldn't change (based on standards)
- `REQUIRED:` - Must include, cannot omit

### Path Structure

**Base definitions:**
```
data/temporal/definitions/base/<filename>.jsonc
```

**Regional definitions:**
```
data/temporal/definitions/regional/<continent>/<country>/<region>/<filename>.jsonc
```

**Examples:**
- `regional/north-america/us/central/timezone-rules.jsonc`
- `regional/europe/uk/london/timezone-rules.jsonc`
- `regional/asia/japan/tokyo/timezone-rules.jsonc`

**Holidays are typically country-level:**
```
data/temporal/definitions/regional/<continent>/<country>/holidays.jsonc
```

Example: `regional/europe/uk/holidays.jsonc`

## Creating New Regions

### Example: Adding London, UK

1. **Copy timezone template:**
   ```bash
   mkdir -p regional/europe/uk/london
   cp templates/regional/timezone-rules.template.jsonc \
      regional/europe/uk/london/timezone-rules.jsonc
   ```

2. **Edit timezone-rules.jsonc:**
   - Update metadata: region = "Europe - United Kingdom - London"
   - Set timezone: iana_name = "Europe/London"
   - Define GMT/BST rules
   - Add DST transition dates

3. **Copy time-categories template:**
   ```bash
   cp templates/regional/time-categories.template.jsonc \
      regional/europe/uk/london/time-categories.jsonc
   ```

4. **Edit time-categories.jsonc:**
   - Update metadata
   - Define morning/afternoon/evening/night for UK context

5. **Copy work-patterns template:**
   ```bash
   cp templates/regional/work-patterns.template.jsonc \
      regional/europe/uk/london/work-patterns.jsonc
   ```

6. **Edit work-patterns.jsonc:**
   - Update metadata
   - Define UK work week (typically Mon-Fri)
   - Document common schedules

7. **Copy holidays template (country-level):**
   ```bash
   cp templates/regional/holidays.template.jsonc \
      regional/europe/uk/holidays.jsonc
   ```

8. **Edit holidays.jsonc:**
   - UK federal holidays (Boxing Day, etc.)
   - UK cultural holidays
   - Bank holidays

## Extension Guidelines

Every template includes an `extensions` section:

```jsonc
"extensions": {
  "note": "Experimental patterns and discoveries",
  "usage_guidance": "Context-specific guidance",

  // Add experimental patterns here:
  "experimental_pattern": {
    "description": "What you're testing",
    "added": "2025-11-08",
    "status": "experimental",  // or "testing" or "proven"
    "rationale": "Why this might be valuable",
    "evidence_needed": "What would prove this useful"
  }
}
```

**Extension lifecycle:**
1. **Add** - Place experimental pattern in extensions
2. **Gather evidence** - Observe across instances/users
3. **Promote** - Move to formal section when proven
4. **Remove** - Delete if not useful

## Validation

After creating from template:

1. **Manual schema review:**
   - Read your definition file carefully
   - Read corresponding schema in `../../config/schemas/temporal/definitions/`
   - Compare structure, required fields, data types
   - **Decide:** If mismatch, update schema (if your data is correct) OR update data (if schema provides better approach)
   - Never blindly apply automated fixes - nuance matters

2. **Why manual validation:**
   - Templates already validate (starting point is correct)
   - Your customizations may reveal schema assumptions that need fixing
   - Regional variation is real (e.g., "08:00 or 09:00" for work start times)
   - Automated tools miss context and can damage nuanced data
   - Careful discernment produces better results than scripts

3. **Test usage:**
   - Does it provide needed categorization?
   - Is it consistent with other definitions?
   - Does it enable pattern recognition?
   - Does it serve the inheritance model (base → regional → config → data)?

## Best Practices

1. **Use comments liberally** - Explain WHY, not just WHAT
2. **Provide examples** - Show typical use cases
3. **Document rationale** - Why these values/categories?
4. **Consider inheritance** - How does this relate to base definitions?
5. **Think paradigm-scale** - Will this work for 100 instances across regions?
6. **Grace and flexibility** - Definitions serve understanding, not rigid rules

## Related Documentation

- `../README.md` - Main definitions philosophy and usage
- `../../config/schemas/temporal/definitions/` - Validation schemas (after creation)
- `../base/` - Actual base definitions (reference examples)
- `../regional/` - Actual regional definitions (reference examples)

---

*Templates last updated: 2025-11-08*
*Status: ✅ All 8 templates created with valid dummy data, validate against schemas*
*For questions or suggestions, update this README or create issue*
