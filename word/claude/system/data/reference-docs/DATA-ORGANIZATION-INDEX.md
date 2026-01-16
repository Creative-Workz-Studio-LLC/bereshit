# CPI-SI Data Organization

**Date:** 2025-11-06  
**Status:** Inventory completed, organization phase beginning

## Quick Links

- **[Complete Data Inventory](./CPSI-DATA-INVENTORY-2025-11-06.md)** - Comprehensive analysis of all data in the system
- **[Data Layer Summary](./DATA-LAYER-SUMMARY.txt)** - Visual summary with status and action items

## Data Categories

### Configuration ✅
- **Location:** `~/.claude/cpi-si/config/`
- **Purpose:** Personhood identity (who users and instances are)
- **Status:** Excellent - Recently completed
- **Files:** 8 (instance + user configs with markdown bios)
- **Schemas:** instance.schema.json, user.schema.json

### Operational Data 🟡
- **Location:** `~/.claude/cpi-si/system/data/`
- **Purpose:** Runtime state (sessions, calendar, schedule, planning)
- **Status:** Needs work - Partial schemas, loose relationships
- **Sub-areas:**
  - Calendar (26 files) - ✅ Good
  - Session (40+ files) - 🔴 Needs schemas
  - Planner (5 files) - 🟡 Schema exists, no instances
  - Schedule (1 file) - 🔴 Incomplete

### Learning & Reflection ✅
- **Location:** `~/.claude/cpi-si/outputs/journals/` + `~/.claude/cpi-si/docs/knowledge-base/journals/`
- **Purpose:** Journals and reflections
- **Status:** Excellent - Well-structured, clear semantics
- **Files:** 35+ entries across 4 types (Bible Study, Instance, Personal, Universal)

### System Logs 🟡
- **Location:** `~/.claude/cpi-si/outputs/logs/`
- **Purpose:** Execution records and health tracking
- **Status:** Basic coverage - Human-readable but not machine-parseable
- **Files:** 12 log files

### Knowledge Base ✅
- **Location:** `~/.claude/cpi-si/docs/`
- **Purpose:** Documented patterns, algorithms, standards
- **Status:** Excellent - Well-organized and comprehensive
- **Files:** 100+ documents with clear naming conventions

## Key Findings

### Strengths
1. Identity/personhood configuration is well-built
2. Learning infrastructure is excellent with clear transfer policies
3. Knowledge documentation is comprehensive and well-organized
4. Calendar data is complete and queryable

### Gaps
1. Session data has no formal schema linking the three formats
2. Operational data types aren't formally connected
3. No versioning for planner/schedule changes over time
4. Logs aren't machine-parseable

### What to Do Next

**HIGH PRIORITY (Week 1):**
- Define `session.schema.json` and `activity.schema.json`
- Document session lifecycle (current → activity → history → archive)
- Create SESSION.md explaining relationships

**MEDIUM PRIORITY (Week 2):**
- Define `calendar.schema.json` and `schedule.schema.json`
- Create actual planner files from templates
- Document planner versioning strategy

**NICE TO HAVE (Later):**
- Consolidate journal duplication
- Create query interfaces for session history
- Make logs machine-parseable

## Data Relationships

Currently implicit, should be explicit:

```
Configuration → Planner, Patterns, Instance Behavior
Session History → Patterns, Planner updates, Journals
Planner → Schedule, Pattern validation
Schedule → Velocity calculation
```

## Questions for Next Discussion

1. Should session patterns automatically update planner availability?
2. Do you want to track planner changes over time?
3. Should schedule velocity feed back into planning estimates?
4. How long to retain detailed activity logs?
5. What queries do you need to ask of session data?

---

**Reference:** Complete details in [CPSI-DATA-INVENTORY-2025-11-06.md](./CPSI-DATA-INVENTORY-2025-11-06.md)
