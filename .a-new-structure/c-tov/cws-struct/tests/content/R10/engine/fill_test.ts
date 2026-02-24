// R-level: R10 (engine layer — fill/generation)
// Migrated from: tests/engine/fill_test.ts
// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/T00/R10/engine/fill_test.ts
// key:     B-tov-cws-struct-tests-engine-fill
// title:   CWS Struct — Fill Engine Tests
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-19
// authors: Nova Dawn (CPI-SI)
// purpose: Tests for the schema-driven fill engine. THE PROOF:
//          generate a file from schema → lint it → 0 errors.
//          If this test passes, schema = template = linter = generator.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert, assertStringIncludes } from "jsr:@std/assert";
import { generateFile, generateFileText } from "@lib/engine/fill.ts";
import type { FillContext } from "@lib/engine/fill.ts";
import { getFormat } from "@lib/engine/registry.ts";

// Register format handlers (side-effect imports)
import "@lib/handlers/go.ts";
import "@lib/handlers/rust.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Helper — build a standard FillContext
// ---------------------------------------------------------------------------

function makeCtx(format: "go" | "rust", subtype = "library"): FillContext {
  return {
    format,
    subtype,
    key: "B-test-generated-file",
    title: "Test Generated File",
    purpose: "Verify schema-driven file generation.",
    filename: format === "go" ? "generated.go" : "generated.rs",
    packageOrCrate: "generated",
    from: format === "go"
      ? "b-word/test/generated.go"
      : "b-word/test/generated.rs",
  };
}

// ---------------------------------------------------------------------------
// Go — generation tests
// ---------------------------------------------------------------------------

Deno.test("[CWS-T00-088] fill/go-library: generates valid Go library file", async () => {
  const text = await generateFileText(makeCtx("go", "library"));

  // Has all 4 blocks
  assertStringIncludes(text, "// METADATA");
  assertStringIncludes(text, "// END METADATA");
  assertStringIncludes(text, "// SETUP");
  assertStringIncludes(text, "// END SETUP");
  assertStringIncludes(text, "// BODY");
  assertStringIncludes(text, "// END BODY");
  assertStringIncludes(text, "// CLOSING");
  assertStringIncludes(text, "// END CLOSING");
});

Deno.test("fill/go-library: has directives before package", async () => {
  const text = await generateFileText(makeCtx("go", "library"));
  const keyIdx = text.indexOf("//omni:key");
  const pkgIdx = text.indexOf("package ");
  assert(keyIdx >= 0, "Should have //omni:key");
  assert(pkgIdx > keyIdx, "package should come after directives");
});

Deno.test("fill/go-library: has Pragma and Metadata vars", async () => {
  const text = await generateFileText(makeCtx("go", "library"));
  assertStringIncludes(text, "var Pragma = [][2]string{");
  assertStringIncludes(text, "var Metadata = [][2]string{");
  assertStringIncludes(text, '"I1.key"');
  assertStringIncludes(text, '"C1.version"');
});

Deno.test("fill/go-library: placeholders substituted", async () => {
  const text = await generateFileText(makeCtx("go", "library"));
  assertStringIncludes(text, "B-test-generated-file");
  assertStringIncludes(text, "package generated");
  assertStringIncludes(text, "generated.go");
  // No remaining {{placeholders}} except consumers (defaults to [consumers])
  const unsubstituted = text.match(/\{\{(?!consumers)\w+\}\}/g);
  assertEquals(unsubstituted, null, `No unsubstituted placeholders: ${unsubstituted}`);
});

Deno.test("fill/go-library: CLOSING has X1 and X5 with default content", async () => {
  const text = await generateFileText(makeCtx("go", "library"));
  assertStringIncludes(text, "// X1: Policy");
  assertStringIncludes(text, "// X5: Note");
  assertStringIncludes(text, "// Never:");
  assertStringIncludes(text, "// Careful:");
  assertStringIncludes(text, "// Safe:");
  assertStringIncludes(text, "// Scripture:");
});

Deno.test("fill/go-library: CLOSING does NOT have X6 (template-only)", async () => {
  const text = await generateFileText(makeCtx("go", "library"));
  assert(!text.includes("// X6:"), "Generated file should not have X6 (template-only section)");
});

