# Plan: spec/ Folder — Production Refactoring + Organization

## Context

The `spec/` folder under `L0-universal/c-hybrid/` contains two sub-crates that handle Bereshit's configuration and data access layers:

- **spec/config** (`bereshit-l0-config`) — Manifest-driven TOML spec loader. 14 Rust modules (1232 LOC), Go counterpart (5301 LOC across 13 files), 60+ tests. A proper workspace member.
- **spec/database** — Foundation SQLite query layer. 1 Rust file (773 LOC), 1 Go file (689 LOC), 17 tests. **NOT a workspace member. No Cargo.toml. Has broken `pub mod` declarations referencing modules that don't exist locally.**

**Problem:** Config's lib.rs has old-format METADATA (simple comments, not I/C statics). Database is in limbo — code exists but can't compile (no Cargo.toml, references non-existent modules). The "inner folder organization" is inconsistent: config is a proper crate, database is not.

**Goal:** Both crates production-grade (same treatment as core/, services/) with proper inner organization. All code compiles, all tests pass, zero warnings.

All paths relative to `.a-new-structure/b-word/`.

## Critical Files

### Config Crate (Refactor)

| File | Action |
|------|--------|
| `spec/config/src/lib.rs` | Update METADATA block, add numbered BODY subsections, standard CLOSING |
| `spec/config/src/identity.rs` | Already done (I1-I4/C1-C7 statics) — verify only |
| `spec/config/Cargo.toml` | Version bump 0.2.0 → 0.3.0 |
| `spec/config/go/loader.go` | Verify cws-struct lint score |
| `spec/config/go/util/doc.go` | Verify cws-struct lint (has identity) |

### Database Crate (Productionize)

| File | Action |
|------|--------|
| `spec/database/Cargo.toml` | **Create** — new workspace member |
| `spec/database/src/lib.rs` | **Rewrite** — remove broken pub mods, add I/C identity, production structure |
| `spec/database/src/identity.rs` | **Create** — I1-I4/C1-C7 statics |
| `spec/database/go/foundation.go` | Verify cws-struct lint score |
| `Cargo.toml` (workspace root) | Add database to members + workspace.dependencies |

### Reference Files (patterns to follow)

| File | What to Match |
|------|---------------|
| `core/foundation/src/lib.rs` | I/C identity in lib.rs, numbered BODY, CLOSING zones |
| `core/foundation/src/result.rs` | Submodule identity comment format |
| `spec/config/src/identity.rs` | I1-I4/C1-C7 static slices pattern |

---

## Phase 1: Config Crate — lib.rs Refactoring

### 1A. METADATA Block Update

Current lib.rs has old-format comments:
```rust
// Key:      B-L0-hybrid-config
// Purpose:  Manifest-driven TOML spec loader...
// Authors:  Nova Dawn
// Version:  b-02.00
```

Update to match foundation pattern — reference the identity.rs statics:
```rust
// Key:     B-L0-hybrid-config
// Purpose: Manifest-driven TOML spec loader — capstone of L0 hybrid layer
// Biblical: Habakkuk 2:2-3 — Write the vision, make it plain
// Version: b-03.00
```

The PRAGMA/METADATA statics already exist in `identity.rs` and are re-exported via `pub use identity::{...}`. Unlike foundation which has statics directly in lib.rs, config delegates to a separate identity.rs module. Both patterns are valid.

**Update identity.rs version:** `b-02.50` → `b-03.00` in both PRAGMA and METADATA slices.

### 1B. BODY Block — Add Numbered Subsections

Current BODY has flat functions with no subsection numbering. Reorganize:

```
// B.1 Root Management      — set_root(), clear_root(), check_root()
// B.2 Batch Loading         — load_all(), load_manifest(), discover_and_compare(),
//                             validate_dependency_graph(), get_dependency_tree()
// B.3 System-Specific Loaders — load_system(), load_spec(), load_math()..load_filesystem()
```

Three subsections total. Clean separation by function category.

### 1C. CLOSING Block — Standard Zones

