// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/types/data.ts
// key:     B-tov-cws-struct-lib-types-data
// title:   CWS Struct — Data Layer Types
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-24
// authors: Nova Dawn (CPI-SI)
// purpose: Pure type definitions for the data layer — SectionEntry,
//          ConceptEntry, FormDefinition, ErrorCodeEntry, and supporting
//          interfaces. Runtime validation functions and registry population
//          remain in data/types.ts.
//
// biblical_foundation: "The words of the LORD are pure words: as silver tried
//   in a furnace of earth, purified seven times." — Psalm 12:6
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import type { Severity, BlockName } from "./core.ts";
import type { HebrewState, HebrewDirection } from "./health.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Ternary — the atomic unit of the data layer
// ---------------------------------------------------------------------------

/**
 * Ternary concept value — granted/denied/defer.
 *
 * Maps to the building block method:
 *   granted (+1) — pattern expected in this container
 *   denied  (-1) — pattern NOT expected in this container
 *   defer    (0) — context-dependent, needs more information
 *
 * 42 containers x 11 concepts = 462 ternary values.
 * Every one has been verified against the schema.
 */
export type TernaryValue = "granted" | "denied" | "defer";

// ---------------------------------------------------------------------------
// Section Entry — R[25] container definition
// ---------------------------------------------------------------------------

/**
 * One section (container) in the 4-block structure.
 *
 * The 42 containers across METADATA(6), SETUP(15), BODY(13), CLOSING(8).
 * Each carries its concept_map (11 ternary values), detection_weight,
 * and per-form availability (required/available/reserved).
 *
 * Source: R25_blocks/{block}/{section}/_structure.jsonc + _content.jsonc
 */
export interface SectionEntry {
  /** Unique tag identifier (e.g., "imports", "core-types", "validation"). */
  tag: string;

  /** Parent block. */
  block: BlockName;

  /**
   * Position within block.
   *
   * Most blocks use 1-based integers.
   * CLOSING has two zones:
   *   - Core zone: 1, 2, 3 (validation, execution, cleanup)
   *   - Extension zone: "X1"-"X5" (modification-policy through closing-note)
   */
  position: number | string;

  /** Content nature (e.g., "dependency-declaration", "creation", "proof"). */
  nature: string;

  /** Brief description of what this section contains. */
  description: string;

  /**
   * Concept map — 11 ternary values indicating which R[5] patterns
   * are expected in this container.
   *
   * Keys are concept IDs: function_call, method_call, self_access,
   * variable_binding, conditional_if, conditional_match, return_value,
   * error_propagation, err_wrap, ok_wrap, string_format.
   */
  conceptMap: Record<string, TernaryValue>;

  /**
   * Detection weight — |avg| of concept_map.
   *
   * Range: 0.00 to 1.00.
   * Containers with all granted/denied = 1.00 (high certainty).
   * Containers with many defers = lower (context-dependent).
   *
   * Used by the health scorer to weight observations from this container.
   */
  detectionWeight: number;

  /**
   * Per-form section status.
   *
   * Key: form name (e.g., "bare-bone", "library").
   * Value: "required" | "available" | "reserved".
   *
   * Reserved sections are NOT in a form and should be absent.
   * Required sections MUST be present.
   * Available sections CAN be present.
   */
  formStatus: Record<string, "required" | "available" | "reserved">;

  /**
   * Human-readable overview text from the schema.
   * Used in diagnostic messages (e.g., "Imports — What this file depends on").
   *
   * Source: _structure.jsonc -> section.overview_text
   */
  overviewText?: string;

  /**
   * Sub-containers within this section (if any).
   * METADATA sections have sub-containers (e.g., directives has I1-I4).
   * Populated from directory structure, not schema fields.
   */
  subContainers?: string[];

  /**
   * Routing network — what concepts this section can handle,
   * and where misplaced content should go.
   *
   * Not the same as conceptMap: conceptMap says which CODE PATTERNS appear;
   * routing says which CONCEPTS belong and where misplaced code GOES.
   *
   * Optional for v1. Required for R[10] diagnostics.
   *
   * Source: _structure.jsonc -> can/cannot/defer_to
   */
  routing?: {
    /** Concepts this section handles. */
    canDo: string[];
    /** Concept -> target section tag for misplaced content. */
    deferTo: Record<string, string>;
  };
}

// ---------------------------------------------------------------------------
// Concept Entry — R[5] pattern definition
// ---------------------------------------------------------------------------

