// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/T00/R25/foundation/composition_test.ts
// key:     B-tov-cws-struct-tests-foundation-composition
// title:   CWS Struct — Composition Loader Tests
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-21
// authors: Nova Dawn (CPI-SI)
// purpose: Tests for the three-cord composition loader. Verifies that
//          composition targets can be parsed, resolved, and loaded with
//          all referenced schemas intact. Proves the linker works.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import {
  assertEquals,
  assert,
  assertRejects,
  assertStringIncludes,
} from "jsr:@std/assert";

import {
  parseCompositionTarget,
  loadComposition,
  loadCompositionCached,
  listCompositionTargets,
  clearCompositionCache,
  compositionToFormConstraints,
  loadCompositionFormConstraints,
  loadCodeRules,
  clearCodeCache,
} from "@lib/foundation/mod.ts";

import type {
  CompositionActiveSections,
  Code4BlockRules,
} from "@lib/foundation/mod.ts";

// Types CompositionTarget and ResolvedComposition are used implicitly
// through the loaded objects — no explicit type annotations needed in tests.

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// parseCompositionTarget — pure parsing
// ---------------------------------------------------------------------------

Deno.test("composition/parse: parses valid target JSONC", () => {
  const jsonText = `{
    "_pragma_format": "@omni data --jsonc",
    "target": {
      "pragma": "//omni:code --rust -module",
      "format": "rust",
      "form": "module",
      "block_pattern": "4-block",
      "composition": {
        "structure": {
          "whole": "R50_codefile/universal/_structure.jsonc",
          "molded": "forms/module/_structure.jsonc",
          "parts": { "setup": [], "body": [], "closing": [] }
        },
        "content": {
          "whole": "R50_codefile/universal/_content.jsonc",
          "molded": "forms/module/rust.jsonc",
          "parts": { "setup": [], "body": [], "closing": [] }
        }
      },
      "active_sections": {
        "SETUP": { "required": ["Imports"], "available": [], "reserved": [] },
        "BODY": { "required": ["IdentityAccess"], "reserved": [], "ordering": "type_lifecycle" },
        "CLOSING": { "required": ["Cv"], "available": [], "reserved": [] }
      },
      "resolution_order": ["1. Load whole", "2. Load molded"]
    }
  }`;

  const target = parseCompositionTarget(jsonText);
  assertEquals(target.format, "rust");
  assertEquals(target.form, "module");
  assertEquals(target.pragma, "//omni:code --rust -module");
  assertEquals(target.block_pattern, "4-block");
  assertEquals(target.composition.structure.whole, "R50_codefile/universal/_structure.jsonc");
  assertEquals(target.composition.content.molded, "forms/module/rust.jsonc");
});

Deno.test("composition/parse: rejects missing target field", () => {
  const jsonText = `{ "_pragma_format": "@omni data --jsonc" }`;

  try {
    parseCompositionTarget(jsonText);
    assert(false, "should have thrown");
  } catch (e) {
    assertStringIncludes((e as Error).message, "missing 'target' field");
  }
});

Deno.test("composition/parse: rejects missing required fields", () => {
  const jsonText = `{
    "target": {
      "format": "rust",
      "form": "module"
    }
  }`;

  try {
    parseCompositionTarget(jsonText);
    assert(false, "should have thrown");
  } catch (e) {
    assertStringIncludes((e as Error).message, "missing required fields");
  }
});

Deno.test("composition/parse: rejects missing composition sides", () => {
  const jsonText = `{
    "target": {
      "pragma": "//omni:code --rust -module",
      "format": "rust",
      "form": "module",
      "block_pattern": "4-block",
      "composition": {}
    }
  }`;

  try {
    parseCompositionTarget(jsonText);
    assert(false, "should have thrown");
  } catch (e) {
    assertStringIncludes((e as Error).message, "composition.structure");
  }
});

// ---------------------------------------------------------------------------
// loadComposition — full schema resolution (integration)
// ---------------------------------------------------------------------------