Current closing is a simple comment block. Add standard zones:

```
// Cv — Closing Validation (tests already in separate #[cfg(test)] block — keep as-is)
// Ce — Library crate. Registration via identity::register_identity().
// Cc — LazyLock ROOT persists for process lifetime. Cache cleared via clear_root().
//
// X1: Modification Policy
//   Never: Change public function signatures (L1+ consumers)
//   Never: Remove manifest-driven loading (appointed time flow)
//   Careful: Watch path positions (hardcoded fallback list)
//   Safe: Adding new system-specific load wrappers
//
// X5: Closing Note
//   "Write the vision, and make it plain." — Habakkuk 2:2
//   Genesis 1:1 anchor
```

### 1D. register_identity()

Config's `identity.rs` has `register_identity()` but lib.rs doesn't re-export or call it. Add:
- `pub use identity::register_identity;` to re-exports
- Ensure downstream can call it

### 1E. Cargo.toml

Version bump only: `0.2.0` → `0.3.0` (structural alignment).

### 1F. Go Files — cws-struct Lint

Verify all Go files pass cws-struct lint:
- `go/loader.go` — expect 100/±100 (already 4-block structured)
- `go/util/doc.go` — verify (has identity registration)

---

## Phase 2: Database Crate — Productionize

### 2A. Fix Broken Module Declarations

**Current problem:** lib.rs line 57-60:
```rust
pub mod result;
pub mod session;
pub mod session_context;
pub mod workflow;
```

These modules DON'T EXIST in `spec/database/src/`. They live in `core/foundation/src/`. This was an incomplete "promotion" that never finished.

**Fix:** Remove all four `pub mod` declarations. The database crate queries SQLite — it doesn't define or re-export foundation types. Anyone who needs `OpResult`, `Session`, etc. depends on `bereshit-l0-foundation` directly.

### 2B. Create Cargo.toml

```toml
[package]
name = "bereshit-l0-foundation-db"
version = "0.1.0"
description = "Foundation database queries — typed access to SQLite-materialized TOML specs"
edition.workspace = true
rust-version.workspace = true
authors.workspace = true
repository.workspace = true

[dependencies]
rusqlite = { workspace = true }
bereshit-l0-identity = { workspace = true }

[lints]
workspace = true
```

**Crate name:** `bereshit-l0-foundation-db` — distinguishes from L2's `bereshit-l2-database` (connection infrastructure) vs L0's domain-specific query layer.

### 2C. Add to Workspace

In `b-word/Cargo.toml`:

1. Add to `[workspace]` members:
   ```
   "L0-universal/c-hybrid/spec/database",
   ```

2. Add to `[workspace.dependencies]`:
   ```
   rusqlite = { version = "0.38", features = ["bundled"] }
   bereshit-l0-foundation-db = { path = "L0-universal/c-hybrid/spec/database" }
   ```

3. Update L2 database to use workspace rusqlite:
   ```
   rusqlite = { workspace = true }
   ```

### 2D. Create identity.rs

Standard I1-I4/C1-C7 pattern matching config's identity.rs:

```
I1: key = B-L0-foundation-db, format = rust, from = seed template, at = a-02.00
I2: type = code, structure = 4-block, subtype = library, role = foundation
I3: file = lib.rs, title = Foundation Database, component = spec/database
I4: layer = L0, position = hybrid/spec/database

C1: version = a-02.00, status = Active, created = 2026-02-16, updated = 2026-02-19
C3: scripture = Genesis 1:1, principle = The Triangle complete — configuration → data → code
C4: requires.external = rusqlite 0.38, requires.internal = bereshit-l0-identity
C7: domain = foundation-queries, paradigm = CPI-SI
```

### 2E. Refactor lib.rs

