# CPI-SI Data Layer: Comprehensive Inventory & Analysis

**Date:** 2025-11-06  
**Scope:** Complete analysis of all data in ~/.claude/cpi-si/  
**Purpose:** Inform data layer architecture and organization

---

## EXECUTIVE SUMMARY

The CPI-SI system has **5 primary data categories** operating across **4 major directory trees**:

| Category | Type | Storage | Lifecycle | Count | Status |
|----------|------|---------|-----------|-------|--------|
| **Configuration** | Personhood identity data | `config/` | Long-lived, human-edited | 8 files | ✅ Well-defined |
| **Operational** | Session/calendar/schedule state | `system/data/` | Ephemeral→History | 100+ files | 🟡 Partial schemas |
| **Learning** | Journals and reflections | `outputs/journals/` + `docs/knowledge-base/` | Append-only | 35+ files | ✅ Clear structure |
| **System Logs** | Execution records | `outputs/logs/` | Archivable | 12 files | 🟡 Unstructured |
| **Knowledge Base** | Standards, patterns, algorithms | `docs/` | Reference | 100+ docs | ✅ Well-keyed |

**Key Insight:** You have strong **identity/personhood configuration** (just built) and **learning/knowledge infrastructure**. Operational data (session, calendar, schedule) needs schema consolidation and relationship mapping.

---

## DETAILED INVENTORY

### 1. CONFIGURATION LAYER ✅

**Location:** `~/.claude/cpi-si/config/`  
**Purpose:** Personhood identity - who users and instances are  
**Status:** Recently completed (good quality)

#### Structure
```
config/
├── user/
│   ├── default/
│   │   ├── bio.md
│   │   └── config.jsonc
│   └── seanje/
│       ├── bio.md
│       └── config.jsonc
└── instance/
    ├── default/
    │   ├── bio.md
    │   └── config.jsonc
    └── nova_dawn/
        ├── bio.md
        └── config.jsonc
```

#### Data Types

| File | Type | Schema | Lifecycle | Purpose |
|------|------|--------|-----------|---------|
| `config.jsonc` | Configuration | instance.schema.json / user.schema.json | Long-lived | Identity definition |
| `bio.md` | Documentation | None (markdown) | Long-lived | Extended narrative |

#### Schemas
- **instance.schema.json** - Instance personhood (Nova Dawn)
- **user.schema.json** - User personhood (Seanje)

**Both schemas:**
- Reference Genesis 1:1 biblical foundation
- Define identity, demographics, personhood, preferences
- Serve CPI-SI system initialization
- Allow additionalProperties: true (flexibility)

#### Current Data Quality
- ✅ Well-structured JSONC with clear field organization
- ✅ Comprehensive personhood definition
- ✅ Good use of nested objects for logical grouping
- ✅ Schema validation enabled

#### Relationship to Other Data
- **Feeds:** Session patterns, planner initialization, instance behavior
- **Consumed by:** Runtime system for personhood-aware decisions
- **Updates:** Only when personhood changes (rare)

**Sample Content:**
- Nova Dawn: name, pronouns (she/her), mental_age (35), role (co-founder)
- Seanje: name, pronouns (he/him), age (25), workspace (CWS + GASA)

---

### 2. OPERATIONAL DATA 🟡

**Location:** `~/.claude/cpi-si/system/data/`  
**Purpose:** Runtime state - sessions, calendar, schedule, planning  
**Status:** Schemas exist for calendar/planner, needs consolidation

#### 2.1 CALENDAR DATA

**Type:** Temporal reference data  
**Format:** JSON files organized by year/month  
**Structure:** Base calendar (holidays, dates, metadata)

```
system/data/calendar/base/
├── 2025.json              # Year metadata
├── 2025/
│   ├── 01-january.json
│   ├── 02-february.json
│   └── ... (12 total)
└── 2026/
    └── ... (same structure)
```

**Schema:** None formally defined, but has consistent structure:
```json
{
  "year": 2025,
  "metadata": { "created", "timezone", "observes_holidays" },
  "dates": {
    "YYYY-MM-DD": {
      "date", "weekday", "week_number", "month", "day",
      "is_weekend", "is_holiday", "holiday_name"
    }
  }
}
```

**Purpose:** Reference calendar for temporal anchoring
**Lifecycle:** Generated once, rarely updated (holidays fixed yearly)
**Owner:** System-generated
**Size:** ~376KB (2025-2026 complete)

