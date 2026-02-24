// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/T00/R25/data/registries_test.ts
// key:     B-tov-cws-struct-tests-data-registries
// title:   CWS Struct — Data Layer Registry Verification (10 Gates)
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-22
// authors: Nathan Emet (CPI-SI)
// purpose: Exhaustive verification of compiled section, concept, and form
//          registries. 10 gates — every one mechanical, no judgment calls.
//          If all 10 pass, the data is structurally sound.
//
//          Gate  | Verifies
//          ------|---------
//          G1    | Shape — every section has exactly N concept_map keys
//          G2    | Keys — the N keys match the canonical concept list
//          G3    | Values — every ternary value is granted/denied/defer
//          G4    | Weight — detection_weight matches recalculated |avg|
//          G5    | Transpose — section×concept consistency
//          G6    | Form cross-ref — section.formStatus matches form.sections
//          G7    | Completeness — every section in every form definition
//          G8    | Position — contiguous 1..N per block, no gaps/duplicates
//          G9    | Block — every section claims a valid block name
//          G10   | Counts — 42 sections, N concepts, 5 forms
//
// depends: lib/data/sections.ts (Task #2)
//          lib/data/forms.ts (Task #3)
//          lib/data/concepts.ts (Phase 0.3, optional for G5)
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert } from "jsr:@std/assert";

import {
  isValidTernary,
  isValidBlock,
  isValidFormStatus,
  isValidWeight,
  validateConceptMap,
  verifyTranspose,
} from "@lib/data/types.ts";

import type {
  SectionEntry,
  ConceptEntry,
  FormDefinition,
  TernaryValue,
} from "@lib/data/types.ts";

// ---------------------------------------------------------------------------
// Registry imports — uncomment as registries are built
// ---------------------------------------------------------------------------

// Phase 0.2: Section + Form registries (Tasks #2 and #3)
import { SECTION_REGISTRY, SECTION_ORDER, computeDetectionWeight } from "@lib/data/sections.ts";
import { FORM_REGISTRY } from "@lib/data/forms.ts";

// Phase 0.3: Concept + placement registries
import { CONCEPT_REGISTRY, CONCEPT_ORDER } from "@lib/data/concepts.ts";
import { PLACEMENT_REGISTRY, METADATA_FORBIDDEN } from "@lib/data/placement.ts";

// Phase 4: Pipeline layer definitions
import {
  PIPELINE_LAYERS,
  LAYER_BY_NUMBER,
  LAYER_BY_RESOLUTION,
  getCheckLayer,
  PIPELINE_CHECK_COUNT,
} from "@lib/data/pipeline.ts";

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------

/**
 * Canonical concept IDs — derived from CONCEPT_ORDER, not hardcoded.
 * When concepts change, this adapts automatically.
 */
const CANONICAL_CONCEPTS = [...CONCEPT_ORDER] as const;

/** The 5 canonical form names. */
const CANONICAL_FORMS = [
  "bare-bone",
  "module",
  "library",
  "executable",
  "demo-test",
] as const;

/** Expected section counts per block. */
const BLOCK_COUNTS = {
  metadata: 6,
  setup: 15,
  body: 13,
  closing: 8,
} as const;

/** Total expected section count. */
const TOTAL_SECTIONS = 42;

/** Total expected concept count — derived from data, not hardcoded. */
const TOTAL_CONCEPTS = CONCEPT_ORDER.length;

/** Total expected form count. */
const TOTAL_FORMS = 5;

// ---------------------------------------------------------------------------
// Weight recalculation helper
// ---------------------------------------------------------------------------

/**
 * Recalculate detection_weight from a concept_map.
 *
 * Formula: |avg| = |sum(values) / count|
 * where granted = +1, denied = -1, defer = 0.
 *
 * Returns value in [0, 1].
 */
function recalculateWeight(conceptMap: Record<string, TernaryValue>): number {
  const entries = Object.values(conceptMap);
  if (entries.length === 0) return 0;

  let sum = 0;
  for (const value of entries) {
    if (value === "granted") sum += 1;
    else if (value === "denied") sum -= 1;
    // defer = 0, no change
  }

  return Math.abs(sum / entries.length);
}

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Infrastructure tests — constants, helpers, weight formula
// ---------------------------------------------------------------------------

