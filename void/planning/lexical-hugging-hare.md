# L2 Production Pass + Database Foundation (L0 Down)

> *"Except the LORD build the house, they labour in vain that build it."* — Psalm 127:1

## Context

L0 and L1 are production-grade — quality gates passing, Makefiles orchestrating Go+Rust, structural lints clean. Now L2-platform needs the same treatment, AND we begin the deeper work: making the database explicit from L0 down.

The database isn't something we're adding — it's something we're NAMING. L0's config loader IS a read-only database (manifest, cache, graph, integrity checking). L1's pragma IS a universal record schema. L2's health specs ARE monitoring database schemas. L3's SQLite system IS the implementation. The entire stack is already a distributed database. We're making the implicit explicit.

## What Exists

### L2-platform (current)
- **9 TOML specs**: `filesystem/` (2), `os/health/` (6), `os/permission/` (1) — all 3-block aligned with `[_pragma]` and `[_metadata]`
- **11 Go files**: `server/config/` (4), `server/services/builder/` (1), `util/term/` (5), `util/vcs/git/` (1) — all 4-block structured
- **Placeholders**: `drivers/`, `fuse/`, `millenniumos/`, `substrates/millenniumos/` (`.gitkeep`)
- **No** `ladder/`/`hybrid/`/`spiral/` structure, no Makefile, no Rust, no `go.mod` (uses parent b-word module)
- **Import paths**: Old locations (`cws.studio/server/internal/...`, `creativeworkzstudio.com/bereshit/word/work/pkg/util/...`)

### L3 Database (already built)
- 5 domain databases: sessions, cognition, growth, temporal, projects
- 14 tables + 6 views in `schema/001_initial.sql`
- Interface + SQLite implementation pattern (5 `interface.go` + 5 `sqlite.go`)
- Bridge pattern (files ↔ database), MultiDB coordinator
- Pure Go SQLite (`modernc.org/sqlite` — no CGO)

### L0 Type System (already specified)
- `types/composition/storage.toml` — memory alignment (storage layer types)
- `types/composition/relationships.toml` — subtype, compatible, coercion, equality
- `types/composition/references.toml` — reference types
- `types/composition/constraints.toml` — type constraints
- Config loader: `IndexManifest`, `ConfigFile`, `LoadResult`, cache, graph, discovery

## Plan

### Phase 1: L2 Structural Reorganization

**Move specs into `ladder/`, code into `hybrid/`**

```
L2-platform/
├── ladder/                    # Specs (data)
│   ├── filesystem/            # contract.toml, types.toml, DESIGN-*.md
│   └── os/                    # health/ (6 TOMLs), permission/ (1 TOML)
├── hybrid/                    # Code (Go — currently Go-only)
│   ├── server/                # config/ (4), services/builder/ (1)
│   └── util/                  # term/ (5), vcs/git/ (1)
├── spiral/                    # Future (Rust)
│   └── .gitkeep
├── drivers/                   # Keep (placeholder)
├── fuse/                      # Keep (placeholder)
├── millenniumos/              # Keep (placeholder)
├── substrates/                # Keep (moved from deleted top-level)
│   └── millenniumos/
└── root.adoc                  # Update paths
```

**File moves (git mv):**
1. `filesystem/` → `ladder/filesystem/`
2. `os/` → `ladder/os/`
3. `server/` → `hybrid/server/`
4. `util/` → `hybrid/util/`
5. Create `spiral/.gitkeep`

**Files:** `root.adoc` — update directory references

### Phase 2: L2 Go Code — Import Path Audit

L2 Go files reference OLD import paths that won't resolve in the b-word module:
- `cws.studio/server/internal/config` → needs remapping
- `creativeworkzstudio.com/bereshit/word/work/pkg/util/term/display` → needs remapping

**Action:** Update import paths to match b-word module structure:
- `creativeworkzstudio.com/bereshit/L2-platform/hybrid/server/config`
- `creativeworkzstudio.com/bereshit/L2-platform/hybrid/util/term/display`
- etc.

**Add `//omni:` directives** to all 11 Go files (currently missing).

### Phase 3: L2 Makefile + Quality Gate

**File:** `.a-new-structure/b-word/L2-platform/Makefile`

Pattern matches L0/L1 Makefiles:
- Go-only initially (no Rust crates yet)
- Targets: build, test, vet, lint, check, clean, help
- Cached sentinels: `.test-go`, `.vet`, `.lint`
- `cws-struct lint go` for 4-block alignment
- Quality gate banner matching L0/L1 style

**Scripture:** *"And the waters were gathered together unto one place, and the dry land appeared."* — Genesis 1:9 (L2 = platform appearing)

### Phase 4: Wire into Root Makefile

Update `.a-new-structure/root-files/Makefile`:
- Add L2 to `bword-check`, `bword-build`, `bword-test`, `bword-clean`
- Expand `GO_SRC_DIRS` to include L2 hybrid paths
- Update help target with L2 info

### Phase 5: Database Foundation Seeds (L0 Down)

This is the architectural beginning — not completing the database, but planting the seeds that make the implicit explicit.

**L0 — Recognize what's already there:**
- The config loader (Rust + Go) IS a read-only document database
- Add a `ladder/foundation/storage/` spec area (parallel to `types/`, `math/`, `bible/`)
- Specs to create:
  - `record.toml` — fundamental record types (maps to `ConfigFile`, `Pragma`, L3's `Session`)
  - `index.toml` (concept already exists as `foundation/index.toml` — the manifest IS an index)
  - `persistence.toml` — storage abstraction types (file, memory, sqlite, network)
  - `integrity.toml` — validation, constraint checking, tripwire patterns

**L2 — Position the platform database:**
- Create `ladder/database/` spec area for platform-level storage contracts
- `contract.toml` — platform storage interface (parallels `filesystem/contract.toml`)
- This is WHERE SQLite lives as a platform service (L3 currently owns it — that should migrate down)

**L3 — Remains the consumer (no changes this pass)**
- L3's `foundation/database/` becomes a CONSUMER of L2's platform database
- Migration happens later when L2's contract is solid

### Phase 6: Register L2 in Cargo Workspace (No Rust Yet)

No Rust crates to add, but verify `Cargo.toml` workspace doesn't need changes.
Update `b-word/go.mod` if any new Go dependencies are needed for L2.

## Critical Files

| File | Action |
|------|--------|
| `L2-platform/root.adoc` | Update directory references |
| `L2-platform/hybrid/server/config/*.go` | Fix import paths, add `//omni:` |
| `L2-platform/hybrid/util/**/*.go` | Fix import paths, add `//omni:` |
| `L2-platform/Makefile` | Create (matching L0/L1 pattern) |
| `root-files/Makefile` | Add L2 to bword-* targets |
| `L0-universal/ladder/foundation/storage/` | Create database foundation specs |
| `L2-platform/ladder/database/` | Create platform database contract specs |
| `L0-universal/ladder/foundation/index.toml` | Add storage system entries |

## Execution Order

```
Phase 1 (structural moves) → Phase 2 (import fixes) → Phase 3 (Makefile)
→ Phase 4 (root wire-up) → Phase 5 (database seeds)
```

Each phase independently verifiable. Quality gate after Phase 4.

## Verification

1. `git mv` — all moves tracked in git history
2. `go build ./L2-platform/...` — Go code compiles with new paths
3. `make -C L2-platform check` — quality gate passes
4. `make bword-check` — L0 + L1 + L2 all pass from root
5. `cws-struct lint go L2-platform/hybrid/` — 0 errors, 0 warnings
6. Database seed specs pass `cws-struct lint toml` validation
