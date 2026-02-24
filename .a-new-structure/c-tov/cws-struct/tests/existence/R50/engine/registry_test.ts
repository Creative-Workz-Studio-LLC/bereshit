// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/T00/R50/engine/registry_test.ts
// key:     B-tov-cws-struct-tests-engine-registry
// title:   CWS Struct — Registry Tests
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-17
// authors: Nathan Emet (CPI-SI)
// purpose: Tests for registerFormat, getFormat, listFormats, listFormatDetails,
//          detectFormat. Verifies the format handler registry works correctly.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert, assertGreater } from "jsr:@std/assert";

// Side-effect imports to register all handlers (via helpers)
import "@test/helpers.ts";

import {
  getFormat, listFormats, listFormatDetails, detectFormat,
} from "@lib/engine/mod.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// getFormat
// ---------------------------------------------------------------------------

Deno.test("[CWS-T00-080] registry/getFormat: returns handler for registered 'toml'", () => {
  const handler = getFormat("toml");
  assert(handler !== undefined, "TOML handler should be registered");
  assertEquals(handler!.name, "toml");
});

Deno.test("registry/getFormat: returns handler for registered 'rust'", () => {
  const handler = getFormat("rust");
  assert(handler !== undefined, "Rust handler should be registered");
  assertEquals(handler!.name, "rust");
});

Deno.test("registry/getFormat: returns handler for registered 'go'", () => {
  const handler = getFormat("go");
  assert(handler !== undefined, "Go handler should be registered");
  assertEquals(handler!.name, "go");
});

Deno.test("registry/getFormat: returns undefined for unknown format", () => {
  const handler = getFormat("nonexistent");
  assertEquals(handler, undefined);
});

// ---------------------------------------------------------------------------
// listFormats
// ---------------------------------------------------------------------------

Deno.test("registry/listFormats: returns sorted array of format names", () => {
  const formats = listFormats();
  assertGreater(formats.length, 0, "Should have at least one registered format");
  assert(formats.includes("toml"), "Should include 'toml'");
  assert(formats.includes("rust"), "Should include 'rust'");
  assert(formats.includes("go"), "Should include 'go'");

  // Verify sorted
  const sorted = [...formats].sort();
  assertEquals(formats, sorted, "Should be alphabetically sorted");
});

// ---------------------------------------------------------------------------
// listFormatDetails
// ---------------------------------------------------------------------------

Deno.test("registry/listFormatDetails: returns array of {name, description, extensions}", () => {
  const details = listFormatDetails();
  assertGreater(details.length, 0, "Should have format details");

  const toml = details.find((d) => d.name === "toml");
  assert(toml !== undefined, "Should include TOML details");
  assert(toml!.description.length > 0, "TOML should have a description");
  assert(toml!.extensions.includes(".toml"), "TOML should have .toml extension");
});

Deno.test("registry/listFormatDetails: sorted by name", () => {
  const details = listFormatDetails();
  const names = details.map((d) => d.name);
  const sorted = [...names].sort();
  assertEquals(names, sorted, "Should be sorted by name");
});

// ---------------------------------------------------------------------------
// detectFormat
// ---------------------------------------------------------------------------

Deno.test("registry/detectFormat: detects TOML from extension", () => {
  const result = detectFormat("/path/to/file.toml");
  assertEquals(result, "toml");
});

Deno.test("registry/detectFormat: detects Rust from extension", () => {
  const result = detectFormat("/path/to/file.rs");
  assertEquals(result, "rust");
});

Deno.test("registry/detectFormat: detects Go from extension", () => {
  const result = detectFormat("/path/to/file.go");
  assertEquals(result, "go");
});

Deno.test("registry/detectFormat: returns undefined for unknown extension", () => {
  const result = detectFormat("/path/to/file.xyz");
  assertEquals(result, undefined);
});

Deno.test("registry/detectFormat: handles directory dots correctly", () => {
  // Directory has a dot, but file is a Makefile
  const result = detectFormat("/path/my.project/Makefile");
  // Should detect "makefile" if registered, or undefined. Either way, not "project"
  assert(result !== "project", "Should not confuse directory dot with file extension");
});

Deno.test("registry/detectFormat: detects basename-matched files", () => {
  // Makefile handler registers basename matching
  const makefile = getFormat("makefile");
  if (makefile?.basenames?.includes("Makefile")) {
    const result = detectFormat("/path/to/Makefile");
    assertEquals(result, "makefile");
  }
});

// ============================================================================
// CLOSING
// ============================================================================
//
// Registry tests — verifying the format handler lookup works correctly.
// "For as the body is one, and hath many members." — 1 Corinthians 12:12
// ============================================================================
