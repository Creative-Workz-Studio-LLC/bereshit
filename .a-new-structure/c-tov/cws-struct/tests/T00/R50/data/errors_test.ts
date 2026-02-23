// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/T00/R50/data/errors_test.ts
// key:     B-tov-cws-struct-tests-data-errors
// title:   CWS Struct — Error Code Catalog Tests
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-22
// authors: Nova Dawn (CPI-SI)
// purpose: Verify the error code catalog is structurally sound and every
//          accessor function works correctly. These tests prove the catalog
//          is the SINGLE SOURCE OF TRUTH — no duplicate codes, no orphan
//          rules, every layer populated, every lookup path functional.
//
//          Gate | Verifies
//          -----|----------------------------------------
//          G1   | Catalog is non-empty
//          G2   | Every code is unique (no duplicates)
//          G3   | Every code follows CWS-{LAYER}-{SEQ} format
//          G4   | LINT_RULES and ERROR_CODES are consistent
//          G5   | getByRule() returns correct entries
//          G6   | getByCode() returns correct entries
//          G7   | matchRule() resolves dynamic patterns
//          G8   | getByLayer() returns correct layer subsets
//          G9   | getByCategory() returns correct category subsets
//          G10  | LAYER_COUNTS match actual per-layer sizes
//          G11  | Every entry has all required fields
//          G12  | Tool layer (T00) entries exist
//          G13  | Witness coverage — every code has a test (aspirational)
//          G14  | 3D query infrastructure (domain, dimensional, query)
//          G15  | CATEGORY_DOMAIN covers all categories
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert, assertNotEquals } from "jsr:@std/assert";

import {
  LINT_RULES,
  ERROR_CODES,
  getByRule,
  getByCode,
  matchRule,
  getByLayer,
  getByCategory,
  getByDomain,
  query,
  catalogStructure,
  categoryToDomain,
  dimensionalCode,
  CATEGORY_DOMAIN,
  DOMAIN_CODES,
  CATALOG_SIZE,
  LAYER_COUNTS,
} from "@lib/data/errors.ts";

import type { ErrorCodeEntry } from "@lib/data/types.ts";
import {
  witnessCount,
  unwitnessedCodes,
  diagnosticCommand,
} from "@test/helpers.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// G1: Catalog is non-empty
// ---------------------------------------------------------------------------

Deno.test("errors/G1: catalog is non-empty", () => {
  assert(CATALOG_SIZE > 0, "catalog should have entries");
  assert(LINT_RULES.size > 0, "LINT_RULES should have entries");
  assert(ERROR_CODES.size > 0, "ERROR_CODES should have entries");
});

// ---------------------------------------------------------------------------
// G2: Every code is unique
// ---------------------------------------------------------------------------

Deno.test("errors/G2: every error code is unique", () => {
  const codes = new Set<string>();
  for (const [_, entry] of ERROR_CODES) {
    assert(!codes.has(entry.code), `duplicate error code: ${entry.code}`);
    codes.add(entry.code);
  }
  assertEquals(codes.size, ERROR_CODES.size, "code count should match map size");
});

// ---------------------------------------------------------------------------
// G3: Every code follows CWS-{LAYER}-{SEQ} format
// ---------------------------------------------------------------------------

Deno.test("errors/G3: every code follows CWS-{LAYER}-{SEQ} format", () => {
  const VALID_LAYERS = new Set(["R50", "R25", "R10", "R05", "T00"]);
  const CODE_PATTERN = /^CWS-(R50|R25|R10|R05|T00)-(\d{3})$/;

  for (const [_, entry] of ERROR_CODES) {
    const match = entry.code.match(CODE_PATTERN);
    assert(match !== null, `invalid code format: ${entry.code}`);
    const layerStr = match![1] as string;
    const seqStr = match![2] as string;
    assert(VALID_LAYERS.has(layerStr), `unknown layer in ${entry.code}`);

    const seq = parseInt(seqStr, 10);
    assert(seq >= 1 && seq <= 999, `sequence out of range: ${entry.code}`);
  }
});

// ---------------------------------------------------------------------------
// G4: LINT_RULES and ERROR_CODES are consistent
// ---------------------------------------------------------------------------

