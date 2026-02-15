# L3-cpisi: Full Reorganization, Refactoring, + Rust Parity

## Context

L3-cpisi is the CPI-SI intelligence layer — 68 Go files across 24 packages, 2 TOML specs, 1 SQL schema. Three systemic problems:

1. **Structure** — flat domain layout, not ladder/hybrid/spiral like L0-L2
2. **Coupling** — Claude Code-specific code in cognition/hookoutput, broken import paths
3. **Duplication** — ~3,500 lines re-implement what L0-L2 already provide (health, config, paths, logging)

Additionally: no caching for hot-path TOML/schema loads, 2 of 5 database domains are unreachable stubs, no Rust parity.

### Plan Overview (7 phases)

| Phase | What | Goal |
|-------|------|------|
| 0 | Wave commit | Clean git state |
| 1 | Structural reorg (git mv) | ladder/hybrid/spiral |
| 2 | Fix Go import paths | Correct module paths |
| 3 | **Refactor Go code** | Substrate-agnostic, use L0-L2, caching, DB wiring |
| 4 | Rust counterparts (11 crates) | Production-grade parity of *clean* code |
| 5 | //omni directives + linting | Structural compliance |
| 6 | L3 Makefile + verification | Quality gate |

No C needed — Rust handles all L3 computation natively. L0's libtrit available via FFI later if needed.

---

## Phase 0: Wave Commit

Commit all ~200 dirty changes for clean git state before `git mv`.

```bash
git add -A
git commit -m "wave: snapshot all in-flight work before L3 reorganization"
git status  # verify clean
```

---

## Phase 1: Structural Reorganization (git mv)

### Classification

