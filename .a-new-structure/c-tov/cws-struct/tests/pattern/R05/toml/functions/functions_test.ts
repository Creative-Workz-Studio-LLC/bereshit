// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/unit/toml/functions_test.ts
// key:     B-tov-cws-struct-tests-unit-toml-functions
// title:   CWS Struct — TOML Unit Tests (Pure Functions)
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-23
// authors: Nova Dawn (CPI-SI)
// source:  Extracted from tests/handlers/toml_test.ts
// purpose: Unit tests for TOML handler exported functions.
//          Pure function tests — no fixtures, no lint pipeline.
//          parsePragmaLine, parseFormatField, normalizeCargoData,
//          has, getTable, resolveTable, buildLineMap, classifyZone,
//          findSectionPositions, schema-cache behavior.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert, assertGreater } from "jsr:@std/assert";
import { fixture, lazyFormat, errors } from "@test/helpers.ts";
import {
  parsePragmaLine, parseFormatField, normalizeCargoData,
  has, getTable, resolveTable, buildLineMap, classifyZone, findSectionPositions,
} from "@lib/handlers/toml.ts";
import type { PragmaInfo, TomlData } from "@lib/handlers/toml.ts";
import { loadRules } from "@lib/foundation/mod.ts";

// ============================================================================
// BODY
// ============================================================================

const toml = lazyFormat("toml");

// ---------------------------------------------------------------------------
// unit/schema-cache — Schema cache behavior
// ---------------------------------------------------------------------------

Deno.test("[CWS-T00-109] unit/schema-cache: clearCache allows re-loading", async () => {
  const results1 = await toml.lint(fixture("toml/structure/valid-complete.toml"));
  const { clearCache } = await import("@lib/foundation/mod.ts");
  clearCache();
  const results2 = await toml.lint(fixture("toml/structure/valid-complete.toml"));
  assertEquals(errors(results1).length, errors(results2).length, "Results should be consistent after cache clear");
});

// ---------------------------------------------------------------------------
// unit/parsePragmaLine — Pragma line parsing
// ---------------------------------------------------------------------------

Deno.test("unit/parsePragmaLine: valid data --toml", () => {
  const result = parsePragmaLine("# #!omni data --toml");
  assert(result !== null, "Should parse valid pragma");
  assertEquals(result!.type, "data");
  assertEquals(result!.format, "toml");
  assertEquals(result!.args.length, 0);
});

Deno.test("unit/parsePragmaLine: template --toml --cargo", () => {
  const result = parsePragmaLine("# #!omni template --toml --cargo");
  assert(result !== null, "Should parse with derivation args");
  assertEquals(result!.type, "template");
  assertEquals(result!.format, "toml");
  assertEquals(result!.args, ["cargo"]);
});

Deno.test("unit/parsePragmaLine: code --rs --library", () => {
  const result = parsePragmaLine("# #!omni code --rs --library");
  assert(result !== null);
  assertEquals(result!.type, "code");
  assertEquals(result!.format, "rs");
  assertEquals(result!.args, ["library"]);
});

Deno.test("unit/parsePragmaLine: returns null for non-pragma", () => {
  assertEquals(parsePragmaLine("# Just a comment"), null);
  assertEquals(parsePragmaLine(""), null);
  assertEquals(parsePragmaLine("[_metadata]"), null);
});

Deno.test("unit/parsePragmaLine: returns null for malformed pragma", () => {
  assertEquals(parsePragmaLine("# #!omni --toml"), null);
  assertEquals(parsePragmaLine("# #!omni data"), null);
});

// ---------------------------------------------------------------------------
// unit/parseFormatField — Format field parsing
// ---------------------------------------------------------------------------

Deno.test("unit/parseFormatField: valid @omni data --toml", () => {
  const result = parseFormatField("@omni data --toml");
  assert(result !== null, "Should parse valid format field");
  assertEquals(result!.type, "data");
  assertEquals(result!.format, "toml");
  assertEquals(result!.args.length, 0);
});

Deno.test("unit/parseFormatField: @omni template --toml --compiler", () => {
  const result = parseFormatField("@omni template --toml --compiler");
  assert(result !== null);
  assertEquals(result!.args, ["compiler"]);
});

Deno.test("unit/parseFormatField: returns null for plain string", () => {
  assertEquals(parseFormatField("just a string"), null);
  assertEquals(parseFormatField(""), null);
});

// ---------------------------------------------------------------------------
// unit/normalizeCargoData — Cargo.toml normalization
// ---------------------------------------------------------------------------

Deno.test("unit/normalizeCargoData: extracts _metadata from package.metadata.omni", () => {
  const data: TomlData = {
    package: {
      name: "my-crate",
      metadata: {
        omni: {
          I1_core: { key: "B-test" },
          _closing: { X1_policy: { never: ["test"] } },
        },
      },
    },
  };
  const result = normalizeCargoData(data);
  assert(has(result, "_metadata"), "Should extract _metadata");
  assert(has(result, "_closing"), "Should extract _closing");
  assertEquals(getTable(result, "_metadata")!["I1_core"]?.key, "B-test");
});

Deno.test("unit/normalizeCargoData: returns original if no package.metadata.omni", () => {
  const data: TomlData = { some_key: "value" };
  const result = normalizeCargoData(data);
  assertEquals(result, data);
});

Deno.test("unit/normalizeCargoData: returns original if package exists but no metadata", () => {
  const data: TomlData = { package: { name: "test" } };
  const result = normalizeCargoData(data);
  assertEquals(result, data);
});

