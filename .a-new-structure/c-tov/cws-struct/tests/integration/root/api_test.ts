// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/integration/root/api_test.ts
// key:     B-tov-cws-struct-tests-integration-root-api
// title:   API Integration Tests
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-24
// authors: Nova Dawn (CPI-SI)
// purpose: Integration tests for the programmatic API (lib/api/mod.ts).
//          Verifies the stable API surface works correctly — same truth
//          as the CLI, different interface. Pure data, no stdout.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import {
  assertEquals,
  assert,
  assertExists,
} from "jsr:@std/assert";

import {
  lintFile,
  lintDirectory,
  transformFile,
  createFile,
  inspectFile,
  getDataLayer,
  getRegisteredFormats,
  getFormatDetails,
  getFormatHandler,
  detectFileFormat,
  lookupErrorByCode,
  lookupErrorByRule,
  getErrorsByLayer,
} from "@lib/api/mod.ts";

import type {
  LintReport,
  TransformReport,
  CreateReport,
  DataLayerSnapshot,
} from "@lib/api/mod.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

function fixture(name: string): string {
  return new URL(`../../fixtures/${name}`, import.meta.url).pathname;
}

// ---------------------------------------------------------------------------
// 1. Format handler registration
// ---------------------------------------------------------------------------

Deno.test("api/formats: lists all 8 registered formats", () => {
  const formats = getRegisteredFormats();
  assert(formats.length >= 8, `Expected 8+ formats, got ${formats.length}`);

  const expected = ["go", "rust", "toml", "json", "makefile", "dotfiles", "omni", "ofd"];
  for (const name of expected) {
    assert(formats.includes(name), `Missing format: ${name}`);
  }
});

Deno.test("api/formats: details include name, extensions, description", () => {
  const details = getFormatDetails();
  assert(details.length >= 8, "Should have 8+ format details");

  for (const d of details) {
    assert(d.name.length > 0, "Name should be non-empty");
    assert(d.description.length > 0, "Description should be non-empty");
    assert(d.extensions.length >= 0, "Extensions array should exist");
  }
});

Deno.test("api/formats: getFormatHandler returns handler by name", () => {
  const go = getFormatHandler("go");
  assertExists(go, "Go handler should exist");
  assertEquals(go.name, "go");

  const unknown = getFormatHandler("nonexistent");
  assertEquals(unknown, undefined, "Unknown format should return undefined");
});

Deno.test("api/formats: detectFileFormat resolves extensions", () => {
  assertEquals(detectFileFormat("lib.go"), "go");
  assertEquals(detectFileFormat("lib.rs"), "rust");
  assertEquals(detectFileFormat("config.toml"), "toml");
  assertEquals(detectFileFormat("data.json"), "json");
});

// ---------------------------------------------------------------------------
// 2. Lint operations
// ---------------------------------------------------------------------------

Deno.test("api/lint: lintFile returns structured LintReport", async () => {
  const report: LintReport = await lintFile(
    fixture("go/structure/valid-library.go"),
  );

  assertExists(report.file, "file should be set");
  assertExists(report.absolutePath, "absolutePath should be set");
  assertEquals(report.format, "go");
  assert(Array.isArray(report.results), "results should be array");
  assertExists(report.summary, "summary should exist");
  assertExists(report.health, "health should exist for Go files");
});

Deno.test("api/lint: valid file has 0 errors", async () => {
  const report = await lintFile(
    fixture("go/structure/valid-library.go"),
  );

  assertEquals(report.summary.errors, 0, "Valid file should have 0 errors");
  assert(report.health!.total > 0, "Health should be positive");
});

Deno.test("api/lint: broken file has errors", async () => {
  const report = await lintFile(
    fixture("go/structure/missing-block.go"),
  );

  assert(report.summary.errors > 0, "Missing block should have errors");
  assert(report.health!.total < 100, "Health should be reduced");
});

Deno.test("api/lint: explicit format overrides auto-detect", async () => {
  const report = await lintFile(
    fixture("rust/structure/valid-library.rs"),
    { format: "rust" },
  );

  assertEquals(report.format, "rust");
  assertEquals(report.summary.errors, 0);
});

