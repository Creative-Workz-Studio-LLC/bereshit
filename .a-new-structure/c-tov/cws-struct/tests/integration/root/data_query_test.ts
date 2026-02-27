// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/integration/root/data_query_test.ts
// key:     B-tov-cws-struct-tests-integration-root-data-query
// title:   Data Layer Query Tests — Dimensional Navigation
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-24
// authors: Nova Dawn (CPI-SI)
// purpose: Integration tests for the 3D error code query infrastructure.
//          Exercises dimensional navigation: layer (X), domain (Y),
//          severity (Z). Verifies catalog consistency, lookup functions,
//          QoL functions, and structural invariants.
//
//          "By wisdom a house is built, and by understanding it is
//           established; by knowledge the rooms are filled."
//           — Proverbs 24:3-4
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import {
  assertEquals,
  assert,
  assertExists,
  assertGreater,
} from "jsr:@std/assert";

import {
  ERROR_CODES,
  LINT_RULES,
  CATALOG_SIZE,
  LAYER_COUNTS,
  LAYER_DESCRIPTIONS,
  DOMAIN_CODES,
  CATEGORY_DOMAIN,
  getByRule,
  getByCode,
  getByLayer,
  getByCategory,
  getByDomain,
  query,
  matchRule,
  dimensionalCode,
  categoryToDomain,
  catalogStructure,
  codeSummary,
  explain,
  search,
  related,
} from "@lib/data/errors.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// 1. Catalog integrity
// ---------------------------------------------------------------------------

Deno.test("data/catalog: CATALOG_SIZE matches ERROR_CODES map", () => {
  assertEquals(CATALOG_SIZE, ERROR_CODES.size, "CATALOG_SIZE should match map size");
});

Deno.test("data/catalog: CATALOG_SIZE matches LINT_RULES map", () => {
  assertEquals(CATALOG_SIZE, LINT_RULES.size, "Every code has a rule");
});

Deno.test("data/catalog: layer counts sum to total", () => {
  const sum = Object.values(LAYER_COUNTS).reduce((a, b) => a + b, 0);
  assertEquals(sum, CATALOG_SIZE, "Layer counts should sum to CATALOG_SIZE");
});

Deno.test("data/catalog: all 5 layers have descriptions", () => {
  const expected = ["R50", "R25", "R10", "R05", "T00"];
  for (const layer of expected) {
    assertExists(LAYER_DESCRIPTIONS[layer], `${layer} should have description`);
  }
});

Deno.test("data/catalog: all entries have required fields", () => {
  for (const [_code, entry] of ERROR_CODES) {
    assert(entry.code.startsWith("CWS-"), `Code ${entry.code} should start with CWS-`);
    assert(entry.layer >= 0 && entry.layer <= 4, `Layer ${entry.layer} out of range`);
    assert(entry.category.length > 0, `${entry.code}: category should be non-empty`);
    assert(entry.severity === "error" || entry.severity === "warn" || entry.severity === "info",
      `${entry.code}: invalid severity ${entry.severity}`);
    assert(entry.messageTemplate.length > 0, `${entry.code}: message should be non-empty`);
    assert(entry.checkFunction.length > 0, `${entry.code}: checkFunction should be non-empty`);
  }
});

Deno.test("data/catalog: code format is CWS-{layer}-{seq}", () => {
  const pattern = /^CWS-(R50|R25|R10|R05|T00)-\d{3}$/;
  for (const [code] of ERROR_CODES) {
    assert(pattern.test(code), `Code ${code} doesn't match CWS-{layer}-{seq} pattern`);
  }
});

Deno.test("data/catalog: no duplicate codes", () => {
  const codes = new Set<string>();
  for (const [code] of ERROR_CODES) {
    assert(!codes.has(code), `Duplicate code: ${code}`);
    codes.add(code);
  }
});

// ---------------------------------------------------------------------------
// 2. Layer queries
// ---------------------------------------------------------------------------

Deno.test("data/layer: getByLayer(0) returns R50 entries", () => {
  const results = getByLayer(0);
  assertGreater(results.length, 0, "Layer 0 should have entries");
  for (const e of results) {
    assertEquals(e.layer, 0, "All entries should be Layer 0");
    assertEquals(e.layerName, "R50");
  }
  assertEquals(results.length, LAYER_COUNTS.R50);
});

Deno.test("data/layer: getByLayer(1) returns R25 entries", () => {
  const results = getByLayer(1);
  assertGreater(results.length, 0);
  for (const e of results) assertEquals(e.layer, 1);
  assertEquals(results.length, LAYER_COUNTS.R25);
});

Deno.test("data/layer: getByLayer(2) returns R10 entries", () => {
  const results = getByLayer(2);
  assertGreater(results.length, 0);
  for (const e of results) assertEquals(e.layer, 2);
  assertEquals(results.length, LAYER_COUNTS.R10);
});

