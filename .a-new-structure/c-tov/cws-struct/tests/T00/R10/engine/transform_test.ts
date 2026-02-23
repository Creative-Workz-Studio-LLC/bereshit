// R-level: R10 (engine layer — transform pipeline)
// Migrated from: tests/engine/transform_test.ts
// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/T00/R10/engine/transform_test.ts
// key:     B-tov-cws-struct-tests-engine-transform
// title:   CWS Struct — Transform Pipeline Tests
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-22
// authors: Nova Dawn (CPI-SI)
// purpose: Tests for data-referenced messages, form-aware scaffold filtering,
//          and subsection reorder pass.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assertStringIncludes } from "jsr:@std/assert";
import type { TransformOptions } from "@lib/foundation/types.ts";
import type { Code4BlockRules } from "@lib/foundation/code-schema.ts";
import {
  createSubsectionScaffoldPass,
  createSubsectionReorderPass,
  createCodeMovePass,
} from "@lib/engine/transform.ts";
import type {
  TransformContext,
  SubsectionScaffoldSpec,
  SubsectionReorderSpec,
  CodeMoveSpec,
} from "@lib/engine/transform.ts";
import type { SubsectionDef } from "@lib/shared/mod.ts";

// ---------------------------------------------------------------------------
// Test helpers
// ---------------------------------------------------------------------------

/** Minimal subsection definitions for testing. */
const TEST_SUBSECTION_DEFS: SubsectionDef[] = [
  { tag: "Imports", pattern: /^\/\/\s{1,2}(?:\d+\.?\s+)?Imports\b/i },
  { tag: "Constants", pattern: /^\/\/\s{1,2}(?:\d+\.?\s+)?Constants\b/i },
  { tag: "Variables", pattern: /^\/\/\s{1,2}(?:\d+\.?\s+)?Variables\b/i },
  { tag: "Types", pattern: /^\/\/\s{1,2}(?:\d+\.?\s+)?Types\b/i },
];

const TEST_CANONICAL_ORDER = ["Imports", "Constants", "Variables", "Types"];

/** Build a minimal TransformContext from lines. */
function buildCtx(
  lines: string[],
  opts: Partial<TransformOptions> = {},
): TransformContext {
  const closingZones = [
    { tag: "Cv", kind: "code" as const, pattern: /Cv\s*[-–—]?\s*Closing Validation/ },
    { tag: "Ce", kind: "code" as const, pattern: /Ce\s*[-–—]?\s*Closing Execution/ },
    { tag: "Cc", kind: "code" as const, pattern: /Cc\s*[-–—]?\s*Closing Cleanup/ },
    { tag: "X1", kind: "doc" as const, pattern: /X1\s*[-–—]?\s*Closing Documentation/ },
    { tag: "X2", kind: "doc" as const, pattern: /X2\s*[-–—]?\s*Purpose/ },
    { tag: "X3", kind: "doc" as const, pattern: /X3\s*[-–—]?\s*Biblical/ },
  ];

  return {
    filePath: "/test/file.go",
    lines: [...lines], // copy to prevent mutation issues
    rules: {
      closingData: {
        zones: closingZones,
        docRequirements: [],
        fieldPatterns: {},
      },
    } as unknown as Code4BlockRules,
    opts: {
      dryRun: false,
      extensions: true,
      force: false,
      steps: false,
      ...opts,
    },
    results: [],
    modified: false,
    wouldModify: false,
  };
}

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// 1. Data-referenced scaffold messages
// ---------------------------------------------------------------------------

