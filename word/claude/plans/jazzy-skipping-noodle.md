# TOML Form-Aware Linter + Cargo.toml Validation

> *"Diverse weights, and diverse measures, both of them are alike abomination to the LORD."* — Proverbs 20:10

## Context

**Where we are:** The Rust form-aware linter upgrade is complete (405 tests, Phases 1-4 done). We discovered a critical flow gap: a file with a pragma but no 4-block structure scored 92/100 instead of failing. Fixed it — pragma = claim = must deliver. Now applying the same layer chain thinking to TOML.

**Why TOML next:** Before we can properly align the Rust files in `spec/config/` (the Phase A/B plan), we need the Cargo.toml linter to enforce the same standard. Can't lint `exists.rs` as a module until `Cargo.toml` defines what that crate IS.

**The layer chain for TOML:**
```
File Type (data) → Data Type (toml) → Derivation (cargo) → Form (library/executable/test)
```

**Current state of TOML handler:**
- 58 tests, 13 fixtures, comprehensive unit coverage
- Pragma args classified into forms vs derivations (INFO only — no enforcement)
- Cargo derivation has layout/normalization (`normalizeCargoData()`)
- `DerivedRules.pragmaTaxonomy` has `knownForms` and `derivationLayouts`
- **No form-aware checks** — all forms treated identically
- **No Cargo.toml integration fixtures** — unit tests only for normalization
- **No `data/forms/` schema directory** — unlike `code/forms/` for Rust

---

## Phase 1: Cargo.toml Integration Fixture + Baseline Test

Before adding form checks, establish a baseline: what does the linter say about a real Cargo.toml right now?

### 1.1: Create Cargo.toml Integration Fixture

Create `tests/fixtures/toml/cargo/valid-library.toml` — a complete, well-formed Cargo.toml for a library crate with full `package.metadata.omni` (I1-I4, C1-C7, X1-X5).

**Pattern from:** `tests/fixtures/toml/structure/valid-complete.toml` (for OmniCode metadata structure) + the real `spec/config/Cargo.toml` (for Cargo sections).

### 1.2: Baseline Integration Test

Add test: `cargo/valid-library: lints a well-formed library Cargo.toml with zero errors`.

This establishes the ground truth before form changes.

---

## Phase 2: Form Schema Infrastructure for TOML

### 2.1: Create `data/forms/` Schema Directory

Mirror the Rust pattern:
```
schemas/data/forms/
├── bare-bone/
│   └── toml-bare-bone.jsonc          # 3-block minimum: _metadata + _content + _closing
└── declared/
    └── toml-cargo-library.jsonc      # Cargo library: [package] + [lib] + [dependencies] + OmniCode metadata
```

**Bare-bone for TOML 3-block:**
- _metadata block with I1-I3 minimum (REQUIRED)
- _content block with at least [identity] (Cc.1 REQUIRED) + [_validation] (Cv.1 REQUIRED)
- _closing block with X1 + X5 (REQUIRED)
- Reserved Omission: acknowledged for completeness

**Cargo library form:**
- Inherits bare-bone
- REQUIRED Cargo sections: `[package]`
- REQUIRED OmniCode metadata: full I1-I4, C1-C7 (Cargo.toml IS the crate identity)
- DEFINED Cargo sections: `[dependencies]`, `[dev-dependencies]`, `[lints]`
- RESERVED: `[[bin]]` (not a library), `[workspace]` (not a workspace root — unless it is)

### 2.2: Extend `parseFormSchema` or Create `parseTomlFormSchema`

The Rust `parseFormSchema` operates on 4-block containers (SETUP/BODY/CLOSING). TOML needs 3-block containers (METADATA/CONTENT/CLOSING) — or more precisely, Cargo needs host-format sections + OmniCode metadata sections.

**Decision:** Create a new `parseTomlFormSchema()` in `schema.ts` (or a new `data-schema.ts`) that understands:
- `required_cargo_sections`: `["package"]`
- `defined_cargo_sections`: `["dependencies", "dev-dependencies", "lints"]`
- `reserved_cargo_sections`: `[["bin"]]` with `why_reserved`
- `required_omni_sections`: I1-I4, C1-C7 (which sections of `package.metadata.omni` must exist)

**Alternatively:** Extend the existing `DerivationLayout` — it already has `requiredSections` and `definedSections`. Add `reservedSections` to the interface and schema.

**Recommended approach:** Extend `DerivationLayout` with `reservedSections` + form overlays. The derivation defines the structural layout. The form defines expectations within that layout. This keeps the hierarchy clean:

```
DerivationLayout (cargo) — structural: normalization + required/defined/reserved host sections
  + FormOverlay (library) — expectation: which additional sections expected/reserved for this form
```

### 2.3: Load Form Constraints in TOML Handler

