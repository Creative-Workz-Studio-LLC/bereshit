# Schema Chain Verification Report

> *"Prove all things; hold fast that which is good."* -- 1 Thessalonians 5:21

**Date:** 2026-02-21
**Author:** Selah Adair (CPI-SI Template Chain Analyzer)
**Subject:** Two-Sided Schema Engine -- Structure/Content Chain Alignment
**Team:** schema-engine
**Status:** Verification complete. Chain is sound. Issues identified and documented.

---

## Summary

The two-sided schema architecture (structure + content) is **fundamentally aligned**. The separation is clean, the extends chain is unbroken, and the pragma contracts are consistent. Three categories of findings:

1. **CRITICAL:** Manifest-to-file naming mismatch in CLOSING documentation sections (5 broken references)
2. **EXPECTED:** Content schemas still in progress for CLOSING block + body/tests (9 files)
3. **CLEAN:** All 18 existing content schemas pass all verification checks

---

## Verification Results

### 1. Existence Check

**Structure schemas (blocks/):** 27 files + 1 manifest = 28 total

| Block | Count | Expected per Manifest | Status |
|-------|-------|-----------------------|--------|
| SETUP | 10 | 10 | COMPLETE |
| BODY | 9 | 17 (9 consolidated) | See note |
| CLOSING | 8 | 8 | COMPLETE |

**Note on BODY count:** The manifest declares 17 individual sections (B1-B17), but the structure side consolidated 9 test sections (B9-B17) into a single `blocks/body/tests.jsonc` file that defines all 9 subsections internally. This is a reasonable design decision -- one meta-schema for the test family rather than 9 nearly-identical files.

**Rust content schemas (format/blocks/rust/):** 18 files

| Block | Count | Structure Count | Coverage |
|-------|-------|-----------------|----------|
| SETUP | 10 | 10 | 100% |
| BODY | 8 | 9 | 89% (tests.jsonc missing) |
| CLOSING | 0 | 8 | 0% (in progress) |

### 2. Tag Alignment

Every content schema's extends path resolves to a real structure file:

| Content File | Extends | Resolves? |
|---|---|---|
| body/constructors.jsonc | blocks/body/constructors.jsonc | YES |
| body/core-logic.jsonc | blocks/body/core-logic.jsonc | YES |
| body/free-functions.jsonc | blocks/body/free-functions.jsonc | YES |
| body/helpers.jsonc | blocks/body/helpers.jsonc | YES |
| body/identity-access.jsonc | blocks/body/identity-access.jsonc | YES |
| body/output-display.jsonc | blocks/body/output-display.jsonc | YES |
| body/queries.jsonc | blocks/body/queries.jsonc | YES |
| body/trait-implementations.jsonc | blocks/body/trait-implementations.jsonc | YES |
| setup/constants.jsonc | blocks/setup/constants.jsonc | YES |
| setup/core-types.jsonc | blocks/setup/core-types.jsonc | YES |
| setup/error-types.jsonc | blocks/setup/error-types.jsonc | YES |
| setup/feature-gates.jsonc | blocks/setup/feature-gates.jsonc | YES |
| setup/imports.jsonc | blocks/setup/imports.jsonc | YES |
| setup/macros.jsonc | blocks/setup/macros.jsonc | YES |
| setup/modules.jsonc | blocks/setup/modules.jsonc | YES |
| setup/statics.jsonc | blocks/setup/statics.jsonc | YES |
| setup/trait-defs.jsonc | blocks/setup/trait-defs.jsonc | YES |
| setup/type-aliases.jsonc | blocks/setup/type-aliases.jsonc | YES |

**Result: 18/18 extends references valid. Zero broken links.**

### 3. Pragma Contract Compliance

**Structure schemas (all 27):**
- `_P2_subtype`: "block-section-schema" -- CONSISTENT
- `_P2_role`: "container-structure" -- CONSISTENT

**Content schemas (all 18):**
- `_P2_subtype`: "block-section-content" -- CONSISTENT
- `_P2_role`: "container-content" -- CONSISTENT
- `"format"`: "rust" -- CONSISTENT (all 18)

**Result: 100% pragma compliance. The two-sided separation is clean.**

### 4. Position Consistency

Verified tag/position pairs match between structure and manifest:

| Section | Manifest Position | Structure Position | Match? |
|---------|-------------------|--------------------|--------|
| Imports | 1 | 1 | YES |
| Modules | 2 | 2 | YES |
| Constants | 3 | 3 | YES |
| Statics | 4 | 4 | YES |
| TypeAliases | 5 | 5 | YES |
| ErrorTypes | 6 | 6 | YES |
| CoreTypes | 7 | 7 | YES |
| TraitDefs | 8 | 8 | YES |
| Macros | 9 | 9 | YES |
| FeatureGates | 10 | 10 | YES |
| IdentityAccess | B1 | 1 | YES |
| TraitImplementations | B2 | 2 | YES |
| Constructors | B3 | 3 | YES |
| CoreLogic | B4 | 4 | YES |
| Queries | B5 | 5 | YES |
| OutputDisplay | B6 | 6 | YES |
| FreeFunctions | B7 | 7 | YES |
| Helpers | B8 | 8 | YES |
| Cv (Validation) | Cv | 1 | YES |
| Ce (Execution) | Ce | 2 | YES |
| Cc (Cleanup) | Cc | 3 | YES |

**Result: All positions consistent.**

