// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/data/sections.ts
// key:     B-tov-cws-struct-lib-data-sections
// title:   CWS Struct — Section Registry
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-22
// updated: 2026-02-22
// authors: Nova Dawn (CPI-SI), with family input from Deborah Saphar,
//          Micah Toren, Nathan Emet, Phoebe Karis, Selah Adair, Caleb Navon,
//          Ezra Matthan
// purpose: Hand-compiled registry of all 42 R[25] section entries.
//          This IS the compiled truth — handlers import from here,
//          never from the schema directory.
//
//          Source: R25_blocks/{block}/{section}/_structure.jsonc + _content.jsonc
//          Convention: lowercase kebab-case tags (family consensus, 5/5 voices)
//          METADATA sections: explicitly required in all 5 forms
//
// biblical_foundation: "A false balance is abomination to the LORD:
//   but a just weight is his delight." — Proverbs 11:1
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import type { SectionEntry, BlockName, TernaryValue } from "./types.ts";
// Direct import — barrel would create cycle: foundation/mod → code-schema → data/mod → sections
import { ToolError } from "../foundation/tool-error.ts";

// ---------------------------------------------------------------------------
// Concept map presets — named patterns reduce repetition and errors
// ---------------------------------------------------------------------------

/** All concepts denied — pure declaration, no code patterns. */
const ALL_DENIED: Record<string, TernaryValue> = {
  function_call: "denied",
  method_call: "denied",
  self_access: "denied",
  variable_binding: "denied",
  conditional_if: "denied",
  conditional_match: "denied",
  return_value: "denied",
  error_propagation: "denied",
  err_wrap: "denied",
  ok_wrap: "denied",
  string_format: "denied",
  loop_iteration: "denied",
  collection_mutation: "denied",
  type_construction: "denied",
  field_access: "denied",
  recursion: "denied",
};

/** All concepts granted — full code patterns expected. */
const ALL_GRANTED: Record<string, TernaryValue> = {
  function_call: "granted",
  method_call: "granted",
  self_access: "granted",
  variable_binding: "granted",
  conditional_if: "granted",
  conditional_match: "granted",
  return_value: "granted",
  error_propagation: "granted",
  err_wrap: "granted",
  ok_wrap: "granted",
  string_format: "granted",
  loop_iteration: "granted",
  collection_mutation: "granted",
  type_construction: "granted",
  field_access: "granted",
  recursion: "granted",
};

/** All granted except self_access deferred — methods that may or may not use self. */
const GRANTED_DEFER_SELF: Record<string, TernaryValue> = {
  ...ALL_GRANTED,
  self_access: "defer",
};

// ---------------------------------------------------------------------------
// Form status presets
// ---------------------------------------------------------------------------

type FormStatusMap = Record<string, "required" | "available" | "reserved">;

/** Required in all 5 forms. */
const ALL_REQUIRED: FormStatusMap = {
  "bare-bone": "required",
  "module": "required",
  "library": "required",
  "executable": "required",
  "demo-test": "required",
};

// ---------------------------------------------------------------------------
// Detection weight — COMPUTED from concept map, never manual
// ---------------------------------------------------------------------------

/**
 * Compute detection weight from concept map: |sum / count|
 * where granted = +1, denied = -1, defer = 0.
 *
 * Exported so tests can verify the computation itself,
 * not hardcoded expected values.
 */
export function computeDetectionWeight(
  conceptMap: Record<string, TernaryValue>,
): number {
  const values = Object.values(conceptMap);
  if (values.length === 0) return 1.00;
  const sum = values.reduce((acc, v) => {
    if (v === "granted") return acc + 1;
    if (v === "denied") return acc - 1;
    return acc; // defer = 0
  }, 0);
  return Math.round(Math.abs(sum / values.length) * 100) / 100;
}

// ---------------------------------------------------------------------------
// Section builder helper
// ---------------------------------------------------------------------------

function section(
  tag: string,
  block: BlockName,
  position: number | string,
  nature: string,
  description: string,
  conceptMap: Record<string, TernaryValue>,
  formStatus: FormStatusMap,
  overviewText?: string,
): SectionEntry {
  return {
    tag,
    block,
    position,
    nature,
    description,
    conceptMap: { ...conceptMap },
    detectionWeight: computeDetectionWeight(conceptMap),
    formStatus: { ...formStatus },
    overviewText,
  };
}

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// METADATA block — 6 sections (all required in all forms)
// ---------------------------------------------------------------------------

