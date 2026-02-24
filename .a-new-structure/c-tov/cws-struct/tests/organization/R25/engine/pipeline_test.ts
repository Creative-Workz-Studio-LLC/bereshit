// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/T00/R25/engine/pipeline_test.ts
// key:     B-tov-cws-struct-tests-engine-pipeline
// title:   CWS Struct — Pipeline Tests (Layers + Data-Driven + Cascade Chain)
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-22
// authors: Nova Dawn (CPI-SI)
// purpose: Verify the 4-layer pipeline contract, data-driven linkage between
//          registries and checks, and cascade chain behavior at the handler level.
//
//          Complements cascade_test.ts (unit mechanics) and registries_test.ts
//          G13 (layer integrity). This file tests the PIPELINE as a system:
//          layers execute in order, data drives behavior, cascade preserves
//          root cause.
//
//          Categories:
//            P1: Layer contract — structure, ordering, resolution
//            P2: Pipeline ↔ handler consistency — checks match declarations
//            P3: Data-driven linkage — registries drive check behavior
//            P4: Cascade chain verification — handler-level cascade semantics
//            P5: Action count + layer tag preservation
//
// biblical_foundation: "For precept must be upon precept, precept upon precept;
//   line upon line, line upon line." — Isaiah 28:10
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert, assertNotEquals } from "jsr:@std/assert";

import {
  PIPELINE_LAYERS,
  LAYER_BY_NUMBER,
  getCheckLayer,
  PIPELINE_CHECK_COUNT,
} from "@lib/data/pipeline.ts";

import {
  SECTION_REGISTRY,
  SECTION_ORDER,
  FORM_REGISTRY,
  PLACEMENT_REGISTRY,
  METADATA_FORBIDDEN,
  CONCEPT_REGISTRY,
} from "@lib/data/mod.ts";

import type { AtomicAction } from "@lib/foundation/health.ts";

import {
  cascadeActions,
  cascadeActionGroups,
  cascadeBlock,
  hasStructuralFailure,
  tagLayer,
} from "@lib/engine/pipeline/cascade.ts";

// ---------------------------------------------------------------------------
// Test fixtures
// ---------------------------------------------------------------------------

/** Make a minimal AtomicAction. */
function action(
  direction: -1 | 0 | 1,
  block: string,
  check = "test-check",
  impact: "error" | "warn" | "info" = "warn",
): AtomicAction {
  return { check, container: "test", block, direction, impact };
}

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// P1: Layer contract — structure, ordering, resolution
// ---------------------------------------------------------------------------

Deno.test("[CWS-T00-081] pipeline/P1: layers are sequential 0→1→2→3", () => {
  for (let i = 0; i < PIPELINE_LAYERS.length; i++) {
    assertEquals(
      PIPELINE_LAYERS[i]!.layer,
      i,
      `Layer ${i} should have .layer = ${i}`,
    );
  }
});

Deno.test("pipeline/P1: resolution narrows R[50] → R[25] → R[10] → R[5]", () => {
  const expected = ["R50", "R25", "R10", "R05"];
  for (let i = 0; i < PIPELINE_LAYERS.length; i++) {
    assertEquals(PIPELINE_LAYERS[i]!.resolution, expected[i]);
  }
});

Deno.test("pipeline/P1: each layer has a description and question", () => {
  for (const layer of PIPELINE_LAYERS) {
    assert(layer.description.length > 0, `Layer ${layer.layer} needs description`);
    assert(layer.question.length > 0, `Layer ${layer.layer} needs question`);
    assert(layer.cascadeBehavior.length > 0, `Layer ${layer.layer} needs cascadeBehavior`);
  }
});

Deno.test("pipeline/P1: Layer 0 describes whole-file checks", () => {
  const layer0 = LAYER_BY_NUMBER[0]!;
  assert(
    layer0.description.toLowerCase().includes("whole") ||
    layer0.description.toLowerCase().includes("structural"),
    "Layer 0 should describe whole-file or structural checks",
  );
});

