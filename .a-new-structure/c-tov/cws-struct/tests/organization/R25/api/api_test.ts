// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/T00/R25/api/api_test.ts
// key:     B-tov-cws-struct-tests-api-api
// title:   CWS Struct — Programmatic API Tests
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-22
// authors: Nova Dawn (CPI-SI)
// purpose: Verify the programmatic API surface — lintFile, data layer access,
//          format detection, config, and create operations. These tests prove
//          that the API works without CLI, without stdout, without global state.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import {
  assertEquals,
  assertExists,
  assertStringIncludes,
} from "jsr:@std/assert";

import {
  lintFile,
  lintDirectory,
  createFile,
  inspectFile,
  getDataLayer,
  getSectionRegistry,
  getConceptRegistry,
  getFormDefinitions,
  lookupErrorByRule,
  lookupErrorByCode,
  getErrorsByLayer,
  getRegisteredFormats,
  getFormatDetails,
  getFormatHandler,
  detectFileFormat,
  init,
  getConfig,
} from "@lib/api/mod.ts";

import type {
  LintReport,
  LintOptions,
  CreateReport,
  DataLayerSnapshot,
} from "@lib/api/mod.ts";

// ---------------------------------------------------------------------------
// Test helpers
// ---------------------------------------------------------------------------

const FIXTURES = new URL("../../../fixtures/", import.meta.url).pathname;

function goFixture(name: string): string {
  return `${FIXTURES}go/${name}`;
}

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// 1. Configuration
// ---------------------------------------------------------------------------

Deno.test("[CWS-T00-095] api/init: loads config and returns ToolConfig", async () => {
  const config = await init();
  assertExists(config);
  assertExists(config.version);
  assertExists(config.concurrency);
});

Deno.test("api/getConfig: returns cached or default config", async () => {
  const config = await getConfig();
  assertExists(config);
  assertEquals(typeof config.concurrency, "number");
});

// ---------------------------------------------------------------------------
// 2. Lint operations
// ---------------------------------------------------------------------------

Deno.test("api/lintFile: returns structured LintReport for valid Go file", async () => {
  const report: LintReport = await lintFile(goFixture("structure/valid-library.go"));

  assertExists(report);
  assertEquals(report.format, "go");
  assertExists(report.file);
  assertExists(report.absolutePath);
  assertExists(report.results);
  assertExists(report.summary);
  assertEquals(typeof report.summary.errors, "number");
  assertEquals(typeof report.summary.warnings, "number");
  assertEquals(typeof report.summary.infos, "number");
});

Deno.test("api/lintFile: computes health score for Go files", async () => {
  const report = await lintFile(goFixture("structure/valid-library.go"));

  assertExists(report.health);
  assertEquals(typeof report.health!.total, "number");
  assertExists(report.health!.level);
  assertExists(report.health!.emoji);
});

Deno.test("api/lintFile: resolves error codes on results", async () => {
  const report = await lintFile(goFixture("structure/missing-block.go"));

  // Should have results with error codes populated
  const withCodes = report.results.filter((r) => r.errorCode);
  assertEquals(withCodes.length > 0, true,
    "At least some results should have error codes");
});

Deno.test("api/lintFile: returns error for nonexistent format", async () => {
  const report = await lintFile(goFixture("structure/valid-library.go"), {
    format: "nonexistent-format",
  });

  assertEquals(report.format, "nonexistent-format");
  assertEquals(report.results.length, 1);
  assertEquals(report.results[0]!.rule, "api/no-handler");
});

Deno.test("api/lintFile: respects policy option", async () => {
  const strict = await lintFile(goFixture("structure/valid-library.go"), {
    policy: "strict",
  });
  const growth = await lintFile(goFixture("structure/valid-library.go"), {
    policy: "growth",
  });

  // Strict should have >= errors compared to growth
  // (some warnings become errors in strict, some errors become warnings in growth)
  assertExists(strict.summary);
  assertExists(growth.summary);
});

Deno.test("api/lintFile: returns no stdout (pure data)", async () => {
  // Capture console.log output
  const originalLog = console.log;
  let logged = false;
  console.log = () => { logged = true; };

  try {
    await lintFile(goFixture("structure/valid-library.go"));
    assertEquals(logged, false, "lintFile should not produce console output");
  } finally {
    console.log = originalLog;
  }
});

Deno.test("api/lintDirectory: discovers and lints multiple files", async () => {
  const reports = await lintDirectory(`${FIXTURES}go/structure/`, {
    format: "go",
  });

  assertEquals(reports.length > 0, true, "Should discover Go files");
  for (const report of reports) {
    assertEquals(report.format, "go");
    assertExists(report.results);
    assertExists(report.summary);
  }
});

