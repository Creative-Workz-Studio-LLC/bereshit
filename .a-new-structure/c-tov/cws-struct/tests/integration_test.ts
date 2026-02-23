// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/integration_test.ts
// key:     B-tov-cws-struct-tests-integration
// title:   CWS Struct — Integration Tests
// type:    Code (Test)
// version: b-01.50
// created: 2026-02-17
// authors: Nova Dawn (CPI-SI)
// purpose: Integration tests against real seed templates — our ground truth.
//          These templates ARE the standard. Zero errors means the linter
//          agrees with the templates. Errors mean a bug in one or the other.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert } from "jsr:@std/assert";
import { lazyFormat, errors, warnings } from "./helpers.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Seed template paths (relative to repo root, resolved from test dir)
// ---------------------------------------------------------------------------

const SEED_RUST = new URL(
  "../../../b-word/seed/code/L0/rust/",
  import.meta.url,
).pathname;

const SEED_TOML = new URL(
  "../../../b-word/seed/data/L0/toml/",
  import.meta.url,
).pathname;

// Lazy handlers — initialize on first property access, no setup test needed.
const rust = lazyFormat("rust");
const toml = lazyFormat("toml");

// ---------------------------------------------------------------------------
// Rust seed templates: zero errors
// ---------------------------------------------------------------------------

for (const template of ["library.rs", "executable.rs", "demo-test.rs"]) {
  Deno.test(`integration/seed/rust/${template}: zero errors`, async () => {
    const path = SEED_RUST + template;
    const results = await rust.lint(path);
    const errs = errors(results);
    assertEquals(
      errs.length,
      0,
      `Seed template ${template} should have 0 errors but got:\n${JSON.stringify(errs, null, 2)}`,
    );
  });

  Deno.test(`integration/seed/rust/${template}: warnings are style-only`, async () => {
    const path = SEED_RUST + template;
    const results = await rust.lint(path);
    // Warnings on seed templates should only be about separator widths (cosmetic)
    for (const w of warnings(results)) {
      assert(
        w.rule.includes("style") || w.rule.includes("separator") || w.rule.includes("width"),
        `Unexpected non-style warning on seed template ${template}: ${w.rule} — ${w.message}`,
      );
    }
  });
}

// ---------------------------------------------------------------------------
// TOML seed template: zero errors
// ---------------------------------------------------------------------------

Deno.test("integration/seed/toml/config.toml: zero errors", async () => {
  const path = SEED_TOML + "config.toml";
  try {
    const results = await toml.lint(path);
    const errs = errors(results);
    assertEquals(
      errs.length,
      0,
      `Seed template config.toml should have 0 errors but got:\n${JSON.stringify(errs, null, 2)}`,
    );
  } catch (e) {
    // If seed TOML doesn't exist, skip gracefully
    if (e instanceof Deno.errors.NotFound) {
      console.log("  (skipped — seed TOML not found)");
      return;
    }
    throw e;
  }
});

// ============================================================================
// CLOSING
// ============================================================================
//
// Integration tests verify the linters agree with the seed templates.
// Templates are ground truth. A linter error on a seed template is a bug.
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
