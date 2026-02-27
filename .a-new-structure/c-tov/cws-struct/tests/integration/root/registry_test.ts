// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/integration/root/registry_test.ts
// key:     B-tov-cws-struct-tests-integration-root-registry
// title:   Data Layer Registry Tests — Schema Integrity
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-24
// authors: Nova Dawn (CPI-SI)
// purpose: Integration tests for all data layer registries. Verifies:
//          sections, concepts, forms, placement, pipeline layers, and
//          format registry. The proof: schema IS the single source of truth.
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
  getDataLayer,
  getSectionRegistry,
  getConceptRegistry,
  getFormDefinitions,
  getRegisteredFormats,
  getFormatDetails,
  getFormatHandler,
  detectFileFormat,
  lookupErrorByCode,
  lookupErrorByRule,
  getErrorsByLayer,
} from "@lib/api/mod.ts";

import type { DataLayerSnapshot } from "@lib/api/mod.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// 1. DataLayerSnapshot completeness
// ---------------------------------------------------------------------------

Deno.test("registry/snapshot: getDataLayer returns all registries", () => {
  const dl: DataLayerSnapshot = getDataLayer();
  assertExists(dl.sections, "sections registry");
  assertExists(dl.sectionOrder, "section order");
  assertExists(dl.concepts, "concepts registry");
  assertExists(dl.conceptOrder, "concept order");
  assertExists(dl.forms, "forms registry");
  assertExists(dl.placement, "placement registry");
  assertExists(dl.errorCodes, "error codes");
  assertExists(dl.lintRules, "lint rules");
  assertExists(dl.pipelineLayers, "pipeline layers");
});

Deno.test("registry/snapshot: registries are non-empty", () => {
  const dl = getDataLayer();
  assertGreater(Object.keys(dl.sections).length, 0, "sections");
  assertGreater(Object.keys(dl.concepts).length, 0, "concepts");
  assertGreater(Object.keys(dl.forms).length, 0, "forms");
  assertGreater(Object.keys(dl.placement).length, 0, "placement");
  assertGreater(dl.errorCodes.size, 0, "error codes");
  assertGreater(dl.lintRules.size, 0, "lint rules");
  assertGreater(dl.pipelineLayers.length, 0, "pipeline layers");
});

// ---------------------------------------------------------------------------
// 2. Section registry
// ---------------------------------------------------------------------------

Deno.test("registry/sections: all sections have required fields", () => {
  const sections = getSectionRegistry();
  for (const [tag, entry] of Object.entries(sections)) {
    assert(entry.tag === tag, `${tag}: tag should match key`);
    assert(entry.block.length > 0, `${tag}: block should be non-empty`);
    assert(
      ["metadata", "setup", "body", "closing"].includes(entry.block),
      `${tag}: block '${entry.block}' should be valid`,
    );
  }
});

Deno.test("registry/sections: section order covers all 4 blocks", () => {
  const dl = getDataLayer();
  const blocks = Object.keys(dl.sectionOrder);
  assert(blocks.includes("metadata"), "Should have metadata order");
  assert(blocks.includes("setup"), "Should have setup order");
  assert(blocks.includes("body"), "Should have body order");
  assert(blocks.includes("closing"), "Should have closing order");
});

Deno.test("registry/sections: section order entries are strings", () => {
  const dl = getDataLayer();
  for (const [block, tags] of Object.entries(dl.sectionOrder)) {
    assertGreater(tags.length, 0, `${block} should have ordered sections`);
    for (const tag of tags) {
      assert(typeof tag === "string", `${block}: ${tag} should be string`);
    }
  }
});

Deno.test("registry/sections: ordered tags exist in registry", () => {
  const sections = getSectionRegistry();
  const dl = getDataLayer();
  for (const [block, tags] of Object.entries(dl.sectionOrder)) {
    for (const tag of tags) {
      assertExists(sections[tag], `${block} order references missing section: ${tag}`);
    }
  }
});