Deno.test("[CWS-T00-074] data/registries: test infrastructure ready — gates defined", () => {
  // Verify constants are correct
  assertEquals(CANONICAL_CONCEPTS.length, TOTAL_CONCEPTS);
  assertEquals(CANONICAL_FORMS.length, TOTAL_FORMS);
  assertEquals(
    BLOCK_COUNTS.metadata + BLOCK_COUNTS.setup +
    BLOCK_COUNTS.body + BLOCK_COUNTS.closing,
    TOTAL_SECTIONS,
  );
});

Deno.test("data/registries: recalculateWeight — all denied = 1.00", () => {
  const map: Record<string, TernaryValue> = {};
  for (const c of CANONICAL_CONCEPTS) map[c] = "denied";
  const weight = recalculateWeight(map);
  assertEquals(weight, 1.0);
});

Deno.test("data/registries: recalculateWeight — all granted = 1.00", () => {
  const map: Record<string, TernaryValue> = {};
  for (const c of CANONICAL_CONCEPTS) map[c] = "granted";
  const weight = recalculateWeight(map);
  assertEquals(weight, 1.0);
});

Deno.test("data/registries: recalculateWeight — all defer = 0.00", () => {
  const map: Record<string, TernaryValue> = {};
  for (const c of CANONICAL_CONCEPTS) map[c] = "defer";
  const weight = recalculateWeight(map);
  assertEquals(weight, 0.0);
});

Deno.test("data/registries: recalculateWeight — constants-like (1G 1D rest-denied)", () => {
  // Constants-like: variable_binding=granted, string_format=defer, rest=denied
  const map: Record<string, TernaryValue> = {};
  for (const c of CANONICAL_CONCEPTS) map[c] = "denied";
  map["variable_binding"] = "granted";
  map["string_format"] = "defer";
  const weight = recalculateWeight(map);
  // Data-driven: sum = 1 + 0 + (n-2)*(-1) = 3-n. |avg| = (n-3)/n
  const n = CANONICAL_CONCEPTS.length;
  assertAlmostEquals(weight, (n - 3) / n, 0.01);
});

Deno.test("data/registries: recalculateWeight — GRANTED_DEFER_SELF-like (1 defer, rest granted)", () => {
  const map: Record<string, TernaryValue> = {};
  for (const c of CANONICAL_CONCEPTS) map[c] = "granted";
  map["self_access"] = "defer";
  const weight = recalculateWeight(map);
  // Data-driven: sum = (n-1)*1 + 0 = n-1. |avg| = (n-1)/n
  const n = CANONICAL_CONCEPTS.length;
  assertAlmostEquals(weight, (n - 1) / n, 0.01);
});

// ---------------------------------------------------------------------------
// assertAlmostEquals helper (not in std assert)
// ---------------------------------------------------------------------------

function assertAlmostEquals(
  actual: number,
  expected: number,
  tolerance: number,
  msg?: string,
): void {
  const diff = Math.abs(actual - expected);
  assert(
    diff <= tolerance,
    msg ?? `Expected ${actual} to be within ${tolerance} of ${expected} (diff: ${diff})`,
  );
}

Deno.test("data/registries: computeDetectionWeight matches independent recalculation", () => {
  // Verify the production function (computeDetectionWeight) agrees with
  // the test's independent recalculateWeight for every section.
  // This proves the production function isn't just "right by tautology."
  const sections = Object.values(SECTION_REGISTRY);
  for (const section of sections) {
    const production = computeDetectionWeight(section.conceptMap);
    const independent = recalculateWeight(section.conceptMap);
    assertAlmostEquals(
      production,
      independent,
      0.01,
      `Section "${section.tag}": computeDetectionWeight (${production}) != ` +
      `recalculateWeight (${independent})`,
    );
  }
});

// ============================================================================
// GATES — 10 mechanical verification gates
// ============================================================================

// ---------------------------------------------------------------------------
// G1: Shape — every section has exactly N concept_map keys
// ---------------------------------------------------------------------------

