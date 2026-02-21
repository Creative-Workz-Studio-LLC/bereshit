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
- bare-bone REQUIRED CLOSING sections: Cv (Validation), Ce (Execution), X1 (Modification Policy), X4 (Reference), X5 (Closing Note)
- bare-bone RESERVED CLOSING sections: Cc (Cleanup), X2 (Extension Points), X3 (Troubleshooting)

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

### Key Insight
A file can pass every structural lint check and still be semantically wrong. The `forms.bare-bone` status being wrong in 10/19 structure schemas (53% error rate) is exactly the gap between valid and TRUE. Manifest cross-references pointing to non-existent files is the same class of error — structurally valid JSON paths, factually wrong locations. Linters check structure. Content review checks meaning.