Deno.test("pipeline/P1: Layer 3 is terminal — no further cascade", () => {
  const layer3 = LAYER_BY_NUMBER[3]!;
  assert(
    layer3.cascadeBehavior.toLowerCase().includes("terminal") ||
    layer3.cascadeBehavior.toLowerCase().includes("no further"),
    "Layer 3 should be terminal — no further cascade",
  );
});

Deno.test("pipeline/P1: check count is at least 20", () => {
  assert(
    PIPELINE_CHECK_COUNT >= 20,
    `Pipeline should have at least 20 checks across all layers, got ${PIPELINE_CHECK_COUNT}`,
  );
});

// ---------------------------------------------------------------------------
// P2: Pipeline ↔ handler consistency — checks match layer declarations
// ---------------------------------------------------------------------------

Deno.test("pipeline/P2: Layer 0 checks include block structure and directives", () => {
  const layer0 = LAYER_BY_NUMBER[0]!;
  assert(
    layer0.checks.includes("checkDirectives"),
    "Layer 0 should include checkDirectives",
  );
  assert(
    layer0.checks.includes("checkBlockStructure"),
    "Layer 0 should include checkBlockStructure",
  );
  assert(
    layer0.checks.includes("checkSeparatorConsistency"),
    "Layer 0 should include checkSeparatorConsistency",
  );
  assert(
    layer0.checks.includes("checkTemplateVsDerived"),
    "Layer 0 should include checkTemplateVsDerived",
  );
});

Deno.test("pipeline/P2: Layer 1 checks include subsection ordering", () => {
  const layer1 = LAYER_BY_NUMBER[1]!;
  assert(
    layer1.checks.includes("checkSetupSubsectionOrder"),
    "Layer 1 should include checkSetupSubsectionOrder",
  );
  assert(
    layer1.checks.includes("checkBodySubsectionOrder"),
    "Layer 1 should include checkBodySubsectionOrder",
  );
  assert(
    layer1.checks.includes("checkClosingZoneOrder"),
    "Layer 1 should include checkClosingZoneOrder",
  );
});

Deno.test("pipeline/P2: Layer 1 checks include identity structure", () => {
  const layer1 = LAYER_BY_NUMBER[1]!;
  assert(
    layer1.checks.includes("checkPragmaMetadata"),
    "Layer 1 should include checkPragmaMetadata",
  );
  assert(
    layer1.checks.includes("checkCommentMetadata"),
    "Layer 1 should include checkCommentMetadata",
  );
});

Deno.test("pipeline/P2: Layer 2 checks include content placement", () => {
  const layer2 = LAYER_BY_NUMBER[2]!;
  assert(
    layer2.checks.includes("checkContentPlacement"),
    "Layer 2 should include checkContentPlacement",
  );
  assert(
    layer2.checks.includes("checkClosingContentPlacement"),
    "Layer 2 should include checkClosingContentPlacement",
  );
});

Deno.test("pipeline/P2: Layer 3 checks include content expectations", () => {
  const layer3 = LAYER_BY_NUMBER[3]!;
  assert(
    layer3.checks.includes("checkMetadataContentExpectations"),
    "Layer 3 should include checkMetadataContentExpectations",
  );
  assert(
    layer3.checks.includes("checkBodyContentExpectations"),
    "Layer 3 should include checkBodyContentExpectations",
  );
  assert(
    layer3.checks.includes("checkClosingContentExpectations"),
    "Layer 3 should include checkClosingContentExpectations",
  );
});

Deno.test("pipeline/P2: structural checks are ONLY in Layer 0", () => {
  const structuralChecks = [
    "checkBlockStructure",
    "checkDirectives",
    "checkSeparatorConsistency",
    "checkTemplateVsDerived",
  ];
  for (const check of structuralChecks) {
    const layer = getCheckLayer(check);
    assert(layer, `Check "${check}" should be in the pipeline`);
    assertEquals(
      layer!.layer,
      0,
      `Structural check "${check}" should be in Layer 0, found in Layer ${layer!.layer}`,
    );
  }
});