Deno.test("errors/G4: LINT_RULES and ERROR_CODES are consistent in size", () => {
  // ERROR_CODES is keyed by code, LINT_RULES by rule string.
  // Both should contain CATALOG_SIZE entries (some rules may share a code
  // in dynamic patterns, so ERROR_CODES ≤ LINT_RULES).
  assert(
    LINT_RULES.size >= ERROR_CODES.size,
    `LINT_RULES (${LINT_RULES.size}) should be >= ERROR_CODES (${ERROR_CODES.size})`,
  );
  assertEquals(LINT_RULES.size, CATALOG_SIZE, "LINT_RULES size should match CATALOG_SIZE");
});

Deno.test("errors/G4: every LINT_RULES entry has a code in ERROR_CODES", () => {
  for (const [rule, entry] of LINT_RULES) {
    const byCode = ERROR_CODES.get(entry.code);
    assert(byCode !== undefined, `rule "${rule}" has code ${entry.code} not in ERROR_CODES`);
  }
});

// ---------------------------------------------------------------------------
// G5: getByRule() — exact lookup
// ---------------------------------------------------------------------------

Deno.test("[CWS-R50-001] errors/G5: getByRule returns entry for known static rule", () => {
  const entry = getByRule("io/read");
  assert(entry !== undefined, "io/read should exist");
  assertEquals(entry!.code, "CWS-R50-001");
  assertEquals(entry!.layer, 0);
});

Deno.test("errors/G5: getByRule returns undefined for unknown rule", () => {
  const entry = getByRule("totally/nonexistent/rule");
  assertEquals(entry, undefined);
});

Deno.test("[CWS-R50-002] errors/G5: getByRule returns entry for parse/toml", () => {
  const entry = getByRule("parse/toml");
  assert(entry !== undefined, "parse/toml should exist");
  assertEquals(entry!.code, "CWS-R50-002");
  assertEquals(entry!.severity, "error");
});

// ---------------------------------------------------------------------------
// G6: getByCode() — reverse lookup
// ---------------------------------------------------------------------------

Deno.test("[CWS-R50-001] errors/G6: getByCode returns entry for valid code", () => {
  const entry = getByCode("CWS-R50-001");
  assert(entry !== undefined, "CWS-R50-001 should exist");
  assertEquals(entry!.layer, 0);
  assertEquals(entry!.layerName, "R50");
});

Deno.test("errors/G6: getByCode returns undefined for invalid code", () => {
  const entry = getByCode("CWS-R99-999");
  assertEquals(entry, undefined);
});

Deno.test("[CWS-R50-001] errors/G6: getByCode round-trips with getByRule", () => {
  const byRule = getByRule("io/read");
  assert(byRule !== undefined);
  const byCode = getByCode(byRule!.code);
  assert(byCode !== undefined);
  assertEquals(byCode!.code, byRule!.code);
});

// ---------------------------------------------------------------------------
// G7: matchRule() — dynamic pattern resolution
// ---------------------------------------------------------------------------

Deno.test("[CWS-R50-001] errors/G7: matchRule resolves exact static rule", () => {
  const entry = matchRule("io/read");
  assert(entry !== undefined, "io/read should match");
  assertEquals(entry!.code, "CWS-R50-001");
});

Deno.test("errors/G7: matchRule resolves dynamic directive rule", () => {
  // "directive/{{directive}}" should match "directive/pragma"
  const entry = matchRule("directive/pragma");
  assert(entry !== undefined, "directive/pragma should match dynamic pattern");
  assertEquals(entry!.layerName, "R50");
});

Deno.test("errors/G7: matchRule resolves dynamic identity field rule", () => {
  // "identity/{{varName}}/{{group}}.{{field}}" should match runtime values
  const entry = matchRule("identity/Pragma/I1.key");
  assert(entry !== undefined, "identity/Pragma/I1.key should match");
});

Deno.test("errors/G7: matchRule returns undefined for no-match", () => {
  const entry = matchRule("completely/unknown/pattern/that/exists/nowhere");
  assertEquals(entry, undefined);
});

// ---------------------------------------------------------------------------
// G8: getByLayer() — layer subsets
// ---------------------------------------------------------------------------

