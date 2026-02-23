// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/shared/mod.ts
// key:     B-tov-cws-struct-lib-shared-mod
// title:   CWS Struct — Shared 4-Block Module Barrel
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-18
// authors: Nova Dawn (CPI-SI)
// purpose: Re-exports all shared types, constants, and functions for 4-block
//          code handlers. Import from here, not from individual files.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// (Nothing — this is a barrel module.)

// ============================================================================
// BODY
// ============================================================================

// Types and constants
export type {
  BlockPosition, DirectiveInfo, SubsectionRange, IdentityField,
  BlockRange, SubsectionDef, BaseFileContext,
  FieldContentRule,
  LanguageAdapter,
} from "./types.ts";

export {
  BLOCKS, BLOCK_PATTERNS, END_PATTERNS,
  REQUIRED_DIRECTIVES, RECOMMENDED_DIRECTIVES,
  BLOCK_SEPARATOR_WIDTH, SUBSECTION_SEPARATOR_WIDTH,
  PRAGMA_CONTENT_RULES, METADATA_CONTENT_RULES,
  VERSION_PATTERN, KEY_PATTERN, DATE_PATTERN,
  KNOWN_FORMATS, KNOWN_TYPES, KNOWN_STRUCTURES, KNOWN_STATUSES,
  BODY_SUBSECTION_PATTERN, BODY_SUBSECTION_LEGACY,
  SCALING_THRESHOLDS,
} from "./types.ts";

// Lint grid orchestration — block × layer
export type {
  CheckFn, ContainerCheckSet, BlockCheckSet, BlockLintChain,
} from "./code-4block.ts";

export { runLintGrid } from "./code-4block.ts";

// Shared functions
export {
  findBlocks, getBlockLines, blockLineToFile, findBlockRange,
  getSubsectionRanges, getContainerLines,
  checkSeparatorConsistency, checkClosingZoneOrder,
  checkClosingRequiredZones, checkClosingZoneContent,
  checkClosingX6TemplateOnly, checkClosingDocFieldContent,
  checkSetupSubsectionOrder, checkBodySubsectionOrder, checkScalingSignals,
  checkRequiredSetupSubsections, checkRequiredBodySubsections, checkSubtypeEmphasis,
  validateICFields,
  validateICFieldContent,
  loadCompiledNonCodePatterns, clearCompiledPatternsCache,
  parseReservedOmissions,
  validateTemplateVsDerived, validateSubtypeConsistency, validateFormatConsistency,
  // Content linting — schema-driven content expectations
  checkBodyContentExpectations,
  checkClosingContentExpectations,
  checkSetupContentExpectations,
  checkMetadataContentExpectations,
} from "./code-4block.ts";

// Content expectations type
export type { ContentExpectations } from "./code-4block.ts";

// Concept validation — R[5] per-container concept checks
export type { ConceptCheckOptions } from "./concept-check.ts";
export {
  validateContainerConcepts,
  buildConceptContainers,
} from "./concept-check.ts";

// Format config — schema-driven replacement for hardcoded constants
export type {
  FormatConfig, FormatConfigGap,
} from "./format-config.ts";

export {
  loadFormatConfig,
  defaultFormatConfig,
  hasFormatGaps,
  formatGapReport,
  clearFormatConfigCache,
} from "./format-config.ts";

// Chunker — universal code chunking engine
export type { ChunkerConfig } from "./chunker.ts";
export {
  parseChunks,
  isOmniPragma, isSeparatorBanner, isRegularComment,
} from "./chunker.ts";

// Scaffold — universal structural scaffold pipeline
export type {
  SectionEntry, GenericChunk, BlockTarget,
  ScaffoldAdapter, ClosingFieldValues, ScaffoldResult, ScaffoldOptions,
} from "./scaffold.ts";

export {
  // Label + overview helpers
  tagToLabel,
  buildSectionIndex,
  emitSectionIndex,
  emitClosingOverview,
  buildFormAwareReservedOmission,
  SECTION_DESCRIPTIONS,
  CLOSING_ZONE_LABELS,
  CLOSING_ZONE_DESCRIPTIONS,
  // Closing field preservation
  extractClosingFields,
  // Block structure stripping (for --force re-scaffold)
  stripBlockStructure,
  // METADATA block generation (universal, schema-driven)
  buildMetadataBlock,
  // Structural scaffold orchestrator (universal R[50] → R[5] pipeline)
  structuralScaffold,
} from "./scaffold.ts";

// ============================================================================
// CLOSING
// ============================================================================
//
// Single import point: `import { findBlocks, BLOCKS } from "./shared/mod.ts"`
// ============================================================================
