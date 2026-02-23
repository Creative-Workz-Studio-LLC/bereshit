// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/data/mod.ts
// key:     B-tov-cws-struct-lib-data-mod
// title:   CWS Struct — Data Layer Barrel
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-22
// updated: 2026-02-22
// authors: Nova Dawn (CPI-SI)
// purpose: Re-export all data layer types, registries, and accessors.
//          Single import path: import { ... } from "../data/mod.ts";
//
//          The data layer is the compiled expression of schema truth.
//          Handlers import from here — they never touch schemas directly.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// ---------------------------------------------------------------------------
// Core types
// ---------------------------------------------------------------------------

export type {
  TernaryValue,
  BlockName,
  SectionEntry,
  ConceptEntry,
  FormDefinition,
  PlacementRule,
  HebrewStateDefinition,
  ErrorCodeEntry,
  PipelineLayer,
  LogEntry,
  DataLayer,
} from "./types.ts";

export {
  isValidTernary,
  isValidBlock,
  isValidFormStatus,
  isValidLayer,
  isValidWeight,
  validateConceptMap,
  verifyTranspose,
} from "./types.ts";

// ---------------------------------------------------------------------------
// Registries (populated data — added as they're built)
// ---------------------------------------------------------------------------

// Phase 0.2: Section + form registries
export { SECTION_REGISTRY, SECTION_ORDER } from "./sections.ts";
export { FORM_REGISTRY } from "./forms.ts";

// Phase 0.3: Concept + placement registries
export { CONCEPT_REGISTRY, CONCEPT_ORDER } from "./concepts.ts";
export { PLACEMENT_REGISTRY, METADATA_FORBIDDEN } from "./placement.ts";

// Phase 0.4: Hebrew state + weight definitions
// export { STATE_DEFINITIONS } from "./states.ts";

// Phase 1: Error code catalog + 3D query infrastructure
export {
  LINT_RULES,
  ERROR_CODES,
  getByRule,
  getByCode,
  matchRule,
  getByLayer,
  getByCategory,
  getByDomain,
  query,
  catalogStructure,
  categoryToDomain,
  dimensionalCode,
  CATEGORY_DOMAIN,
  DOMAIN_CODES,
  CATALOG_SIZE,
  LAYER_COUNTS,
} from "./errors.ts";

// Phase 2: SQLite persistence
// NOTE: CwsDatabase and detectInstance are NOT re-exported here because
// database.ts triggers SQLite FFI loading at module level. Consumers that
// need the database should import directly from "./database.ts" or use
// lazy dynamic imports to avoid crashing commands that don't need FFI.
//
// Type-only exports are safe — they're erased at runtime:
export type {
  FileRecord,
  LintRunRecord,
  HealthHistoryRecord,
  PatternRecord,
  FileHealthReport,
  ActionRecord,
  RunDetailsReport,
  InstanceRecord,
  SessionRecord,
  PatternDetailReport,
  InstanceActivityReport,
  SessionSummaryReport,
  DetectedInstance,
} from "./database.ts";

// Phase 4: Pipeline layer definitions
export {
  PIPELINE_LAYERS,
  LAYER_BY_NUMBER,
  LAYER_BY_RESOLUTION,
  getCheckLayer,
  PIPELINE_CHECK_COUNT,
} from "./pipeline.ts";

// Phase 5: Schema-driven format rules (R50_codefile)
export type {
  SeparatorRule,
  BlockBoundaryRules,
  PragmaRules,
  IdentityFieldRules,
  ClosingZoneRules,
  ReservedOmissionRules,
  UniversalFormatRules,
  CommentSyntax,
  PragmaExpression,
  IdentityConstruct,
  LanguageFormatRules,
  ComposedFormatRules,
} from "./format-rules.ts";

export {
  loadUniversalFormatRules,
  loadLanguageFormatRules,
  loadFormatRules,
  clearFormatCache,
} from "./format-rules.ts";

// Phase 5: Schema-driven non-code categories (R50_codefile)
export type {
  ChunkCategory,
  ChunkingRule,
  LanguageExtensionContract,
  UniversalNonCode,
  LanguageNonCode,
  ComposedNonCode,
} from "./non-code.ts";

export {
  loadUniversalNonCode,
  loadLanguageNonCode,
  loadNonCode,
  clearNonCodeCache,
} from "./non-code.ts";

// Phase 6: Schema-driven concept detectors (R5_patterns)
export type {
  ConceptPattern,
  ConceptDetector,
} from "./concept-detectors.ts";

export {
  detectConcept,
  matchConceptPattern,
  loadConceptDetectors,
  getConceptDetector,
  detectCoverage,
  clearConceptDetectorCache,
} from "./concept-detectors.ts";

// ============================================================================
// BODY
// ============================================================================

// (No body logic — barrel file. Registries will be exported as they're built.)

// ============================================================================
// CLOSING
// ============================================================================
//
// Data layer barrel — one import path for all compiled schema truth.
// Registry exports are commented out until their respective phases complete.
// Each uncomment is a checkpoint — the data layer grows by addition.
//
// "The entrance of thy words giveth light; it giveth understanding
//  unto the simple." — Psalm 119:130
// ============================================================================
