// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/data/forms.ts
// key:     B-tov-cws-struct-lib-data-forms
// title:   CWS Struct — Form Registry
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-22
// updated: 2026-02-22
// authors: Nova Dawn (CPI-SI), with family input
// purpose: Hand-compiled registry of all 5 form definitions.
//          Each form defines which of the 42 sections are required,
//          available, or reserved. The section registry (sections.ts)
//          carries the per-form status on each section — this file
//          carries the per-form identity, ordering, and description.
//
//          Forms represent the derivation progression:
//          bare-bone (foundation) → module/library/executable/demo-test
//
//          bare-bone uses 2-state (active/reserved).
//          Derived forms use 3-state (required/available/reserved).
//          In the compiled registry: active = required.
//
// biblical_foundation: "And he made in the most holy house two cherubims
//   of image work, and overlaid them with gold." — 2 Chronicles 3:10
//   (Forms give shape to the structure — each a distinct expression
//    of the same foundation.)
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import type { FormDefinition } from "./types.ts";
import { SECTION_REGISTRY } from "./sections.ts";

// ---------------------------------------------------------------------------
// Form section builder — derives from SECTION_REGISTRY formStatus
// ---------------------------------------------------------------------------

type FormSectionMap = Record<string, Record<string, "required" | "available" | "reserved">>;

/**
 * Build the sections map for a form by reading each section's formStatus.
 *
 * This ensures forms.ts and sections.ts stay in perfect sync —
 * the section registry is the single source of truth for form status.
 */
function buildFormSections(formName: string): FormSectionMap {
  const result: FormSectionMap = {
    metadata: {},
    setup: {},
    body: {},
    closing: {},
  };

  for (const section of Object.values(SECTION_REGISTRY)) {
    const status = section.formStatus[formName];
    if (status) {
      result[section.block]![section.tag] = status;
    }
  }

  return result;
}

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Form definitions — 5 forms, foundation through demo-test
// ---------------------------------------------------------------------------

/**
 * bare-bone — the absolute minimum valid file.
 *
 * Foundation form. All other forms derive by PROMOTING reserved sections.
 * active_sections(form) = active_sections(bare-bone) + promoted_sections
 *
 * 2-state vocabulary: active (= required) or reserved. No "available."
 * "Boundaries before content" — badal.
 */
const BARE_BONE: FormDefinition = {
  name: "bare-bone",
  isFoundation: true,
  description: "Absolute minimum valid file — foundation all forms derive from. cp bare-bone → change pragma → promote sections → structurally complete before a single line of real code.",
  sections: buildFormSections("bare-bone"),
  // bare-bone has no ordering — sections are unpositioned
  ordering: "unpositioned",
};

/**
 * module — internal module, focused single-concern file.
 *
 * "Module is a member of the body (1 Cor 12:18). Each module has a
 *  specific function — focused, single-concern, serving the whole."
 *
 * Reserves crate-level concerns (Modules, Macros, FeatureGates).
 * Promotes TraitImplementations, Constructors, Queries to required.
 */
const MODULE: FormDefinition = {
  name: "module",
  isFoundation: false,
  inheritsFrom: "bare-bone",
  description: "Internal module — focused single-concern file within a crate/package. Reserves crate-level concerns (modules, macros, feature-gates).",
  sections: buildFormSections("module"),
  ordering: "type_lifecycle",
  orderingSequence: [
    "identity-access",
    "trait-implementations",
    "constructors",
    "core-logic",
    "queries",
    // output-display is reserved for modules
    "free-functions",
  ],
};

/**
 * library — library/package root, fullest non-test form.
 *
 * "Library is the body itself — the crate root, the package entry.
 *  It declares the public surface, owns the output layer, and
 *  orchestrates modules."
 *
 * All SETUP sections available. Output promoted to required.
 */
const LIBRARY: FormDefinition = {
  name: "library",
  isFoundation: false,
  inheritsFrom: "bare-bone",
  description: "Library/package root — exposes public API, holds crate/package identity. The fullest non-test form. All SETUP sections available.",
  sections: buildFormSections("library"),
  ordering: "type_lifecycle",
  orderingSequence: [
    "identity-access",
    "trait-implementations",
    "constructors",
    "core-logic",
    "queries",
    "output-display",
    "free-functions",
  ],
};

/**
 * executable — binary, orchestrates via main() → run() → Result.
 *
 * "Executables are thin. They parse args, call libraries, and report
 *  results. The logic lives in library crates."
 *
 * Only 3 BODY sections required: identity-access, helpers, core-logic.
 * Reserves trait-defs and macros (executables consume, don't define).
 */
const EXECUTABLE: FormDefinition = {
  name: "executable",
  isFoundation: false,
  inheritsFrom: "bare-bone",
  description: "Binary — orchestrates via main() → run() → Result pattern. Thin entry point delegating to libraries. Only 3 BODY sections required.",
  sections: buildFormSections("executable"),
  ordering: "execution_flow",
  orderingSequence: [
    "identity-access",
    "helpers",
    "core-logic",
  ],
};

/**
 * demo-test — test file, validates behavior, proves claims.
 *
 * "Tests are the accountability mechanism. Every claim in BODY is
 *  proved in Cv. Every type in SETUP is exercised."
 *
 * BODY contains TESTS, not implementations.
 * All standard BODY implementation sections are reserved.
 */
const DEMO_TEST: FormDefinition = {
  name: "demo-test",
  isFoundation: false,
  inheritsFrom: "bare-bone",
  description: "Test file — validates behavior, proves claims. BODY contains tests, not implementations. All standard BODY sections reserved; 9 test sections required.",
  sections: buildFormSections("demo-test"),
  ordering: "concern_coverage",
  orderingSequence: [
    // Tests section covers positions 9-17 with subsections:
    // ConstructorTests, BuilderTests, TransitionTests, QueryTests,
    // IdentityTests, ErrorTests, EdgeCaseTests, TableDrivenTests,
    // IntegrationTests
    "tests",
  ],
};

// ---------------------------------------------------------------------------
// Build the registry
// ---------------------------------------------------------------------------

/** All 5 form definitions keyed by form name. */
export const FORM_REGISTRY: Record<string, FormDefinition> = {
  "bare-bone": BARE_BONE,
  "module": MODULE,
  "library": LIBRARY,
  "executable": EXECUTABLE,
  "demo-test": DEMO_TEST,
};

// ============================================================================
// CLOSING
// ============================================================================
//
// Form registry — 5 hand-compiled definitions.
// bare-bone (foundation) → module, library, executable, demo-test.
//
// Form sections derived from SECTION_REGISTRY.formStatus —
// single source of truth, verified by Nathan's G6 gate.
//
// Ordering captures the structural intent:
//   bare-bone: unpositioned (foundation, no order)
//   module/library: type_lifecycle (identity → types → logic)
//   executable: execution_flow (setup → run → cleanup)
//   demo-test: concern_coverage (constructor → builder → ... → integration)
//
// Known gaps (form schemas reference but no registry entries):
//   X6 (template-only section) — only active in bare-bone template
//   TestCoverageSummary — only available in demo-test
//   Individual test subsections (ConstructorTests, etc.) — mapped to
//     parent "tests" entry in registry with subContainers
//
// "And he made in the most holy house two cherubims of image work,
//  and overlaid them with gold." — 2 Chronicles 3:10
// ============================================================================
