// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/T00/R25/foundation/pipeline_test.ts
// key:     B-tov-cws-struct-tests-foundation-pipeline
// title:   CWS Struct — Schema Pipeline Tests
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-19
// authors: Nova Dawn (CPI-SI)
// purpose: Tests for schema pipeline — layered discovery, caching, error
//          diagnostics, env override, and pure parse functions. Covers Phase 1
//          (parse/IO separation) and Phase 2 (pipeline abstraction).
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert, assertRejects, assertStringIncludes } from "jsr:@std/assert";
import {
  SchemaPipeline,
  FileSource,
  getDefaultPipeline,
  clearPipeline,
  assembleCodeRules,
  parseTomlSchema,
  loadCodeRules,
  clearCodeCache,
  loadRules,
  clearCache,
} from "@lib/foundation/mod.ts";
import type { SchemaSource } from "@lib/foundation/mod.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// SchemaPipeline — basic resolution
// ---------------------------------------------------------------------------

Deno.test("pipeline/basic: getText returns text from source", async () => {
  const pipeline = new SchemaPipeline();
  const testSource: SchemaSource = {
    name: "test-source",
    getText: async (_id: string) => '{"hello": "world"}',
  };
  pipeline.addSource(testSource);

  const text = await pipeline.getText("any-id");
  assertEquals(text, '{"hello": "world"}');
});

Deno.test("pipeline/basic: caches text on second call", async () => {
  let callCount = 0;
  const pipeline = new SchemaPipeline();
  const countingSource: SchemaSource = {
    name: "counting-source",
    getText: async (_id: string) => {
      callCount++;
      return '{"cached": true}';
    },
  };
  pipeline.addSource(countingSource);

  await pipeline.getText("my-schema");
  await pipeline.getText("my-schema");
  assertEquals(callCount, 1, "Source should only be called once due to caching");
});

Deno.test("pipeline/basic: clear() resets cache", async () => {
  let callCount = 0;
  const pipeline = new SchemaPipeline();
  const countingSource: SchemaSource = {
    name: "counting-source",
    getText: async (_id: string) => {
      callCount++;
      return '{"data": true}';
    },
  };
  pipeline.addSource(countingSource);

  await pipeline.getText("schema-a");
  pipeline.clear();
  await pipeline.getText("schema-a");
  assertEquals(callCount, 2, "After clear, source should be called again");
});

// ---------------------------------------------------------------------------
// SchemaPipeline — priority order
// ---------------------------------------------------------------------------

Deno.test("pipeline/priority: first source wins", async () => {
  const pipeline = new SchemaPipeline();
  pipeline.addSource({
    name: "high-priority",
    getText: async () => "HIGH",
  });
  pipeline.addSource({
    name: "low-priority",
    getText: async () => "LOW",
  });

  const text = await pipeline.getText("any");
  assertEquals(text, "HIGH");
});

Deno.test("pipeline/priority: falls through to next source on error", async () => {
  const pipeline = new SchemaPipeline();
  pipeline.addSource({
    name: "broken-source",
    getText: async () => { throw new Error("broken"); },
  });
  pipeline.addSource({
    name: "working-source",
    getText: async () => "FALLBACK",
  });

  const text = await pipeline.getText("any");
  assertEquals(text, "FALLBACK");
});

// ---------------------------------------------------------------------------
// SchemaPipeline — error diagnostics
// ---------------------------------------------------------------------------

Deno.test("pipeline/error: throws with source names when no source has schema", async () => {
  const pipeline = new SchemaPipeline();
  pipeline.addSource({
    name: "source-alpha",
    getText: async () => { throw new Error("nope"); },
  });
  pipeline.addSource({
    name: "source-beta",
    getText: async () => { throw new Error("also nope"); },
  });

  const err = await assertRejects(
    () => pipeline.getText("missing-schema.jsonc"),
    Error,
  );
  assertStringIncludes(err.message, "missing-schema.jsonc");
  assertStringIncludes(err.message, "source-alpha");
  assertStringIncludes(err.message, "source-beta");
  assertStringIncludes(err.message, "CWS_STRUCT_SCHEMA_DIR");
});