Deno.test("fill/go-library: SETUP has subsection headers", async () => {
  const text = await generateFileText(makeCtx("go", "library"));
  assertStringIncludes(text, "Imports");
});

Deno.test("fill/go-library: BODY has subsection headers", async () => {
  const text = await generateFileText(makeCtx("go", "library"));
  // Library body subsections include "Public APIs" (or similar)
  assert(
    text.includes("Public") || text.includes("Core") || text.includes("Helpers"),
    "BODY should have subsection headers from schema",
  );
});

// ---------------------------------------------------------------------------
// Rust — generation tests
// ---------------------------------------------------------------------------

Deno.test("fill/rust-library: generates valid Rust library file", async () => {
  const text = await generateFileText(makeCtx("rust", "library"));

  assertStringIncludes(text, "// METADATA");
  assertStringIncludes(text, "// END METADATA");
  assertStringIncludes(text, "// SETUP");
  assertStringIncludes(text, "// END SETUP");
  assertStringIncludes(text, "// BODY");
  assertStringIncludes(text, "// END BODY");
  assertStringIncludes(text, "// CLOSING");
  assertStringIncludes(text, "// END CLOSING");
});

Deno.test("fill/rust-library: has Rust-specific syntax", async () => {
  const text = await generateFileText(makeCtx("rust", "library"));
  assertStringIncludes(text, "pub static PRAGMA");
  assertStringIncludes(text, "pub static METADATA");
  assertStringIncludes(text, "//!");
  // No Go-style package declaration (check for line-start "package ", not "package" in comments)
  assert(!text.split("\n").some((l) => /^package\s/.test(l)), "Rust file should not have package declaration");
});

Deno.test("fill/rust-library: placeholders substituted", async () => {
  const text = await generateFileText(makeCtx("rust", "library"));
  assertStringIncludes(text, "B-test-generated-file");
  assertStringIncludes(text, "generated.rs");
  const unsubstituted = text.match(/\{\{(?!consumers)\w+\}\}/g);
  assertEquals(unsubstituted, null, `No unsubstituted placeholders: ${unsubstituted}`);
});

// ---------------------------------------------------------------------------
// THE PROOF: generate → lint → 0 errors
// ---------------------------------------------------------------------------

Deno.test("fill/go-library: generated file lints with 0 errors", async () => {
  const text = await generateFileText(makeCtx("go", "library"));
  const tmp = await Deno.makeTempFile({ suffix: ".go" });

  try {
    await Deno.writeTextFile(tmp, text);

    const go = getFormat("go");
    assert(go !== undefined, "Go format handler should be registered");
    const results = await go!.lint(tmp);

    const errors = results.filter((r) => r.severity === "error");
    assertEquals(
      errors.length, 0,
      `Generated Go library should lint with 0 errors, got ${errors.length}:\n` +
      errors.map((e) => `  ${e.rule}: ${e.message}`).join("\n"),
    );
  } finally {
    await Deno.remove(tmp);
  }
});

Deno.test("fill/rust-library: generated file lints with 0 errors", async () => {
  const text = await generateFileText(makeCtx("rust", "library"));
  const tmp = await Deno.makeTempFile({ suffix: ".rs" });

  try {
    await Deno.writeTextFile(tmp, text);

    const rust = getFormat("rust");
    assert(rust !== undefined, "Rust format handler should be registered");
    const results = await rust!.lint(tmp);

    const errors = results.filter((r) => r.severity === "error");
    assertEquals(
      errors.length, 0,
      `Generated Rust library should lint with 0 errors, got ${errors.length}:\n` +
      errors.map((e) => `  ${e.rule}: ${e.message}`).join("\n"),
    );
  } finally {
    await Deno.remove(tmp);
  }
});

Deno.test("fill/go-executable: generated executable lints with 0 errors", async () => {
  const ctx = makeCtx("go", "executable");
  ctx.packageOrCrate = "main";
  const text = await generateFileText(ctx);
  const tmp = await Deno.makeTempFile({ suffix: ".go" });

  try {
    await Deno.writeTextFile(tmp, text);

    const go = getFormat("go");
    const results = await go!.lint(tmp);

    const errors = results.filter((r) => r.severity === "error");
    assertEquals(
      errors.length, 0,
      `Generated Go executable should lint with 0 errors, got ${errors.length}:\n` +
      errors.map((e) => `  ${e.rule}: ${e.message}`).join("\n"),
    );
  } finally {
    await Deno.remove(tmp);
  }
});

