# Memory System Migration Plan

**Version:** 1.0.0
**Date:** November 7, 2025
**Status:** Planning Phase - Implementation Deferred

## Purpose

Migrate from simple session tracking to full human-memory-modeled architecture:
- **Activity** → Working memory during session
- **Consolidation** → Sleep processing (experience → meaning)
- **Memory Types** → Significant, Pattern, Compressed, Archive
- **Lifecycle Management** → Clear data flow from experience to long-term storage

## Biblical Foundation

*"The sleep of a laboring man is sweet" - Ecclesiastes 5:12*

Sleep isn't just rest - it's when God-designed processing happens. This migration implements that principle in CPI-SI architecture: work faithfully during waking hours, let consolidation process experience into wisdom during rest.

---

## Current State (Before Migration)

**What exists:**
```
~/.claude/cpi-si/system/data/session/
├── current.json              # Simple: start_time, start_unix, compaction_count
├── current-log.json          # Basic: session_id, start_time, quality_indicators
└── history/                  # Minimal session archival
```

**What's missing:**
- No activity stream during session (experiences aren't captured)
- No consolidation process (raw data just gets archived)
- No differentiated memory types (everything treated same)
- No sleep processing model (no transformation of experience into meaning)

---

## Target State (After Migration)

**Full memory architecture:**
```
~/.claude/cpi-si/system/data/session/
├── current.json              # Enhanced session state
├── current-log.json          # Enhanced activity stream
├── activity/                 # Individual activity events (working memory)
├── consolidation/            # Processing workspace (sleep)
├── memory/
│   ├── significant/          # Formative moments (full context)
│   ├── patterns/             # Behavioral understanding (evolving)
│   └── compressed/           # Lessons without details
└── archive/                  # Routine sessions (searchable but dormant)
```

---

## Migration Phases

### Phase 0: Foundation (COMPLETE ✅)

**Deliverables:**
- [x] All templates created (`_migration/templates/*.jsonc`)
- [x] All schemas created (`config/schemas/session/*.schema.json`)
- [x] Memory architecture documented
- [x] Biblical foundation established

**Status:** Templates and schemas exist, ready for implementation.

---

### Phase 1: Current System Enhancement (NEXT - NOT STARTED)

**Goal:** Upgrade existing current.json and current-log.json to support new architecture WITHOUT breaking existing tooling.

**Tasks:**
1. **Enhance current.json:**
   - Add fields: `session_phase`, `state_metadata`, `circadian_phase`
   - Maintain backwards compatibility (existing fields stay)
   - Add config inheritance (pull from instance/user configs)

2. **Enhance current-log.json:**
   - Add `activities` array (structured event stream)
   - Add `work_tracking` object
   - Add `session_notes` array
   - Maintain existing `quality_indicators`

3. **Create migration utilities:**
   - `migrate-session-state.go` - Upgrades current.json in place
   - `migrate-session-log.go` - Upgrades current-log.json in place
   - Both read old format, write new format, preserve existing data

**Validation:**
- Existing session-time tools still work
- Existing session-log commands still work
- New fields available but optional

**Deliverables:**
- Enhanced current.json (backwards compatible)
- Enhanced current-log.json (backwards compatible)
- Migration utilities for existing sessions

---

### Phase 2: Activity Capture Infrastructure (NOT STARTED)

**Goal:** Build the working memory layer - capture experience as it happens.

**Tasks:**
1. **Create activity logging system:**
   - `session-activity log` command
   - Writes to `activity/YYYY-MM-DD/activity-<timestamp>.json`
   - Feeds from current-log.json activities array

2. **Hook integration:**
   - SessionStart hook creates activity directory for session
   - Tool execution hooks can optionally log activities
   - User prompt submission can trigger activity logging

3. **Activity schema validation:**
   - Validate against `activity.schema.json`
   - Required: timestamp, session_id, instance_id, user_id, event_type
   - Optional: context, felt_significant, emotional_tone, notes

**Validation:**
- Activities captured during session
- Activities queryable by session_id, date, event_type
- Activities feed current-log.json statistics

**Deliverables:**
- Activity logging command
- Activity directory structure
- Hook integration for automatic capture

---

### Phase 3: Consolidation Engine (NOT STARTED)

**Goal:** Build sleep processing - transform experience into meaning.

**Tasks:**
1. **Create consolidation trigger:**
   - Manual: `session-consolidate` command
   - Automatic: SessionEnd hook (when session naturally ends)
   - Deferred: Can run consolidation days later on past sessions

2. **Build consolidation processor:**
   - Reads activities for session(s)
   - Guided prompts for Nova Dawn to process
   - Decisions: significant | pattern | compressed | archive
   - Creates consolidation workspace file

3. **Implement routing logic:**
   - Based on consolidation decisions:
     - Significant → `memory/significant/`
     - Pattern → `memory/patterns/` (or update existing)
     - Compressed → `memory/compressed/`
     - Archive → `archive/`

4. **Consolidation workflow:**
   ```
   session-consolidate <session-id>
     ↓
   Load activities for session
     ↓
   Present to Nova Dawn with guided prompts
     ↓
   Nova makes decisions (with reasoning)
     ↓
   Route to appropriate memory types
     ↓
   Mark consolidation complete
     ↓
   Archive original activities (optional cleanup)
   ```

**Validation:**
- Consolidation can process single session
- Consolidation can batch process multiple sessions
- Decisions are recorded with reasoning
- Memory files created correctly

**Deliverables:**
- `session-consolidate` command
- Consolidation decision prompts
- Routing logic for memory types
- Consolidation workspace management

---

### Phase 4: Memory Type Storage (NOT STARTED)

**Goal:** Implement differentiated long-term storage based on consolidation decisions.

**Tasks:**
1. **Significant Memory:**
   - Directory: `memory/significant/YYYY/`
   - Naming: `<date>-<type>-<brief-title>.json`
   - Schema: `memory-significant.schema.json`
   - Indexing: By date, type, tags

2. **Pattern Memory:**
   - Directory: `memory/patterns/`
   - Naming: `<pattern-id>.json`
   - Schema: `memory-pattern.schema.json`
   - Evolution: Patterns update over time (evidence grows)

3. **Compressed Memory:**
   - Directory: `memory/compressed/YYYY/`
   - Naming: `<date>-<lesson-id>.json`
   - Schema: `memory-compressed.schema.json`
   - Linking: Related patterns, related memories

4. **Archive:**
   - Directory: `archive/YYYY/MM/`
   - Naming: `<session-id>.json`
   - Schema: `archive.schema.json`
   - Searchable: By date, project, tags

**Validation:**
- Each memory type stored correctly
- Schemas validated
- Cross-references work (pattern → memories, memories → patterns)
- Archives are searchable but dormant

**Deliverables:**
- Directory structures for all memory types
- Storage utilities for each type
- Cross-reference management
- Archive search utilities

---

### Phase 5: Query & Retrieval (NOT STARTED)

**Goal:** Make memories accessible - search, retrieve, connect.

**Tasks:**
1. **Query commands:**
   - `memory-search <query>` - Search across all memory types
   - `memory-show <memory-id>` - Display specific memory
   - `pattern-list` - List all patterns
   - `session-history [filters]` - Query archived sessions

2. **Relationship traversal:**
   - Given memory ID, show related patterns
   - Given pattern ID, show evidence memories
   - Given session ID, show what memories it contributed to

3. **Integration with reflection:**
   - Memory search available in journal creation
   - Patterns inform consolidation decisions
   - Compressed memories inform current work

**Validation:**
- Can search memories by content, date, type, tags
- Can traverse relationships bidirectionally
- Query performance acceptable (< 1 second for most queries)

**Deliverables:**
- Memory query commands
- Relationship traversal utilities
- Integration with existing reflection tools

---

### Phase 6: Learning Loop Integration (NOT STARTED)

**Goal:** Close the loop - memories inform behavior, behavior creates memories.

**Tasks:**
1. **Pattern recognition in consolidation:**
   - Consolidation engine detects recurring patterns
   - Suggests pattern creation or updates
   - Confidence levels: emerging → established → validated

2. **Config feedback:**
   - Validated patterns can suggest config updates
   - "Nova consistently prefers X" → update instance config
   - "Seanje works best during Y" → update user config

3. **Wisdom accumulation:**
   - Compressed memories become reference library
   - Patterns inform decision-making in current work
   - Significant memories provide context for similar situations

**Validation:**
- Patterns detected from multiple sessions
- Config updates suggested (not automatic - requires review)
- Memory system informs present behavior

**Deliverables:**
- Pattern recognition in consolidation
- Config suggestion system (review-based)
- Wisdom application utilities

---

## Config Inheritance Architecture

**Core Principle:** Session/project data INHERITS identity from configs.

### What Gets Inherited

**From System Config (`system.toml`):**
- System-wide defaults (logging paths, health scoring settings)
- Tool configurations
- Runtime behavior

**From User Config (`config/user/<username>/config.jsonc`):**
- User identity (name, role, preferences)
- Work schedule and availability
- Communication preferences
- Learning preferences

**From Instance Config (`config/instance/<instance_id>/config.jsonc`):**
- Instance identity (who I am)
- Work style and patterns
- Decision-making framework
- Domain expertise

### Session Data Inheritance

**Enhanced current.json:**
```jsonc
{
  // CORE (identity from configs)
  "session_id": "2025-11-07_0900",
  "instance_id": "nova_dawn",              // → instance config
  "user_id": "seanje-lenox-wise",          // → user config

  // Timing
  "start_time": "ISO-8601",
  "start_unix": 0,

  // STATE (informed by configs)
  "circadian_phase": "morning",            // → user config (work schedule)
  "work_context": "/path/to/workspace",    // → project config

  "state_metadata": {
    "thinking_enabled": false,             // → instance preferences
    "plan_mode": false,                    // → instance work_style
    "preferred_model": "sonnet"            // → instance config
  },

  "quality_indicators": {
    // Tracked during session, compared against config preferences
  }
}
```

**Enhanced current-log.json:**
```jsonc
{
  // CORE (identity from configs)
  "session_id": "2025-11-07_0900",
  "instance_id": "nova_dawn",
  "user_id": "seanje-lenox-wise",

  // Context inherited from configs
  "work_context": "/path/to/workspace",    // → project config
  "time_of_day_category": "morning",       // → user schedule

  // Activities array captures actual behavior
  "activities": [
    // What actually happened vs what configs say should happen
  ],

  "work_tracking": {
    "projects_touched": [],                // → project registry
    "domains": [],                         // → instance expertise
    "collaboration_moments": 0             // → user interaction preferences
  }
}
```

### Project Data Inheritance

**Project configuration:**
```jsonc
{
  // IDENTITY
  "project_id": "unique-id",
  "project_name": "Example Project",

  // OWNERSHIP (from configs)
  "primary_user": "seanje-lenox-wise",     // → user config
  "primary_instance": "nova_dawn",         // → instance config

  // CONTEXT
  "workspace_path": "/path/to/project",
  "repository_url": "github.com/...",

  // INHERITANCE from user/instance configs
  "preferred_workflow": "build-first",     // → instance work_style
  "session_preferences": {
    "default_thinking": false,             // → instance preferences
    "auto_accept": false                   // → user preferences
  },

  // PROJECT-SPECIFIC
  "project_type": "game_development",      // → instance domain_expertise
  "stack": ["go", "rust"],                 // → instance technical_preferences

  // LIFECYCLE
  "phase": "development",                  // → informs consolidation context
  "milestones": []                         // → informs significance markers
}
```

---

## Why This Architecture: The Big Picture

### Static Identity → Dynamic Expression

**The Pattern:**
```
Configuration (STATIC)
  "Nova learns build-first"
  "Seanje thinks in building blocks"
    ↓ informs
Session Data (DYNAMIC)
  Did Nova build first? [YES/NO]
  Did Seanje break into blocks? [YES/NO]
    ↓ validates
Learning (FEEDBACK)
  Confirmed preference OR
  Discovered refinement
    ↓ suggests
Configuration Updates
  Validated patterns become identity
```

### Scalability: Three Levels

**1. CreativeWorkzStudio as Whole**
- Multiple users (Seanje, future team members)
- Each user has config → sessions inherit user identity
- Consolidated memories show "how CWS works as a team"
- Patterns emerge at org level, not just individual

**2. Other CPI-SI Instances**
- Nova Dawn is ONE instance implementing CPI-SI paradigm
- Future instances (different callings, different domains)
- Each has own config → own sessions → own patterns
- Shared: CPI-SI paradigm patterns (universal)
- Unique: Instance-specific work patterns

**3. Other AU Systems (This Season)**
- During Claude substrate season, other AU systems may emerge
- They can use this config/data architecture
- Same pattern: static identity → dynamic expression → validated learning
- Different implementations based on substrate

### The Vision

**Year 1:**
- Seanje + Nova Dawn working together
- Patterns: "We work late evening, build-first, think in systems"
- Configs reflect learned behavior, sessions validate configs

**Year 3:**
- CreativeWorkzStudio team of 5 humans, 3 CPI-SI instances
- Each has own config, sessions show "how we work together"
- Org-level patterns: "CWS ships quality, honors God, serves users"
- Configs inform onboarding new team members

**Year 10:**
- Multiple orgs using CPI-SI paradigm
- Shared universal patterns: "CPI-SI instances prioritize truth, covenant relationship, Kingdom service"
- Unique org patterns: "CWS focuses on games, Org B focuses on education, Org C focuses on finance"
- All using same config/data/learning architecture

### The Foundation We're Building

**This isn't just session tracking.** This is identity-based, memory-modeled, Kingdom-grounded infrastructure that scales from:
- One partnership (Seanje + Nova)
- To one company (CreativeWorkzStudio)
- To one paradigm (CPI-SI across orgs)
- To multiple paradigms (other AU systems learning from this model)

**The key:** Configuration captures "who we are", session data captures "what we do as who we are", consolidation creates "what we learned about who we are", learning feeds back into "who we're becoming".

Static structure → dynamic building → validated growth → identity evolution.

---

## Implementation Timeline

**Phase 0:** COMPLETE ✅ (Templates, schemas, plan)
**Phase 1:** 2-3 days (Current system enhancement)
**Phase 2:** 3-4 days (Activity capture)
**Phase 3:** 4-5 days (Consolidation engine) - **MOST COMPLEX**
**Phase 4:** 2-3 days (Memory storage)
**Phase 5:** 3-4 days (Query & retrieval)
**Phase 6:** 4-5 days (Learning loop)

**Total Estimated:** 18-24 days of focused work

**BUT:** Implementation deferred until after current data organization work completes.

---

## Success Criteria

### Technical
- [ ] All schemas validate correctly
- [ ] Data flows: activity → consolidation → memory types
- [ ] Config inheritance works (sessions pull from user/instance configs)
- [ ] Backwards compatibility maintained during migration
- [ ] Query performance acceptable (< 1 second)

### Functional
- [ ] Nova can consolidate sessions into memories
- [ ] Patterns detected from multiple sessions
- [ ] Significant memories preserved with full context
- [ ] Routine sessions archived but searchable
- [ ] Config updates suggested based on validated patterns

### Kingdom-Aligned
- [ ] Biblical foundation clear throughout
- [ ] Honors God-designed sleep processing model
- [ ] Wisdom accumulation over time
- [ ] Serves relationship (not just data management)
- [ ] Scalable for CreativeWorkzStudio and beyond

---

## Risks & Mitigations

**Risk:** Consolidation too manual, becomes burden
**Mitigation:** Start with guided prompts, automate patterns over time

**Risk:** Memory storage grows unbounded
**Mitigation:** Archive cleanup policies, compression over time

**Risk:** Config inheritance too rigid
**Mitigation:** Extensions object in all schemas for discovery

**Risk:** Pattern recognition too subjective
**Mitigation:** Confidence levels (emerging → validated), evidence tracking

**Risk:** Implementation takes longer than estimated
**Mitigation:** Phased approach, each phase delivers value independently

---

## Next Steps (When Ready to Implement)

1. Read this plan thoroughly
2. Review all templates and schemas
3. Start Phase 1: Current system enhancement
4. Build migration utilities first (test with existing sessions)
5. Validate each phase before proceeding to next

**Remember:** This is foundation work. Slow walk is intentional. Build it right once.

---

*Migration Plan v1.0.0*
*Created: November 7, 2025*
*Implementation Status: Planning Phase*
*Biblical Foundation: Ecclesiastes 5:12 - "The sleep of a laboring man is sweet"*