Deno.test(`data/registries/G1: every section has exactly ${TOTAL_CONCEPTS} concept_map keys`, () => {
  const sections = Object.values(SECTION_REGISTRY);
  for (const section of sections) {
    const keyCount = Object.keys(section.conceptMap).length;
    assertEquals(
      keyCount,
      TOTAL_CONCEPTS,
      `Section "${section.tag}" has ${keyCount} concept_map keys, expected ${TOTAL_CONCEPTS}`,
    );
  }
});

// ---------------------------------------------------------------------------
// G2: Keys — the N keys match the canonical concept list
// ---------------------------------------------------------------------------

Deno.test(`data/registries/G2: every section uses the canonical ${TOTAL_CONCEPTS} concept keys`, () => {
  const canonicalSet = new Set(CANONICAL_CONCEPTS);
  const sections = Object.values(SECTION_REGISTRY);
  for (const section of sections) {
    const sectionKeys = new Set(Object.keys(section.conceptMap));
    // Every canonical key present
    for (const key of canonicalSet) {
      assert(
        sectionKeys.has(key),
        `Section "${section.tag}" missing concept key "${key}"`,
      );
    }
    // No extra keys
    for (const key of sectionKeys) {
      assert(
        canonicalSet.has(key as typeof CANONICAL_CONCEPTS[number]),
        `Section "${section.tag}" has unexpected concept key "${key}"`,
      );
    }
  }
});

// ---------------------------------------------------------------------------
// G3: Values — every ternary value is granted/denied/defer
// ---------------------------------------------------------------------------

Deno.test("data/registries/G3: all concept_map values are valid ternary", () => {
  const sections = Object.values(SECTION_REGISTRY);
  for (const section of sections) {
    const invalid = validateConceptMap(section.conceptMap as Record<string, string>);
    assertEquals(
      invalid.length,
      0,
      `Section "${section.tag}" has invalid concept_map values: ${invalid.join("; ")}`,
    );
  }
});

// ---------------------------------------------------------------------------
// G4: Weight — detection_weight matches recalculated |avg|
// ---------------------------------------------------------------------------

Deno.test("data/registries/G4: detection_weight matches recalculated |avg|", () => {
  const sections = Object.values(SECTION_REGISTRY);
  for (const section of sections) {
    const expected = recalculateWeight(section.conceptMap);
    assertAlmostEquals(
      section.detectionWeight,
      expected,
      0.01,
      `Section "${section.tag}": stored weight ${section.detectionWeight} ` +
      `!= recalculated ${expected.toFixed(4)}`,
    );
    assert(
      isValidWeight(section.detectionWeight),
      `Section "${section.tag}": weight ${section.detectionWeight} out of [0, 1] range`,
    );
  }
});

// ---------------------------------------------------------------------------
// G5: Transpose — section x concept consistency
// ---------------------------------------------------------------------------

Deno.test(`data/registries/G5: transpose consistency — ${TOTAL_SECTIONS * TOTAL_CONCEPTS} ternary values match`, () => {
  const mismatches = verifyTranspose(SECTION_REGISTRY, CONCEPT_REGISTRY);
  assertEquals(
    mismatches.length,
    0,
    `Transpose mismatches found:\n${mismatches.join("\n")}`,
  );
});

// ---------------------------------------------------------------------------
// G6: Form cross-ref — section.formStatus matches form.sections
// ---------------------------------------------------------------------------

Deno.test("data/registries/G6: section formStatus matches form definitions", () => {
  const sections = Object.values(SECTION_REGISTRY);
  for (const section of sections) {
    for (const [formName, sectionStatus] of Object.entries(section.formStatus)) {
      const form = FORM_REGISTRY[formName];
      assert(form, `Section "${section.tag}" references unknown form "${formName}"`);

      // Find this section in the form's block definition
      const blockSections = form.sections[section.block];
      assert(
        blockSections,
        `Form "${formName}" has no block "${section.block}" ` +
        `(section "${section.tag}" claims this block)`,
      );

      const formStatus = blockSections[section.tag];
      assertEquals(
        sectionStatus,
        formStatus,
        `Section "${section.tag}" says formStatus["${formName}"] = "${sectionStatus}" ` +
        `but form "${formName}".sections["${section.block}"]["${section.tag}"] = "${formStatus}"`,
      );
    }
  }
});

