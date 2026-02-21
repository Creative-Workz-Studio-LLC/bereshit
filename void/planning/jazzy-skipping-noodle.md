# Form-Aware Linter Upgrade

## Context

We now have **5 form schemas** (bare-bone + 4 declared) that define exactly what CAN and CANNOT be in each container for each Rust form. The linter currently hardcodes only 3 subtypes (library, executable, demo-test) and has no concept of form constraints. The form schemas ARE the validation rules — schema = template = linter.

**The bare-bone schema defines the floor.** Every form inherits from it and promotes sections. The declared form schemas define what's required, available, and reserved per container. The linter should load these and validate against them.

**Missing subtypes:** `module` (just built) and `bare-bone` (the foundation) are valid forms with schemas and templates, but the linter rejects them as unknown.

---

## Phase 1: Make Subtypes Dynamic (Unblock module + bare-bone)

### 1a. Format schema — add module + bare-bone subtypes

**File:** `schemas/code/format/rust-4block-schema.jsonc`

- Add `module` and `bare_bone` entries to `METADATA.subtypes` (alongside library/executable/demo_test)
- Add `module` BODY subsection_order (B1-B5, B7 — 6 sections, type_lifecycle)
- Add `bare_bone` BODY subsection_order (B1, B4, B7 — 3 sections, universal)
- Add `module` and `bare_bone` to SETUP and BODY `subtype_emphasis`
- Update `file_classification.template.subtypes` array to include `--rust -module` and `--rust -bare-bone`
- Update `directives.recommended["//omni:code"].known_values` to include the two new directives

### 1b. code-schema.ts — dynamic subtype extraction

**File:** `lib/foundation/code-schema.ts`

Three functions currently loop over `["library", "executable", "demo_test"]`. Change all three to read keys dynamically from the schema:

| Function | Line | Change |
|----------|------|--------|
| `extractBodySubtypes()` | 410 | `Object.keys(subOrder)` instead of hardcoded list; filter out non-object/metadata keys |
| `extractSubtypeEmphasis()` | 495 | `Object.keys(emphasis)` instead of hardcoded list |
| `extractSubtypeDefs()` | 718 | `Object.keys(subtypes)` instead of hardcoded list |

Key normalization stays: `replace(/_/g, "-")` converts `demo_test` to `demo-test`, `bare_bone` to `bare-bone`.

### 1c. rust.ts — derive KNOWN_SUBTYPES from schema

**File:** `lib/handlers/rust.ts`

- **Line 438** (`buildContext`): Replace `const KNOWN_SUBTYPES = ["library", "executable", "demo-test"]` with `Object.keys(_rustRules!.subtypeDefinitions)` (rules are already loaded by `ensureRustRules()` before buildContext runs)
- **Line 829** (`checkPragmaMetadata`): Same change
- **Lines 140-142** (`KNOWN_CODE_DIRECTIVES`): Add `"--rust -module"` and `"--rust -bare-bone"`

### 1d. Tests

- Add `tests/fixtures/rust/structure/valid-module.rs` — minimal module file with `//omni:code --rust -module`
- Add test: `structure/valid-module: zero errors`
- Confirm existing `metadata/bad-content-values` test still catches truly unknown subtypes

---

## Phase 2: Form Schema Loader

### 2a. New types in code-schema.ts

```typescript
export interface FormSectionConstraint {
  position: number;
  tag: string;
  status: "REQUIRED" | "AVAILABLE";
  contentConstraints?: string;  // brief summary
}

export interface FormReservedSection {
  tag: string;
  why_reserved: string;
}

export interface FormContainerConstraints {
  can: FormSectionConstraint[];
  cannot: FormReservedSection[];
}

export interface FormConstraints {
  name: string;
  isFoundation: boolean;
  inherits?: string;
  ordering: string;
  SETUP: FormContainerConstraints;
  BODY: FormContainerConstraints;
  CLOSING: FormContainerConstraints;
}
```

### 2b. New functions in code-schema.ts

