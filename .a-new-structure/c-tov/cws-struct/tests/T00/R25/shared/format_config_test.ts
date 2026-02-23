// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/T00/R25/shared/format_config_test.ts
// key:     B-tov-cws-struct-tests-shared-format-config
// title:   CWS Struct — FormatConfig Tests
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-22
// authors: Nova Dawn (CPI-SI)
// purpose: Verify FormatConfig loading, gap detection, and fallback behavior.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert } from "jsr:@std/assert";
import {
  loadFormatConfig,
  defaultFormatConfig,
  hasFormatGaps,
  formatGapReport,
  clearFormatConfigCache,
} from "@lib/shared/format-config.ts";
import {
  BLOCK_SEPARATOR_WIDTH,
  SUBSECTION_SEPARATOR_WIDTH,
} from "@lib/shared/types.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Default config — hardcoded fallback values
// ---------------------------------------------------------------------------

Deno.test("format-config/default: block separator width matches hardcoded", () => {
  const config = defaultFormatConfig();
  assertEquals(config.blockSeparatorWidth, BLOCK_SEPARATOR_WIDTH);
});

Deno.test("format-config/default: subsection separator width matches hardcoded", () => {
  const config = defaultFormatConfig();
  assertEquals(config.subsectionSeparatorWidth, SUBSECTION_SEPARATOR_WIDTH);
});

Deno.test("format-config/default: has required directives", () => {
  const config = defaultFormatConfig();
  assert(config.requiredDirectives.length > 0);
  assert(config.requiredDirectives.includes("//omni:key"));
});

Deno.test("format-config/default: has no gaps", () => {
  const config = defaultFormatConfig();
  assertEquals(config.gaps.length, 0);
  assert(!hasFormatGaps(config));
});

Deno.test("format-config/default: closing zones populated", () => {
  const config = defaultFormatConfig();
  assert(config.closingCodeZones.length > 0);
  assert(config.closingDocSections.length > 0);
  assert(config.closingZoneOrder.length > 0);
});

// ---------------------------------------------------------------------------
// Schema-loaded config — universal
// ---------------------------------------------------------------------------

Deno.test("format-config/schema: loads universal config", async () => {
  clearFormatConfigCache();
  const config = await loadFormatConfig();
  assert(config.blockSeparatorWidth > 0, "Should have positive block width");
  assert(config.subsectionSeparatorWidth > 0, "Should have positive subsection width");
});

Deno.test("format-config/schema: block separator matches schema", async () => {
  clearFormatConfigCache();
  const config = await loadFormatConfig();
  // Schema says 76 for block — same as handler
  assertEquals(config.blockSeparatorWidth, 76);
});

Deno.test("format-config/schema: detects subsection width gap", async () => {
  clearFormatConfigCache();
  const config = await loadFormatConfig();
  // Schema says 76, handler says 74 — this is a known gap
  if (config.subsectionSeparatorWidth !== SUBSECTION_SEPARATOR_WIDTH) {
    assert(hasFormatGaps(config), "Should detect subsection width gap");
    const report = formatGapReport(config);
    assert(report.some((r) => r.includes("subsectionSeparatorWidth")));
  }
});

Deno.test("format-config/schema: has pragma patterns", async () => {
  clearFormatConfigCache();
  const config = await loadFormatConfig();
  assert(config.pragmaPattern.length > 0, "Should have pragma pattern");
  assert(config.pragmaLegacyPattern.length > 0, "Should have legacy pragma pattern");
});

Deno.test("format-config/schema: has closing zones from schema", async () => {
  clearFormatConfigCache();
  const config = await loadFormatConfig();
  assert(config.closingCodeZones.includes("Cv"), "Should include Cv");
  assert(config.closingCodeZones.includes("Ce"), "Should include Ce");
  assert(config.closingCodeZones.includes("Cc"), "Should include Cc");
});

Deno.test("format-config/schema: required directives include //omni:key", async () => {
  clearFormatConfigCache();
  const config = await loadFormatConfig();
  assert(
    config.requiredDirectives.includes("//omni:key"),
    `Required should include //omni:key: ${config.requiredDirectives.join(", ")}`,
  );
});

// ---------------------------------------------------------------------------
// Language-specific config
// ---------------------------------------------------------------------------

Deno.test("format-config/go: loads Go config with comment syntax", async () => {
  clearFormatConfigCache();
  const config = await loadFormatConfig("go");
  assert(config.commentSyntax !== null, "Go should have comment syntax");
  assertEquals(config.commentSyntax!.line, "//");
});

Deno.test("format-config/rust: loads Rust config with comment syntax", async () => {
  clearFormatConfigCache();
  const config = await loadFormatConfig("rust");
  assert(config.commentSyntax !== null, "Rust should have comment syntax");
  assertEquals(config.commentSyntax!.line, "//");
  assertEquals(config.commentSyntax!.doc_comment, "///");
});

// ---------------------------------------------------------------------------
// Caching
// ---------------------------------------------------------------------------

Deno.test("format-config/cache: returns same object on second call", async () => {
  clearFormatConfigCache();
  const first = await loadFormatConfig("go");
  const second = await loadFormatConfig("go");
  assert(first === second, "Should return cached instance");
});

Deno.test("format-config/cache: clear cache produces fresh load", async () => {
  const first = await loadFormatConfig("go");
  clearFormatConfigCache();
  const second = await loadFormatConfig("go");
  assert(first !== second, "Should be a different instance after cache clear");
  assertEquals(first.blockSeparatorWidth, second.blockSeparatorWidth);
});

// ---------------------------------------------------------------------------
// Gap reporting
// ---------------------------------------------------------------------------

Deno.test("format-config/gaps: gap report is array of strings", async () => {
  clearFormatConfigCache();
  const config = await loadFormatConfig();
  const report = formatGapReport(config);
  assert(Array.isArray(report));
  for (const line of report) {
    assert(typeof line === "string");
  }
});

// ============================================================================
// CLOSING
// ============================================================================
//
// FormatConfig tests — verify schema loading, gap detection, fallback.
// These ensure the bridge between schema and handler is solid before
// handlers swap hardcoded constants for loaded values.
//
// "Prove all things; hold fast that which is good." — 1 Thess 5:21
// ============================================================================
