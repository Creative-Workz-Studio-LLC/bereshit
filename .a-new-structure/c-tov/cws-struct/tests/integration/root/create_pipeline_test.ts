// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/integration/root/create_pipeline_test.ts
// key:     B-tov-cws-struct-tests-integration-root-create-pipeline
// title:   Create Pipeline Tests — Schema → Fill → Lint Round-Trip
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-24
// authors: Nova Dawn (CPI-SI)
// purpose: Integration tests for the create pipeline across all formats
//          and subtypes. Verifies: schema drives fill engine, fill engine
//          produces valid output, output lints clean. The proof:
//          schema = specification.
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

import { generateFileText } from "@lib/engine/fill.ts";
import type { FillContext } from "@lib/engine/fill.ts";
import { lintFile } from "@lib/api/mod.ts";
import { getRegisteredFormats, getFormatHandler } from "@lib/api/mod.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// 1. Fill engine basics
// ---------------------------------------------------------------------------

Deno.test("create/fill: Go library generates non-empty output", async () => {
  const ctx: FillContext = {
    format: "go",
    subtype: "library",
    key: "B-test-fill-go-lib",
    title: "Fill Go Library Test",
    purpose: "Test fill engine produces Go library",
    filename: "test_fill.go",
    from: "test/create_pipeline_test.ts",
    packageOrCrate: "testpkg",
  };

  const text = await generateFileText(ctx);
  assertGreater(text.length, 100, "Should generate substantial content");
  assert(text.includes("package testpkg"), "Should have Go package declaration");
  assert(text.includes("METADATA"), "Should have METADATA block");
  assert(text.includes("SETUP"), "Should have SETUP block");
  assert(text.includes("BODY"), "Should have BODY block");
  assert(text.includes("CLOSING"), "Should have CLOSING block");
});

Deno.test("create/fill: Rust library generates non-empty output", async () => {
  const ctx: FillContext = {
    format: "rust",
    subtype: "library",
    key: "B-test-fill-rust-lib",
    title: "Fill Rust Library Test",
    purpose: "Test fill engine produces Rust library",
    filename: "test_fill.rs",
    from: "test/create_pipeline_test.ts",
    packageOrCrate: "testcrate",
  };

  const text = await generateFileText(ctx);
  assertGreater(text.length, 100);
  assert(text.includes("METADATA"), "Should have METADATA block");
  assert(text.includes("CLOSING"), "Should have CLOSING block");
});

Deno.test("create/fill: key substitution works", async () => {
  const ctx: FillContext = {
    format: "go",
    subtype: "library",
    key: "B-specific-key-value",
    title: "Key Sub Test",
    purpose: "Verify key substitution",
    filename: "key_test.go",
    from: "test/create_pipeline_test.ts",
    packageOrCrate: "keypkg",
  };

  const text = await generateFileText(ctx);
  assert(text.includes("B-specific-key-value"), "Key should appear in output");
  assert(!text.includes("{{key}}"), "No unfilled key placeholder");
});

Deno.test("create/fill: title substitution works", async () => {
  const ctx: FillContext = {
    format: "go",
    subtype: "library",
    key: "B-title-test",
    title: "My Specific Title Here",
    purpose: "Verify title substitution",
    filename: "title_test.go",
    from: "test/create_pipeline_test.ts",
    packageOrCrate: "titlepkg",
  };

  const text = await generateFileText(ctx);
  assert(text.includes("My Specific Title Here"), "Title should appear in output");
  assert(!text.includes("{{title}}"), "No unfilled title placeholder");
});

Deno.test("create/fill: purpose substitution works", async () => {
  const ctx: FillContext = {
    format: "go",
    subtype: "library",
    key: "B-purpose-test",
    title: "Purpose Test",
    purpose: "This is a specific purpose string",
    filename: "purpose_test.go",
    from: "test/create_pipeline_test.ts",
    packageOrCrate: "purppkg",
  };

  const text = await generateFileText(ctx);
  assert(text.includes("This is a specific purpose string"), "Purpose should appear");
  assert(!text.includes("{{purpose}}"), "No unfilled purpose placeholder");
});

// ---------------------------------------------------------------------------
// 2. All Go subtypes generate valid files
// ---------------------------------------------------------------------------

const goSubtypes = ["library", "executable", "module", "bare-bone", "demo-test"];

for (const subtype of goSubtypes) {
  Deno.test(`create/go/${subtype}: generates → lints clean`, async () => {
    const tmp = await Deno.makeTempFile({ suffix: ".go" });
    try {
      const ctx: FillContext = {
        format: "go",
        subtype,
        key: `B-test-create-go-${subtype}`,
        title: `Go ${subtype} Create Test`,
        purpose: `Test Go ${subtype} creation`,
        filename: tmp,
        from: "test/create_pipeline_test.ts",
        packageOrCrate: subtype === "executable" ? "main" : "testpkg",
      };

      const text = await generateFileText(ctx);
      await Deno.writeTextFile(tmp, text);

      const report = await lintFile(tmp, { format: "go" });
      assertEquals(
        report.summary.errors,
        0,
        `Go ${subtype} should lint clean. Errors: ${report.results.filter(r => r.severity === "error").map(r => r.rule).join(", ")}`,
      );
    } finally {
      await Deno.remove(tmp);
    }
  });
}

// ---------------------------------------------------------------------------
// 3. All Rust subtypes generate valid files
// ---------------------------------------------------------------------------

