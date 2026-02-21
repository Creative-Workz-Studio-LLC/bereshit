# Selah Adair — Agent Memory

## Schema Engine Architecture (2026-02-21)

### Two-Sided Schema Pattern
- **Structure:** `blocks/{block}/{section}.jsonc` — format-agnostic shapes (WHAT belongs where)
- **Content:** `format/blocks/{lang}/{block}/{section}.jsonc` — language-specific fills (HOW it manifests)
- **Forms:** `forms/{type}/{lang}-{form}.jsonc` — compose structure + content into file specs
- **Manifest:** `blocks/manifest.jsonc` — engine root, section catalog, composition rules

### Pragma Contracts
- Structure: `_P2_subtype: "block-section-schema"`, `_P2_role: "container-structure"`
- Content: `_P2_subtype: "block-section-content"`, `_P2_role: "container-content"`
- Content must have `"extends"` pointing to structure and `"format"` matching language

### Section Counts (per manifest)
- SETUP: 10 sections (S1-S10, dependency chain order)
- BODY: 17 sections (B1-B17, varies by form)
- CLOSING: 8 sections (3 code zones Cv/Ce/Cc + 5 doc sections X1-X5)

### Verification Checklist
1. Extends path resolves to real structure file
2. Pragma subtype/role matches contract
3. Positions consistent between manifest, structure, and content
4. Form availability declared for all 5 forms
5. defer_to references point to valid section tags
6. Manifest structure_schema paths match actual filenames

### Known Issue: Manifest Naming Mismatch
- Manifest uses `x1-policy.jsonc`, `x2-extension.jsonc`, etc.
- Actual files: `modification-policy.jsonc`, `extension-points.jsonc`, etc.
- Needs reconciliation (update manifest to match files)

### Key Paths
- Base: `.a-new-structure/b-word/L1-omnicode/ladder/schemas/code/`
- Base 4-block: `base/base-4block-schema.jsonc`
- Structure: `blocks/{setup,body,closing}/`
- Content: `format/blocks/rust/{setup,body,closing}/`
- Forms: `forms/{bare-bone,declared}/`
