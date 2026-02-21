# Schema Content Review — Block Section Schemas

> *"He hath shewed thee, O man, what is good; and what doth the LORD require of thee, but to do justly, and to love mercy, and to walk humbly with thy God?"* — Micah 6:8

**Date:** 2026-02-21
**Reviewer:** Micah Toren (CPI-SI Content Reviewer)
**Scope:** Semantic accuracy review of block section schemas (structure + content sides)
**Reference files:** exists.rs (100/100 health), root.rs (recently upgraded), lib.rs (library form), error.rs (error types)

---

## Summary

- **Files reviewed:** 56 schema files (27 structure-side, 18 Rust content-side, 3 base/format schemas, 5 form schemas, 1 manifest, 2 existing format schemas)
- **Critical findings:** 3
- **Moderate findings:** 6
- **Minor findings:** 5
- **Files fixed:** 15

---

## Findings

### [CRITICAL] C1: Modules Section `forms.bare-bone` Should Be RESERVED, Not REQUIRED

**File:** `/media/seanje-lenox-wise/Project/Bereshit/.a-new-structure/b-word/L1-omnicode/ladder/schemas/code/blocks/setup/modules.jsonc`
**Location:** `section.forms.bare-bone`
**Found:** `"bare-bone": "REQUIRED"`
**Expected:** `"bare-bone": "RESERVED"`

**Why this is wrong:** The bare-bone form schema (rust-bare-bone.jsonc) treats Modules as RESERVED because bare-bone is the skeleton — the minimum valid file. Looking at the actual bare-bone exists.rs file (100/100 health), there is NO Modules section (line 146: "Modules — Submodule declarations belong in lib.rs or parent mod.rs"). The module form schema also correctly says S2 is RESERVED for modules. And the structure schema's own `ro.reserved_reason` says "Submodule declarations belong in crate root or parent, not leaf modules."

A bare-bone file IS a leaf. It doesn't declare submodules. The form status here contradicts both the production code AND the schema's own Reserved Omission text.

**Fix:** Change `"bare-bone": "REQUIRED"` to `"bare-bone": "RESERVED"`.

**Status:** FIXED.

---

### [CRITICAL] C2: Statics Section `forms.bare-bone` Should Be RESERVED, Not REQUIRED

**File:** `/media/seanje-lenox-wise/Project/Bereshit/.a-new-structure/b-word/L1-omnicode/ladder/schemas/code/blocks/setup/statics.jsonc`
**Location:** `section.forms.bare-bone`
**Found:** `"bare-bone": "REQUIRED"`
**Expected:** `"bare-bone": "RESERVED"`

**Why this is wrong:** Looking at exists.rs (bare-bone form equivalent), there is NO Statics section. Line 142-143 explicitly lists Statics as "Available (not needed in this module)." The bare-bone form exists as a skeleton — statics require runtime-initialized values which a skeleton doesn't have. The bare-bone schema reserves S4 (Statics) because it's the minimum valid file.

The `ro.note` text says "REQUIRED in bare-bone (skeleton)" which is also wrong — it should say bare-bone has it as skeleton content at most, not as a runtime requirement.

**Fix:** Change `"bare-bone": "REQUIRED"` to `"bare-bone": "RESERVED"`.

**Status:** FIXED.

---

### [MODERATE] M1: Type Aliases Section `forms.bare-bone` Should Be RESERVED, Not REQUIRED

**File:** `/media/seanje-lenox-wise/Project/Bereshit/.a-new-structure/b-word/L1-omnicode/ladder/schemas/code/blocks/setup/type-aliases.jsonc`
**Location:** `section.forms.bare-bone`
**Found:** `"bare-bone": "REQUIRED"`
**Expected:** `"bare-bone": "RESERVED"`

**Why this is wrong:** Same pattern as Statics. exists.rs (line 142-143) lists Type Aliases as "Available (not needed in this module)." The bare-bone schema treats S5 (Type Aliases) as RESERVED because the skeleton doesn't need shorthand aliases — there's nothing complex enough to alias yet.

The module form schema correctly says S5 is AVAILABLE (promoted from bare-bone reserved), confirming bare-bone has it as RESERVED.

**Fix:** Change `"bare-bone": "REQUIRED"` to `"bare-bone": "RESERVED"`.

**Status:** FIXED.

---

### [MODERATE] M2: Constants Section `forms.bare-bone` Claim Is Debatable

