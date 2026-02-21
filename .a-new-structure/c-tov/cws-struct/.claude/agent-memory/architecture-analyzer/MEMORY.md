# Ezra Matthan — Architecture Analyzer Memory

## Schema Engine Architecture (2026-02-21)

### Two-Sided Schema System
- **Structure** (format-agnostic shapes): `blocks/{block}/{section}.jsonc`
  - Contains: position, tag, label, can/cannot/defer contracts, form availability, header rules
- **Content** (language-specific fills): `format/blocks/{lang}/{block}/{section}.jsonc`
  - Contains: constructs, patterns, defer_signals (regex), scaffold templates
  - Uses `"extends"` field pointing to the structure schema

### Key Files
- **Manifest**: `.a-new-structure/b-word/L1-omnicode/ladder/schemas/code/blocks/manifest.jsonc`
- **Base schema**: `schemas/code/base/base-4block-schema.jsonc` (1339 lines, comprehensive)
- **Example structure**: `blocks/body/core-logic.jsonc`
- **Example content**: `format/blocks/rust/body/core-logic.jsonc`
- **Rich form schema**: `forms/declared/rust-module.jsonc` (1164 lines)

### Section Counts
- SETUP: 10 sections (S1-S10, dependency chain ordering)
- BODY: 17 sections (B1-B17, ordering varies by form)
- CLOSING: 8 sections (Cv/Ce/Cc code zones + X1-X5 doc sections)
- Total: 35 sections needing structure schemas + 35 per language needing content schemas

### Merge Rules (Asymmetric)
- Structure provides: can (concepts), cannot (concepts), defer (signals), header rules, form availability
- Content provides: can (constructs), cannot (constructs), defer_signals (regex), scaffold (templates)
- Content MUST NOT contradict structure can or weaken structure cannot
- Scaffold and patterns are content-only; headers and forms are structure-only

### Form Resolution Order
1. FORM — Which sections active? What ordering?
2. STRUCTURE — Abstract contracts per section
3. CONTENT — Language-specific constructs and templates
4. MERGE — Structure + Content = complete validation rules