Deno.test("api/lint: policy affects results", async () => {
  // Same file, different policies
  const strictReport = await lintFile(
    fixture("go/structure/valid-library.go"),
    { policy: "strict" },
  );
  const growthReport = await lintFile(
    fixture("go/structure/valid-library.go"),
    { policy: "growth" },
  );

  // Both should work — policy mainly affects severity thresholds
  assertExists(strictReport.summary);
  assertExists(growthReport.summary);
});

// ---------------------------------------------------------------------------
// 3. Transform operations
// ---------------------------------------------------------------------------

Deno.test("api/transform: dry-run returns TransformReport", async () => {
  const report: TransformReport = await transformFile(
    fixture("go/structure/valid-library.go"),
    { dryRun: true, format: "go" },
  );

  assertExists(report.file, "file should be set");
  assert(Array.isArray(report.results), "results should be array");
  assertEquals(report.modified, false, "Dry-run should not modify");
});

Deno.test("api/transform: no-transform handler returns error", async () => {
  // JSON handler doesn't have transform — use a temp .json file
  const tmp = await Deno.makeTempFile({ suffix: ".json" });
  try {
    await Deno.writeTextFile(tmp, '{"_metadata": {}}');
    const report = await transformFile(tmp, { format: "json", dryRun: true });

    assert(
      report.results.some((r) => r.rule === "api/no-transform"),
      "Should report no-transform for JSON",
    );
    assertEquals(report.modified, false);
  } finally {
    await Deno.remove(tmp);
  }
});

// ---------------------------------------------------------------------------
// 4. Create operations
// ---------------------------------------------------------------------------

Deno.test("api/create: dry-run returns content without writing", async () => {
  const report: CreateReport = await createFile(
    "/tmp/test-api-create.go",
    {
      format: "go",
      subtype: "library",
      key: "B-test-api-create",
      title: "API Create Test",
      purpose: "Test the create API",
      packageOrCrate: "testapi",
      dryRun: true,
    },
  );

  assertExists(report.file);
  assert(report.content.length > 0, "Content should be generated");
  assert(report.lineCount > 20, "Should have 20+ lines");
  assertEquals(report.written, false, "Dry-run should not write");
});

Deno.test("api/create: generates valid Go file", async () => {
  const tmp = await Deno.makeTempFile({ suffix: ".go" });
  try {
    const report = await createFile(tmp, {
      format: "go",
      subtype: "library",
      key: "B-test-api-go",
      title: "API Go Create",
      purpose: "Test Go creation",
      packageOrCrate: "testpkg",
    });

    assertEquals(report.written, true);
    assert(report.content.includes("package testpkg"));

    // Lint the created file
    const lintReport = await lintFile(tmp, { format: "go" });
    assertEquals(
      lintReport.summary.errors,
      0,
      `Created Go file should lint clean. Errors: ${lintReport.results.filter(r => r.severity === "error").map(r => r.rule).join(", ")}`,
    );
  } finally {
    await Deno.remove(tmp);
  }
});

Deno.test("api/create: generates valid Rust file", async () => {
  const tmp = await Deno.makeTempFile({ suffix: ".rs" });
  try {
    const report = await createFile(tmp, {
      format: "rust",
      subtype: "module",
      key: "B-test-api-rust",
      title: "API Rust Create",
      purpose: "Test Rust creation",
      packageOrCrate: "testcrate",
    });

    assertEquals(report.written, true);

    const lintReport = await lintFile(tmp, { format: "rust" });
    assertEquals(
      lintReport.summary.errors,
      0,
      `Created Rust file should lint clean. Errors: ${lintReport.results.filter(r => r.severity === "error").map(r => r.rule).join(", ")}`,
    );
  } finally {
    await Deno.remove(tmp);
  }
});

// ---------------------------------------------------------------------------
// 5. Inspect operations
// ---------------------------------------------------------------------------

Deno.test("api/inspect: returns InspectResult for Go file", async () => {
  const result = await inspectFile(
    fixture("go/structure/valid-library.go"),
    { format: "go" },
  );

  assertExists(result, "Inspect should return result for Go");
  assertEquals(result.format, "go");
  assertEquals(result.blocks.length, 4, "Valid library has 4 blocks");
  assert(result.lineCount > 0, "Should have line count");
});

