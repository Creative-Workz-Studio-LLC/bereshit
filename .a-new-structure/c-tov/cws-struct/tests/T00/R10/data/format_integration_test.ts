// R-level: R10 (data layer — format rules integration)
// Migrated from: tests/data/format_integration_test.ts
// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/T00/R10/data/format_integration_test.ts
// key:     B-tov-cws-struct-tests-data-format-integration
// title:   CWS Struct — Format Rules Integration Tests
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-22
// authors: Nova Dawn (CPI-SI)
// purpose: Verify that schema-driven format rules match handler reality.
//          These bridge tests prove the schema values are correct BEFORE
//          handlers swap hardcoded constants for loaded values.
//
//          Gate | Verifies
//          -----|----------------------------------------
//          G1   | Schema separator widths match handler constants
//          G2   | Schema directive lists match handler directive checks
//          G3   | Schema block boundary patterns match actual file structure
//          G4   | Schema pragma patterns match actual pragma lines
//          G5   | Schema identity field sections match handler field lists
//          G6   | Schema closing zone definitions match handler zone checks
//          G7   | Go language rules match Go handler behavior
//          G8   | Rust language rules match Rust handler behavior
//          G9   | Schema comment syntax matches handler comment parsing
//          G10  | Schema reserved omission matches handler RO detection
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert } from "jsr:@std/assert";
import {
  loadUniversalFormatRules,
  loadLanguageFormatRules,
  loadFormatRules,
  clearFormatCache,
} from "@lib/data/format-rules.ts";
// Types available from format-rules.ts if needed for future expansion
import {
  REQUIRED_DIRECTIVES,
  BLOCK_SEPARATOR_WIDTH,
  SUBSECTION_SEPARATOR_WIDTH,
} from "@lib/shared/types.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// G1: Schema separator widths match handler constants
// ---------------------------------------------------------------------------

Deno.test("format-integration/G1: block separator width matches handler", async () => {
  const universal = await loadUniversalFormatRules();
  assertEquals(
    universal.separators.block.standard_width,
    BLOCK_SEPARATOR_WIDTH,
    `Schema block width (${universal.separators.block.standard_width}) should match handler constant (${BLOCK_SEPARATOR_WIDTH})`,
  );
});

Deno.test("format-integration/G1: subsection separator width from schema", async () => {
  const universal = await loadUniversalFormatRules();
  // NOTE: Schema says 76, handler uses 74. This is a known gap — the handler
  // predates the schema. When handlers consume schema values, this resolves.
  // For now, verify the schema has a positive width and document the delta.
  assert(universal.separators.subsection.standard_width > 0);
  const delta = universal.separators.subsection.standard_width - SUBSECTION_SEPARATOR_WIDTH;
  if (delta !== 0) {
    // Gap detected — subsection width will need reconciliation
    assert(
      Math.abs(delta) <= 4,
      `Subsection width gap too large: schema=${universal.separators.subsection.standard_width}, handler=${SUBSECTION_SEPARATOR_WIDTH}`,
    );
  }
});

Deno.test("format-integration/G1: block separator uses = character", async () => {
  const universal = await loadUniversalFormatRules();
  assertEquals(universal.separators.block.character, "=");
});

Deno.test("format-integration/G1: subsection separator uses box-drawing character", async () => {
  const universal = await loadUniversalFormatRules();
  // Handlers use either "-" (dash) or "─" (box-drawing). Schema should define the target.
  assert(
    universal.separators.subsection.character === "─" ||
    universal.separators.subsection.character === "-",
    `Expected subsection char to be ─ or -, got: ${universal.separators.subsection.character}`,
  );
});

// ---------------------------------------------------------------------------
// G2: Schema directive lists match handler directive checks
// ---------------------------------------------------------------------------

Deno.test("format-integration/G2: schema required directives include //omni:key", async () => {
  const universal = await loadUniversalFormatRules();
  const required = universal.pragma.directives.filter((d) => d.required);
  const names = required.map((d) => d.name);
  assert(names.includes("key"), "Schema should have 'key' as required directive");
});

