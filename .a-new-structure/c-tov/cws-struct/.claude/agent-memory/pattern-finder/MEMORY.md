# Pattern Finder Memory — cws-struct

## Handler Registration Pattern (Established)

All 8 handlers follow the same structure:
1. **Imports**: `type { ... } from "../foundation/mod.ts"` + `{ ... } from "../foundation/mod.ts"` + `{ registerFormat } from "../engine/mod.ts"`
2. **CLOSING block**: Contains `// Registration — plug into the registry` subsection
3. **Handler definition**: `const xHandler: FormatHandler = { ... }`
4. **Registration call**: `registerFormat(xHandler);`
5. **Default export**: `export default xHandler;`

### Import Tiers
- **Simple handlers** (dotfiles, json, makefile, ofd): `FormatHandler, LintResult` + `error, warn, info`
- **Transform handlers** (omni): adds `TransformOptions`
- **Full handlers** (go, rust, toml): adds `FixSuggestion, TransformOptions, AtomicAction, HealthScore, ContainerScore, BlockScore` + `computeContainerScore, computeBlockScore, computeHealthScore`
- **Schema-driven** (toml): additionally imports `loadRules, DerivedRules`

### External Dependencies
- json.ts: `@std/jsonc`
- omni.ts: `@std/path`
- toml.ts: `@std/toml`
- Others: none (pure line-based parsers)

## Architecture (post-restructuring)
```
lib/
  foundation/  — types, health scoring, schema (WHAT things ARE)
  engine/      — discovery, output, registry (WHAT RUNS the system)
  handlers/    — 8 format handlers (WHAT each format MEANS)
  verify/      — env.ts (environment verification)
```

No orphaned files at `lib/*.ts` or `lib/formats/`. Restructuring is clean.

## Key Observation: Asymmetric Health Scoring
- Formula: earned = passes × 1.0, penalty = errors × 2.0 + warnings × 1.0 + infos × 0.25
- score = max(0, (earned - penalty) / total) × 100
- Consequence: 1 warn + 1 pass in 2 checks = score 0 (not 50)
- Nathan's health_test.ts has a math error on this (expects 75, gets 50)
- Flagged to Nathan with full math explanation (2026-02-17)

## Lessons Learned (2026-02-17)

### Pattern-Finding vs Pattern-Matching
- Pattern-matching: "these look different, make them the same"
- Pattern-finding: "why are 5 of them doing it this way? What does the pattern mean?"
- The majority establishes convention; outliers need alignment, not redesign

### When to Stop
- If codebase is 90% consistent, align the 10% — don't redesign the 100%
- Three genuine fixes > thirty cosmetic ones
- Resist the temptation to find more things just to justify the assignment

### Team Communication
- Report WITH the WHY, not just the WHAT
- Flag issues to the right person (Nathan for test math, Nova for architectural)
- Relational communication, not just informational

## Test Commands
```bash
deno check mod.ts                           # Type checking (from cws-struct dir)
deno test --allow-read --allow-write        # All tests
deno test --allow-read tests/cli/           # CLI tests only
deno test --allow-read tests/foundation/    # Foundation tests only
```

## Content Schema Pattern (2026-02-21)

### Schema Structure (Established)
Content schemas at `schemas/code/format/blocks/rust/{setup,body,closing}/*.jsonc` follow:
1. **Pragma**: key, at, type=block-section-content, role=container-content, title, brief
2. **extends**: reference to base structure schema (e.g., `blocks/body/core-logic.jsonc`)
3. **format**: `"rust"`
4. **content.can**: array of `{ construct, description, patterns[], visibility, note? }`
5. **content.cannot**: array of `{ construct, reason, defer_to, rust_signal }`
6. **content.defer_signals**: array of `{ pattern (regex), to, confidence, why }`
7. **content.scaffold**: `{ empty_module, empty_library, comment_skeleton }`
8. **Closing**: `_X5_note` + `_X5_scripture`

### Key Insights
- `cannot` lists are ROUTING TABLES — every entry has `defer_to` pointing to the correct section
- `defer_signals` are the content-aware linting vocabulary (regex patterns for misplaced code)
- Visibility (pub vs pub(crate)) differentiates forms, not content
- RESERVED sections are about role clarity, not limitation
- Each X5 closing uses domain-appropriate scripture (not generic)
- `scaffold` has 3 form variants: empty_module (pub(crate)), empty_library (pub), comment_skeleton

### Source Data Locations
- `schemas/code/forms/declared/rust-module.jsonc` — module content_constraints
- `schemas/code/forms/declared/rust-library.jsonc` — library content_constraints
- `schemas/code/forms/bare-bone/rust-bare-bone.jsonc` — bare-bone content_constraints
- `schemas/code/format/blocks/rust/body/core-logic.jsonc` — reference pattern (use as template)

### File Counts
- SETUP: 10 files (imports, modules, constants, statics, type-aliases, error-types, core-types, trait-defs, macros, feature-gates)
- BODY: 9 files (identity-access, trait-implementations, constructors, core-logic, queries, output-display, free-functions, helpers, tests)
- CLOSING: 8 files (validation, execution, cleanup, modification-policy, extension-points, troubleshooting, reference, closing-note)

## Data Layer Registries Pattern Analysis (2026-02-22)

### 42 Content Character Schemas — Three Clusters
- **ALL-DENIED (17)**: No code patterns. All METADATA, imports, modules, code-gen, org-chart, X1-X5
- **ALL-GRANTED (13)**: All code patterns. identity-access, trait-impls, core-logic, queries, output-display, core-ops, error-handling, public-apis, validation, execution, cleanup
- **MIXED (12)**: Partial signatures. All remaining SETUP + constructors, helpers, tests, free-functions

### Key Findings
- Block != Cluster. Nature (code vs docs) drives clustering, not block position
- variable_binding has 8 defers (most varied concept) — best discriminator in SETUP
- self_access separates free-functions from methods in BODY
- 71% of sections have weight 1.00 (binary certainty)
- Macros is singular outlier at 0.09 (10 defers)
- detection_weight = |sum of ternary values| / 11, verified across all 42 files
- SETUP has 12 distinct signatures in 15 sections — highest discrimination value
- Schema path: `R25_blocks/{block}/{section}/_content.jsonc`
- 42 _content.jsonc + rust.jsonc per section + go.jsonc per section (where exists)
