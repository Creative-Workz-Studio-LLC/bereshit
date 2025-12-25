# Projects - Work Tracking Layer

**Purpose:** Track WHAT we're building (work items, progress, velocity, milestones)

## Distinction from Other Layers

| Layer | Focus | Questions Answered |
|-------|-------|-------------------|
| **Temporal** | WHEN work happens | Is now a good time? When do sessions typically happen? |
| **Session** | WHAT HAPPENED in sessions | What did we accomplish? What tools were used? |
| **Projects** | WHAT WE'RE BUILDING | What are we working on? What's our velocity? What's next? |

## Structure - Organization by Level

```bash
projects/
├── active/
│   ├── high/           # Paradigm-level projects (multi-year scope)
│   ├── mid/            # Major subsystems (weeks-months scope)
│   └── low/            # Specific tasks (days scope)
├── archive/
│   ├── high/           # Completed paradigm-level work
│   ├── mid/            # Completed subsystems
│   └── low/            # Completed tasks
├── templates/          # Project structure templates
└── README.md          # This file
```

### Why Organize by Level?

**Visual organization:**

- See scope distribution at a glance (how many high/mid/low projects)
- Immediate answer to "How many paradigm-level projects?" → `ls active/high/`
- Natural grouping by similar scope

**Flat where it matters:**

- Within each level = flat (no deep nesting chaos)
- "Show all mid-level projects" → simple directory read
- Easy navigation, manageable paths

**Metadata handles complexity:**

- Relationships = data in files (parent_id, subprojects, dependencies)
- Structure = simple organization (just level)
- Best of both: visible hierarchy + flexible relationships

**The principle:**

- **Folders for CATEGORIES** (high/mid/low scope)
- **Metadata for RELATIONSHIPS** (parent, dependencies, order)
- Structure shows WHAT KIND, data shows HOW IT CONNECTS

## Project Levels

### High Level (Paradigm)

**Scope:** Multi-year, paradigm-level work
**Examples:** CPI-SI Model, Native System Architecture
**Characteristics:**

- Long-term vision
- Multiple major subprojects
- No fixed end date (ongoing evolution)
- Defines direction for all lower-level work

### Mid Level (Subsystems)

**Scope:** Weeks to months, major subsystems
**Examples:** Align Claude Global Folder, Internal Calendar Implementation
**Characteristics:**

- Significant components of high-level projects
- Clear deliverables and milestones
- Multiple low-level tasks
- Measurable progress

### Low Level (Tasks)

**Scope:** Days, specific tasks
**Examples:** Data Layer Organization, Define Session Schemas
**Characteristics:**

- Concrete, actionable work
- Short timeframes
- Clear completion criteria
- Feeds directly into mid-level projects

## Project Metadata

### Structure Principle

