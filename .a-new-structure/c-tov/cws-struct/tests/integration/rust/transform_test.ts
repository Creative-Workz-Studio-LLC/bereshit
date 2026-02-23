// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/integration/rust/transform_test.ts
// purpose: Rust transform pipeline tests — separator normalization, test/main
//          movement, closing zone reorder, scaffold (structure generation,
//          section headers, overviews, Reserved Omission, metadata formatting,
//          auto-derivation), and valid-library no-op.
// source:  Split from tests/handlers/rust_test.ts
// layer:   Integration — transform pipeline
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert, assertGreater } from "jsr:@std/assert";
import {
  fixture, lazyFormat, byRule, hasRule, hasMessage,
} from "../../helpers.ts";

// ============================================================================
// BODY
// ============================================================================

const rust = lazyFormat("rust");

// ---------------------------------------------------------------------------
// transform/separators — Separator normalization
// ---------------------------------------------------------------------------

Deno.test("transform/separators: dry-run detects ASCII dashes and prefix issues", async () => {
  const f = fixture("rust/format/transform-separators.rs");
  const results = await rust.transform!(f, { dryRun: true, extensions: false, force: false, steps: false });
  const dashRules = byRule(results, "dash-to-unicode");
  assert(dashRules.length >= 2, `Expected ≥2 dash-to-unicode, got ${dashRules.length}`);
  const prefixRules = byRule(results, "prefix-normalize");
  assert(prefixRules.length >= 1, `Expected ≥1 prefix-normalize, got ${prefixRules.length}`);
});

Deno.test("transform/separators: dry-run does not modify file", async () => {
  const f = fixture("rust/format/transform-separators.rs");
  const before = await Deno.readTextFile(f);
  await rust.transform!(f, { dryRun: true, extensions: false, force: false, steps: false });
  const after = await Deno.readTextFile(f);
  assertEquals(before, after, "File should be unchanged after dry-run");
});

// ---------------------------------------------------------------------------
// transform/move-tests — Test module movement to CLOSING Cv
// ---------------------------------------------------------------------------

Deno.test("transform/move-tests: dry-run detects #[cfg(test)] in BODY", async () => {
  const f = fixture("rust/closing/tests-in-body.rs");
  const results = await rust.transform!(f, { dryRun: true, extensions: false, force: false, steps: false });
  assert(hasRule(results, "move-tests"), "Should detect tests to move");
  assert(hasMessage(results, "#[cfg(test)]"), "Message should mention #[cfg(test)]");
});

Deno.test("transform/move-tests: actual transform moves tests to CLOSING Cv", async () => {
  // Work on a temp copy
  const src = fixture("rust/closing/tests-in-body.rs");
  const tmp = await Deno.makeTempFile({ suffix: ".rs" });
  await Deno.copyFile(src, tmp);

  try {
    const results = await rust.transform!(tmp, { dryRun: false, extensions: false, force: false, steps: false });
    assert(hasRule(results, "move-tests"), "Should report move-tests");
    assert(hasRule(results, "written"), "Should write file");

    // Lint the transformed file — should have NO test-placement warning
    const lintResults = await rust.lint(tmp);
    const testPlacement = byRule(lintResults, "test-placement");
    assertEquals(testPlacement.length, 0, "Transformed file should have no test-placement warnings");

    // Verify the content moved: CLOSING should contain cfg(test), BODY should not
    const content = await Deno.readTextFile(tmp);
    const closingStart = content.indexOf("// CLOSING\n");
    const bodyStart = content.indexOf("// BODY\n");
    const bodyEnd = content.indexOf("// END BODY");
    assert(closingStart > 0, "Should have CLOSING block");
    assert(content.indexOf("#[cfg(test)]", closingStart) > 0, "cfg(test) should be in CLOSING");
    // Check BODY section specifically — extract between BODY and END BODY
    const bodySection = content.slice(bodyStart, bodyEnd);
    assertEquals(bodySection.includes("\n#[cfg(test)]"), false,
      "BODY section should not contain #[cfg(test)] code");
  } finally {
    await Deno.remove(tmp);
  }
});

// ---------------------------------------------------------------------------
// transform/move-main — Main function movement to CLOSING Ce
// ---------------------------------------------------------------------------

