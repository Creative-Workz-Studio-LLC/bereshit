# Plan: L0-Universal Layer Processing

## Context

Phases 1-4 of the throughline consolidation are complete:
- Phase 1-3: Extracted L0 shared primitives, refactored loaders, filled gaps
- Phase 4: Placed 99 of 100 Go files across 5 layers from both forks

**Now:** Process every Go file in L0-universal — align to 4-block structure, add identity registration, wire imports, add production patterns. Nothing passes through untouched. This is the first layer in a bottom-up pass through the entire codebase.

**Design directive:** The architecture IS the CPI-SI paradigm. Identity registration, assurance, building blocks — actual design components, not metaphor. The system is larger now and needs production-grade patterns.

---

## L0 Inventory: 17 Go Files

| # | Package | File | Current State | Work Level |
|:-:|---------|------|---------------|:----------:|
| 1 | identity/ | registry.go | 4-block, no pragma maps | **Medium** |
| 2 | config/util/ | doc.go | 4-block, has pragma maps | Light |
| 3 | config/util/ | errors.go | 4-block | Light |
| 4 | config/util/ | exists.go | 4-block | Light |
| 5 | config/util/ | formats.go | 4-block | Light |
| 6 | config/util/ | jsonc.go | 4-block | Light |
| 7 | config/util/ | loader.go | 4-block | Light |
| 8 | config/util/ | must.go | 4-block | Light |
| 9 | config/util/ | root.go | 4-block | Light |
| 10 | config/util/ | tripwire.go | 4-block | Light |
| 11 | config/util/ | util_test.go | 4-block | Light |
| 12 | config/ | loader.go | 4-block, exemplary | Light |
| 13 | config/ | loader_test.go | 4-block | Light |
| 14 | util/fs/paths/ | paths.go | 3-block, hardcoded paths | **Heavy** |
| 15 | util/pure/cli/ | cli.go | 3-block, no identity | **Medium** |
| 16 | util/pure/format/ | format.go | 3-block, no identity | **Medium** |
| 17 | util/pure/output/ | output.go | 3-block, no identity | **Medium** |

---

## Processing Standard

Every Go file in L0 gets all of these. No exceptions.

### Structure (4-block)

```
// ============================================================================
// METADATA                    (═ 76 chars)
// ============================================================================

// ────────────────────────────────────────────────────────────────  (─ 64 chars)
// Imports / Pragma / Metadata subsections
// ────────────────────────────────────────────────────────────────

// ============================================================================
// SETUP
// ============================================================================

// Order: I(Imports already in METADATA) → T(Types) → TM(Type Methods)
//        → K(Constants) → V(Variables) → PS(Package State)

// ============================================================================
// BODY
// ============================================================================

// Order: Org(Chart) → H(Helpers) → C(Core Operations) → E(Error) → APIs

// ============================================================================
// CLOSING
// ============================================================================

// Minimum: Overview, Modification Policy, Related Components
// Back-reference to METADATA/BODY for the rest
```

### Identity (every file)

```go
//omni:code --go -library
//omni:key B-L0-<package>-<file>
//omni:version b-01.00

var _pragma = map[string]string{
    "P1.key": "B-L0-<package>-<file>",
    "P1.type": "code", "P1.format": "go", "P1.style": "library",
    "P2.blocks": "4-block",
    "P3.from": "b-word/seed/code/L0/go/library.go",
    "P4.at": "b-01.00",
    "P5.title": "<title>", "P5.summary": "<summary>",
}

var _metadata = map[string]string{
    "M1.key": "...", "M1.component_type": "...",
    "M1.architect": "Seanje Lenox-Wise", "M1.implementation": "Nova Dawn",
    // M2-M10...
}

func init() { identity.Register(_pragma["P1.key"], _pragma, _metadata) }
func Pragma() map[string]string { return _pragma }
func Metadata() map[string]string { return _metadata }
```

### Production Patterns

| Pattern | Where | Implementation |
|---------|-------|----------------|
| **Caching** | paths.go (home dir, config dirs) | `sync.Once` for immutable lookups |
| **Thread safety** | Already done in config/util | Verify all shared state protected |
| **Godoc comments** | All exported functions | Add where missing |
| **Import paths** | All files | `creativeworkzstudio.com/bereshit/L0-universal/...` |
| **Error ops** | All error returns | `util.NewLoadError(file, op, err)` vocabulary |