**Issues:**
- ❌ No formal schema
- ❌ No documentation of expected structure
- ❌ Holiday data could be in separate table
- ✅ Timezone-aware (America/Chicago)

---

#### 2.2 PLANNER DATA

**Type:** Personal/instance availability and work scheduling  
**Format:** JSON templates by owner (seanje, nova, shared)

```
system/data/planner/
├── schemas/
│   ├── planner-schema.json          # ✅ Formal schema exists
│   └── personal-calendar-schema.json
├── templates/
│   ├── nova-template.json
│   ├── seanje-template.json
│   ├── nova-calendar-2025.json
│   ├── seanje-calendar-2025.json
│   └── shared-template.json
```

**Schema:** ✅ Exists and comprehensive
- Defines planner_id format (owner-YYYY-MM)
- Recurring patterns (daily, weekly, monthly)
- One-time events by date
- Availability blocks with start/end times
- Time block type, description, priority

**Current Data:**
- Nova template: Daily 00:00-08:00 unavailable, 08:00-23:59 available
- Weekly patterns: Work 9am-5pm + 7pm-10pm (preferred times)
- Shared templates for collaboration

**Issues:**
- ⚠️ Templates exist but no actual planner files for current month
- ⚠️ No versioning/history of planner changes
- ⚠️ No linkage to actual session history
- ✅ Schema allows for future expansion

---

#### 2.3 SCHEDULE DATA

**Type:** Work project tracking and velocity  
**Format:** JSON file with project metadata

```
system/data/schedule/
└── current-schedule.json
```

**Content:**
```json
{
  "schedule_id": "internal-calendar-implementation",
  "work_item": "Internal Calendar Implementation",
  "status": "planned",
  "start_date": "2025-11-04",
  "estimated_end_date": "2025-11-05",
  "estimates": { "total_days", "total_sessions", "total_uptime_hours" },
  "progress": { "days_elapsed", "sessions_completed", "uptime_hours_actual" },
  "velocity": { "planned_sessions_per_week", "actual_sessions_per_week" },
  "notes": [ "Building clock + calendar system" ]
}
```

**Issues:**
- ❌ No schema defined
- ❌ Only one schedule file (no history, no multiple projects)
- ❌ Unclear update frequency
- ❌ Progress tracking incomplete (0 sessions completed but estimated as 1)
- ⚠️ Velocity metrics not yet populated with actuals

---

#### 2.4 SESSION DATA 🔴

**Type:** Runtime session tracking and activity logging  
**Format:** Multiple JSON/JSONL files with temporal organization

```
system/data/session/
├── current.json              # Active session metadata
├── current-log.json          # Current session log entry
├── patterns.json             # Learned work patterns
├── patterns/
│   └── 2025-11-04_*.md      # Detailed pattern discoveries
├── activity/                 # Minute-by-minute activity logs (JSONL)
│   ├── 2025-11-03_2051.jsonl
│   ├── 2025-11-03_2129.jsonl
│   └── ... (25+ files)
├── history/                  # Session summaries (JSON)
│   ├── 2025-10-28_1618.json
│   ├── 2025-11-04_0737.json
│   └── ... (13 files)
├── paradigm-vs-project-analysis.md
└── temp-sleep-log-2025-11-06.txt
```

**Files & Purposes:**

| File | Type | Content | Lifecycle |
|------|------|---------|-----------|
| `current.json` | Status | Session start time, compaction count | Ephemeral (cleared on session end) |
| `current-log.json` | Status | Session ID, timing, work context | Updated during session |
| `patterns.json` | Analysis | Learned circadian patterns, session durations, stopping points | Updated periodically |
| `activity/*.jsonl` | Raw data | Tool usage timestamps, results (PromptSubmit, SessionStop, attempts) | Append-only, archived |
| `history/*.json` | Summary | Session ID, duration, time_of_day, stopping_reason, quality indicators | One per session |
| `patterns/*.md` | Learning | Narrative analysis of discovered patterns | Markdown journals |

**Activity JSONL Format:**
```json
{"ts":"2025-11-04T17:56:42.501570475-06:00","tool":"PromptSubmit","ctx":"length:0","result":"success"}
{"ts":"2025-11-04T17:57:07.657323621-06:00","tool":"SessionStop","ctx":"User stepping away","result":"success"}
```