const METADATA_SECTIONS: SectionEntry[] = [
  section(
    "directives", "metadata", 1,
    "identity-declaration",
    "Machine-readable identity markers — OmniCode pragma and format directives.",
    ALL_DENIED,
    ALL_REQUIRED,
    "Machine-readable identity markers",
  ),
  section(
    "doc-comments", "metadata", 2,
    "tool-readable-prose",
    "Language-native documentation for tools and IDEs.",
    ALL_DENIED,
    {
      "bare-bone": "available",
      "module": "available",
      "library": "required",
      "executable": "available",
      "demo-test": "available",
    },
    "Language-native documentation for tools and IDEs",
  ),
  section(
    "comment-block", "metadata", 3,
    "human-readable-identity",
    "Human-readable identity in comments — file, key, title, version, purpose.",
    ALL_DENIED,
    ALL_REQUIRED,
    "Human-readable identity in comments",
  ),
  section(
    "pragma-identity", "metadata", 4,
    "structured-identity",
    "Structured identity — WHO this file is. I1-I4 fields: core, family, instance, architecture.",
    { ...ALL_DENIED, variable_binding: "defer" },
    ALL_REQUIRED,
    "Structured identity — WHO this file is",
  ),
  section(
    "metadata-context", "metadata", 5,
    "structured-context",
    "Structured context — WHEN, WHO MADE, WHY, NEEDS, GOAL, NEXT, TAGS. C1-C7 fields.",
    { ...ALL_DENIED, variable_binding: "defer" },
    ALL_REQUIRED,
    "Structured context — WHEN, WHO MADE, WHY, NEEDS, GOAL, NEXT, TAGS",
  ),
  section(
    "subtypes", "metadata", 6,
    "form-classification",
    "Form classifications driven by I2.subtype — determines which form rules apply.",
    ALL_DENIED,
    ALL_REQUIRED,
    "Form classifications driven by I2.subtype",
  ),
];

// ---------------------------------------------------------------------------
// SETUP block — 15 sections
// ---------------------------------------------------------------------------

