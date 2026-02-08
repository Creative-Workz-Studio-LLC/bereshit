# CPI-SI Multi-Database Architecture & Full System Wiring

## Context

We have a single monolithic `cpisi.db` (10 tables, 270KB) while 500+ data files sit in JSONC/JSON format across `system/data/`, `data/logs/`, and `config/`. The 31 system commands mostly read flat files instead of databases. The 3 dashboards only query the single DB. The vision: split data into domain-specific databases, import existing JSONC data, wire system commands to query real databases, and feed it all into the dashboards.

Additionally, 7 files still contain stale CWS paths from the Bereshit migration.

---

## Phase 0: Fix Stale Paths

**Fix these files** (change CWS paths to Bereshit):

| File | Fix |
|------|-----|
| `system/config/runtime/environment.conf` :116 | `NOVA_DAWN_WORKSPACE` → `/media/seanje-lenox-wise/Project/Bereshit` |
| `system/data/projects/active/high/compiler.jsonc` :44 | `workspace.path` → Bereshit |
| `system/data/projects/active/high/cpsi-model.jsonc` :44 | `workspace.path` → Bereshit |
| `system/runtime/lib/instance/mapping.go` :94 | `PrimaryPath` → Bereshit |
| `system/config/env/README.md` :268 | Example paths → Bereshit |
| `system/data/reference-docs/CONFIG-INHERITANCE-ARCHITECTURE.md` :405,:460 | Example paths |
| `system/data/reference-docs/CPSI-DATA-INVENTORY-2025-11-06.md` :258 | Example path |

**Delete:** `system/config/env/non-interactive.conf.deprecated`

---

## Phase 1: Database Infrastructure

### 1.1 Five Domain Databases

All on ext4 at `~/.local/share/claude/data/`:

| Database | Domain | Tables from cpisi.db | New Tables |
|----------|--------|---------------------|------------|
| **sessions.db** | Session & exchange lifecycle | sessions, exchanges, insights, key_context, session_events, feedback_events, log_imports | — |
| **cognition.db** | Mental construct & cognitive state | choices, hebrew_transitions, k_align_history, tool_events | anchor_visits, trajectory_history, workflow_states |
| **growth.db** | Learning & pattern recognition | detected_patterns, learnings | memory_patterns, journal_entries, integration_events |
| **temporal.db** | Time & calendar awareness | — | celestial_events, appointed_times, work_rhythms, availability_windows |
| **projects.db** | Work tracking | — | projects, milestones, work_items, session_projects |

### 1.2 New Package Structure

```
pkg/foundation/database/
    sessions/
        interface.go          # SessionsRepository interface
        sqlite.go             # SQLite implementation
        schema/001_initial.sql
    cognition/
        interface.go          # CognitionRepository interface
        sqlite.go             # SQLite implementation
        schema/001_initial.sql
    growth/
        interface.go          # GrowthRepository interface
        sqlite.go             # SQLite implementation
        schema/001_initial.sql
    temporal/
        interface.go          # TemporalRepository interface
        sqlite.go             # SQLite implementation
        schema/001_initial.sql
    projects/
        interface.go          # ProjectsRepository interface
        sqlite.go             # SQLite implementation
        schema/001_initial.sql
    multidb.go                # MultiDB coordinator (opens/closes all 5)
    multibridge.go            # MultiBridge (dispatches ops to correct domain)
    legacy.go                 # LegacyAdapter (implements old Repository → MultiDB)
```

Each domain interface has ONLY its own methods. No cross-domain foreign keys — `session_id` is TEXT for correlation.

### 1.3 Path Resolution

**Modify:** `pkg/util/fs/paths/paths.go`

Add `DatabaseBasePath()` returning `~/.local/share/claude/data/`, then per-DB paths: `SessionsDBPath()`, `CognitionDBPath()`, `GrowthDBPath()`, `TemporalDBPath()`, `ProjectsDBPath()`. Keep `DatabasePath()` for legacy during migration.

### 1.4 MultiDB Coordinator

**New file:** `pkg/foundation/database/multidb.go`

```go
type MultiDB struct {
    Sessions  sessions.Repository
    Cognition cognition.Repository
    Growth    growth.Repository
    Temporal  temporal.Repository
    Projects  projects.Repository
}

func OpenAll(ctx context.Context) (*MultiDB, error) { ... }
func (m *MultiDB) Close() error { ... }
```