Deno.test("transform/move-main: dry-run detects fn main() in BODY", async () => {
  const f = fixture("rust/closing/main-in-body.rs");
  const results = await rust.transform!(f, { dryRun: true, extensions: false, force: false, steps: false });
  assert(hasRule(results, "move-main"), "Should detect main to move");
  assert(hasMessage(results, "fn main()"), "Message should mention fn main()");
});

Deno.test("transform/move-main: actual transform moves main to CLOSING Ce", async () => {
  const src = fixture("rust/closing/main-in-body.rs");
  const tmp = await Deno.makeTempFile({ suffix: ".rs" });
  await Deno.copyFile(src, tmp);

  try {
    const results = await rust.transform!(tmp, { dryRun: false, extensions: false, force: false, steps: false });
    assert(hasRule(results, "move-main"), "Should report move-main");

    // Lint the transformed file
    const lintResults = await rust.lint(tmp);
    const mainPlacement = byRule(lintResults, "main-placement");
    assertEquals(mainPlacement.length, 0, "Transformed file should have no main-placement warnings");

    // Verify main is in CLOSING
    const content = await Deno.readTextFile(tmp);
    const closingStart = content.indexOf("// CLOSING");
    assert(content.indexOf("fn main()", closingStart) > 0, "fn main() should be in CLOSING");
  } finally {
    await Deno.remove(tmp);
  }
});

// ---------------------------------------------------------------------------
// transform/reorder-closing — CLOSING zone reorder
// ---------------------------------------------------------------------------

Deno.test("transform/reorder-closing: dry-run detects out-of-order zones", async () => {
  const f = fixture("rust/closing/wrong-closing-order.rs");
  const results = await rust.transform!(f, { dryRun: true, extensions: false, force: false, steps: false });
  assert(hasRule(results, "reorder-closing"), "Should detect zones needing reorder");
});

Deno.test("transform/reorder-closing: actual transform fixes zone order", async () => {
  const src = fixture("rust/closing/wrong-closing-order.rs");
  const tmp = await Deno.makeTempFile({ suffix: ".rs" });
  await Deno.copyFile(src, tmp);

  try {
    const results = await rust.transform!(tmp, { dryRun: false, extensions: false, force: false, steps: false });
    assert(hasRule(results, "reorder-closing"), "Should report reorder");

    // Lint the transformed file
    const lintResults = await rust.lint(tmp);
    const zoneOrder = byRule(lintResults, "zone-order");
    assertEquals(zoneOrder.length, 0, "Transformed file should have no zone-order warnings");

    // Verify code zones come before doc zones in the output
    const content = await Deno.readTextFile(tmp);
    const cvPos = content.indexOf("// Cv");
    const x1Pos = content.indexOf("// X1");
    assert(cvPos > 0 && x1Pos > 0, "Should have both Cv and X1 zones");
    assert(cvPos < x1Pos, "Cv should come before X1 after reorder");
  } finally {
    await Deno.remove(tmp);
  }
});

// ---------------------------------------------------------------------------
// transform/valid-library — No-op on valid file
// ---------------------------------------------------------------------------

Deno.test("transform/valid-library: no changes needed", async () => {
  const f = fixture("rust/structure/valid-library.rs");
  const results = await rust.transform!(f, { dryRun: true, extensions: false, force: false, steps: false });
  // Valid library may have some separator width fixes but no structural moves
  const moves = results.filter((r) =>
    r.rule.includes("move-tests") ||
    r.rule.includes("move-main") ||
    r.rule.includes("reorder-closing"));
  assertEquals(moves.length, 0, "Valid library should need no structural transforms");
});

// ---------------------------------------------------------------------------
// transform/scaffold — Transform 0: structural scaffold from raw files
// ---------------------------------------------------------------------------

Deno.test("transform/scaffold: raw module gets 4-block structure", async () => {
  const src = fixture("rust/format/scaffold-raw-module.rs");
  const tmp = await Deno.makeTempFile({ suffix: ".rs" });
  await Deno.copyFile(src, tmp);

  try {
    const results = await rust.transform!(tmp, { dryRun: false, extensions: false, force: false, steps: false });
    assert(hasRule(results, "scaffold"), "Should report scaffold transform");

    const content = await Deno.readTextFile(tmp);

    // All 4 blocks present with tagged format
    assert(content.includes("// METADATA BLOCK [METADATA]"), "Should have METADATA block");
    assert(content.includes("// SETUP BLOCK [SETUP]"), "Should have SETUP block");
    assert(content.includes("// BODY BLOCK [BODY]"), "Should have BODY block");
    assert(content.includes("// CLOSING BLOCK [CLOSING]"), "Should have CLOSING block");

    // All 4 END markers present
    assert(content.includes("// END METADATA [END]"), "Should have END METADATA");
    assert(content.includes("// END SETUP [END]"), "Should have END SETUP");
    assert(content.includes("// END BODY [END]"), "Should have END BODY");
    assert(content.includes("// END CLOSING [END]"), "Should have END CLOSING");
  } finally {
    await Deno.remove(tmp);
  }
});

