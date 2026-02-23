// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/T00/R25/engine/discovery_test.ts
// key:     B-tov-cws-struct-tests-engine-discovery
// title:   CWS Struct — Discovery Tests
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-17
// authors: Nathan Emet (CPI-SI)
// purpose: Tests for discoverFiles and relativePaths. Verifies file
//          discovery walks directories and matches extensions correctly.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert, assertGreater } from "jsr:@std/assert";

// Side-effect imports to register handlers
import "@test/helpers.ts";

import { discoverFiles, relativePaths } from "@lib/engine/mod.ts";
import { getFormat } from "@lib/engine/mod.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// discoverFiles — file targets
// ---------------------------------------------------------------------------

Deno.test("discovery/discoverFiles: single file target returns that file", async () => {
  const toml = getFormat("toml")!;
  const fixture = new URL("../../../fixtures/toml/structure/valid-complete.toml", import.meta.url).pathname;
  const files = await discoverFiles([fixture], toml);
  assertEquals(files.length, 1, "Should find exactly 1 file");
  assert(files[0]!.endsWith("valid-complete.toml"), "Should be the target file");
});

Deno.test("discovery/discoverFiles: non-matching file returns empty", async () => {
  const toml = getFormat("toml")!;
  // Pass a .ts file — TOML handler won't match it
  const fixture = new URL("../../../helpers.ts", import.meta.url).pathname;
  const files = await discoverFiles([fixture], toml);
  assertEquals(files.length, 0, "Should not match .ts files for TOML handler");
});

// ---------------------------------------------------------------------------
// discoverFiles — directory targets
// ---------------------------------------------------------------------------

Deno.test("discovery/discoverFiles: directory walk finds TOML files", async () => {
  const toml = getFormat("toml")!;
  const fixtureDir = new URL("../../../fixtures/toml/", import.meta.url).pathname;
  const files = await discoverFiles([fixtureDir], toml);
  assertGreater(files.length, 0, "Should find TOML files in fixtures directory");

  // All results should be .toml files
  for (const f of files) {
    assert(f.endsWith(".toml"), `Expected .toml file, got: ${f}`);
  }
});

Deno.test("discovery/discoverFiles: directory walk finds Rust files", async () => {
  const rust = getFormat("rust")!;
  const fixtureDir = new URL("../../../fixtures/rust/", import.meta.url).pathname;
  const files = await discoverFiles([fixtureDir], rust);
  assertGreater(files.length, 0, "Should find Rust files in fixtures directory");

  for (const f of files) {
    assert(f.endsWith(".rs"), `Expected .rs file, got: ${f}`);
  }
});

Deno.test("discovery/discoverFiles: results are sorted", async () => {
  const toml = getFormat("toml")!;
  const fixtureDir = new URL("../../../fixtures/toml/", import.meta.url).pathname;
  const files = await discoverFiles([fixtureDir], toml);
  const sorted = [...files].sort();
  assertEquals(files, sorted, "Files should be sorted alphabetically");
});

Deno.test("discovery/discoverFiles: non-existent path doesn't crash", async () => {
  const toml = getFormat("toml")!;
  const files = await discoverFiles(["/tmp/definitely-does-not-exist-xyz123"], toml);
  assertEquals(files.length, 0, "Should return empty for non-existent path");
});

// ---------------------------------------------------------------------------
// relativePaths
// ---------------------------------------------------------------------------

Deno.test("discovery/relativePaths: converts absolute to relative", () => {
  const abs = ["/home/user/project/file.toml", "/home/user/project/sub/other.toml"];
  const rel = relativePaths(abs, "/home/user/project");
  assertEquals(rel[0], "file.toml");
  assertEquals(rel[1], "sub/other.toml");
});

Deno.test("discovery/relativePaths: empty array returns empty", () => {
  const rel = relativePaths([]);
  assertEquals(rel.length, 0);
});

// ============================================================================
// CLOSING
// ============================================================================
//
// Discovery tests — verifying file finding works correctly.
// "Seek, and ye shall find." — Matthew 7:7
// ============================================================================
