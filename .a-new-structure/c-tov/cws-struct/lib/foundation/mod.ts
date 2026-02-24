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
  LintLayer,
  BlockName,
  LintGridOptions,
  FixAction,
  FixSuggestion,
  LintResult,
  LintSummary,
  AnatomyBlock,
  AnatomySection,
  ParsedAnatomy,
  InspectBlock,
  InspectSection,
  SubsectionDef,
  InspectContent,
  InspectResult,
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
  setLintFocus,
  getLintFocus,
  summarize,
} from "./types.ts";

// Health scoring
export type {
  HebrewState,
  HebrewDirection,
  AtomicAction,
  ContainerScore,
  BlockScore,
  HealthScore,
} from "./health.ts";

export {
  IMPACT_WEIGHT,
  setImpactWeights,
  TRITE_POSITIONS,
  trueToLevel,
  levelToEmoji,
  levelToDirection,
  normalizeBase50,
  encodeHealthTrite,
  decodeHealthTrite,
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
  TypingProfile,
  TypingBlockProfile,
  // Form constraint types
  FormSectionConstraint,
  FormReservedSection,
  FormContainerConstraints,
  FormConstraints,
} from "./code-schema.ts";

export {
  assembleCodeRules,
  loadCodeRules,
  clearCodeCache,
  // Form schema loader
  parseFormSchema,
  loadFormConstraints,
  clearFormCache,
  // Composition bridge
  compositionToFormConstraints,
  loadCompositionFormConstraints,
  // Registry fallback
  buildFormConstraintsFromRegistry,
} from "./code-schema.ts";

// Tool configuration
export type { ToolConfig } from "./config.ts";
export { loadConfig } from "./config.ts";

// Structured tool errors
export { ToolError } from "./tool-error.ts";

// Cache registry
export { registerCache, clearAllCaches, listCaches } from "./cache-registry.ts";

// Composition loader (three-cord architecture)
export type {
  CompositionParts,
  CompositionSide,
  CompositionBlockSections,
  CompositionActiveSections,
  CompositionTarget,
  LoadedSchema,
  ResolvedComposition,
} from "./composition-loader.ts";

export {
  parseCompositionTarget,
  loadComposition,
  loadCompositionCached,
  listCompositionTargets,
  clearCompositionCache,
} from "./composition-loader.ts";

// ============================================================================
// CLOSING
// ============================================================================
//
// One import path for all foundation contracts.
// "A just weight and balance are the LORD's." — Proverbs 16:11
// ============================================================================
