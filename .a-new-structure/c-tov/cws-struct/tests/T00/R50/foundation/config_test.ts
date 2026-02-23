// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/T00/R50/foundation/config_test.ts
// key:     B-tov-cws-struct-tests-foundation-config
// title:   CWS Struct — Tool Config Tests
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-22
// updated: 2026-02-22
// authors: Nova Dawn (CPI-SI)
// purpose: Tests for config loading from deno.jsonc — tripwire fallback,
//          partial config, and full config.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals } from "jsr:@std/assert";
import { loadConfig } from "@lib/foundation/config.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Tripwire — missing config → graceful fallback
// ---------------------------------------------------------------------------

Deno.test("config/loadConfig: missing file → defaults", async () => {
  const config = await loadConfig("/nonexistent/path/deno.jsonc");
  assertEquals(config.version, "0.1.0");
  assertEquals(config.concurrency, 8);
  assertEquals(config.maxFiles, 10_000);
  assertEquals(config.maxFileSize, 1_048_576);
  assertEquals(config.excludedDirs.length, 6);
  assertEquals(config.impactWeights.error, 2.0);
  assertEquals(config.impactWeights.warn, 1.0);
  assertEquals(config.impactWeights.info, 0.25);
});

// ---------------------------------------------------------------------------
// Real config — reads from actual deno.jsonc
// ---------------------------------------------------------------------------

Deno.test("config/loadConfig: reads version from deno.jsonc", async () => {
  const config = await loadConfig();
  // Should match the actual version in deno.jsonc
  assertEquals(config.version, "0.1.0");
});

Deno.test("config/loadConfig: reads _cwsStruct section", async () => {
  const config = await loadConfig();
  // Should match what's in deno.jsonc _cwsStruct
  assertEquals(config.concurrency, 8);
  assertEquals(config.maxFiles, 10_000);
  assertEquals(config.maxFileSize, 1_048_576);
  assertEquals(config.impactWeights.error, 2.0);
});

// ---------------------------------------------------------------------------
// Config immutability — frozen after load
// ---------------------------------------------------------------------------

Deno.test("config/loadConfig: config is frozen (immutable)", async () => {
  const config = await loadConfig();

  // Object.freeze prevents property assignment (throws in strict mode)
  let threw = false;
  try {
    // @ts-ignore — intentionally testing runtime freeze behavior
    config.concurrency = 999;
  } catch {
    threw = true;
  }

  // In frozen objects, assignment either throws or silently fails
  assertEquals(
    threw || config.concurrency !== 999,
    true,
    "Config should be immutable (frozen)",
  );
});

// ---------------------------------------------------------------------------
// Partial config — missing fields fallback to defaults
// ---------------------------------------------------------------------------

Deno.test("config/loadConfig: partial config → defaults for missing fields", async () => {
  // Write a temporary deno.jsonc with only some _cwsStruct fields
  const tmpDir = await Deno.makeTempDir();
  const tmpPath = `${tmpDir}/deno.jsonc`;
  await Deno.writeTextFile(tmpPath, JSON.stringify({
    version: "9.9.9",
    _cwsStruct: {
      concurrency: 4,
      // maxFiles, maxFileSize, excludedDirs, impactWeights NOT provided
    },
  }));

  try {
    const config = await loadConfig(tmpPath);
    // Provided values used
    assertEquals(config.version, "9.9.9");
    assertEquals(config.concurrency, 4);
    // Missing values fall back to defaults
    assertEquals(config.maxFiles, 10_000);
    assertEquals(config.maxFileSize, 1_048_576);
    assertEquals(config.excludedDirs.length, 6);
    assertEquals(config.impactWeights.error, 2.0);
  } finally {
    await Deno.remove(tmpDir, { recursive: true });
  }
});

Deno.test("config/loadConfig: empty _cwsStruct → all defaults used", async () => {
  const tmpDir = await Deno.makeTempDir();
  const tmpPath = `${tmpDir}/deno.jsonc`;
  await Deno.writeTextFile(tmpPath, JSON.stringify({
    version: "1.2.3",
    _cwsStruct: {},
  }));

  try {
    const config = await loadConfig(tmpPath);
    assertEquals(config.version, "1.2.3");
    assertEquals(config.concurrency, 8);
    assertEquals(config.maxFiles, 10_000);
    assertEquals(config.impactWeights.error, 2.0);
  } finally {
    await Deno.remove(tmpDir, { recursive: true });
  }
});