Deno.test("pipeline/P2: ordering checks are in Layer 1 (structure)", () => {
  const orderingChecks = [
    "checkSetupSubsectionOrder",
    "checkBodySubsectionOrder",
    "checkRequiredSetupSubsections",
    "checkRequiredBodySubsections",
  ];
  for (const check of orderingChecks) {
    const layer = getCheckLayer(check);
    assert(layer, `Check "${check}" should be in the pipeline`);
    assertEquals(
      layer!.layer,
      1,
      `Ordering check "${check}" should be in Layer 1, found in Layer ${layer!.layer}`,
    );
  }
});

Deno.test("pipeline/P2: content checks are in Layer 2 or 3", () => {
  const contentChecks = [
    "checkContentPlacement",
    "checkClosingContentPlacement",
    "checkBodySubtypeContent",
  ];
  for (const check of contentChecks) {
    const layer = getCheckLayer(check);
    assert(layer, `Check "${check}" should be in the pipeline`);
    assert(
      layer!.layer >= 2,
      `Content check "${check}" should be Layer 2 or 3, found in Layer ${layer!.layer}`,
    );
  }
});

// ---------------------------------------------------------------------------
// P3: Data-driven linkage — registries drive check behavior
// ---------------------------------------------------------------------------

Deno.test("pipeline/P3: section registry has entries for all 4 blocks", () => {
  const blocks = new Set<string>();
  for (const section of Object.values(SECTION_REGISTRY)) {
    blocks.add(section.block);
  }
  assert(blocks.has("metadata"), "Registry should have metadata sections");
  assert(blocks.has("setup"), "Registry should have setup sections");
  assert(blocks.has("body"), "Registry should have body sections");
  assert(blocks.has("closing"), "Registry should have closing sections");
});

Deno.test("pipeline/P3: SECTION_ORDER drives subsection ordering checks", () => {
  // SECTION_ORDER.setup is the canonical ordering for SETUP subsections.
  // checkSetupSubsectionOrder uses this via code-schema.ts → loadRules().
  // Verify the order matches schema expectations.
  const setupOrder = SECTION_ORDER.setup;
  assert(setupOrder.length >= 5, "SETUP should have at least 5 ordered sections");

  // imports must come before constants
  const importsIdx = setupOrder.indexOf("imports");
  const constantsIdx = setupOrder.indexOf("constants");
  assert(importsIdx >= 0, "imports should be in SECTION_ORDER.setup");
  assert(constantsIdx >= 0, "constants should be in SECTION_ORDER.setup");
  assert(
    importsIdx < constantsIdx,
    "imports should come before constants in SECTION_ORDER.setup",
  );
});

Deno.test("pipeline/P3: SECTION_ORDER.body has expected ordering", () => {
  const bodyOrder = SECTION_ORDER.body;
  assert(bodyOrder.length >= 5, "BODY should have at least 5 ordered sections");

  // identity-access (pos 1) comes before core-logic (pos 4)
  const identityIdx = bodyOrder.indexOf("identity-access");
  const coreIdx = bodyOrder.indexOf("core-logic");
  if (identityIdx >= 0 && coreIdx >= 0) {
    assert(
      identityIdx < coreIdx,
      "identity-access should come before core-logic in BODY ordering",
    );
  }

  // core-logic (pos 4) comes before free-functions (pos 5)
  const freeIdx = bodyOrder.indexOf("free-functions");
  if (coreIdx >= 0 && freeIdx >= 0) {
    assert(
      coreIdx < freeIdx,
      "core-logic should come before free-functions in BODY ordering",
    );
  }
});

Deno.test("pipeline/P3: placement registry maps constructs to blocks", () => {
  // Go placement: all import_decl → setup, func_decl → body
  const goRules = PLACEMENT_REGISTRY["go"]!;
  assert(goRules, "Go placement rules should exist");

  const importRule = goRules.find((r) => r.construct === "import_decl");
  assert(importRule, "Go should have import_decl rule");
  assertEquals(importRule!.targetBlock, "setup", "imports belong in setup");

  const funcRule = goRules.find((r) => r.construct === "func_decl");
  assert(funcRule, "Go should have func_decl rule");
  assertEquals(funcRule!.targetBlock, "body", "func_decl belongs in body");
});

