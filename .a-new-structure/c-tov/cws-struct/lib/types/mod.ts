// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/types/mod.ts
// key:     B-tov-cws-struct-lib-types-mod
// title:   CWS Struct — Types Barrel Export
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-24
// authors: Nova Dawn (CPI-SI)
// purpose: Single entry point for all pure type definitions. Every type in
//          the system can be imported from @lib/types/mod.ts. Internal DAG:
//          core.ts (0 deps) <- health.ts <- foundation.ts, data.ts <- mod.ts.
//
// biblical_foundation: "And God saw every thing that he had made, and, behold,
//   it was very good." — Genesis 1:31
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// (No imports — this is a barrel re-export module.)

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Core primitives (zero dependencies)
// ---------------------------------------------------------------------------

export type {
  Severity,
  LintPolicy,
  LintLayer,
  BlockName,
} from "./core.ts";

// ---------------------------------------------------------------------------
// Health scoring types (depends on: core)
// ---------------------------------------------------------------------------

export type {
  HebrewState,
  HebrewDirection,
  AtomicAction,
  ContainerScore,
  BlockScore,
  HealthScore,
} from "./health.ts";

// ---------------------------------------------------------------------------
// Foundation types — lint, handler, CLI (depends on: core, health)
// ---------------------------------------------------------------------------

export type {
  LintGridOptions,
  FixAction,
  FixSuggestion,
  LintResult,
  AnatomyBlock,
  AnatomySection,
  ParsedAnatomy,
  LintSummary,
  TransformOptions,
  InspectBlock,
  InspectSection,
  SubsectionDef,
  InspectContent,
  InspectResult,
  FormatHandler,
  CliOptions,
} from "./foundation.ts";

// ---------------------------------------------------------------------------
// Data layer types (depends on: core, health)
// ---------------------------------------------------------------------------

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
} from "./data.ts";

// ============================================================================
// CLOSING
// ============================================================================
//
// Types barrel — one import for all pure type definitions.
// Internal DAG: core (0) <- health <- foundation, data <- mod.
// No runtime code. No circular dependencies. Pure contracts.
//
// "And God saw every thing that he had made, and, behold,
//  it was very good." — Genesis 1:31
// ============================================================================