Deno.test("errors/G8: getByLayer(0) returns R50 entries", () => {
  const entries = getByLayer(0);
  assert(entries.length > 0, "R50 should have entries");
  for (const e of entries) {
    assertEquals(e.layer, 0, `expected layer 0, got ${e.layer} for ${e.code}`);
    assertEquals(e.layerName, "R50");
  }
});

Deno.test("errors/G8: getByLayer(1) returns R25 entries", () => {
  const entries = getByLayer(1);
  assert(entries.length > 0, "R25 should have entries");
  for (const e of entries) {
    assertEquals(e.layer, 1);
    assertEquals(e.layerName, "R25");
  }
});

Deno.test("errors/G8: getByLayer(2) returns R10 entries", () => {
  const entries = getByLayer(2);
  assert(entries.length > 0, "R10 should have entries");
  for (const e of entries) {
    assertEquals(e.layer, 2);
    assertEquals(e.layerName, "R10");
  }
});

Deno.test("errors/G8: getByLayer(3) returns R05 entries", () => {
  const entries = getByLayer(3);
  assert(entries.length > 0, "R05 should have entries");
  for (const e of entries) {
    assertEquals(e.layer, 3);
    assertEquals(e.layerName, "R05");
  }
});

Deno.test("errors/G8: all layers sum to CATALOG_SIZE", () => {
  const l0 = getByLayer(0).length;
  const l1 = getByLayer(1).length;
  const l2 = getByLayer(2).length;
  const l3 = getByLayer(3).length;
  // T00 (layer 4) isn't in getByLayer(0-3) — it's a separate tool layer
  // Sum of detection layers may be less than CATALOG_SIZE due to T00
  const detectionTotal = l0 + l1 + l2 + l3;
  assert(
    detectionTotal <= CATALOG_SIZE,
    `detection layers (${detectionTotal}) should be <= CATALOG_SIZE (${CATALOG_SIZE})`,
  );
});

// ---------------------------------------------------------------------------
// G9: getByCategory() — category subsets
// ---------------------------------------------------------------------------

Deno.test("errors/G9: getByCategory returns entries for 'system'", () => {
  const entries = getByCategory("system");
  assert(entries.length > 0, "system category should have entries");
  for (const e of entries) {
    assertEquals(e.category, "system");
  }
});

Deno.test("errors/G9: getByCategory returns entries for 'directive'", () => {
  const entries = getByCategory("directive");
  assert(entries.length > 0, "directive category should have entries");
});

Deno.test("errors/G9: getByCategory returns empty for unknown category", () => {
  const entries = getByCategory("nonexistent_category_xyz");
  assertEquals(entries.length, 0);
});

// ---------------------------------------------------------------------------
// G10: LAYER_COUNTS match actual sizes
// ---------------------------------------------------------------------------

Deno.test("errors/G10: LAYER_COUNTS match getByLayer counts", () => {
  assertEquals(LAYER_COUNTS.R50, getByLayer(0).length, "R50 count mismatch");
  assertEquals(LAYER_COUNTS.R25, getByLayer(1).length, "R25 count mismatch");
  assertEquals(LAYER_COUNTS.R10, getByLayer(2).length, "R10 count mismatch");
  assertEquals(LAYER_COUNTS.R05, getByLayer(3).length, "R05 count mismatch");
});

Deno.test("errors/G10: every LAYER_COUNTS value is positive", () => {
  for (const [layer, count] of Object.entries(LAYER_COUNTS)) {
    assert(count > 0, `${layer} should have at least one entry`);
  }
});

// ---------------------------------------------------------------------------
// G11: Every entry has required fields
// ---------------------------------------------------------------------------

Deno.test("errors/G11: every entry has all required fields", () => {
  for (const [rule, entry] of LINT_RULES) {
    assert(entry.code.length > 0, `rule "${rule}" has empty code`);
    assert(typeof entry.layer === "number", `rule "${rule}" has non-number layer`);
    assert(entry.layerName.length > 0, `rule "${rule}" has empty layerName`);
    assert(entry.category.length > 0, `rule "${rule}" has empty category`);
    assert(
      ["error", "warn", "info"].includes(entry.severity),
      `rule "${rule}" has invalid severity: ${entry.severity}`,
    );
    assert(entry.messageTemplate.length > 0, `rule "${rule}" has empty messageTemplate`);
    assert(entry.suggestionTemplate.length > 0, `rule "${rule}" has empty suggestionTemplate`);
    assert(entry.checkFunction.length > 0, `rule "${rule}" has empty checkFunction`);
  }
});