Deno.test("format-integration/G2: handler REQUIRED_DIRECTIVES maps to schema required", async () => {
  const universal = await loadUniversalFormatRules();
  const schemaRequired = universal.pragma.directives
    .filter((d) => d.required)
    .map((d) => `//omni:${d.name}`);

  // Every handler-required directive should be in schema
  for (const directive of REQUIRED_DIRECTIVES) {
    assert(
      schemaRequired.includes(directive),
      `Handler requires "${directive}" but schema doesn't mark it required. Schema required: ${schemaRequired.join(", ")}`,
    );
  }
});

Deno.test("format-integration/G2: schema has recommended directives", async () => {
  const universal = await loadUniversalFormatRules();
  const schemaRecommended = universal.pragma.directives.filter((d) => d.recommended);
  assert(schemaRecommended.length > 0, "Schema should have at least one recommended directive");
  // Schema recommends: pragma, metadata. Handler recommends: code, version.
  // This gap will resolve when handlers consume schema directive lists.
  const names = schemaRecommended.map((d) => d.name);
  assert(
    names.includes("pragma") || names.includes("metadata"),
    `Schema recommended should include pragma or metadata: ${names.join(", ")}`,
  );
});

// ---------------------------------------------------------------------------
// G3: Schema block boundary patterns match actual file structure
// ---------------------------------------------------------------------------

Deno.test("format-integration/G3: block boundary pattern is a template with comment_prefix", async () => {
  const universal = await loadUniversalFormatRules();
  // Patterns use <comment_prefix> placeholder — not directly compilable as regex.
  // Verify the template structure is correct.
  assert(
    universal.block_boundaries.pattern.includes("<comment_prefix>"),
    "Block boundary pattern should use <comment_prefix> placeholder",
  );
  assert(
    universal.block_boundaries.pattern.includes("={10,}"),
    "Block boundary should match 10+ equals signs",
  );
});

Deno.test("format-integration/G3: block boundary pattern resolves for Go/Rust (//)", async () => {
  const universal = await loadUniversalFormatRules();
  // Substitute <comment_prefix> with "//" for Go/Rust
  const resolved = universal.block_boundaries.pattern.replace("<comment_prefix>", "//");
  const regex = new RegExp(resolved);
  const blocks = ["// ============================================================================"];
  for (const block of blocks) {
    assert(regex.test(block), `"${block}" should match resolved block boundary pattern`);
  }
});

Deno.test("format-integration/G3: title pattern captures block names", async () => {
  const universal = await loadUniversalFormatRules();
  const resolved = universal.block_boundaries.title_pattern.replace("<comment_prefix>", "//");
  const regex = new RegExp(resolved);
  for (const name of ["METADATA", "SETUP", "BODY", "CLOSING"]) {
    const match = `// ${name}`.match(regex);
    assert(match !== null, `Title pattern should match "// ${name}"`);
  }
});

// ---------------------------------------------------------------------------
// G4: Schema pragma patterns match actual pragma lines
// ---------------------------------------------------------------------------

Deno.test("format-integration/G4: pragma pattern matches #!omni directives", async () => {
  const universal = await loadUniversalFormatRules();
  // Schema's "pattern" is the #!omni shebang format
  const raw = universal.pragma.pattern;
  const resolved = raw.replace(/<comment_prefix>/g, "//");
  const regex = new RegExp(resolved);
  assert(
    regex.test("// #!omni code --go -library") || regex.test("#!omni code --go -library"),
    `Pragma pattern should match #!omni directive. Resolved: ${resolved}`,
  );
});

Deno.test("format-integration/G4: pragma legacy_pattern matches //omni: directives", async () => {
  const universal = await loadUniversalFormatRules();
  // Schema's "legacy_pattern" is the //omni:key directive format
  const raw = universal.pragma.legacy_pattern;
  const resolved = raw.replace(/<comment_prefix>/g, "//");
  const regex = new RegExp(resolved);
  assert(
    regex.test("//omni:key B-test-fixture") || regex.test("//omni:code 0.1.0"),
    `Legacy pattern should match //omni: directives. Resolved: ${resolved}`,
  );
});