Deno.test("transform/scaffold: messages include position number per section registry", () => {
  const lines = [
    "// ============================================================================",
    "// SETUP",
    "// ============================================================================",
    "",
    "import \"fmt\"",
    "",
    "// ============================================================================",
    "// BODY",
    "// ============================================================================",
  ];

  const spec: SubsectionScaffoldSpec = {
    block: "SETUP",
    canonicalOrder: TEST_CANONICAL_ORDER,
    subsectionDefs: TEST_SUBSECTION_DEFS,
    classifyLine: (trimmed: string) => {
      if (trimmed.startsWith("import")) return "import";
      return "other";
    },
    kindToSubsection: { import: "Imports" },
  };

  const pass = createSubsectionScaffoldPass(spec);
  const ctx = buildCtx(lines, { dryRun: true });
  pass.apply(ctx);

  // Should have at least one scaffold result
  const scaffoldResults = ctx.results.filter((r) => r.rule.includes("scaffold"));
  assertEquals(scaffoldResults.length > 0, true, "Should have scaffold results");

  // Messages should include position reference
  for (const r of scaffoldResults) {
    assertStringIncludes(r.message, "position", "Message should reference position");
    assertStringIncludes(r.message, "per section registry", "Message should cite registry");
  }
});

// ---------------------------------------------------------------------------
// 2. Form-aware scaffold filtering
// ---------------------------------------------------------------------------

Deno.test("transform/scaffold: skips reserved sections when formStatus provided", () => {
  const lines = [
    "// ============================================================================",
    "// SETUP",
    "// ============================================================================",
    "",
    "import \"fmt\"",
    "const FOO = 42",
    "",
    "// ============================================================================",
    "// BODY",
    "// ============================================================================",
  ];

  const spec: SubsectionScaffoldSpec = {
    block: "SETUP",
    canonicalOrder: TEST_CANONICAL_ORDER,
    subsectionDefs: TEST_SUBSECTION_DEFS,
    classifyLine: (trimmed: string) => {
      if (trimmed.startsWith("import")) return "import";
      if (trimmed.startsWith("const")) return "const";
      return "other";
    },
    kindToSubsection: { import: "Imports", const: "Constants" },
    formStatus: {
      "Imports": "required",
      "Constants": "reserved",  // Should be skipped
    },
  };

  const pass = createSubsectionScaffoldPass(spec);
  const ctx = buildCtx(lines, { dryRun: true });
  pass.apply(ctx);

  // Should only scaffold Imports (not Constants which is reserved)
  const scaffoldResults = ctx.results.filter((r) => r.rule.includes("scaffold"));
  const hasImports = scaffoldResults.some((r) => r.message.includes("Imports"));
  const hasConstants = scaffoldResults.some((r) => r.message.includes("Constants"));

  assertEquals(hasImports, true, "Should scaffold Imports (required)");
  assertEquals(hasConstants, false, "Should NOT scaffold Constants (reserved)");
});

Deno.test("transform/scaffold: scaffolds all sections when no formStatus provided", () => {
  const lines = [
    "// ============================================================================",
    "// SETUP",
    "// ============================================================================",
    "",
    "import \"fmt\"",
    "const FOO = 42",
    "",
    "// ============================================================================",
    "// BODY",
    "// ============================================================================",
  ];

  const spec: SubsectionScaffoldSpec = {
    block: "SETUP",
    canonicalOrder: TEST_CANONICAL_ORDER,
    subsectionDefs: TEST_SUBSECTION_DEFS,
    classifyLine: (trimmed: string) => {
      if (trimmed.startsWith("import")) return "import";
      if (trimmed.startsWith("const")) return "const";
      return "other";
    },
    kindToSubsection: { import: "Imports", const: "Constants" },
    // No formStatus — all should be scaffolded
  };

  const pass = createSubsectionScaffoldPass(spec);
  const ctx = buildCtx(lines, { dryRun: true });
  pass.apply(ctx);

  const scaffoldResults = ctx.results.filter((r) => r.rule.includes("scaffold"));
  const hasImports = scaffoldResults.some((r) => r.message.includes("Imports"));
  const hasConstants = scaffoldResults.some((r) => r.message.includes("Constants"));

  assertEquals(hasImports, true, "Should scaffold Imports");
  assertEquals(hasConstants, true, "Should scaffold Constants (no form filter)");
});

// ---------------------------------------------------------------------------
// 3. Subsection reorder pass
// ---------------------------------------------------------------------------