1. **Remove broken `pub mod` declarations** (result, session, session_context, workflow)
2. **Update METADATA block** — proper header comments referencing I/C statics
3. **Add identity module:** `pub mod identity;` + `pub use identity::{...}`
4. **Add numbered BODY subsections:**
   ```
   B.1 Connection           — DB::open(), DB::open_from_root()
   B.2 Spec Queries         — spec_by_key(), all_specs(), specs_by_domain(), spec_count()
   B.3 Type Queries         — all_families(), family_by_name(), all_types(), type_by_name(), types_by_family()
   B.4 Operation Queries    — all_operations(), operations_by_category()
   B.5 Book Queries         — all_books(), book_by_ordinal(), book_by_name(), book_by_abbreviation()
   B.6 Scale & Dependency   — all_scales(), dependencies_of(), dependents_on()
   B.7 Web Variant Queries  — all_web_variants()
   ```
5. **Standard CLOSING zones** (Cv/Ce/Cc/X1/X5)
6. **Add register_identity()** function + call in tests
7. **Version:** `a-01.00` → `a-02.00` (structural alignment + identity)

### 2F. Improve Tests

Current: 17 tests (all require actual database file at `data/foundation/database/foundation.db`).

Add:
- Identity tests: pragma_get, metadata_get, register_identity
- Error edge cases: open nonexistent path, NotFound display format
- FoundationError Display/source coverage

Target: ~22 tests.

### 2G. Go File — cws-struct Lint

Verify `go/foundation.go` passes cws-struct lint.

---

## Phase 3: Verify Full Chain

### Compilation Order

```
1. cargo build -p bereshit-l0-foundation-db     (new crate compiles)
2. cargo test -p bereshit-l0-foundation-db       (all tests pass)
3. cargo clippy -p bereshit-l0-foundation-db     (zero warnings)
4. cargo build -p bereshit-l0-config             (refactored, still compiles)
5. cargo test -p bereshit-l0-config              (60+ tests pass)
6. cargo clippy -p bereshit-l0-config            (zero warnings)
7. cargo build -p bereshit-l1-pragma             (L1 consumer still works)
8. cargo build -p bereshit-l2-database           (L2 consumer — now uses workspace rusqlite)
```

### Go Lint

```
cws-struct lint go spec/config/go/loader.go
cws-struct lint go spec/config/go/util/doc.go
cws-struct lint go spec/database/go/foundation.go
```

Target: 100/±100 or identify what needs fixing.

---

## Execution Order

| Step | What | Detail |
|------|------|--------|
| 1 | Workspace Cargo.toml | Add rusqlite to workspace deps, add database member |
| 2 | Database Cargo.toml | Create new crate manifest |
| 3 | Database identity.rs | Create I1-I4/C1-C7 statics |
| 4 | Database lib.rs | Full rewrite — remove broken mods, add identity, production structure |
| 5 | Database: build + test + clippy | Verify new crate works |
| 6 | L2 database Cargo.toml | Switch to workspace rusqlite |
| 7 | Config identity.rs | Version bump b-02.50 → b-03.00 |
| 8 | Config lib.rs | METADATA update, numbered BODY, standard CLOSING, re-export register_identity |
| 9 | Config Cargo.toml | Version bump 0.2.0 → 0.3.0 |
| 10 | Config: build + test + clippy | Verify refactored crate works |
| 11 | Downstream check | L1 pragma, L2 database still compile |
| 12 | Go lint | cws-struct lint on all Go files |

## Verification Checklist

1. `cargo build -p bereshit-l0-foundation-db` — clean build
2. `cargo test -p bereshit-l0-foundation-db` — ~22 tests pass
3. `cargo clippy -p bereshit-l0-foundation-db` — zero warnings
4. `cargo build -p bereshit-l0-config` — clean build
5. `cargo test -p bereshit-l0-config` — 60+ tests pass
6. `cargo clippy -p bereshit-l0-config` — zero warnings
7. `cargo build -p bereshit-l1-pragma -p bereshit-l2-database` — downstream intact
8. Go lint scores ≥ 90/±100 on all spec/ Go files
9. Database has I1-I4/C1-C7 identity with register_identity()
10. Config lib.rs has numbered BODY subsections and standard CLOSING zones