Deno.test("registry/sections: metadata has pragma-related sections", () => {
  const sections = getSectionRegistry();
  // pragma-identity and metadata-context are core metadata sections
  const metaSections = Object.values(sections).filter(s => s.block === "metadata");
  assertGreater(metaSections.length, 0, "Should have metadata sections");
});

// ---------------------------------------------------------------------------
// 3. Concept registry
// ---------------------------------------------------------------------------

Deno.test("registry/concepts: all concepts have required fields", () => {
  const concepts = getConceptRegistry();
  for (const [id, entry] of Object.entries(concepts)) {
    assert(entry.id === id, `${id}: id should match key`);
    assert(entry.dirName.length > 0, `${id}: dirName should be non-empty`);
    assert(entry.description.length > 0, `${id}: description should be non-empty`);
  }
});

Deno.test("registry/concepts: concept order matches registry keys", () => {
  const dl = getDataLayer();
  const conceptKeys = Object.keys(dl.concepts);
  // Every ordered concept should exist in registry
  for (const id of dl.conceptOrder) {
    assert(conceptKeys.includes(id), `Ordered concept '${id}' missing from registry`);
  }
  // Every registry concept should be in order
  for (const id of conceptKeys) {
    assert(dl.conceptOrder.includes(id), `Registry concept '${id}' missing from order`);
  }
});

Deno.test("registry/concepts: dirName uses kebab-case", () => {
  const concepts = getConceptRegistry();
  const kebab = /^[a-z][a-z0-9]*(-[a-z0-9]+)*$/;
  for (const [id, entry] of Object.entries(concepts)) {
    assert(kebab.test(entry.dirName), `${id}: dirName '${entry.dirName}' should be kebab-case`);
  }
});

// ---------------------------------------------------------------------------
// 4. Form definitions
// ---------------------------------------------------------------------------

Deno.test("registry/forms: all 5 forms registered", () => {
  const forms = getFormDefinitions();
  const expected = ["bare-bone", "module", "library", "executable", "demo-test"];
  for (const name of expected) {
    assertExists(forms[name], `Missing form: ${name}`);
  }
  assertEquals(Object.keys(forms).length, 5, "Should have exactly 5 forms");
});

Deno.test("registry/forms: bare-bone is foundation", () => {
  const forms = getFormDefinitions();
  const bb = forms["bare-bone"];
  assertExists(bb, "bare-bone should exist");
  assert(bb.isFoundation, "bare-bone should be foundation");
  const mod = forms["module"];
  assertExists(mod, "module should exist");
  assert(!mod.isFoundation, "module should not be foundation");
  const lib = forms["library"];
  assertExists(lib, "library should exist");
  assert(!lib.isFoundation, "library should not be foundation");
});

Deno.test("registry/forms: non-foundation forms inherit from bare-bone", () => {
  const forms = getFormDefinitions();
  for (const [name, def] of Object.entries(forms)) {
    if (!def.isFoundation) {
      assertEquals(
        def.inheritsFrom,
        "bare-bone",
        `${name} should inherit from bare-bone`,
      );
    }
  }
});

Deno.test("registry/forms: all forms have sections", () => {
  const forms = getFormDefinitions();
  for (const [name, def] of Object.entries(forms)) {
    assertExists(def.sections, `${name} should have sections`);
    assertGreater(
      Object.keys(def.sections).length,
      0,
      `${name} should have >0 sections`,
    );
  }
});

Deno.test("registry/forms: ordering types are valid", () => {
  const forms = getFormDefinitions();
  const validOrderings = ["unpositioned", "type_lifecycle", "execution_flow", "concern_coverage"];
  for (const [name, def] of Object.entries(forms)) {
    if (def.ordering) {
      assert(
        validOrderings.includes(def.ordering),
        `${name}: ordering '${def.ordering}' should be valid`,
      );
    }
  }
});