Deno.test("config/loadConfig: no _cwsStruct section → all defaults", async () => {
  const tmpDir = await Deno.makeTempDir();
  const tmpPath = `${tmpDir}/deno.jsonc`;
  await Deno.writeTextFile(tmpPath, JSON.stringify({
    version: "0.0.1",
  }));

  try {
    const config = await loadConfig(tmpPath);
    assertEquals(config.version, "0.0.1");
    assertEquals(config.concurrency, 8);
    assertEquals(config.maxFiles, 10_000);
  } finally {
    await Deno.remove(tmpDir, { recursive: true });
  }
});

// ---------------------------------------------------------------------------
// Invalid values — wrong types fall back to defaults
// ---------------------------------------------------------------------------

Deno.test("config/loadConfig: invalid concurrency type → default", async () => {
  const tmpDir = await Deno.makeTempDir();
  const tmpPath = `${tmpDir}/deno.jsonc`;
  await Deno.writeTextFile(tmpPath, JSON.stringify({
    version: "1.0.0",
    _cwsStruct: {
      concurrency: "not-a-number",
      maxFiles: true,
      maxFileSize: null,
    },
  }));

  try {
    const config = await loadConfig(tmpPath);
    assertEquals(config.concurrency, 8, "String concurrency → default 8");
    assertEquals(config.maxFiles, 10_000, "Boolean maxFiles → default");
    assertEquals(config.maxFileSize, 1_048_576, "null maxFileSize → default");
  } finally {
    await Deno.remove(tmpDir, { recursive: true });
  }
});

Deno.test("config/loadConfig: invalid impactWeights → default", async () => {
  const tmpDir = await Deno.makeTempDir();
  const tmpPath = `${tmpDir}/deno.jsonc`;
  await Deno.writeTextFile(tmpPath, JSON.stringify({
    version: "1.0.0",
    _cwsStruct: {
      impactWeights: "not-an-object",
    },
  }));

  try {
    const config = await loadConfig(tmpPath);
    assertEquals(config.impactWeights.error, 2.0, "String impactWeights → default");
    assertEquals(config.impactWeights.warn, 1.0);
  } finally {
    await Deno.remove(tmpDir, { recursive: true });
  }
});

Deno.test("config/loadConfig: non-numeric impactWeights values → default", async () => {
  const tmpDir = await Deno.makeTempDir();
  const tmpPath = `${tmpDir}/deno.jsonc`;
  await Deno.writeTextFile(tmpPath, JSON.stringify({
    version: "1.0.0",
    _cwsStruct: {
      impactWeights: { error: "high", warn: true, info: null },
    },
  }));

  try {
    const config = await loadConfig(tmpPath);
    // isImpactWeights returns false → defaults used
    assertEquals(config.impactWeights.error, 2.0, "Non-numeric values → default weights");
  } finally {
    await Deno.remove(tmpDir, { recursive: true });
  }
});

Deno.test("config/loadConfig: invalid excludedDirs type → default", async () => {
  const tmpDir = await Deno.makeTempDir();
  const tmpPath = `${tmpDir}/deno.jsonc`;
  await Deno.writeTextFile(tmpPath, JSON.stringify({
    version: "1.0.0",
    _cwsStruct: {
      excludedDirs: "not-an-array",
    },
  }));

  try {
    const config = await loadConfig(tmpPath);
    assertEquals(config.excludedDirs.length, 6, "String excludedDirs → default array");
  } finally {
    await Deno.remove(tmpDir, { recursive: true });
  }
});

// ---------------------------------------------------------------------------
// Config values actually used — behavior changes with config
// ---------------------------------------------------------------------------

Deno.test("config/loadConfig: custom values propagate correctly", async () => {
  const tmpDir = await Deno.makeTempDir();
  const tmpPath = `${tmpDir}/deno.jsonc`;
  await Deno.writeTextFile(tmpPath, JSON.stringify({
    version: "2.0.0",
    _cwsStruct: {
      concurrency: 16,
      maxFiles: 50_000,
      maxFileSize: 2_097_152,
      excludedDirs: [".git", "vendor"],
      impactWeights: { error: 3.0, warn: 1.5, info: 0.5 },
    },
  }));

  try {
    const config = await loadConfig(tmpPath);
    assertEquals(config.version, "2.0.0");
    assertEquals(config.concurrency, 16);
    assertEquals(config.maxFiles, 50_000);
    assertEquals(config.maxFileSize, 2_097_152);
    assertEquals(config.excludedDirs, [".git", "vendor"]);
    assertEquals(config.impactWeights.error, 3.0);
    assertEquals(config.impactWeights.warn, 1.5);
    assertEquals(config.impactWeights.info, 0.5);
  } finally {
    await Deno.remove(tmpDir, { recursive: true });
  }
});

// ============================================================================
// CLOSING
// ============================================================================
//
// Config tests — tripwire fallback, partial config, invalid values,
// real config loading, immutability, and behavior propagation.
// "Where no counsel is, the people fall." — Proverbs 11:14
// ============================================================================