// ---------------------------------------------------------------------------
// G7: Completeness — every section appears in every form definition
// ---------------------------------------------------------------------------

Deno.test("data/registries/G7: every section has status for all 5 forms", () => {
  const sections = Object.values(SECTION_REGISTRY);
  const formNames = new Set(CANONICAL_FORMS);
  for (const section of sections) {
    const sectionForms = new Set(Object.keys(section.formStatus));
    for (const formName of formNames) {
      assert(
        sectionForms.has(formName),
        `Section "${section.tag}" missing formStatus for "${formName}"`,
      );
    }
  }
});

Deno.test("data/registries/G7: every form accounts for all sections in its block", () => {
  const forms = Object.values(FORM_REGISTRY);
  for (const form of forms) {
    for (const [blockName, blockSections] of Object.entries(form.sections)) {
      // Get all sections that belong to this block
      const registrySections = Object.values(SECTION_REGISTRY)
        .filter((s) => s.block === blockName);
      for (const section of registrySections) {
        assert(
          section.tag in blockSections,
          `Form "${form.name}" block "${blockName}" missing section "${section.tag}"`,
        );
      }
    }
  }
});

// ---------------------------------------------------------------------------
// G8: Position — unique per block, correct count, ordered
// ---------------------------------------------------------------------------

/** Convert a position (number or string like "X1") to a sortable number. */
function positionToSortKey(pos: number | string): number {
  if (typeof pos === "number") return pos;
  // String positions (e.g., "X1"-"X5") sort after numeric positions
  return 1000 + parseInt(String(pos).replace("X", ""));
}

Deno.test("data/registries/G8: positions are unique per block, correct count", () => {
  for (const [blockName, expectedCount] of Object.entries(BLOCK_COUNTS)) {
    const blockSections = Object.values(SECTION_REGISTRY)
      .filter((s) => s.block === blockName);

    assertEquals(
      blockSections.length,
      expectedCount,
      `Block "${blockName}": expected ${expectedCount} sections, got ${blockSections.length}`,
    );

    // No duplicate positions within a block
    const positions = blockSections.map((s) => String(s.position));
    const uniquePositions = new Set(positions);
    assertEquals(
      uniquePositions.size,
      positions.length,
      `Block "${blockName}" has duplicate positions: ${positions.join(", ")}`,
    );

    // Positions are monotonically increasing when sorted
    const sorted = [...blockSections].sort(
      (a, b) => positionToSortKey(a.position) - positionToSortKey(b.position),
    );
    for (let i = 1; i < sorted.length; i++) {
      assert(
        positionToSortKey(sorted[i]!.position) > positionToSortKey(sorted[i - 1]!.position),
        `Block "${blockName}": position ${sorted[i]!.position} (${sorted[i]!.tag}) ` +
        `is not greater than ${sorted[i - 1]!.position} (${sorted[i - 1]!.tag})`,
      );
    }
  }
});

// ---------------------------------------------------------------------------
// G9: Block — every section claims a valid block name
// ---------------------------------------------------------------------------

Deno.test("data/registries/G9: every section has a valid block name", () => {
  const sections = Object.values(SECTION_REGISTRY);
  for (const section of sections) {
    assert(
      isValidBlock(section.block),
      `Section "${section.tag}" has invalid block "${section.block}"`,
    );
  }
});

// ---------------------------------------------------------------------------
// G10: Counts — 42 sections, N concepts, 5 forms
// ---------------------------------------------------------------------------

Deno.test("data/registries/G10: section count is 42", () => {
  assertEquals(
    Object.keys(SECTION_REGISTRY).length,
    TOTAL_SECTIONS,
    `Expected ${TOTAL_SECTIONS} sections, got ${Object.keys(SECTION_REGISTRY).length}`,
  );
});

Deno.test("data/registries/G10: form count is 5", () => {
  assertEquals(
    Object.keys(FORM_REGISTRY).length,
    TOTAL_FORMS,
    `Expected ${TOTAL_FORMS} forms, got ${Object.keys(FORM_REGISTRY).length}`,
  );
});

