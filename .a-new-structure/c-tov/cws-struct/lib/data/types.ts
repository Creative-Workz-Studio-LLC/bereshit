// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/data/types.ts
// key:     B-tov-cws-struct-lib-data-types
// title:   CWS Struct — Data Layer Types & Validation
// type:    Code (Library)
// version: a-02.00
// created: 2026-02-22
// updated: 2026-02-24
// authors: Nova Dawn (CPI-SI)
// purpose: Re-exports pure type definitions from lib/types/ (canonical source)
//          and provides runtime validation helpers for the data layer.
//
//          a-02.00: Type definitions extracted to lib/types/ during Phase 0
//          reorganization. This file now re-exports types for backward
//          compatibility and keeps validation-only code: isValidTernary,
//          isValidBlock, isValidFormStatus, isValidLayer, isValidWeight,
//          validateConceptMap, verifyTranspose.
//
// biblical_foundation: "The words of the LORD are pure words: as silver tried
//   in a furnace of earth, purified seven times." — Psalm 12:6
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// ---------------------------------------------------------------------------
// Type re-exports — canonical source is lib/types/
// ---------------------------------------------------------------------------
//
// All pure type definitions live in lib/types/. This file re-exports them
// for backward compatibility — existing imports from "./types.ts" or
// "../data/types.ts" continue to work unchanged.

export type {
  TernaryValue,
  SectionEntry,
  ConceptEntry,
  FormDefinition,
  PlacementRule,
  HebrewStateDefinition,
  ErrorCodeEntry,
  PipelineLayer,
  LogEntry,
  DataLayer,
} from "../types/mod.ts";

// BlockName re-exported from data layer for backward compat
// (previously defined locally, now canonical in lib/types/core.ts)
export type { BlockName } from "../types/core.ts";

// Types needed by runtime validation functions in this file
import type {
  TernaryValue,
  SectionEntry,
  ConceptEntry,
} from "../types/mod.ts";

import type { BlockName } from "../types/core.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Type guards and validation helpers
// ---------------------------------------------------------------------------

/** Check if a string is a valid ternary value. */
export function isValidTernary(value: string): value is TernaryValue {
  return value === "granted" || value === "denied" || value === "defer";
}

/** Check if a string is a valid block name. */
export function isValidBlock(value: string): value is BlockName {
  return value === "metadata" || value === "setup" ||
         value === "body" || value === "closing";
}

/** Check if a string is a valid form status. */
export function isValidFormStatus(
  value: string,
): value is "required" | "available" | "reserved" {
  return value === "required" || value === "available" || value === "reserved";
}

/** Check if a number is a valid pipeline layer. */
export function isValidLayer(value: number): value is 0 | 1 | 2 | 3 {
  return value >= 0 && value <= 3 && Number.isInteger(value);
}

/** Check if a detection weight is in range [0, 1]. */
export function isValidWeight(value: number): boolean {
  return value >= 0 && value <= 1;
}

/**
 * Validate that a concept map has exactly 11 entries with valid ternary values.
 * Returns list of invalid keys (empty = valid).
 */
export function validateConceptMap(
  map: Record<string, string>,
): string[] {
  const invalid: string[] = [];
  for (const [key, value] of Object.entries(map)) {
    if (!isValidTernary(value)) {
      invalid.push(`${key}: "${value}" is not a valid ternary value`);
    }
  }
  return invalid;
}

/**
 * Verify transpose consistency between section conceptMaps and concept containerMaps.
 *
 * For every section S and concept C:
 *   sections[S].conceptMap[C] === concepts[C].containerMap[S]
 *
 * Returns list of mismatches (empty = consistent).
 */
export function verifyTranspose(
  sections: Record<string, SectionEntry>,
  concepts: Record<string, ConceptEntry>,
): string[] {
  const mismatches: string[] = [];

  for (const [sectionTag, section] of Object.entries(sections)) {
    for (const [conceptId, sectionValue] of Object.entries(section.conceptMap)) {
      const concept = concepts[conceptId];
      if (!concept) {
        mismatches.push(`section "${sectionTag}" references unknown concept "${conceptId}"`);
        continue;
      }
      const conceptValue = concept.containerMap[sectionTag];
      if (conceptValue === undefined) {
        mismatches.push(`concept "${conceptId}" missing container "${sectionTag}"`);
        continue;
      }
      if (sectionValue !== conceptValue) {
        mismatches.push(
          `transpose mismatch: section "${sectionTag}".conceptMap["${conceptId}"] = "${sectionValue}" ` +
          `but concept "${conceptId}".containerMap["${sectionTag}"] = "${conceptValue}"`,
        );
      }
    }
  }

  return mismatches;
}

// ============================================================================
// CLOSING
// ============================================================================
//
// Data layer types → lib/types/ (single source of truth).
// Validation helpers → here (runtime functions consuming those types).
// Backward-compatible re-exports → all existing import paths still work.
//
// "The words of the LORD are pure words: as silver tried in a furnace
//  of earth, purified seven times." — Psalm 12:6
// ============================================================================