Deno.test("transform/scaffold: numbered section headers in SETUP", async () => {
  const src = fixture("rust/format/scaffold-raw-module.rs");
  const tmp = await Deno.makeTempFile({ suffix: ".rs" });
  await Deno.copyFile(src, tmp);

  try {
    await rust.transform!(tmp, { dryRun: false, extensions: false, force: false, steps: false });
    const content = await Deno.readTextFile(tmp);

    // Imports should be numbered "1. Imports" (from form schema position)
    assert(content.includes("// 1. Imports"), "SETUP should have numbered Imports header");
  } finally {
    await Deno.remove(tmp);
  }
});

Deno.test("transform/scaffold: SETUP block overview with section index", async () => {
  const src = fixture("rust/format/scaffold-raw-module.rs");
  const tmp = await Deno.makeTempFile({ suffix: ".rs" });
  await Deno.copyFile(src, tmp);

  try {
    await rust.transform!(tmp, { dryRun: false, extensions: false, force: false, steps: false });
    const content = await Deno.readTextFile(tmp);

    // SETUP overview text
    assert(content.includes("SETUP makes things EXIST"), "SETUP should have overview text");
    // Section index with descriptions
    assert(content.includes("Imports"), "Section index should list Imports");
    assert(content.includes("RESERVED"), "Section index should mark RESERVED sections");
  } finally {
    await Deno.remove(tmp);
  }
});

Deno.test("transform/scaffold: BODY block overview with section index", async () => {
  const src = fixture("rust/format/scaffold-raw-module.rs");
  const tmp = await Deno.makeTempFile({ suffix: ".rs" });
  await Deno.copyFile(src, tmp);

  try {
    await rust.transform!(tmp, { dryRun: false, extensions: false, force: false, steps: false });
    const content = await Deno.readTextFile(tmp);

    // BODY overview text
    assert(content.includes("BODY makes things HAPPEN"), "BODY should have overview text");
    // Section index
    assert(content.includes("Identity Access"), "BODY index should list Identity Access");
    assert(content.includes("Free Functions"), "BODY index should list Free Functions");
  } finally {
    await Deno.remove(tmp);
  }
});

Deno.test("transform/scaffold: CLOSING uses descriptive zone headers", async () => {
  const src = fixture("rust/format/scaffold-raw-module.rs");
  const tmp = await Deno.makeTempFile({ suffix: ".rs" });
  await Deno.copyFile(src, tmp);

  try {
    await rust.transform!(tmp, { dryRun: false, extensions: false, force: false, steps: false });
    const content = await Deno.readTextFile(tmp);

    // Descriptive zone headers (not bare)
    assert(content.includes("Cv \u2014 Closing Validation"), "Should have 'Cv \u2014 Closing Validation'");
    assert(content.includes("Ce \u2014 Closing Execution"), "Should have 'Ce \u2014 Closing Execution'");
    assert(content.includes("X1: Modification Policy"), "Should have 'X1: Modification Policy'");
    assert(content.includes("X4: Reference"), "Should have 'X4: Reference'");
    assert(content.includes("X5: Closing Note"), "Should have 'X5: Closing Note'");
  } finally {
    await Deno.remove(tmp);
  }
});

