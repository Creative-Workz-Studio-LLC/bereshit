// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/transpiler/mod.ts
// key:     B-tov-cws-struct-lib-transpiler-mod
// title:   CWS Struct — Transpiler Barrel
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-24
// authors: Nova Dawn (CPI-SI)
// purpose: Re-export all transpiler modules — types, mappings, AST, orchestrator.
//          The three-cord transpiler: Source → Semantic IR → Target.
//
// ============================================================================

// Types — Semantic IR (intermediate representation)
export type {
  ExtractedIdentity,
  ExtractedSemantics,
  SemanticChunk,
  ChunkItem,
  ItemKind,
  ItemMeta,
  TranspileOptions,
  TranspileResult,
  TranspileDiagnostic,
  TranspileStats,
  FormatFormMapping,
} from "./types.ts";

// Schema-driven mappings (first principles)
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
  ImportMappingEntry,
  GenericTypePattern,
} from "./mappings.ts";
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
} from "./mappings.ts";

// Universal Concept AST (body translation)
export type { AstNode, BodyTranslation } from "./ast.ts";
export {
  translateBody,
  parseSourceBody,
  emitTargetBody,
} from "./ast.ts";

// Three-cord orchestrator
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

// ============================================================================
// CLOSING
// ============================================================================
//
// Transpiler barrel — types, mappings, AST, orchestrator.
// "A threefold cord is not quickly broken." — Ecclesiastes 4:12
// ============================================================================