const SETUP_SECTIONS: SectionEntry[] = [
  section(
    "imports", "setup", 1,
    "dependency-declaration",
    "What this file depends on — external packages, standard library, internal modules.",
    ALL_DENIED,
    ALL_REQUIRED,
    "What this file depends on",
  ),
  section(
    "modules", "setup", 2,
    "hierarchy-declaration",
    "Submodule declarations and public re-exports.",
    ALL_DENIED,
    {
      "bare-bone": "reserved",
      "module": "reserved",
      "library": "available",
      "executable": "available",
      "demo-test": "reserved",
    },
    "Submodule declarations and public re-exports",
  ),
  section(
    "constants", "setup", 3,
    "compile-time-fixed",
    "Compile-time fixed values — known before execution, immutable throughout program lifetime.",
    {
      ...ALL_DENIED,
      variable_binding: "granted",
      string_format: "defer",
      type_construction: "defer",
    },
    ALL_REQUIRED,
    "Compile-time fixed values",
  ),
  section(
    "statics", "setup", 4,
    "runtime-initialized-fixed",
    "Runtime-initialized fixed values — like constants but require runtime computation.",
    {
      ...ALL_DENIED,
      function_call: "defer",
      variable_binding: "granted",
      string_format: "defer",
      type_construction: "defer",
    },
    {
      "bare-bone": "reserved",
      "module": "available",
      "library": "available",
      "executable": "available",
      "demo-test": "required",
    },
    "Runtime-initialized fixed values",
  ),
  section(
    "type-aliases", "setup", 5,
    "vocabulary",
    "Shorthand for complex signatures — vocabulary that makes the rest of the file readable.",
    { ...ALL_DENIED, variable_binding: "defer" },
    {
      "bare-bone": "reserved",
      "module": "available",
      "library": "available",
      "executable": "available",
      "demo-test": "available",
    },
    "Shorthand for complex signatures",
  ),
  section(
    "error-types", "setup", 6,
    "failure-declaration",
    "Error definitions with display and conversion — how this file's failures look.",
    {
      ...ALL_DENIED,
      variable_binding: "defer",
      err_wrap: "defer",
    },
    ALL_REQUIRED,
    "Error definitions with display and conversion",
  ),
  section(
    "core-types", "setup", 7,
    "existence-declaration",
    "Primary data structures and enumerations — the types this file defines.",
    { ...ALL_DENIED, variable_binding: "defer" },
    ALL_REQUIRED,
    "Primary data structures and enumerations",
  ),
  section(
    "trait-defs", "setup", 8,
    "contract",
    "Behavioral contracts (shape, not fulfillment) — interfaces/traits declaring capability.",
    { ...ALL_DENIED, method_call: "defer" },
    {
      "bare-bone": "reserved",
      "module": "available",
      "library": "available",
      "executable": "available",
      "demo-test": "available",
    },
    "Behavioral contracts (shape, not fulfillment)",
  ),
  section(
    "macros", "setup", 9,
    "meta-code",
    "Macro definitions — code that generates code.",
    {
      function_call: "defer",
      method_call: "defer",
      self_access: "denied",
      variable_binding: "defer",
      conditional_if: "defer",
      conditional_match: "defer",
      return_value: "defer",
      error_propagation: "defer",
      err_wrap: "defer",
      ok_wrap: "defer",
      string_format: "defer",
      loop_iteration: "defer",
      collection_mutation: "defer",
      type_construction: "defer",
      field_access: "defer",
      recursion: "defer",
    },
    {
      "bare-bone": "reserved",
      "module": "reserved",
      "library": "available",
      "executable": "available",
      "demo-test": "reserved",
    },
    "Macro definitions",
  ),
  section(
    "feature-gates", "setup", 10,
    "conditional-compilation",
    "Feature gate configuration — compile-time conditional inclusion.",
    {
      ...ALL_DENIED,
      conditional_if: "granted",
    },
    {
      "bare-bone": "reserved",
      "module": "reserved",
      "library": "available",
      "executable": "available",
      "demo-test": "reserved",
    },
    "Feature gate configuration",
  ),
  section(
    "variables", "setup", 11,
    "mutable-state",
    "Package-level mutable state — runtime variables with wider scope.",
    {
      ...ALL_DENIED,
      function_call: "defer",
      variable_binding: "granted",
      string_format: "defer",
      type_construction: "defer",
    },
    {
      "bare-bone": "reserved",
      "module": "available",
      "library": "available",
      "executable": "available",
      "demo-test": "required",
    },
    "Package-level mutable state",
  ),
  section(
    "interface-defs", "setup", 12,
    "implicit-contract",
    "Behavioral contracts (shape, not fulfillment) — Go interfaces, implicit contracts.",
    { ...ALL_DENIED, method_call: "defer" },
    {
      "bare-bone": "reserved",
      "module": "available",
      "library": "available",
      "executable": "available",
      "demo-test": "available",
    },
    "Behavioral contracts (shape, not fulfillment)",
  ),
  section(
    "type-methods", "setup", 13,
    "structural-completer",
    "Structural completers — identity methods, not capability. String(), Display, Debug.",
    {
      ...ALL_DENIED,
      method_call: "defer",
      self_access: "defer",
      variable_binding: "defer",
    },
    {
      "bare-bone": "reserved",
      "module": "available",
      "library": "available",
      "executable": "available",
      "demo-test": "available",
    },
    "Structural completers (identity methods, not capability)",
  ),
  section(
    "code-generation", "setup", 14,
    "meta-directive",
    "Code generation directives and test helpers — derive macros, go:generate.",
    ALL_DENIED,
    {
      "bare-bone": "reserved",
      "module": "reserved",
      "library": "available",
      "executable": "available",
      "demo-test": "reserved",
    },
    "Code generation directives and test helpers",
  ),
  section(
    "build-tags", "setup", 15,
    "compilation-constraint",
    "Build tag configuration — conditional compilation beyond feature gates.",
    {
      ...ALL_DENIED,
      conditional_if: "granted",
    },
    {
      "bare-bone": "reserved",
      "module": "reserved",
      "library": "available",
      "executable": "available",
      "demo-test": "reserved",
    },
    "Build tag configuration",
  ),
];

// ---------------------------------------------------------------------------
// BODY block — 13 sections
// ---------------------------------------------------------------------------