Deno.test("transform/scaffold: test module moves to CLOSING Cv", async () => {
  const src = fixture("rust/format/scaffold-raw-module.rs");
  const tmp = await Deno.makeTempFile({ suffix: ".rs" });
  await Deno.copyFile(src, tmp);

  try {
    await rust.transform!(tmp, { dryRun: false, extensions: false, force: false, steps: false });
    const content = await Deno.readTextFile(tmp);

    // Tests should be in CLOSING, not BODY
    const closingStart = content.indexOf("// CLOSING BLOCK");
    const bodyStart = content.indexOf("// BODY BLOCK");
    const bodyEnd = content.indexOf("// END BODY");
    assert(closingStart > 0, "Should have CLOSING block");

    // cfg(test) should be after CLOSING, not between BODY/END BODY
    const bodySection = content.slice(bodyStart, bodyEnd);
    assertEquals(bodySection.includes("#[cfg(test)]"), false,
      "BODY should not contain test module");

    const closingSection = content.slice(closingStart);
    assert(closingSection.includes("#[cfg(test)]"),
      "CLOSING should contain test module");
  } finally {
    await Deno.remove(tmp);
  }
});

Deno.test("transform/scaffold: Reserved Omission uses readable labels", async () => {
  const src = fixture("rust/format/scaffold-raw-module.rs");
  const tmp = await Deno.makeTempFile({ suffix: ".rs" });
  await Deno.copyFile(src, tmp);

  try {
    await rust.transform!(tmp, { dryRun: false, extensions: false, force: false, steps: false });
    const content = await Deno.readTextFile(tmp);

    // Labels should be human-readable (spaces, not CamelCase)
    assert(content.includes("Type Aliases"), "Should use 'Type Aliases' not 'TypeAliases'");
    assert(content.includes("Core Types"), "Should use 'Core Types' not 'CoreTypes'");
    assert(content.includes("Error Types"), "Should use 'Error Types' not 'ErrorTypes'");
    assert(content.includes("Trait Defs"), "Should use 'Trait Defs' not 'TraitDefs'");
  } finally {
    await Deno.remove(tmp);
  }
});

Deno.test("transform/scaffold: Reserved Omission uses Available/Reserved groups", async () => {
  const src = fixture("rust/format/scaffold-raw-module.rs");
  const tmp = await Deno.makeTempFile({ suffix: ".rs" });
  await Deno.copyFile(src, tmp);

  try {
    await rust.transform!(tmp, { dryRun: false, extensions: false, force: false, steps: false });
    const content = await Deno.readTextFile(tmp);
    const lines = content.split("\n");

    // Strict mode (default): Should have "Available (" header and "Reserved (" header
    const hasAvailable = lines.some((l: string) => l.includes("Available (not needed in this module):"));
    const hasReserved = lines.some((l: string) => l.includes("Reserved (structural"));
    assert(hasAvailable, "Should have 'Available (not needed in this module):' group header");
    assert(hasReserved, "Should have 'Reserved (structural' group header");

    // Reserved entries should use em-dash (\u2014) separator
    const reservedLines = lines.filter((l: string) =>
      l.startsWith("//   ") && l.includes("\u2014") && !l.includes("Available") && !l.includes("Reserved"));
    assertGreater(reservedLines.length, 0, "Should have reserved entries with em-dash separator");
  } finally {
    await Deno.remove(tmp);
  }
});

Deno.test("transform/scaffold: block overviews use bracket format [N]", async () => {
  const src = fixture("rust/format/scaffold-raw-module.rs");
  const tmp = await Deno.makeTempFile({ suffix: ".rs" });
  await Deno.copyFile(src, tmp);

  try {
    await rust.transform!(tmp, { dryRun: false, extensions: false, force: false, steps: false });
    const content = await Deno.readTextFile(tmp);
    const lines = content.split("\n");

    // SETUP overview should use bracket format [N], not numbered N.
    const setupOverview = lines.filter((l: string) => /\/\/\s+\[\d+\]\s+\w/.test(l));
    assertGreater(setupOverview.length, 0, "SETUP overview should use bracket format [N]");
    // Should NOT have old numbered format (//   N. Label) in overview lines
    const oldFormat = lines.filter((l: string) => /\/\/\s{2,}\d+\.\s+\w/.test(l) && !l.includes("BLOCK"));
    // Old format lines would be section HEADERS (like "// 1. Imports") — those are fine
    // But overview lines should be brackets
    assert(setupOverview.some((l: string) => l.includes("[1]")), "Should have [1] bracket tag");

    // CLOSING overview should use bracket format [Cv], [X1], etc.
    const closingOverview = lines.filter((l: string) => /\/\/\s+\[(?:Cv|Ce|Cc|X[1-5])\]/.test(l));
    assertGreater(closingOverview.length, 5, "CLOSING should have 5+ bracket-tagged zones");
    assert(closingOverview.some((l: string) => l.includes("[Cv]")), "Should have [Cv] in CLOSING overview");
    assert(closingOverview.some((l: string) => l.includes("[X5]")), "Should have [X5] in CLOSING overview");
  } finally {
    await Deno.remove(tmp);
  }
});