const rustSubtypes = ["library", "executable", "module", "bare-bone", "demo-test"];

for (const subtype of rustSubtypes) {
  Deno.test(`create/rust/${subtype}: generates → lints clean`, async () => {
    const tmp = await Deno.makeTempFile({ suffix: ".rs" });
    try {
      const ctx: FillContext = {
        format: "rust",
        subtype,
        key: `B-test-create-rust-${subtype}`,
        title: `Rust ${subtype} Create Test`,
        purpose: `Test Rust ${subtype} creation`,
        filename: tmp,
        from: "test/create_pipeline_test.ts",
        packageOrCrate: "testcrate",
      };

      const text = await generateFileText(ctx);
      await Deno.writeTextFile(tmp, text);

      const report = await lintFile(tmp, { format: "rust" });
      assertEquals(
        report.summary.errors,
        0,
        `Rust ${subtype} should lint clean. Errors: ${report.results.filter(r => r.severity === "error").map(r => r.rule).join(", ")}`,
      );
    } finally {
      await Deno.remove(tmp);
    }
  });
}

// ---------------------------------------------------------------------------
// 4. Create → Transform round-trip
// ---------------------------------------------------------------------------

Deno.test("create/roundtrip: Go library create → transform → lint", async () => {
  const tmp = await Deno.makeTempFile({ suffix: ".go" });
  try {
    const ctx: FillContext = {
      format: "go",
      subtype: "library",
      key: "B-test-roundtrip-go",
      title: "Roundtrip Test",
      purpose: "Test create-transform-lint cycle",
      filename: tmp,
      from: "test/create_pipeline_test.ts",
      packageOrCrate: "rtpkg",
    };

    const text = await generateFileText(ctx);
    await Deno.writeTextFile(tmp, text);

    // Lint clean before transform
    const before = await lintFile(tmp, { format: "go" });
    assertEquals(before.summary.errors, 0, "Pre-transform should be clean");

    // Transform
    const handler = getFormatHandler("go");
    assertExists(handler?.transform, "Go should have transform");
    await handler.transform!(tmp, { dryRun: false, extensions: false, force: false, steps: false });

    // Lint clean after transform
    const after = await lintFile(tmp, { format: "go" });
    assertEquals(after.summary.errors, 0, "Post-transform should be clean");
  } finally {
    await Deno.remove(tmp);
  }
});

Deno.test("create/roundtrip: Rust module create → transform → lint", async () => {
  const tmp = await Deno.makeTempFile({ suffix: ".rs" });
  try {
    const ctx: FillContext = {
      format: "rust",
      subtype: "module",
      key: "B-test-roundtrip-rust",
      title: "Roundtrip Rust Test",
      purpose: "Test create-transform-lint cycle for Rust",
      filename: tmp,
      from: "test/create_pipeline_test.ts",
      packageOrCrate: "rtcrate",
    };

    const text = await generateFileText(ctx);
    await Deno.writeTextFile(tmp, text);

    const before = await lintFile(tmp, { format: "rust" });
    assertEquals(before.summary.errors, 0, "Pre-transform should be clean");

    const handler = getFormatHandler("rust");
    assertExists(handler?.transform);
    await handler.transform!(tmp, { dryRun: false, extensions: false, force: false, steps: false });

    const after = await lintFile(tmp, { format: "rust" });
    assertEquals(after.summary.errors, 0, "Post-transform should be clean");
  } finally {
    await Deno.remove(tmp);
  }
});

// ---------------------------------------------------------------------------
// 5. Format handler inventory
// ---------------------------------------------------------------------------

Deno.test("create/formats: all 8 handlers registered", () => {
  const formats = getRegisteredFormats();
  assertGreater(formats.length, 7);

  const expected = ["go", "rust", "toml", "json", "makefile", "dotfiles", "omni", "ofd"];
  for (const name of expected) {
    assert(formats.includes(name), `Missing format: ${name}`);
  }
});

Deno.test("create/formats: all handlers have lint function", () => {
  const formats = getRegisteredFormats();
  for (const name of formats) {
    const handler = getFormatHandler(name);
    assertExists(handler, `Handler for ${name} should exist`);
    assertExists(handler.lint, `Handler ${name} should have lint`);
  }
});

Deno.test("create/formats: Go and Rust have all capabilities", () => {
  for (const name of ["go", "rust"]) {
    const handler = getFormatHandler(name);
    assertExists(handler, `${name} handler`);
    assertExists(handler.lint, `${name} lint`);
    assertExists(handler.transform, `${name} transform`);
    assertExists(handler.computeHealth, `${name} computeHealth`);
    assertExists(handler.inspect, `${name} inspect`);
  }
});

Deno.test("create/formats: TOML has transform and health", () => {
  const handler = getFormatHandler("toml");
  assertExists(handler);
  assertExists(handler.lint);
  assertExists(handler.transform, "TOML should have transform");
  assertExists(handler.computeHealth, "TOML should have computeHealth");
});

// ============================================================================
// CLOSING
// ============================================================================
//
// Create pipeline integration tests — 22 tests. Fill engine validation,
// all Go subtypes (5), all Rust subtypes (5), substitution verification (3),
// create-transform round-trips (2), format handler inventory (4).
// The proof: schema IS specification. What the schema says, the output IS.
//
// "And God saw every thing that he had made, and, behold,
//  it was very good." — Genesis 1:31
// ============================================================================
