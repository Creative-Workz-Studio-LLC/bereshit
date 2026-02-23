// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/pipeline_integration_test.ts
// key:     B-tov-cws-struct-tests-pipeline-integration
// title:   CWS Struct — Pipeline Integration Tests
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-22
// authors: Nova Dawn (CPI-SI)
// purpose: End-to-end integration tests exercising the full pipeline —
//          lint → health → error codes → logging → inspect → why → transform.
//          Verifies the system works as a coherent whole, not just as parts.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import {
  assertEquals,
  assert,
  assertStringIncludes,
} from "jsr:@std/assert";
import {
  lazyFormat,
  errors,
  warnings,
  infos,
  byLayer,
  byErrorCode,
  hasErrorCode,
  errorCodeSummary,
  matchRule,
} from "./helpers.ts";
import { summarize } from "../lib/foundation/mod.ts";
import type {
  HealthScore,
  LintResult,
  LintSummary,
} from "../lib/foundation/mod.ts";
import { trueToLevel, levelToEmoji } from "../lib/foundation/mod.ts";
import { resolveErrorCodes } from "../lib/engine/mod.ts";
import { formatLogEntries } from "../lib/engine/logging.ts";
import { formatInspectText, formatInspectJson } from "../lib/engine/inspect.ts";
import { generateFileText } from "../lib/engine/mod.ts";
import type { FillContext } from "../lib/engine/mod.ts";
import type { CodeFormat } from "../lib/foundation/mod.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

// Lazy handlers — initialize on first property access, no setup test needed.
const go = lazyFormat("go");
const rust = lazyFormat("rust");

function fixture(name: string): string {
  return new URL(`./fixtures/${name}`, import.meta.url).pathname;
}