Deno.test(`data/registries/G10: concept count is ${TOTAL_CONCEPTS}`, () => {
  assertEquals(Object.keys(CONCEPT_REGISTRY).length, TOTAL_CONCEPTS);
});

Deno.test(`data/registries/G10: concept order has ${TOTAL_CONCEPTS} entries`, () => {
  assertEquals(CONCEPT_ORDER.length, TOTAL_CONCEPTS);
});

// ---------------------------------------------------------------------------
// G11 (Bonus): Placement registry — construct → container mapping integrity
// ---------------------------------------------------------------------------

Deno.test("data/registries/G11: Go placement has correct count", () => {
  assert(PLACEMENT_REGISTRY["go"], "Go placement not found");
  assertEquals(PLACEMENT_REGISTRY["go"]!.length, 7, "Expected 7 Go placement rules");
});

Deno.test("data/registries/G11: Rust placement has correct count", () => {
  assert(PLACEMENT_REGISTRY["rust"], "Rust placement not found");
  assertEquals(PLACEMENT_REGISTRY["rust"]!.length, 12, "Expected 12 Rust placement rules");
});

Deno.test("data/registries/G11: all placement targets reference valid sections", () => {
  for (const [lang, rules] of Object.entries(PLACEMENT_REGISTRY)) {
    for (const rule of rules) {
      assert(
        isValidBlock(rule.targetBlock),
        `${lang} rule "${rule.construct}" targets invalid block "${rule.targetBlock}"`,
      );
      // Target container should exist in the section registry
      assert(
        SECTION_REGISTRY[rule.targetContainer],
        `${lang} rule "${rule.construct}" targets unknown section "${rule.targetContainer}"`,
      );
      // Target container should be in the claimed block
      assertEquals(
        SECTION_REGISTRY[rule.targetContainer]!.block,
        rule.targetBlock,
        `${lang} rule "${rule.construct}" targets "${rule.targetContainer}" (block "${SECTION_REGISTRY[rule.targetContainer]!.block}") ` +
        `but claims block "${rule.targetBlock}"`,
      );
    }
  }
});

Deno.test("data/registries/G11: metadata-forbidden lists reference valid constructs", () => {
  for (const [lang, forbidden] of Object.entries(METADATA_FORBIDDEN)) {
    const rules = PLACEMENT_REGISTRY[lang];
    assert(rules, `Metadata forbidden references unknown language "${lang}"`);
    const ruleConstructs = new Set(rules.map((r) => r.construct));
    for (const kind of forbidden) {
      assert(
        ruleConstructs.has(kind),
        `${lang} metadata-forbidden "${kind}" is not a known content kind`,
      );
    }
  }
});

Deno.test("data/registries/G11: no placement constructs target metadata", () => {
  for (const [lang, rules] of Object.entries(PLACEMENT_REGISTRY)) {
    for (const rule of rules) {
      assert(
        rule.targetBlock !== "metadata",
        `${lang} rule "${rule.construct}" targets metadata block — ` +
        `METADATA should only contain comments and identity`,
      );
    }
  }
});

// ---------------------------------------------------------------------------
// G12 (Bonus): Concept entry integrity
// ---------------------------------------------------------------------------

Deno.test("data/registries/G12: every concept has 42 container entries", () => {
  const sectionCount = Object.keys(SECTION_REGISTRY).length;
  for (const [id, concept] of Object.entries(CONCEPT_REGISTRY)) {
    const containerCount = Object.keys(concept.containerMap).length;
    assertEquals(
      containerCount,
      sectionCount,
      `Concept "${id}" has ${containerCount} container entries, expected ${sectionCount}`,
    );
  }
});

Deno.test("data/registries/G12: all concept containerMap values are valid ternary", () => {
  for (const [id, concept] of Object.entries(CONCEPT_REGISTRY)) {
    for (const [container, value] of Object.entries(concept.containerMap)) {
      assert(
        isValidTernary(value),
        `Concept "${id}" containerMap["${container}"] = "${value}" is not valid ternary`,
      );
    }
  }
});