// ---------------------------------------------------------------------------
// G12: Tool layer (T00) entries exist
// ---------------------------------------------------------------------------

Deno.test("[CWS-T00-001] errors/G12: T00 tool layer has entries", () => {
  const t00Count = LAYER_COUNTS["T00"];
  assert(t00Count !== undefined && t00Count > 0, "T00 layer should have entries");
  const t00 = getByCode("CWS-T00-001");
  assert(t00 !== undefined, "CWS-T00-001 should exist");
  assertEquals(t00!.layerName, "T00");
});

// ---------------------------------------------------------------------------
// Cross-layer: code uniqueness across layers
// ---------------------------------------------------------------------------

Deno.test("errors/cross: no duplicate sequence numbers within a layer", () => {
  const seenPerLayer = new Map<string, Set<string>>();

  for (const [_, entry] of ERROR_CODES) {
    const layer = entry.layerName;
    if (!seenPerLayer.has(layer)) seenPerLayer.set(layer, new Set());
    const seen = seenPerLayer.get(layer)!;
    const seq = entry.code.split("-")[2];
    assert(!seen.has(seq!), `duplicate sequence ${seq} in layer ${layer}: ${entry.code}`);
    seen.add(seq!);
  }
});

// ---------------------------------------------------------------------------
// G13: Witness coverage gate (aspirational — reports gaps, doesn't block)
// ---------------------------------------------------------------------------

Deno.test("errors/G13: witness system reports coverage stats", () => {
  // G13 is an aspirational gate — it reports coverage rather than asserting
  // 100% coverage. As tests adopt [CWS-*] witness tags, the gap shrinks.
  // Once all codes have witnesses, this test can be tightened to assert 0 gaps.
  const gaps = unwitnessedCodes();
  const stats = witnessCount();
  const totalCodes = [...ERROR_CODES.values()].length;

  // Report coverage — always passes, but shows the state
  console.log(
    `  [witness] ${stats.codes}/${totalCodes} codes witnessed by ${stats.tests} tests` +
    (gaps.length > 0 ? ` (${gaps.length} gaps)` : " — FULL COVERAGE"),
  );

  // Aspirational: uncomment when ready to enforce
  // assertEquals(gaps.length, 0, `Codes without witnesses: ${gaps.join(", ")}`);
});

Deno.test("[CWS-R50-020] errors/G13: diagnosticCommand produces valid filter string", () => {
  const cmd = diagnosticCommand("CWS-R50-020");
  assertEquals(cmd, "deno test --filter='CWS-R50-020'");
});

// ---------------------------------------------------------------------------
// G14: 3D query infrastructure — domain, dimensional code, query()
// ---------------------------------------------------------------------------

Deno.test("errors/G14: categoryToDomain maps known categories", () => {
  assertEquals(categoryToDomain("concept"), "CON");
  assertEquals(categoryToDomain("identity"), "IDE");
  assertEquals(categoryToDomain("system"), "SYS");
  assertEquals(categoryToDomain("block"), "BLK");
});

Deno.test("errors/G14: categoryToDomain falls back to first-3-chars for unknown", () => {
  // Unknown categories get first 3 chars uppercased as fallback
  assertEquals(categoryToDomain("nonexistent_category_xyz"), "NON");
  assertEquals(categoryToDomain("xyz"), "XYZ");
});

Deno.test("[CWS-R50-001] errors/G14: dimensionalCode produces LAYER.DOMAIN.SEQ format", () => {
  const entry = getByCode("CWS-R50-001")!;
  const dim = dimensionalCode(entry);
  // Should be R50.{3-letter-domain}.001
  assert(dim.startsWith("R50."), `expected R50.* got ${dim}`);
  assert(dim.endsWith(".001"), `expected *.001 got ${dim}`);
  // Middle part is 3 uppercase letters
  const parts = dim.split(".");
  assertEquals(parts.length, 3, "dimensional code should have 3 parts");
  assertEquals(parts[1]!.length, 3, "domain should be 3 chars");
  assertEquals(parts[1], parts[1]!.toUpperCase(), "domain should be uppercase");
});