Deno.test("pipeline/P3: placement registry maps Rust constructs correctly", () => {
  const rustRules = PLACEMENT_REGISTRY["rust"]!;
  assert(rustRules, "Rust placement rules should exist");

  const useRule = rustRules.find((r) => r.construct === "use_decl");
  assert(useRule, "Rust should have use_decl rule");
  assertEquals(useRule!.targetBlock, "setup", "use_decl belongs in setup");

  const implRule = rustRules.find((r) => r.construct === "impl_block");
  assert(implRule, "Rust should have impl_block rule");
  assertEquals(implRule!.targetBlock, "body", "impl_block belongs in body");
});

Deno.test("pipeline/P3: metadata forbidden lists prevent code in METADATA", () => {
  const goForbidden = METADATA_FORBIDDEN["go"]!;
  assert(goForbidden, "Go metadata forbidden should exist");
  assert(goForbidden.includes("import_decl"), "imports forbidden in Go METADATA");
  assert(goForbidden.includes("func_decl"), "func_decl forbidden in Go METADATA");

  const rustForbidden = METADATA_FORBIDDEN["rust"]!;
  assert(rustForbidden, "Rust metadata forbidden should exist");
  assert(rustForbidden.includes("use_decl"), "use_decl forbidden in Rust METADATA");
  assert(rustForbidden.includes("fn_decl"), "fn_decl forbidden in Rust METADATA");
});

Deno.test("pipeline/P3: form definitions drive section requirements", () => {
  const barebone = FORM_REGISTRY["bare-bone"]!;
  assert(barebone, "bare-bone form should exist");

  // bare-bone should have mostly required/reserved, not available
  const setupSections = barebone.sections.setup;
  assert(setupSections, "bare-bone should define setup sections");

  // imports should be required in bare-bone
  assertEquals(
    setupSections["imports"],
    "required",
    "imports should be required in bare-bone",
  );
});

Deno.test("pipeline/P3: library form has more available sections than bare-bone", () => {
  const barebone = FORM_REGISTRY["bare-bone"]!;
  const library = FORM_REGISTRY["library"]!;

  // Count available sections per form
  function countAvailable(form: typeof barebone): number {
    let count = 0;
    for (const blockSections of Object.values(form.sections)) {
      for (const status of Object.values(blockSections)) {
        if (status === "available") count++;
      }
    }
    return count;
  }

  const bbAvailable = countAvailable(barebone);
  const libAvailable = countAvailable(library);

  assert(
    libAvailable >= bbAvailable,
    `library (${libAvailable} available) should have at least as many ` +
    `available sections as bare-bone (${bbAvailable})`,
  );
});

Deno.test("pipeline/P3: concept maps drive expected patterns per container", () => {
  // METADATA sections should have most concepts denied (no code in METADATA)
  const directives = SECTION_REGISTRY["directives"]!;
  assert(directives, "directives section should exist");
  assertEquals(
    directives.conceptMap.function_call,
    "denied",
    "function_call should be denied in directives section",
  );
  assertEquals(
    directives.conceptMap.variable_binding,
    "denied",
    "variable_binding should be denied in directives section",
  );

  // core-logic in BODY should have most concepts granted
  const coreLogic = SECTION_REGISTRY["core-logic"];
  if (coreLogic) {
    assertEquals(
      coreLogic.conceptMap.function_call,
      "granted",
      "function_call should be granted in core-logic section",
    );
    assertEquals(
      coreLogic.conceptMap.conditional_if,
      "granted",
      "conditional_if should be granted in core-logic section",
    );
  }
});