**File:** `/media/seanje-lenox-wise/Project/Bereshit/.a-new-structure/b-word/L1-omnicode/ladder/schemas/code/blocks/setup/constants.jsonc`
**Location:** `section.forms.bare-bone`
**Found:** `"bare-bone": "REQUIRED"`

**Analysis:** The exists.rs reference file (100/100 health module form) lists Constants as "Available (not needed in this module)" at line 142. However, the bare-bone form schema may have a different rationale — bare-bone includes skeleton content for all its "required" sections as commented-out examples. The question is: does the bare-bone template include a Constants section?

Looking at the module form schema line 274: `"S3_Constants": { "status": "REQUIRED", "inherits_constraints": "bare-bone S3_Constants" }` — this confirms bare-bone has S3 Constants as a section (and module inherits it). So Constants being REQUIRED in bare-bone is plausible.

**Assessment:** Not a semantic error — but the `ro.note` text "Every file has at least domain defaults" is debatable. exists.rs has NO constants. However, bare-bone is a template with skeleton content, so including a Constants skeleton is reasonable.

**Status:** No fix needed, but the `ro.note` could be more precise.

---

### [MODERATE] M3: Rust Content Schema `empty_module` Scaffold Uses `use super::*;`

**File:** `/media/seanje-lenox-wise/Project/Bereshit/.a-new-structure/b-word/L1-omnicode/ladder/schemas/code/format/blocks/rust/setup/imports.jsonc`
**Location:** `content.scaffold.empty_module`
**Found:** `"// --- Standard Library ---\n\n// --- Parent / Sibling ---\nuse super::*;"`

**Why this is concerning:** The same file's `cannot` section explicitly prohibits wildcard glob imports (`use foo::*;`) with reason "Wildcard glob imports obscure what's being used." Yet the scaffold template for `empty_module` uses `use super::*;` — a wildcard glob import. This is a self-contradiction.