Deno.test("data/layer: getByLayer(3) returns R05 entries", () => {
  const results = getByLayer(3);
  assertGreater(results.length, 0);
  for (const e of results) assertEquals(e.layer, 3);
  assertEquals(results.length, LAYER_COUNTS.R05);
});

Deno.test("data/layer: getByLayer(4) returns T00 entries", () => {
  const results = getByLayer(4);
  assertGreater(results.length, 0);
  for (const e of results) assertEquals(e.layer, 4);
  assertEquals(results.length, LAYER_COUNTS.T00);
});

// ---------------------------------------------------------------------------
// 3. Domain queries
// ---------------------------------------------------------------------------

Deno.test("data/domain: DOMAIN_CODES is non-empty", () => {
  assertGreater(DOMAIN_CODES.length, 0, "Should have domain codes");
});

Deno.test("data/domain: all domains are 3-letter uppercase", () => {
  for (const code of DOMAIN_CODES) {
    assertEquals(code.length, 3, `Domain ${code} should be 3 chars`);
    assertEquals(code, code.toUpperCase(), `Domain ${code} should be uppercase`);
  }
});

Deno.test("data/domain: getByDomain returns entries for each known domain", () => {
  for (const domain of DOMAIN_CODES) {
    const results = getByDomain(domain);
    assertGreater(results.length, 0, `Domain ${domain} should have entries`);
  }
});

Deno.test("data/domain: CATEGORY_DOMAIN maps all used categories", () => {
  const unmapped: string[] = [];
  for (const [_, entry] of ERROR_CODES) {
    if (!CATEGORY_DOMAIN[entry.category]) {
      unmapped.push(`${entry.code}: ${entry.category}`);
    }
  }
  assertEquals(unmapped.length, 0, `Unmapped categories: ${unmapped.join(", ")}`);
});

Deno.test("data/domain: categoryToDomain round-trips through getByDomain", () => {
  // For each category that has a domain mapping, verify getByDomain finds entries
  for (const [category, domain] of Object.entries(CATEGORY_DOMAIN)) {
    const byDomain = getByDomain(domain);
    const byCategory = getByCategory(category);
    // byDomain should include all byCategory entries (domain may map to multiple categories)
    for (const entry of byCategory) {
      assert(
        byDomain.some(d => d.code === entry.code),
        `${entry.code} (${category}) missing from domain ${domain}`,
      );
    }
  }
});

// ---------------------------------------------------------------------------
// 4. 3D query
// ---------------------------------------------------------------------------

Deno.test("data/query: empty query returns all entries", () => {
  const results = query({});
  assertEquals(results.length, CATALOG_SIZE);
});

Deno.test("data/query: layer filter works", () => {
  const r50 = query({ layer: 0 });
  assertEquals(r50.length, LAYER_COUNTS.R50);
  for (const e of r50) assertEquals(e.layer, 0);
});

Deno.test("data/query: severity filter works", () => {
  const errs = query({ severity: "error" });
  assertGreater(errs.length, 0);
  for (const e of errs) assertEquals(e.severity, "error");

  const warns = query({ severity: "warn" });
  assertGreater(warns.length, 0);
  for (const e of warns) assertEquals(e.severity, "warn");
});

Deno.test("data/query: domain filter works", () => {
  const blk = query({ domain: "BLK" });
  assertGreater(blk.length, 0);
  for (const e of blk) assertEquals(e.category, "block");
});

Deno.test("data/query: combined layer+domain filter", () => {
  const r50Blk = query({ layer: 0, domain: "BLK" });
  assertGreater(r50Blk.length, 0);
  for (const e of r50Blk) {
    assertEquals(e.layer, 0);
    assertEquals(e.category, "block");
  }
});

Deno.test("data/query: combined layer+severity filter", () => {
  const r50Errors = query({ layer: 0, severity: "error" });
  for (const e of r50Errors) {
    assertEquals(e.layer, 0);
    assertEquals(e.severity, "error");
  }
});

Deno.test("data/query: combined domain+severity filter", () => {
  const idErrors = query({ domain: "IDE", severity: "error" });
  for (const e of idErrors) {
    assertEquals(e.category, "identity");
    assertEquals(e.severity, "error");
  }
});

Deno.test("data/query: all three dimensions", () => {
  const r25IdErrors = query({ layer: 1, domain: "IDE", severity: "error" });
  for (const e of r25IdErrors) {
    assertEquals(e.layer, 1);
    assertEquals(e.category, "identity");
    assertEquals(e.severity, "error");
  }
});

// ---------------------------------------------------------------------------
// 5. Lookup functions
// ---------------------------------------------------------------------------

Deno.test("data/lookup: getByCode finds known code", () => {
  const entry = getByCode("CWS-R50-001");
  assertExists(entry, "CWS-R50-001 should exist");
  assertEquals(entry.code, "CWS-R50-001");
  assertEquals(entry.layer, 0);
});

Deno.test("data/lookup: getByCode returns undefined for unknown", () => {
  const entry = getByCode("CWS-R50-999");
  assertEquals(entry, undefined);
});