Deno.test("pipeline/P3: concept registry is transpose of section registry", () => {
  // For any concept C and section S:
  // SECTION_REGISTRY[S].conceptMap[C] === CONCEPT_REGISTRY[C].containerMap[S]
  const sectionKeys = Object.keys(SECTION_REGISTRY);
  const conceptKeys = Object.keys(CONCEPT_REGISTRY);

  // Spot-check a few pairs
  for (const sKey of sectionKeys.slice(0, 5)) {
    const section = SECTION_REGISTRY[sKey]!;
    for (const cKey of conceptKeys.slice(0, 3)) {
      const sectionValue = section.conceptMap[cKey];
      const conceptValue = CONCEPT_REGISTRY[cKey]!.containerMap[sKey];
      assertEquals(
        sectionValue,
        conceptValue,
        `Transpose mismatch: section "${sKey}" × concept "${cKey}": ` +
        `section says "${sectionValue}", concept says "${conceptValue}"`,
      );
    }
  }
});

// ---------------------------------------------------------------------------
// P4: Cascade chain verification — multi-layer cascade semantics
// ---------------------------------------------------------------------------

Deno.test("pipeline/P4: Layer 0 failure cascades ALL downstream layers", () => {
  // Simulate: file has missing blocks (Layer 0 structural failure)
  const layer0Actions = [action(-1, "structural", "block/SETUP", "error")];
  const layer1Actions = [
    action(1, "setup", "identity-check"),
    action(1, "body", "ordering-check"),
  ];
  const layer2Actions = [
    action(1, "setup", "placement-check"),
    action(1, "body", "content-check"),
  ];
  const layer3Actions = [
    action(1, "setup", "pattern-check"),
    action(1, "body", "pattern-check"),
  ];

  tagLayer(layer0Actions, 0);
  tagLayer(layer1Actions, 1);
  tagLayer(layer2Actions, 2);
  tagLayer(layer3Actions, 3);

  // Cascade all downstream
  cascadeActionGroups(
    [layer1Actions, layer2Actions, layer3Actions],
    "Layer 0: blocks missing",
    0,
  );

  // All downstream should be neutral
  for (const group of [layer1Actions, layer2Actions, layer3Actions]) {
    for (const a of group) {
      assertEquals(a.direction, 0, "Downstream should be neutral");
      assertEquals(a.impact, "info", "Cascaded impact should be info");
    }
  }

  // Layer 0 misaligned should be preserved
  assertEquals(layer0Actions[0]!.direction, -1, "Layer 0 failure preserved");
});

Deno.test("pipeline/P4: Layer 1 per-block cascade — SETUP fails, BODY preserved", () => {
  // Simulate: SETUP sections disordered, BODY is fine
  const layer2Setup = [
    action(1, "setup", "setup-content"),
    action(1, "setup", "setup-identity"),
  ];
  const layer2Body = [
    action(1, "body", "body-content"),
    action(1, "body", "body-placement"),
  ];
  const layer2Closing = [
    action(1, "closing", "closing-content"),
  ];

  tagLayer(layer2Setup, 2);
  tagLayer(layer2Body, 2);
  tagLayer(layer2Closing, 2);

  // Cascade only SETUP block
  cascadeBlock(
    [...layer2Setup, ...layer2Body, ...layer2Closing],
    "setup",
    "SETUP sections disordered",
    1,
  );

  // SETUP actions should be cascaded
  assertEquals(layer2Setup[0]!.direction, 0, "setup should cascade");
  assertEquals(layer2Setup[1]!.direction, 0, "setup should cascade");

  // BODY actions should be preserved
  assertEquals(layer2Body[0]!.direction, 1, "body should NOT cascade");
  assertEquals(layer2Body[1]!.direction, 1, "body should NOT cascade");

  // CLOSING actions should be preserved
  assertEquals(layer2Closing[0]!.direction, 1, "closing should NOT cascade");
});