Deno.test("registry/forms: forms with ordering have orderingSequence", () => {
  const forms = getFormDefinitions();
  for (const [name, def] of Object.entries(forms)) {
    if (def.ordering && def.ordering !== "unpositioned") {
      assertExists(
        def.orderingSequence,
        `${name}: non-unpositioned form should have orderingSequence`,
      );
      assertGreater(
        def.orderingSequence!.length,
        0,
        `${name}: orderingSequence should be non-empty`,
      );
    }
  }
});

// ---------------------------------------------------------------------------
// 5. Placement registry
// ---------------------------------------------------------------------------

Deno.test("registry/placement: Go and Rust have placement rules", () => {
  const dl = getDataLayer();
  assertExists(dl.placement["go"], "Go should have placement rules");
  assertExists(dl.placement["rust"], "Rust should have placement rules");
  assertGreater(dl.placement["go"].length, 0, "Go should have >0 rules");
  assertGreater(dl.placement["rust"].length, 0, "Rust should have >0 rules");
});

Deno.test("registry/placement: placement rules have required fields", () => {
  const dl = getDataLayer();
  for (const [lang, rules] of Object.entries(dl.placement)) {
    for (const rule of rules) {
      assert(rule.construct.length > 0, `${lang}: construct should be non-empty`);
      assert(rule.targetBlock.length > 0, `${lang}: targetBlock should be non-empty`);
      assert(rule.targetContainer.length > 0, `${lang}: targetContainer should be non-empty`);
    }
  }
});

// ---------------------------------------------------------------------------
// 6. Pipeline layers
// ---------------------------------------------------------------------------

Deno.test("registry/pipeline: exactly 4 layers", () => {
  const dl = getDataLayer();
  assertEquals(dl.pipelineLayers.length, 4, "Should have 4 pipeline layers");
});

Deno.test("registry/pipeline: layers are R50, R25, R10, R05", () => {
  const dl = getDataLayer();
  const resolutions = dl.pipelineLayers.map(l => l.resolution);
  assertEquals(resolutions, ["R50", "R25", "R10", "R05"]);
});

Deno.test("registry/pipeline: layers numbered 0-3", () => {
  const dl = getDataLayer();
  for (let i = 0; i < 4; i++) {
    const layer = dl.pipelineLayers[i];
    assertExists(layer, `Layer ${i} should exist`);
    assertEquals(layer.layer, i as 0 | 1 | 2 | 3);
  }
});

Deno.test("registry/pipeline: all layers have checks", () => {
  const dl = getDataLayer();
  for (const layer of dl.pipelineLayers) {
    assertGreater(
      layer.checks.length,
      0,
      `Layer ${layer.resolution} should have checks`,
    );
  }
});

Deno.test("registry/pipeline: all layers have descriptions", () => {
  const dl = getDataLayer();
  for (const layer of dl.pipelineLayers) {
    assert(
      layer.description.length > 0,
      `Layer ${layer.resolution} should have description`,
    );
  }
});

// ---------------------------------------------------------------------------
// 7. Format registry
// ---------------------------------------------------------------------------

Deno.test("registry/formats: all 8 formats registered", () => {
  const formats = getRegisteredFormats();
  const expected = ["go", "rust", "toml", "json", "makefile", "dotfiles", "omni", "ofd"];
  for (const name of expected) {
    assert(formats.includes(name), `Missing format: ${name}`);
  }
});

Deno.test("registry/formats: format details include extensions", () => {
  const details = getFormatDetails();
  assertGreater(details.length, 0);
  for (const d of details) {
    assert(d.name.length > 0, "Each format should have a name");
    assertExists(d.extensions, `${d.name} should have extensions`);
  }
});

Deno.test("registry/formats: detectFileFormat works for Go", () => {
  assertEquals(detectFileFormat("test.go"), "go");
});

Deno.test("registry/formats: detectFileFormat works for Rust", () => {
  assertEquals(detectFileFormat("lib.rs"), "rust");
});

