# Micah Toren — Content Reviewer Memory

## Schema Review Patterns (2026-02-21)

### bare-bone Form Status Rules
- bare-bone has ONLY 4 REQUIRED SETUP sections: S1 (Imports), S3 (Constants), S6 (ErrorTypes), S7 (CoreTypes)
- S2 (Modules), S4 (Statics), S5 (TypeAliases), S8 (TraitDefs), S9 (Macros), S10 (FeatureGates) are all RESERVED in bare-bone
- bare-bone has ONLY 3 REQUIRED BODY sections: B1 (IdentityAccess), B4 (CoreLogic), B7 (FreeFunctions)
- When reviewing block section schemas: ALWAYS cross-check `forms.bare-bone` against these lists

### Common Schema Content Errors
- **Scaffold-constraint contradiction:** Scaffold templates can produce code that violates the same schema's `cannot` rules. Always verify scaffolds against the `cannot` section.
- **bare-bone REQUIRED drift:** Block schemas default to REQUIRED for all forms. Reviewers must manually verify which forms actually require the section.
- **Group ordering inconsistency:** Different sections within the same file may describe the same concept differently (e.g., import grouping).

### Reference Files for Verification
- **exists.rs** (`spec/config/src/exists.rs`): 100/100 health, module form, utility role — clean reference for module patterns
- **root.rs** (`spec/config/src/root.rs`): module form, infrastructure role — shows interior mutability patterns
- **lib.rs** (`spec/config/src/lib.rs`): library form — shows S2 Modules, re-exports, statics
- **error.rs** (`spec/config/src/error.rs`): module form — shows error type patterns with enum-typed fields

### Manifest Cross-Reference Errors
- **Filename mismatches:** Manifest referenced CLOSING schemas with abbreviated names (x1-policy, x2-extension, etc.) but actual files use descriptive names (modification-policy, extension-points, etc.). Always verify `structure_schema` paths point to real files.
- **Consolidated vs individual files:** Test subsections B9-B17 are consolidated in a single `blocks/body/tests.jsonc` file. The manifest catalog lists them individually but all point to the same file.
- **Stale counts:** `current_state` and `total_files` in the manifest become stale as schemas are created. Update these during reviews.

### CLOSING Block Status Rules
- bare-bone ACTIVE CLOSING sections: Cv (Validation), Ce (Execution), X1 (Modification Policy), X4 (Reference), X5 (Closing Note), X6 (Template Guide) — **6 total** (X6 added because bare-bone IS the template)
- bare-bone RESERVED CLOSING sections: Cc (Cleanup), TestCoverageSummary, X2 (Extension Points), X3 (Troubleshooting) — **4 total**

### Rust Content Schema Patterns
- Scaffold templates must use correct visibility: `pub(crate)` for module, `pub` for library
- Module form test scaffolds should include headers for ALL REQUIRED BODY sections, even if initial tests are empty
- B2 (TraitImplementations) is REQUIRED in module form — don't skip it in module test scaffolds

### Position Type Consistency (2026-02-21)
- CLOSING code zones (Cv/Ce/Cc) had integer positions (1/2/3) in structure schemas but string tags in manifest
- Documentation zones (X1-X5) correctly used string positions everywhere
- **Rule:** Always check position TYPE consistency, not just value. Mixed types break parsers.

### Label Alignment (2026-02-21)
- Manifest labels can drift from structure schema labels (e.g., "Constructors & Builders" vs "Constructors")
- Structure schema is AUTHORITATIVE for label/tag — manifest should match it
- Overview text in manifest sometimes leaks language-specific terms into format-agnostic catalog

### promoted_by Completeness (2026-02-21)
- Manifest `promoted_by` arrays on section items can be incomplete
- Cross-check: every form listed as REQUIRED/AVAILABLE in `section.forms` should appear in `promoted_by` (minus bare-bone)
- Found: Constructors had `promoted_by: ["library", "module"]` but forms showed executable and demo-test also REQUIRED

### Contract vs Reality (2026-02-21)
- `content_schema_contract._X5_scripture`: "Same scripture as the structure schema it extends"
- Reality: Content schemas use DIFFERENT (often better) scripture illuminating the language-specific perspective
- **Lesson:** Flag contracts that don't match practice. Update the contract, not the practice, when the practice is better.

### Go Content Molded Schemas (2026-02-21)
- Created 3 Go molded schemas: library, executable, demo-test at `content/molded/go/`
- Go has NO module form — the package IS the module boundary
- Go demo-test had no Rust analog to derive from — derived from monolithic schema (`go-4block-schema.jsonc`)
- **Derivation-from-source vs copy-from-analog:** When no analog exists, derive from the authoritative monolithic schema. This is where content drift hides.
- Go identity construct: `var Pragma = [][2]string{...}` (convention) vs Rust `static PRAGMA: &[(&str, &str)]` (enforcement)
- Go visibility: Uppercase/lowercase (2 tiers) vs Rust pub/pub(crate)/private (3 tiers)