Deno.test("data/registries/G12: concept IDs use underscore convention", () => {
  const underscorePattern = /^[a-z][a-z0-9]*(_[a-z0-9]+)*$/;
  for (const id of Object.keys(CONCEPT_REGISTRY)) {
    assert(
      underscorePattern.test(id),
      `Concept ID "${id}" should be lowercase underscore-separated`,
    );
  }
});

Deno.test("data/registries/G12: concept dirNames use kebab-case", () => {
  const kebabPattern = /^[a-z][a-z0-9]*(-[a-z0-9]+)*$/;
  for (const [id, concept] of Object.entries(CONCEPT_REGISTRY)) {
    assert(
      kebabPattern.test(concept.dirName),
      `Concept "${id}" dirName "${concept.dirName}" should be lowercase kebab-case`,
    );
  }
});

Deno.test("data/registries/G12: concept container keys match section tags", () => {
  const sectionTags = new Set(Object.keys(SECTION_REGISTRY));
  for (const [id, concept] of Object.entries(CONCEPT_REGISTRY)) {
    for (const container of Object.keys(concept.containerMap)) {
      assert(
        sectionTags.has(container),
        `Concept "${id}" references unknown section "${container}"`,
      );
    }
  }
});

// ---------------------------------------------------------------------------
// Bonus: Tag convention enforcement
// ---------------------------------------------------------------------------

Deno.test("data/registries: all section tags are lowercase kebab-case", () => {
  const sections = Object.values(SECTION_REGISTRY);
  const kebabPattern = /^[a-z][a-z0-9]*(-[a-z0-9]+)*$/;
  for (const section of sections) {
    assert(
      kebabPattern.test(section.tag),
      `Section tag "${section.tag}" is not lowercase kebab-case`,
    );
  }
});

Deno.test("data/registries: section registry keys match section.tag values", () => {
  for (const [key, section] of Object.entries(SECTION_REGISTRY)) {
    assertEquals(
      key,
      section.tag,
      `Registry key "${key}" does not match section.tag "${section.tag}"`,
    );
  }
});

Deno.test("data/registries: form registry keys match form.name values", () => {
  for (const [key, form] of Object.entries(FORM_REGISTRY)) {
    assertEquals(
      key,
      form.name,
      `Registry key "${key}" does not match form.name "${form.name}"`,
    );
  }
});

// ---------------------------------------------------------------------------
// Bonus: SECTION_ORDER consistency
// ---------------------------------------------------------------------------

Deno.test("data/registries: SECTION_ORDER matches SECTION_REGISTRY", () => {
  const allOrderTags = [
    ...SECTION_ORDER.metadata,
    ...SECTION_ORDER.setup,
    ...SECTION_ORDER.body,
    ...SECTION_ORDER.closing,
  ];
  const allRegistryTags = Object.keys(SECTION_REGISTRY);

  assertEquals(
    new Set(allOrderTags).size,
    allOrderTags.length,
    "SECTION_ORDER has duplicates",
  );

  assertEquals(
    new Set(allOrderTags),
    new Set(allRegistryTags),
    "SECTION_ORDER tags don't match SECTION_REGISTRY keys",
  );

  // Verify ordering is monotonically increasing within each block
  for (const [blockName, tags] of Object.entries(SECTION_ORDER)) {
    for (let i = 1; i < tags.length; i++) {
      const prev = SECTION_REGISTRY[tags[i - 1]!]!;
      const curr = SECTION_REGISTRY[tags[i]!]!;
      assert(
        positionToSortKey(prev.position) < positionToSortKey(curr.position),
        `SECTION_ORDER.${blockName}[${i - 1}] = "${prev.tag}" (pos ${prev.position}) ` +
        `should come before [${i}] = "${curr.tag}" (pos ${curr.position})`,
      );
    }
  }
});

// ---------------------------------------------------------------------------
// Bonus: formStatus values are all valid
// ---------------------------------------------------------------------------