Deno.test("fill/rust-executable: generated executable lints with 0 errors", async () => {
  const ctx = makeCtx("rust", "executable");
  const text = await generateFileText(ctx);
  const tmp = await Deno.makeTempFile({ suffix: ".rs" });

  try {
    await Deno.writeTextFile(tmp, text);

    const rust = getFormat("rust");
    const results = await rust!.lint(tmp);

    const errors = results.filter((r) => r.severity === "error");
    assertEquals(
      errors.length, 0,
      `Generated Rust executable should lint with 0 errors, got ${errors.length}:\n` +
      errors.map((e) => `  ${e.rule}: ${e.message}`).join("\n"),
    );
  } finally {
    await Deno.remove(tmp);
  }
});

// ---------------------------------------------------------------------------
// Form-aware generation — SETUP sections filtered by form
// ---------------------------------------------------------------------------

/** Extract subsection headers (lines matching "// N. Name") from a block. */
function extractHeaders(text: string, block: string): string[] {
  const lines = text.split("\n");
  const blockStart = lines.findIndex((l) => l.includes(`// ${block}`) && !l.includes("END"));
  const blockEnd = lines.findIndex((l) => l.includes(`// END ${block}`));
  if (blockStart < 0 || blockEnd < 0) return [];
  const blockLines = lines.slice(blockStart, blockEnd);
  return blockLines
    .filter((l) => /^\/\/\s+\d+\.\s/.test(l))
    .map((l) => l.trim());
}

Deno.test("fill/form-aware: bare-bone has fewer SETUP sections than library", async () => {
  const bareText = await generateFileText(makeCtx("go", "bare-bone"));
  const libText = await generateFileText(makeCtx("go", "library"));

  const bareHeaders = extractHeaders(bareText, "SETUP");
  const libHeaders = extractHeaders(libText, "SETUP");

  assert(bareHeaders.length > 0, "bare-bone should have at least some SETUP sections");
  assert(libHeaders.length > bareHeaders.length,
    `library (${libHeaders.length}) should have more SETUP sections than bare-bone (${bareHeaders.length})`);
});

Deno.test("fill/form-aware: bare-bone SETUP skips reserved sections", async () => {
  const text = await generateFileText(makeCtx("go", "bare-bone"));
  const headers = extractHeaders(text, "SETUP");
  const headerText = headers.join(" ");

  // bare-bone reserves: modules, statics, type-aliases, trait-defs, macros,
  // feature-gates, variables, interface-defs, type-methods, code-generation, build-tags
  assert(!headerText.includes("Modules"), "bare-bone should not have Modules (reserved)");
  assert(!headerText.includes("Macros"), "bare-bone should not have Macros (reserved)");
  assert(!headerText.includes("Feature Gates"), "bare-bone should not have Feature Gates (reserved)");

  // bare-bone requires: imports, constants, error-types, core-types
  assert(headerText.includes("Imports"), "bare-bone should have Imports (required)");
  assert(headerText.includes("Constants"), "bare-bone should have Constants (required)");
  assert(headerText.includes("Core Types"), "bare-bone should have Core Types (required)");
});

Deno.test("fill/form-aware: library has available sections that bare-bone reserves", async () => {
  const text = await generateFileText(makeCtx("go", "library"));
  const headers = extractHeaders(text, "SETUP");
  const headerText = headers.join(" ");

  // library has modules as available
  assert(headerText.includes("Modules"), "library should have Modules (available)");
  assert(headerText.includes("Macros"), "library should have Macros (available)");
});

Deno.test("fill/form-aware: different forms produce different BODY sections", async () => {
  const libText = await generateFileText(makeCtx("go", "library"));
  const execText = await generateFileText({
    ...makeCtx("go", "executable"),
    packageOrCrate: "main",
  });

  const libBody = extractHeaders(libText, "BODY");
  const execBody = extractHeaders(execText, "BODY");

  assert(libBody.length !== execBody.length || libBody.join() !== execBody.join(),
    "library and executable should have different BODY sections");
});

