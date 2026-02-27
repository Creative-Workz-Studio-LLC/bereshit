// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/integration/root/inspect_test.ts
// key:     B-tov-cws-struct-tests-integration-root-inspect
// title:   Inspect System Integration Tests
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-24
// authors: Nova Dawn (CPI-SI)
// purpose: Integration tests for the inspect command across Go and Rust.
//          Verifies: file anatomy extraction, block detection, identity
//          resolution, and null returns for unsupported formats.
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

import { inspectFile, getFormatHandler } from "@lib/api/mod.ts";

// ============================================================================
// BODY
// ============================================================================

function fixture(name: string): string {
  return new URL(`../../fixtures/${name}`, import.meta.url).pathname;
}

// ---------------------------------------------------------------------------
// 1. Go inspect
// ---------------------------------------------------------------------------

Deno.test("inspect/go: valid library returns result", async () => {
  const path = fixture("go/structure/valid-library.go");
  const result = await inspectFile(path, { format: "go" });
  assertExists(result, "Should return inspect result for Go file");
});

Deno.test("inspect/go: result has blocks array", async () => {
  const path = fixture("go/structure/valid-library.go");
  const result = await inspectFile(path, { format: "go" });
  assertExists(result);
  assertExists(result.blocks, "Should have blocks");
  assert(Array.isArray(result.blocks), "blocks should be array");
});

Deno.test("inspect/go: valid library has 4 blocks", async () => {
  const path = fixture("go/structure/valid-library.go");
  const result = await inspectFile(path, { format: "go" });
  assertExists(result);
  assertEquals(result.blocks.length, 4, "Should detect 4 blocks");
});

Deno.test("inspect/go: blocks are in correct order", async () => {
  const path = fixture("go/structure/valid-library.go");
  const result = await inspectFile(path, { format: "go" });
  assertExists(result);
  const names = result.blocks.map((b: { name: string }) => b.name);
  assertEquals(names, ["METADATA", "SETUP", "BODY", "CLOSING"]);
});

// ---------------------------------------------------------------------------
// 2. Rust inspect
// ---------------------------------------------------------------------------

Deno.test("inspect/rust: valid library returns result", async () => {
  const path = fixture("rust/structure/valid-library.rs");
  const result = await inspectFile(path, { format: "rust" });
  assertExists(result, "Should return inspect result for Rust file");
});

Deno.test("inspect/rust: valid library has 4 blocks", async () => {
  const path = fixture("rust/structure/valid-library.rs");
  const result = await inspectFile(path, { format: "rust" });
  assertExists(result);
  assertEquals(result.blocks.length, 4, "Should detect 4 blocks");
});

// ---------------------------------------------------------------------------
// 3. Format capability check
// ---------------------------------------------------------------------------

Deno.test("inspect/capability: Go handler has inspect", () => {
  const handler = getFormatHandler("go");
  assertExists(handler);
  assertExists(handler.inspect, "Go should have inspect capability");
});

Deno.test("inspect/capability: Rust handler has inspect", () => {
  const handler = getFormatHandler("rust");
  assertExists(handler);
  assertExists(handler.inspect, "Rust should have inspect capability");
});

Deno.test("inspect/capability: TOML handler lacks inspect", () => {
  const handler = getFormatHandler("toml");
  assertExists(handler);
  assertEquals(handler.inspect, undefined, "TOML should not have inspect");
});

Deno.test("inspect/capability: JSON handler lacks inspect", () => {
  const handler = getFormatHandler("json");
  assertExists(handler);
  assertEquals(handler.inspect, undefined, "JSON should not have inspect");
});

// ---------------------------------------------------------------------------
// 4. Null/error cases
// ---------------------------------------------------------------------------

Deno.test("inspect/null: format without inspect returns null", async () => {
  const tmp = await Deno.makeTempFile({ suffix: ".json" });
  try {
    await Deno.writeTextFile(tmp, '{"_metadata": {}}');
    const result = await inspectFile(tmp, { format: "json" });
    assertEquals(result, null, "JSON inspect should return null");
  } finally {
    await Deno.remove(tmp);
  }
});

// ============================================================================
// CLOSING
// ============================================================================
//
// Inspect integration tests — 11 tests covering Go/Rust inspect results,
// block detection, format capability checks, and null returns.
//
// "Examine me, O LORD, and prove me; try my reins and my heart."
// — Psalm 26:2
// ============================================================================
