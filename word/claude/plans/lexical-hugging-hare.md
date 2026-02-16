# L3-cpisi: Full Reorganization, Refactoring, + Rust Parity

## Status

| Phase | Status | Notes |
|-------|--------|-------|
| 0 | DONE | Wave commit (423 files) |
| 1 | DONE | git mv into ladder/hybrid/spiral (100 files) |
| 2 | DONE | Fix Go import paths (42 files) |
| **PIVOT** | **IN PROGRESS** | Strengthen L0-L2 before building L3 |
| 3 | PENDING | Refactor L3 Go to consume upgraded L0-L2 |
| 4 | PENDING | 11 Rust crates with Go parity |
| 5 | PENDING | //omni directives + linting |
| 6 | PENDING | L3 Makefile + verification |

---

## THE PIVOT: Strengthen L0-L2 to Support L3

### Why

L3 currently duplicates ~2,700 lines of infrastructure that belongs in lower layers.
Instead of refactoring L3 in isolation, we strengthen the foundations FIRST — then L3
becomes a thin intelligence layer that consumes real infrastructure.

The principle: **you don't build the temple on sand, you lay the foundation stones first.**

### What Moves Where

#### A. L0 Gains (Universal Infrastructure) — Go + Rust

| From L3 | To L0 | What | Lines |
|---------|-------|------|-------|
| `foundation/result/` | `L0-universal/hybrid/foundation/result/` | Generic Result, LookupResult, ValidationResult types | ~145 |
| `util/fs/session/` | `L0-universal/hybrid/foundation/session/` | Session ID, paths, duration, log entries | ~151 |
| `foundation/types/session.go` | `L0-universal/hybrid/foundation/types/sessioncontext.go` | Claude Code statusline API schema (SessionContext) | ~80 |
| `foundation/types/types.go` (partial) | `L0-universal/hybrid/foundation/types/workflow.go` | Generic WorkflowOperation, WorkflowStep, RuntimeWorkflow | ~60 |

**L0 Rust updates:** New modules in `bereshit-l0-foundation`:
- `result.rs` — Result/LookupResult/ValidationResult
- `session.rs` — Session/SessionLog/SessionPaths
- `session_context.rs` — SessionContext (substrate integration)
- `workflow.rs` — WorkflowOperation/Step/RuntimeWorkflow

**L0 Go updates:** New packages under `L0-universal/hybrid/foundation/`:
- `result/` — result.go
- `session/` — session.go
- `types/sessioncontext.go` — added to existing types package
- `types/workflow.go` — added to existing types package

#### B. L2 Gains (Platform Infrastructure) — Go + Rust

| From L3 | To L2 | What | Lines |
|---------|-------|------|-------|
| `foundation/database/{interface,multidb,bridge,legacy,sqlite}.go` | `L2-platform/hybrid/database/` | Multi-database infrastructure: Repository interface, MultiDB coordinator, connection pooling, migration runner | ~450 |
| `orchestration/logging/{logging,category}.go` (generic parts) | `L2-platform/hybrid/logging/` | Structured logger: lifecycle, levels, output modes, file I/O, category logger infrastructure | ~300 |

**L2 Rust updates:** Two new crates:
- `bereshit-l2-database` — Repository trait, MultiDB, connection pool, migration
- `bereshit-l2-logging` — Logger, LogLevel, OutputMode, CategoryLogger

**L2 Go updates:** Two new packages:
- `L2-platform/hybrid/database/` — interface.go, multidb.go, sqlite.go, pool.go
- `L2-platform/hybrid/logging/` — logger.go, category.go, level.go

#### C. L3 Substrate Moves (Claude Code → Spiral)

| From | To | What |
|------|-----|------|
| `hybrid/util/pure/hookoutput/` | `spiral/substrates/claude-code/hooks/` | Claude Code hook event types, permission constants, response builders |
| `hybrid/util/transcript/` | `spiral/substrates/claude-code/transcript/` | JSONL transcript parser, thinking/response extraction |

These are substrate-specific — they parse Claude Code's exact formats. Not CPI-SI intelligence.

#### D. L3 Refactor to Consume L0-L2