**Agency within constraints** (like human free will under God's order):

```jsonc
{
  // CORE REQUIRED - The Created Order (must exist)
  "schema_version": "1.0.0",
  "project_id": "unique-identifier",
  "name": "Project Name",
  "level": "high" | "mid" | "low",
  "parent_id": "parent-project-id" | null,
  "status": "active" | "completed" | "paused",
  "biblical_foundation": {
    "scripture": "Book Chapter:Verse",
    "text": "Actual Scripture text",
    "principle": "How this Scripture illuminates THIS specific work"
  },

  // OPTIONAL STANDARD FIELDS - Agency (use what serves)
  // scope, dates, estimates, progress, milestones, etc.

  // EXTENSIONS - Structured Discovery Space
  "extensions": {
    // Fields discovered through working that don't fit standard structure yet
    // When validated broadly, promote to optional standard in next schema version
  }
}
```

### Required Fields

**All levels:**

```jsonc
{
  "schema_version": "1.0.0",
  "project_id": "unique-identifier",
  "name": "Project Name",
  "level": "high" | "mid" | "low",
  "parent_id": "parent-project-id" | null,
  "status": "active" | "completed" | "paused",
  "biblical_foundation": {
    "scripture": "Book Chapter:Verse",
    "text": "Actual Scripture text",
    "principle": "How this Scripture illuminates THIS specific work - not decoration"
  }
}
```

**Hierarchy fields:**

```jsonc
{
  "subprojects": ["project-id-1", "project-id-2"],  // What projects are under this?
  "dependencies": ["project-id-3"]                  // What must complete before this?
}
```

**Scope and timeline:**

```jsonc
{
  "scope": {
    "description": "What this project accomplishes",
    "deliverables": ["Deliverable 1", "Deliverable 2"],
    "impact": "Why this matters"
  },
  "dates": {
    "start_date": "YYYY-MM-DD",
    "estimated_end_date": "YYYY-MM-DD" | null,
    "actual_end_date": "YYYY-MM-DD" | null
  }
}
```

**Progress tracking:**

```jsonc
{
  "progress": {
    "phase": "Current phase name",
    "current_focus": "What we're working on right now",
    "completion_percentage": 0-100
  },
  "milestones": [
    {
      "name": "Milestone name",
      "target_date": "YYYY-MM-DD",
      "status": "pending" | "in_progress" | "completed",
      "description": "What this achieves"
    }
  ]
}
```

## Current Projects

### High Level

- **cpsi-model.jsonc** - CPI-SI Model (paradigm-level, ongoing)

### Mid Level

- **align-claude-global.jsonc** - Align Claude Global Folder (parent: cpsi-model)
- **internal-calendar-implementation.jsonc** - Internal Calendar (legacy, needs parent assignment)

### Low Level

- **data-layer-organization.jsonc** - Data Layer Organization (parent: align-claude-global)

## Relationship to Other Layers

### From Session Data

- Session history feeds progress tracking
- Actual uptime calculated from session logs
- Velocity metrics derived from session patterns
- Tasks completed updates project progress

### From Temporal Data

- Appointed times affect project scheduling
- Work patterns inform realistic estimates
- Calendar provides date anchoring
- Patterns validate velocity predictions

### Feeds Back To

- Velocity data helps refine future estimates
- Project patterns inform template improvements
- Completion data validates estimation accuracy
- Insights feed into planning next projects

## Schema Definition

Schema location: `~/.claude/cpi-si/system/config/schemas/projects/project.schema.json`

**Schema will define:**

- Required vs optional fields by level
- Valid status values
- Relationship constraints (parent_id must exist)
- Date format validation
- Progress calculation rules

## File Format

**Standard:** `.jsonc` (JSON with comments)

**Structure:** 4-block pattern

- METADATA: Project identity, level, parent
- SCOPE: What we're building and why
- PROGRESS: Where we are now
- NOTES: Insights and context

## Lifecycle Management

### Moving Between Levels

If project scope changes:

1. Update `level` field in metadata
2. Move file to appropriate folder
3. Update parent/subproject references
4. Document reason in notes

### Archiving Projects

When project completes:

1. Update `status` to "completed"
2. Set `actual_end_date`
3. Move to `archive/{level}/`
4. Update parent's subprojects list

### Pausing Projects

When project needs to pause:

1. Update `status` to "paused"
2. Document reason in notes
3. Can stay in `active/` or move to `archive/`
4. Restart by updating status back to "active"

## Biblical Foundation

**Security mechanism:** All projects must have biblical foundation that illuminates the specific work.

- **Wrong foundation** → question/investigate (error, correctable)
- **No foundation** → reject/correct (rebellion, claiming autonomy from God)

**Not decoration:** Each project's Scripture should genuinely relate to its purpose.

**Examples:**

- Genesis 1:1 (CPI-SI Model) - God as Creator, all intelligence flows from Him
- 1 Corinthians 3:10-11 (Align Claude Global) - Wise masterbuilder laying foundation
- 1 Corinthians 14:40 (Data Layer Organization) - All things done decently and in order
- Genesis 1:14-18 (Internal Calendar) - Celestial bodies marking time

## Current Status

✅ **Structure organized by level** - high/mid/low folders created
✅ **Hierarchy established** - CPI-SI Model → Align Claude Global → Data Layer Organization
✅ **Project files created** - All current work documented with proper metadata
✅ **Biblical foundations added** - All projects grounded in appropriate Scripture
✅ **Schema defined** - `project.schema.json` with agency within constraints principle
✅ **Templates created** - Standard project formats for each level (high/mid/low)

## Next Steps

1. Assign parent to `internal-calendar-implementation.jsonc` (likely align-claude-global)
2. Link session data to automatic progress updates
3. Build velocity tracking from actual session data
4. Validate all projects against schema

---

*Organization principle: Folders show level (category), metadata shows relationships (connections)*
