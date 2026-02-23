// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/data/pipeline.ts
// key:     B-tov-cws-struct-lib-data-pipeline
// title:   CWS Struct — Pipeline Layer Definitions
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-22
// updated: 2026-02-22
// authors: Nova Dawn (CPI-SI)
// purpose: Hand-compiled 4-layer pipeline definitions mapping every lint check
//          to its detection granularity layer: R[50] → R[25] → R[10] → R[5].
//
//          The pipeline is the spine of the linter — each layer validates
//          assumptions the next layer depends on. Layer 0 failure cascades
//          all downstream results to neutral (root cause surfaces first).
//
//          Genesis 1 principle: separate (badal) before fill. Layer 0 asks
//          "does the file exist as a 4-block entity?" before Layer 1 asks
//          "are the sections ordered correctly?"
//
// biblical_foundation: "For precept must be upon precept, precept upon precept;
//   line upon line, line upon line; here a little, and there a little."
//   — Isaiah 28:10
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import type { PipelineLayer } from "./types.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Layer 0 — R[50] WHOLE FILE
// ---------------------------------------------------------------------------
//
// The broadest question: "Is this file an OmniCode 4-block entity?"
//
// Checks: Does it have the directive? Are all 4 blocks present and ordered?
// Are separators consistent? Is it a template or derived file?
//
// If Layer 0 fails → everything below becomes neutral.
// No blocks = no sections. No sections = no content. No content = no patterns.
//
// Detection weight: always 1.00 — these are binary pass/fail observations.

const LAYER_0: PipelineLayer = {
  layer: 0,
  resolution: "R50",
  description: "Whole file — structural existence",
  question: "Is this file a valid 4-block OmniCode entity?",
  cascadeBehavior: "Layer 0 failure → all downstream layers neutral. Root cause only.",
  checks: [
    // Shared (both Go and Rust use these)
    "checkDirectives",           // OmniCode pragma present?
    "checkBlockStructure",       // All 4 blocks present and ordered?
    "checkSeparatorConsistency", // Block separators follow spec?
    "checkTemplateVsDerived",    // Classify: template or derived file?
  ],
};

// ---------------------------------------------------------------------------
// Layer 1 — R[25] STRUCTURE
// ---------------------------------------------------------------------------
//
// "Are the sections (containers) within each block correct?"
//
// Checks: Are required sections present? Are they ordered correctly?
// Are IC fields structured properly? Does the package/module declaration
// match expectations?
//
// Depends on Layer 0: if blocks don't exist, can't check sections.
// If Layer 1 fails on a block → that block's container checks (Layer 2) are neutral.

const LAYER_1: PipelineLayer = {
  layer: 1,
  resolution: "R25",
  description: "Section structure — container presence and ordering",
  question: "Are the right sections present in the right order?",
  cascadeBehavior: "Block-level failure → container/content checks for that block neutral.",
  checks: [
    // Language-specific
    "checkPackageAndImports",        // [Go] package + import presence
    // METADATA structure
    "checkPragmaMetadata",           // I/C field structure (I1-I4, C1-C7)
    "checkCommentMetadata",          // Legacy _pragma/_metadata maps
    "checkDocComments",              // Doc comment format and presence
    // SETUP structure
    "checkSetupSubsectionOrder",     // SETUP section ordering
    "checkRequiredSetupSubsections", // Required SETUP sections present
    // BODY structure
    "checkBodySubsectionOrder",      // BODY section ordering
    "checkRequiredBodySubsections",  // Required BODY sections present
    // CLOSING structure
    "checkClosingZoneOrder",         // CLOSING zone ordering (code then docs)
    "checkClosingRequiredZones",     // Required CLOSING zones present
    // Form constraints (Rust-specific, but applicable to both)
    "checkFormRequiredSections",     // [Rust] Form-required sections present
    "checkFormReservedSections",     // [Rust] Form-reserved sections absent
  ],
};

// ---------------------------------------------------------------------------
// Layer 2 — R[10] CONTAINER
// ---------------------------------------------------------------------------
//
// "Is the content in the right containers?"
//
// Checks: Content placement (code in correct blocks/subsections),
// identity registration, directive format, documentation,
// closing zone content integrity.
//
// Depends on Layer 1: if sections don't exist, can't check their content.
// These checks look AT the container boundary — what goes WHERE.