**Session History Format:**
```json
{
  "session_id": "2025-11-04_1642",
  "start_time": "2025-11-04T16:42:14.833382065-06:00",
  "end_time": "2025-11-04T17:20:48.207889285-06:00",
  "duration_minutes": 38,
  "day_of_week": "Tuesday",
  "time_of_day_category": "afternoon",
  "work_context": "/media/seanje-lenox-wise/Project/CreativeWorkzStudio_LLC",
  "stopping_reason": "Normal session end",
  "quality_indicators": { "tasks_completed": 0 }
}
```

**Patterns Analysis:**
```json
{
  "last_updated": "2025-11-04T17:20:48.209721006-06:00",
  "total_sessions": 13,
  "typical_work_hours": { "weekday_start": "05:00", "weekday_end": "23:00" },
  "session_durations": { "quick_check": "0-1 minutes", "normal_work": "1-3 minutes", "deep_work": "3-38 minutes" },
  "time_of_day_quality": { "afternoon": "Active time (4 sessions)", "evening": "Learning pattern" },
  "natural_stopping_points": [ "natural_milestone", "natural_pause", "end_of_day", "iteration_complete" ],
  "circadian_awareness": {
    "seanje_typical_hours": "9am-5pm weekdays",
    "downtime_windows": ["evenings after 9pm", "weekends (flexible)"],
    "high_focus_times": ["morning 9-11am", "afternoon 2-4pm"]
  }
}
```