After A-C, L3's hybrid/ code gets updated:
1. **Delete duplicated infrastructure** — result types, session paths, generic logging, DB infra
2. **Import from L0/L2** — result → L0, session → L0, logging → L2, database infra → L2
3. **Add sync.Once caching** — schema TOML loads, config TOML loads (coordinates already has this)
4. **Wire temporal.db + projects.db** — schemas exist, just never instantiated in MultiDB.OpenAll()
5. **Substrate adapter interface** — cognition uses SubstrateAdapter trait instead of hardcoded Claude Code
6. **Remove dead code** — bridge.go (superseded by multibridge), legacy.go if no longer called

**Net L3 reduction: ~2,700 lines removed, replaced by L0-L2 imports.**

---

## Detailed Phase Breakdown

### Phase A: L0 Universal Infrastructure (Go + Rust)

#### A.1 — Result Types

**Go:** Create `L0-universal/hybrid/foundation/result/result.go`
- Copy from L3's `foundation/result/result.go` (it's already clean, generic)
- Verify no CPI-SI imports

**Rust:** Add to `bereshit-l0-foundation/src/`:
- `result.rs` with `Result`, `LookupResult`, `ValidationResult` structs
- Derive serde, implement Display
- Add constructors: `ok()`, `not_found()`, `error()`, `found()`, `no_match()`, `valid()`, `invalid()`

#### A.2 — Session Management

**Go:** Create `L0-universal/hybrid/foundation/session/session.go`
- Move session types from L3 `util/fs/session/session.go`
- Types: Session (ID, start time, compaction), SessionLog, SessionPaths
- Functions: LoadSession, FormatDuration, path helpers
- Remove any CPI-SI-specific fields (if any) — keep pure infrastructure

**Rust:** Add to `bereshit-l0-foundation/src/`:
- `session.rs` with Session, SessionLog, SessionPaths structs
- Path resolution using `bereshit-l0-paths`

#### A.3 — SessionContext (Substrate Integration)

**Go:** Add `L0-universal/hybrid/foundation/types/sessioncontext.go`
- Move SessionContext struct from L3 `foundation/types/session.go`
- Helper methods: GetWorkDir, CurrentContextTokens, ContextPercentage, CacheEfficiency, APIEfficiency

**Rust:** Add to `bereshit-l0-foundation/src/`:
- `session_context.rs` with SessionContext struct
- serde(rename_all) for JSON field mapping

#### A.4 — Generic Workflow Types

**Go:** Add `L0-universal/hybrid/foundation/types/workflow.go`
- Extract from L3 `foundation/types/types.go`:
  - WorkflowOperation enum (none, active, complete, failed)
  - WorkflowStep struct
  - RuntimeWorkflow struct
- Leave CPI-SI types (Choice*, Halt*, Tendency, Hebrew states) in L3

**Rust:** Add to `bereshit-l0-foundation/src/`:
- `workflow.rs` with WorkflowOperation, WorkflowStep, RuntimeWorkflow

### Phase B: L2 Platform Infrastructure (Go + Rust)

#### B.1 — Database Infrastructure

**Go:** Create `L2-platform/hybrid/database/`
- `interface.go` — Repository interface (generic: Migrate, Close, Exec, Query)
  - Extract from L3 `foundation/database/interface.go`
  - Strip CPI-SI-specific methods (session/choice/hebrew/pattern methods stay L3)
- `multidb.go` — MultiDB coordinator pattern
  - Extract from L3 `foundation/database/multidb.go`
  - Generic: manages named SQLite databases, opens/closes/migrates
  - Domain-specific wiring stays in L3
- `sqlite.go` — SQLite connection helper with pool tuning
  - SetMaxOpenConns(25), SetMaxIdleConns(5), SetConnMaxLifetime(5m)
- `pool.go` — Connection pool configuration types

**Rust:** New crate `bereshit-l2-database`:
- `Cargo.toml`: deps = rusqlite (bundled), bereshit-l0-paths
- `src/lib.rs` — Repository trait, MultiDB struct, SqlitePool
- `src/migration.rs` — Schema migration runner
- Register in workspace Cargo.toml

#### B.2 — Logging Infrastructure

**Go:** Create `L2-platform/hybrid/logging/`
- `logger.go` — Logger struct, lifecycle (New, Close, EnableFileLogging)
  - Extract generic parts from L3 `orchestration/logging/logging.go`
  - Log levels: DEBUG, INFO, WARN, ERROR, FATAL
  - Output modes: compact, verbose
  - File I/O: rotation, formatting