Deno.test("transform/reorder: reorders out-of-order SETUP subsections", () => {
  const lines = [
    "// ============================================================================",
    "// SETUP",
    "// ============================================================================",
    "",
    "// ──────────────────────────────────────────────────────────────────────────",
    "// 2. Constants",
    "// ──────────────────────────────────────────────────────────────────────────",
    "",
    "const FOO = 42",
    "",
    "// ──────────────────────────────────────────────────────────────────────────",
    "// 1. Imports",
    "// ──────────────────────────────────────────────────────────────────────────",
    "",
    "import \"fmt\"",
    "",
    "// ============================================================================",
    "// BODY",
    "// ============================================================================",
  ];

  const spec: SubsectionReorderSpec = {
    block: "SETUP",
    canonicalOrder: TEST_CANONICAL_ORDER,
    subsectionDefs: TEST_SUBSECTION_DEFS,
  };

  const pass = createSubsectionReorderPass(spec);
  const ctx = buildCtx(lines);
  pass.apply(ctx);

  assertEquals(ctx.modified, true, "Should have modified lines");

  // After reorder: Imports should come before Constants
  const importsIdx = ctx.lines.findIndex((l) => l.includes("1. Imports"));
  const constantsIdx = ctx.lines.findIndex((l) => l.includes("2. Constants"));
  assertEquals(importsIdx < constantsIdx, true,
    `Imports (${importsIdx}) should come before Constants (${constantsIdx})`);

  // Check data-referenced message
  const reorderResult = ctx.results.find((r) => r.rule.includes("reorder-setup"));
  assertEquals(reorderResult !== undefined, true, "Should have reorder result");
  assertStringIncludes(reorderResult!.message, "per section registry");
});

Deno.test("transform/reorder: no-op when already in canonical order", () => {
  const lines = [
    "// ============================================================================",
    "// SETUP",
    "// ============================================================================",
    "",
    "// ──────────────────────────────────────────────────────────────────────────",
    "// 1. Imports",
    "// ──────────────────────────────────────────────────────────────────────────",
    "",
    "import \"fmt\"",
    "",
    "// ──────────────────────────────────────────────────────────────────────────",
    "// 2. Constants",
    "// ──────────────────────────────────────────────────────────────────────────",
    "",
    "const FOO = 42",
    "",
    "// ============================================================================",
    "// BODY",
    "// ============================================================================",
  ];

  const spec: SubsectionReorderSpec = {
    block: "SETUP",
    canonicalOrder: TEST_CANONICAL_ORDER,
    subsectionDefs: TEST_SUBSECTION_DEFS,
  };

  const pass = createSubsectionReorderPass(spec);
  const ctx = buildCtx(lines);
  pass.apply(ctx);

  assertEquals(ctx.modified, false, "Should NOT modify — already in order");
  assertEquals(ctx.results.length, 0, "Should have no results");
});

Deno.test("transform/reorder: dry-run reports without modifying", () => {
  const lines = [
    "// ============================================================================",
    "// SETUP",
    "// ============================================================================",
    "",
    "// ──────────────────────────────────────────────────────────────────────────",
    "// 2. Constants",
    "// ──────────────────────────────────────────────────────────────────────────",
    "",
    "const FOO = 42",
    "",
    "// ──────────────────────────────────────────────────────────────────────────",
    "// 1. Imports",
    "// ──────────────────────────────────────────────────────────────────────────",
    "",
    "import \"fmt\"",
    "",
    "// ============================================================================",
    "// BODY",
    "// ============================================================================",
  ];

  const spec: SubsectionReorderSpec = {
    block: "SETUP",
    canonicalOrder: TEST_CANONICAL_ORDER,
    subsectionDefs: TEST_SUBSECTION_DEFS,
  };

  const pass = createSubsectionReorderPass(spec);
  const ctx = buildCtx(lines, { dryRun: true });
  pass.apply(ctx);

  assertEquals(ctx.modified, false, "Dry-run should NOT modify");
  assertEquals(ctx.wouldModify, true, "Dry-run should set wouldModify");

  const result = ctx.results.find((r) => r.rule.includes("reorder-setup"));
  assertEquals(result !== undefined, true, "Should have dry-run result");
  assertStringIncludes(result!.message, "would be reordered");
});

