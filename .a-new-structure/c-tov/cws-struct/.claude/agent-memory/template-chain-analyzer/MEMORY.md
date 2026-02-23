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

### Section Counts (per filesystem — manifest is STALE)
- METADATA: 6 sections (directives, doc-comments, comment-block, pragma-identity, metadata-context, subtypes)
- SETUP: 15 sections (imports, constants, core-types, error-types, variables, modules, statics, type-aliases, trait-defs, interface-defs, macros, feature-gates, build-tags, code-generation, type-methods)
- BODY: 13 sections (helpers, identity-access, trait-implementations, constructors, core-logic, core-operations, queries, output-display, free-functions, error-handling, org-chart, public-apis, tests)
- CLOSING: 8 sections (validation, execution, cleanup, modification-policy, extension-points, reference, troubleshooting, closing-note)
- **TOTAL: 42** (6+15+13+8)
- NOTE: Old manifest said SETUP=10, BODY=17, CLOSING=8 — these are outdated

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

### Critical Schema Contradictions (2026-02-22)
- **BODY section/form mismatch:** 4 BODY sections (OrgChart@0, CoreOperations@20, ErrorHandling@21, PublicAPIs@22) declare REQUIRED in all forms via `section.forms`, but NO form schema lists them
- **CLOSING section/form mismatch:** 2 CLOSING sections (X6/TemplateGuide, TestCoverageSummary) listed in bare-bone but have NO schema dirs on disk
- **BODY.tests multi-section:** One schema dir, 9 `subsections[]` entries (positions 9-17). Forms list individual test tags. Builder must expand 1->9.
- **Position polymorphism:** CLOSING code=numeric (Cv=1,Ce=2,Cc=3), doc=string ("X1"-"X5"). SectionEntry.position typed `number`.
- **BODY gaps:** 0-8, 9-17, then 20-22. Gap at 18-19. OrgChart at 0 (not 1-based).
- **"42" count ambiguous:** 42 dirs, but tests expand to 9, 4 BODY sections not in forms, 2 CLOSING have no dirs

### Data Layer Chain (2026-02-22)
- **Chain:** Schema (_structure + _content) -> types.ts (SectionEntry, FormDefinition) -> registries (sections.ts, forms.ts)
- **SectionEntry needs BOTH files:** tag/block/position/formStatus/conceptMap/detectionWeight from _structure; nature/description from _content
- **Missing type fields:** `overviewText` (from _structure.section.overview_text), `ordering` on FormDefinition (from form.{BLOCK}.ordering + ordering_sequence)
- **Transform rules:** UPPERCASE->lowercase for formStatus, bare-bone `active`->`required`, PascalCase tags->lowercase kebab-case for lookup keys
- **Tag naming:** _structure uses PascalCase ("Imports"), _content uses lowercase ("imports"), R5 uses lowercase. Consensus: lowercase kebab-case canonical.
- **METADATA sections:** Not listed in form schemas but implicitly required in all forms — registry should make explicit
- **subContainers:** Not a schema field — filesystem discovery (pragma-identity/i1-i4, metadata-context/c1-c7)
- **Routing (future R[10]):** `cannot[].defer_to` creates a correction network distinct from conceptMap detection

### Key Paths
- Base: `.a-new-structure/b-word/L1-omnicode/ladder/schemas/code/`
- Base 4-block: `base/base-4block-schema.jsonc`
- Structure: `blocks/{setup,body,closing}/`
- Content: `format/blocks/rust/{setup,body,closing}/`
- Forms: `forms/{bare-bone,declared}/`