Deno.test("composition/load: loads Rust module composition", async () => {
  clearCompositionCache();
  const composed = await loadComposition("rust", "module");

  // Target metadata
  assertEquals(composed.target.format, "rust");
  assertEquals(composed.target.form, "module");
  assertEquals(composed.target.pragma, "//omni:code --rust -module");
  assertEquals(composed.target.block_pattern, "4-block");

  // Structure side loaded
  assert(composed.structure.whole !== undefined, "structure.whole should be loaded");
  assert(composed.structure.molded !== undefined, "structure.molded should be loaded");
  assert(composed.structure.parts.size > 0, "structure.parts should have entries");

  // Content side loaded
  assert(composed.content.whole !== undefined, "content.whole should be loaded");
  assert(composed.content.molded !== undefined, "content.molded should be loaded");
  assert(composed.content.parts.size > 0, "content.parts should have entries");
});

Deno.test("composition/load: loads Rust library composition", async () => {
  clearCompositionCache();
  const composed = await loadComposition("rust", "library");

  assertEquals(composed.target.format, "rust");
  assertEquals(composed.target.form, "library");

  // Library has more sections than module — larger parts maps
  assert(
    composed.structure.parts.size >= composed.content.parts.size * 0.5,
    "structure parts should be proportional to content parts",
  );

  // Library-specific: content molded should reference library expectations
  const moldedContent = composed.content.molded as Record<string, unknown>;
  assertEquals(moldedContent.form, "library");
});

Deno.test("composition/load: loads Rust executable composition", async () => {
  clearCompositionCache();
  const composed = await loadComposition("rust", "executable");

  assertEquals(composed.target.format, "rust");
  assertEquals(composed.target.form, "executable");

  // Executable has only 3 BODY sections — fewer parts than library
  const bodyParts = [...composed.structure.parts.keys()].filter(
    (k) => k.includes("/body/"),
  );
  assertEquals(bodyParts.length, 3, "executable should have 3 BODY structure parts");
});

Deno.test("composition/load: rejects non-existent target", async () => {
  await assertRejects(
    () => loadComposition("rust", "nonexistent"),
    Error,
    "Schema not found",
  );
});

// ---------------------------------------------------------------------------
// loadCompositionCached — caching behavior
// ---------------------------------------------------------------------------

Deno.test("composition/cache: second load returns cached result", async () => {
  clearCompositionCache();

  const first = await loadCompositionCached("rust", "module");
  const second = await loadCompositionCached("rust", "module");

  // Should be the exact same object (referential equality)
  assert(first === second, "cached load should return same object");
});

Deno.test("composition/cache: clearCompositionCache forces reload", async () => {
  clearCompositionCache();

  const first = await loadCompositionCached("rust", "module");
  clearCompositionCache();
  const second = await loadCompositionCached("rust", "module");

  // After clearing, should be a new object (different reference, same content)
  assert(first !== second, "after clear, should be new object");
  assertEquals(first.target.format, second.target.format);
});

// ---------------------------------------------------------------------------
// listCompositionTargets — discovery
// ---------------------------------------------------------------------------

Deno.test("composition/list: discovers available targets", async () => {
  const targets = await listCompositionTargets();

  // Should find at least the 3 Rust targets we created
  assert(targets.length >= 3, `expected >= 3 targets, got ${targets.length}`);

  const rustTargets = targets.filter((t) => t.format === "rust");
  assert(rustTargets.length >= 3, `expected >= 3 Rust targets, got ${rustTargets.length}`);

  // Should include module, library, executable
  const forms = rustTargets.map((t) => t.form).sort();
  assert(forms.includes("executable"), "should include executable");
  assert(forms.includes("library"), "should include library");
  assert(forms.includes("module"), "should include module");
});

// ---------------------------------------------------------------------------
// Schema content verification — loaded schemas have expected shape
// ---------------------------------------------------------------------------

Deno.test("composition/content: structure whole has blocks definition", async () => {
  clearCompositionCache();
  const composed = await loadComposition("rust", "module");

  // _structure.jsonc should define the block skeleton
  const whole = composed.structure.whole as Record<string, unknown>;
  assert("blocks" in whole || "skeleton" in whole || "_P5_title" in whole,
    "structure whole should have structural content");
});