- `parseFormSchema(jsonText: string): FormConstraints` — pure function, extracts from `form_identity` + `containers`
- `loadFormConstraints(subtype: string): Promise<FormConstraints | null>` — I/O wrapper, uses schema pipeline, caches per-subtype

Schema ID mapping:
- `bare-bone` -> `code/forms/bare-bone/rust-bare-bone.jsonc`
- All others -> `code/forms/declared/rust-{subtype}.jsonc`

Returns `null` gracefully if schema not found (forward-compatible).

### 2c. Integrate into RustFileContext

Add `formConstraints: FormConstraints | null` to `RustFileContext`. Populate in `buildContext()` after subtype detection.

---

## Phase 3: Form-Aware Checks

Three new check functions in `rust.ts`, all gated on `ctx.formConstraints !== null`:

### Check A: `checkFormRequiredSections`

Rule: `form/required-section-missing` (severity: warn)

For each container (SETUP, BODY, CLOSING): iterate `formConstraints.can` entries where `status == "REQUIRED"`. Check if that section tag is present in the file's actual subsections. If absent, warn with the tag and form name.

Uses existing `SETUP_SUBSECTIONS` regex patterns and `BODY_SUBSECTION_PATTERN` for tag matching.

### Check B: `checkFormReservedSections`

Rule: `form/reserved-section-present` (severity: info)

For each container: iterate `formConstraints.cannot` entries. Check if that section tag appears in the file. If present, emit info with `why_reserved` from the form schema.

This is the key check — module files with S2 (Modules) content or B6 (Output) content get flagged. Library files with test subsections (B8-B16) get flagged.

### Check C: `checkFormCompleteness`

Rule: `form/completeness-violation` (severity: info)

Count: `can.length + cannot.length` should equal expected total per container. If the file has sections that aren't in either `can` or `cannot`, flag as unexpected.

### Integration

Add to `lintRustFile()` orchestrator after existing body checks:

```typescript
// Check 17: Form-aware validation
if (ctx.formConstraints) {
  results.push(...checkFormRequiredSections(ctx));
  results.push(...checkFormReservedSections(ctx));
  results.push(...checkFormCompleteness(ctx));
}
```

---

## Phase 4: Tests

### New fixtures

```
tests/fixtures/rust/form/
  module-has-reserved-section.rs   -- module with S2 Modules (reserved)
  module-missing-required.rs       -- module missing S1 Imports
  library-has-test-section.rs      -- library with B8 tests (reserved)
```

### New test cases (~8)

- `form/module-valid: no form violations`
- `form/module-reserved-section: info on S2 in module`
- `form/module-missing-required: warn on missing S1`
- `form/library-reserved-body: info on test sections in library`
- `form/bare-bone-valid: no form violations on template`
- `form/unknown-subtype: graceful no-op when form schema missing`

### Target: 232 existing + ~12 new = ~244 tests

---

## Critical Files

| File | Changes |
|------|---------|
| `schemas/code/format/rust-4block-schema.jsonc` | Add module + bare_bone to subtypes, BODY order, emphasis |
| `lib/foundation/code-schema.ts` | Dynamic subtype loops + FormConstraints types + parseFormSchema + loadFormConstraints |
| `lib/foundation/mod.ts` | Export new types and functions |
| `lib/handlers/rust.ts` | Dynamic KNOWN_SUBTYPES + 3 new form checks + formConstraints in context |
| `tests/handlers/rust_test.ts` | New form-aware test group |
| `tests/fixtures/rust/` | New structure/valid-module.rs + form/ directory |

## Verification

1. `deno task test` — all 232 existing + new tests pass
2. `deno task lint` — zero lint errors
3. Manual: `deno run --allow-read --allow-run mod.ts lint rust path/to/module.rs` — no "Unknown subtype" warning
4. Manual: create a module.rs with S2 Modules section populated — linter reports `form/reserved-section-present`
5. Manual: create a module.rs missing S1 Imports — linter reports `form/required-section-missing`