// ---------------------------------------------------------------------------
// unit/has, getTable, resolveTable — Data access utilities
// ---------------------------------------------------------------------------

Deno.test("unit/has: returns true for existing key", () => {
  assertEquals(has({ name: "test" }, "name"), true);
});

Deno.test("unit/has: returns false for missing key", () => {
  assertEquals(has({ name: "test" }, "other"), false);
});

Deno.test("unit/has: returns false for null/undefined", () => {
  assertEquals(has(null as unknown as TomlData, "key"), false);
  assertEquals(has(undefined as unknown as TomlData, "key"), false);
});

Deno.test("unit/getTable: returns table for object values", () => {
  const data = { section: { key: "value" } };
  const result = getTable(data, "section");
  assert(result !== undefined);
  assertEquals(result!["key"], "value");
});

Deno.test("unit/getTable: returns undefined for non-object values", () => {
  const data = { name: "string-value", arr: [1, 2, 3] };
  assertEquals(getTable(data, "name"), undefined);
  assertEquals(getTable(data, "arr"), undefined);
});

Deno.test("unit/getTable: returns undefined for missing key", () => {
  assertEquals(getTable({ a: 1 }, "b"), undefined);
});

Deno.test("unit/resolveTable: single-segment path", () => {
  const data = { section: { key: "value" } };
  const result = resolveTable(data, "section");
  assert(result !== undefined);
  assertEquals(result!["key"], "value");
});

Deno.test("unit/resolveTable: dotted path walks tree", () => {
  const data = { a: { b: { c: { value: 42 } } } };
  const result = resolveTable(data, "a.b.c");
  assert(result !== undefined);
  assertEquals(result!["value"], 42);
});

Deno.test("unit/resolveTable: returns undefined for broken path", () => {
  const data = { a: { b: "not-a-table" } };
  assertEquals(resolveTable(data, "a.b.c"), undefined);
});

// ---------------------------------------------------------------------------
// unit/buildLineMap — Line number mapping
// ---------------------------------------------------------------------------

Deno.test("unit/buildLineMap: maps table headers and key-value pairs", () => {
  const text = `[_metadata]

[_metadata.I1_core]
key = "B-test"
format = "@omni data --toml"

[_closing]
`;
  const map = buildLineMap(text);
  assertEquals(map.get("_metadata"), 1);
  assertEquals(map.get("_metadata.I1_core"), 3);
  assertEquals(map.get("_metadata.I1_core.key"), 4);
  assertEquals(map.get("_metadata.I1_core.format"), 5);
  assertEquals(map.get("_closing"), 7);
});

Deno.test("unit/buildLineMap: skips comments and blank lines", () => {
  const text = `# comment
[section]
# another comment

key = "val"
`;
  const map = buildLineMap(text);
  assertEquals(map.get("section"), 2);
  assertEquals(map.get("section.key"), 5);
  assertEquals(map.size, 2, "Should only have section and key entries");
});

// ---------------------------------------------------------------------------
// unit/classifyZone — Zone classification
// ---------------------------------------------------------------------------

Deno.test("unit/classifyZone: metadata paths", async () => {
  const r = await loadRules();
  assertEquals(classifyZone("_metadata", r), "meta");
  assertEquals(classifyZone("_metadata.I1_core", r), "meta");
});

Deno.test("unit/classifyZone: closing paths", async () => {
  const r = await loadRules();
  assertEquals(classifyZone("_closing", r), "closing");
  assertEquals(classifyZone("_closing.X1_policy", r), "closing");
});

Deno.test("unit/classifyZone: content-header", async () => {
  const r = await loadRules();
  assertEquals(classifyZone("_content", r), "content-header");
});

Deno.test("unit/classifyZone: Cv zone for _validation", async () => {
  const r = await loadRules();
  assertEquals(classifyZone("_validation", r), "Cv");
  assertEquals(classifyZone("_validation.schema", r), "Cv");
});

Deno.test("unit/classifyZone: extension zone", async () => {
  const r = await loadRules();
  assertEquals(classifyZone("_extensions", r), "extension");
  assertEquals(classifyZone("_extensions.custom", r), "extension");
});

// ---------------------------------------------------------------------------
// unit/findSectionPositions — Section position extraction
// ---------------------------------------------------------------------------

Deno.test("unit/findSectionPositions: extracts standard table headers", () => {
  const text = `[_metadata]
key = "val"

[_metadata.I1_core]
key = "B-test"

[_content]
Cc = "Config"

[_closing]
`;
  const positions = findSectionPositions(text);
  assertEquals(positions.get("_metadata"), 1);
  assertEquals(positions.get("_metadata.I1_core"), 4);
  assertEquals(positions.get("_content"), 7);
  assertEquals(positions.get("_closing"), 10);
});

Deno.test("unit/findSectionPositions: handles array-of-tables", () => {
  const text = `[[items]]
name = "first"

[[items]]
name = "second"
`;
  const positions = findSectionPositions(text);
  assertEquals(positions.get("items"), 1, "Should record first occurrence of array-of-tables");
  assertEquals(positions.size, 1, "Should not duplicate array-of-tables entry");
});

Deno.test("unit/findSectionPositions: returns empty map for no sections", () => {
  const positions = findSectionPositions("key = \"value\"\n# just a comment\n");
  assertEquals(positions.size, 0);
});

// ============================================================================
// CLOSING
// ============================================================================
//
// TOML unit tests — pure function verification, no fixtures needed.
// Tests the building blocks: pragma parsing, cargo normalization,
// data access utilities, line mapping, zone classification, section positions.
//
// 31 tests. Each function tested in isolation.
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
