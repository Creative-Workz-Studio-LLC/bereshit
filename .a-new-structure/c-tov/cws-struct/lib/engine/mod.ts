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
// purpose: Re-export all engine modules — organized by concern layers that
//          mirror the R[n] architecture:
//
//          discover/  → REG domain — find files and formats (structural foundation)
//          pipeline/  → PIP domain — cascade results through R[n] layers
//          present/   → Output domain — human-facing presentation
//          (root)     → Operations + cross-cutting utilities
//          transpiler → TRN domain — extracted to lib/transpiler/
//
// ============================================================================

// ─── discover/ — REG domain: find files and formats ─────────────────────────

export { discoverFiles, discoverAllFiles, relativePaths, setDiscoveryLimits } from "./discover/files.ts";
export {
  registerFormat,
  getFormat,
  listFormats,
  listFormatDetails,
  detectFormat,
} from "./discover/registry.ts";

// ─── pipeline/ — PIP domain: cascade results through R[n] layers ────────────

export { resolveErrorCodes } from "./pipeline/enrich.ts";
export {
  cascadeActions,
  cascadeActionGroups,
  cascadeBlock,
  hasStructuralFailure,
  detectFailedBlocks,
  tagLayer,
} from "./pipeline/cascade.ts";
export type {
  LayerResult,
  PipelineResult,
} from "./pipeline/cascade.ts";

// ─── present/ — Output domain: human-facing presentation ────────────────────

export {
  COLORS,
  severityColor,
  severityIcon,
  printFileSummary,
  printTotals,
  printHeader,
  emitJson,
} from "./present/output.ts";
export {
  formatLogEntry,
  formatLogEntries,
  writeLogHeader,
  writeLogEntries,
  formatTransformLogEntries,
  writeTransformLogEntries,
  emitHealthLog,
} from "./present/logging.ts";
export type { QuickFixSuggestion } from "./present/suggest.ts";
export {
  computeQuickFixes,
  printQuickFixes,
  printSuggestSummary,
} from "./present/suggest.ts";

// ─── root — Operations: fill, transform, inspect, git ───────────────────────

export type { FillContext } from "./fill.ts";
export { generateFile, generateFileText } from "./fill.ts";

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

export { formatInspectText, formatInspectJson, runInspect } from "./inspect.ts";
export { getChangedFiles } from "./git.ts";

// ─── root — T00 cross-cutting: debug/trace ──────────────────────────────────

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

// ─── transpiler — TRN domain: extracted to lib/transpiler/ ──────────────────

export type {
  ExtractedIdentity,
  ExtractedSemantics,
  SemanticChunk,
  ChunkItem,
  TranspileOptions,
  TranspileResult,
  FormatFormMapping,
  ResolvedMappings,
  ConceptMappingPair,
  LanguageConcept,
  ConceptPattern,
  FormStructure,
  FormContent,
  FormContentExpectations,
  BlockSections,
  BodyBlockSections,
  AstNode,
  BodyTranslation,
} from "../transpiler/mod.ts";
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
  translateBody,
  parseSourceBody,
  emitTargetBody,
} from "../transpiler/mod.ts";

// ============================================================================
// CLOSING
// ============================================================================
//
// Engine barrel — organized by concern layers mirroring R[n] architecture.
// discover/ (REG) → pipeline/ (PIP) → present/ (output) → operations (root)
// "Seek, and ye shall find." — Matthew 7:7
// ============================================================================