---

## Processing Order (dependency-driven)

### Step 1: identity/registry.go — The Foundation

**Current:** 4-block structure, `sync.RWMutex`, zero dependencies. Has Register/Lookup/All/ByLayer/ByDomain/ByTag plus self-registration.

**Missing:** `//omni:` directives, `_pragma`/`_metadata` maps (ironic — the registry doesn't register itself the same way others do).

**Changes:**
1. Add `//omni:` directives at top
2. Add `_pragma` map (P1-P5) — identity needs identity
3. Add `_metadata` map (M1-M10)
4. Bootstrap self-registration in `init()` (register itself)
5. Verify comment markers at standard widths (═ 76, ─ 64)
6. Add SETUP subsection labels (I→T→TM→K→V→PS)
7. Add BODY subsection labels (Org→H→C→APIs)
8. Add godoc on all exported functions
9. Expand CLOSING with overview + modification policy

**Design note:** Registry is the only file that doesn't import identity (it IS identity). Self-registration is the bootstrap anchor — mirrors Genesis 1:1 as the halt condition that doesn't need a deeper anchor.

### Step 2: config/util/ (10 files) — Verify + Polish

**Current:** Already 4-block from Phases 1-3. `doc.go` has pragma/metadata maps. Thread-safe. Well-tested.

**Changes per file:**
1. **doc.go** — Verify pragma/metadata completeness (M6-M10 present?), verify init() registers
2. **errors.go** — Add `//omni:` directives, `_pragma`/`_metadata` (file-level, not just package-level via doc.go)
3. **exists.go** — Same: directives + maps
4. **formats.go** — Same + verify sync.Mutex usage
5. **jsonc.go** — Same + verify regex caching at init
6. **loader.go** — Same + verify format dispatch completeness
7. **must.go** — Same
8. **root.go** — Same + verify RWMutex usage
9. **tripwire.go** — Same + consider bounded cache for `bannerSeen` (memory leak in long processes)
10. **util_test.go** — No pragma maps (test file), but verify `//omni:` directives

**Key decision:** Do individual files in a package need their own `_pragma`/`_metadata`, or just `doc.go`?

**Answer:** `doc.go` holds package-level identity and does `identity.Register()`. Individual files get `//omni:` directives (for linter compliance) but do NOT duplicate pragma/metadata maps — they reference `doc.go`. This prevents 10 copies of the same registration.

**Revised per-file work:**
- Add `//omni:code/key/version` directives (3 lines at top)
- Verify comment marker widths
- Verify SETUP/BODY subsection labels
- Add godoc where missing
- No pragma/metadata maps in individual files (doc.go handles package identity)

### Step 3: config/ (2 files) — Verify + Polish

**Current:** `loader.go` is exemplary — full 4-block, pragma maps, M1-M10, identity registration, caching with RWMutex. `loader_test.go` has comprehensive assessment pattern.

**Changes:**
- `loader.go` — Verify all M1-M10 fields current, verify CLOSING has minimum sections, add any missing godoc
- `loader_test.go` — Add `//omni:` directives, verify test structure labels

### Step 4: util/fs/paths/paths.go — Heavy Upgrade

**Current:** 3-block (METADATA → BODY → CLOSING). 50+ exported functions. Hardcoded paths like `/media/seanje-lenox-wise/Project/CreativeWorkzStudio_LLC/`. No caching. No identity.

**Changes:**
1. Restructure to full 4-block
2. Add `//omni:` directives + `_pragma`/`_metadata` maps
3. Add `init()` with identity registration
4. Add SETUP block (imports, types, constants, variables)
5. Reorganize BODY into subsection groups:
   - H: Pure path construction helpers
   - C.1: Config resolution
   - C.2: Bereshit paths
   - C.3: Claude paths
   - C.4: State machine paths
   - C.5: Database paths
   - C.6: Logging paths
   - C.7: Directory initialization (EnsureAllDataDirs etc.)
6. **Cache home directory** — `os.UserHomeDir()` called repeatedly, cache with `sync.Once`
7. **Environment variable fallbacks** for hardcoded paths:
   - `BERESHIT_ROOT` → fallback to detected path
   - `CLAUDE_GLOBAL_ROOT` → already has env override, verify
8. Add godoc on all 50+ exported functions
9. Add CLOSING with overview + modification policy + section map
10. **Portability note:** Document that hardcoded paths work on CWS workstation; env vars enable other machines

**Import additions:** `sync` (for caching), `L0-universal/identity` (for registration)

### Step 5: util/pure/cli/cli.go — Moderate Upgrade

**Current:** 3-block. CLI app scaffolding with flag parsing. Pure (stdlib only).

**Changes:**
1. Restructure to 4-block
2. Add `//omni:` directives + `_pragma`/`_metadata` maps
3. Add `init()` with identity registration (import `identity`)
4. Add SETUP subsection labels (T: App struct, Usage types; K: constants; V: none)
5. Add BODY subsection labels (H: helpers; C: App methods; APIs: New, Parse, Fatal)
6. Add godoc on all exported functions/types
7. Add CLOSING (minimum sections)

**Import addition:** `L0-universal/identity`

### Step 6: util/pure/format/format.go — Moderate Upgrade

**Current:** 3-block. Pure formatting functions. Internal `itoa()` avoids strconv.

**Changes:**
1. Restructure to 4-block
2. Add `//omni:` directives + `_pragma`/`_metadata`
3. Add `init()` with identity registration
4. Add SETUP subsection labels (I: imports; no types/constants)
5. Reorganize BODY into groups:
   - H: itoa() helper
   - C.1: Duration formatting
   - C.2: Path formatting
   - C.3: Number formatting
   - C.4: String formatting
   - C.5: Model formatting
6. Add godoc on all exported functions
7. Add CLOSING

**Import addition:** `L0-universal/identity`

### Step 7: util/pure/output/output.go — Moderate Upgrade

**Current:** 3-block. Pure output formatting (headers, tables, JSON helpers).

**Changes:**
1. Restructure to 4-block
2. Add `//omni:` directives + `_pragma`/`_metadata`
3. Add `init()` with identity registration
4. Add SETUP subsection labels
5. Reorganize BODY:
   - H: none
   - C.1: Headers and sections
   - C.2: Formatted output (KeyValue, Bullet, Indent)
   - C.3: Tables
   - C.4: Status indicators
   - C.5: JSON helpers
6. Add godoc on all exported functions
7. Add CLOSING

**Import addition:** `L0-universal/identity`

---

## Design Decisions

### 1. Package-level vs file-level identity

**Decision:** `doc.go` (or the primary file in single-file packages) holds `_pragma`/`_metadata` maps and does `identity.Register()`. Other files in the same package get `//omni:` directives only.

**Why:** Prevents N copies of identical registration. The package is the unit of identity, not the file. Files are components of the package.

### 2. Pure packages importing identity

**Decision:** `util/pure/cli`, `util/pure/format`, `util/pure/output` WILL import `L0-universal/identity` for registration.

**Why:** "Pure" means no external dependencies outside stdlib. `identity` is L0-internal — it's part of our foundation. Self-knowledge is universal. The architecture IS the paradigm.

**Trade-off:** These packages gain one internal dependency. But the identity system is the paradigm in code — every package knowing itself is CPI-SI.

### 3. Hardcoded paths in paths.go

**Decision:** Keep hardcoded paths as defaults, add environment variable overrides.

**Why:** This IS the CWS workstation. The paths work. But portability matters for CI, testing, and eventual deployment. Environment variables are the escape hatch, not the primary path.

### 4. Assurance at L0

**Decision:** Note as future design, don't implement in this pass.

**Why:** Assurance types (`Score`, `AssuranceResult`, `AssuranceSuite`) currently live in L3's `foundation/schema/assurance.go`. They SHOULD be L0 — self-examination is universal, Score is ternary (fundamental). But moving them requires L3 to import from L0 instead, which touches many files. That belongs in the L3 processing pass, not L0.

**Future:** When processing L3, extract core assurance types to `L0-universal/assurance/` and have L3 import them.

### 5. go.mod for L0

**Decision:** Create `go.mod` in `L0-universal/` AFTER all files are processed.

**Why:** Need all import paths settled before module definition. The go.mod establishes `creativeworkzstudio.com/bereshit/L0-universal` as the module path. External dep: `github.com/BurntSushi/toml v1.6.0` (only config packages need it).

---

## Execution Sequence

| # | Step | Files | Est. Changes |
|:-:|------|:-----:|-------------|
| 1 | Process identity/registry.go | 1 | Add omni directives, pragma/metadata, self-bootstrap, SETUP/BODY labels, CLOSING |
| 2 | Polish config/util/ (10 files) | 10 | Add omni directives, verify markers, add missing godoc |
| 3 | Polish config/ (2 files) | 2 | Verify completeness, update M2 version history |
| 4 | Process util/fs/paths/paths.go | 1 | Full 4-block restructure, caching, env fallbacks, identity |
| 5 | Process util/pure/cli/cli.go | 1 | 4-block restructure, identity, godoc |
| 6 | Process util/pure/format/format.go | 1 | 4-block restructure, identity, godoc |
| 7 | Process util/pure/output/output.go | 1 | 4-block restructure, identity, godoc |
| 8 | Create go.mod | 1 | Module definition with deps |
| 9 | Verify: `go build ./...` | — | Compile all packages |

---

## Critical Files

**Templates (reference standard):**
- `.a-new-structure/b-word/seed/code/L0/go/library.go` — 4-block template for libraries
- `.a-new-structure/c-tov/cws-struct/lib/formats/go.ts` — Go linter (11 checks)

**Exemplar (already processed to standard):**
- `.a-new-structure/b-word/L0-universal/config/loader.go` — Gold standard for 4-block Go

**Files to modify:**
```
.a-new-structure/b-word/L0-universal/identity/registry.go
.a-new-structure/b-word/L0-universal/config/util/doc.go
.a-new-structure/b-word/L0-universal/config/util/errors.go
.a-new-structure/b-word/L0-universal/config/util/exists.go
.a-new-structure/b-word/L0-universal/config/util/formats.go
.a-new-structure/b-word/L0-universal/config/util/jsonc.go
.a-new-structure/b-word/L0-universal/config/util/loader.go
.a-new-structure/b-word/L0-universal/config/util/must.go
.a-new-structure/b-word/L0-universal/config/util/root.go
.a-new-structure/b-word/L0-universal/config/util/tripwire.go
.a-new-structure/b-word/L0-universal/config/util/util_test.go
.a-new-structure/b-word/L0-universal/config/loader.go
.a-new-structure/b-word/L0-universal/config/loader_test.go
.a-new-structure/b-word/L0-universal/util/fs/paths/paths.go
.a-new-structure/b-word/L0-universal/util/pure/cli/cli.go
.a-new-structure/b-word/L0-universal/util/pure/format/format.go
.a-new-structure/b-word/L0-universal/util/pure/output/output.go
```

**New file:**
```
.a-new-structure/b-word/L0-universal/go.mod
```

---

## Verification

### After each file
- `//omni:` directives present at top
- 4-block markers at correct widths (═ 76, ─ 64)
- SETUP subsections in order (I→T→TM→K→V→PS) — unused sections omitted, not empty
- BODY subsections in order (Org→H→C→E→APIs) — unused sections omitted
- CLOSING has minimum: overview + modification policy
- All exported functions have godoc comments
- Package doc.go has `_pragma`/`_metadata` maps and `identity.Register()`

### After all files
- `go build ./...` compiles without errors from L0-universal/
- `go vet ./...` passes
- No duplicate `identity.Register()` calls (one per package, not per file)
- Import graph remains acyclic (L0 imports nothing above L0)
- Run Go linter: `deno run --allow-read .a-new-structure/c-tov/cws-struct/mod.ts lint go <file>` on each file

### After go.mod
- `go mod tidy` succeeds
- Only external dep: `github.com/BurntSushi/toml v1.6.0`
- Module path: `creativeworkzstudio.com/bereshit/L0-universal`

---

## What This Enables (Next Steps)

After L0 is processed:
1. **L1-omnicode** processing (3 files — OmniCode format handlers)
2. **L2-platform** processing (11 files — terminal, git, dashboard platform)
3. **L3-cpisi** processing (79 files — the big one: state machine, CPI, database, orchestration)
4. **L5-applications** processing (8 files — dashboard interface)
5. **go.work** establishing full layer hierarchy
6. **Second pass** — find remaining code per layer, repeat