/**
 * One concept (pattern) in the R[5] detection system.
 *
 * 11 concepts: function-call, method-call, self-access, variable-binding,
 * conditional-if, conditional-match, return-value, error-propagation,
 * err-wrap, ok-wrap, string-format.
 *
 * Each carries a container_map — the TRANSPOSE of section concept_maps.
 * 11 concepts x 42 containers = 462 ternary values (same data, two views).
 *
 * Source: R5_patterns/{concept}/concepts.jsonc
 */
export interface ConceptEntry {
  /** Concept identifier (e.g., "function_call", "self_access"). */
  id: string;

  /** Kebab-case directory name (e.g., "function-call", "self-access"). */
  dirName: string;

  /** Human description. */
  description: string;

  /**
   * Container map — 42 ternary values indicating which containers
   * expect this pattern.
   *
   * Keys are section tags: "imports", "core-types", "constructors", etc.
   * Values: "granted" | "denied" | "defer".
   *
   * MUST transpose-match section conceptMaps (verified at build time).
   */
  containerMap: Record<string, TernaryValue>;
}

// ---------------------------------------------------------------------------
// Form Definition — subtype form constraints
// ---------------------------------------------------------------------------

/**
 * One form (subtype) definition — what sections are active/reserved.
 *
 * 5 forms: bare-bone, module, library, executable, demo-test.
 * Forms define which of the 42 sections are required, available,
 * or reserved for each subtype.
 *
 * Source: schemas/code/forms/{form-name}.jsonc
 */
export interface FormDefinition {
  /** Form name (e.g., "bare-bone", "library"). */
  name: string;

  /** Whether this is the foundation form. */
  isFoundation: boolean;

  /** Parent form this inherits from (usually "bare-bone"). */
  inheritsFrom?: string;

  /** Brief description of this form's purpose. */
  description: string;

  /**
   * Per-block section constraints.
   *
   * Key: block name (e.g., "metadata", "setup", "body", "closing").
   * Value: map of section_tag -> "required" | "available" | "reserved".
   *
   * METADATA is explicit — all 6 sections are required in all forms.
   */
  sections: Record<string, Record<string, "required" | "available" | "reserved">>;

  /**
   * Section ordering strategy for this form.
   *
   * "unpositioned" — bare-bone (no ordering enforced)
   * "type_lifecycle" — library (IdentityAccess -> TraitImpl -> Constructors -> ...)
   * "execution_flow" — executable (setup -> run -> cleanup)
   *
   * Source: forms/{name}/_structure.jsonc -> body.ordering
   */
  ordering?: string;

  /**
   * Canonical section order when ordering is specified.
   *
   * Array of section tags in the expected order.
   * Only meaningful when ordering !== "unpositioned".
   *
   * Source: forms/{name}/_structure.jsonc -> body.ordering_sequence
   */
  orderingSequence?: string[];
}

// ---------------------------------------------------------------------------
// Placement Rule — R[10] language-specific construct placement
// ---------------------------------------------------------------------------

/**
 * One placement rule — where a language construct belongs.
 *
 * Source: schemas/code/R50_codefile/languages/{go,rust}/*.jsonc
 */
export interface PlacementRule {
  /** Language construct name (e.g., "package_decl", "use_decl", "struct_def"). */
  construct: string;

  /** Target container tag (e.g., "imports", "core-types"). */
  targetContainer: string;

  /** Target block. */
  targetBlock: BlockName;

  /** Human-readable reason for placement. */
  reason: string;
}

// ---------------------------------------------------------------------------
// Hebrew State Definition — health system data
// ---------------------------------------------------------------------------

/**
 * One Hebrew state definition — range, emoji, direction.
 *
 * 7 states: broken, wanting, lacking, even, sound, whole, perfect.
 * Already implemented in health.ts — this mirrors it for data layer access.
 *
 * Source: bereshit-base-algorithms.adoc + log.toml [ternary.levels]
 */
export interface HebrewStateDefinition {
  /** State name. */
  name: HebrewState;

  /** Hebrew name. */
  hebrew: string;

  /** Score range: [min, max]. */
  range: [number, number];

  /** Visual emoji indicator. */
  emoji: string;

  /** Direction: -1 (misaligned), 0 (neutral), +1 (aligned). */
  direction: HebrewDirection;

  /** Terminal color for output. */
  color: string;
}

// ---------------------------------------------------------------------------
// Error Code — structured error identification
// ---------------------------------------------------------------------------

