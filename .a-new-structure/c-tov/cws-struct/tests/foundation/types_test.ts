// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/foundation/types_test.ts
// key:     B-tov-cws-struct-tests-foundation-types
// title:   CWS Struct — Types Tests
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-17
// authors: Nathan Emet (CPI-SI)
// purpose: Tests for result constructors (error, warn, info), summarize,
//          and FixSuggestion handling. Verifies the foundation contracts.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert } from "jsr:@std/assert";
import {
  error, warn, info, summarize,
} from "../../lib/foundation/mod.ts";
import type {
  LintResult, FixSuggestion, LintSummary,
} from "../../lib/foundation/mod.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Result constructors — error, warn, info
// ---------------------------------------------------------------------------

Deno.test("types/error: creates error-severity result", () => {
  const r = error("test.toml", "test/rule", "Something broke");
  assertEquals(r.severity, "error");
  assertEquals(r.file, "test.toml");
  assertEquals(r.rule, "test/rule");
  assertEquals(r.message, "Something broke");
  assertEquals(r.line, undefined);
  assertEquals(r.fix, undefined);
});

Deno.test("types/warn: creates warn-severity result", () => {
  const r = warn("test.toml", "test/rule", "Be careful");
  assertEquals(r.severity, "warn");
  assertEquals(r.message, "Be careful");
});

Deno.test("types/info: creates info-severity result", () => {
  const r = info("test.toml", "test/rule", "FYI");
  assertEquals(r.severity, "info");
  assertEquals(r.message, "FYI");
});

Deno.test("types/error: accepts line number via ResultOpts", () => {
  const r = error("test.toml", "test/rule", "Line 42 error", { line: 42 });
  assertEquals(r.line, 42);
});

Deno.test("types/warn: accepts fix suggestion via ResultOpts", () => {
  const fix: FixSuggestion = {
    description: "Add missing table",
    toml: '[_metadata.I1_core]\nkey = ""',
    location: "after [_metadata]",
  };
  const r = warn("test.toml", "test/rule", "Missing table", { fix });
  assert(r.fix !== undefined, "Should have fix attached");
  assertEquals(r.fix!.description, "Add missing table");
  assertEquals(r.fix!.location, "after [_metadata]");
});

Deno.test("types/error: accepts bare FixSuggestion for backward compat", () => {
  const fix: FixSuggestion = {
    description: "Add section",
    toml: "[section]\nkey = \"\"",
  };
  // Pass FixSuggestion directly (not wrapped in ResultOpts)
  const r = error("test.toml", "test/rule", "Missing section", fix);
  assert(r.fix !== undefined, "Should detect bare FixSuggestion");
  assertEquals(r.fix!.description, "Add section");
});

Deno.test("types/info: accepts line + fix together", () => {
  const fix: FixSuggestion = { description: "Fix it", toml: "key = \"\"" };
  const r = info("test.toml", "test/rule", "Note", { line: 10, fix });
  assertEquals(r.line, 10);
  assert(r.fix !== undefined);
});

// ---------------------------------------------------------------------------
// summarize
// ---------------------------------------------------------------------------

Deno.test("types/summarize: tallies errors/warnings/infos correctly", () => {
  const results: LintResult[] = [
    error("f.toml", "r1", "err1"),
    error("f.toml", "r2", "err2"),
    warn("f.toml", "r3", "w1"),
    info("f.toml", "r4", "i1"),
    info("f.toml", "r5", "i2"),
    info("f.toml", "r6", "i3"),
  ];
  const s = summarize("f.toml", results);
  assertEquals(s.file, "f.toml");
  assertEquals(s.errors, 2);
  assertEquals(s.warnings, 1);
  assertEquals(s.infos, 3);
  assertEquals(s.results.length, 6);
  assertEquals(s.health, undefined);
});

Deno.test("types/summarize: empty results array", () => {
  const s = summarize("clean.toml", []);
  assertEquals(s.errors, 0);
  assertEquals(s.warnings, 0);
  assertEquals(s.infos, 0);
  assertEquals(s.results.length, 0);
});

Deno.test("types/summarize: includes health when provided", () => {
  const health = {
    total: 85,
    blocks: [],
    totalActions: 10,
    passCount: 8,
    failCount: 2,
  };
  const s = summarize("f.toml", [], health);
  assert(s.health !== undefined, "Should include health");
  assertEquals(s.health!.total, 85);
});

Deno.test("types/summarize: omits health when undefined", () => {
  const s = summarize("f.toml", []);
  assertEquals(s.health, undefined);
  // Verify health key doesn't exist in the object
  assert(!("health" in s), "Should not have health key");
});

// ============================================================================
// CLOSING
// ============================================================================
//
// Foundation type tests — verifying the contracts that every handler depends on.
// "A just weight and balance are the LORD's." — Proverbs 16:11
// ============================================================================