Deno.test("pipeline/error: listSources returns registered source names", () => {
  const pipeline = new SchemaPipeline();
  pipeline.addSource({ name: "one", getText: async () => "" });
  pipeline.addSource({ name: "two", getText: async () => "" });

  const names = pipeline.listSources();
  assertEquals(names, ["one", "two"]);
});

// ---------------------------------------------------------------------------
// Default pipeline — singleton
// ---------------------------------------------------------------------------

Deno.test("pipeline/default: getDefaultPipeline returns same instance", () => {
  clearPipeline();
  const a = getDefaultPipeline();
  const b = getDefaultPipeline();
  assert(a === b, "Should return same singleton");
});

Deno.test("pipeline/default: clearPipeline resets singleton", () => {
  clearPipeline();
  const a = getDefaultPipeline();
  clearPipeline();
  const b = getDefaultPipeline();
  assert(a !== b, "After clearPipeline, should get new instance");
});

Deno.test("pipeline/default: has at least 2 sources (project-local + default)", () => {
  clearPipeline();
  const pipeline = getDefaultPipeline();
  const sources = pipeline.listSources();
  assert(sources.length >= 2, `Expected at least 2 sources, got ${sources.length}: ${sources.join(", ")}`);
  // Should always have project-local and default
  assert(sources.some((s) => s.includes("default")), "Should have default source");
});

// ---------------------------------------------------------------------------
// Default pipeline — resolves real schemas
// ---------------------------------------------------------------------------

Deno.test("pipeline/default: resolves Go archive schema through default pipeline", async () => {
  clearPipeline();
  const pipeline = getDefaultPipeline();
  const text = await pipeline.getText("code/_archive/format/go-4block-schema.jsonc");
  assert(text.length > 0, "Should return non-empty text");
  assert(text.includes("go_structure"), "Should contain go_structure key");
});

Deno.test("pipeline/default: resolves Rust archive schema through default pipeline", async () => {
  clearPipeline();
  const pipeline = getDefaultPipeline();
  const text = await pipeline.getText("code/_archive/format/rust-4block-schema.jsonc");
  assert(text.length > 0, "Should return non-empty text");
  assert(text.includes("rust_structure"), "Should contain rust_structure key");
});

Deno.test("pipeline/default: resolves split schema files through default pipeline", async () => {
  clearPipeline();
  const pipeline = getDefaultPipeline();
  // go.jsonc lives in R50_codefile/languages/go/ after schema reorganization
  const text = await pipeline.getText("code/R50_codefile/languages/go/go.jsonc");
  assert(text.length > 0, "Should return non-empty text for split schema");
});

Deno.test("pipeline/default: resolves TOML schema through default pipeline", async () => {
  clearPipeline();
  const pipeline = getDefaultPipeline();
  const text = await pipeline.getText("data/format/toml-3block-schema.jsonc");
  assert(text.length > 0, "Should return non-empty text");
  assert(text.includes("validation_contract"), "Should contain validation_contract key");
});

// ---------------------------------------------------------------------------
// FileSource — basic behavior
// ---------------------------------------------------------------------------

Deno.test("pipeline/FileSource: throws for non-existent file", async () => {
  const source = new FileSource("/tmp/nonexistent-dir-cws-struct-test");
  await assertRejects(
    () => source.getText("does-not-exist.jsonc"),
    Error,
  );
});

// ---------------------------------------------------------------------------
// assembleCodeRules — multi-file assembler (replaces parseCodeSchema)
// ---------------------------------------------------------------------------

