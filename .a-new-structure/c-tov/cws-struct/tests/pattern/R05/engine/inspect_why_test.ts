// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/T00/R05/engine/inspect_why_test.ts
// key:     B-tov-cws-struct-tests-engine-inspect-why
// title:   CWS Struct — Inspect & Why Mode Tests
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-22
// authors: Nova Dawn (CPI-SI)
// purpose: Verify --inspect shows parsed structure and --why shows reasoning
//          chains for lint results. Both CLI integration and unit tests.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert, assertStringIncludes } from "jsr:@std/assert";
import { formatInspectText, formatInspectJson } from "@lib/engine/inspect.ts";
import type { InspectResult } from "@lib/foundation/types.ts";

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

/** Run CLI and capture stdout + stderr. */
async function runCli(args: string[]): Promise<{ code: number; stdout: string; stderr: string }> {
  const command = new Deno.Command("deno", {
    args: ["run", "--allow-read", "mod.ts", ...args],
    cwd: Deno.cwd(),
    stdout: "piped",
    stderr: "piped",
  });
  const result = await command.output();
  return {
    code: result.code,
    stdout: new TextDecoder().decode(result.stdout),
    stderr: new TextDecoder().decode(result.stderr),
  };
}

/** Build a minimal InspectResult for unit tests. */
function mockInspectResult(overrides?: Partial<InspectResult>): InspectResult {
  return {
    filePath: "test/example.go",
    format: "go",
    isTemplate: false,
    lineCount: 100,
    blocks: [
      { name: "METADATA", startLine: 1, endLine: 30, separatorLine: 1 },
      { name: "SETUP", startLine: 31, endLine: 60, separatorLine: 31 },
      { name: "BODY", startLine: 61, endLine: 90, separatorLine: 61 },
      { name: "CLOSING", startLine: 91, endLine: 100, separatorLine: 91 },
    ],
    sections: [
      { name: "Imports", block: "SETUP", line: 35, position: 1 },
      { name: "Constants", block: "SETUP", line: 45, position: 2 },
    ],
    content: [
      { category: "functions", count: 5, blocks: ["BODY"] },
      { category: "imports", count: 1, blocks: ["SETUP"] },
    ],
    directives: {
      "//omni:key": "B-test-example",
      "//omni:code": "--go -library",
      "//omni:version": "a-01.00",
    },
    pragma: "//omni:code --go -library",
    ...overrides,
  };
}

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// I1: formatInspectText — unit tests
// ---------------------------------------------------------------------------

Deno.test("[CWS-T00-089] inspect/I1: text output includes file path", () => {
  const result = mockInspectResult();
  const text = formatInspectText(result);
  assertStringIncludes(text, "test/example.go");
});

Deno.test("inspect/I1: text output shows format and line count", () => {
  const result = mockInspectResult();
  const text = formatInspectText(result);
  assertStringIncludes(text, "go");
  assertStringIncludes(text, "Lines: 100");
});

Deno.test("inspect/I1: text output shows all 4 blocks", () => {
  const result = mockInspectResult();
  const text = formatInspectText(result);
  assertStringIncludes(text, "METADATA");
  assertStringIncludes(text, "SETUP");
  assertStringIncludes(text, "BODY");
  assertStringIncludes(text, "CLOSING");
  assertStringIncludes(text, "4/4");
});

Deno.test("inspect/I1: text output shows sections", () => {
  const result = mockInspectResult();
  const text = formatInspectText(result);
  assertStringIncludes(text, "Imports");
  assertStringIncludes(text, "Constants");
  assertStringIncludes(text, "Sections detected (2)");
});

Deno.test("inspect/I1: text output shows content classification", () => {
  const result = mockInspectResult();
  const text = formatInspectText(result);
  assertStringIncludes(text, "functions: 5");
  assertStringIncludes(text, "imports: 1");
});

