// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/engine/fill_test.ts
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
import { generateFile, generateFileText } from "../../lib/engine/fill.ts";
import type { FillContext } from "../../lib/engine/fill.ts";
import { getFormat } from "../../lib/engine/registry.ts";

// Register format handlers (side-effect imports)
import "../../lib/handlers/go.ts";
import "../../lib/handlers/rust.ts";

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

Deno.test("fill/go-library: generates valid Go library file", async () => {
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
  // No package declaration
  assert(!text.includes("package "), "Rust file should not have package declaration");
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

// ============================================================================
// CLOSING
// ============================================================================
//
// THE PROOF: If the "generated file lints with 0 errors" tests pass,
// the schema IS the complete specification.
//
// Schema defines. Linter validates. Engine generates.
// Three operations on the same truth.
// Schema = Template = Linter = Generator.
//
// "And God saw every thing that he had made, and, behold, it was very good."
//   — Genesis 1:31
// ============================================================================
