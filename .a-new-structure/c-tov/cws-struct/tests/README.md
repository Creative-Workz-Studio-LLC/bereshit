<!--
#!omni document --md
===============================================================================
METADATA BLOCK
===============================================================================
-->
<!--
:key: B-tov-cws-struct-tests-README
:title: CWS Struct — Test Suite Guide
:type: Documentation (Guide)
:version: a-01.00
:created: 2026-02-23
:updated: 2026-02-23
:authors: Deborah Saphar (CPI-SI)
:purpose: Definitive guide to cws-struct test directory structure, detection
  hierarchy, running tests, and contributing new tests. PhD rigor, 60-second
  scannable.
:derives_from: 5-block markdown documentation template
:biblical_foundation: "Prove all things; hold fast that which is good."
  — 1 Thessalonians 5:21
-->

<!--
===============================================================================
HEADER BLOCK
===============================================================================
-->

# CWS Struct — Test Suite

> *"Prove all things; hold fast that which is good."* — 1 Thessalonians 5:21

**416+ tests** validating the cws-struct linter, transformer, and schema pipeline. Tests are organized by **detection layer** (what the linter checks) as the primary dimension, with **handler** (go/rust/toml) as the secondary dimension.

**Quick orientation:**

| If you want to... | Go to |
|-------------------|-------|
| Run the full suite | `deno task test` |
| Run one handler | `deno task test:go` |
| Run one detection layer | `deno task test:R50` |
| Understand the layer hierarchy | [Detection Layers](#detection-layers) |
| Add a new test | [Contributing Tests](#contributing-tests) |
| Understand the witness system | [Witness System](#witness-system) |

---

<!--
===============================================================================
CONTEXT BLOCK
===============================================================================
-->

## Context

### Scope

This document covers the `tests/` directory within `cws-struct`. It documents:

- Directory structure and organization philosophy
- Detection layer definitions (R50 through R05, T00)
- Test running commands (deno task suite)
- Fixture management
- Witness system (error code coverage tracking)
- Contribution guidelines

### Prerequisites

| Requirement | Why |
|-------------|-----|
| Deno 2.6+ | Test runner and permissions |
| `tests/helpers.ts` | Shared utilities — imported by all test files |
| `lib/data/errors.ts` | Error catalog — defines detection layers and codes |
| `tests/fixtures/` | Sample files exercised by handler tests |

### Key Terms

| Term | Definition |
|------|------------|
| **Detection layer** | Granularity level at which the linter examines a file (R50 = widest, R05 = finest) |
| **Handler** | Format-specific lint implementation (go, rust, toml, json, etc.) |
| **Witness** | A test that exercises and proves a specific CWS error code |
| **Fixture** | A sample source file used as test input |
| **Ladder** | The layered detection philosophy — each layer stands on the one below |
| **Baton** | Results passed between detection layers, narrowing focus at each step |

---

<!--
===============================================================================
CONTENT BLOCK
===============================================================================
-->

## Directory Structure

```
tests/
|
|-- helpers.ts               # Shared test infrastructure (ALL tests import this)
|-- integration_test.ts      # Cross-handler integration tests
|-- pipeline_integration_test.ts  # Schema pipeline integration tests
|
|-- R50/                     # Layer 0 — File-Level Structure
|   |-- go/                  #   Go handler R50 tests
|   |-- rust/                #   Rust handler R50 tests
|   `-- toml/                #   TOML handler R50 tests
|
|-- R25/                     # Layer 1 — Block-Level Structure
|   |-- go/
|   |-- rust/
|   `-- toml/
|
|-- R10/                     # Layer 2 — Content Placement
|   |-- go/
|   |-- rust/
|   `-- toml/
|
|-- R05/                     # Layer 3 — Content Detail
|   |-- go/
|   |-- rust/
|   `-- toml/
|
|-- T00/                     # Layer 4 — Tool Infrastructure
|   |-- api/                 #   API layer tests
|   |-- data/                #   Data layer tests (errors, registries, concepts)
|   |-- engine/              #   Engine tests (pipeline, discovery, transform)
|   |-- foundation/          #   Foundation tests (types, schemas, health)
|   `-- verify/              #   Verification command tests
|
|-- unit/                    # Pure unit tests (no fixtures, no file I/O)
|   |-- go/                  #   Go-specific unit tests (classifyLine, parse*)
|   |-- rust/                #   Rust-specific unit tests
|   `-- shared/              #   Shared logic unit tests (chunker, grid)
|
|-- integration/             # Cross-concern integration tests
|   |-- go/                  #   Go integration tests
|   `-- rust/                #   Rust integration tests
|
|-- fixtures/                # Sample source files for handler tests
|   |-- go/                  #   Go fixtures (by concern: structure/, metadata/, etc.)
|   |-- rust/                #   Rust fixtures
|   |-- toml/                #   TOML fixtures
|   `-- transpiler/          #   Transpiler test fixtures
|
|-- handlers/                # [LEGACY] Monolithic handler tests (being split)
|   |-- go_test.ts           #   1346 lines — splitting into R50/go, R25/go, etc.
|   |-- rust_test.ts         #   1836 lines — splitting into R50/rust, R25/rust, etc.
|   `-- toml_test.ts         #   679 lines — splitting into R50/toml, R25/toml, etc.
|
|-- data/                    # Data layer tests (errors, types, registries)
|-- engine/                  # Engine layer tests (cascade, pipeline, transform)
|-- foundation/              # Foundation layer tests (schemas, health, types)
|-- shared/                  # Shared logic tests (chunker, concept, grid)
|-- cli/                     # CLI tests (diff-aware, mod)
|-- api/                     # API tests (REST endpoints, health queries)
`-- verify/                  # Environment verification tests
```

### What Changed

**Before:** Handler tests lived in monolithic files (`handlers/go_test.ts` at 1346 lines). All R50, R25, R10, R05, unit, transform, and adapter tests were in one file per handler. Finding "all R50 tests across handlers" required `--filter`.

**After:** Tests are split by detection layer first, handler second. `R50/go/structure_test.ts` lives next to `R50/rust/structure_test.ts`. The directory hierarchy IS the detection hierarchy.

---

## Detection Layers

The linter examines files in layers, from widest scope to finest detail. Each layer stands on the one below — you cannot check content placement (R10) if block structure (R50) is broken.

### The Ladder

```
R50  ----  Widest: Does the file have bones?
  |
R25  ----  Are sections present and ordered?
  |
R10  ----  Is content in the right place?
  |
R05  ----  Finest: Are values and patterns correct?
  |
T00  ----  Tool: Is the tooling itself correct?
```

### Layer Definitions

| Layer | Code Range | Scope | What It Checks | Example |
|:-----:|:----------:|-------|----------------|---------|
| **R50** | CWS-R50-001 to CWS-R50-073 | **Whole file** | Pragma, block boundaries, separators, file classification | "Does this file have a METADATA block?" |
| **R25** | CWS-R25-001 to CWS-R25-099 | **Block structure** | Section presence, ordering, required fields, form constraints | "Are SETUP subsections in dependency order?" |
| **R10** | CWS-R10-001 to CWS-R10-061 | **Content placement** | Code in correct block, identity field validation, emphasis | "Is this function in BODY or leaked into SETUP?" |
| **R05** | CWS-R05-001 to CWS-R05-071 | **Content detail** | Doc comments, field values, concept detection, template content | "Does Pragma I1.at have a valid version format?" |
| **T00** | CWS-T00-001 to CWS-T00-070 | **Tool internals** | Schema pipeline, composition, transpiler, registry, database | "Can the schema file be parsed and loaded?" |

### The Baton

Results from wider layers inform narrower ones:

1. **R50** determines: Does the file belong to us? Is it parseable? What blocks exist?
2. **R25** receives block positions and asks: Are sections ordered? Are required sections present?
3. **R10** receives section positions and asks: Is code placed correctly within sections?
4. **R05** receives placement context and asks: Are the actual values and patterns correct?

If R50 fails (no block structure), R25-R05 checks are meaningless. The baton must be passed cleanly.

---

## Running Tests

### Full Suite

```bash
deno task test           # Run all tests
deno task test:watch     # Watch mode — re-run on file changes
```

### By Handler

```bash
deno task test:go        # All Go handler tests
deno task test:rust      # All Rust handler tests
deno task test:toml      # All TOML handler tests
```

### By Detection Layer

```bash
deno task test:R50       # All R50 tests (cross-handler)
deno task test:R25       # All R25 tests
deno task test:R10       # All R10 tests
deno task test:R05       # All R05 tests
```

These use `--filter` to match test names that begin with the layer prefix (e.g., `R50/`).

### By Handler + Layer (Targeted)

```bash
deno task test:go:R50    # Go R50 tests only
deno task test:rust:R25  # Rust R25 tests only
deno task test:toml:R05  # TOML R05 tests only
```

### By Concern (Cross-Handler)

```bash
deno task test:structure     # Structure tests across all handlers
deno task test:closing       # Closing block tests
deno task test:form          # Form/subtype constraint tests
deno task test:unit          # Pure unit tests (no fixtures)
deno task test:transform     # Transform tests
deno task test:concept       # Concept detection tests
```

### By Tool Layer

```bash
deno task test:foundation    # Foundation types, schemas, health
deno task test:engine        # Engine pipeline, discovery, transform
deno task test:data          # Data layer (errors, registries, concepts)
deno task test:cli           # CLI argument parsing, diff-aware
deno task test:verify        # Environment verification
deno task test:shared        # Shared logic (chunker, grid, patterns)
```

### By Error Code

```bash
deno test --filter='CWS-R50-020' tests/    # All witnesses for a specific code
deno test --filter='CWS-R25' tests/         # All witnesses in R25 layer
```

### Development Workflow

```bash
deno task dev            # Type-check + full test suite
deno task ci             # Same as dev (CI pipeline)
deno task check          # Type-check only (no tests)
```

---

## Test Naming Convention

Every test name follows a structured path format:

```
{layer}/{concern}/{fixture}: {description}
```

### Examples

```
R50/structure/valid-library: zero errors
R25/setup/subsection-order-wrong: detects Core Types before Constants
R10/metadata/missing-ic-fields: warns about missing required fields
R05/closing/X1-depth: detects empty and placeholder field values
unit/classifyGoLine: identifies all major Go constructs
transform/move-tests: actual transform moves test to CLOSING Cv
adapter/go: format is 'go'
regression/valid-library: still zero errors after content validation additions
```

| Segment | Meaning |
|---------|---------|
| `R50/`, `R25/`, `R10/`, `R05/` | Detection layer being tested |
| `unit/` | Pure unit test (no file I/O) |
| `transform/` | Transform operation test |
| `adapter/` | Format handler adapter interface test |
| `regression/` | Regression guard against previous bugs |
| `structure/`, `metadata/`, `closing/` | Concern within the layer |
| `valid-library`, `missing-block` | Fixture or scenario name |
| After `:` | What the test asserts |

This naming convention enables `--filter` targeting at any granularity.

---

## Witness System

Tests that exercise a specific error code include the code in their name and call `witnessFor()`:

```typescript
Deno.test("[CWS-R50-020] missing block fires error", () => {
  witnessFor("CWS-R50-020", "[CWS-R50-020] missing block fires error");
  // ... test body
});
```

### What Witnesses Provide

| Capability | How |
|------------|-----|
| **Error code coverage** | `unwitnessedCodes()` returns codes with no test |
| **Targeted debugging** | `deno test --filter='CWS-R50-020'` runs just that code's tests |
| **Coverage gate (G13)** | Every catalog code must have at least one witness |
| **Diagnostic output** | `errorCodeReport(results)` shows code distribution in failures |

### Helper Functions for Witnesses

From `tests/helpers.ts`:

```typescript
import {
  witnessFor,        // Register this test as a witness
  witnessesFor,      // Get all test names witnessing a code
  unwitnessedCodes,  // Codes with no witness (G13 gate)
  diagnosticCommand, // Get --filter command for a code
  witnessCount,      // Total witness statistics
} from "../../helpers.ts";
```

---

## Fixture Organization

Fixtures live in `tests/fixtures/` organized by handler, then by concern:

```
fixtures/
|-- go/
|   |-- structure/       # Block structure fixtures (valid-library.go, missing-block.go, etc.)
|   |-- metadata/        # Metadata field fixtures
|   |-- setup/           # SETUP subsection fixtures
|   |-- body/            # BODY content fixtures
|   |-- closing/         # CLOSING zone fixtures
|   `-- format/          # Format detection fixtures
|
|-- rust/
|   |-- structure/
|   |-- metadata/
|   |-- setup/
|   |-- body/
|   |-- closing/
|   |-- form/            # Form/subtype constraint fixtures
|   `-- format/
|
|-- toml/
|   |-- structure/
|   |-- metadata/
|   |-- content/
|   |-- closing/
|   `-- cargo/           # Cargo.toml specific fixtures
|
`-- transpiler/          # Transpiler pipeline fixtures
```

### Resolving Fixtures in Tests

Use `fixture()` from helpers — it resolves relative to the calling module:

```typescript
import { fixture, lazyFormat, errors } from "../../helpers.ts";

const go = lazyFormat("go");

Deno.test("R50/structure/valid-library: zero errors", async () => {
  const results = await go.lint(fixture("go/structure/valid-library.go"));
  assertEquals(errors(results).length, 0);
});
```

**Note:** `fixture()` uses `import.meta.url` for resolution. When tests move to new directories, the relative path from test file to `fixtures/` changes — but `fixture()` always resolves from `tests/fixtures/` regardless of test file location.

---

## Import Pattern

All test files import from `tests/helpers.ts`. The import path varies by depth:

| Test Location | Import Path |
|---------------|-------------|
| `tests/handlers/go_test.ts` | `../helpers.ts` |
| `tests/R50/go/structure_test.ts` | `../../helpers.ts` |
| `tests/T00/data/errors_test.ts` | `../../helpers.ts` |
| `tests/unit/go/classify_test.ts` | `../../helpers.ts` |

`helpers.ts` provides:

| Export | Purpose |
|--------|---------|
| `lazyFormat(name)` | Lazy proxy for format handler — safe at module level |
| `requireFormat(name)` | Throws if handler not registered — use inside test bodies |
| `getFormat` | Re-export from engine registry |
| `fixture(path)` | Resolve fixture path relative to `tests/fixtures/` |
| `errors(results)` | Filter to error-severity results |
| `warnings(results)` | Filter to warn-severity results |
| `infos(results)` | Filter to info-severity results |
| `byRule(results, fragment)` | Filter by rule string |
| `byErrorCode(results, code)` | Filter by CWS error code |
| `byLayer(results, layer)` | Filter by detection layer |
| `byCategory(results, cat)` | Filter by error category |
| `resolveErrorCode(result)` | Map a lint result to its catalog entry |
| `errorCodeReport(results)` | Human-readable code distribution |
| `witnessFor(code, name)` | Register test as witness for an error code |
| `createMockContext(overrides)` | Build minimal `BaseFileContext` for unit tests |
| `createBlockPositions(blocks)` | Build `BlockPosition[]` from simplified input |
| `createSubsectionRanges(ranges)` | Build `SubsectionRange[]` from simplified input |

---

## Contributing Tests

### Adding a New Handler Test

1. **Identify the detection layer.** What does this test verify?
   - File has correct blocks? -> R50
   - Sections are present and ordered? -> R25
   - Content is in the right place? -> R10
   - Values and patterns are correct? -> R05

2. **Create the test file** in the appropriate directory:
   ```
   tests/R{layer}/{handler}/{concern}_test.ts
   ```

3. **Name the test** following the convention:
   ```typescript
   Deno.test("R25/setup/subsection-order-wrong: detects X before Y", async () => {
   ```

4. **Import from helpers:**
   ```typescript
   import { fixture, lazyFormat, errors, warnings } from "../../helpers.ts";
   ```

5. **If testing a specific error code,** add a witness:
   ```typescript
   import { witnessFor } from "../../helpers.ts";

   Deno.test("[CWS-R25-023] SETUP subsection out of order", async () => {
     witnessFor("CWS-R25-023", "[CWS-R25-023] SETUP subsection out of order");
     // ... test body
   });
   ```

6. **Create fixtures** if needed in `tests/fixtures/{handler}/{concern}/`.

### Adding a Tool Layer Test

Tool tests go in `T00/` mirroring the `lib/` directory:

| `lib/` module | Test location |
|---------------|---------------|
| `lib/foundation/` | `tests/T00/foundation/` |
| `lib/engine/` | `tests/T00/engine/` |
| `lib/data/` | `tests/T00/data/` |
| `lib/api/` | `tests/T00/api/` |
| `lib/verify/` | `tests/T00/verify/` |

### Adding a Unit Test

Unit tests require no fixtures and no file I/O. They test pure functions:

```
tests/unit/{handler}/{function}_test.ts    # Handler-specific units
tests/unit/shared/{module}_test.ts         # Shared logic units
```

### Checklist Before Submitting

- [ ] Test name follows `{layer}/{concern}/{fixture}: {description}` format
- [ ] Test file is in the correct `R{n}/{handler}/` directory
- [ ] Import path to `helpers.ts` is correct for the depth
- [ ] Fixtures are in `tests/fixtures/{handler}/{concern}/`
- [ ] Error code witnesses call `witnessFor()` and include `[CWS-XXX-NNN]` in test name
- [ ] `deno task test` passes with the new test included

---

## Architecture Relationship

The test hierarchy mirrors the detection pipeline's architecture:

```
Detection Pipeline          Test Directory
================          ==============
R50 (whole file)    <-->   tests/R50/{handler}/
R25 (block level)   <-->   tests/R25/{handler}/
R10 (placement)     <-->   tests/R10/{handler}/
R05 (content)       <-->   tests/R05/{handler}/
T00 (tool infra)    <-->   tests/T00/{lib-module}/
```

The `lib/` directory organizes by **what the code does** (foundation, engine, handlers, data). The `tests/` directory organizes by **what the tests prove** (detection granularity). These two dimensions are complementary, not redundant.

| Dimension | Organized By | Example |
|-----------|--------------|---------|
| `lib/` | Function (what it does) | `lib/handlers/go.ts` |
| `tests/R[n]/` | Detection (what it proves) | `tests/R50/go/structure_test.ts` |
| `tests/T00/` | Infrastructure (what supports) | `tests/T00/engine/pipeline_test.ts` |
| `tests/unit/` | Isolation (pure logic) | `tests/unit/go/classify_test.ts` |

---

<!--
===============================================================================
FOOTER BLOCK
===============================================================================
-->

## References

| Resource | Location |
|----------|----------|
| Error catalog (all codes) | `lib/data/errors.ts` |
| Test helpers | `tests/helpers.ts` |
| Deno task definitions | `deno.jsonc` (50 tasks) |
| Fixture files | `tests/fixtures/` |
| Format handlers | `lib/handlers/` (go.ts, rust.ts, toml.ts) |
| Foundation types | `lib/foundation/mod.ts` |
| Schema pipeline | `lib/foundation/pipeline.ts` |

## Related Documents

| Document | Relationship |
|----------|-------------|
| `deno.jsonc` | Task definitions — all test commands defined here |
| `lib/data/errors.ts` | Error catalog — canonical source for layer definitions |
| `tests/helpers.ts` | Test infrastructure — shared utilities, witness registry |
| `CLAUDE.md` (project root) | Project-level documentation and build commands |

---

> *"Prove all things; hold fast that which is good."* — 1 Thessalonians 5:21

The test suite proves structure. The structure proves the linter. The linter proves the code.
Each layer stands on the one below. When R50 holds, R25 can build. When all layers hold,
the code is sound. This is the ladder.
