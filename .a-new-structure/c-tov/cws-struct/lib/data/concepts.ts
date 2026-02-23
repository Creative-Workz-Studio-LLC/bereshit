// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/data/concepts.ts
// key:     B-tov-cws-struct-lib-data-concepts
// title:   CWS Struct — Concept Registry
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-22
// updated: 2026-02-22
// authors: Nova Dawn (CPI-SI)
// purpose: Hand-compiled registry of all 11 R[5] concept entries — the
//          TRANSPOSE of section concept_maps. 11 concepts x 42 containers
//          = 462 ternary values, same data as sections.ts but indexed by
//          concept instead of container.
//
//          The concept registry answers: "For concept C, which containers
//          expect/deny/defer this pattern?" — the inverse question to
//          sections.ts which asks "For container S, which concepts apply?"
//
//          Cross-validated at build time via verifyTranspose().
//
// biblical_foundation: "For as the body is one, and hath many members, and
//   all the members of that one body, being many, are one body."
//   — 1 Corinthians 12:12
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import type { ConceptEntry, TernaryValue } from "./types.ts";
import { SECTION_REGISTRY } from "./sections.ts";
// Direct import — barrel would create cycle: foundation/mod → code-schema → data/mod → concepts
import { ToolError } from "../foundation/tool-error.ts";

// ---------------------------------------------------------------------------
// Concept IDs — the 11 R[5] patterns
// ---------------------------------------------------------------------------

const CONCEPT_IDS = [
  "function_call",
  "method_call",
  "self_access",
  "variable_binding",
  "conditional_if",
  "conditional_match",
  "return_value",
  "error_propagation",
  "err_wrap",
  "ok_wrap",
  "string_format",
] as const;

type ConceptId = typeof CONCEPT_IDS[number];

// ---------------------------------------------------------------------------
// Concept descriptions — what each pattern represents
// ---------------------------------------------------------------------------

const CONCEPT_DESCRIPTIONS: Record<ConceptId, string> = {
  function_call:
    "Function invocation — calling standalone or package-level functions.",
  method_call:
    "Method invocation — calling methods on an instance or type.",
  self_access:
    "Self/receiver access — accessing fields or methods via self/receiver.",
  variable_binding:
    "Variable binding — let, const, var, := bindings that create named values.",
  conditional_if:
    "Conditional branching (if) — if/else, guard clauses, early returns.",
  conditional_match:
    "Pattern matching — match/switch exhaustive value dispatch.",
  return_value:
    "Return value — explicit returns, last-expression returns, Ok/Err wrapping.",
  error_propagation:
    "Error propagation — ?, try, or explicit error forwarding.",
  err_wrap:
    "Error wrapping — creating or wrapping error values (Err, fmt.Errorf).",
  ok_wrap:
    "Success wrapping — creating success values (Ok, Some).",
  string_format:
    "String formatting — format!, println!, fmt.Sprintf, template literals.",
};

// ---------------------------------------------------------------------------
// Concept directory names — kebab-case for filesystem paths
// ---------------------------------------------------------------------------

const CONCEPT_DIR_NAMES: Record<ConceptId, string> = {
  function_call: "function-call",
  method_call: "method-call",
  self_access: "self-access",
  variable_binding: "variable-binding",
  conditional_if: "conditional-if",
  conditional_match: "conditional-match",
  return_value: "return-value",
  error_propagation: "error-propagation",
  err_wrap: "err-wrap",
  ok_wrap: "ok-wrap",
  string_format: "string-format",
};

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Transpose — build concept container maps from section concept maps
// ---------------------------------------------------------------------------
//
// The section registry has 42 entries, each with a conceptMap of 11 values.
// We transpose this to get 11 entries, each with a containerMap of 42 values.
//
// The transpose is computed at module load time from the section registry.
// This guarantees consistency — same data, two views, single source of truth.

function transposeSections(): Record<string, ConceptEntry> {
  const result: Record<string, ConceptEntry> = {};

  // Initialize empty container maps for each concept
  for (const id of CONCEPT_IDS) {
    result[id] = {
      id,
      dirName: CONCEPT_DIR_NAMES[id],
      description: CONCEPT_DESCRIPTIONS[id],
      containerMap: {},
    };
  }

  // Transpose: for each section, copy its concept values into the concept's container map
  for (const [sectionTag, section] of Object.entries(SECTION_REGISTRY)) {
    for (const conceptId of CONCEPT_IDS) {
      const value = section.conceptMap[conceptId];
      if (value === undefined) {
        throw new ToolError("CWS-T00-060", {
          section: sectionTag,
          concept: conceptId,
        });
      }
      result[conceptId]!.containerMap[sectionTag] = value as TernaryValue;
    }
  }

  // Validate: each concept should have exactly 42 container entries
  const sectionCount = Object.keys(SECTION_REGISTRY).length;
  for (const id of CONCEPT_IDS) {
    const containerCount = Object.keys(result[id]!.containerMap).length;
    if (containerCount !== sectionCount) {
      throw new ToolError("CWS-T00-061", {
        concept: id,
        actual: String(containerCount),
        expected: String(sectionCount),
      });
    }
  }

  return result;
}

// ---------------------------------------------------------------------------
// Exports
// ---------------------------------------------------------------------------

/**
 * All 11 concept entries keyed by concept ID.
 *
 * Computed by transposing the section registry's concept maps.
 * Guaranteed consistent with SECTION_REGISTRY — same 462 ternary values.
 */
export const CONCEPT_REGISTRY: Record<string, ConceptEntry> = transposeSections();

/**
 * Ordered list of all concept IDs.
 *
 * Stable order for iteration, display, and hash computation.
 */
export const CONCEPT_ORDER: readonly string[] = [...CONCEPT_IDS];

// ============================================================================
// CLOSING
// ============================================================================
//
// Concept registry — the transpose view of the 462-value ternary matrix.
// Built from sections.ts at module load time, guaranteed consistent.
//
// "For as the body is one, and hath many members, and all the members of
//  that one body, being many, are one body." — 1 Corinthians 12:12
//
// The body has many members. The matrix has two views. Both tell the same truth.
// ============================================================================
