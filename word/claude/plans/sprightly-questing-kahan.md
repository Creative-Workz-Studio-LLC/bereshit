# Deep Validation: Both Linters + Base Schema Tripwire

## Context

Templates and schemas are aligned. The linters haven't caught up. Both need the full treatment — deep validation of every block, tests proving they work, transformers that fix what they find, and base schemas completing the tripwire control layer.

**Current state:**

| Linter | Checks | Missing |
|--------|--------|---------|
| **Rust** (rust.ts, 1,489 lines, 11 checks) | Directives, block structure, doc comments, PRAGMA/METADATA I/C fields, comment metadata, separators, template vs derived, SETUP subsection ordering, identity registration, directive format, content placement (METADATA/SETUP/BODY) | subtype on context, BODY subsection ordering, CLOSING zone ordering, CLOSING content placement, tests |
| **TOML** (toml.ts, 2,257 lines, 7 check functions) | Schema-driven metadata I/C, content Cc/Co/Cv zones + ordering, closing X1-X5 + field validation + structural lock, consistency/pragma cross-check, field values, content body density + zone ordering, Cargo normalization | X section ordering in CLOSING, test infrastructure |

**The Tripwire (Seanje's insight — "the universal one gives the system a form of a control how"):**

| Layer | Role | State |
|-------|------|-------|
| **Primary** | Format-specific linter → format-specific schema | Working (rust.ts, toml.ts) |
| **Control** | Base schema → universal minimum | 3-block exists, **4-block missing** |
| **Fallback** | No handler → base schema validates minimum | Future (needs schema-loader changes) |

Base schemas ARE the control layer. Without base-4block, any 4-block file without a format handler gets zero validation.

## Critical Files

| File | Lines | Action |
|------|-------|--------|
| `c-tov/cws-struct/lib/formats/rust.ts` | 1,489 | Add subtype ctx, 2 new checks, expand 1, expand transformer |
| `c-tov/cws-struct/lib/formats/toml.ts` | 2,257 | Add X section ordering, expand transformer |
| `c-tov/cws-struct/tests/rust_test.ts` | **NEW** | Full test suite for Rust linter |
| `c-tov/cws-struct/tests/toml_test.ts` | **NEW** | Full test suite for TOML linter |
| `c-tov/cws-struct/tests/fixtures/` | **NEW** | Synthetic .rs and .toml files targeting specific conditions |
| `c-tov/cws-struct/tests/helpers.ts` | **NEW** | Shared test utilities |
| `b-word/L1-omnicode/ladder/schemas/base-4block-schema.jsonc` | **NEW** | Universal 4-block $defs |
| `b-word/L1-omnicode/ladder/schemas/base-3block-schema.jsonc` | ~37K | Update X2, verify alignment |
| `b-word/L1-omnicode/ladder/schemas/rust-4block-schema.jsonc` | ~1080 | Version bump, check list, base ref |

All paths relative to `.a-new-structure/`.

## Plan

### Phase 1: Test Infrastructure + Fixtures

**Why first:** Tests prove everything else works. Build the harness, create targeted fixtures, then write failing tests that pass as we implement each phase.

**Directory structure:**
```
c-tov/cws-struct/tests/
  helpers.ts            # Shared: lint helper, assertion helpers, fixture path resolver
  rust_test.ts          # Rust linter tests
  toml_test.ts          # TOML linter tests
  fixtures/
    rust/               # Synthetic .rs files
    toml/               # Synthetic .toml files
```

**Test pattern:** Import format modules for side-effect registration, use `getFormat("rust")?.lint(path)` to test through the CLI interface. `new URL(path, import.meta.url).pathname` for fixture resolution.

**helpers.ts:**
```typescript
import "../lib/formats/rust.ts";    // side-effect: register
import "../lib/formats/toml.ts";    // side-effect: register
import { getFormat } from "../lib/formats/registry.ts";

export async function lintRust(fixturePath: string) { ... }
export async function lintToml(fixturePath: string) { ... }
export function assertNoErrors(results: LintResult[]) { ... }
export function assertHasRule(results: LintResult[], rule: string) { ... }
export function fixturePath(format: string, name: string): string { ... }
```

**Rust fixtures:**

| Fixture | Tests |
|---------|-------|
| `valid-library.rs` | Happy path — full 4-block lib template (copy from seed, minimize) |
| `valid-executable.rs` | Happy path — executable template (copy from seed, minimize) |
| `missing-block.rs` | Missing METADATA entirely → block error |
| `missing-ic-fields.rs` | Has PRAGMA but missing I1.key, C1.version → field warnings |
| `wrong-block-order.rs` | SETUP before METADATA → order error |
| `no-omni.rs` | Plain .rs, no markers → info only |
| `placeholder-values.rs` | Template-style `[bracketed]` values → warning |
| `wrong-body-order.rs` | B3 before B1 → BODY ordering warning (Phase 2 target) |
| `wrong-closing-order.rs` | X1 before Cv → CLOSING zone error (Phase 3 target) |
| `tests-in-body.rs` | `#[cfg(test)]` in BODY block → error (Phase 3 target) |
| `main-in-body.rs` | `fn main()` in BODY → warning (Phase 4 target) |

**TOML fixtures:**

| Fixture | Tests |
|---------|-------|
| `valid-complete.toml` | Happy path — full 3-block with all required sections |
| `empty.toml` | Empty file → parse error, not crash |
| `parse-error.toml` | Invalid TOML syntax → parse error |
| `missing-metadata.toml` | No `[_metadata]` → error with fix suggestion |
| `bom-utf8.toml` | Valid TOML with UTF-8 BOM → works (BOM stripping test) |
| `bad-contract.toml` | `_contract.required_identity` declares phantom section |
| `wrong-x-order.toml` | X3 before X1 in CLOSING → ordering warning (Phase 5 target) |
| `zone-disorder.toml` | Co section before Cc section → zone ordering warning |

**Verify:** `deno test --allow-read tests/` runs. Tests targeting future phases fail as expected (marked with comments).

### Phase 2: Rust — Subtype Context + BODY Subsection Ordering (check 12)

**2a. Add `subtype` to RustFileContext** (line 232):
```typescript
subtype: string | null;  // "library" | "executable" | "demo-test" | null
```

In `buildContext()`, detect from:
1. Directive: `//omni:code --rust -library` → extract subtype after last `-`
2. Template directive: `#!omni template --rust -library` → same extraction
3. PRAGMA `I2.subtype` field (reuse `parseStaticFields`)
4. If none found → `null`

**2b. New function: `checkBodySubsectionOrder(ctx)`** — check 12.

Follows `checkSetupSubsectionOrder()` pattern (lines 1117-1165):

```typescript
const BODY_SUBSECTIONS: Record<string, Array<{ tag: string; pattern: RegExp }>> = {
  library: [
    { tag: "B1", pattern: /^\/\/\s+B1\s+[—\-]/ },
    // ... B2-B7
  ],
  executable: [/* B1-B3 */],
  "demo-test": [/* B1-B9 */],
};
```

Algorithm: get BODY lines → scan for B-markers → verify canonical order → out-of-order = `warn`.

Wire: `...checkBodySubsectionOrder(ctx)` after `checkSetupSubsectionOrder` in `lintRustFile()`.

**Tests pass:** `wrong-body-order.rs` fixture produces expected warning. Seed templates produce zero errors.

### Phase 3: Rust — CLOSING Zone Ordering (check 13)

**New function: `checkClosingZoneOrder(ctx)`**

Zone markers (from schema CLOSING section):
```typescript
const CLOSING_ZONES = [
  { tag: "Cv", pattern: /^\/\/\s+Cv\s+[—\-]/ },
  { tag: "Ce", pattern: /^\/\/\s+Ce\s+[—\-]/ },
  { tag: "Cc", pattern: /^\/\/\s+Cc\s+[—\-]/ },
  { tag: "X1", pattern: /^\/\/\s+X\.?1\s/ },
  { tag: "X2", pattern: /^\/\/\s+X\.?2\s/ },
  { tag: "X3", pattern: /^\/\/\s+X\.?3\s/ },
  { tag: "X4", pattern: /^\/\/\s+X\.?4\s/ },
  { tag: "X5", pattern: /^\/\/\s+X\.?5\s/ },
  { tag: "X6", pattern: /^\/\/\s+X\.?6\s/ },
];
```

Algorithm: get CLOSING lines → scan for zone markers → verify canonical order (Cv→Ce→Cc→X1→...→X6) → out-of-order = `error`.

**`#[cfg(test)]` placement check:**
- Scan ALL blocks for `#[cfg(test)]`
- If found in BODY or SETUP → `error` ("tests belong in CLOSING Cv, not {block}")
- If found in CLOSING but outside Cv zone boundaries → `error`

Wire: `...checkClosingZoneOrder(ctx)` after `checkBodySubsectionOrder` in `lintRustFile()`.

**Tests pass:** `wrong-closing-order.rs` and `tests-in-body.rs` produce expected errors. Seeds produce zero errors.

### Phase 4: Rust — CLOSING Content Placement (check 11 level 4)

Expand `checkContentPlacement()` with a 4th level.

**Level 4: CLOSING zone-specific placement.**
1. Parse zone boundaries within CLOSING (reuse zone scanning logic — extract to shared helper `findClosingZones()` used by both check 13 and check 11)
2. Specific construct checks:
   - `fn main()` in BODY → `warn` ("main() should be in CLOSING Ce")
   - `fn main()` in CLOSING but not in Ce zone → `warn`
   - `impl Drop` outside Cc zone → `info`
3. Don't duplicate `#[cfg(test)]` check (already in check 13)

**Extract shared helper:** `findClosingZones(closingLines)` returns `Array<{ tag, startIdx, endIdx }>` — used by both check 13 and check 11 level 4.

**Tests pass:** `main-in-body.rs` produces expected warning. Seeds produce zero errors.

### Phase 5: TOML — X Section Ordering in CLOSING

**New check in `checkClosing()` (toml.ts line 733).**

TOML closing sections should follow X ordering: X1 → X2 → X3 → X4 → X5 → X6.

Currently `checkClosing` checks presence and field validation but NOT ordering. Add ordering check using the text-based `findSectionPositions()` helper (already exists, used by `checkContentBody`).

**Implementation:**
1. After existing closing checks, scan for `_closing.X{N}_` table positions in text
2. Verify X sections appear in numerical order
3. Out-of-order → `warn` (TOML tables can technically be in any order, but convention matters)

**Also consider:** Add closing zone ordering to `checkContentBody` model — the same `findSectionPositions` + classify approach works for closing sections.

**Tests pass:** `wrong-x-order.toml` produces expected warning. TOML seed templates produce zero errors.

### Phase 6: Rust Transformer Expansion

Current transformer fixes: separator widths only (2 transforms).

**Add new transforms:**

1. **Fix BODY subsection marker format** — if a B-marker exists but doesn't match the canonical pattern (e.g., `// B1:` instead of `// B1 —`), rewrite to canonical
2. **Fix CLOSING zone marker format** — if zone markers use wrong dash style (ASCII `-` vs Unicode `—`), normalize
3. **Scaffold missing CLOSING zones** — if CLOSING block exists but has no Cv/Ce/Cc markers, insert commented-out zone scaffolding (like `// Cv — Closing Validation`)

These are safe transforms (comment markers, not code).

### Phase 7: TOML Transformer Review

The TOML transformer is already schema-driven with FixSuggestion-based corrections. Review:

1. Does every lint error that emits a `fix` actually produce a correct correction?
2. Are there lint results that COULD have a fix but don't?
3. Missing fix suggestions for newly added checks (X ordering has no fix — info only)

Document findings. Add fixes where straightforward.

### Phase 8: Create base-4block-schema.jsonc

Universal patterns extracted from rust-4block-schema.jsonc:

| $def | What It Defines |
|------|----------------|
| `block_ordering` | 4 blocks: METADATA → SETUP → BODY → CLOSING, each with boundary/label/end_marker |
| `setup_zones` | Sc (configuration), So (operations), Sv (validation) — universal setup zone concept |
| `body_zones` | Numbered subsection ordering concept (B1, B2, ...) — subtype-driven count |
| `closing_code_zones` | Cv (validation/tests), Ce (execution/entry), Cc (cleanup/teardown) |
| `closing_docs` | X1-X6 documentation sections with required flags (X1 + X5 required) |
| `zone_ordering` | Code zones precede documentation sections in CLOSING |
| `placement_rules` | Universal categories: declarations→SETUP, logic→BODY, tests→CLOSING |
| `identity` | I1-I4 Identity, C1-C7 Context (shared with base-3block) |

**Structure:** 3-block JSONC (METADATA → CONTENT → CLOSING), `$defs` only, version a-01.00.

### Phase 9: Update Schemas

**rust-4block-schema.jsonc:**
1. Add `"_base_schema": "base-4block-schema.jsonc"` reference
2. Update checks list: check 12 actual (not planned), check 13 new
3. Update check 11 description for CLOSING awareness
4. Version bump: `_P1_at` and `_M2_state.version` → `b-02.50`
5. Update CLOSING section version in linter description

**base-3block-schema.jsonc:**
1. X2 planned: mark "4-block base schema" as delivered (a-01.00)
2. Verify zone patterns match what TOML linter enforces
3. Version bump if changes made

### Phase 10: Integration Verification

**Full test suite:**
```bash
cd .a-new-structure/c-tov/cws-struct
deno check mod.ts                        # Type checking
deno test --allow-read tests/            # All unit tests
```

**Rust seed templates (zero errors):**
```bash
deno run --allow-read mod.ts lint rust .a-new-structure/b-word/seed/code/L0/rust/library.rs
deno run --allow-read mod.ts lint rust .a-new-structure/b-word/seed/code/L0/rust/executable.rs
deno run --allow-read mod.ts lint rust .a-new-structure/b-word/seed/code/L0/rust/demo-test.rs
```

**TOML seed templates (zero errors):**
```bash
deno run --allow-read mod.ts lint toml .a-new-structure/b-word/seed/data/L0/
```

**Schema validation:**
```bash
# Verify all JSONC parses clean
node -e "const fs=require('fs'); /* proper JSONC parse */ ..."
```

**Transform dry-run:**
```bash
deno run --allow-read mod.ts transform rust --dry-run .a-new-structure/b-word/seed/code/L0/rust/
deno run --allow-read mod.ts transform toml --dry-run .a-new-structure/b-word/seed/data/L0/
```