Deno.test("registry/formats: detectFileFormat works for TOML", () => {
  assertEquals(detectFileFormat("config.toml"), "toml");
});

Deno.test("registry/formats: detectFileFormat works for JSON", () => {
  assertEquals(detectFileFormat("data.json"), "json");
  assertEquals(detectFileFormat("config.jsonc"), "json");
});

Deno.test("registry/formats: detectFileFormat works for Makefile", () => {
  assertEquals(detectFileFormat("Makefile"), "makefile");
  assertEquals(detectFileFormat("build.mk"), "makefile");
});

Deno.test("registry/formats: detectFileFormat works for dotfiles", () => {
  assertEquals(detectFileFormat(".editorconfig"), "dotfiles");
});

Deno.test("registry/formats: detectFileFormat works for OFD", () => {
  assertEquals(detectFileFormat("project.ofd"), "ofd");
});

Deno.test("registry/formats: detectFileFormat returns undefined for unknown", () => {
  assertEquals(detectFileFormat("mystery.xyz"), undefined);
});

// ---------------------------------------------------------------------------
// 8. Cross-registry consistency
// ---------------------------------------------------------------------------

Deno.test("registry/cross: error codes and lint rules same size", () => {
  const dl = getDataLayer();
  assertEquals(dl.errorCodes.size, dl.lintRules.size, "Should have same count");
});

Deno.test("registry/cross: every error code has a lint rule", () => {
  const dl = getDataLayer();
  for (const [code] of dl.errorCodes) {
    const entry = lookupErrorByCode(code);
    assertExists(entry, `${code} should be findable`);
  }
});

Deno.test("registry/cross: getErrorsByLayer covers pipeline layers 0-3", () => {
  const layers = [0, 1, 2, 3] as const;
  let total = 0;
  for (const layer of layers) {
    const entries = getErrorsByLayer(layer);
    assertGreater(entries.length, 0, `Layer ${layer} should have errors`);
    total += entries.length;
  }
  // Pipeline layers 0-3 won't cover T00 (layer 4) codes
  // Total from pipeline should be <= total error codes
  const dl = getDataLayer();
  assert(total <= dl.errorCodes.size, "Pipeline layers should be subset of total codes");
  assertGreater(total, 0, "Pipeline layers should have codes");
});

// ---------------------------------------------------------------------------
// 9. Handler health capability (Phase 5 upgrade)
// ---------------------------------------------------------------------------

Deno.test("registry/health: all 8 handlers have computeHealth", () => {
  const expected = ["go", "rust", "toml", "json", "makefile", "dotfiles", "omni", "ofd"];
  for (const name of expected) {
    const handler = getFormatHandler(name);
    assertExists(handler, `${name} handler should exist`);
    assertExists(handler.computeHealth, `${name} should have computeHealth`);
  }
});

Deno.test("registry/health: Go handler has full capabilities", () => {
  const handler = getFormatHandler("go");
  assertExists(handler);
  assertExists(handler.lint, "Go: lint");
  assertExists(handler.computeHealth, "Go: computeHealth");
  assertExists(handler.inspect, "Go: inspect");
  assertExists(handler.transform, "Go: transform");
});

Deno.test("registry/health: lightweight handlers have lint + health", () => {
  const lightweight = ["json", "makefile", "dotfiles", "omni", "ofd"];
  for (const name of lightweight) {
    const handler = getFormatHandler(name);
    assertExists(handler, `${name} handler`);
    assertExists(handler.lint, `${name}: lint`);
    assertExists(handler.computeHealth, `${name}: computeHealth`);
  }
});

// ============================================================================
// CLOSING
// ============================================================================
//
// Registry integration tests — 38 tests covering all data layer registries:
// DataLayerSnapshot (2), sections (5), concepts (3), forms (6),
// placement (2), pipeline (5), format registry (10), cross-registry (3),
// health capability (3).
//
// "The LORD by wisdom hath founded the earth; by understanding hath he
//  established the heavens." — Proverbs 3:19
// ============================================================================
