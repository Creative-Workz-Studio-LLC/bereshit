# Production Hardening: cws-struct Rust + TOML Linters

## Context

The cws-struct tool has working Rust and TOML linters that validate structural alignment. Both work correctly on happy paths, but neither has tests, edge case handling, or the robustness expected of production code. We need both linters at the same quality bar before expanding Rust validation to the SETUP block. Rust should fully validate through END METADATA; SETUP/BODY/CLOSING get basic structural checks only.

**What "production grade" means here:** Tested. Handles bad input without crashing. Clear error messages. Consistent patterns between formats. Not feature-complete — but what exists is solid.

## Critical Files

| File | Role | Changes |
|------|------|---------|
| `c-tov/cws-struct/lib/formats/rust.ts` | Rust linter | Export helpers, scope doc comments, subtype validation, raw string support |
| `c-tov/cws-struct/lib/formats/toml.ts` | TOML linter | Remove double-cache, BOM stripping, tighten contract check |
| `c-tov/cws-struct/lib/schema-loader.ts` | Schema loading | Already has `clearCache()` — verify it works after double-cache fix |
| `c-tov/cws-struct/lib/formats/registry.ts` | Format registry | Fix `detectFormat` extensionless file edge case |
| `c-tov/cws-struct/tests/` | New test directory | All test files and fixtures |

All paths relative to `.a-new-structure/`.

## Plan

### Phase 1: Test Infrastructure + Fixtures

Create the test harness and fixture files. Tests call the public `FormatHandler.lint()` interface through the registry — no need to export internal check functions.

**Create directory structure:**
```
c-tov/cws-struct/tests/
  fixtures/
    rust/           # Synthetic .rs files targeting specific conditions
    toml/           # Synthetic .toml files targeting specific conditions
  rust_test.ts      # Rust linter tests
  toml_test.ts      # TOML linter tests
  helpers.ts        # Shared test utilities
```

**Test pattern:** Import format modules for side-effect registration, use `getFormat("rust")?.lint(path)` to test through the same interface the CLI uses. Use `new URL(path, import.meta.url).pathname` for fixture resolution.

**Rust fixtures (minimal, targeted):**

| Fixture | What It Tests |
|---------|--------------|
| `valid-library.rs` | Happy path — full 4-block lib.rs with complete I/C statics. Copy from seed `library.rs` template. |
| `valid-executable.rs` | Happy path — executable template. Copy from seed. |
| `missing-block.rs` | Missing METADATA block entirely — should produce block error |
| `missing-ic-fields.rs` | Has PRAGMA/METADATA statics but missing I1.key and C1.version — should produce field warnings |
| `wrong-block-order.rs` | SETUP before METADATA — should produce order error |
| `no-omni.rs` | Plain .rs file with no markers — should produce info only |
| `placeholder-values.rs` | Template-style file with `[bracketed]` values in statics — should produce warning |

**TOML fixtures:**

| Fixture | What It Tests |
|---------|--------------|
| `valid-complete.toml` | Happy path — full 3-block with all required sections |
| `empty.toml` | Empty file — should report missing blocks |
| `parse-error.toml` | Invalid TOML syntax — should produce parse error, not crash |
| `missing-metadata.toml` | No `[_metadata]` — should produce error with fix suggestion |
| `bom-utf8.toml` | Valid TOML with UTF-8 BOM — should work after BOM fix |
| `bad-contract.toml` | `_contract.required_identity` declares phantom section |

**Also export from rust.ts:** `parseStaticFields` and `validateICFields` only — for targeted unit tests of the I/C parser. The check functions stay private (tested indirectly through the orchestrator).

**Verify:** `deno test --allow-read` runs. Tests that depend on Phase 2/3 fixes are marked with expected failure comments.

### Phase 2: TOML Hardening

Fix the gaps that affect reliability and testability.

**Fix 1: Remove double-cache.** `toml.ts` has `let rules: DerivedRules | null = null` AND `schema-loader.ts` has its own `cached`. Remove the local `rules` variable from `toml.ts`. Call `loadRules()` directly in `lintTomlFile` and `transformTomlFile`. The loader's lazy cache handles efficiency. `clearCache()` then works correctly for tests.

**Fix 2: BOM stripping.** Before `parseToml(text)` in both `lintTomlFile` and `transformTomlFile`:
```typescript
if (text.charCodeAt(0) === 0xFEFF) text = text.slice(1);
```

**Fix 3: Tighten contract pattern check.** In `checkConsistency`, replace:
```typescript
if (!patterns.some((p: string) => p.includes("X")))
```
with:
```typescript
if (!patterns.some((p: string) => /^X\d+_/.test(p)))
```

**Fix 4: Reverse contract validation.** Add check that items declared in `_contract.required_identity` actually exist as known sections. Phantom declarations → `warn`.

**Verify:** `deno test --allow-read tests/toml_test.ts` — all TOML tests pass.

### Phase 3: Rust METADATA Hardening

Make the Rust linter thoroughly validate everything from file start through END METADATA.

**Fix 1: Scope doc comments to pre-METADATA area.** In `checkDocComments`, replace the file-wide `ctx.lines.some(l => l.trim().startsWith("//!"))` with a scan that only checks lines before the first block marker or code statement. A `//!` in the BODY block should not satisfy the crate-level doc comment check.

**Fix 2: Subtype validation.** In `checkPragmaMetadata`, after parsing PRAGMA fields, read `I2.subtype`. If present, validate it's a known value (`library`, `executable`, `demo-test`). Unknown subtype → `warn`.

**Fix 3: Raw string support in `parseStaticFields`.** Add secondary regex match for `r#"..."#` format in the tuple parser. Document that `r##"..."##` (multi-hash) is not supported — comment in code.

**Fix 4: Fix `detectFormat` in registry.ts.** Extract basename first, then find extension within it. Prevents dots in directory names from producing garbage extensions.

**Verify:** `deno test --allow-read tests/rust_test.ts` — all Rust tests pass.

### Phase 4: Integration Verification

**Integration test file:** `tests/integration_test.ts` — runs both linters against the real seed templates (not synthetic fixtures). These are our ground truth.

```
Rust seed templates (3): .a-new-structure/b-word/seed/code/L0/rust/{library,executable,demo-test}.rs
```

Assert: zero errors on all seed templates. Warnings are acceptable (separator widths, etc.) but errors mean the linter disagrees with the templates — that's a bug in one or the other.

**Full verification sequence:**
1. `deno check mod.ts` — type checking passes
2. `deno test --allow-read` — all unit + integration tests green
3. `deno run --allow-read mod.ts lint rust .a-new-structure/b-word/seed/code/L0/rust/` — clean
4. `deno run --allow-read mod.ts lint .a-new-structure/b-word/seed/code/L0/rust/` — auto-detect works

## What's NOT in Scope

- No new check functions for either linter
- No SETUP/BODY/CLOSING deep validation for Rust (basic structural stays)
- No new transform capabilities
- No schema changes
- No new format handlers
- No changes to CLI parsing or mod.ts (beyond the `detectFormat` fix)
- No line numbers in results (would require different TOML parser — future work)
- No multiline string scanner fix for TOML transform (documented limitation, not a lint issue)