Deno.test("transform/scaffold: CLOSING has block overview", async () => {
  const src = fixture("rust/format/scaffold-raw-module.rs");
  const tmp = await Deno.makeTempFile({ suffix: ".rs" });
  await Deno.copyFile(src, tmp);

  try {
    await rust.transform!(tmp, { dryRun: false, extensions: false, force: false, steps: false });
    const content = await Deno.readTextFile(tmp);

    // CLOSING overview purpose statement
    assert(content.includes("Closing ensures correctness"), "Should have CLOSING purpose statement");
    assert(content.includes("Section order:"), "Should have 'Section order:' in CLOSING");
  } finally {
    await Deno.remove(tmp);
  }
});

Deno.test("transform/scaffold: dry-run produces no file changes", async () => {
  const src = fixture("rust/format/scaffold-raw-module.rs");
  const before = await Deno.readTextFile(src);
  const results = await rust.transform!(src, { dryRun: true, extensions: false, force: false, steps: false });
  const after = await Deno.readTextFile(src);

  assertEquals(before, after, "Dry-run should not modify fixture");
  assert(hasRule(results, "scaffold"), "Should report scaffold in dry-run");
});

Deno.test("transform/scaffold: scaffolded file passes lint", async () => {
  const src = fixture("rust/format/scaffold-raw-module.rs");
  const tmp = await Deno.makeTempFile({ suffix: ".rs" });
  await Deno.copyFile(src, tmp);

  try {
    await rust.transform!(tmp, { dryRun: false, extensions: false, force: false, steps: false });
    const lintResults = await rust.lint(tmp);

    const errors = lintResults.filter((r) => r.severity === "error");
    assertEquals(errors.length, 0,
      `Scaffolded file should have 0 lint errors, got: ${errors.map((e) => e.message).join("; ")}`);
  } finally {
    await Deno.remove(tmp);
  }
});

// ---------------------------------------------------------------------------
// transform/metadata — METADATA block formatting with identity groups
// ---------------------------------------------------------------------------

Deno.test("transform/scaffold: METADATA has section headers", async () => {
  const src = fixture("rust/format/scaffold-raw-module.rs");
  const tmp = await Deno.makeTempFile({ suffix: ".rs" });
  await Deno.copyFile(src, tmp);

  try {
    await rust.transform!(tmp, { dryRun: false, extensions: false, force: false, steps: false });
    const content = await Deno.readTextFile(tmp);
    const lines = content.split("\n");

    // Section headers should be present
    const hasIdentityHeader = lines.some(l => l.includes("Identity (I1-I4)"));
    const hasContextHeader = lines.some(l => l.includes("Context (C1-C7)"));
    assert(hasIdentityHeader, "METADATA should have Identity (I1-I4) section header");
    assert(hasContextHeader, "METADATA should have Context (C1-C7) section header");
  } finally {
    await Deno.remove(tmp);
  }
});

Deno.test("transform/scaffold: METADATA has group comments", async () => {
  const src = fixture("rust/format/scaffold-raw-module.rs");
  const tmp = await Deno.makeTempFile({ suffix: ".rs" });
  await Deno.copyFile(src, tmp);

  try {
    await rust.transform!(tmp, { dryRun: false, extensions: false, force: false, steps: false });
    const content = await Deno.readTextFile(tmp);
    const lines = content.split("\n");

    // Group comments should be present for at least I1 and C1
    const hasI1Core = lines.some(l => l.trim() === "// I1: Core");
    const hasC1State = lines.some(l => l.trim() === "// C1: State");
    assert(hasI1Core, "PRAGMA should have // I1: Core group comment");
    assert(hasC1State, "METADATA should have // C1: State group comment");
  } finally {
    await Deno.remove(tmp);
  }
});