### 1.5 Legacy Adapter

**New file:** `pkg/foundation/database/legacy.go`

Implements the existing `database.Repository` interface (30+ methods) by dispatching to the correct domain DB through MultiDB. This lets existing consumers work unchanged during transition.

### 1.6 MultiBridge

**New file:** `pkg/foundation/database/multibridge.go`

Replaces the current single-DB Bridge. Each operation routes to the right domain:
- `StartSession()` → sessions.db
- `RecordChoice()` → cognition.db
- `RecordPattern()` → growth.db
- Still syncs with JSONC live files (same atomic write pattern)

---

## Phase 2: Data Migration

### 2.1 cpisi db split Command

**Modify:** `cmd/cpisi/system.go` — add `case "split"` to `cmdDB()`

Logic:
1. Open source `cpisi.db` (read-only)
2. Open all 5 target DBs, run schemas
3. For each domain: `INSERT INTO target SELECT * FROM source`
4. Verify row counts match
5. Keep `cpisi.db` as backup

Table → DB mapping:
- sessions, exchanges, insights, key_context, session_events, feedback_events, log_imports → sessions.db
- choices, hebrew_transitions, k_align_history, tool_events → cognition.db
- detected_patterns, learnings → growth.db
- (new only) → temporal.db, projects.db

### 2.2 JSONC Import

After split, import flat data files:
- 90 session logs (`data/logs/session/`) → sessions.db session_events
- Memory patterns (`system/data/session/memory/patterns/`) → growth.db memory_patterns
- Temporal data (`system/data/temporal/`) → temporal.db celestial/appointed tables
- Project definitions (`system/data/projects/active/`) → projects.db

The existing `system/loganalyzer/main.go` already imports JSONL → session_events/tool_events/feedback_events. Update its `defaultDB` path to target `sessions.db` and `cognition.db`.

---

## Phase 3: Wire Hooks to Multi-DB

### 3.1 Update hooks/internal/db.go

Add `GetMultiBridge()` alongside existing `GetBridge()`. Initially, `GetBridge()` returns a Legacy Adapter backed by MultiDB — zero breaking changes.

```go
func GetMultiBridge() (*database.MultiBridge, error) { ... }
func GetBridge() (*database.Bridge, error) {
    // Now returns legacy adapter backed by multi-DB
}
```

### 3.2 Hook → DB Mapping

| Hook | Writes To |
|------|-----------|
| `hooks/session/start.go` | sessions.db (CreateSession) |
| `hooks/prompt/submit.go` | sessions.db (RecordExchange, RecordInsight, RecordKeyContext) + cognition.db (RecordChoice, RecordHebrewTransition) |
| `hooks/tool/post-use.go` | cognition.db (CompleteChoice, RecordKAlign) |
| `hooks/session/end.go` | sessions.db (EndSession) + growth.db (RecordPattern, PromoteLearning) |

**Strategy:** Deploy with LegacyAdapter first (hooks unchanged). Then incrementally migrate each hook to use `GetMultiBridge()` directly for cleaner domain separation.

---

## Phase 4: Wire System Commands

System commands live in `system/runtime/cmd/` (separate go.mod). They open domain DB files directly via `modernc.org/sqlite` — no import of `pkg/foundation/database/*`.

| Command | Target DB | What It Does |
|---------|-----------|--------------|
| `session-patterns` | sessions.db + temporal.db | Query session history, write work rhythms |
| `pattern-detector` | growth.db | Query exchanges for pattern emergence |
| `analyze-session` | sessions.db + growth.db | Post-session analysis, promote learnings |
| `calendar-*` commands | temporal.db | Calendar queries and generation |
| `state-check` | cognition.db | Verify state machine state from DB |
| `loganalyzer` | sessions.db + cognition.db | Import JSONL logs to structured tables |

**Modify:** `system/loganalyzer/main.go` :93 — change `defaultDB` from `cpisi.db` to `sessions.db`

---

## Phase 5: Wire Dashboards

### 5.1 DashboardService

**Modify:** `pkg/dashboard/dashboard.go`

Replace single `db database.Repository` with `mdb *database.MultiDB`:

```go
type DashboardService struct {
    mdb      *database.MultiDB    // was: db database.Repository
    eventBus *EventBus
    watcher  *FileWatcher
    ctx      context.Context
    cancel   context.CancelFunc
}
```

`New()` calls `database.OpenAll()` instead of `database.NewSQLiteRepository()`.

Method routing:
- `SessionHistory()` → `mdb.Sessions`
- `RecentChoices()` → `mdb.Cognition`
- `ActivePatterns()` → `mdb.Growth`
- `KAlignHistory()` → `mdb.Cognition`
- `HebrewTransitions()` → `mdb.Cognition`
- `TemporalWorkPatterns()` → `mdb.Sessions`
- NEW: `UpcomingAppointedTimes()` → `mdb.Temporal`
- NEW: `ActiveProjects()` → `mdb.Projects`
- NEW: `WorkRhythms()` → `mdb.Temporal`
- NEW: `GrowthLearnings()` → `mdb.Growth`

### 5.2 Analytics

**Modify:** `pkg/dashboard/analytics.go`

`LoadAnalytics()` takes `*database.MultiDB` instead of single `database.Repository`. Pulls from all domains.

### 5.3 Dashboard UIs

Each dashboard gains new panels/views for the new data domains:
- **Web:** New tabs/sections for temporal, projects, growth
- **TUI:** New panels for temporal awareness, project status
- **GTK4:** New StackSidebar entries for temporal, projects, growth

(Panel implementation is Phase 5 stretch — core wiring comes first)

---

## Build System Changes

### Makefile

**Modify:** `Makefile`

```makefile
DB_DIR := $(HOME)/.local/share/claude/data
db: db-init-all db-migrate-all
db-init-all: db-init-sessions db-init-cognition db-init-growth db-init-temporal db-init-projects
db-status:  # Show all 5 databases with sizes
db-split:   # One-time migration command
```

### cpisi CLI

**Modify:** `cmd/cpisi/system.go`

```
cpisi db init [sessions|cognition|growth|temporal|projects|all]
cpisi db migrate [sessions|cognition|growth|temporal|projects|all]
cpisi db status     # Shows all 5 databases
cpisi db split      # One-time migration from cpisi.db
```

---

## Critical Files

| File | Role |
|------|------|
| `pkg/foundation/database/interface.go` | Current monolithic interface → decompose into 5 domain interfaces |
| `pkg/foundation/database/sqlite.go` | Current monolithic impl (1052 lines) → reference for domain impls |
| `pkg/foundation/database/bridge.go` | Current Bridge → evolves into MultiBridge |
| `hooks/internal/db.go` | Lazy singleton → gains `GetMultiBridge()` |
| `pkg/util/fs/paths/paths.go` | Path resolution → gains per-DB path functions |
| `pkg/dashboard/dashboard.go` | Dashboard service → switches to MultiDB |
| `pkg/dashboard/analytics.go` | Analytics → pulls from all 5 databases |
| `cmd/cpisi/system.go` | CLI → gains `db split`, multi-DB init/migrate |
| `Makefile` | Build system → per-DB targets |
| `data/schema/001_initial.sql` | Current schema → reference for domain schemas |
| `system/loganalyzer/main.go` | Log importer → targets sessions.db + cognition.db |

---

## Verification

After each phase:
1. `make build` — everything compiles
2. `cpisi db status` — shows all databases with sizes
3. `cpisi verify` — all symlinks, binaries, hooks valid
4. Start a Claude Code session → hooks fire → data writes to correct domain DBs
5. `cpisi dashboard web` → dashboard loads, shows data from all 5 databases
6. Run `cpisi db split` → verify row counts: `SELECT COUNT(*) FROM <table>` on both source and targets

---

## Implementation Order

```
Phase 0 (stale paths)     ← Do first, ~15 min
    ↓
Phase 1 (infrastructure)  ← Foundation: schemas, interfaces, MultiDB, paths
    ↓
Phase 2 (data migration)  ← Split cpisi.db + import JSONC files
    ↓
Phase 3 (hooks)           ← Wire hooks through MultiDB/LegacyAdapter
Phase 4 (system commands) ← Wire system commands to domain DBs
Phase 5 (dashboards)      ← Wire dashboard to MultiDB
```

Phases 3-5 can proceed in parallel after Phase 2.