Deno.test("transform/reorder: only runs with --extensions", () => {
  const spec: SubsectionReorderSpec = {
    block: "SETUP",
    canonicalOrder: TEST_CANONICAL_ORDER,
    subsectionDefs: TEST_SUBSECTION_DEFS,
  };

  const pass = createSubsectionReorderPass(spec);
  assertEquals(pass.requiresExtensions, true, "Reorder pass should require --extensions");
});

Deno.test("transform/reorder: batch operation — single result for multiple subsections", () => {
  const lines = [
    "// ============================================================================",
    "// SETUP",
    "// ============================================================================",
    "",
    "// ──────────────────────────────────────────────────────────────────────────",
    "// 4. Types",
    "// ──────────────────────────────────────────────────────────────────────────",
    "",
    "type Foo struct {}",
    "",
    "// ──────────────────────────────────────────────────────────────────────────",
    "// 2. Constants",
    "// ──────────────────────────────────────────────────────────────────────────",
    "",
    "const FOO = 42",
    "",
    "// ──────────────────────────────────────────────────────────────────────────",
    "// 1. Imports",
    "// ──────────────────────────────────────────────────────────────────────────",
    "",
    "import \"fmt\"",
    "",
    "// ============================================================================",
    "// BODY",
    "// ============================================================================",
  ];

  const spec: SubsectionReorderSpec = {
    block: "SETUP",
    canonicalOrder: TEST_CANONICAL_ORDER,
    subsectionDefs: TEST_SUBSECTION_DEFS,
  };

  const pass = createSubsectionReorderPass(spec);
  const ctx = buildCtx(lines);
  pass.apply(ctx);

  // Should produce exactly ONE result (batch, not per-subsection)
  const reorderResults = ctx.results.filter((r) => r.rule.includes("reorder-setup"));
  assertEquals(reorderResults.length, 1, "Should produce exactly one batch result");

  // Message should show before/after
  assertStringIncludes(reorderResults[0]!.message, "Types, Constants, Imports");
  assertStringIncludes(reorderResults[0]!.message, "Imports, Constants, Types");
});

// ---------------------------------------------------------------------------
// 4. Code move data-referenced messages
// ---------------------------------------------------------------------------

Deno.test("transform/code-move: messages include zone position from section registry", () => {
  // Build lines with a test function in BODY
  const lines = [
    "// ============================================================================",
    "// METADATA",
    "// ============================================================================",
    "// file: test.go",
    "// ============================================================================",
    "// SETUP",
    "// ============================================================================",
    "",
    "// ============================================================================",
    "// BODY",
    "// ============================================================================",
    "",
    "func TestFoo(t *testing.T) {",
    "  t.Log(\"test\")",
    "}",
    "",
    "// ============================================================================",
    "// CLOSING",
    "// ============================================================================",
    "",
    "// ============================================================================",
  ];

  const spec: CodeMoveSpec = {
    description: "test function",
    detector: (lines, start, end) => {
      for (let i = start; i <= end; i++) {
        if (lines[i]?.trim().startsWith("func Test")) {
          // Find closing brace
          let j = i;
          while (j <= end && !lines[j]?.trim().startsWith("}")) j++;
          return { start: i, end: j };
        }
      }
      return null;
    },
    sourceBlock: "BODY",
    targetBlock: "CLOSING",
    zoneTag: "Cv",
    zoneLabel: "Closing Validation",
    loop: true,
    resultTag: "transform/move-tests",
  };

  const pass = createCodeMovePass(spec);
  const ctx = buildCtx(lines, { dryRun: true });
  pass.apply(ctx);

  const moveResult = ctx.results.find((r) => r.rule === "transform/move-tests");
  assertEquals(moveResult !== undefined, true, "Should have move result");
  // Message should include zone position reference
  assertStringIncludes(moveResult!.message, "CLOSING.Cv");
  assertStringIncludes(moveResult!.message, "zone 1, per section registry");
});

// ============================================================================
// CLOSING
// ============================================================================
//
// Transform pipeline tests — data-referenced messages, form-aware scaffold,
// and subsection reorder pass. All three items from task #98.
//
// "Every purpose is established by counsel." — Proverbs 20:18
// ============================================================================
