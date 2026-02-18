# Plan: Reorganize Test Directory — Block-Based Fixture Layout + Targeted Testing

## Context

CLOSING block alignment is complete (229/229 tests passing). Now the test directory needs organization: fixtures are flat within each handler directory, and there's no way to run targeted tests by block or category. Seanje's request: "organize your test 1 layer deeper... organize the fixture based on the 4 blocks it's testing, the overall structure and format specific testing, then update the deno so you can run targeted test." Scope: the **entire** `tests/` folder, not just Go.

**Goal:** Organize all 48 fixtures into block-based subdirectories, prefix all 226+ test names with categories for `--filter` targeting, and add granular deno tasks.

## Part 1: Fixture Reorganization

### Go Fixtures (17 files → 6 subdirectories)

| Category | Files | What They Test |
|----------|-------|---------------|
| **`structure/`** | valid-library.go, valid-executable.go, missing-block.go, wrong-block-order.go, content-placement-correct.go, content-placement-wrong.go | Block presence, order, cross-block content |
| **`metadata/`** | missing-ic-fields.go, placeholder-values.go, identity-registration.go, metadata-leak.go | I/C fields, placeholders, code leaks |
| **`setup/`** | subsection-order-correct.go, subsection-order-wrong.go | SETUP subsection ordering |
| **`body/`** | wrong-body-order.go | BODY subsection ordering |
| **`closing/`** | wrong-closing-order.go, tests-in-body.go, main-in-body.go | Zone order, content placement |
| **`format/`** | no-omni.go | Non-4-block detection |

### Rust Fixtures (18 files + 1 crate dir → 6 subdirectories)

| Category | Files | What They Test |
|----------|-------|---------------|
| **`structure/`** | valid-library.rs, valid-executable.rs, missing-block.rs, wrong-block-order.rs, content-placement-correct.rs, content-placement-wrong.rs | Block presence, order, cross-block content |
| **`metadata/`** | missing-ic-crate/ (dir with lib.rs), placeholder-values.rs, metadata-leak.rs | I/C fields, placeholders, code leaks |
| **`setup/`** | subsection-order-correct.rs, subsection-order-wrong.rs, subsection-partial-correct.rs | SETUP subsection ordering |
| **`body/`** | wrong-body-order.rs | BODY subsection ordering |
| **`closing/`** | wrong-closing-order.rs, tests-in-body.rs, main-in-body.rs | Zone order, content placement |
| **`format/`** | no-omni.rs, transform-separators.rs | Non-4-block, separator transforms |

### TOML Fixtures (13 files → 4 subdirectories)

TOML is 3-block (no SETUP/BODY split), so categories adapt:

| Category | Files | What They Test |
|----------|-------|---------------|
| **`structure/`** | valid-complete.toml, empty.toml, parse-error.toml, missing-metadata.toml, missing-content.toml, missing-closing.toml, pragma-mismatch.toml, bom-utf8.toml | 3-block structure, parsing |
| **`metadata/`** | bad-contract.toml | Contract validation |
| **`content/`** | content-body-zones.toml, value-problems.toml, value-version-mismatch.toml | Value validation, zones |
| **`closing/`** | wrong-x-order.toml | X section ordering |

## Part 2: Test Name Prefixing

### Handler Tests — Category Prefixes

Every `Deno.test()` in handler test files gets a category prefix matching its fixture subdirectory. This enables `--filter "category/"` targeting.

**Categories for Go/Rust (4-block handlers):**

| Prefix | Tests | Example |
|--------|-------|---------|
| `structure/` | Valid paths, missing blocks, wrong order, cross-block placement | `structure/valid-library: zero errors` |
| `metadata/` | I/C fields, placeholders, identity, leaks | `metadata/missing-ic-fields: warns about missing required fields` |
| `setup/` | SETUP subsection ordering | `setup/subsection-order-correct: zero order warnings` |
| `body/` | BODY subsection ordering | `body/wrong-body-order: detects out of order` |
| `closing/` | Zone ordering, content placement | `closing/wrong-closing-order: detects code after doc` |
| `format/` | Format detection | `format/no-omni: info only, no errors` |
| `unit/` | classifyLine, getTopLevelDeclarations, parseFields, validateICFields | `unit/classifyGoLine: identifies all major Go constructs` |
| `transform/` | Dry-run and actual transforms | `transform/move-tests: dry-run detects Test func in BODY` |

**Categories for TOML (3-block handler):**

| Prefix | Tests | Example |
|--------|-------|---------|
| `structure/` | Valid, empty, parse error, missing blocks | `structure/valid-complete: zero errors` |
| `metadata/` | Contract validation | `metadata/bad-contract: detects phantom section` |
| `content/` | Value validation, zones | `content/value-problems: detects bad version format` |
| `closing/` | X section ordering | `closing/wrong-x-order: detects X sections out of order` |
| `unit/` | parsePragmaLine, parseFormatField, etc. | `unit/parsePragmaLine: valid data --toml` |

### Non-Handler Tests — Already File-Scoped

These files already test one focused area each. Add category prefixes for consistency and `--filter` across files:

| File | Prefix | Example |
|------|--------|---------|
| foundation/health_test.ts | `scoring/` | `scoring/computeContainerScore: empty actions = 100` |
| foundation/schema_test.ts | `schema/` | `schema/loadRules: returns DerivedRules object` |
| foundation/types_test.ts | `types/` | `types/error: creates error-severity result` |
| engine/discovery_test.ts | `discovery/` | `discovery/discoverFiles: single file target` |
| engine/registry_test.ts | `registry/` | `registry/getFormat: returns handler for toml` |
| cli/mod_test.ts | `cli/` | `cli/help: --help exits 0` |
| verify/env_test.ts | `env/` | `env/verifyEnvironment: returns boolean` |
| integration_test.ts | `integration/` | `integration/seed/rust/library.rs: zero errors` |