Deno.test("data/lookup: getByRule finds exact rule", () => {
  const entry = getByRule("block/order");
  assertExists(entry, "block/order should exist");
  assertEquals(entry.code, "CWS-R50-020");
});

Deno.test("data/lookup: matchRule finds parametric rules", () => {
  const entry = matchRule("block/METADATA");
  assertExists(entry, "block/METADATA should match block/{{tag}}");
});

Deno.test("data/lookup: matchRule returns undefined for nonsense", () => {
  const entry = matchRule("completely/nonsensical/rule/that/exists/nowhere");
  assertEquals(entry, undefined);
});

// ---------------------------------------------------------------------------
// 6. Dimensional codes
// ---------------------------------------------------------------------------

Deno.test("data/dimension: dimensionalCode format is {layer}.{domain}.{seq}", () => {
  const pattern = /^(R50|R25|R10|R05|T00)\.[A-Z]{3}\.\d{3}$/;
  for (const [_, entry] of ERROR_CODES) {
    const dim = dimensionalCode(entry);
    assert(pattern.test(dim), `${entry.code} → ${dim} doesn't match pattern`);
  }
});

Deno.test("data/dimension: dimensionalCode preserves layer", () => {
  const entry = getByCode("CWS-R50-020");
  assertExists(entry);
  const dim = dimensionalCode(entry);
  assert(dim.startsWith("R50."), `Should start with R50., got ${dim}`);
});

// ---------------------------------------------------------------------------
// 7. Catalog structure
// ---------------------------------------------------------------------------

Deno.test("data/structure: catalogStructure has all layers", () => {
  const structure = catalogStructure();
  const layers = Object.keys(structure);
  assert(layers.includes("R50"));
  assert(layers.includes("R25"));
  assert(layers.includes("R10"));
  assert(layers.includes("R05"));
  assert(layers.includes("T00"));
});

Deno.test("data/structure: catalogStructure counts sum per layer", () => {
  const structure = catalogStructure();
  for (const [layer, domains] of Object.entries(structure)) {
    const sum = Object.values(domains).reduce((a, b) => a + b, 0);
    const expected = LAYER_COUNTS[layer as keyof typeof LAYER_COUNTS];
    assertEquals(sum, expected, `${layer} domain counts should sum to layer count`);
  }
});

// ---------------------------------------------------------------------------
// 8. QoL functions
// ---------------------------------------------------------------------------

Deno.test("data/qol: codeSummary for known code", () => {
  const summary = codeSummary("CWS-R50-020");
  assert(summary.includes("CWS-R50-020"), "Should contain code");
  assert(summary.includes("BLK"), "Should contain domain");
  assert(summary.includes("block"), "Should contain category");
});

Deno.test("data/qol: codeSummary for unknown code", () => {
  const summary = codeSummary("CWS-R50-999");
  assert(summary.includes("unknown"), "Should indicate unknown");
});

Deno.test("data/qol: explain for known code", () => {
  const text = explain("CWS-R50-020");
  assert(text.includes("CWS-R50-020"), "Should contain code");
  assert(text.includes("Layer"), "Should contain layer label");
  assert(text.includes("Domain"), "Should contain domain label");
  assert(text.includes("Severity"), "Should contain severity label");
  assert(text.includes("deno test"), "Should contain test filter");
});

Deno.test("data/qol: explain for unknown code", () => {
  const text = explain("CWS-R50-999");
  assert(text.includes("not found"), "Should indicate not found");
});

Deno.test("data/qol: search finds by keyword", () => {
  const results = search("block");
  assertGreater(results.length, 0, "Should find block-related codes");
  // Top results should be block category
  assert(
    results.some(e => e.category === "block"),
    "Should include block category entries",
  );
});

Deno.test("data/qol: search finds by code prefix", () => {
  const results = search("CWS-R50");
  assertGreater(results.length, 0);
});

Deno.test("data/qol: search returns empty for gibberish", () => {
  const results = search("xyzzy_plugh_frobozz");
  assertEquals(results.length, 0);
});

Deno.test("data/qol: related for known code", () => {
  const rel = related("CWS-R50-020");
  assertGreater(rel.sameCategory.length, 0, "Should find same-category codes");
  assertGreater(rel.sameLayer.length, 0, "Should find same-layer codes");
});

Deno.test("data/qol: related for unknown code returns empty", () => {
  const rel = related("CWS-R50-999");
  assertEquals(rel.sameCategory.length, 0);
  assertEquals(rel.sameLayer.length, 0);
  assertEquals(rel.sameCheck.length, 0);
  assertEquals(rel.nearby.length, 0);
});

// ============================================================================
// CLOSING
// ============================================================================
//
// Dimensional query tests — 38 tests covering catalog integrity, layer queries,
// domain queries, 3D queries, lookups, dimensional codes, catalog structure,
// and QoL functions. The error code catalog exercises its full query surface.
//
// "By wisdom a house is built, by understanding it is established,
//  by knowledge the rooms are filled." — Proverbs 24:3-4
// ============================================================================