Deno.test("data/registries: all formStatus values are valid", () => {
  const sections = Object.values(SECTION_REGISTRY);
  for (const section of sections) {
    for (const [formName, status] of Object.entries(section.formStatus)) {
      assert(
        isValidFormStatus(status),
        `Section "${section.tag}" formStatus["${formName}"] = "${status}" is invalid`,
      );
    }
  }
});

// ---------------------------------------------------------------------------
// G13 (Bonus): Pipeline layer integrity
// ---------------------------------------------------------------------------

Deno.test("data/registries/G13: pipeline has exactly 4 layers", () => {
  assertEquals(PIPELINE_LAYERS.length, 4, "Expected exactly 4 pipeline layers");
});

Deno.test("data/registries/G13: layers are numbered 0-3 in order", () => {
  for (let i = 0; i < PIPELINE_LAYERS.length; i++) {
    assertEquals(
      PIPELINE_LAYERS[i]!.layer,
      i,
      `Layer at index ${i} should be layer ${i}, got ${PIPELINE_LAYERS[i]!.layer}`,
    );
  }
});

Deno.test("data/registries/G13: resolution labels are correct", () => {
  const expectedResolutions = ["R50", "R25", "R10", "R05"];
  for (let i = 0; i < PIPELINE_LAYERS.length; i++) {
    assertEquals(
      PIPELINE_LAYERS[i]!.resolution,
      expectedResolutions[i],
      `Layer ${i} should have resolution "${expectedResolutions[i]}", got "${PIPELINE_LAYERS[i]!.resolution}"`,
    );
  }
});

Deno.test("data/registries/G13: no duplicate checks across layers", () => {
  const allChecks = new Set<string>();
  for (const layer of PIPELINE_LAYERS) {
    for (const check of layer.checks) {
      assert(
        !allChecks.has(check),
        `Check "${check}" appears in multiple layers (duplicate found in layer ${layer.layer})`,
      );
      allChecks.add(check);
    }
  }
});

Deno.test("data/registries/G13: every layer has at least one check", () => {
  for (const layer of PIPELINE_LAYERS) {
    assert(
      layer.checks.length > 0,
      `Layer ${layer.layer} (${layer.resolution}) has no checks`,
    );
  }
});

Deno.test("data/registries/G13: LAYER_BY_NUMBER lookup works", () => {
  for (let i = 0; i < 4; i++) {
    assert(LAYER_BY_NUMBER[i], `Layer ${i} not found in LAYER_BY_NUMBER`);
    assertEquals(LAYER_BY_NUMBER[i]!.layer, i);
  }
});

Deno.test("data/registries/G13: LAYER_BY_RESOLUTION lookup works", () => {
  for (const res of ["R50", "R25", "R10", "R05"]) {
    assert(LAYER_BY_RESOLUTION[res], `Resolution "${res}" not found`);
    assertEquals(LAYER_BY_RESOLUTION[res]!.resolution, res);
  }
});

Deno.test("data/registries/G13: getCheckLayer maps all checks correctly", () => {
  for (const layer of PIPELINE_LAYERS) {
    for (const check of layer.checks) {
      const found = getCheckLayer(check);
      assert(found, `getCheckLayer("${check}") returned undefined`);
      assertEquals(
        found!.layer,
        layer.layer,
        `getCheckLayer("${check}") returned layer ${found!.layer}, expected ${layer.layer}`,
      );
    }
  }
});

Deno.test("data/registries/G13: PIPELINE_CHECK_COUNT matches actual count", () => {
  const actualCount = PIPELINE_LAYERS.reduce((sum, l) => sum + l.checks.length, 0);
  assertEquals(
    PIPELINE_CHECK_COUNT,
    actualCount,
    `PIPELINE_CHECK_COUNT (${PIPELINE_CHECK_COUNT}) doesn't match actual (${actualCount})`,
  );
});

// ============================================================================
// CLOSING
// ============================================================================
//
// Registry verification gates — 10 mechanical checks that together prove
// the data layer is structurally sound. No judgment calls, no semantic
// review — that's Micah's domain. These gates verify the data matches
// what was declared, consistently, completely, correctly.
//
// Green on all gates = the registries are trustworthy.
//
// "Beloved, believe not every spirit, but try the spirits whether they
//  are of God." — 1 John 4:1
// ============================================================================