## Part 3: Test Code Reorganization

Within each handler test file, reorder `Deno.test()` blocks to group by category. Current order in go_test.ts is already roughly grouped — formalize it:

```
// BODY sections in order:
// 1. structure/ tests
// 2. metadata/ tests
// 3. setup/ tests
// 4. body/ tests
// 5. closing/ tests
// 6. format/ tests
// 7. unit/ tests
// 8. transform/ tests
```

Each section gets a clear separator:
```typescript
// ---------------------------------------------------------------------------
// structure/ — Overall block structure
// ---------------------------------------------------------------------------
```

## Part 4: Deno Task Updates

### New Tasks in deno.jsonc

```jsonc
"tasks": {
    // Existing
    "test": "deno test --allow-read --allow-write --allow-run tests/",
    "test:go": "deno test --allow-read --allow-write --allow-run tests/handlers/go_test.ts",
    "test:rust": "deno test --allow-read --allow-write --allow-run tests/handlers/rust_test.ts",
    "test:toml": "deno test --allow-read --allow-write --allow-run tests/handlers/toml_test.ts",

    // New — by infrastructure layer
    "test:foundation": "deno test --allow-read --allow-write --allow-run tests/foundation/",
    "test:engine": "deno test --allow-read --allow-write --allow-run tests/engine/",
    "test:cli": "deno test --allow-read --allow-write --allow-run --allow-env tests/cli/",
    "test:verify": "deno test --allow-read --allow-run tests/verify/",
    "test:integration": "deno test --allow-read tests/integration_test.ts",

    // New — by block category (cross-handler, via --filter)
    "test:structure": "deno test --allow-read --allow-write --allow-run --filter 'structure/' tests/handlers/",
    "test:metadata": "deno test --allow-read --allow-write --allow-run --filter 'metadata/' tests/handlers/",
    "test:setup": "deno test --allow-read --allow-write --allow-run --filter 'setup/' tests/handlers/",
    "test:body": "deno test --allow-read --allow-write --allow-run --filter 'body/' tests/handlers/",
    "test:closing": "deno test --allow-read --allow-write --allow-run --filter 'closing/' tests/handlers/",
    "test:unit": "deno test --allow-read --allow-write --allow-run --filter 'unit/' tests/handlers/",
    "test:transform": "deno test --allow-read --allow-write --allow-run --filter 'transform/' tests/handlers/",

    // Existing
    "compile": "..."
}
```

**Targeted by handler + block** uses inline composition:
```bash
deno task test:go -- --filter "structure/"   # Go structure tests only
```

## Part 5: Execution Order

### Step 1: Move fixtures (filesystem ops)
Create subdirectories and move files for all 3 handlers. Use `mkdir -p` and `mv`.

### Step 2: Update fixture paths in test files
Every `fixture("go/valid-library.go")` → `fixture("go/structure/valid-library.go")`. Mechanical find-and-replace within each test file.

### Step 3: Add category prefixes to test names
Every `Deno.test("valid-library: ...")` → `Deno.test("structure/valid-library: ...")`. Update all 226+ test names.

### Step 4: Reorder test sections within handler files
Group tests by category with clear section headers.

### Step 5: Prefix non-handler test names
Add category prefixes to foundation, engine, cli, verify, integration tests.

### Step 6: Update deno.jsonc tasks
Add new targeted tasks.

### Step 7: Run full test suite
`deno task test` — all 229+ tests must pass.

## Critical Files

| File | Changes |
|------|---------|
| `tests/fixtures/go/` | Move 17 files into 6 subdirectories |
| `tests/fixtures/rust/` | Move 18 files + 1 dir into 6 subdirectories |
| `tests/fixtures/toml/` | Move 13 files into 4 subdirectories |
| `tests/handlers/go_test.ts` | Update 45 fixture paths + test name prefixes + reorder |
| `tests/handlers/rust_test.ts` | Update 45 fixture paths + test name prefixes + reorder |
| `tests/handlers/toml_test.ts` | Update 58 fixture paths + test name prefixes + reorder |
| `tests/foundation/health_test.ts` | Add prefixes to 13 test names |
| `tests/foundation/schema_test.ts` | Add prefixes to 15 test names |
| `tests/foundation/types_test.ts` | Add prefixes to 11 test names |
| `tests/engine/discovery_test.ts` | Add prefixes to 8 test names |
| `tests/engine/registry_test.ts` | Add prefixes to 13 test names |
| `tests/cli/mod_test.ts` | Add prefixes to 11 test names |
| `tests/verify/env_test.ts` | Add prefixes to 3 test names |
| `tests/integration_test.ts` | Add prefixes to 7 test names |
| `tests/helpers.ts` | No changes (fixture() resolves from import.meta.url) |
| `deno.jsonc` | Add 12 new targeted tasks |

## Verification

1. `deno task test` — full suite (229+ tests) passes
2. `deno task test:structure` — runs only structure/ tests across all handlers
3. `deno task test:go -- --filter "closing/"` — runs only Go closing tests
4. `deno task test:foundation` — runs only foundation tests
5. Verify fixture subdirectories exist and old flat locations are empty