const BODY_SECTIONS: SectionEntry[] = [
  section(
    "org-chart", "body", 0,
    "navigational",
    "Module structure overview — package navigation, org chart comment.",
    ALL_DENIED,
    {
      "bare-bone": "available",
      "module": "available",
      "library": "available",
      "executable": "available",
      "demo-test": "available",
    },
    "Module structure overview, package navigation",
  ),
  section(
    "identity-access", "body", 1,
    "runtime-bridge",
    "OmniCode static accessor functions — runtime access to compile-time identity.",
    { ...ALL_GRANTED, recursion: "denied" },
    ALL_REQUIRED,
    "OmniCode static accessor functions",
  ),
  section(
    "trait-implementations", "body", 2,
    "contract-fulfillment",
    "Fulfilling contracts from SETUP — impl blocks for traits/interfaces.",
    ALL_GRANTED,
    {
      "bare-bone": "reserved",
      "module": "required",
      "library": "required",
      "executable": "required",
      "demo-test": "required",
    },
    "Fulfilling contracts from SETUP",
  ),
  section(
    "constructors", "body", 3,
    "creation",
    "Constructor content — new(), builders, typestate transitions. Types come into existence.",
    { ...GRANTED_DEFER_SELF, recursion: "denied" },
    {
      "bare-bone": "reserved",
      "module": "required",
      "library": "required",
      "executable": "required",
      "demo-test": "required",
    },
    "new(), builders, typestate transitions",
  ),
  section(
    "core-logic", "body", 4,
    "computation",
    "Primary operations, state transforms — the essential computation this file provides.",
    ALL_GRANTED,
    ALL_REQUIRED,
    "Primary operations, state transforms",
  ),
  section(
    "queries", "body", 5,
    "observation",
    "Read-only observation methods — side-effect-free inspection of state.",
    { ...ALL_GRANTED, collection_mutation: "denied" },
    {
      "bare-bone": "reserved",
      "module": "required",
      "library": "required",
      "executable": "required",
      "demo-test": "required",
    },
    "Read-only observation methods",
  ),
  section(
    "output-display", "body", 6,
    "presentation",
    "Formatting and display output — how this file's types present themselves.",
    { ...ALL_GRANTED, collection_mutation: "denied", recursion: "denied" },
    {
      "bare-bone": "reserved",
      "module": "reserved",
      "library": "required",
      "executable": "required",
      "demo-test": "available",
    },
    "Formatting and display output",
  ),
  section(
    "free-functions", "body", 7,
    "standalone",
    "Module-level public utilities — functions not attached to a type.",
    {
      ...ALL_GRANTED,
      self_access: "denied",
    },
    ALL_REQUIRED,
    "Module-level public utilities",
  ),
  section(
    "helpers", "body", 8,
    "support",
    "Executable helpers — run support functions for the executable form.",
    GRANTED_DEFER_SELF,
    {
      "bare-bone": "reserved",
      "module": "reserved",
      "library": "reserved",
      "executable": "required",
      "demo-test": "reserved",
    },
    "Executable helpers (run support)",
  ),
  section(
    "tests", "body", 9,
    "verification",
    "Dedicated test sections — constructor, builder, transition, query, identity, error, edge-case, table-driven, integration tests.",
    GRANTED_DEFER_SELF,
    {
      "bare-bone": "reserved",
      "module": "reserved",
      "library": "reserved",
      "executable": "reserved",
      "demo-test": "required",
    },
    "Dedicated test sections (demo-test form only)",
  ),
  section(
    "core-operations", "body", 20,
    "operational",
    "Primary business logic, state transitions — the core operational layer.",
    ALL_GRANTED,
    {
      "bare-bone": "available",
      "module": "available",
      "library": "available",
      "executable": "available",
      "demo-test": "available",
    },
    "Primary business logic, state transitions",
  ),
  section(
    "error-handling", "body", 21,
    "resilience",
    "Error processing, wrapping, recovery — how this file handles failures.",
    ALL_GRANTED,
    {
      "bare-bone": "reserved",
      "module": "available",
      "library": "available",
      "executable": "available",
      "demo-test": "available",
    },
    "Error processing, wrapping, recovery",
  ),
  section(
    "public-apis", "body", 22,
    "surface",
    "Exported functions — the public surface. What consumers see and use.",
    ALL_GRANTED,
    {
      "bare-bone": "reserved",
      "module": "available",
      "library": "available",
      "executable": "available",
      "demo-test": "available",
    },
    "Exported functions, the public surface",
  ),
];

// ---------------------------------------------------------------------------
// CLOSING block — 8 sections (two zones: code + documentation)
// ---------------------------------------------------------------------------