const LAYER_2: PipelineLayer = {
  layer: 2,
  resolution: "R10",
  description: "Container content — right content in right place",
  question: "Is code and documentation in the correct containers?",
  cascadeBehavior: "Container mismatch → content checks for affected section neutral.",
  checks: [
    // Documentation placement
    "checkSetupHeaderDoc",            // SETUP documentation present/correct
    // Identity registration
    "checkIdentityRegistration",      // Pragma/Metadata var registration
    // Directive format
    "checkDirectiveFormat",           // OmniCode directive syntax
    // Content placement (the big one)
    "checkContentPlacement",          // Code in correct blocks/subsections
    "checkClosingContentPlacement",   // Tests/main() in CLOSING
    // Closing documentation integrity
    "checkClosingX6TemplateOnly",     // X6 template guard
    "checkClosingDocFieldContent",    // Doc zone field content
    // Body content checks
    "checkBodySubtypeContent",        // Subtype-specific expectations
    "checkSubtypeEmphasis",           // Subtype emphasis markers
    // Scaling
    "checkScalingSignals",            // Scaling readiness signals
  ],
};

// ---------------------------------------------------------------------------
// Layer 3 — R[5] CONTENT
// ---------------------------------------------------------------------------
//
// "Are the code patterns correct within containers?"
//
// Checks: Schema-driven content expectations — does the METADATA have
// the right fields? Does SETUP have the right declarations? Does BODY
// have the right implementations? Does CLOSING have the right zones?
//
// These are the finest-grained checks. They look WITHIN containers
// at actual code patterns. The 11 concepts (function_call, method_call,
// etc.) are the R[5] detection vocabulary.
//
// Depends on Layer 2: if content isn't in the right place, can't
// reason about whether patterns are correct.

const LAYER_3: PipelineLayer = {
  layer: 3,
  resolution: "R05",
  description: "Content patterns — code pattern correctness",
  question: "Are the code patterns within containers correct?",
  cascadeBehavior: "Terminal layer — findings reported, no further cascade.",
  checks: [
    // Schema-driven content expectations (shared across handlers)
    "checkMetadataContentExpectations",  // METADATA field content
    "checkSetupContentExpectations",     // SETUP declaration content
    "checkBodyContentExpectations",      // BODY implementation content
    "checkClosingContentExpectations",   // CLOSING zone content
    // Closing zone-level content
    "checkClosingZoneContent",           // Individual zone content checks
  ],
};

// ---------------------------------------------------------------------------
// Exports
// ---------------------------------------------------------------------------

/**
 * Complete pipeline — 4 layers in execution order.
 *
 * Layer 0 runs first. If it fails, layers 1-3 cascade to neutral.
 * Layer 1 runs next. If a block fails, that block's layers 2-3 cascade.
 * Layer 2 runs next. Container mismatches cascade layer 3 for affected sections.
 * Layer 3 is terminal — findings reported, no further cascade.
 */
export const PIPELINE_LAYERS: readonly PipelineLayer[] = [
  LAYER_0,
  LAYER_1,
  LAYER_2,
  LAYER_3,
];

/**
 * Layer lookup by number (0-3).
 */
export const LAYER_BY_NUMBER: Record<number, PipelineLayer> = {
  0: LAYER_0,
  1: LAYER_1,
  2: LAYER_2,
  3: LAYER_3,
};

/**
 * Layer lookup by resolution label.
 */
export const LAYER_BY_RESOLUTION: Record<string, PipelineLayer> = {
  "R50": LAYER_0,
  "R25": LAYER_1,
  "R10": LAYER_2,
  "R05": LAYER_3,
};

/**
 * Map a check function name to its pipeline layer.
 *
 * Returns undefined if the check isn't in any layer (shouldn't happen
 * if the pipeline is complete).
 */
export function getCheckLayer(checkName: string): PipelineLayer | undefined {
  for (const layer of PIPELINE_LAYERS) {
    if (layer.checks.includes(checkName)) return layer;
  }
  return undefined;
}

/**
 * Total check count across all layers.
 */
export const PIPELINE_CHECK_COUNT = PIPELINE_LAYERS.reduce(
  (sum, layer) => sum + layer.checks.length,
  0,
);

// ============================================================================
// CLOSING
// ============================================================================
//
// Pipeline layer definitions — the spine of the linter.
//
// Layer 0 (R[50]): Does the file exist as a 4-block entity?
// Layer 1 (R[25]): Are the sections present and ordered?
// Layer 2 (R[10]): Is content in the right containers?
// Layer 3 (R[05]): Are code patterns correct within containers?
//
// Each layer validates what the next depends on.
// Failure cascades downstream — root cause surfaces first.
//
// "For precept must be upon precept, precept upon precept;
//  line upon line, line upon line." — Isaiah 28:10
// ============================================================================