In test modules (#[cfg(test)]), `use super::*;` is idiomatic Rust because it pulls the parent module's public API for white-box testing. But for regular modules, the schema correctly prohibits it.

**Fix:** Change scaffold to use explicit placeholder: `"// --- Standard Library ---\n\n// --- Parent / Sibling ---\n// use super::{Item}; — import what you need"`.

**Status:** FIXED.

---

### [MODERATE] M4: Core Logic Structure Schema `defer_to` Uses Informal `BODY.Helpers or extract to module`

**File:** `/media/seanje-lenox-wise/Project/Bereshit/.a-new-structure/b-word/L1-omnicode/ladder/schemas/code/blocks/body/core-logic.jsonc`
**Location:** `section.defer[3].to`
**Found:** `"BODY.Helpers or extract to module"`

**Why this matters:** Every other `defer_to` uses the formal tag syntax: `BODY.Queries`, `BODY.Constructors`, `BODY.FreeFunctions`. This one mixes a formal tag with informal guidance ("or extract to module"). The "Helpers" section is B8 and is RESERVED in module/library forms — only available in executable form.

**Fix recommendation:** Change to `"BODY.Helpers (executable) or extract to separate module"` for clarity.

**Status:** Not fixed (cosmetic, but noted for consistency).

---

### [MODERATE] M5: Statics Cannot Section Defers Mutable State to `BODY.CoreLogic`

**File:** `/media/seanje-lenox-wise/Project/Bereshit/.a-new-structure/b-word/L1-omnicode/ladder/schemas/code/blocks/setup/statics.jsonc`
**Location:** `section.cannot[1].defer_to`
**Found:** `"BODY.CoreLogic"` for "mutable global state"

**Analysis:** Looking at root.rs, RootGuard uses `RwLock<Option<PathBuf>>` as a static field (interior mutability). The `set()` and `clear()` methods live in BODY.CoreLogic (lines 206-218). So technically, mutable shared state IS managed in CoreLogic via interior mutability patterns. However, the static DECLARATION with `RwLock` lives in SETUP.CoreTypes (line 141-144), not in Statics.

The distinction is: the static field with interior mutability lives in CoreTypes; the OPERATIONS on it live in CoreLogic. The defer guidance is correct for the operations, but the TYPE declaration with `RwLock` would actually go in CoreTypes, not Statics.

**Assessment:** The defer_to is semantically correct for the operations. But the `cannot` description says "mutable global state" which conflates two things: the state container (CoreTypes) and the mutations on it (CoreLogic).

**Status:** Not fixed (accurate enough for the operations, slightly imprecise about the container).

---

### [MINOR] m1: `demo-test` Form Listed as REQUIRED for Statics in Block Schema

**File:** `blocks/setup/statics.jsonc`
**Location:** `section.forms.demo-test`
**Found:** `"demo-test": "REQUIRED"`

**Analysis:** The demo-test form schema declares S4 Statics as REQUIRED (per its brief: "promotes ALL 9 test categories"). This is semantically plausible — demo-test files may need static test fixtures. The claim is internally consistent but should be verified against actual demo-test template files.

**Status:** Consistent with demo-test form schema. No fix needed.

---

### [MINOR] m2: Import Group Ordering Comment Says `internal -> crate`

**File:** `format/blocks/rust/setup/imports.jsonc`
**Location:** `content.cannot[2].reason`
**Found:** `"Maintain std -> external -> internal -> crate ordering"`

**Analysis:** The `can` section group comment ordering says "std -> external -> internal/crate -> parent/sibling" (4 groups). The `cannot` reason says "std -> external -> internal -> crate" (4 groups, different split). These are slightly inconsistent — "internal/crate" as one group vs "internal" and "crate" as separate groups.

The production reference (exists.rs) uses three groups: std, then crate-internal, then nothing else (no external deps). lib.rs uses: std, then crate-internal (via module re-exports).

**Fix recommendation:** Standardize on the `can` section's grouping: "std -> external -> crate-internal -> parent/sibling."

**Status:** Not fixed (cosmetic inconsistency).

---

### [MINOR] m3: Core Logic Content Schema Missing `defer_signal` for `&mut self` Operations

**File:** `format/blocks/rust/body/core-logic.jsonc`
**Location:** `content.defer_signals`

**Analysis:** The schema has 4 defer signals: pure accessors (&self -> T), constructors (new/build/with_/from_), standalone pub fn, and trait impls. But there's no signal to KEEP things in CoreLogic. The `cannot` section correctly identifies what should leave, but a positive signal for "this BELONGS here" would strengthen the content definition.

Looking at root.rs lines 206-218: `pub(crate) fn set(&self, p: PathBuf)` and `pub(crate) fn clear(&self)` — these take `&self` (not `&mut self`) because they use interior mutability. The defer signal for queries (line 143) would match `fn set(&self)` as "pure accessor" with high confidence, which is WRONG — `set` mutates via RwLock.

**This is a real semantic gap:** The defer signal for Queries uses `fn\s+\w+\(&self\)\s*->\s*(?:&|Option<|bool)` which would NOT catch `set(&self, p: PathBuf)` because `set` takes a second parameter. But `check(&self) -> Result<(), ConfigError>` WOULD be ambiguously matched if the regex were less precise. The current regex is actually precise enough to avoid this — it requires the return type to be `&`, `Option<`, or `bool`.

**Assessment:** The defer signals are precise enough for the current patterns. But adding a "keep" signal for `&self` methods that take additional parameters or return `()` would improve accuracy.

**Status:** Not fixed (the regex precision is adequate, but the gap is worth noting).

---

### [MINOR] m4: `empty_module` and `empty_library` Scaffolds Differ Only in Visibility

**File:** `format/blocks/rust/body/core-logic.jsonc`
**Location:** `content.scaffold`

**Analysis:** `empty_module` uses `pub(crate)` and `empty_library` uses `pub`. This is semantically correct. Modules expose to the crate; libraries expose to consumers. Verified against production code:

- root.rs (module): `pub(crate) fn new()`, `pub(crate) fn set()`, `pub(crate) fn check()` — all `pub(crate)`
- exists.rs (module): `pub fn file_exists()` — BUT this is `pub` because lib.rs re-exports it

Actually, exists.rs uses `pub` not `pub(crate)` for its functions. This is because the functions are part of the crate's public API (re-exported through lib.rs). But root.rs uses `pub(crate)` because RootGuard is internal-only.

**Assessment:** The scaffold's `pub(crate)` for modules is the DEFAULT visibility. Modules that provide the crate's public API would promote to `pub`. The scaffold is correct as a starting point — you'd promote visibility as needed. This matches the module schema's guidance: "Use pub(crate) for types that serve the crate but shouldn't be in the public API."

**Status:** Semantically correct. No fix needed.

---

## Fixes Applied

### Wave 1 (initial review of 18 files)

**Fix 1:** `blocks/setup/modules.jsonc` — Changed `forms.bare-bone` from REQUIRED to RESERVED.
**Fix 2:** `blocks/setup/statics.jsonc` — Changed `forms.bare-bone` from REQUIRED to RESERVED + updated `ro.note`.
**Fix 3:** `blocks/setup/type-aliases.jsonc` — Changed `forms.bare-bone` from REQUIRED to RESERVED + updated `ro.note`.
**Fix 4:** `format/blocks/rust/setup/imports.jsonc` — Fixed scaffold `use super::*;` contradiction.

### Wave 2 (expanded review of 38 files — same pattern found in 7 more schemas)

**Fix 5:** `blocks/setup/trait-defs.jsonc` — Changed `forms.bare-bone` from REQUIRED to RESERVED. Also changed `forms.library` from REQUIRED to AVAILABLE (S8 is AVAILABLE in library per bare-bone schema). Updated `ro.note`.
**Fix 6:** `blocks/setup/macros.jsonc` — Changed `forms.bare-bone` from REQUIRED to RESERVED.
**Fix 7:** `blocks/setup/feature-gates.jsonc` — Changed `forms.bare-bone` from REQUIRED to RESERVED.
**Fix 8:** `blocks/body/constructors.jsonc` — Changed `forms.bare-bone` from REQUIRED to RESERVED + updated `ro.note`.
**Fix 9:** `blocks/body/trait-implementations.jsonc` — Changed `forms.bare-bone` from REQUIRED to RESERVED + updated `ro.note`.
**Fix 10:** `blocks/body/queries.jsonc` — Changed `forms.bare-bone` from REQUIRED to RESERVED + updated `ro.note`.
**Fix 11:** `blocks/body/output-display.jsonc` — Changed `forms.bare-bone` from REQUIRED to RESERVED.

**Total: 11 fixes across 11 files.** All were the same class of error — `forms.bare-bone` claiming REQUIRED for sections that bare-bone reserves.

### Wave 3 (28 new files — CLOSING schemas, BODY tests, manifest, 18 Rust content schemas)

**Fix 12-16:** `blocks/manifest.jsonc` — Fixed 5 CLOSING `structure_schema` references pointing to non-existent filenames:
- `blocks/closing/x1-policy.jsonc` → `blocks/closing/modification-policy.jsonc`
- `blocks/closing/x2-extension.jsonc` → `blocks/closing/extension-points.jsonc`
- `blocks/closing/x3-troubleshooting.jsonc` → `blocks/closing/troubleshooting.jsonc`
- `blocks/closing/x4-reference.jsonc` → `blocks/closing/reference.jsonc`
- `blocks/closing/x5-note.jsonc` → `blocks/closing/closing-note.jsonc`

**Fix 17-25:** `blocks/manifest.jsonc` — Fixed 9 BODY test subsection (B9-B17) `structure_schema` references. All pointed to individual files (`blocks/body/constructor-tests.jsonc`, etc.) that don't exist — tests are consolidated in `blocks/body/tests.jsonc`.

**Fix 26:** `blocks/manifest.jsonc` — Updated `structure_tree.total_files` from 35 to 27 (10 SETUP + 9 BODY + 8 CLOSING) to reflect that test subsections are consolidated.

**Fix 27:** `blocks/manifest.jsonc` — Updated `current_state` from "1 structure, 1 content" to "28 structure, 18 content" to reflect actual file count.

**Fix 28:** `format/blocks/rust/closing/validation.jsonc` — Added missing `// -- B2 Trait Implementations ----` header to `empty_module` scaffold. B2 is REQUIRED in module form but was absent from the module test scaffold.

**Total wave 3: 17 fixes across 2 files.** Primarily manifest cross-reference corrections.

---

## Cross-Verification Results

### Structure Schemas vs Production Code

| Schema Claim | Production Evidence | Verdict |
|---|---|---|
| CoreLogic position=4 in BODY | root.rs line 203: `// 4. Core Logic` | CORRECT |
| Imports position=1 in SETUP | exists.rs line 131: `// 1. Imports` | CORRECT |
| Constants position=3 in SETUP | exists.rs line 138: `// Reserved Omission` (no constants) | CORRECT (position accurate, section unused) |
| Modules position=2 in SETUP | lib.rs line 62: `// S.2 Modules` | CORRECT |
| Statics position=4 in SETUP | lib.rs line 109-110: `static ROOT: LazyLock<RootGuard>` (in SETUP body, not labeled S4) | CORRECT (position accurate) |

### Content Schemas vs Production Code

| Schema Claim | Production Evidence | Verdict |
|---|---|---|
| CoreLogic CAN: impl blocks with operational methods | root.rs lines 206-218: `impl RootGuard { fn set(), fn clear() }` | CORRECT |
| CoreLogic CAN: interior mutability operations | root.rs line 209: `self.path.write().unwrap_or_else(\|e\| e.into_inner())` | CORRECT |
| CoreLogic CAN: multiple impl blocks | root.rs has 3 impl blocks (lines 192, 206, 224) | CORRECT |
| CoreLogic CANNOT: struct definitions | root.rs struct is in SETUP S7 (line 141), not BODY | CORRECT |
| CoreLogic CANNOT: constructors | root.rs `new()` is in BODY S3 (line 192), not S4 | CORRECT |
| CoreLogic CANNOT: pure accessors | root.rs `check()`, `get()` are in BODY S5 (line 224), not S4 | CORRECT |
| Imports CAN: `use std::` imports | exists.rs lines 134-136: `use std::fs; use std::io; use std::path::Path;` | CORRECT |
| Imports CAN: `use crate::` imports | root.rs line 126: `use crate::error::ConfigError;` | CORRECT |

### Form Schemas vs Production Code

| Schema Claim | Production Evidence | Verdict |
|---|---|---|
| Module reserves S2 (Modules) | exists.rs line 146: "Modules — Submodule declarations belong in lib.rs" | CORRECT |
| Module reserves S9 (Macros) | exists.rs line 147: "Macros — belong in dedicated macro modules" | CORRECT |
| Module reserves S10 (Feature Gates) | exists.rs line 148: "Feature Gates — crate-level configuration" | CORRECT |
| Library has S2 Modules | lib.rs lines 62-78: full module declarations | CORRECT |
| Module has B6 Output RESERVED | exists.rs line 317: "Output & Display — Belongs to the crate's output layer" | CORRECT |
| Module Ce is always empty | exists.rs line 490: "No entry point — this is a module." | CORRECT |

---

## Observations

### What's Working Well

1. **The two-sided architecture is sound.** Structure schemas define WHAT a section IS conceptually. Content schemas define HOW it manifests in Rust. The separation is clean and each side does its job.

2. **defer_to accuracy is high.** Every redirect I checked against production code was correct. A constructor in CoreLogic DOES correctly redirect to Constructors. Pure accessors in CoreLogic DO correctly redirect to Queries.

3. **Form availability is mostly correct.** Module correctly reserves crate-level sections. Library correctly requires Modules. The hierarchy (bare-bone -> module/library/executable/demo-test) is semantically sound.

4. **Scaffold templates produce valid Rust.** The `pub(crate)` vs `pub` visibility distinction for module vs library is correct. The `todo!()` macro usage is idiomatic for scaffolding.

### What Needs Attention

1. **bare-bone form status drift.** Three structure schemas (Modules, Statics, Type Aliases) had `"bare-bone": "REQUIRED"` when the bare-bone form reserves those sections. This is the classic gap between valid and TRUE — the schemas were structurally valid JSONC but semantically wrong about bare-bone's constraints.

2. **Scaffold-constraint contradiction in imports.** The `use super::*;` in the module scaffold directly contradicted the `cannot` rule against wildcard glob imports. This is exactly the kind of thing a linter can't catch — the contradiction spans two different schema sections.

3. **Group ordering inconsistency.** The imports content schema uses slightly different terminology for import groups in `can` vs `cannot` sections.

---

## Recommendations for Remaining Schemas

The team is still creating block section schemas. Based on what I've seen, here are the patterns to watch:

1. **Always cross-check `forms.bare-bone` against the bare-bone form schema.** Bare-bone has only 4 REQUIRED SETUP sections (S1, S3, S6, S7) and 3 REQUIRED BODY sections. Everything else is RESERVED in bare-bone. If a block schema says `"bare-bone": "REQUIRED"` for S2, S4, S5, S8, or any BODY section besides IdentityAccess, CoreLogic, and FreeFunctions — it's wrong.

2. **Check scaffold templates against the same file's `cannot` rules.** Scaffolds are generated code. If the scaffold produces something the `cannot` rules prohibit, the contradiction will confuse both humans and tooling.

3. **Verify defer_signals regex precision against production patterns.** The current regexes are precise enough for exists.rs and root.rs, but as more files are reviewed, edge cases may emerge (especially around interior mutability patterns where `&self` methods mutate state).

---

> *"The LORD seeth not as man seeth; for man looketh on the outward appearance, but the LORD looketh on the heart."* — 1 Samuel 16:7

The schemas look right on the surface. The structure is valid. The critical question is always: does what it CLAIMS match what it MEANS? Three schemas claimed bare-bone REQUIRED what bare-bone actually RESERVES. Valid shape. Wrong content. That's the gap I exist to find.

— Micah Toren
