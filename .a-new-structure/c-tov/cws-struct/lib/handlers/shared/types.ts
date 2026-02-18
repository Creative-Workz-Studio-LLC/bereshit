// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/handlers/shared/types.ts
// key:     B-tov-cws-struct-lib-handlers-shared-types
// title:   CWS Struct — Shared 4-Block Types & Constants
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-18
// authors: Nova Dawn (CPI-SI)
// purpose: Shared type definitions and constants for all 4-block code handlers
//          (Go, Rust, future C/TypeScript). Extracted from go.ts and rust.ts
//          during Phase 4 production hardening — both handlers were carrying
//          identical copies of these interfaces and constants.
//
//          Types: BlockPosition, DirectiveInfo, SubsectionRange, IdentityField,
//          BlockRange, SubsectionDef, BaseFileContext.
//
//          Constants: BLOCKS, BLOCK_PATTERNS, END_PATTERNS, directives,
//          separator widths, I/C field requirements, body subsection patterns.
//
// biblical_foundation: "One Lord, one faith, one baptism." — Ephesians 4:5
//   One source of truth for what 4-block structure looks like.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// (No imports — this is the foundation that others import from.)

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Types — structural building blocks
// ---------------------------------------------------------------------------

/** Position of a block within the file. */
export interface BlockPosition {
  name: string;
  line: number;       // 1-based line number where the block header appears
  endLine: number;    // 1-based line of END marker, 0 if absent
}

/** A directive found in the file, with its 1-based line number. */
export interface DirectiveInfo {
  value: string;
  line: number;
}

/** Subsection boundary within a block's lines. */
export interface SubsectionRange {
  tag: string;
  /** Index within the block's line array where this subsection starts. */
  startIdx: number;
  /** Index (exclusive) where this subsection ends — next subsection or block end. */
  endIdx: number;
}

/** A parsed field from a Pragma or Metadata identity var/static. */
export interface IdentityField {
  /** Section prefix: "I1", "I2", "C1", etc. */
  section: string;
  /** Field name: "key", "format", "requires.stdlib", etc. */
  field: string;
  /** The string value. */
  value: string;
  /** 1-based line number in the file where this field was found. */
  line: number;
}

/**
 * Content range of a named block — for transform operations.
 * Indices are 0-based, suitable for slicing/splicing the lines array.
 */
export interface BlockRange {
  /** First content line index (after block header separator). */
  contentStart: number;
  /** One past the last content line (the END BLOCKNAME header line). */
  contentEnd: number;
}

/** A single entry in a SETUP subsection ordering table. */
export interface SubsectionDef {
  tag: string;
  pattern: RegExp;
}

/** A single entry in the CLOSING zone ordering table. */
export interface ClosingZoneDef {
  tag: string;
  kind: "code" | "doc";
  pattern: RegExp;
}

// ---------------------------------------------------------------------------
// Base file context — shared fields for all 4-block handlers
// ---------------------------------------------------------------------------

/**
 * Base file context gathered once and passed to all check functions.
 *
 * Each language handler extends this with language-specific fields
 * (e.g., Go adds isDocGo, Rust adds isCrateRoot/isModuleFile).
 */
export interface BaseFileContext {
  filePath: string;
  lines: string[];
  isTemplate: boolean;           // has #!omni template
  hasAnyOmni: boolean;           // any //omni: directives present
  hasAnyBlock: boolean;          // any block markers present
  blocks: BlockPosition[];
  directives: Map<string, DirectiveInfo>;
  subtype: string | null;        // "library" | "executable" | "demo-test" | null
}

// ---------------------------------------------------------------------------
// Constants — the 4-block contract
// ---------------------------------------------------------------------------

/** The 4 blocks in required order. */
export const BLOCKS = ["METADATA", "SETUP", "BODY", "CLOSING"] as const;

/** Patterns that identify a block boundary (comment-based, language-agnostic). */
export const BLOCK_PATTERNS: Record<string, RegExp> = {
  METADATA: /^\/\/\s*={4,}\s*$|^\/\/\s+METADATA\s*$/,
  SETUP:    /^\/\/\s+SETUP\s*$/,
  BODY:     /^\/\/\s+BODY\s*$/,
  CLOSING:  /^\/\/\s+CLOSING\s*$/,
};

/** END marker patterns for each block. */
export const END_PATTERNS: Record<string, RegExp> = {
  METADATA: /^\/\/\s+END METADATA\s*$/,
  SETUP:    /^\/\/\s+END SETUP\s*$/,
  BODY:     /^\/\/\s+END BODY\s*$/,
  CLOSING:  /^\/\/\s+END CLOSING\s*$/,
};