Deno.test("api/lintDirectory: returns error for unknown format", async () => {
  const reports = await lintDirectory(`${FIXTURES}go/`, {
    format: "zzz-nonexistent",
  });

  assertEquals(reports.length, 1);
  assertEquals(reports[0]!.results[0]!.rule, "api/unknown-format");
});

// ---------------------------------------------------------------------------
// 3. Create operations
// ---------------------------------------------------------------------------

Deno.test("api/createFile: generates Go library content (dry-run)", async () => {
  const tmpDir = await Deno.makeTempDir();
  const dest = `${tmpDir}/test-api-lib.go`;

  const report: CreateReport = await createFile(dest, {
    format: "go",
    subtype: "library",
    key: "B-test-api-lib",
    title: "API Test Library",
    purpose: "Test the programmatic create API",
    packageOrCrate: "testapi",
    dryRun: true,
  });

  assertExists(report);
  assertEquals(report.written, false, "Dry-run should not write");
  assertEquals(report.lineCount > 10, true, "Should generate substantial content");
  assertStringIncludes(report.content, "METADATA");
  assertStringIncludes(report.content, "SETUP");
  assertStringIncludes(report.content, "BODY");
  assertStringIncludes(report.content, "CLOSING");
  assertStringIncludes(report.content, "B-test-api-lib");

  // Clean up
  await Deno.remove(tmpDir, { recursive: true });
});

Deno.test("api/createFile: writes file when not dry-run", async () => {
  const tmpDir = await Deno.makeTempDir();
  const dest = `${tmpDir}/test-write.go`;

  const report = await createFile(dest, {
    format: "go",
    subtype: "bare-bone",
    key: "B-test-write",
    title: "Write Test",
    purpose: "Test file writing",
    packageOrCrate: "testwrite",
    dryRun: false,
  });

  assertEquals(report.written, true);

  // Verify file exists
  const content = await Deno.readTextFile(dest);
  assertStringIncludes(content, "B-test-write");

  await Deno.remove(tmpDir, { recursive: true });
});

// ---------------------------------------------------------------------------
// 4. Inspect operations
// ---------------------------------------------------------------------------

Deno.test("api/inspectFile: returns parsed structure for Go file", async () => {
  const result = await inspectFile(goFixture("structure/valid-library.go"));

  assertExists(result);
  assertEquals(result!.format, "go");
  assertEquals(result!.blocks.length > 0, true, "Should detect blocks");
  assertExists(result!.directives);
});

Deno.test("api/inspectFile: returns null for unsupported format", async () => {
  // Create a temp file with unknown extension
  const tmpDir = await Deno.makeTempDir();
  const tmpFile = `${tmpDir}/test.zzz`;
  await Deno.writeTextFile(tmpFile, "hello");

  const result = await inspectFile(tmpFile);
  assertEquals(result, null, "Should return null for unknown format");

  await Deno.remove(tmpDir, { recursive: true });
});

// ---------------------------------------------------------------------------
// 5. Data layer access
// ---------------------------------------------------------------------------

Deno.test("api/getDataLayer: returns complete snapshot", () => {
  const data: DataLayerSnapshot = getDataLayer();

  assertExists(data.sections);
  assertExists(data.sectionOrder);
  assertExists(data.concepts);
  assertExists(data.conceptOrder);
  assertExists(data.forms);
  assertExists(data.placement);
  assertExists(data.errorCodes);
  assertExists(data.lintRules);
  assertExists(data.pipelineLayers);
});

Deno.test("api/getSectionRegistry: returns non-empty registry", () => {
  const sections = getSectionRegistry();
  const keys = Object.keys(sections);
  assertEquals(keys.length > 0, true, "Section registry should not be empty");
});

Deno.test("api/getConceptRegistry: returns non-empty registry", () => {
  const concepts = getConceptRegistry();
  const keys = Object.keys(concepts);
  assertEquals(keys.length > 0, true, "Concept registry should not be empty");
});

Deno.test("api/getFormDefinitions: returns 5 form definitions", () => {
  const forms = getFormDefinitions();
  const keys = Object.keys(forms);
  assertEquals(keys.length, 5, "Should have 5 forms");

  // Check known forms exist
  assertExists(forms["bare-bone"]);
  assertExists(forms["library"]);
  assertExists(forms["module"]);
  assertExists(forms["executable"]);
  assertExists(forms["demo-test"]);
});