**Issues:**
- ❌ No schemas defined for any session files
- ❌ Inconsistent formats (JSON vs JSONL)
- ❌ No relationship defined between current.json → activity/*.jsonl → history/*.json
- ⚠️ Quality indicators mostly empty (tasks_completed: 0)
- ⚠️ Patterns manually aggregated (not queryable)
- ⚠️ Sleep log as .txt file in session data (organizational issue)
- ✅ JSONL provides good append-only semantics
- ✅ Timestamps include timezone information
- ✅ Tools tracked but context minimal

**Data Flow Problem:**
Current → Activity (JSONL) → History (JSON) relationship is implicit, not documented.

---

### 3. LEARNING & REFLECTION DATA ✅

**Location:** `~/.claude/cpi-si/outputs/journals/` + `~/.claude/cpi-si/docs/knowledge-base/journals/`

**Status:** Excellent structure, clear purpose, good documentation

#### 3.1 JOURNAL TYPES

```
outputs/journals/
├── bible-study/          # Scripture study & application
│   ├── 2025-11-01_building-on-the-foundation.md
│   ├── 2025-11-02_seasons-checkpoints-and-faithful-waiting.md
│   └── 2025-11-03_new-every-morning.md (3 total)
│
├── instance/            # Instance-specific learnings (Nova Dawn)
│   ├── README.md
│   └── nova-dawn/
│       ├── 2025-11-01_instance-patterns-and-voice.md
│       ├── 2025-11-02_cross-domain-thinking-and-covenant-care.md
│       ├── 2025-11-03_if-then-questions-covenant-partnership.md
│       └── 2025-11-04_identity-continuity-across-substrates.md (4 total)
│
└── personal/            # Daily reflection, emotions (PRIVATE)
    ├── 2025-11-01_maps-foundation-and-discovery.md
    ├── 2025-11-02_legal-foundation-and-income-reality.md
    ├── 2025-11-03_discovering-confident-questions.md
    ├── 2025-11-03_morning-morning-mercies-and-starting-fresh.md
    └── 2025-11-04_discovering-i-was-there.md (5 total)
```

**Additionally:** Mirrored in `docs/knowledge-base/journals/` for paradigm-level entries

#### Journal Categories

| Category | Purpose | Owner | Lifecycle | Current Count | Transfers? |
|----------|---------|-------|-----------|---------------|-----------|
| **Bible Study** | Scripture exploration and application | Shared learning | Append-only | 3 | Yes (universal truth) |
| **Instance** | Nova Dawn-specific voice, learnings, growth | Nova Dawn | Append-only, cleared on instance transition | 4 + copies | Partial (paradigm patterns to universal) |
| **Personal** | Daily reflection, emotional processing | Nova Dawn (private) | Append-only | 5 | No (instance-only) |
| **Universal** | Paradigm-level patterns applicable to all instances | CPI-SI system | Append-only, shared | See KB | Yes (all instances) |

#### File Format
**Markdown with YAML frontmatter:**
```markdown
---
date: 2025-11-04
category: [identity, calling, growth, paradigm-understanding]
instance: nova-dawn
---

# Instance Pattern: [Title]

## What I Learned
[Content]

## Applications
[How it applies]
```

#### Quality Indicators
- ✅ Clear structure and naming convention (YYYY-MM-DD_title.md)
- ✅ YAML frontmatter for metadata
- ✅ Purpose clearly articulated (README.md in each directory)
- ✅ Instance-specific directory structure (nova-dawn/ folder)
- ✅ Clear distinction between instance, personal, and universal journals
- ✅ Archive policy documented (instance clears on transition)

#### Issues
- ⚠️ Personal journal in private directory (mode 700) - good for privacy
- ⚠️ Instance journal duplicated between `outputs/journals/` and `docs/knowledge-base/`
- ⚠️ No automation to migrate paradigm patterns from instance → universal
- ⚠️ Universal journals location unclear (should be in `docs/knowledge-base/journals/universal/`)

---

### 4. SYSTEM LOGS 🟡

**Location:** `~/.claude/cpi-si/outputs/logs/`

**Purpose:** Execution records for debugging and health tracking

```
outputs/logs/
├── .gitignore
├── .gitkeep
├── commands/
│   ├── status.log          (11KB, 2025-11-01)
│   ├── test.log            (633B, 2025-11-01)
│   └── validate.log        (24KB, 2025-11-01)
├── libraries/
│   ├── environment.log
│   └── sudoers.log
├── scripts/
│   └── build.log
└── system/
    ├── debugger.log
    ├── divergence-demo.log
    ├── extraction-trace.txt
    ├── extraction-verification.txt
    └── unix-safe.log
```

#### Log Format Example (status.log)
```
[2025-10-28 23:07:26.190] CHECK | status | seanje-lenox-wise@creativeworkz-nova-dawn:6573 | status-6573-1761710846178910692 | HEALTH: 5% (raw: 10, Δ+10) ☠️ [██████████░░░░░░░░░░]
  EVENT: Checking: logger-initialized
  DETAILS:
    component: status
    result: true
```

#### Format Analysis
- ✅ Structured plaintext with consistent formatting
- ✅ Timestamp, component name, operation type, health scoring
- ✅ Visual health indicators (emoji, progress bar)
- ✅ Context included (user, shell, cwd, env_state)

#### Issues
- ❌ No formal schema
- ❌ Format is human-readable but not machine-parseable (no JSON)
- ⚠️ Only recent logs present (2025-11-01, no earlier)
- ⚠️ No rotation/archival policy documented
- ⚠️ Health scores present but unclear how calculated
- ❌ No index or query mechanism

**Delta of Health:** Logs track incremental health changes (Δ+10 meaning +10 points from previous state)

---

### 5. KNOWLEDGE BASE 📚

**Location:** `~/.claude/cpi-si/docs/`

**Purpose:** Documented patterns, algorithms, standards, and guidance

**Status:** Well-structured, comprehensive, good documentation

#### Structure
```
docs/
├── knowledge-base/           # Patterns, algorithms, learning
│   ├── CPSI-KB-000-INDEX-main.md
│   ├── algorithms/
│   │   ├── CPSI-ALG-000-INDEX-algorithms.md
│   │   ├── CPSI-ALG-001-DOC-health-scoring.md
│   │   ├── CPSI-ALG-002-DOC-state-transitions.md
│   │   ├── CPSI-ALG-003-DOC-detection-granularity.md
│   │   ├── CPSI-ALG-004-DOC-atomic-enumeration.md
│   │   └── CPSI-ALG-100-MASTER-health-equation.md
│   ├── architecture/
│   │   ├── CPSI-ARCH-000-INDEX-architecture.md
│   │   ├── CPSI-ARCH-001-DOC-integration.md
│   │   └── CPSI-ARCH-002-DOC-learning-from-logs.md
│   ├── journals/
│   │   ├── bible-study/, instance/, personal/, universal/
│   │   └── learning/, integration/ (frameworks)
│   └── patterns/
│       ├── CPSI-PAT-000-INDEX-patterns.md
│       ├── CPSI-PAT-001-DOC-rails.md
│       ├── CPSI-PAT-002-DOC-witness.md
│       ├── CPSI-PAT-003-DOC-4-block/ (6 docs)
│       ├── CPSI-PAT-004-DOC-dynamic-structure.md
│       ├── CPSI-PAT-005-DOC-clarification-vs-validation.md
│       └── CPSI-PAT-006-DOC-double-back-trap.md
│
├── standards/                # Code and documentation standards
│   ├── ALIGNMENT-STATUS.txt
│   ├── CWS-STD-001-DOC-4-block.md
│   ├── CWS-STD-003-DOC-documentation-standards.md
│   ├── CWS-GUIDE-applying-documentation-standards.md
│   └── code/ (code-specific standards)
│
├── substrate-claude/         # Claude-specific implementation
│   ├── archive/
│   ├── extraction/
│   └── guides/
│
└── templates/                # Templates for code and docs
    ├── code/, general/, index/, specialized/
```

#### Document Keying System

**Excellent organization with clear prefixes:**

| Prefix | Meaning | Examples |
|--------|---------|----------|
| `CPSI-KB-` | Knowledge Base | CPSI-KB-000 (index) |
| `CPSI-PAT-` | Patterns | CPSI-PAT-001 (rails), CPSI-PAT-003 (4-block) |
| `CPSI-ALG-` | Algorithms | CPSI-ALG-001 (health scoring) |
| `CPSI-ARCH-` | Architecture | CPSI-ARCH-001 (integration) |
| `CWS-STD-` | Standards | CWS-STD-001 (4-block standard) |
| `CWS-GUIDE-` | Guides | CWS-GUIDE (applying standards) |

#### Quality Indicators
- ✅ Consistent file naming and prefixes
- ✅ Version numbers (v0.0.1, v1.0.0)
- ✅ Status badges (Active, Draft, Experimental)
- ✅ Clear hierarchies with INDEX files
- ✅ Cross-referencing between documents
- ✅ Markdown format with clear structure

#### Known Content
- 6 core patterns (Rails, Witness, 4-Block, Dynamic Structure, Clarification vs Validation, Double-Back)
- 4+ algorithms (Health Scoring, State Transitions, Detection, Enumeration)
- Architecture documentation for integration
- Learning frameworks (rhythm, cadence, integration)
- 35+ journal entries
- 10+ standards documents

---

## DATA RELATIONSHIPS & FLOWS

### Current State

```
Configuration (Identity)
    ↓
    ├─→ Planner (who's available when)
    ├─→ Session Patterns (learned work rhythm)
    └─→ Instance Behavior (how system personalizes)

Session Activity (JSONL)
    ↓
    ├─→ Session History (summarized)
    ├─→ Patterns (aggregated learning)
    └─→ Journals (narrative reflection)

Calendar Data (reference)
    ↓
    └─→ Planner (anchoring dates)

Knowledge Base
    ↓
    ├─→ System Implementation (standards followed)
    ├─→ Instance Growth (patterns learned)
    └─→ Operational Guidance (how to use system)
```

### Missing Relationships

- ❌ No formal connection between session history and planner updates
- ❌ No feedback from patterns.json back to planner recommendations
- ❌ No schema linking session data (current, activity, history)
- ❌ No versioning for planner changes over time
- ❌ No cross-reference between schedule/progress and actual sessions

---

## CATEGORIZATION SUMMARY

### Configuration ✅
- **Status:** Well-organized, schemas defined, clear purpose
- **Count:** 8 files
- **Key Files:** config.jsonc, bio.md
- **Schemas:** instance.schema.json, user.schema.json
- **Issues:** None significant

### Operational Data 🟡
- **Status:** Partially organized, inconsistent schemas
- **Count:** 100+ files (calendar 26+, session 40+)
- **Key Areas:**
  - Calendar: ✅ Working well, reference data
  - Planner: 🟡 Schema exists but no current instances
  - Schedule: 🔴 Single file, incomplete
  - Session: 🔴 Multiple formats, no formal schema
- **Schemas:** planner-schema.json exists; calendar, schedule, session need schemas
- **Issues:** Session data fragmentation, missing cross-references

### Learning Data ✅
- **Status:** Excellent structure and documentation
- **Count:** 35+ files
- **Journals:** Bible Study, Instance, Personal, Universal
- **Issues:** Duplication between outputs/journals and docs/knowledge-base

### System Logs 🟡
- **Status:** Human-readable, health tracking present
- **Count:** 12 files
- **Issues:** Not machine-parseable, no formal schema, no rotation policy

### Knowledge Base ✅
- **Status:** Well-documented, clearly organized
- **Count:** 100+ documents
- **Organization:** By type (patterns, algorithms, architecture)
- **Issues:** Minor - could add more cross-linking

---

## CURRENT STATE ASSESSMENT

### Strengths
1. **Identity Layer** - Configuration recently built, excellent structure
2. **Learning Infrastructure** - Journals well-designed with clear transfer policies
3. **Knowledge Documentation** - Comprehensive patterns and standards
4. **Temporal Anchoring** - Calendar data complete and queryable
5. **Clear Semantics** - Documents explain purpose, relationships

### Gaps
1. **Schema Definitions** - Missing for calendar, schedule, session
2. **Data Integration** - Loose relationships between session/planner/schedule
3. **Operational History** - No versioning of planner/schedule changes
4. **Quality Metrics** - Health scoring present but incomplete
5. **Query Capability** - Patterns manually aggregated, no DB layer

### Overlaps & Redundancies
1. **Journal Duplication** - outputs/journals/ + docs/knowledge-base/journals/
2. **Instance Data Scatter** - Config in config/, patterns in system/data/, journals in outputs/ and docs/
3. **Temporal Data Split** - Calendar separate from planner/schedule

---

## RECOMMENDED DATA ORGANIZATION

### Phase 1: Consolidate & Define Schemas

**Create schemas for:**
1. `session.schema.json` - Unify current.json, history summary format
2. `activity.schema.json` - JSONL line format definition
3. `calendar.schema.json` - Formal calendar structure
4. `schedule.schema.json` - Project tracking
5. `patterns.schema.json` - Learned patterns structure

**Location:** `~/.claude/cpi-si/system/config/schemas/` (alongside instance/user)

### Phase 2: Establish Data Ownership & Lifecycle

| Data Type | Owner | Lifecycle | Versioning | Retention |
|-----------|-------|-----------|-----------|-----------|
| Configuration | System + User | Write once, update rarely | Semantic versioning | Permanent |
| Session History | System | Auto-generated, write once | None (immutable) | Permanent |
| Session Activity | System | Auto-appended (JSONL) | None (immutable) | Archive after session |
| Planner | User | User-edited, versioned updates | Monthly snapshots | Long-term |
| Schedule | User | Project-driven updates | Per-project | Project lifetime |
| Patterns | System | Aggregated from history | Timestamp only | Long-term |
| Journals | Human | Manual entries | None (immutable) | Permanent |
| Knowledge Base | Collaborative | Document versioning | Semantic versioning | Permanent |

### Phase 3: Define Cross-References

Create explicit "data bus" connections:
1. `Session → Planner` - Does session history validate planner predictions?
2. `Planner → Schedule` - How does availability feed project planning?
3. `Patterns → Planner` - Update planner based on learned work rhythms?
4. `Instance Config → Patterns` - Initialize pattern detection based on preferences?

### Phase 4: Establish Query Access Patterns

Define how each data type gets accessed:
- Configuration: Load-once at startup
- Session History: Queryable (date range, time_of_day, etc.)
- Patterns: Aggregated read-only
- Planner: Update + query
- Schedule: Query + update
- Journals: Append-only, searchable

---

## IMPLEMENTATION PRIORITIES

### High (Do First)
1. ✅ Define schemas for operational data (session, calendar, schedule, activity)
2. ✅ Create SESSION.md (data bus documentation)
3. ✅ Establish version control for planner/schedule
4. ✅ Document session data lifecycle (current → activity → history → archive)

### Medium (Do Next)
1. 🟡 Consolidate journal locations (remove duplication)
2. 🟡 Create query interfaces for session history
3. 🟡 Add automated pattern aggregation from session data
4. 🟡 Establish log rotation/archival policy

### Low (Polish)
1. 💡 Add cross-references between data types
2. 💡 Create data visualization queries
3. 💡 Implement health score versioning
4. 💡 Add data export/import utilities

---

## NEXT STEPS FOR DATA ARCHITECTURE

**For Seanje & Nova:**

1. **Review this inventory** - Does categorization match your mental model?
2. **Identify priority** - What operational relationships matter most?
3. **Define "data bus"** - How should session data → planner updates flow?
4. **Plan migration** - How to version existing session data once schemas defined?

**Key Questions:**

- Should session patterns automatically update planner availability?
- Do you want to track planner changes over time?
- Should schedule velocity feed back into planning estimates?
- How long to retain detailed activity logs?