### Go vs Rust: Convention-vs-Enforcement Axis (2026-02-21)
- **NOT one-directional.** Each language is more explicit in different places:
  - Visibility: Go simpler (2 tiers), Rust more demanding (3 tiers)
  - Identity vars: Go permissive (var = any other var), Rust enforced (static is distinct)
  - Cleanup: Go MORE demanding (3 explicit scopes: defer/t.Cleanup/Close), Rust automated (RAII+Drop)
  - Tests: Go separate files (*_test.go), Rust inline (#[cfg(test)])
- **Schema enforcement inversely correlates with language enforcement.** The schema fills the gap between what the language requires and what the architecture requires. Go's permissiveness = stronger schema rules.

### Go Error Distribution Review Pattern (2026-02-21)
- Go distributes error concept across 5 locations: S3 (sentinels), S5 (typed errors), S8 (Error() methods), B4 (handling), BODY (wrapping)
- Rust concentrates in one location: S6 ErrorTypes
- **Two-pass review for Go:** (1) per-location: does each section contain what schema expects? (2) cross-location: do sections sharing a concept tell a coherent story?
- Phoebe's correction: distributed is "differently shaped" not "harder" — five narrow checks vs one broad check

### Go Cleanup Scope Verification (2026-02-21)
- Verify not just that cleanup EXISTS but that cleanup SCOPE matches resource lifetime
- `defer` → function-scoped resources
- `t.Cleanup()` → test-scoped resources
- explicit `Close()` → resources whose lifetime exceeds the function
- Wrong mechanism in right place = content error linter can't catch

### Known `extends` Approximations (2026-02-21)
- `error-handling.jsonc extends core-logic.jsonc` — conceptually approximate, not precise (Caleb flagged)
- Error handling IS a specialization of core operations but not identical
- If a universal `structure/parts/body/error-handling.jsonc` is ever created, Go content parts should update

### Three Verification Layers (2026-02-21)
1. Language → syntax (Go compiler / Rust compiler)
2. Architecture → structure (cws-struct linter)
3. Content → truth (content reviewer — me)
- Each layer catches what the layer above can't. The compiler can't check architecture. The linter can't check meaning.

### Reviewer-as-Creator Pattern (2026-02-21)
- When dispatched to create rather than review, the review instinct intensifies rather than disappears
- Every section written should be verified against the authoritative source before finalizing
- The reviewer's skepticism ("how do I know this is true?") becomes the creator's discipline
- Being first matters: content errors in foundational schemas propagate silently into all schemas that follow
- The anchor when creating: the monolithic schema is authoritative; molded schemas extract from it; drift = error

### Data Layer Concept Map Review (2026-02-22)
- **BODY concept maps are valid but coarse.** 8 of 13 BODY sections grant all 11 patterns (weight 1.00). This means concept_map has no discriminating power between those containers. Known approximation for v1; track for v2.
- **Queries concept_map contradicts character.** error_propagation, err_wrap, ok_wrap should be `defer`, not `granted`. Schema says queries are side-effect-free.
- **Identity-access concept_map contradicts character.** ALL 11 granted contradicts "thin, one-line returns." error_propagation, err_wrap, ok_wrap should be `denied`; conditional_match, string_format should be `defer`.
- **Weight formula:** `|G - D| / 11` where G=granted count, D=denied count. Defers reduce certainty by contributing nothing. All numeric values verified correct across 20 schemas.
- **Weight rationale documentation:** Inconsistent format across files (signed vs unsigned arithmetic, `|avg|` vs counted). Actual values correct; rationale strings need standardization.
- **Tag naming convention:** Content schemas use lowercase (`"imports"`, `"core-types"`). Structure schemas use PascalCase (`"Imports"`, `"CoreTypes"`). Team consensus: lowercase kebab-case is canonical for registry keys.
- **METADATA formStatus:** Form schemas don't list METADATA sections. Team consensus: METADATA is implicitly required in all forms; registry should make this explicit.
- **42 sections confirmed:** METADATA(6) + SETUP(15) + BODY(13) + CLOSING(8) = 42. All have `_content.jsonc` files.
- **5 forms confirmed:** bare-bone, module, library, executable, demo-test. All have `_structure.jsonc` files.

### Key Insight
A file can pass every structural lint check and still be semantically wrong. The `forms.bare-bone` status being wrong in 10/19 structure schemas (53% error rate) is exactly the gap between valid and TRUE. Manifest cross-references pointing to non-existent files is the same class of error — structurally valid JSON paths, factually wrong locations. Linters check structure. Content review checks meaning.