/**
 * One error code definition.
 *
 * Format: CWS-{LAYER}-{SEQ}
 * - LAYER: R50 (whole file), R25 (structure), R10 (container), R05 (content)
 * - SEQ: 3-digit sequence within layer (001-999)
 *
 * Example: CWS-R50-001 = "Missing OmniCode directive"
 */
export interface ErrorCodeEntry {
  /** Full code string (e.g., "CWS-R50-001"). */
  code: string;

  /** Detection layer: 0 = R50, 1 = R25, 2 = R10, 3 = R05, 4 = T00 (tool). */
  layer: 0 | 1 | 2 | 3 | 4;

  /** Human layer name for display. */
  layerName: "R50" | "R25" | "R10" | "R05" | "T00";

  /** Category within the layer (e.g., "directive", "block", "identity"). */
  category: string;

  /** Default severity for this error. */
  severity: Severity;

  /** Human-readable message template. Use {{field}} for substitution. */
  messageTemplate: string;

  /** Suggestion template — what to do about it. */
  suggestionTemplate: string;

  /** Which check function produces this code (for traceability). */
  checkFunction: string;
}

// ---------------------------------------------------------------------------
// Pipeline Layer — detection granularity mapping
// ---------------------------------------------------------------------------

/**
 * Pipeline layer definition — maps R[n] to lint checks.
 *
 * Layer 0 = R[50] — whole file (blocks present, ordered)
 * Layer 1 = R[25] — structure (sections present, ordered)
 * Layer 2 = R[10] — container (content in right place)
 * Layer 3 = R[05] — content (code patterns correct)
 */
export interface PipelineLayer {
  /** Layer number: 0-3. */
  layer: 0 | 1 | 2 | 3;

  /** Resolution label. */
  resolution: "R50" | "R25" | "R10" | "R05";

  /** Human description. */
  description: string;

  /** The question this layer answers. */
  question: string;

  /** Cascade behavior on failure. */
  cascadeBehavior: string;

  /** Check function names that belong to this layer. */
  checks: string[];
}

// ---------------------------------------------------------------------------
// Log Entry — structured logging
// ---------------------------------------------------------------------------

/**
 * Structured log entry — TIMESTAMP|ACTION|DELTA|SOURCE|DETAIL.
 *
 * Source: log.toml format specification.
 */
export interface LogEntry {
  /** ISO 8601 timestamp. */
  timestamp: string;

  /** Action category: success, failure, neutral, cascade, skip. */
  action: "success" | "failure" | "neutral" | "cascade" | "skip";

  /** Score delta: +1, -1, 0, or scaled by impact weight. */
  delta: number;

  /** Source identifier: format/check or format/layer. */
  source: string;

  /** Detail: container/block path + reason. */
  detail: string;
}

// ---------------------------------------------------------------------------
// DataLayer — the composite registry
// ---------------------------------------------------------------------------

/**
 * The complete data layer — all registries assembled.
 *
 * Schema-driven, not schema-dependent. This is a COMPILED copy of
 * schema truth — the linter doesn't import from the schema directory
 * at runtime. It has its own copy.
 */
export interface DataLayer {
  /** All 42 section entries, keyed by tag. */
  sections: Record<string, SectionEntry>;

  /** Section tags ordered by block and position. */
  sectionOrder: {
    metadata: string[];
    setup: string[];
    body: string[];
    closing: string[];
  };

  /** All 11 concept entries, keyed by concept ID. */
  concepts: Record<string, ConceptEntry>;

  /** All 5 form definitions, keyed by form name. */
  forms: Record<string, FormDefinition>;

  /** Per-language placement rules. */
  placement: Record<string, PlacementRule[]>;

  /** Hebrew state definitions (7 entries). */
  states: HebrewStateDefinition[];

  /** Error code catalog, keyed by code string. */
  errorCodes: Record<string, ErrorCodeEntry>;

  /** Pipeline layer definitions (4 entries). */
  pipelineLayers: PipelineLayer[];
}

// ============================================================================
// CLOSING
// ============================================================================
//
// Data layer types — the compiled schema truth that handlers consume.
// 42 sections, 11 concepts, 5 forms, 7 states, 4 pipeline layers.
// Pure type definitions only. Runtime validation in data/types.ts.
//
// "The words of the LORD are pure words: as silver tried in a furnace
//  of earth, purified seven times." — Psalm 12:6
// ============================================================================