Deno.test("pipeline/P4: multi-block cascade — SETUP + CLOSING fail, BODY preserved", () => {
  const allActions = [
    action(1, "setup", "s1"),
    action(1, "body", "b1"),
    action(1, "closing", "c1"),
    action(1, "body", "b2"),
    action(1, "setup", "s2"),
  ];

  tagLayer(allActions, 2);

  // Cascade both SETUP and CLOSING
  cascadeBlock(allActions, "setup", "SETUP failure", 1);
  cascadeBlock(allActions, "closing", "CLOSING failure", 1);

  // Check results
  assertEquals(allActions[0]!.direction, 0, "setup s1 cascaded");
  assertEquals(allActions[1]!.direction, 1, "body b1 preserved");
  assertEquals(allActions[2]!.direction, 0, "closing c1 cascaded");
  assertEquals(allActions[3]!.direction, 1, "body b2 preserved");
  assertEquals(allActions[4]!.direction, 0, "setup s2 cascaded");
});

Deno.test("pipeline/P4: cascade does not change misaligned actions", () => {
  // A file with an actual error in body — cascade shouldn't mask it
  const actions = [
    action(-1, "body", "real-error", "error"),
    action(1, "body", "good-check"),
    action(1, "setup", "setup-check"),
  ];

  tagLayer(actions, 2);

  // Cascade body block (simulating Layer 1 body failure)
  cascadeBlock(actions, "body", "BODY sections failed", 1);

  // Misaligned preserved, aligned cascaded
  assertEquals(actions[0]!.direction, -1, "Real error preserved");
  assertEquals(actions[1]!.direction, 0, "Aligned body cascaded");
  assertEquals(actions[2]!.direction, 1, "Setup untouched by body cascade");
});

Deno.test("pipeline/P4: sub-cascade chain — identity empty → field checks neutral", () => {
  // Simulate the sub-cascade from go.ts: Pragma empty → field checks neutral
  const identityActions = [
    action(-1, "metadata", "identity/Pragma/empty", "warn"),   // the root cause
    action(1, "metadata", "identity/Pragma/I1.key"),            // would pass...
    action(1, "metadata", "identity/Pragma/I1.format"),         // would pass...
    action(1, "metadata", "identity/Metadata/C1.version"),      // different var — preserved
  ];

  // Sub-cascade: empty Pragma → I-section field checks neutral
  for (const a of identityActions) {
    if (a.direction === 1 && a.check.startsWith("identity/Pragma/I")) {
      (a as { direction: -1 | 0 | 1 }).direction = 0;
      a.impact = "info";
      a.reason = "Pragma var empty — field check cannot run";
    }
  }

  assertEquals(identityActions[0]!.direction, -1, "Root cause preserved");
  assertEquals(identityActions[1]!.direction, 0, "Pragma I field cascaded");
  assertEquals(identityActions[2]!.direction, 0, "Pragma I field cascaded");
  assertEquals(identityActions[3]!.direction, 1, "Metadata C field preserved");
});

// ---------------------------------------------------------------------------
// P5: Action count + layer tag preservation
// ---------------------------------------------------------------------------

Deno.test("pipeline/P5: cascade preserves total action count", () => {
  const actions = [
    action(1, "setup"),
    action(1, "body"),
    action(-1, "closing"),
    action(0, "metadata"),
  ];
  const originalCount = actions.length;

  cascadeActions(actions, "test cascade", 0);

  assertEquals(
    actions.length,
    originalCount,
    "Cascade should not add or remove actions",
  );
});

Deno.test("pipeline/P5: cascade preserves total across multiple groups", () => {
  const group1 = [action(1, "setup"), action(-1, "setup")];
  const group2 = [action(1, "body")];
  const group3 = [action(1, "closing"), action(1, "closing"), action(0, "closing")];
  const totalBefore = group1.length + group2.length + group3.length;

  cascadeActionGroups([group1, group2, group3], "test", 0);

  const totalAfter = group1.length + group2.length + group3.length;
  assertEquals(totalAfter, totalBefore, "Total action count should be preserved");
});

Deno.test("pipeline/P5: tagLayer sets layer on all actions", () => {
  const actions = [
    action(1, "setup"),
    action(-1, "body"),
    action(0, "closing"),
  ];

  // Tag all as Layer 2
  tagLayer(actions, 2);

  for (const a of actions) {
    assertEquals(a.layer, 2, "All actions should have layer 2");
  }
});