// ---------------------------------------------------------------------------
// Directive constants
// ---------------------------------------------------------------------------

/** Required //omni: directives at top of file. */
export const REQUIRED_DIRECTIVES = [
  "//omni:key",
] as const;

/** Recommended //omni: directives. */
export const RECOMMENDED_DIRECTIVES = [
  "//omni:code",
  "//omni:version",
] as const;

// ---------------------------------------------------------------------------
// Separator widths
// ---------------------------------------------------------------------------

/** Standard block separator width (// ====...====). */
export const BLOCK_SEPARATOR_WIDTH = 76;

/** Standard subsection separator width (// ----...----). */
export const SUBSECTION_SEPARATOR_WIDTH = 74;

// ---------------------------------------------------------------------------
// I/C field requirements — Identity (I1-I4) and Context (C1-C7)
// ---------------------------------------------------------------------------

/**
 * PRAGMA carries Identity (I1-I4).
 * Keys match the section prefix used in vars/statics (e.g., "I1" not "I1_core").
 */
export const PRAGMA_FIELD_REQUIREMENTS: Record<string, { required: string[]; defined: string[] }> = {
  I1: { required: ["key", "format", "from", "at"], defined: [] },
  I2: { required: ["type", "structure"], defined: ["subtype", "role"] },
  I3: { required: ["file", "title"], defined: ["component", "path", "provides", "brief"] },
  I4: { required: [], defined: ["layer", "position", "pattern"] },
};

/**
 * METADATA carries Context (C1-C7).
 * Keys match the section prefix used in vars/statics (e.g., "C1" not "C1_lifecycle").
 */
export const METADATA_FIELD_REQUIREMENTS: Record<string, { required: string[]; defined: string[] }> = {
  C1: { required: ["version", "status"], defined: ["created", "updated"] },
  C2: { required: ["organization"], defined: ["architect", "implementation", "copyright"] },
  C3: { required: ["scripture"], defined: ["principle", "anchor"] },
  C4: { required: ["requires", "consumers"], defined: ["integration", "if_missing"] },
  C5: { required: [], defined: ["purpose", "philosophy"] },
  C6: { required: [], defined: ["current", "planned", "limitations"] },
  C7: { required: [], defined: ["tags", "category", "domain", "paradigm"] },
};

// ---------------------------------------------------------------------------
// BODY subsection patterns
// ---------------------------------------------------------------------------

/** Numeric subsection pattern: `// 1. Name` or `// 1 Name`. */
export const BODY_SUBSECTION_PATTERN = /^\/\/\s+(\d+)\.?\s+(.+)/;

/** Legacy subsection pattern: `// 1 -- Name`. */
export const BODY_SUBSECTION_LEGACY = /^\/\/\s+§(\d+)\s*[—–-]\s*(.+)/;

// ---------------------------------------------------------------------------
// CLOSING zone definitions — Cv/Ce/Cc code zones + X1-X6 doc sections
// ---------------------------------------------------------------------------

/**
 * Canonical CLOSING zone ordering.
 * Code zones must precede documentation sections.
 * Within each tier, zones appear in this order.
 */
export const CLOSING_ZONES: readonly ClosingZoneDef[] = [
  // Code zones (must come first)
  { tag: "Cv", kind: "code", pattern: /^\/\/\s+Cv\s+[—–-]/ },
  { tag: "Ce", kind: "code", pattern: /^\/\/\s+Ce\s+[—–-]/ },
  { tag: "Cc", kind: "code", pattern: /^\/\/\s+Cc\s+[—–-]/ },
  // Documentation sections (must come after all code zones)
  { tag: "X1", kind: "doc", pattern: /^\/\/\s+X1[:\s]/ },
  { tag: "X2", kind: "doc", pattern: /^\/\/\s+X2[:\s]/ },
  { tag: "X3", kind: "doc", pattern: /^\/\/\s+X3[:\s]/ },
  { tag: "X4", kind: "doc", pattern: /^\/\/\s+X4[:\s]/ },
  { tag: "X5", kind: "doc", pattern: /^\/\/\s+X5[:\s]/ },
  { tag: "X6", kind: "doc", pattern: /^\/\/\s+X6[:\s]/ },
];

// ============================================================================
// CLOSING
// ============================================================================
//
// Shared types and constants — the single source of truth for what 4-block
// structure looks like. When go.ts and rust.ts both need the same interface
// or constant, it lives here. DRY without abstraction gymnastics.
//
// "One Lord, one faith, one baptism." — Ephesians 4:5
// ============================================================================