Deno.test("inspect/I1: text output shows directives", () => {
  const result = mockInspectResult();
  const text = formatInspectText(result);
  assertStringIncludes(text, "//omni:key: B-test-example");
  assertStringIncludes(text, "//omni:version: a-01.00");
});

Deno.test("inspect/I1: text output shows pragma", () => {
  const result = mockInspectResult();
  const text = formatInspectText(result);
  assertStringIncludes(text, "//omni:code --go -library");
});

Deno.test("inspect/I1: text output handles empty blocks", () => {
  const result = mockInspectResult({ blocks: [] });
  const text = formatInspectText(result);
  assertStringIncludes(text, "(none)");
  assertStringIncludes(text, "0/4");
});

Deno.test("inspect/I1: text output shows subtype when present", () => {
  const result = mockInspectResult({ subtype: "library" });
  const text = formatInspectText(result);
  assertStringIncludes(text, "Subtype: library");
});

Deno.test("inspect/I1: text output shows template status", () => {
  const result = mockInspectResult({ isTemplate: true });
  const text = formatInspectText(result);
  assertStringIncludes(text, "Template: true");
});

// ---------------------------------------------------------------------------
// I2: formatInspectJson — unit tests
// ---------------------------------------------------------------------------

Deno.test("inspect/I2: JSON output is valid JSON", () => {
  const result = mockInspectResult();
  const json = formatInspectJson(result);
  const parsed = JSON.parse(json);
  assertEquals(parsed.format, "go");
  assertEquals(parsed.lineCount, 100);
  assertEquals(parsed.blocks.length, 4);
});

Deno.test("inspect/I2: JSON output preserves all fields", () => {
  const result = mockInspectResult({ subtype: "library" });
  const json = formatInspectJson(result);
  const parsed = JSON.parse(json);
  assertEquals(parsed.subtype, "library");
  assertEquals(parsed.sections.length, 2);
  assertEquals(parsed.content.length, 2);
  assertEquals(parsed.directives["//omni:key"], "B-test-example");
});

// ---------------------------------------------------------------------------
// I3: --inspect CLI integration — Go handler
// ---------------------------------------------------------------------------

Deno.test("inspect/I3: --inspect shows parsed structure for Go file", async () => {
  const { code, stdout } = await runCli([
    "lint", "go", "tests/fixtures/go/structure/valid-library.go", "--inspect",
  ]);
  assertEquals(code, 0, "Inspect should exit 0");
  assertStringIncludes(stdout, "File:");
  assertStringIncludes(stdout, "Blocks detected");
  assertStringIncludes(stdout, "METADATA");
  assertStringIncludes(stdout, "SETUP");
  assertStringIncludes(stdout, "BODY");
  assertStringIncludes(stdout, "CLOSING");
});

Deno.test("inspect/I3: --inspect does NOT show lint results", async () => {
  const { stdout } = await runCli([
    "lint", "go", "tests/fixtures/go/structure/missing-block.go", "--inspect",
  ]);
  // Inspect shows structure, not lint errors
  assert(!stdout.includes("[E]"), "Inspect should not show error markers");
  assert(!stdout.includes("[W]"), "Inspect should not show warning markers");
  assertStringIncludes(stdout, "Blocks detected");
});

Deno.test("inspect/I3: --inspect --json produces valid JSON", async () => {
  const { code, stdout } = await runCli([
    "lint", "go", "tests/fixtures/go/structure/valid-library.go", "--inspect", "--json",
  ]);
  assertEquals(code, 0);
  const parsed = JSON.parse(stdout);
  assertEquals(parsed.format, "go");
  assert(parsed.blocks.length > 0, "Should have blocks");
});

// ---------------------------------------------------------------------------
// I4: --inspect CLI integration — Rust handler
// ---------------------------------------------------------------------------