Deno.test("pipeline/P5: cascade on layer-tagged actions preserves source layer info", () => {
  const actions = [
    action(1, "setup"),
    action(1, "body"),
  ];

  // Tag as Layer 2
  tagLayer(actions, 2);

  // Cascade from Layer 0 failure
  cascadeActions(actions, "Layer 0 failure", 0);

  // After cascade, layer should be updated to 0 (source of cascade)
  for (const a of actions) {
    assertEquals(a.layer, 0, "Cascade source layer should overwrite original tag");
    assertEquals(a.direction, 0, "Should be neutral");
  }
});

Deno.test("pipeline/P5: cascaded actions have reason set", () => {
  const actions = [
    action(1, "setup"),
    action(1, "body"),
  ];

  cascadeActions(actions, "METADATA block missing", 0);

  for (const a of actions) {
    assertEquals(a.reason, "METADATA block missing", "Reason should be set");
  }
});

Deno.test("pipeline/P5: hasStructuralFailure detects block-level errors", () => {
  // Test with rule patterns matching how Go handler emits them
  const results = [
    { file: "test.go", severity: "error" as const, rule: "block/SETUP", message: "missing" },
    { file: "test.go", severity: "warn" as const, rule: "identity/Pragma/I1.key", message: "bad" },
  ];

  assert(
    hasStructuralFailure(results, ["block/METADATA", "block/SETUP", "block/BODY", "block/CLOSING"]),
    "Should detect block/SETUP as structural failure",
  );
});

Deno.test("pipeline/P5: hasStructuralFailure ignores non-block errors", () => {
  const results = [
    { file: "test.go", severity: "error" as const, rule: "identity/Pragma/I1.key", message: "bad" },
    { file: "test.go", severity: "error" as const, rule: "content/metadata-leak", message: "leak" },
  ];

  assertEquals(
    hasStructuralFailure(results, ["block/METADATA", "block/SETUP", "block/BODY", "block/CLOSING"]),
    false,
    "Identity and content errors are NOT structural failures",
  );
});

// ---------------------------------------------------------------------------
// P6: Layer dependency chain — each layer validates next layer's assumptions
// ---------------------------------------------------------------------------

Deno.test("pipeline/P6: Layer 0 checks are prerequisites for Layer 1", () => {
  // Layer 0 checks structural existence (blocks present).
  // Layer 1 checks section ordering WITHIN blocks.
  // Can't check section order without blocks — dependency is real.
  const layer0 = LAYER_BY_NUMBER[0]!;
  const layer1 = LAYER_BY_NUMBER[1]!;

  assert(
    layer0.checks.includes("checkBlockStructure"),
    "Layer 0 must check block structure (prerequisite for Layer 1)",
  );
  assert(
    layer1.checks.includes("checkSetupSubsectionOrder"),
    "Layer 1 checks subsection order (depends on blocks existing)",
  );
});

Deno.test("pipeline/P6: Layer 1 checks are prerequisites for Layer 2", () => {
  // Layer 1 checks section presence/ordering.
  // Layer 2 checks content placement within sections.
  // Can't check placement without sections.
  const layer1 = LAYER_BY_NUMBER[1]!;
  const layer2 = LAYER_BY_NUMBER[2]!;

  assert(
    layer1.checks.includes("checkRequiredSetupSubsections"),
    "Layer 1 must verify required sections (prerequisite for Layer 2)",
  );
  assert(
    layer2.checks.includes("checkContentPlacement"),
    "Layer 2 checks content placement (depends on sections existing)",
  );
});

Deno.test("pipeline/P6: Layer 2 checks are prerequisites for Layer 3", () => {
  // Layer 2 checks content is in the right place.
  // Layer 3 checks content patterns match schema expectations.
  // Can't validate patterns if content is misplaced.
  const layer2 = LAYER_BY_NUMBER[2]!;
  const layer3 = LAYER_BY_NUMBER[3]!;

  assert(layer2.checks.length > 0, "Layer 2 should have checks");
  assert(layer3.checks.length > 0, "Layer 3 should have checks");
  assert(
    layer3.checks.includes("checkClosingZoneContent"),
    "Layer 3 checks zone content (depends on zones being correct)",
  );
});

