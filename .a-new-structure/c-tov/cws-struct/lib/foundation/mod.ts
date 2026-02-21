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
  LintPolicy,
  FixAction,
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
  policySeverity,
  setGlobalPolicy,
  getGlobalPolicy,
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
  IMPACT_WEIGHT,
  computeContainerScore,
  computeBlockScore,
  computeHealthScore,
} from "./health.ts";

// Schema pipeline (layered discovery)
export type {
  SchemaSource,
} from "./schema-pipeline.ts";

export {
  FileSource,
  SchemaPipeline,
  getDefaultPipeline,
  clearPipeline,
} from "./schema-pipeline.ts";

// TOML schema loader
export type {
  FieldRequirement,
  MetadataContract,
  ContentContract,
  ClosingContract,
  ValidationContract,
  DerivationNormalization,
  DerivationSection,
  DerivationLayout,
  PragmaTaxonomy,
  DerivedRules,
} from "./schema.ts";

export {
  parseTomlSchema,
  loadRules,
  clearCache,
} from "./schema.ts";

// Code schema loader (Go/Rust 4-block)
export type {
  SchemaSubsectionDef,
  SchemaBodySubtype,
  SchemaFieldRequirement,
  SchemaDocComment,
  SchemaSubtypeDef,
  PlacementMaps,
  SchemaClosingZone,
  SchemaClosingDocReq,
  SchemaClosingData,
  SchemaIdentitySyntax,
  SchemaFillDefaults,
  SchemaClosingDefaults,
  SchemaFillContent,
  Code4BlockRules,
  CodeFormat,
  // Form constraint types
  FormSectionConstraint,
  FormReservedSection,
  FormContainerConstraints,
  FormConstraints,
} from "./code-schema.ts";

export {
  parseCodeSchema,
  loadCodeRules,
  clearCodeCache,
  // Form schema loader
  parseFormSchema,
  loadFormConstraints,
  clearFormCache,
} from "./code-schema.ts";

// ============================================================================
// CLOSING
// ============================================================================
//
// One import path for all foundation contracts.
// "A just weight and balance are the LORD's." — Proverbs 16:11
// ============================================================================