Deno.test("transform/scaffold: METADATA has docstrings", async () => {
  const src = fixture("rust/format/scaffold-raw-module.rs");
  const tmp = await Deno.makeTempFile({ suffix: ".rs" });
  await Deno.copyFile(src, tmp);

  try {
    await rust.transform!(tmp, { dryRun: false, extensions: false, force: false, steps: false });
    const content = await Deno.readTextFile(tmp);
    const lines = content.split("\n");

    const hasPragmaDoc = lines.some(l => l.includes("/// OmniCode identity for this module."));
    const hasMetadataDoc = lines.some(l => l.includes("/// OmniCode context for this module."));
    assert(hasPragmaDoc, "PRAGMA static should have docstring");
    assert(hasMetadataDoc, "METADATA static should have docstring");
  } finally {
    await Deno.remove(tmp);
  }
});

Deno.test("transform/scaffold: METADATA pragma entries are column-aligned", async () => {
  const src = fixture("rust/format/scaffold-raw-module.rs");
  const tmp = await Deno.makeTempFile({ suffix: ".rs" });
  await Deno.copyFile(src, tmp);

  try {
    await rust.transform!(tmp, { dryRun: false, extensions: false, force: false, steps: false });
    const content = await Deno.readTextFile(tmp);
    const lines = content.split("\n");

    // Find pragma entry lines — actual tuple entries containing field=value pairs
    // Pattern: whitespace + '("I' + digit + '.' + anything + '",' (the closing of value)
    const pragmaEntries = lines.filter(l => {
      const trimmed = l.trim();
      return trimmed.startsWith('("I') && trimmed.includes('",') && trimmed.endsWith('"),');
    });
    assert(pragmaEntries.length >= 10, `Should have 10+ pragma entries, got ${pragmaEntries.length}`);

    // All entries should have the closing '", "' at the same column
    // (column alignment means field names are padded to align value positions)
    const valueStarts = pragmaEntries.map(l => l.indexOf('", "'));
    const uniqueStarts = new Set(valueStarts.filter(v => v >= 0));
    assertEquals(uniqueStarts.size, 1,
      `Pragma entries should be column-aligned but found ${uniqueStarts.size} different positions: ${[...uniqueStarts].join(", ")}`);
  } finally {
    await Deno.remove(tmp);
  }
});

// ---------------------------------------------------------------------------
// transform/auto-derivation — Phase 7: I3.path, I3.provides, I3.component
// ---------------------------------------------------------------------------

Deno.test("transform/scaffold: auto-derives I3.path from file path", async () => {
  const src = fixture("rust/format/scaffold-raw-module.rs");
  const tmp = await Deno.makeTempFile({ suffix: ".rs" });
  await Deno.copyFile(src, tmp);

  try {
    await rust.transform!(tmp, { dryRun: false, extensions: false, force: false, steps: false });
    const content = await Deno.readTextFile(tmp);

    // I3.path should NOT be a placeholder [path] — should have real content
    // Since the temp file path won't match repo markers, it should use fallback (last 3 segments)
    const pathLine = content.split("\n").find((l: string) => l.includes('"I3.path"'));
    assert(pathLine, "Should have I3.path entry");
    assert(!pathLine.includes('"[path]"'), "I3.path should be auto-derived, not placeholder");
  } finally {
    await Deno.remove(tmp);
  }
});

Deno.test("transform/scaffold: auto-derives I3.provides from pub items", async () => {
  const src = fixture("rust/format/scaffold-raw-module.rs");
  const tmp = await Deno.makeTempFile({ suffix: ".rs" });
  await Deno.copyFile(src, tmp);

  try {
    await rust.transform!(tmp, { dryRun: false, extensions: false, force: false, steps: false });
    const content = await Deno.readTextFile(tmp);

    // I3.provides should be auto-derived if source has pub items
    const providesLine = content.split("\n").find((l: string) => l.includes('"I3.provides"'));
    assert(providesLine, "Should have I3.provides entry");
    // The fixture has pub fn exists() — check it was captured
    if (content.includes("pub fn ")) {
      assert(!providesLine.includes('"[provides]"'),
        "I3.provides should be auto-derived from pub fn signatures");
    }
  } finally {
    await Deno.remove(tmp);
  }
});

// ============================================================================
// CLOSING
// ============================================================================
//
// Rust transform pipeline tests — 27 tests covering: separators (2),
// move-tests (2), move-main (2), reorder-closing (2), valid-library (1),
// scaffold structure (6), scaffold formatting (6), scaffold metadata (4),
// scaffold auto-derivation (2).
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