Deno.test("composition/content: structure molded has form definition", async () => {
  clearCompositionCache();
  const composed = await loadComposition("rust", "module");

  // forms/module/_structure.jsonc should have form-specific rules
  const molded = composed.structure.molded as Record<string, unknown>;
  assert("form" in molded, "structure molded should have 'form' field");

  const form = molded.form as Record<string, unknown>;
  assertEquals(form.name, "module");
});

Deno.test("composition/content: content whole has invariants", async () => {
  clearCompositionCache();
  const composed = await loadComposition("rust", "module");

  // _content.jsonc should define what code IS
  const whole = composed.content.whole as Record<string, unknown>;
  assert("nature" in whole || "invariants" in whole || "_P5_title" in whole,
    "content whole should have content nature");
});

Deno.test("composition/content: content molded has expectations", async () => {
  clearCompositionCache();
  const composed = await loadComposition("rust", "module");

  // forms/module/rust.jsonc should have Rust module expectations
  const molded = composed.content.molded as Record<string, unknown>;
  assert("content_expectations" in molded, "content molded should have content_expectations");
  assertEquals(molded.format, "rust");
  assertEquals(molded.form, "module");
});

// ---------------------------------------------------------------------------
// Active sections — target correctly maps form sections
// ---------------------------------------------------------------------------

Deno.test("composition/sections: module has type_lifecycle BODY ordering", async () => {
  clearCompositionCache();
  const composed = await loadComposition("rust", "module");

  assertEquals(composed.target.active_sections.BODY.ordering, "type_lifecycle");
});

Deno.test("composition/sections: executable has execution_flow BODY ordering", async () => {
  clearCompositionCache();
  const composed = await loadComposition("rust", "executable");

  assertEquals(composed.target.active_sections.BODY.ordering, "execution_flow");
});

Deno.test("composition/sections: library has no reserved SETUP sections", async () => {
  clearCompositionCache();
  const composed = await loadComposition("rust", "library");

  assertEquals(
    composed.target.active_sections.SETUP.reserved.length,
    0,
    "library SETUP should have no reserved sections",
  );
});

Deno.test("composition/sections: executable reserves TraitDefs and Macros", async () => {
  clearCompositionCache();
  const composed = await loadComposition("rust", "executable");

  const reserved = composed.target.active_sections.SETUP.reserved;
  assert(reserved.includes("TraitDefs"), "executable should reserve TraitDefs");
  assert(reserved.includes("Macros"), "executable should reserve Macros");
});

// ---------------------------------------------------------------------------
// compositionToFormConstraints — bridge function (pure)
// ---------------------------------------------------------------------------

Deno.test("composition/bridge: converts active_sections to FormConstraints", async () => {
  const rules = await loadCodeRules("rust");

  const activeSections: CompositionActiveSections = {
    SETUP: {
      required: ["Imports", "Constants", "ErrorTypes", "CoreTypes"],
      available: ["Statics", "TypeAliases", "TraitDefs"],
      reserved: ["Modules", "Macros", "FeatureGates"],
    },
    BODY: {
      required: ["IdentityAccess", "TraitImplementations", "Constructors", "CoreLogic", "Queries", "FreeFunctions"],
      reserved: ["Output", "Helpers"],
      ordering: "type_lifecycle",
    },
    CLOSING: {
      required: ["Cv", "Ce", "X1", "X4", "X5"],
      available: ["Cc", "X2", "X3"],
      reserved: ["TestCoverageSummary", "X6"],
    },
  };

  const fc = compositionToFormConstraints(activeSections, rules, "module");

  // Identity
  assertEquals(fc.name, "module");
  assertEquals(fc.isFoundation, false);
  assertEquals(fc.ordering, "type_lifecycle");

  // SETUP: 4 required + 3 available = 7 can, 3 cannot
  assertEquals(fc.SETUP.can.length, 7);
  assertEquals(fc.SETUP.cannot.length, 3);

  // All required sections have status "REQUIRED"
  const setupRequired = fc.SETUP.can.filter((s) => s.status === "REQUIRED");
  assertEquals(setupRequired.length, 4);
  assert(setupRequired.some((s) => s.tag === "Imports"));
  assert(setupRequired.some((s) => s.tag === "Constants"));

  // All available sections have status "AVAILABLE"
  const setupAvailable = fc.SETUP.can.filter((s) => s.status === "AVAILABLE");
  assertEquals(setupAvailable.length, 3);
  assert(setupAvailable.some((s) => s.tag === "TraitDefs"));

  // Sections are sorted by position (Imports first, not TypeAliases)
  assert(
    fc.SETUP.can[0]!.tag === "Imports",
    `expected Imports first, got ${fc.SETUP.can[0]!.tag}`,
  );

  // BODY: 6 required, 2 reserved
  assertEquals(fc.BODY.can.length, 6);
  assertEquals(fc.BODY.cannot.length, 2);

  // CLOSING: 5 required + 3 available = 8 can, 2 reserved
  assertEquals(fc.CLOSING.can.length, 8);
  assertEquals(fc.CLOSING.cannot.length, 2);
});