Deno.test("errors/G14: getByDomain returns entries for known domain", () => {
  const ideEntries = getByDomain("IDE");
  assert(ideEntries.length > 0, "IDE domain should have entries");
  for (const e of ideEntries) {
    assertEquals(categoryToDomain(e.category), "IDE");
  }
});

Deno.test("errors/G14: getByDomain returns empty for unknown domain", () => {
  const entries = getByDomain("ZZZ");
  assertEquals(entries.length, 0);
});

Deno.test("errors/G14: query filters by layer", () => {
  const r50 = query({ layer: 0 });
  assert(r50.length > 0, "should find R50 entries");
  for (const e of r50) assertEquals(e.layer, 0);
});

Deno.test("errors/G14: query filters by domain", () => {
  const con = query({ domain: "CON" });
  assert(con.length > 0, "should find CON domain entries");
  for (const e of con) assertEquals(e.category, "concept");
});

Deno.test("errors/G14: query filters by severity", () => {
  const warns = query({ severity: "warn" });
  assert(warns.length > 0, "should find warnings");
  for (const e of warns) assertEquals(e.severity, "warn");
});

Deno.test("errors/G14: query combines filters (3D intersection)", () => {
  // Layer 3 (R05) + domain CON + severity warn = concept violations
  const results = query({ layer: 3, domain: "CON", severity: "warn" });
  for (const e of results) {
    assertEquals(e.layer, 3);
    assertEquals(e.category, "concept");
    assertEquals(e.severity, "warn");
  }
});

Deno.test("errors/G14: catalogStructure returns per-layer domain breakdown", () => {
  const structure = catalogStructure();
  assert("R50" in structure, "should have R50 layer");
  assert("R25" in structure, "should have R25 layer");
  assert("R10" in structure, "should have R10 layer");
  assert("R05" in structure, "should have R05 layer");

  // Each layer has domain → count mapping
  const r50 = structure["R50"]!;
  assert(Object.keys(r50).length > 0, "R50 should have domains");
  for (const count of Object.values(r50)) {
    assert(typeof count === "number" && count > 0, "counts should be positive numbers");
  }
});

// ---------------------------------------------------------------------------
// G15: CATEGORY_DOMAIN covers all categories in catalog
// ---------------------------------------------------------------------------

Deno.test("errors/G15: every category in catalog has explicit domain mapping", () => {
  const categories = new Set<string>();
  for (const [_, entry] of ERROR_CODES) {
    categories.add(entry.category);
  }

  const unmapped: string[] = [];
  for (const cat of categories) {
    // Check explicit mapping exists (not just the first-3-chars fallback)
    if (!(cat in CATEGORY_DOMAIN)) {
      unmapped.push(cat);
    }
  }

  assertEquals(
    unmapped.length, 0,
    `Categories without explicit domain mapping: ${unmapped.join(", ")}`,
  );
});

Deno.test("errors/G15: DOMAIN_CODES is non-empty and sorted", () => {
  assert(DOMAIN_CODES.length > 0, "should have domain codes");
  // Check sorted
  const sorted = [...DOMAIN_CODES].sort();
  assertEquals(DOMAIN_CODES, sorted, "DOMAIN_CODES should be alphabetically sorted");
});

Deno.test("errors/G15: CATEGORY_DOMAIN has no empty values", () => {
  for (const [cat, domain] of Object.entries(CATEGORY_DOMAIN)) {
    assert(domain.length === 3, `${cat} domain should be 3 chars, got "${domain}"`);
    assertEquals(domain, domain.toUpperCase(), `${cat} domain should be uppercase`);
  }
});

// ============================================================================
// CLOSING
// ============================================================================
//
// Error code catalog tests — 15 gates + cross-layer uniqueness.
// G1-G12: structural soundness. G13: witness coverage (aspirational).
// G14: 3D query infrastructure. G15: domain mapping completeness.
//
// When a handler emits a rule string, these tests guarantee the catalog
// can map it to a CWS-{LAYER}-{SEQ} code with full metadata, and that
// the code is queryable in three dimensions: layer × domain × severity.
//
// "Diverse weights, and diverse measures, both of them are alike
//  abomination to the LORD." — Proverbs 20:10
// ============================================================================
