// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/engine/mod.ts
// key:     B-tov-cws-struct-lib-engine-mod
// title:   CWS Struct — Engine Barrel
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-17
// authors: Nova Dawn (CPI-SI)
// purpose: Re-export all engine modules — discovery, output, registry.
//          The runtime machinery that drives format handlers.
//
// ============================================================================

// Discovery
export { discoverFiles, discoverAllFiles, relativePaths, setDiscoveryLimits } from "./discovery.ts";

// Output
export {
  COLORS,
  severityColor,
  severityIcon,
  printFileSummary,
  printTotals,
  printHeader,
} from "./output.ts";

// Registry
export {
  registerFormat,
  getFormat,
  listFormats,
  listFormatDetails,
  detectFormat,
} from "./registry.ts";

// Fill engine (schema-driven file generation)
export type { FillContext } from "./fill.ts";
export { generateFile, generateFileText } from "./fill.ts";

// Transpiler (three-cord format conversion)
export type {
  ExtractedIdentity,
  ExtractedSemantics,
  SemanticChunk,
  ChunkItem,
  TranspileOptions,
  TranspileResult,
  FormatFormMapping,
} from "./transpiler-types.ts";

export {
  transpile,
  transpileFile,
  findMapping,
  getTargetForm,
  extractRustSemantics,
  rustNameToGo,
  rustTypeToGo,
  rustImportToGo,
  rustParamsToGo,
} from "./transpiler.ts";

// Transpiler schema-driven mappings (first principles)
export type {
  ResolvedMappings,
  ConceptMappingPair,
  LanguageConcept,
  ConceptPattern,
  FormStructure,
  FormContent,
  FormContentExpectations,
  BlockSections,
  BodyBlockSections,
} from "./transpiler-mappings.ts";
export {
  loadTranspilerMappings,
  loadTranspilerMappingsCached,
  loadFormStructure,
  loadFormStructureCached,
  loadFormContent,
  loadFormContentCached,
  clearFormSchemaCache,
  loadConceptMappings,
  loadConceptMappingsCached,
  loadLanguageConcept,
  clearConceptCache,
  convertType,
  convertImport,
  convertName,
  convertParams,
} from "./transpiler-mappings.ts";

// Transpiler AST (universal concept body translation)
export type { AstNode, BodyTranslation } from "./transpiler-ast.ts";
export {
  translateBody,
  parseSourceBody,
  emitTargetBody,
} from "./transpiler-ast.ts";

// Shared transform pipeline (schema-driven, handler-parameterized)
export type {
  TransformContext,
  TransformPass,
  TransformVerifier,
  CodeRangeDetector,
  CodeMoveSpec,
  SubsectionScaffoldSpec,
  SubsectionReorderSpec,
} from "./transform.ts";
export {
  fixBlockSeparatorWidths,
  fixSubsectionSeparatorWidths,
  convertAsciiToUnicode,
  normalizeSeparatorPrefix,
  reorderClosingZonesPass,
  createCodeMovePass,
  createSubsectionScaffoldPass,
  createSubsectionReorderPass,
  UNIVERSAL_PASSES,
  runTransformPipeline,
} from "./transform.ts";

// Git integration (diff-aware file detection)
export { getChangedFiles } from "./git.ts";

// Structured health logging (TIMESTAMP|ACTION|DELTA|SOURCE|DETAIL)
export {
  formatLogEntry,
  formatLogEntries,
  writeLogHeader,
  writeLogEntries,
  formatTransformLogEntries,
  writeTransformLogEntries,
  emitHealthLog,
} from "./logging.ts";

// Inspect pipeline (parsed structure without lint checks)
export { formatInspectText, formatInspectJson, runInspect } from "./inspect.ts";

// JSON output (machine-readable)
export { emitJson } from "./output.ts";

// Enrichment (rule → error code resolution)
export { resolveErrorCodes } from "./enrich.ts";

// Cascade logic (4-layer pipeline cascade)
export {
  cascadeActions,
  cascadeActionGroups,
  cascadeBlock,
  hasStructuralFailure,
  detectFailedBlocks,
  tagLayer,
} from "./cascade.ts";

export type {
  LayerResult,
  PipelineResult,
} from "./cascade.ts";

// Quick-fix suggestions engine
export type { QuickFixSuggestion } from "./suggest.ts";
export {
  computeQuickFixes,
  printQuickFixes,
  printSuggestSummary,
} from "./suggest.ts";

// Debug / trace diagnostic output
export {
  initDebug,
  isDebug,
  isTrace,
  debugConfig,
  debugPolicy,
  debugDiscovery,
  debugFileStart,
  debugFileEnd,
  debugLayerTransition,
  debugCascade,
  debugHealth,
  traceEnter,
  traceExit,
  traceCheckStart,
  traceCheckEnd,
  traceDetection,
  traceLookup,
  traceAction,
  traceCascadeDecision,
  traceResult,
} from "./debug.ts";

// ============================================================================
// CLOSING
// ============================================================================
//
// One import path for all engine machinery.
// "Seek, and ye shall find." — Matthew 7:7
// ============================================================================