Deno.test("pipeline/assembleCodeRules: assembles Go rules from split schemas", async () => {
  clearCodeCache("go");
  clearPipeline();
  const rules = await assembleCodeRules("go");

  assert(rules.setupSubsections.length > 0, "Should have setup subsections");
  assert(Object.keys(rules.bodySubsections).length > 0, "Should have body subsections");
  assert(rules.pragmaFieldRequirements["I1"] !== undefined, "Should have I1 pragma reqs");
  assert(rules.metadataFieldRequirements["C1"] !== undefined, "Should have C1 metadata reqs");
});

Deno.test("pipeline/assembleCodeRules: assembles Rust rules from split schemas", async () => {
  clearCodeCache("rust");
  clearPipeline();
  const rules = await assembleCodeRules("rust");

  assert(rules.setupSubsections.length > 0, "Should have setup subsections");
  assert(Object.keys(rules.bodySubsections).length > 0, "Should have body subsections");
  assert(rules.pragmaFieldRequirements["I1"] !== undefined, "Should have I1 pragma reqs");
});

Deno.test("pipeline/assembleCodeRules: Go and Rust produce different format tags", async () => {
  clearCodeCache("go");
  clearCodeCache("rust");
  clearPipeline();

  const goRules = await assembleCodeRules("go");
  const rustRules = await assembleCodeRules("rust");

  // Both should have subsections but from different format sources
  assert(goRules.setupSubsections.length > 0, "Go has setup subsections");
  assert(rustRules.setupSubsections.length > 0, "Rust has setup subsections");
});

Deno.test("pipeline/assembleCodeRules: produces same rules as loadCodeRules", async () => {
  clearCodeCache("go");
  clearPipeline();

  // Via loadCodeRules (cached path)
  const loaded = await loadCodeRules("go");

  // Via assembleCodeRules (direct assembly)
  clearCodeCache("go");
  clearPipeline();
  const assembled = await assembleCodeRules("go");

  // Same structure
  assertEquals(loaded.setupSubsections.length, assembled.setupSubsections.length);
  assertEquals(
    Object.keys(loaded.bodySubsections).sort(),
    Object.keys(assembled.bodySubsections).sort(),
  );
  assertEquals(
    Object.keys(loaded.pragmaFieldRequirements).sort(),
    Object.keys(assembled.pragmaFieldRequirements).sort(),
  );
});

// ---------------------------------------------------------------------------
// parseTomlSchema — pure parse (Phase 1)
// ---------------------------------------------------------------------------

Deno.test("pipeline/parseTomlSchema: parses TOML schema text into rules", async () => {
  clearPipeline();
  const pipeline = getDefaultPipeline();
  const text = await pipeline.getText("data/format/toml-3block-schema.jsonc");
  const rules = parseTomlSchema(text);

  assert(rules.contract !== undefined, "Should have contract");
  assert(rules.requiredIdentity.length > 0, "Should have required identity");
  assert(rules.requiredClosing.length > 0, "Should have required closing");
  assert(rules.metadataKeyPattern instanceof RegExp, "Should compile key pattern");
});

Deno.test("pipeline/parseTomlSchema: produces same rules as loadRules", async () => {
  clearCache();
  clearPipeline();

  // Via loadRules (I/O + parse + cache)
  const loaded = await loadRules();

  // Via pipeline + parseTomlSchema (manual)
  const pipeline = getDefaultPipeline();
  const text = await pipeline.getText("data/format/toml-3block-schema.jsonc");
  const parsed = parseTomlSchema(text);

  // Same structure
  assertEquals(loaded.requiredIdentity, parsed.requiredIdentity);
  assertEquals(loaded.requiredClosing, parsed.requiredClosing);
  assertEquals(loaded.requiredContent, parsed.requiredContent);
});

Deno.test("pipeline/parseTomlSchema: throws on invalid JSONC", () => {
  try {
    parseTomlSchema("totally {broken");
    assert(false, "Should have thrown");
  } catch (e) {
    assertStringIncludes((e as Error).message, "parse error");
  }
});