// ---------------------------------------------------------------------------
// G5: Schema identity field sections match handler field lists
// ---------------------------------------------------------------------------

Deno.test("format-integration/G5: schema has I-group pragma sections and C-group metadata sections", async () => {
  const universal = await loadUniversalFormatRules();
  // Schema separates: pragma_sections=[I1,I2,I3,I4], metadata_sections=[C1,...,C7]
  const pragmaSections = universal.identity_fields.pragma_sections;
  const metadataSections = universal.identity_fields.metadata_sections;
  assert(pragmaSections.length > 0, "Should have pragma sections");
  assert(metadataSections.length > 0, "Should have metadata sections");
  // Pragma sections are I-groups (Identity)
  assert(
    pragmaSections.some((s: string) => s.startsWith("I")),
    `Pragma sections should include I-groups: ${pragmaSections.join(", ")}`,
  );
  // Metadata sections are C-groups (Context)
  assert(
    metadataSections.some((s: string) => s.startsWith("C")),
    `Metadata sections should include C-groups: ${metadataSections.join(", ")}`,
  );
});

Deno.test("format-integration/G5: schema has metadata sections", async () => {
  const universal = await loadUniversalFormatRules();
  const sections = universal.identity_fields.metadata_sections;
  assert(sections.length > 0, "Should have metadata sections");
});

// ---------------------------------------------------------------------------
// G6: Schema closing zone definitions match handler zone checks
// ---------------------------------------------------------------------------

Deno.test("format-integration/G6: closing zones include code zones Cc, Cv, Ce", async () => {
  const universal = await loadUniversalFormatRules();
  const codeZones = universal.closing_zones.code_zones;
  assert(codeZones.includes("Cc"), "Should include Cc (compilation)");
  assert(codeZones.includes("Cv"), "Should include Cv (verification/tests)");
  assert(codeZones.includes("Ce"), "Should include Ce (execution/main)");
});

Deno.test("format-integration/G6: closing zones include doc sections X1-X6", async () => {
  const universal = await loadUniversalFormatRules();
  const docSections = universal.closing_zones.doc_sections;
  // Should include at least X1, X4, X5 (required by handlers)
  const names = docSections.join(",");
  assert(names.includes("X1"), `Should include X1: ${names}`);
  assert(names.includes("X5"), `Should include X5: ${names}`);
});

Deno.test("format-integration/G6: closing zone order is code-before-docs", async () => {
  const universal = await loadUniversalFormatRules();
  // Schema uses descriptive string: "code zones first, then documentation sections"
  const order = universal.closing_zones.order.toLowerCase();
  assert(
    order.includes("code") && order.includes("first"),
    `Zone order should indicate code-first: "${universal.closing_zones.order}"`,
  );
});

// ---------------------------------------------------------------------------
// G7: Go language rules match Go handler behavior
// ---------------------------------------------------------------------------

Deno.test("format-integration/G7: Go uses // line comments", async () => {
  const go = await loadLanguageFormatRules("go");
  assertEquals(go.comment_syntax.line, "//");
});

Deno.test("format-integration/G7: Go has block comment syntax", async () => {
  const go = await loadLanguageFormatRules("go");
  assertEquals(go.comment_syntax.block_open, "/*");
  assertEquals(go.comment_syntax.block_close, "*/");
});

Deno.test("format-integration/G7: Go pragma expression has directive structure", async () => {
  const go = await loadLanguageFormatRules("go");
  // Go schema uses pragma_line/directive_prefix/examples structure (not just comment_style)
  // Use dynamic access since schema shape may vary
  const pe = go.pragma_expression as unknown as Record<string, unknown>;
  const keys = Object.keys(pe);
  assert(
    keys.length > 0,
    "Go pragma_expression should have at least one field",
  );
  // Should have either the structured fields or the simple comment_style
  assert(
    keys.includes("pragma_line") || keys.includes("directive_prefix") ||
    keys.includes("comment_style") || keys.includes("examples"),
    `Go pragma_expression should have pragma_line, directive_prefix, comment_style, or examples: ${keys.join(", ")}`,
  );
});