Deno.test("inspect/I4: --inspect shows parsed structure for Rust file", async () => {
  const { code, stdout } = await runCli([
    "lint", "rust", "tests/fixtures/rust/structure/valid-module.rs", "--inspect",
  ]);
  assertEquals(code, 0, "Inspect should exit 0");
  assertStringIncludes(stdout, "File:");
  assertStringIncludes(stdout, "rust");
  assertStringIncludes(stdout, "Blocks detected");
});

// ---------------------------------------------------------------------------
// W1: --why CLI integration
// ---------------------------------------------------------------------------

Deno.test("inspect/W1: --why shows reasoning chain for errors", async () => {
  const { stdout } = await runCli([
    "lint", "go", "tests/fixtures/go/structure/missing-block.go", "--why",
  ]);
  assertStringIncludes(stdout, "why:");
  assertStringIncludes(stdout, "fix:");
  assertStringIncludes(stdout, "src:");
});

Deno.test("inspect/W1: --why shows layer info", async () => {
  const { stdout } = await runCli([
    "lint", "go", "tests/fixtures/go/structure/missing-block.go", "--why",
  ]);
  // --why format: "why: EXISTENCE — Does the file have the right shape?"
  // Layer name appears directly, R50 appears in error code references
  assertStringIncludes(stdout, "EXISTENCE");
  assertStringIncludes(stdout, "R50");
});

Deno.test("inspect/W1: --why shows check function name", async () => {
  const { stdout } = await runCli([
    "lint", "go", "tests/fixtures/go/structure/missing-block.go", "--why",
  ]);
  // At least one result should trace to a check function
  assert(stdout.includes("()"), "Should show check function name with ()");
});

// ---------------------------------------------------------------------------
// W2: --why --json integration
// ---------------------------------------------------------------------------

Deno.test("inspect/W2: --why --json enriches results with reasoning", async () => {
  const { stdout } = await runCli([
    "lint", "go", "tests/fixtures/go/structure/missing-block.go", "--why", "--json",
  ]);
  const parsed = JSON.parse(stdout);
  const files = parsed.files;
  assert(files.length > 0, "Should have file results");

  // Find a result with a why chain
  const allResults = files.flatMap((f: { results: unknown[] }) => f.results);
  const withWhy = allResults.filter((r: { why?: unknown }) => r.why !== null);
  assert(withWhy.length > 0, "At least one result should have a why chain");

  // Check why structure
  const first = withWhy[0]!.why;
  assert(typeof first.layer === "number", "why.layer should be a number");
  assert(typeof first.layerName === "string", "why.layerName should be a string");
  assert(typeof first.category === "string", "why.category should be a string");
  assert(typeof first.suggestion === "string", "why.suggestion should be a string");
  assert(typeof first.checkFunction === "string", "why.checkFunction should be a string");
});

Deno.test("inspect/W2: --json without --why does NOT include why field", async () => {
  const { stdout } = await runCli([
    "lint", "go", "tests/fixtures/go/structure/missing-block.go", "--json",
  ]);
  const parsed = JSON.parse(stdout);
  const allResults = parsed.files.flatMap((f: { results: unknown[] }) => f.results);
  const withWhy = allResults.filter((r: { why?: unknown }) => r.why !== undefined);
  assertEquals(withWhy.length, 0, "Without --why, no why field should be present");
});

// ---------------------------------------------------------------------------
// W3: --why on clean file
// ---------------------------------------------------------------------------

Deno.test("inspect/W3: --why on clean file shows no results", async () => {
  const { stdout } = await runCli([
    "lint", "go", "tests/fixtures/go/structure/valid-library.go", "--why",
  ]);
  // A valid file should show OK, no error lines with why chains
  assertStringIncludes(stdout, "OK");
  assert(!stdout.includes("why: Layer"), "Clean file should not have why annotations");
});

// ============================================================================
// CLOSING
// ============================================================================
//
// Inspect/why tests — verify that --inspect shows structure without judgment
// and --why traces each result to its data layer source.
//
// "The hearing ear, and the seeing eye, the LORD hath made even both of them."
// — Proverbs 20:12
// ============================================================================