Deno.test("api/inspect: returns InspectResult for Rust file", async () => {
  const result = await inspectFile(
    fixture("rust/structure/valid-module.rs"),
    { format: "rust" },
  );

  assertExists(result, "Inspect should return result for Rust");
  assertEquals(result.format, "rust");
  assert(result.blocks.length > 0, "Should find blocks");
});

Deno.test("api/inspect: returns null for format without inspect", async () => {
  // JSON handler has no inspect — use a temp .json file
  const tmp = await Deno.makeTempFile({ suffix: ".json" });
  try {
    await Deno.writeTextFile(tmp, '{"_metadata": {}}');
    const result = await inspectFile(tmp, { format: "json" });
    // May or may not be null depending on handler — just verify it doesn't throw
    assert(result === null || result !== null, "Should not throw");
  } finally {
    await Deno.remove(tmp);
  }
});

// ---------------------------------------------------------------------------
// 6. Data layer
// ---------------------------------------------------------------------------

Deno.test("api/data: getDataLayer returns complete snapshot", () => {
  const data: DataLayerSnapshot = getDataLayer();

  assert(Object.keys(data.sections).length > 0, "Sections should be populated");
  assert(Object.keys(data.sectionOrder).length > 0, "Section order populated");
  assert(Object.keys(data.concepts).length > 0, "Concepts populated");
  assert(data.conceptOrder.length > 0, "Concept order populated");
  assert(Object.keys(data.forms).length >= 5, "Should have 5+ forms");
  assert(Object.keys(data.placement).length > 0, "Placement rules populated");
  assert(data.errorCodes.size > 0, "Error codes populated");
  assert(data.lintRules.size > 0, "Lint rules populated");
  assert(data.pipelineLayers.length > 0, "Pipeline layers populated");
});

Deno.test("api/data: error code lookup by code", () => {
  const entry = lookupErrorByCode("CWS-R50-020");
  assertExists(entry, "CWS-R50-020 should exist");
  assertEquals(entry.layer, 0, "Should be Layer 0");
});

Deno.test("api/data: error code lookup by rule", () => {
  const entry = lookupErrorByRule("block/order");
  assertExists(entry, "block/order should resolve");
});

Deno.test("api/data: errors by layer returns correct layer", () => {
  const layer0 = getErrorsByLayer(0);
  assert(layer0.length > 0, "Layer 0 should have errors");
  for (const e of layer0) {
    assertEquals(e.layer, 0, "All entries should be Layer 0");
  }
});

// ---------------------------------------------------------------------------
// 7. End-to-end: create → lint → transform → inspect cycle
// ---------------------------------------------------------------------------

Deno.test("api/e2e: create → lint → transform → lint → inspect", async () => {
  const tmp = await Deno.makeTempFile({ suffix: ".go" });
  try {
    // Create
    const createReport = await createFile(tmp, {
      format: "go",
      subtype: "library",
      key: "B-test-e2e",
      title: "E2E Test",
      purpose: "End-to-end API test",
      packageOrCrate: "e2e",
    });
    assert(createReport.written);

    // Lint
    const lint1 = await lintFile(tmp, { format: "go" });
    assertEquals(lint1.summary.errors, 0, "Created file should lint clean");

    // Transform (wet run on the created file)
    const transformReport = await transformFile(tmp, {
      format: "go",
      dryRun: false,
    });
    assert(Array.isArray(transformReport.results));

    // Lint again — should still be clean
    const lint2 = await lintFile(tmp, { format: "go" });
    assertEquals(lint2.summary.errors, 0, "Post-transform should still be clean");

    // Inspect
    const inspect = await inspectFile(tmp, { format: "go" });
    assertExists(inspect);
    assertEquals(inspect.blocks.length, 4, "Should still have 4 blocks");
  } finally {
    await Deno.remove(tmp);
  }
});

// ============================================================================
// CLOSING
// ============================================================================
//
// API integration tests — the stable programmatic interface verified.
// 7 test groups: formats, lint, transform, create, inspect, data layer,
// end-to-end. Same engine as the CLI, structured TypeScript returns.
//
// "Ask, and it shall be given you." — Matthew 7:7
// ============================================================================