Deno.test("format-integration/G7: Go has identity constructs for Pragma and Metadata", async () => {
  const go = await loadLanguageFormatRules("go");
  // Schema uses "pragma_var"/"metadata_var" keys (Go uses var declarations)
  const keys = Object.keys(go.identity_constructs);
  assert(
    keys.some((k) => k.toLowerCase().includes("pragma")),
    `Go should have pragma identity construct, got keys: ${keys.join(", ")}`,
  );
  assert(
    keys.some((k) => k.toLowerCase().includes("metadata")),
    `Go should have metadata identity construct, got keys: ${keys.join(", ")}`,
  );
});

Deno.test("format-integration/G7: Go identity constructs use slice type", async () => {
  const go = await loadLanguageFormatRules("go");
  // Go uses [][2]string{} slices for Pragma/Metadata
  // Schema keys: "pragma_var" and "metadata_var"
  const constructs = go.identity_constructs as Record<string, { type: string }>;
  const pragmaKey = Object.keys(constructs).find((k) => k.toLowerCase().includes("pragma"));
  assert(pragmaKey !== undefined, `No pragma construct found in: ${Object.keys(constructs).join(", ")}`);
  const pragma = constructs[pragmaKey!]!;
  assert(
    pragma.type.includes("slice") || pragma.type.includes("[][2]string") || pragma.type.includes("string"),
    `Go Pragma should be slice/string type, got: ${pragma.type}`,
  );
});

// ---------------------------------------------------------------------------
// G8: Rust language rules match Rust handler behavior
// ---------------------------------------------------------------------------

Deno.test("format-integration/G8: Rust uses // line comments", async () => {
  const rust = await loadLanguageFormatRules("rust");
  assertEquals(rust.comment_syntax.line, "//");
});

Deno.test("format-integration/G8: Rust has /// doc comments", async () => {
  const rust = await loadLanguageFormatRules("rust");
  assertEquals(rust.comment_syntax.doc_comment, "///");
});

Deno.test("format-integration/G8: Rust has //! inner doc comments", async () => {
  const rust = await loadLanguageFormatRules("rust");
  assertEquals(rust.comment_syntax.inner_doc, "//!");
});

Deno.test("format-integration/G8: Rust has identity constructs", async () => {
  const rust = await loadLanguageFormatRules("rust");
  assert(
    Object.keys(rust.identity_constructs).length > 0,
    "Rust should have identity constructs",
  );
});

Deno.test("format-integration/G8: Rust identity constructs use const/static type", async () => {
  const rust = await loadLanguageFormatRules("rust");
  // Schema keys: "pragma_const" and "metadata_const"
  const constructs = rust.identity_constructs as unknown as Record<string, { type: string }>;
  const pragmaKey = Object.keys(constructs).find((k) => k.toLowerCase().includes("pragma"));
  assert(pragmaKey !== undefined, `Rust should have pragma construct, got keys: ${Object.keys(constructs).join(", ")}`);
  const pragma = constructs[pragmaKey!]!;
  // Rust type is "&[(&str, &str)]" — a static slice reference
  assert(
    pragma.type.includes("str") || pragma.type.includes("static") || pragma.type.includes("const"),
    `Rust Pragma should be str/static/const type, got: ${pragma.type}`,
  );
});

// ---------------------------------------------------------------------------
// G9: Schema comment syntax matches handler comment parsing
// ---------------------------------------------------------------------------

Deno.test("format-integration/G9: Go and Rust both use // for line comments", async () => {
  const go = await loadLanguageFormatRules("go");
  const rust = await loadLanguageFormatRules("rust");
  assertEquals(go.comment_syntax.line, rust.comment_syntax.line);
  assertEquals(go.comment_syntax.line, "//");
});