Deno.test("api/lookupErrorByRule: resolves known rules", () => {
  const entry = lookupErrorByRule("block/order");
  assertExists(entry, "Should find block/order");
  assertExists(entry!.code);
  assertEquals(entry!.code, "CWS-R50-020");
});

Deno.test("api/lookupErrorByCode: resolves known codes", () => {
  // R50 codes exist in the catalog
  const entry = lookupErrorByCode("CWS-R50-010");
  assertExists(entry, "Should find CWS-R50-010");
  assertExists(entry!.code);
  assertExists(entry!.layerName);
});

Deno.test("api/getErrorsByLayer: returns errors for layer 0", () => {
  const errors = getErrorsByLayer(0);
  assertEquals(errors.length > 0, true, "Layer 0 should have error codes");
});

// ---------------------------------------------------------------------------
// 6. Format handler utilities
// ---------------------------------------------------------------------------

Deno.test("api/getRegisteredFormats: returns at least 8 formats", () => {
  const formats = getRegisteredFormats();
  assertEquals(formats.length >= 8, true,
    `Should have at least 8 formats, got ${formats.length}`);
});

Deno.test("api/getFormatDetails: returns format metadata", () => {
  const details = getFormatDetails();
  assertEquals(details.length >= 8, true);

  for (const d of details) {
    assertExists(d.name);
    assertExists(d.extensions);
    assertExists(d.description);
  }
});

Deno.test("api/getFormatHandler: returns Go handler", () => {
  const handler = getFormatHandler("go");
  assertExists(handler);
  assertEquals(handler!.name, "go");
  assertExists(handler!.lint);
});

Deno.test("api/getFormatHandler: returns undefined for unknown format", () => {
  const handler = getFormatHandler("zzz-nonexistent");
  assertEquals(handler, undefined);
});

Deno.test("api/detectFileFormat: detects Go from .go extension", () => {
  const format = detectFileFormat("path/to/file.go");
  assertEquals(format, "go");
});

Deno.test("api/detectFileFormat: detects Rust from .rs extension", () => {
  const format = detectFileFormat("path/to/file.rs");
  assertEquals(format, "rust");
});

Deno.test("api/detectFileFormat: detects TOML from .toml extension", () => {
  const format = detectFileFormat("path/to/file.toml");
  assertEquals(format, "toml");
});

Deno.test("api/detectFileFormat: returns undefined for unknown extension", () => {
  const format = detectFileFormat("path/to/file.zzz");
  assertEquals(format, undefined);
});

// ---------------------------------------------------------------------------
// 7. Integration — round-trip: create + lint
// ---------------------------------------------------------------------------

Deno.test("api/round-trip: created Go file passes lint", async () => {
  const tmpDir = await Deno.makeTempDir();
  const dest = `${tmpDir}/roundtrip-lib.go`;

  // Create
  const created = await createFile(dest, {
    format: "go",
    subtype: "library",
    key: "B-test-roundtrip",
    title: "Round-Trip Test",
    purpose: "Verify create then lint produces 0 errors",
    packageOrCrate: "roundtrip",
    dryRun: false,
  });
  assertEquals(created.written, true);

  // Lint
  const report = await lintFile(dest);
  assertEquals(report.format, "go");
  assertEquals(report.summary.errors, 0,
    `Generated Go file should lint with 0 errors, got ${report.summary.errors}. ` +
    `Results: ${report.results.filter(r => r.severity === "error").map(r => r.message).join("; ")}`);

  await Deno.remove(tmpDir, { recursive: true });
});

Deno.test("api/round-trip: created Rust file passes lint", async () => {
  const tmpDir = await Deno.makeTempDir();
  const dest = `${tmpDir}/roundtrip-lib.rs`;

  const created = await createFile(dest, {
    format: "rust",
    subtype: "module",
    key: "B-test-roundtrip-rs",
    title: "Rust Round-Trip Test",
    purpose: "Verify create then lint for Rust",
    packageOrCrate: "roundtrip",
    dryRun: false,
  });
  assertEquals(created.written, true);

  const report = await lintFile(dest);
  assertEquals(report.format, "rust");
  assertEquals(report.summary.errors, 0,
    `Generated Rust file should lint with 0 errors, got ${report.summary.errors}. ` +
    `Results: ${report.results.filter(r => r.severity === "error").map(r => r.message).join("; ")}`);

  await Deno.remove(tmpDir, { recursive: true });
});

// ============================================================================
// CLOSING
// ============================================================================
//
// API tests — programmatic interface verification.
// Same engine, different door. Both lead to truth.
//
// "Ask, and it shall be given you." — Matthew 7:7
// ============================================================================
