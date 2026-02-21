# Phoebe Karis — Format Bridge Agent Memory

## Schema Engine Architecture (2026-02-21)

### Two-Sided Schema Design
- **Structure schemas** (format-agnostic): `blocks/{setup,body,closing}/*.jsonc` — define SHAPES
- **Content schemas** (language-specific): `format/blocks/{lang}/{block}/*.jsonc` — define FILLS
- Structure = what each section IS conceptually
- Content = how each section manifests in a specific language
- Composition: Structure(shape) + Content(fill) = real code validation

### Schema Pattern
Each structure schema follows this shape:
```
pragma → section { tag, position, label, block, overview_text, header, can[], cannot[], defer[], forms{}, ro{} } → closing
```
- `can`: 2-4 abstract concepts that belong (with distinguisher)
- `cannot`: 3-6 concepts that don't belong (with defer_to targets)
- `defer`: redirect rules (when meaningful)
- `forms`: per-form availability (REQUIRED/AVAILABLE/RESERVED)
- `ro`: reserved omission template data

### Form Availability — Bare-Bone Is Skeletal
Team lead corrected my initial values — bare-bone RESERVES far more than expected:
- SETUP: S1 (REQUIRED), S2 (RESERVED), S3 (REQUIRED), S4 (RESERVED), S5 (RESERVED), S6 (REQUIRED), S7 (REQUIRED), S8 (RESERVED), S9 (RESERVED), S10 (RESERVED)
- BODY: B1 (REQUIRED), B2 (RESERVED), B3 (RESERVED), B4 (REQUIRED), B5 (RESERVED), B6 (RESERVED), B7 (REQUIRED), B8 (RESERVED), B9-B17 (RESERVED)
- CLOSING: Cv (REQUIRED), Ce (REQUIRED), Cc (RESERVED), X1 (REQUIRED), X2 (RESERVED), X3 (RESERVED), X4 (REQUIRED), X5 (REQUIRED)

### Key Insight: cannot + defer_to = Routing Network
The `cannot` lists with `defer_to` targets create a code placement routing system. Misplaced code gets actively redirected to the correct section. The schema network IS the placement engine.

### File Locations
- Structure schemas: `.a-new-structure/b-word/L1-omnicode/ladder/schemas/code/blocks/`
- Content schemas: `.a-new-structure/b-word/L1-omnicode/ladder/schemas/code/format/blocks/`
- Form schemas: `.a-new-structure/b-word/L1-omnicode/ladder/schemas/code/forms/`
- Reference pattern: `blocks/body/core-logic.jsonc` (the template all structure schemas follow)