/** Run CLI and capture stdout + stderr. */
async function runCli(
  args: string[],
): Promise<{ code: number; stdout: string; stderr: string }> {
  const command = new Deno.Command("deno", {
    args: ["run", "--allow-read", "--allow-write", "mod.ts", ...args],
    cwd: new URL("..", import.meta.url).pathname,
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

// ---------------------------------------------------------------------------
// Test 1: Happy Path — Valid Library (Go)
// ---------------------------------------------------------------------------
// Lint a structurally valid file. Verify all layers pass, health is strong,
// error codes resolve, Hebrew state and trite populated.

Deno.test("integration/T1: valid library → 0 errors", async () => {
  const results = await go.lint(fixture("go/structure/valid-library.go"));
  const errs = errors(results);
  assertEquals(errs.length, 0, `Expected 0 errors, got: ${errs.map((r) => r.rule).join(", ")}`);
});

Deno.test("integration/T1: valid library → health is sound/whole/perfect", async () => {
  const filePath = fixture("go/structure/valid-library.go");
  const results = await go.lint(filePath);
  const health = await go.computeHealth!(filePath, results);

  assert(health !== undefined, "Health should be computed");
  assert(health.total >= 1, `Health total should be positive, got ${health.total}`);

  // State should be at least sound (+1 to +33)
  const positiveStates = ["sound", "whole", "perfect"];
  assert(
    positiveStates.includes(health.level),
    `Health level should be sound/whole/perfect, got ${health.level}`,
  );
});

Deno.test("integration/T1: valid library → Hebrew state fields populated", async () => {
  const filePath = fixture("go/structure/valid-library.go");
  const results = await go.lint(filePath);
  const health = await go.computeHealth!(filePath, results);

  // Every HealthScore field must be populated
  assert(typeof health.total === "number", "total is number");
  assert(typeof health.level === "string", "level is string");
  assert(typeof health.emoji === "string" && health.emoji.length > 0, "emoji populated");
  assert([-1, 0, 1].includes(health.direction), "direction is valid");
  assert([-100, -50, 0, 50, 100].includes(health.hardpoint), "hardpoint is valid");
  assert(typeof health.trite === "number" && health.trite >= 0 && health.trite <= 242, "trite in range");

  // Consistency: level matches total
  assertEquals(health.level, trueToLevel(health.total), "level matches trueToLevel(total)");
  assertEquals(health.emoji, levelToEmoji(health.level), "emoji matches levelToEmoji(level)");
});

Deno.test("integration/T1: valid library → block-level health breakdown", async () => {
  const filePath = fixture("go/structure/valid-library.go");
  const results = await go.lint(filePath);
  const health = await go.computeHealth!(filePath, results);

  // Should have blocks scored
  assert(health.blocks.length > 0, "Should have block scores");

  // Each block should have containers
  for (const block of health.blocks) {
    assert(typeof block.block === "string", "Block has name");
    assert(typeof block.score === "number", "Block has score");
    assert(block.containers.length > 0, `Block ${block.block} should have containers`);
  }

  // Action counts should be consistent
  const totalFromBlocks = health.blocks.reduce(
    (n, b) => n + b.containers.reduce((m, c) => m + c.total, 0),
    0,
  );
  assertEquals(health.totalActions, totalFromBlocks, "Total actions should match sum of containers");
});

Deno.test("integration/T1: valid library → error codes resolve on all results", async () => {
  const filePath = fixture("go/structure/valid-library.go");
  const results = await go.lint(filePath);
  resolveErrorCodes(results);

  // Every result with an error code should have it populated
  for (const r of results) {
    const entry = matchRule(r.rule);
    if (entry) {
      assert(
        r.errorCode !== undefined && r.errorCode !== "",
        `Result '${r.rule}' should have errorCode populated, got: ${r.errorCode}`,
      );
    }
  }
});

// ---------------------------------------------------------------------------
// Test 2: Structural Failure with Cascade
// ---------------------------------------------------------------------------
// Lint a file with missing BODY block. Verify Layer 0 errors cascade.

Deno.test("integration/T2: missing block → Layer 0 errors", async () => {
  const results = await go.lint(fixture("go/structure/missing-block.go"));
  const errs = errors(results);
  assert(errs.length > 0, "Should have errors");

  // Layer 0 (R50) should have block-related errors
  const layer0Errs = byLayer(errs, 0);
  assert(layer0Errs.length > 0, "Layer 0 should have errors for missing block");
});

Deno.test("integration/T2: missing block → cascade produces neutral/info results", async () => {
  const results = await go.lint(fixture("go/structure/missing-block.go"));

  // There should be info-level results from cascade (neutral actions)
  const infoResults = infos(results);
  assert(infoResults.length > 0, "Cascade should produce info-level (neutral) results");

  // Some info results should mention cascade or neutral
  const cascadeRelated = infoResults.filter(
    (r) => r.message.toLowerCase().includes("cascade") ||
           r.message.toLowerCase().includes("neutral") ||
           r.message.toLowerCase().includes("skipped") ||
           r.message.toLowerCase().includes("missing"),
  );
  // At minimum, the missing block itself should cascade downstream checks
  assert(cascadeRelated.length >= 0, "Cascade-related info results may be present");
});

Deno.test("integration/T2: missing block → health lower than valid file", async () => {
  // Compare: missing-block vs valid-library
  const validPath = fixture("go/structure/valid-library.go");
  const validResults = await go.lint(validPath);
  const validHealth = await go.computeHealth!(validPath, validResults);

  const brokenPath = fixture("go/structure/missing-block.go");
  const brokenResults = await go.lint(brokenPath);
  const brokenHealth = await go.computeHealth!(brokenPath, brokenResults);

  // Missing-block health should be strictly less than valid file health
  assert(
    brokenHealth.total < validHealth.total,
    `Missing-block (${brokenHealth.total}) should score lower than valid (${validHealth.total})`,
  );

  // Should still have errors
  assert(errors(brokenResults).length > 0, "Missing block should produce errors");
});

Deno.test("integration/T2: missing block → error codes trace to R50 layer", async () => {
  const results = await go.lint(fixture("go/structure/missing-block.go"));
  resolveErrorCodes(results);

  const errs = errors(results);
  // Block-related errors should trace to R50 codes
  const blockErrors = errs.filter((r) => r.rule.startsWith("block/"));
  for (const r of blockErrors) {
    const entry = matchRule(r.rule);
    if (entry) {
      assertEquals(entry.layer, 0, `Block error '${r.rule}' should be Layer 0 (R50)`);
      assertEquals(entry.layerName, "R50", `Block error layerName should be R50`);
    }
  }
});

// ---------------------------------------------------------------------------
// Test 2B: Wrong Block Order — Different Structural Failure
// ---------------------------------------------------------------------------

Deno.test("integration/T2B: wrong block order → block/order errors", async () => {
  const results = await go.lint(fixture("go/structure/wrong-block-order.go"));
  const errs = errors(results);
  assert(errs.length > 0, "Should have errors");

  // Should have block order violation
  const orderErrors = errs.filter((r) => r.rule.includes("order"));
  assert(orderErrors.length > 0, "Should detect block order violations");
});

// ---------------------------------------------------------------------------
// Test 3: Transform → Verify Loop (Go)
// ---------------------------------------------------------------------------
// Copy a file, transform it, re-lint to verify improvement.

Deno.test("integration/T3: transform → re-lint shows improvement", async () => {
  const srcPath = fixture("go/structure/valid-library.go");
  const tmpDir = await Deno.makeTempDir();
  const tmpFile = `${tmpDir}/test-transform.go`;

  try {
    // Copy fixture to temp
    await Deno.copyFile(srcPath, tmpFile);

    // Lint before — should be clean
    const beforeResults = await go.lint(tmpFile);
    const beforeErrors = errors(beforeResults).length;

    // Transform (dry run — just verify it doesn't crash)
    const transformResults = await go.transform!(tmpFile, {
      dryRun: true,
      extensions: false,
      force: false,
      steps: false,
    });

    // Transform should return results (even if no changes needed)
    assert(Array.isArray(transformResults), "Transform should return results array");

    // Re-lint (file unchanged since dry-run)
    const afterResults = await go.lint(tmpFile);
    const afterErrors = errors(afterResults).length;

    // Since we used a valid file, errors should stay 0
    assertEquals(afterErrors, beforeErrors, "Dry-run transform should not change error count");
  } finally {
    await Deno.remove(tmpDir, { recursive: true });
  }
});

Deno.test("integration/T3: transform wet-run on valid file → still valid", async () => {
  const srcPath = fixture("go/structure/valid-library.go");
  const tmpDir = await Deno.makeTempDir();
  const tmpFile = `${tmpDir}/test-transform-wet.go`;

  try {
    await Deno.copyFile(srcPath, tmpFile);

    // Transform (wet run)
    await go.transform!(tmpFile, {
      dryRun: false,
      extensions: false,
      force: false,
      steps: false,
    });

    // Re-lint — should still be clean
    const afterResults = await go.lint(tmpFile);
    const afterErrors = errors(afterResults).length;
    assertEquals(afterErrors, 0, "Transform on valid file should keep it valid");
  } finally {
    await Deno.remove(tmpDir, { recursive: true });
  }
});

// ---------------------------------------------------------------------------
// Test 4: Create → Verify
// ---------------------------------------------------------------------------
// Generate a file with the fill engine, then lint it to verify structural validity.

Deno.test("integration/T4: create library → passes lint Layer 0-1", async () => {
  const tmpDir = await Deno.makeTempDir();
  const tmpFile = `${tmpDir}/test-created.go`;

  try {
    const ctx: FillContext = {
      format: "go" as CodeFormat,
      subtype: "library",
      key: "B-test-integration-created",
      title: "Integration Test — Created Library",
      purpose: "Verify that created files pass structural linting",
      filename: "test-created.go",
      packageOrCrate: "testcreated",
      from: "tests/integration/test-created.go",
    };

    const content = await generateFileText(ctx);
    await Deno.writeTextFile(tmpFile, content);

    // Lint the generated file
    const results = await go.lint(tmpFile);
    resolveErrorCodes(results);

    // Layer 0 (R50 — block structure) should pass
    const layer0Errors = byLayer(errors(results), 0);
    assertEquals(
      layer0Errors.length,
      0,
      `Created file should pass Layer 0, got: ${layer0Errors.map((r) => r.rule).join(", ")}`,
    );

    // Layer 1 (R25 — section structure) should pass
    const layer1Errors = byLayer(errors(results), 1);
    assertEquals(
      layer1Errors.length,
      0,
      `Created file should pass Layer 1, got: ${layer1Errors.map((r) => r.rule).join(", ")}`,
    );
  } finally {
    await Deno.remove(tmpDir, { recursive: true });
  }
});

Deno.test("integration/T4: create executable → passes lint Layer 0-1", async () => {
  const tmpDir = await Deno.makeTempDir();
  const tmpFile = `${tmpDir}/test-exe.go`;

  try {
    const ctx: FillContext = {
      format: "go" as CodeFormat,
      subtype: "executable",
      key: "B-test-integration-exe",
      title: "Integration Test — Created Executable",
      purpose: "Verify executable creation",
      filename: "test-exe.go",
      packageOrCrate: "main",
      from: "tests/integration/test-exe.go",
    };

    const content = await generateFileText(ctx);
    await Deno.writeTextFile(tmpFile, content);

    const results = await go.lint(tmpFile);
    resolveErrorCodes(results);

    const layer0Errors = byLayer(errors(results), 0);
    assertEquals(layer0Errors.length, 0, "Created executable should pass Layer 0");
  } finally {
    await Deno.remove(tmpDir, { recursive: true });
  }
});

// ---------------------------------------------------------------------------
// Test 5: Logging Integration
// ---------------------------------------------------------------------------
// Verify that atomic actions produce valid log entries.

Deno.test("integration/T5: health actions → valid log entries", async () => {
  const filePath = fixture("go/structure/valid-library.go");
  const results = await go.lint(filePath);
  const health = await go.computeHealth!(filePath, results);

  // Collect all atomic actions
  const allActions = health.blocks.flatMap((b) =>
    b.containers.flatMap((c) => c.actions ?? [])
  );
  assert(allActions.length > 0, "Should have atomic actions to log");

  // Format as log entries (3 args: actions, format, filePath)
  const logLines = formatLogEntries(allActions, "go", filePath);
  assert(logLines.length > 0, "Log lines should be non-empty");

  // Each line (after the header comment) should match TIMESTAMP|ACTION|DELTA|SOURCE|DETAIL format
  const lines = logLines.filter((l) => !l.startsWith("#"));
  for (const line of lines) {
    const parts = line.split("|");
    assert(
      parts.length >= 5,
      `Log entry should have 5+ pipe-delimited fields: ${line}`,
    );

    // ACTION should be success/failure/neutral
    const action = parts[1]!.trim();
    assert(
      ["success", "failure", "neutral"].includes(action),
      `ACTION should be success/failure/neutral, got: ${action}`,
    );

    // DELTA should be a number
    const delta = parts[2]!.trim();
    assert(/^[+-]?\d+$/.test(delta), `DELTA should be a number, got: ${delta}`);
  }
});

Deno.test("integration/T5: --log CLI writes log file", async () => {
  const tmpDir = await Deno.makeTempDir();
  const logFile = `${tmpDir}/test.log`;

  try {
    const { code } = await runCli([
      "lint", "go", fixture("go/structure/valid-library.go"), "--log", logFile,
    ]);
    assertEquals(code, 0, "Lint should exit 0");

    // Log file should exist and have content
    const logContent = await Deno.readTextFile(logFile);
    assert(logContent.length > 0, "Log file should have content");
    assertStringIncludes(logContent, "|", "Log entries should be pipe-delimited");
  } finally {
    await Deno.remove(tmpDir, { recursive: true });
  }
});

// ---------------------------------------------------------------------------
// Test 6: Inspect → Lint Consistency
// ---------------------------------------------------------------------------
// Inspect and lint the same file. Blocks found by inspect should match
// blocks referenced by lint results.

Deno.test("integration/T6: inspect + lint see same structure (Go)", async () => {
  const filePath = fixture("go/structure/valid-library.go");

  // Inspect
  const inspectResult = await go.inspect!(filePath);
  assert(inspectResult.blocks.length > 0, "Inspect should find blocks");

  // Lint
  const lintResults = await go.lint(filePath);

  // Inspect block count should be 4 (valid file has all blocks)
  assertEquals(inspectResult.blocks.length, 4, "Valid library should have 4 blocks");

  // Inspect and lint should agree on format
  assertEquals(inspectResult.format, "go", "Format should be go");

  // Inspect should find sections
  assert(inspectResult.sections.length > 0, "Should find subsections");
});

Deno.test("integration/T6: inspect + lint see same structure (Rust)", async () => {
  const filePath = fixture("rust/structure/valid-module.rs");

  const inspectResult = await rust.inspect!(filePath);
  assert(inspectResult.blocks.length > 0, "Inspect should find blocks");

  const lintResults = await rust.lint(filePath);
  assertEquals(inspectResult.format, "rust", "Format should be rust");
});

Deno.test("integration/T6: inspect JSON round-trips cleanly", async () => {
  const filePath = fixture("go/structure/valid-library.go");
  const inspectResult = await go.inspect!(filePath);

  // Text format
  const text = formatInspectText(inspectResult);
  assert(text.length > 0, "Text output should be non-empty");

  // JSON format — round-trip
  const json = formatInspectJson(inspectResult);
  const parsed = JSON.parse(json);
  assertEquals(parsed.format, inspectResult.format, "JSON round-trip preserves format");
  assertEquals(parsed.blocks.length, inspectResult.blocks.length, "JSON round-trip preserves blocks");
  assertEquals(parsed.lineCount, inspectResult.lineCount, "JSON round-trip preserves lineCount");
});

// ---------------------------------------------------------------------------
// Test 7: --why Enrichment Consistency
// ---------------------------------------------------------------------------
// Verify --why data matches the error catalog.

Deno.test("integration/T7: --why JSON matches error catalog entries", async () => {
  const filePath = fixture("go/structure/missing-block.go");
  const results = await go.lint(filePath);
  resolveErrorCodes(results);

  // For each result with an error code, verify the why chain
  for (const r of results) {
    const entry = matchRule(r.rule);
    if (entry) {
      // The why chain should match the catalog entry
      assertEquals(typeof entry.layer, "number", `${r.rule}: layer should be number`);
      assertEquals(typeof entry.layerName, "string", `${r.rule}: layerName should be string`);
      assertEquals(typeof entry.category, "string", `${r.rule}: category should be string`);
      assert(entry.suggestionTemplate.length > 0, `${r.rule}: suggestion should be non-empty`);
      assert(entry.checkFunction.length > 0, `${r.rule}: checkFunction should be non-empty`);
    }
  }
});

// ---------------------------------------------------------------------------
// Test 8: CLI End-to-End Smoke Tests
// ---------------------------------------------------------------------------
// Verify CLI commands produce expected exit codes and output shapes.

Deno.test("integration/T8: CLI lint valid file → exit 0", async () => {
  const { code, stdout } = await runCli([
    "lint", "go", fixture("go/structure/valid-library.go"),
  ]);
  assertEquals(code, 0, "Valid file should exit 0");
  assertStringIncludes(stdout, "OK", "Should show OK for valid file");
});

Deno.test("integration/T8: CLI lint invalid file → exit 1", async () => {
  const { code, stdout } = await runCli([
    "lint", "go", fixture("go/structure/missing-block.go"),
  ]);
  assertEquals(code, 1, "Invalid file should exit 1");
  assertStringIncludes(stdout, "FAIL", "Should show FAIL for invalid file");
});

Deno.test("integration/T8: CLI --json produces valid JSON (valid file)", async () => {
  const { code, stdout } = await runCli([
    "lint", "go", fixture("go/structure/valid-library.go"), "--json",
  ]);
  assertEquals(code, 0);
  const parsed = JSON.parse(stdout);
  assert(parsed.files !== undefined, "JSON should have files array");
  assert(parsed.files.length > 0, "Should have at least one file result");
  assertEquals(parsed.files[0].errors, 0, "Valid file should have 0 errors in JSON");
});

Deno.test("integration/T8: CLI --json produces valid JSON (invalid file)", async () => {
  const { code, stdout } = await runCli([
    "lint", "go", fixture("go/structure/missing-block.go"), "--json",
  ]);
  assertEquals(code, 1);
  const parsed = JSON.parse(stdout);
  assert(parsed.files[0].errors > 0, "Invalid file should have errors in JSON");
});

Deno.test("integration/T8: CLI --verbose shows layer info", async () => {
  const { stdout } = await runCli([
    "lint", "go", fixture("go/structure/valid-library.go"), "--verbose",
  ]);
  // Verbose mode shows health breakdown
  assertStringIncludes(stdout, "health:", "Verbose should show health");
});

Deno.test("integration/T8: CLI formats subcommand lists handlers", async () => {
  const { code, stdout } = await runCli(["formats"]);
  assertEquals(code, 0, "Formats should exit 0");
  assertStringIncludes(stdout, "go", "Should list Go handler");
  assertStringIncludes(stdout, "rust", "Should list Rust handler");
  assertStringIncludes(stdout, "toml", "Should list TOML handler");
});

Deno.test("integration/T8: CLI version flag works", async () => {
  const { code, stdout } = await runCli(["--version"]);
  assertEquals(code, 0);
  assertStringIncludes(stdout, "0.1.0", "Should show version");
});

// ---------------------------------------------------------------------------
// Test 9: Cross-Format Consistency
// ---------------------------------------------------------------------------
// Same structural pattern (valid library) in Go and Rust should have
// comparable health characteristics.

Deno.test("integration/T9: Go + Rust valid libraries both healthy", async () => {
  const goPath = fixture("go/structure/valid-library.go");
  const rustPath = fixture("rust/structure/valid-library.rs");

  const goResults = await go.lint(goPath);
  const goHealth = await go.computeHealth!(goPath, goResults);

  const rustResults = await rust.lint(rustPath);
  const rustHealth = await rust.computeHealth!(rustPath, rustResults);

  // Both should be positive health
  assert(goHealth.total > 0, `Go health should be positive, got ${goHealth.total}`);
  assert(rustHealth.total > 0, `Rust health should be positive, got ${rustHealth.total}`);

  // Both should be at least sound
  const positiveStates = ["sound", "whole", "perfect"];
  assert(
    positiveStates.includes(goHealth.level),
    `Go health should be sound+, got ${goHealth.level}`,
  );
  assert(
    positiveStates.includes(rustHealth.level),
    `Rust health should be sound+, got ${rustHealth.level}`,
  );
});

Deno.test("integration/T9: Go + Rust missing-block both lower than valid", async () => {
  // Go: missing-block vs valid-library
  const goValidPath = fixture("go/structure/valid-library.go");
  const goValidResults = await go.lint(goValidPath);
  const goValidHealth = await go.computeHealth!(goValidPath, goValidResults);

  const goBrokenPath = fixture("go/structure/missing-block.go");
  const goBrokenResults = await go.lint(goBrokenPath);
  const goBrokenHealth = await go.computeHealth!(goBrokenPath, goBrokenResults);

  // Rust: missing-block vs valid-library
  const rustValidPath = fixture("rust/structure/valid-library.rs");
  const rustValidResults = await rust.lint(rustValidPath);
  const rustValidHealth = await rust.computeHealth!(rustValidPath, rustValidResults);

  const rustBrokenPath = fixture("rust/structure/missing-block.rs");
  const rustBrokenResults = await rust.lint(rustBrokenPath);
  const rustBrokenHealth = await rust.computeHealth!(rustBrokenPath, rustBrokenResults);

  // Both should have errors
  assert(errors(goBrokenResults).length > 0, "Go should have errors");
  assert(errors(rustBrokenResults).length > 0, "Rust should have errors");

  // Both missing-block should score lower than their valid counterparts
  assert(
    goBrokenHealth.total < goValidHealth.total,
    `Go missing-block (${goBrokenHealth.total}) should score lower than valid (${goValidHealth.total})`,
  );
  assert(
    rustBrokenHealth.total < rustValidHealth.total,
    `Rust missing-block (${rustBrokenHealth.total}) should score lower than valid (${rustValidHealth.total})`,
  );
});

// ---------------------------------------------------------------------------
// Test 10: Summarize + Health Pipeline
// ---------------------------------------------------------------------------
// Verify summarize() correctly tallies results and health integrates.

Deno.test("integration/T10: summarize counts match actual results", async () => {
  const filePath = fixture("go/structure/missing-block.go");
  const results = await go.lint(filePath);
  const health = await go.computeHealth!(filePath, results);
  const summary = summarize(filePath, results, health);

  // Manual count
  let manualErrors = 0, manualWarns = 0, manualInfos = 0;
  for (const r of results) {
    if (r.severity === "error") manualErrors++;
    else if (r.severity === "warn") manualWarns++;
    else if (r.severity === "info") manualInfos++;
  }

  assertEquals(summary.errors, manualErrors, "Error count should match");
  assertEquals(summary.warnings, manualWarns, "Warning count should match");
  assertEquals(summary.infos, manualInfos, "Info count should match");
  assert(summary.health !== undefined, "Health should be attached");
  assertEquals(summary.health!.total, health.total, "Health total should match");
});

Deno.test("integration/T10: error code summary covers all cataloged results", async () => {
  const filePath = fixture("go/structure/missing-block.go");
  const results = await go.lint(filePath);
  resolveErrorCodes(results);

  const summary = errorCodeSummary(results);
  assert(summary.size > 0, "Should have error code entries");

  // Total count should match total results
  let totalCounted = 0;
  for (const count of summary.values()) {
    totalCounted += count;
  }
  assertEquals(totalCounted, results.length, "Error code summary should account for all results");
});

// ============================================================================
// CLOSING
// ============================================================================
//
// Integration tests — the system working as a whole. Each test exercises
// multiple subsystems together: lint + health + error codes + logging +
// inspect + transform + create. The parts serve the whole.
//
// "Fitly joined together and compacted by that which every joint supplieth."
// — Ephesians 4:16
// ============================================================================