// ---------------------------------------------------------------------------
// P7: Full cascade chain simulation — 4 layers, realistic scenario
// ---------------------------------------------------------------------------

Deno.test("pipeline/P7: complete cascade chain — all 4 layers", () => {
  // Build action groups representing all 4 layers
  const layer0 = [
    action(-1, "structural", "block/METADATA", "error"),  // METADATA missing
    action(1, "structural", "block/SETUP"),                // SETUP present
    action(1, "structural", "block/BODY"),                 // BODY present
    action(1, "structural", "block/CLOSING"),              // CLOSING present
  ];

  const layer1Metadata = [
    action(1, "metadata", "identity/Pragma/I1.key"),
    action(1, "metadata", "identity/Metadata/C1.version"),
  ];
  const layer1Setup = [
    action(1, "setup", "setup/subsection-order"),
  ];

  const layer2Metadata = [
    action(1, "metadata", "content/metadata-check/directive"),
  ];
  const layer2Setup = [
    action(1, "setup", "content/placement"),
  ];

  const layer3Metadata = [
    action(1, "metadata", "content/metadata-patterns"),
  ];
  const layer3Setup = [
    action(1, "setup", "content/setup-patterns"),
  ];

  // Tag layers
  tagLayer(layer0, 0);
  tagLayer(layer1Metadata, 1);
  tagLayer(layer1Setup, 1);
  tagLayer(layer2Metadata, 2);
  tagLayer(layer2Setup, 2);
  tagLayer(layer3Metadata, 3);
  tagLayer(layer3Setup, 3);

  // Cascade: METADATA missing → all metadata-related actions neutral
  // But SETUP actions are preserved
  cascadeActionGroups(
    [layer1Metadata, layer2Metadata, layer3Metadata],
    "METADATA block missing",
    0,
  );

  // Verify metadata cascaded
  for (const a of [...layer1Metadata, ...layer2Metadata, ...layer3Metadata]) {
    assertEquals(a.direction, 0, `Metadata action ${a.check} should be neutral`);
  }

  // Verify setup preserved
  assertEquals(layer1Setup[0]!.direction, 1, "SETUP layer 1 preserved");
  assertEquals(layer2Setup[0]!.direction, 1, "SETUP layer 2 preserved");
  assertEquals(layer3Setup[0]!.direction, 1, "SETUP layer 3 preserved");

  // Layer 0 root cause preserved
  assertEquals(layer0[0]!.direction, -1, "Root cause (METADATA missing) preserved");
  assertEquals(layer0[1]!.direction, 1, "SETUP block present preserved");
});

Deno.test("pipeline/P7: neutral count tracked correctly after cascade", () => {
  const actions = [
    action(1, "setup"),
    action(1, "setup"),
    action(-1, "setup", "real-error"),
    action(0, "setup"),
  ];

  cascadeActions(actions, "cascade test", 0);

  let aligned = 0, neutral = 0, misaligned = 0;
  for (const a of actions) {
    if (a.direction === 1) aligned++;
    else if (a.direction === 0) neutral++;
    else if (a.direction === -1) misaligned++;
  }

  // 2 aligned → neutral, 1 misaligned preserved, 1 already neutral
  assertEquals(aligned, 0, "No aligned should remain after cascade");
  assertEquals(neutral, 3, "3 should be neutral (2 cascaded + 1 original)");
  assertEquals(misaligned, 1, "1 misaligned preserved");
});

// ============================================================================
// CLOSING
// ============================================================================
//
// Pipeline tests — verifying the 4-layer contract, data-driven linkage,
// and cascade chain semantics. Complements cascade_test.ts (mechanics)
// and registries_test.ts G13 (integrity).
//
// The pipeline is the spine of the linter. These tests verify the spine
// is correctly wired to the nervous system (data registries) and the
// immune system (cascade logic).
//
// "For precept must be upon precept, precept upon precept;
//  line upon line, line upon line." — Isaiah 28:10
// ============================================================================
