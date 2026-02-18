// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/foundation/mod.ts
// key:     B-tov-cws-struct-lib-foundation-mod
// title:   CWS Struct — Foundation Barrel
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-17
// authors: Nova Dawn (CPI-SI)
// purpose: Re-export all foundation types and functions.
//          Handlers and engine import from here — single entry point.
//
// ============================================================================

// Types + result constructors + summarize
export type {
  Severity,
  FixSuggestion,
  LintResult,
  LintSummary,
  FormatHandler,
  TransformOptions,
  CliOptions,
} from "./types.ts";

export {
  error,
  warn,
  info,
  summarize,
} from "./types.ts";

// Health scoring
export type {
  AtomicAction,
  ContainerScore,
  BlockScore,
  HealthScore,
} from "./health.ts";

export {
  computeContainerScore,
  computeBlockScore,
  computeHealthScore,
} from "./health.ts";

// Schema loader
export type {
  FieldRequirement,
  MetadataContract,
  ContentContract,
  ClosingContract,
  ValidationContract,
  DerivedRules,
} from "./schema.ts";

export {
  loadRules,
  clearCache,
} from "./schema.ts";

// ============================================================================
// CLOSING
// ============================================================================
//
// One import path for all foundation contracts.
// "A just weight and balance are the LORD's." — Proverbs 16:11
// ============================================================================