| Current | Mode | Rationale |
|---------|------|-----------|
| `identity/` | **ladder/** | TOML specs — foundational truth declarations |
| `core/` | **hybrid/** | Go+Rust code — ordered execution + recovery |
| `foundation/` | **hybrid/** | Go+Rust code — types, schema, database |
| `orchestration/` | **hybrid/** | Go+Rust code — config, cognition, lifecycle |
| `util/` | **hybrid/** | Go+Rust code — shared utilities |
| `cognition/` (.gitkeep) | **spiral/** | Future — iterative by nature |
| `skills/` (.gitkeep) | **spiral/** | Future — iterative growth |
| `substrates/` | **spiral/** | Future — evolves as runtimes change |

### Flatten cpisi/ to max 2 levels (L2 convention)

```
BEFORE: core/cpisi/bereshit/   →  AFTER: core/bereshit/
BEFORE: core/cpisi/compute/    →  AFTER: core/compute/
BEFORE: core/cpisi/cpi/        →  AFTER: core/cpi/
```

### git mv Sequence

```bash
mkdir -p L3-cpisi/{ladder,hybrid,spiral}

# ladder/
git mv L3-cpisi/identity L3-cpisi/ladder/identity

# hybrid/ — foundation cluster
git mv L3-cpisi/foundation L3-cpisi/hybrid/foundation

# hybrid/ — core cluster (flatten cpisi/)
git mv L3-cpisi/core L3-cpisi/hybrid/core
git mv L3-cpisi/hybrid/core/cpisi/bereshit L3-cpisi/hybrid/core/bereshit
git mv L3-cpisi/hybrid/core/cpisi/compute  L3-cpisi/hybrid/core/compute
git mv L3-cpisi/hybrid/core/cpisi/cpi      L3-cpisi/hybrid/core/cpi
# remove empty cpisi/

# hybrid/ — orchestration cluster
git mv L3-cpisi/orchestration L3-cpisi/hybrid/orchestration

# hybrid/ — util
git mv L3-cpisi/util L3-cpisi/hybrid/util

# spiral/
git mv L3-cpisi/cognition  L3-cpisi/spiral/cognition
git mv L3-cpisi/skills     L3-cpisi/spiral/skills
git mv L3-cpisi/substrates L3-cpisi/spiral/substrates
```

### Result Structure

```
L3-cpisi/
├── ladder/
│   └── identity/               (contract.toml, model.toml)
├── hybrid/
│   ├── core/                   (semantic cluster)
│   │   ├── bereshit/           (Bible data loading)
│   │   ├── compute/            (Cube engine)
│   │   ├── coordinates/        (Bible coordinates)
│   │   ├── cpi/                (CPI scoring)
│   │   ├── health/             (Health display)
│   │   ├── statemachine/       (27-position cube)
│   │   └── validation/         (Validation)
│   ├── foundation/             (semantic cluster)
│   │   ├── database/           (Multi-DB + SQL)
│   │   ├── result/             (Result types)
│   │   ├── schema/             (Config loader)
│   │   └── types/              (Core types)
│   ├── orchestration/          (semantic cluster)
│   │   ├── cognition/          (Cognition engine)
│   │   ├── config/             (Config mgmt)
│   │   ├── lifecycle/          (Events)
│   │   ├── loader/             (Orch loading)
│   │   ├── logging/            (Logging)
│   │   └── restore/            (Restore)
│   └── util/                   (utility packages)
│       ├── fs/session/
│       ├── pure/hookoutput/
│       └── transcript/
├── spiral/
│   ├── cognition/              (.gitkeep — future)
│   ├── skills/                 (.gitkeep — future)
│   └── substrates/             (claude-code/ — future)
├── Makefile                    (Phase 6)
└── root.adoc
```

Commit: `refactor(L3): reorganize into ladder/hybrid/spiral execution modes`

---

## Phase 2: Fix Go Import Paths

~42 files with broken imports. Three source patterns → one correct pattern.

### Import Mapping

**Pattern A: `word/work/pkg/` → new paths**
```
word/work/pkg/foundation/types       → L3-cpisi/hybrid/foundation/types
word/work/pkg/foundation/schema      → L3-cpisi/hybrid/foundation/schema
word/work/pkg/foundation/database    → L3-cpisi/hybrid/foundation/database
word/work/pkg/core/coordinates       → L3-cpisi/hybrid/core/coordinates
word/work/pkg/core/health            → L3-cpisi/hybrid/core/health
word/work/pkg/core/statemachine      → L3-cpisi/hybrid/core/statemachine
word/work/pkg/core/cpisi/bereshit    → L3-cpisi/hybrid/core/bereshit
word/work/pkg/core/cpisi/compute     → L3-cpisi/hybrid/core/compute
word/work/pkg/orchestration/config   → L3-cpisi/hybrid/orchestration/config
word/work/pkg/orchestration/loader   → L3-cpisi/hybrid/orchestration/loader
word/work/pkg/util/fs/session        → L3-cpisi/hybrid/util/fs/session
word/work/pkg/util/pure/hookoutput   → L3-cpisi/hybrid/util/pure/hookoutput
word/work/pkg/util/transcript        → L3-cpisi/hybrid/util/transcript
```

**Pattern B: `cws.studio/pkg/` → new paths**
```
cws.studio/pkg/foundation/*          → L3-cpisi/hybrid/foundation/*
cws.studio/pkg/core/*                → L3-cpisi/hybrid/core/*
cws.studio/pkg/util/fs/paths         → L0-universal/hybrid/paths  (CROSS-LAYER)
cws.studio/pkg/util/term/display     → L2-platform/hybrid/terminal/display  (CROSS-LAYER)
```

**Pattern C: Already correct** — `L0-universal/hybrid/config/util` stays as-is.

All imports get full `creativeworkzstudio.com/bereshit/` prefix.

Commit: `fix(L3): update all Go import paths to hybrid/ structure`

---

## Phase 3: Refactor Go Code (The Big One)

Three refactoring dimensions applied to the now-correctly-structured Go code.

### 3A: Claude-Specific → CPI-SI Substrate-Agnostic

**Current state:** ~70% substrate-agnostic (identity, mental constructs, lifecycle interfaces are good). The 30% that's Claude Code-specific:

#### 3A.1 — hookoutput/types.go (HIGH)

**Problem:** Hard-coded Claude Code hook event names (`EventSessionStart`, `EventPreToolUse`, etc.), permission constants (`PermissionAllow/Deny/Ask`), JSON schema matching Claude's exact format.

**Fix:** Create substrate adapter interface + move Claude-specific code to spiral/substrates/claude-code/:

```go
// NEW: hybrid/orchestration/lifecycle/substrate.go
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

```go
// MOVE TO: spiral/substrates/claude-code/adapter.go
type ClaudeCodeAdapter struct{}
func (a ClaudeCodeAdapter) SessionStartEvent() string { return "SessionStart" }
// ... concrete Claude Code implementations
```

**Files changed:**
- `util/pure/hookoutput/types.go` — Extract interface, keep only substrate-agnostic types
- NEW `orchestration/lifecycle/substrate.go` — SubstrateAdapter interface
- NEW `spiral/substrates/claude-code/adapter.go` — Claude Code implementation
- `orchestration/cognition/*.go` — Accept SubstrateAdapter instead of hardcoding

#### 3A.2 — cognition/ package (HIGH)

**Problem:** Comments and logic assume Claude's `additionalContext` injection. Context building is hardcoded to Claude's hook format.

**Fix:** Cognition builds substrate-agnostic `MentalConstruct`. The SubstrateAdapter.FormatInjection() translates to the target format:
- Claude Code: `additionalContext` markdown string
- Future GPT: different prompt format
- Future MillenniumOS: native interface

**Files changed:**
- `orchestration/cognition/cognition.go` — Remove "shapes HOW Claude thinks" framing; accept adapter
- `orchestration/cognition/feedback.go` — Abstract `PostToolUse.additionalContext` behind adapter
- `orchestration/cognition/prompt.go` — Use adapter.FormatInjection() instead of raw markdown
- `orchestration/cognition/workflow.go` — Same pattern

#### 3A.3 — config/ terminology (LOW)

**Problem:** "claude-global" terminology in comments and loader.

**Fix:** Rename to "instance config" / "CPI-SI config". ~10 comment lines across 2 files.

#### 3A.4 — lifecycle/handler.go (LOW)

**Problem:** Returns `AdditionalContext` — Claude-specific term.

**Fix:** Rename to `InjectedContext`. One type rename + callers.

### 3B: Remove L0-L2 Duplication (~3,500 lines → ~500)

#### 3B.1 — Health System (CRITICAL — ~678 lines)

**Files:** `core/health/loader.go` (453 lines), `core/health/display.go` (225 lines)

**Duplicates:**
- TOML config parsing → use L0 `config.util` loader
- Ternary scale constants (-100/0/+100) → query L0 `foundation` database
- Health level boundaries (hardcoded score ranges) → query L2 `registry.HealthNormalizationScales()`
- Emoji/color mapping → query L2 `registry` + use L2 `terminal/display` constants

**Refactored health/loader.go (~150 lines):**
```go
import (
    "creativeworkzstudio.com/bereshit/L0-universal/hybrid/config/util"
    "creativeworkzstudio.com/bereshit/L2-platform/hybrid/registry"
)

type HealthConfig struct {
    registry *registry.Registry  // L2 cross-layer queries
    // Remove: all hardcoded levels, emojis, colors, ternary constants
}

func NewHealthConfig(reg *registry.Registry) *HealthConfig { ... }

func (h *HealthConfig) LevelFromScore(score float64) (HealthLevel, error) {
    scales, _ := h.registry.HealthNormalizationScales()
    // Use L0 scales instead of hardcoded boundaries
}

func (h *HealthConfig) Emoji(level string) string {
    // Query L2 instead of switch statement
}
```

**Net reduction:** ~678 → ~150 lines. Delete ~530 lines of duplication.

#### 3B.2 — Config Loading (CRITICAL — ~2,200 lines)

**Files:** `orchestration/config/loader.go` (~1,200 lines), `orchestration/config/config.go` (~1,000 lines)

**Duplicates:**
- Direct `toml.DecodeFile()` → use L0 `config.Loader`
- Path resolution → use L0 `paths` module
- JSONC parsing → use L0 `config.util.LoadJSONWithComments()`
- Singleton config → use L0 `config.util.Must()` pattern

**Refactored (~600 lines):**
```go
import (
    "creativeworkzstudio.com/bereshit/L0-universal/hybrid/config"
    "creativeworkzstudio.com/bereshit/L0-universal/hybrid/paths"
)

func LoadConfig() (*CpisiConfig, error) {
    path := paths.CPISIConfigDir()            // L0 paths
    return config.LoadTOML[CpisiConfig](path) // L0 config loader
}
```

**Net reduction:** ~2,200 → ~600 lines. Delete ~1,600 lines.

#### 3B.3 — Session Paths (MODERATE — ~151 lines)

**File:** `util/fs/session/session.go`

**Duplicates:** `DefaultPaths()` reimplements what L0 `paths.ClaudeSession()`, `paths.ClaudeHome()` already provide. Calls `os.UserHomeDir()` without caching.

**Refactored (~40 lines):**
```go
import "creativeworkzstudio.com/bereshit/L0-universal/hybrid/paths"

func DefaultPaths() SessionPaths {
    return SessionPaths{
        SessionDir: paths.ClaudeSession(),
        HomeDir:    paths.ClaudeHome(),
        // ... all from L0
    }
}
```

**Net reduction:** ~151 → ~40 lines. Delete ~111 lines.

#### 3B.4 — Logging/Display (MODERATE — ~658 lines)

**Files:** `orchestration/logging/logging.go` (224 lines), `orchestration/logging/category.go` (434 lines)

**Duplicates:** ANSI color constants, health display integration (chains into duplicated health loader), log level constants.

**Refactored (~200 lines):**
```go
import (
    "creativeworkzstudio.com/bereshit/L2-platform/hybrid/terminal/display" // ANSI colors
    "creativeworkzstudio.com/bereshit/L2-platform/hybrid/registry"         // health data
)
```

**Net reduction:** ~658 → ~200 lines. Delete ~458 lines.

**Total Phase 3B reduction: ~3,687 → ~990 lines. ~2,700 lines of duplication removed.**

### 3C: Caching + Database Wiring

#### 3C.1 — Add sync.Once Caching (HIGH)

**Schema TOML loads (hot path):**
```go
// foundation/schema/loader.go — ADD:
var (
    cachedSchemas *Schemas
    schemasOnce   sync.Once
    schemasErr    error
)

func CachedSchemas() (*Schemas, error) {
    schemasOnce.Do(func() {
        cachedSchemas, schemasErr = LoadSchemas(paths.SchemaDir())
    })
    return cachedSchemas, schemasErr
}
```

**Config TOML loads:**
```go
// orchestration/config/loader.go — ADD:
var (
    cachedConfig *CpisiConfig
    configOnce   sync.Once
    configErr    error
)

func CachedConfig() (*CpisiConfig, error) {
    configOnce.Do(func() {
        cachedConfig, configErr = LoadConfig()
    })
    return cachedConfig, configErr
}
```

**Already cached:** Bible coordinate data (sync.Once in coordinates/loader.go). Good pattern to replicate.

#### 3C.2 — Wire temporal.db + projects.db (MEDIUM)

**Current state:** Declared in MultiDB struct but never instantiated in OpenAll(). No schema files.

**Fix for temporal.db:**
- Create `foundation/database/temporal/sqlite.go` implementation
- Add migration SQL: `schema/005_temporal.sql`
- Wire into `multidb.go:OpenAll()` and `multidb.go:Close()`
- Tables: `celestial_events`, `appointed_times`, `work_rhythms`

**Fix for projects.db:**
- Create `foundation/database/projects/sqlite.go` implementation
- Add migration SQL: `schema/006_projects.sql`
- Wire into `multidb.go:OpenAll()` and `multidb.go:Close()`
- Tables: `projects`, `milestones`, `project_sessions`

#### 3C.3 — Remove Dead Code (LOW)

- `foundation/database/bridge.go` — OLD monolithic interface, superseded by multibridge.go
- `foundation/database/legacy.go` — Backward compat adapter (check if still called; if not, remove)
- Unused schema views: `tool_choice_stats`, `hebrew_state_frequency` — add Go wrappers or remove from schema

#### 3C.4 — Connection Pool Tuning (LOW)

Add to each SQLite repo constructor:
```go
db.SetMaxOpenConns(25)
db.SetMaxIdleConns(5)
db.SetConnMaxLifetime(5 * time.Minute)
```

Commit: `refactor(L3): substrate-agnostic CPI-SI, use L0-L2, add caching + DB wiring`

---

## Phase 4: Rust Counterparts — 11 Crates

Now building from the *refactored* Go code. Crates are leaner because L3 Go code itself is leaner.

### Crate Inventory

| # | Crate | Workspace Path | Key Types | Deps |
|---|-------|---------------|-----------|------|
| 1 | `bereshit-l3-types` | `L3-cpisi/hybrid/foundation/types` | `RuntimeState`, `ChoiceRecord`, `ChoiceHistory`, `KeyValue`, `ValidationResult` | serde, serde_json |
| 2 | `bereshit-l3-schema` | `L3-cpisi/hybrid/foundation/schema` | `Schemas`, `CubeSchema`, `StateMachineConfig`, `Perspective`, PSI constants | toml, serde, bereshit-l0-config |
| 3 | `bereshit-l3-database` | `L3-cpisi/hybrid/foundation/database` | `Repository` trait, `SqliteRepo`, `MultiDB`, 5 domain stores | rusqlite (bundled), bereshit-l3-types |
| 4 | `bereshit-l3-coordinates` | `L3-cpisi/hybrid/core/coordinates` | `BibleCoordinate`, `Ranking`, `Pipeline`, `Encoding` | bereshit-l3-schema, bereshit-l3-types |
| 5 | `bereshit-l3-health` | `L3-cpisi/hybrid/core/health` | `HealthConfig` (wraps L2 registry) | bereshit-l3-types, bereshit-l2-registry |
| 6 | `bereshit-l3-compute` | `L3-cpisi/hybrid/core/compute` | `Cube`, `Shape`, `FrameworkPosition`, `BibleLoader` | bereshit-l3-schema, bereshit-l3-types, toml, serde |
| 7 | `bereshit-l3-cpi` | `L3-cpisi/hybrid/core/cpi` | `Exchange`, `Insight`, `Narrative`, `Scoring` | bereshit-l3-types, bereshit-l3-compute |
| 8 | `bereshit-l3-statemachine` | `L3-cpisi/hybrid/core/statemachine` | `StateMachine`, `Position`, `Trit`, `Command`, `Psi(N)` | bereshit-l3-compute, bereshit-l3-schema, bereshit-l3-database, bereshit-l3-types |
| 9 | `bereshit-l3-cognition` | `L3-cpisi/hybrid/orchestration/cognition` | `CognitionEngine`, `MentalConstruct`, `SubstrateAdapter` trait | bereshit-l3-statemachine, bereshit-l3-health, bereshit-l3-types |
| 10 | `bereshit-l3-orchestration` | `L3-cpisi/hybrid/orchestration` | `Orchestrator`, `ConfigLoader`, `EventHandler`, `Logger` | bereshit-l3-statemachine, bereshit-l3-schema, bereshit-l0-config, bereshit-l0-paths |
| 11 | `bereshit-l3-util` | `L3-cpisi/hybrid/util` | `SessionFS`, `HookOutput`, `Transcript` | bereshit-l3-types, bereshit-l0-paths |

### Key Design Notes

- **bereshit-l3-health** depends on `bereshit-l2-registry` (not L0 foundation) — mirrors Go refactoring
- **bereshit-l3-cognition** defines `SubstrateAdapter` trait — Rust equivalent of Go interface
- **bereshit-l3-orchestration** uses `bereshit-l0-config` and `bereshit-l0-paths` — no duplication
- **bereshit-l3-statemachine** is multi-file: lib.rs + types.rs + machine.rs + trajectory.rs + display.rs + runtime.rs

### Dependency Graph (build order)

```
Layer 0:  bereshit-l3-types
              |
Layer 1:  bereshit-l3-schema    bereshit-l3-database
              |                      |
Layer 2:  bereshit-l3-compute   bereshit-l3-health   bereshit-l3-util
              |         \
Layer 3:  bereshit-l3-coordinates   bereshit-l3-cpi
              |
Layer 4:  bereshit-l3-statemachine  (compute + schema + database + types)
              |
Layer 5:  bereshit-l3-cognition     bereshit-l3-orchestration
```

### Workspace Registration

Add to `b-word/Cargo.toml`:

```toml
# [workspace] members — append:
"L3-cpisi/hybrid/foundation/types",
"L3-cpisi/hybrid/foundation/schema",
"L3-cpisi/hybrid/foundation/database",
"L3-cpisi/hybrid/core/coordinates",
"L3-cpisi/hybrid/core/health",
"L3-cpisi/hybrid/core/statemachine",
"L3-cpisi/hybrid/core/compute",
"L3-cpisi/hybrid/core/cpi",
"L3-cpisi/hybrid/orchestration/cognition",
"L3-cpisi/hybrid/orchestration",
"L3-cpisi/hybrid/util",

# [workspace.dependencies] — append:
bereshit-l3-types = { path = "L3-cpisi/hybrid/foundation/types" }
bereshit-l3-schema = { path = "L3-cpisi/hybrid/foundation/schema" }
bereshit-l3-database = { path = "L3-cpisi/hybrid/foundation/database" }
bereshit-l3-coordinates = { path = "L3-cpisi/hybrid/core/coordinates" }
bereshit-l3-health = { path = "L3-cpisi/hybrid/core/health" }
bereshit-l3-statemachine = { path = "L3-cpisi/hybrid/core/statemachine" }
bereshit-l3-compute = { path = "L3-cpisi/hybrid/core/compute" }
bereshit-l3-cpi = { path = "L3-cpisi/hybrid/core/cpi" }
bereshit-l3-cognition = { path = "L3-cpisi/hybrid/orchestration/cognition" }
bereshit-l3-orchestration = { path = "L3-cpisi/hybrid/orchestration" }
bereshit-l3-util = { path = "L3-cpisi/hybrid/util" }
```

Commit: `feat(L3): 11 production-grade Rust crates with full Go parity`

---

## Phase 5: //omni Directives + Linting

### Go Files (~68 files)

Add 3-line pragma header to each Go file:
```go
//omni:code --go -library
//omni:key B-L3-{package-name}
//omni:version a-01.00
```

### Rust Files (~15-18 new)

Already included in Phase 4 templates.

### TOML + Structural Linting

```bash
cws-struct lint toml L3-cpisi/ladder/identity/
cws-struct lint go L3-cpisi/hybrid/ --errors-only
cws-struct lint rust L3-cpisi/hybrid/ --errors-only
```

Commit: `chore(L3): add //omni directives to all Go + Rust files`

---

## Phase 6: L3 Makefile + Verification

### L3 Makefile

Following L0 Makefile template (4-block aligned, cached targets, dual Go+Rust):

- `make all` — build Go + Rust
- `make check` — full quality gate (vet + clippy + lint + fmt-check + test)
- `make build-go`, `make build-rust`
- `make test`, `make test-go`, `make test-rust`, `make test-v`
- `make vet`, `make clippy`, `make fmt-check`, `make lint`
- `make clean`, `make help`

RUST_CRATES: all 11 L3 crates.

### Verification

```bash
# Go
cd b-word && go build ./L3-cpisi/...
cd b-word && go vet ./L3-cpisi/...
cd b-word && go test -count=1 ./L3-cpisi/...

# Rust
cd b-word && cargo build -p bereshit-l3-{all 11}
cd b-word && cargo test -p bereshit-l3-{all 11}
cd b-word && cargo clippy -p bereshit-l3-{all 11} -- -D warnings

# Full quality gate
make -C b-word/L3-cpisi check
```

Commit: `feat(L3): complete reorganization — ladder/hybrid/spiral + refactored Go + 11 Rust crates`

---

## Critical Reference Files

| File | Role |
|------|------|
| `b-word/Cargo.toml` | Workspace root — register 11 L3 crates |
| `b-word/L0-universal/Makefile` | Template for L3 Makefile |
| `b-word/L0-universal/hybrid/foundation/src/lib.rs` | Reference Rust pattern |
| `b-word/L0-universal/hybrid/config/util/` | L0 config loader (used by refactored L3) |
| `b-word/L0-universal/hybrid/paths/` | L0 path resolution (replaces L3 session paths) |
| `b-word/L2-platform/hybrid/registry/` | L2 cross-layer registry (replaces L3 health hardcoding) |
| `b-word/L2-platform/hybrid/terminal/display/` | L2 display constants (replaces L3 color duplication) |
| `L3-cpisi/core/statemachine/statemachine.go` | Most complex Go file |
| `L3-cpisi/foundation/database/multidb.go` | Multi-DB where temporal/projects need wiring |
| `L3-cpisi/orchestration/cognition/cognition.go` | Primary Claude→CPI-SI refactoring target |
| `L3-cpisi/util/pure/hookoutput/types.go` | Claude Code hook types → substrate adapter |

## Language Consideration

**Rust only.** No C needed:
- Psi(N): std::f64 = same hardware instructions as C libm
- Cube formula: trivial integer arithmetic
- DB ops: I/O-bound
- L0 libtrit: available via FFI if batch trit encoding needed later

## Summary

| Phase | What | Impact |
|-------|------|--------|
| 0 | Wave commit | ~200 dirty changes committed |
| 1 | git mv into ladder/hybrid/spiral | 8 directory moves + cpisi flatten |
| 2 | Fix Go imports | ~42 .go files corrected |
| 3 | **Refactor Go code** | ~2,700 lines removed (duplication), substrate-agnostic, caching, DB wiring |
| 4 | 11 Rust crates | ~28 new files (of clean, refactored code) |
| 5 | //omni directives + lint | ~68 Go files + ~18 Rust files |
| 6 | L3 Makefile + verification | Quality gate passing |