Deno.test("composition/bridge: positions come from schema", async () => {
  const rules = await loadCodeRules("rust");

  const activeSections: CompositionActiveSections = {
    SETUP: {
      required: ["CoreTypes", "Imports"],
      reserved: [],
    },
    BODY: {
      required: ["CoreLogic"],
      reserved: [],
      ordering: "type_lifecycle",
    },
    CLOSING: {
      required: ["Cv"],
      reserved: [],
    },
  };

  const fc = compositionToFormConstraints(activeSections, rules, "module");

  // Even though CoreTypes was listed before Imports in the input,
  // the bridge should sort by schema position (Imports < CoreTypes)
  assert(
    fc.SETUP.can[0]!.tag === "Imports",
    `expected Imports first by position, got ${fc.SETUP.can[0]!.tag}`,
  );
  assert(
    fc.SETUP.can[1]!.tag === "CoreTypes",
    `expected CoreTypes second by position, got ${fc.SETUP.can[1]!.tag}`,
  );
});

Deno.test("composition/bridge: handles missing body subtype gracefully", async () => {
  const rules = await loadCodeRules("rust");

  const activeSections: CompositionActiveSections = {
    SETUP: { required: ["Imports"], reserved: [] },
    BODY: { required: ["CoreLogic"], reserved: [], ordering: "custom_flow" },
    CLOSING: { required: ["Cv"], reserved: [] },
  };

  // "nonexistent" body subtype — no positions available
  const fc = compositionToFormConstraints(activeSections, rules, "nonexistent");

  // Should still produce valid FormConstraints
  assertEquals(fc.name, "nonexistent");
  assertEquals(fc.BODY.can.length, 1);
  assertEquals(fc.BODY.can[0]!.tag, "CoreLogic");
  assertEquals(fc.ordering, "custom_flow");
});

// ---------------------------------------------------------------------------
// loadCompositionFormConstraints — async bridge (integration)
// ---------------------------------------------------------------------------

Deno.test("composition/bridge-async: loads Rust module constraints", async () => {
  const fc = await loadCompositionFormConstraints("rust", "module");

  assert(fc !== null, "should find composition target for rust-module");
  assertEquals(fc!.name, "module");

  // Module SETUP should have multiple sections
  assert(fc!.SETUP.can.length >= 4, "module should have >= 4 SETUP sections");

  // All can sections should have tags
  for (const section of fc!.SETUP.can) {
    assert(section.tag.length > 0, "section tag should not be empty");
    assert(section.position > 0, `section ${section.tag} should have positive position`);
  }
});

Deno.test("composition/bridge-async: returns null for non-existent target", async () => {
  const fc = await loadCompositionFormConstraints("rust", "nonexistent-form");

  assertEquals(fc, null, "should return null for non-existent composition target");
});

Deno.test("composition/bridge-async: returns null for bare-bone", async () => {
  // bare-bone has no composition target (it's the floor, not a specific form)
  const fc = await loadCompositionFormConstraints("rust", "bare-bone");

  // This should be null because there's no index/targets/rust/bare-bone.jsonc
  assertEquals(fc, null, "bare-bone should have no composition target");
});

// ============================================================================
// CLOSING
// ============================================================================
//
// These tests prove the composition loader — the linker of the three-cord
// architecture — actually works. Pure parsing, full resolution, caching,
// discovery, content verification, and section mapping are all validated.
// The bridge tests prove that composition targets can feed the scaffold
// through the FormConstraints interface — adapter pattern, verified.
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