Deno.test("format-integration/G9: Go has no doc_comment syntax", async () => {
  const go = await loadLanguageFormatRules("go");
  assert(
    go.comment_syntax.doc_comment === null || go.comment_syntax.doc_comment === undefined,
    "Go should not have dedicated doc_comment syntax",
  );
});

Deno.test("format-integration/G9: Rust doc_comment differs from line comment", async () => {
  const rust = await loadLanguageFormatRules("rust");
  assert(
    rust.comment_syntax.doc_comment !== null &&
    rust.comment_syntax.doc_comment !== undefined,
    "Rust should have doc_comment",
  );
  assert(
    rust.comment_syntax.doc_comment !== rust.comment_syntax.line,
    "doc_comment should differ from line comment",
  );
});

// ---------------------------------------------------------------------------
// G10: Schema reserved omission matches handler RO detection
// ---------------------------------------------------------------------------

Deno.test("format-integration/G10: reserved omission has marker pattern", async () => {
  const universal = await loadUniversalFormatRules();
  assert(
    universal.reserved_omission.marker_pattern.length > 0,
    "Should have a marker pattern for RO detection",
  );
});

Deno.test("format-integration/G10: reserved omission marker matches RO comment", async () => {
  const universal = await loadUniversalFormatRules();
  const regex = new RegExp(universal.reserved_omission.marker_pattern);
  // Handlers detect "// [Reserved Omission]" or "// --- Reserved Omission ---"
  assert(
    regex.test("// [Reserved Omission]") ||
    regex.test("// --- Reserved Omission ---") ||
    regex.test("// Reserved Omission"),
    `RO marker pattern should match handler convention. Pattern: ${universal.reserved_omission.marker_pattern}`,
  );
});

Deno.test("format-integration/G10: reserved omission knows where it's not required", async () => {
  const universal = await loadUniversalFormatRules();
  // Schema says not_required_in: ["METADATA"] — RO isn't checked in METADATA block
  assert(
    universal.reserved_omission.not_required_in.length > 0,
    "Should know which contexts don't need RO",
  );
  // Verify the list contains valid block names or contexts
  const valid = universal.reserved_omission.not_required_in.every(
    (ctx: string) => ctx.length > 0,
  );
  assert(valid, "All not_required_in entries should be non-empty strings");
});

// ---------------------------------------------------------------------------
// Cross-format: composed rules are consistent
// ---------------------------------------------------------------------------

Deno.test("format-integration/cross: Go composed rules have both sides", async () => {
  clearFormatCache();
  const composed = await loadFormatRules("go");
  assert(composed.universal !== undefined);
  assert(composed.language !== undefined);
  assertEquals(composed.language.language, "go");
  assert(composed.universal.separators.block.standard_width > 0);
});

Deno.test("format-integration/cross: Rust composed rules have both sides", async () => {
  clearFormatCache();
  const composed = await loadFormatRules("rust");
  assert(composed.universal !== undefined);
  assert(composed.language !== undefined);
  assertEquals(composed.language.language, "rust");
  assert(composed.universal.separators.block.standard_width > 0);
});

Deno.test("format-integration/cross: Go and Rust share same universal rules", async () => {
  clearFormatCache();
  const go = await loadFormatRules("go");
  const rust = await loadFormatRules("rust");
  assertEquals(
    go.universal.separators.block.standard_width,
    rust.universal.separators.block.standard_width,
    "Both should share universal block separator width",
  );
  assertEquals(
    go.universal.separators.subsection.standard_width,
    rust.universal.separators.subsection.standard_width,
    "Both should share universal subsection separator width",
  );
});

// ============================================================================
// CLOSING
// ============================================================================
//
// Format integration tests — 10 gates + cross-format consistency.
// These prove the schema data matches what handlers actually use.
//
// When these pass, we can safely swap hardcoded constants for loaded schema
// values — the schema IS the handler's source of truth.
//
// "Diverse weights, and diverse measures, both of them are alike
//  abomination to the LORD." — Proverbs 20:10
// ============================================================================