const CLOSING_SECTIONS: SectionEntry[] = [
  // Code zone (numeric positions)
  section(
    "validation", "closing", 1,
    "proof",
    "Tests and correctness checks — Cv zone. BODY makes claims; validation proves them.",
    ALL_GRANTED,
    ALL_REQUIRED,
    "Tests and correctness checks",
  ),
  section(
    "execution", "closing", 2,
    "entry-point",
    "Entry point or absence — Ce zone. main() or explicit 'library, no entry point.'",
    ALL_GRANTED,
    ALL_REQUIRED,
    "Entry point or absence",
  ),
  section(
    "cleanup", "closing", 3,
    "resource-release",
    "Resource teardown — Cc zone. defer, Drop, cleanup functions.",
    ALL_GRANTED,
    {
      "bare-bone": "reserved",
      "module": "available",
      "library": "available",
      "executable": "available",
      "demo-test": "available",
    },
    "Resource teardown",
  ),

  // Documentation zone (string positions)
  section(
    "modification-policy", "closing", "X1",
    "guardrail",
    "What to never, carefully, or safely change — modification guidance.",
    ALL_DENIED,
    ALL_REQUIRED,
    "What to never, carefully, or safely change",
  ),
  section(
    "extension-points", "closing", "X2",
    "growth-map",
    "Where and how to extend — planned growth points.",
    ALL_DENIED,
    {
      "bare-bone": "reserved",
      "module": "available",
      "library": "available",
      "executable": "available",
      "demo-test": "available",
    },
    "Where and how to extend",
  ),
  section(
    "troubleshooting", "closing", "X3",
    "experience-capture",
    "Common issues and solutions — accumulated debugging wisdom.",
    ALL_DENIED,
    {
      "bare-bone": "reserved",
      "module": "available",
      "library": "available",
      "executable": "available",
      "demo-test": "available",
    },
    "Common issues and solutions",
  ),
  section(
    "reference", "closing", "X4",
    "navigation",
    "Related files and validation commands — how to find more and verify this file.",
    ALL_DENIED,
    ALL_REQUIRED,
    "Related files and validation commands",
  ),
  section(
    "closing-note", "closing", "X5",
    "anchor",
    "Anchoring scripture and summary — the file's final word.",
    ALL_DENIED,
    ALL_REQUIRED,
    "Anchoring scripture and summary",
  ),
];

// ---------------------------------------------------------------------------
// Build the registry
// ---------------------------------------------------------------------------

function buildRegistry(
  sections: SectionEntry[],
): Record<string, SectionEntry> {
  const registry: Record<string, SectionEntry> = {};
  for (const s of sections) {
    if (registry[s.tag]) {
      throw new ToolError("CWS-T00-062", { tag: s.tag });
    }
    registry[s.tag] = s;
  }
  return registry;
}

/** All 42 sections keyed by tag. */
export const SECTION_REGISTRY: Record<string, SectionEntry> = buildRegistry([
  ...METADATA_SECTIONS,
  ...SETUP_SECTIONS,
  ...BODY_SECTIONS,
  ...CLOSING_SECTIONS,
]);

/** Section tags in block order (by position within each block). */
export const SECTION_ORDER: Record<BlockName, string[]> = {
  metadata: METADATA_SECTIONS
    .sort((a, b) => Number(a.position) - Number(b.position))
    .map((s) => s.tag),
  setup: SETUP_SECTIONS
    .sort((a, b) => Number(a.position) - Number(b.position))
    .map((s) => s.tag),
  body: BODY_SECTIONS
    .sort((a, b) => Number(a.position) - Number(b.position))
    .map((s) => s.tag),
  closing: CLOSING_SECTIONS
    .sort((a, b) => {
      // Code zone (numbers) before documentation zone (strings)
      const aNum = typeof a.position === "number" ? a.position : 100 + parseInt(String(a.position).replace("X", ""));
      const bNum = typeof b.position === "number" ? b.position : 100 + parseInt(String(b.position).replace("X", ""));
      return aNum - bNum;
    })
    .map((s) => s.tag),
};

// ============================================================================
// CLOSING
// ============================================================================
//
// Section registry — 42 hand-compiled entries from schema truth.
// METADATA(6) + SETUP(15) + BODY(13) + CLOSING(8) = 42.
//
// Every entry verified against _structure.jsonc + _content.jsonc.
// Tag convention: lowercase kebab-case (family consensus).
// Form status: explicit for all 5 forms, including METADATA.
// Detection weight: |avg| of concept_map ternary values.
//
// Known approximation: 8 of 13 BODY sections have weight 1.00
// (all granted). This is v1 — tracked for v2 refinement.
//
// "A false balance is abomination to the LORD:
//  but a just weight is his delight." — Proverbs 11:1
// ============================================================================