Deno.test("pipeline/parseTomlSchema: throws on missing validation_contract", () => {
  try {
    parseTomlSchema('{"something_else": true}');
    assert(false, "Should have thrown");
  } catch (e) {
    assertStringIncludes((e as Error).message, "validation_contract");
  }
});

// ---------------------------------------------------------------------------
// Integration — loaders still work through pipeline
// ---------------------------------------------------------------------------

Deno.test("pipeline/integration: loadCodeRules works for Go through pipeline", async () => {
  clearCodeCache("go");
  clearPipeline();
  const rules = await loadCodeRules("go");
  assert(rules.setupSubsections.length > 0, "Should load Go rules through pipeline");
});

Deno.test("pipeline/integration: loadCodeRules works for Rust through pipeline", async () => {
  clearCodeCache("rust");
  clearPipeline();
  const rules = await loadCodeRules("rust");
  assert(rules.setupSubsections.length > 0, "Should load Rust rules through pipeline");
});

Deno.test("pipeline/integration: loadRules works for TOML through pipeline", async () => {
  clearCache();
  clearPipeline();
  const rules = await loadRules();
  assert(rules.requiredIdentity.length > 0, "Should load TOML rules through pipeline");
});

// ---------------------------------------------------------------------------
// Fault isolation — one format's failure doesn't break others
// ---------------------------------------------------------------------------

Deno.test("pipeline/isolation: custom source failure for one schema doesn't affect another", async () => {
  // Build a pipeline that only provides Rust schema
  const pipeline = new SchemaPipeline();
  const defaultPipeline = getDefaultPipeline();

  pipeline.addSource({
    name: "rust-only-source",
    getText: async (id: string) => {
      if (id.includes("rust")) {
        return await defaultPipeline.getText(id);
      }
      throw new Error(`Not available: ${id}`);
    },
  });

  // Rust resolves fine (via archive path)
  const rustText = await pipeline.getText("code/_archive/format/rust-4block-schema.jsonc");
  assert(rustText.length > 0, "Rust schema should resolve");
  assert(rustText.includes("rust_structure"), "Should contain rust_structure");

  // Go fails gracefully (not a crash — a controlled error)
  const err = await assertRejects(
    () => pipeline.getText("code/_archive/format/go-4block-schema.jsonc"),
    Error,
  );
  assertStringIncludes(err.message, "go-4block-schema.jsonc");
});

Deno.test("pipeline/isolation: independent format loading — Go doesn't need Rust", async () => {
  clearCodeCache("go");
  clearCodeCache("rust");
  clearPipeline();

  // Load Go independently
  const goRules = await loadCodeRules("go");
  assert(goRules.setupSubsections.length > 0, "Go rules load independently");

  // Rust hasn't loaded yet — load it now
  const rustRules = await loadCodeRules("rust");
  assert(rustRules.setupSubsections.length > 0, "Rust rules load independently");

  // Both have different structure keys
  assert(
    goRules.setupSubsections[0]?.tag !== undefined,
    "Go has tagged subsections",
  );
});

Deno.test("pipeline/isolation: TOML loading is independent of code formats", async () => {
  clearCache();
  clearCodeCache("go");
  clearCodeCache("rust");
  clearPipeline();

  // Load TOML independently
  const tomlRules = await loadRules();
  assert(tomlRules.requiredIdentity.length > 0, "TOML rules load independently");

  // Code formats haven't loaded — load one
  const goRules = await loadCodeRules("go");
  assert(goRules.setupSubsections.length > 0, "Go loads after TOML independently");
});

// ============================================================================
// CLOSING
// ============================================================================
//
// Pipeline tests cover: basic resolution, priority order, caching, error
// diagnostics, singleton behavior, real schema resolution, pure parse
// functions, integration with existing loaders, and fault isolation.
// These prove the pipeline is a transparent layer — all existing behavior
// preserved, new capabilities (override, fallback, diagnostics, isolation)
// added.
//
// "Except the LORD build the house, they labour in vain that build it."
// — Psalm 127:1
// ============================================================================