Add to `lintTomlFile()`:
```typescript
// After pragma parsing, if this is a Cargo file with a form arg:
const form = pragma?.args.find(a => rules.pragmaTaxonomy.knownForms.has(a));
const formConstraints = form ? await loadTomlFormConstraints("cargo", form) : null;
```

---

## Phase 3: Form-Aware Checks for TOML

### 3.1: `checkCargoFormRequired()`

For Cargo.toml with a form declared:
- Check that form-required sections exist
- Example: A library Cargo.toml should have `package.metadata.omni` with all required I/C sections

### 3.2: `checkCargoFormReserved()`

For Cargo.toml with a form declared:
- Check that form-reserved sections are NOT present
- Example: A library Cargo.toml should NOT have `[[bin]]`

### 3.3: Wire Into Orchestrator

Add form-aware checks to the Cargo branch of `lintTomlFile()`:
```typescript
if (isCargo) {
  return [
    ...checkMetadata(filePath, lintData, rules, lineMap),
    ...checkCargoContent(filePath, data, rules, lineMap),
    ...checkClosing(filePath, lintData, rules, lineMap),
    ...checkConsistency(filePath, lintData, rules, pragma, lineMap),
    ...checkFieldValues(filePath, lintData, rules, pragma, lineMap),
    // NEW: Form-aware validation
    ...(formConstraints ? checkCargoFormRequired(filePath, data, formConstraints) : []),
    ...(formConstraints ? checkCargoFormReserved(filePath, data, formConstraints) : []),
  ];
}
```

---

## Phase 4: Test Fixtures + Tests

### 4.1: Additional Fixtures

| Fixture | Purpose |
|---------|---------|
| `cargo/valid-library.toml` | Happy path — library Cargo.toml with full OmniCode (Phase 1) |
| `cargo/missing-omni-metadata.toml` | Cargo.toml with pragma but no `package.metadata.omni` |
| `cargo/library-with-bin.toml` | Library form but has `[[bin]]` — reserved section violation |

### 4.2: Test Cases

| Test | What It Proves |
|------|---------------|
| `cargo/valid-library` | Well-formed library Cargo.toml passes with 0 errors |
| `cargo/missing-omni` | Cargo.toml with pragma but no OmniCode metadata fails |
| `cargo/reserved-section` | Library with `[[bin]]` triggers `form/reserved-section-present` |
| `cargo/form-classification` | Pragma args correctly classified as form vs derivation |

---

## Execution Order

```
1.1 (Create Cargo.toml fixture)
  ↓
1.2 (Baseline test — what does linter say NOW)
  ↓
2.1 (Create data/forms/ schema files)
  ↓
2.2 (Extend DerivationLayout or create parseTomlFormSchema)
  ↓
2.3 (Load form constraints in TOML handler)
  ↓
3.1-3.3 (Form-aware checks + wire into orchestrator)
  ↓
4.1-4.2 (Additional fixtures + tests)
```

**Constraint:** `deno task test` green after every step.

---

## Critical Files

| File | Phase | Action |
|------|:-----:|--------|
| `tests/fixtures/toml/cargo/valid-library.toml` | 1.1 | CREATE: well-formed library Cargo.toml |
| `tests/handlers/toml_test.ts` | 1.2, 4.2 | ADD: Cargo integration tests |
| `schemas/data/forms/bare-bone/toml-bare-bone.jsonc` | 2.1 | CREATE: 3-block bare-bone form |
| `schemas/data/forms/declared/toml-cargo-library.jsonc` | 2.1 | CREATE: Cargo library form |
| `lib/foundation/schema.ts` | 2.2 | EXTEND: DerivationLayout + form loading |
| `lib/handlers/toml.ts` | 2.3, 3.1-3.3 | ADD: form loading + form-aware checks |
| `tests/fixtures/toml/cargo/missing-omni-metadata.toml` | 4.1 | CREATE: error case fixture |
| `tests/fixtures/toml/cargo/library-with-bin.toml` | 4.1 | CREATE: reserved section fixture |

## Reference Files

| File | What It Shows |
|------|---------------|
| `lib/handlers/rust.ts` (checkFormRequired/Reserved) | Pattern for form-aware checks |
| `lib/foundation/code-schema.ts` (FormConstraints) | Pattern for form types + loading |
| `schemas/code/forms/declared/rust-module.jsonc` | Pattern for form schema structure |
| `schemas/data/format/toml-3block-schema.jsonc` | TOML schema with pragma_taxonomy |
| `tests/fixtures/toml/structure/valid-complete.toml` | Pattern for well-formed TOML fixture |

## Verification

```bash
# After each phase:
deno task test                    # All tests pass

# After Phase 3:
deno run --allow-read mod.ts lint toml <cargo-fixture>    # Form checks fire

# Final:
deno run --allow-read mod.ts lint toml <real-cargo.toml>  # Real-world validation
```