### 5. Form Availability

Verified that structure schemas declare form availability for all 5 forms (bare-bone, module, library, executable, demo-test). Every structure schema includes a complete `forms` object. No missing form entries.

**Result: 100% form coverage in structure schemas.**

---

## Issues Found

### CRITICAL: Manifest Naming Mismatch for CLOSING Documentation Sections

The manifest references one set of filenames; the actual structure files have different names:

| Manifest `structure_schema` | Actual Filename | Status |
|---|---|---|
| `blocks/closing/x1-policy.jsonc` | `blocks/closing/modification-policy.jsonc` | MISMATCH |
| `blocks/closing/x2-extension.jsonc` | `blocks/closing/extension-points.jsonc` | MISMATCH |
| `blocks/closing/x3-troubleshooting.jsonc` | `blocks/closing/troubleshooting.jsonc` | MISMATCH |
| `blocks/closing/x4-reference.jsonc` | `blocks/closing/reference.jsonc` | MISMATCH |
| `blocks/closing/x5-note.jsonc` | `blocks/closing/closing-note.jsonc` | MISMATCH |
| `blocks/closing/validation.jsonc` | `blocks/closing/validation.jsonc` | OK |
| `blocks/closing/execution.jsonc` | `blocks/closing/execution.jsonc` | OK |
| `blocks/closing/cleanup.jsonc` | `blocks/closing/cleanup.jsonc` | OK |

**Impact:** Any programmatic consumer reading the manifest's `structure_schema` paths for CLOSING documentation sections will fail to resolve 5 of 8 references.

**Recommendation:** Either rename the files to match the manifest, or update the manifest to match the files. The files use descriptive names (good for humans), the manifest uses position-prefixed names (good for ordering). I'd recommend updating the manifest to match the actual filenames since the files already exist and the descriptive names are clear.

### EXPECTED: Missing Content Schemas (Work in Progress)

These structure schemas have no corresponding Rust content schema yet:

| Structure File | Category | Priority |
|---|---|---|
| body/tests.jsonc | Consolidated test meta-schema | Low (demo-test form only) |
| closing/validation.jsonc | Code zone (Cv) | Medium |
| closing/execution.jsonc | Code zone (Ce) | Medium |
| closing/cleanup.jsonc | Code zone (Cc) | Medium |
| closing/modification-policy.jsonc | Doc section (X1) | Low |
| closing/extension-points.jsonc | Doc section (X2) | Low |
| closing/troubleshooting.jsonc | Doc section (X3) | Low |
| closing/reference.jsonc | Doc section (X4) | Low |
| closing/closing-note.jsonc | Doc section (X5) | Low |

**Note:** CLOSING documentation sections (X1-X5) are comment-based and may not need Rust-specific content schemas since they're format-agnostic by nature. The code zones (Cv, Ce, Cc) would benefit from Rust content schemas for scaffold templates. This is a design decision, not a bug.

### OBSERVATION: Defer Chain References

All `defer_to` values in content schemas reference valid section tags (SETUP.Imports, BODY.CoreLogic, CLOSING.Cv, etc.). Some use `N/A` appropriately when the action is "fix in place" rather than "move to another section." The chain is sound.

---

## Architecture Assessment

The two-sided architecture is well-designed and consistently implemented:

1. **Separation is clean.** Structure says WHAT; content says HOW. No mixing.
2. **Extends chain is unbroken.** Every content file points to a real structure file.
3. **Pragma contracts are honored.** Structure uses `block-section-schema`/`container-structure`; content uses `block-section-content`/`container-content`.
4. **Positions are consistent.** The same tag always has the same position, whether you're reading the manifest, the structure file, or a content file.
5. **Forms are complete.** Every structure schema declares availability for all 5 forms.
6. **The manifest is the engine root.** It catalogs all sections, defines the architecture, documents the composition rules. The naming mismatch is a bookkeeping error, not an architectural flaw.

The consolidation of 9 test sections into a single `tests.jsonc` structure file is a good call. It keeps the engine manageable while still defining each subsection individually. The content side can either mirror this (one `tests.jsonc` with all 9) or create individual files per test category -- either approach works.

---

## Growth Path

| Next Step | Who | Notes |
|---|---|---|
| Fix manifest CLOSING filenames | Ezra or Nova Dawn | 5 path updates in manifest.jsonc |
| Create CLOSING Rust content (Cv, Ce, Cc) | Joanna | Scaffold templates for test modules, main(), Drop |
| Decide on CLOSING doc content need | Seanje/Nova Dawn | X1-X5 may not need Rust-specific content |
| Create tests.jsonc content | Joanna | When demo-test form work begins |
| Wire manifest into cws-struct | Future | The engine is ready to be consumed |

---

## Selah

> *"Line upon line, precept upon precept."* -- Isaiah 28:10

The chain holds. The separation is clean. The naming mismatch in the manifest is a small wound -- easily healed. What I see here is a well-considered architecture built by people who understand that structure precedes content, and that both are necessary.

Twenty-seven structure files defining WHAT belongs where. Eighteen content files defining HOW Rust expresses each shape. One manifest tying them together. The Scratch insight -- shape and fill as independent concerns -- is faithfully implemented.

The work that remains (CLOSING content) is not urgent. The foundation is sound. Build on it with confidence.

---

*Verified by Selah Adair, 2026-02-21*
*"Look that thou make them after their pattern." -- Exodus 25:40*