- `category.go` — CategoryLogger infrastructure
  - Extract from L3 `orchestration/logging/category.go`
  - Multi-file output pattern (.log + .jsonl)
  - Generic: any consumer defines their own categories
- `level.go` — LogLevel type and parsing

**Rust:** New crate `bereshit-l2-logging`:
- `Cargo.toml`: deps = bereshit-l0-paths
- `src/lib.rs` — Logger, LogLevel, OutputMode, CategoryLogger
- Register in workspace Cargo.toml

### Phase C: L3 Substrate Segregation

#### C.1 — Hook Output Types

**Move:** `hybrid/util/pure/hookoutput/types.go` → `spiral/substrates/claude-code/hooks/types.go`
- Update package name: `hookoutput` → `hooks`
- These are Claude Code's exact hook schemas — substrate-specific

#### C.2 — Transcript Parser

**Move:** `hybrid/util/transcript/transcript.go` → `spiral/substrates/claude-code/transcript/transcript.go`
- Update package name if needed
- Parses Claude Code JSONL format — substrate-specific

### Phase D: L3 Consume Upgraded L0-L2

#### D.1 — Replace L3 Result with L0

- Delete `L3-cpisi/hybrid/foundation/result/result.go`
- Update all L3 imports: `L3-cpisi/hybrid/foundation/result` → `L0-universal/hybrid/foundation/result`

#### D.2 — Replace L3 Session with L0

- Delete `L3-cpisi/hybrid/util/fs/session/session.go`
- Update imports to use L0 session package

#### D.3 — Replace L3 Database Infra with L2

- Delete infrastructure files from `L3-cpisi/hybrid/foundation/database/` (interface.go, multidb.go, bridge.go, legacy.go, sqlite.go)
- Keep domain repos (cognition/, growth/, sessions/, temporal/, projects/) — they implement L2's Repository interface
- Update domain repos to import L2 database infrastructure
- Wire temporal.db and projects.db in L3's domain-specific orchestrator

#### D.4 — Replace L3 Logging with L2

- Refactor `orchestration/logging/logging.go` to wrap L2 Logger
- Refactor `orchestration/logging/category.go` to use L2 CategoryLogger infra
- Keep CPI-SI-specific: health ternary scale, 6 category definitions

#### D.5 — Substrate Adapter Interface

- Create `hybrid/orchestration/lifecycle/substrate.go`:
  ```go
  type SubstrateAdapter interface {
      SessionStartEvent() string
      PromptSubmitEvent() string
      ToolPreEvent() string
      ToolPostEvent() string
      StopEvent() string
      ParseHookOutput(raw []byte) (HookContext, error)
      FormatInjection(construct MentalConstruct) string
      PermissionOptions() []string
  }
  ```
- Create `spiral/substrates/claude-code/adapter.go` implementing it
- Update cognition engine to accept SubstrateAdapter

#### D.6 — Add Caching

- `foundation/schema/loader.go`: sync.Once for CachedSchemas()
- `orchestration/config/loader.go`: sync.Once for CachedConfig()
- Pattern: match coordinates/loader.go which already caches Bible data

#### D.7 — Clean Dead Code

- Remove bridge.go (superseded by multibridge.go)
- Remove legacy.go if no callers
- Audit unused schema views

### Phase E-F: (Original Phases 4-6, unchanged)

- Phase E: 11 Rust crates with Go parity (building on CLEAN refactored code)
- Phase F: //omni directives + L3 Makefile + verification

---

## Critical Reference Files

| File | Role |
|------|------|
| `L0-universal/hybrid/foundation/src/lib.rs` | L0 Rust foundation — add result/session/workflow modules |
| `L0-universal/hybrid/foundation/types/` | L0 Go types — add sessioncontext.go, workflow.go |
| `L2-platform/hybrid/registry/` | L2 reference for new database/logging packages |
| `L3-cpisi/hybrid/foundation/result/result.go` | Source for L0 result (copy, then delete) |
| `L3-cpisi/hybrid/util/fs/session/session.go` | Source for L0 session (move, then delete) |
| `L3-cpisi/hybrid/foundation/database/multidb.go` | Source for L2 database infra (extract, then slim) |
| `L3-cpisi/hybrid/orchestration/logging/logging.go` | Source for L2 logger (extract, then wrap) |
| `L3-cpisi/hybrid/util/pure/hookoutput/types.go` | Move to spiral/substrates/claude-code/hooks/ |