Deno.test("fill/form-aware: bare-bone generated file lints with 0 errors", async () => {
  const text = await generateFileText(makeCtx("go", "bare-bone"));
  const tmp = await Deno.makeTempFile({ suffix: ".go" });

  try {
    await Deno.writeTextFile(tmp, text);

    const go = getFormat("go");
    assert(go !== undefined, "Go format handler should be registered");
    const results = await go!.lint(tmp);

    const errors = results.filter((r) => r.severity === "error");
    assertEquals(
      errors.length, 0,
      `Generated Go bare-bone should lint with 0 errors, got ${errors.length}:\n` +
      errors.map((e) => `  ${e.rule}: ${e.message}`).join("\n"),
    );
  } finally {
    await Deno.remove(tmp);
  }
});

Deno.test("fill/form-aware: module generated file lints with 0 errors", async () => {
  const text = await generateFileText(makeCtx("go", "module"));
  const tmp = await Deno.makeTempFile({ suffix: ".go" });

  try {
    await Deno.writeTextFile(tmp, text);

    const go = getFormat("go");
    assert(go !== undefined, "Go format handler should be registered");
    const results = await go!.lint(tmp);

    const errors = results.filter((r) => r.severity === "error");
    assertEquals(
      errors.length, 0,
      `Generated Go module should lint with 0 errors, got ${errors.length}:\n` +
      errors.map((e) => `  ${e.rule}: ${e.message}`).join("\n"),
    );
  } finally {
    await Deno.remove(tmp);
  }
});

Deno.test("fill/form-aware: demo-test generated file lints with 0 errors", async () => {
  const text = await generateFileText(makeCtx("go", "demo-test"));
  const tmp = await Deno.makeTempFile({ suffix: ".go" });

  try {
    await Deno.writeTextFile(tmp, text);

    const go = getFormat("go");
    assert(go !== undefined, "Go format handler should be registered");
    const results = await go!.lint(tmp);

    const errors = results.filter((r) => r.severity === "error");
    assertEquals(
      errors.length, 0,
      `Generated Go demo-test should lint with 0 errors, got ${errors.length}:\n` +
      errors.map((e) => `  ${e.rule}: ${e.message}`).join("\n"),
    );
  } finally {
    await Deno.remove(tmp);
  }
});

// ---------------------------------------------------------------------------
// Rust form-aware generation
// ---------------------------------------------------------------------------

Deno.test("fill/form-aware: Rust bare-bone generated file lints with 0 errors", async () => {
  const text = await generateFileText(makeCtx("rust", "bare-bone"));
  const tmp = await Deno.makeTempFile({ suffix: ".rs" });

  try {
    await Deno.writeTextFile(tmp, text);

    const rust = getFormat("rust");
    assert(rust !== undefined, "Rust format handler should be registered");
    const results = await rust!.lint(tmp);

    const errors = results.filter((r) => r.severity === "error");
    assertEquals(
      errors.length, 0,
      `Generated Rust bare-bone should lint with 0 errors, got ${errors.length}:\n` +
      errors.map((e) => `  ${e.rule}: ${e.message}`).join("\n"),
    );
  } finally {
    await Deno.remove(tmp);
  }
});

Deno.test("fill/form-aware: Rust module generated file lints with 0 errors", async () => {
  const text = await generateFileText(makeCtx("rust", "module"));
  const tmp = await Deno.makeTempFile({ suffix: ".rs" });

  try {
    await Deno.writeTextFile(tmp, text);

    const rust = getFormat("rust");
    assert(rust !== undefined, "Rust format handler should be registered");
    const results = await rust!.lint(tmp);

    const errors = results.filter((r) => r.severity === "error");
    assertEquals(
      errors.length, 0,
      `Generated Rust module should lint with 0 errors, got ${errors.length}:\n` +
      errors.map((e) => `  ${e.rule}: ${e.message}`).join("\n"),
    );
  } finally {
    await Deno.remove(tmp);
  }
});

// ============================================================================
// CLOSING
// ============================================================================
//
// THE PROOF: If the "generated file lints with 0 errors" tests pass,
// the schema IS the complete specification. Form-aware generation extends
// this proof — every form (bare-bone, module, library, executable, demo-test)
// produces structurally valid output that passes the linter.
//
// Schema defines. Linter validates. Engine generates.
// Form constrains. Three operations × five forms = one truth.
//
// "And God saw every thing that he had made, and, behold, it was very good."
//   — Genesis 1:31
// ============================================================================
