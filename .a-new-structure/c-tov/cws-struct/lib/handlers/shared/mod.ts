// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/handlers/shared/mod.ts
// key:     B-tov-cws-struct-lib-handlers-shared-mod
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
  BlockRange, SubsectionDef, ClosingZoneDef, BaseFileContext,
} from "./types.ts";

export {
  BLOCKS, BLOCK_PATTERNS, END_PATTERNS,
  REQUIRED_DIRECTIVES, RECOMMENDED_DIRECTIVES,
  BLOCK_SEPARATOR_WIDTH, SUBSECTION_SEPARATOR_WIDTH,
  PRAGMA_FIELD_REQUIREMENTS, METADATA_FIELD_REQUIREMENTS,
  BODY_SUBSECTION_PATTERN, BODY_SUBSECTION_LEGACY,
  CLOSING_ZONES,
} from "./types.ts";

// Shared functions
export {
  findBlocks, getBlockLines, blockLineToFile, findBlockRange,
  getSubsectionRanges,
  checkSeparatorConsistency, checkClosingZoneOrder,
} from "./code-4block.ts";

// ============================================================================
// CLOSING
// ============================================================================
//
// Single import point: `import { findBlocks, BLOCKS } from "./shared/mod.ts"`
// ============================================================================
