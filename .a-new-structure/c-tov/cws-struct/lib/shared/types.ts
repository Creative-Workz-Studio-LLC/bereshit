// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/shared/types.ts
// key:     B-tov-cws-struct-lib-shared-types
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

import type { FormatConfig } from "./format-config.ts";

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

// SubsectionDef lives in foundation/types.ts — imported here for local use
// and re-exported for backward compatibility with existing consumers.
import type { SubsectionDef } from "../foundation/types.ts";
export type { SubsectionDef };

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
  /** Typing arrow refinement. When directive says "-module->utility",
   *  typing = "utility". Narrows section expectations from generic form
   *  profile to specific variant. The arrow IS the R[10] → R[5] step. */
  typing: string | null;
  /** Schema-driven format config. When present, check functions use schema
   *  values instead of hardcoded constants. Loaded at handler init. */
  formatConfig?: FormatConfig;
}

// ---------------------------------------------------------------------------
// Constants — the 4-block contract
// ---------------------------------------------------------------------------

/** The 4 blocks in required order. */
export const BLOCKS = ["METADATA", "SETUP", "BODY", "CLOSING"] as const;

/** Patterns that identify a block boundary (comment-based, language-agnostic).
 *  Accepts both bare format (// SETUP) and tagged format (// SETUP BLOCK [SETUP]).
 *  Tags are identity markers — the block declaring what it IS. Machine-parseable,
 *  grep-friendly, self-documenting. Tagged format is preferred; bare is accepted
 *  for backwards compatibility. */
export const BLOCK_PATTERNS: Record<string, RegExp> = {
  METADATA: /^\/\/\s*={4,}\s*$|^\/\/\s+METADATA(\s+BLOCK\s+\[METADATA\])?\s*$/,
  SETUP:    /^\/\/\s+SETUP(\s+BLOCK\s+\[SETUP\])?\s*$/,
  BODY:     /^\/\/\s+BODY(\s+BLOCK\s+\[BODY\])?\s*$/,
  CLOSING:  /^\/\/\s+CLOSING(\s+BLOCK\s+\[CLOSING\])?\s*$/,
};

/** END marker patterns for each block.
 *  Accepts both bare format (// END SETUP) and tagged format (// END SETUP [END]). */
export const END_PATTERNS: Record<string, RegExp> = {
  METADATA: /^\/\/\s+END METADATA(\s+\[END\])?\s*$/,
  SETUP:    /^\/\/\s+END SETUP(\s+\[END\])?\s*$/,
  BODY:     /^\/\/\s+END BODY(\s+\[END\])?\s*$/,
  CLOSING:  /^\/\/\s+END CLOSING(\s+\[END\])?\s*$/,
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
// I/C field requirements — now fully schema-driven
// ---------------------------------------------------------------------------
//
// Formerly contained hardcoded PRAGMA_FIELD_REQUIREMENTS and
// METADATA_FIELD_REQUIREMENTS. Both migrated to schema-driven values:
//   Go handler: _goRules.pragmaFieldRequirements / .metadataFieldRequirements
//   Rust handler: _rustRules.pragmaFieldRequirements / .metadataFieldRequirements
// Source of truth: code-schema.ts → loadCodeRules()
//

// ---------------------------------------------------------------------------
// Content validation — field value patterns and known values
// ---------------------------------------------------------------------------
//
// Structure → Existence → Content. Each layer stands on the previous.
// Existence checks ask "is the field present?" (validateICFields).
// Content checks ask "is the value valid?" (validateICFieldContent).
//

/** Living versioning format: stage-phase.completion (e.g., a-01.00, b-02.50) or "template". */
export const VERSION_PATTERN = /^([abc]-\d{2}\.\d{2}|template)$/;

/** Key format: capital letter, dash, then identifier segments (e.g., B-my-project). */
export const KEY_PATTERN = /^[A-Z]-[a-zA-Z0-9][a-zA-Z0-9_.-]*$/;

/** Date format: YYYY-MM-DD. */
export const DATE_PATTERN = /^\d{4}-\d{2}-\d{2}$/;

/** Known format identifiers (stored lowercase for case-insensitive comparison). */
export const KNOWN_FORMATS = new Set([
  "go", "rust", "c", "typescript", "python", "bash",
  "toml", "json", "jsonc", "yaml",
  "adoc", "asciidoc", "md", "markdown",
  "makefile", "dockerfile",
  "editorconfig", "gitignore", "gitmessage", "env",
]);

/** Known type values (stored lowercase). */
export const KNOWN_TYPES = new Set(["code", "data", "documentation"]);

/** Known structure values (stored lowercase). */
export const KNOWN_STRUCTURES = new Set(["3-block", "4-block", "5-block"]);

/** Known status values (stored lowercase). */
export const KNOWN_STATUSES = new Set([
  "active", "draft", "deprecated", "archived", "planned", "template",
]);

/** A content validation rule: what field, what check, what severity. */
export interface FieldContentRule {
  /** Section.field path (e.g., "I1.key", "C1.version"). */
  field: string;
  /** Validation type. */
  check: "pattern" | "enum" | "non-empty" | "path-like";
  /** For pattern checks: the regex to match. */
  pattern?: RegExp;
  /** For enum checks: the set of known valid values (lowercase). */
  values?: ReadonlySet<string>;
  /** Severity when validation fails. */
  severity: "error" | "warn" | "info";
  /** Human-readable message describing what the field should be. */
  message: string;
}

/**
 * Content validation rules for PRAGMA (Identity I1-I4) fields.
 * Applied only to fields that exist and aren't placeholders.
 */
export const PRAGMA_CONTENT_RULES: readonly FieldContentRule[] = [
  { field: "I1.key", check: "pattern", pattern: KEY_PATTERN, severity: "warn",
    message: "should match pattern B-{identifier} (e.g., B-my-project-name)" },
  { field: "I1.format", check: "enum", values: KNOWN_FORMATS, severity: "warn",
    message: "should be a known format (go, rust, c, toml, etc.)" },
  { field: "I1.from", check: "path-like", severity: "info",
    message: "should be a path containing /" },
  { field: "I1.at", check: "pattern", pattern: VERSION_PATTERN, severity: "warn",
    message: "should follow living versioning: a-01.00, b-02.50, c-01.00" },
  { field: "I2.type", check: "enum", values: KNOWN_TYPES, severity: "warn",
    message: "should be: code, data, or documentation" },
  { field: "I2.structure", check: "enum", values: KNOWN_STRUCTURES, severity: "warn",
    message: "should be: 3-block, 4-block, or 5-block" },
  { field: "I3.file", check: "non-empty", severity: "warn",
    message: "should not be empty" },
  { field: "I3.title", check: "non-empty", severity: "warn",
    message: "should not be empty" },
];

/**
 * Content validation rules for METADATA (Context C1-C7) fields.
 * Applied only to fields that exist and aren't placeholders.
 */
export const METADATA_CONTENT_RULES: readonly FieldContentRule[] = [
  { field: "C1.version", check: "pattern", pattern: VERSION_PATTERN, severity: "warn",
    message: "should follow living versioning: a-01.00, b-02.50, c-01.00" },
  { field: "C1.status", check: "enum", values: KNOWN_STATUSES, severity: "warn",
    message: "should be: Active, Draft, Deprecated, Archived, Planned, or Template" },
  { field: "C1.created", check: "pattern", pattern: DATE_PATTERN, severity: "info",
    message: "should be a date: YYYY-MM-DD" },
  { field: "C1.updated", check: "pattern", pattern: DATE_PATTERN, severity: "info",
    message: "should be a date: YYYY-MM-DD" },
  { field: "C2.organization", check: "non-empty", severity: "warn",
    message: "should not be empty" },
  { field: "C3.scripture", check: "non-empty", severity: "info",
    message: "should contain a scripture reference" },
];

// ---------------------------------------------------------------------------
// BODY subsection patterns
// ---------------------------------------------------------------------------

/** Numeric subsection pattern: `// 1. Name` or `// 1 Name`.
 *  Uses \s{1,2} to match headers (1-2 spaces) but NOT overview TOC lines (3+ spaces). */
export const BODY_SUBSECTION_PATTERN = /^\/\/\s{1,2}(\d+)\.?\s+(.+)/;

/** Legacy subsection pattern: `// §1 — Name`. */
export const BODY_SUBSECTION_LEGACY = /^\/\/\s{1,2}§(\d+)\s*[—–-]\s*(.+)/;

// ---------------------------------------------------------------------------
// Scaling thresholds — block size warnings
// ---------------------------------------------------------------------------
//
// CLOSING zone definitions, documentation requirements, and field patterns
// are now entirely schema-driven via code-schema.ts (SchemaClosingZone,
// SchemaClosingDocReq, closingData.fieldPatterns). The hardcoded constants
// CLOSING_ZONES, CLOSING_DOC_REQUIREMENTS, X1_FIELD_PATTERNS, X5_FIELD_PATTERNS,
// ClosingZoneDef, and ClosingDocRequirement were removed 2026-02-19.
// ---------------------------------------------------------------------------

/**
 * Lines-of-code thresholds per block.
 * Source: base-4block-schema.jsonc $defs.setup_subsections.production_design.scaling_signal
 * and $defs.body_zones.production_design.scaling_signal
 */
export const SCALING_THRESHOLDS = {
  SETUP: 200,
  BODY: 500,
} as const;

// ---------------------------------------------------------------------------
// LanguageAdapter — the generalization interface
// ---------------------------------------------------------------------------

/**
 * Language-specific adapter for 4-block code handlers.
 *
 * Captures what differs between Go and Rust (and future C, TypeScript, etc.)
 * while everything else is shared. A future GenericCode4BlockHandler would
 * take a LanguageAdapter + Code4BlockRules and produce a full handler.
 *
 * Today: adapters are defined alongside existing handlers. The handlers
 * still use their own functions directly. The adapter packages them for
 * a future that's open by design:
 *
 *   new format = new LanguageAdapter + new schema. Not new engine code.
 */
export interface LanguageAdapter {
  /** Format name — matches schema key and handler registration. */
  readonly format: import("../foundation/code-schema.ts").CodeFormat;

  /** File extensions this language handles (e.g., [".go"], [".rs"]). */
  readonly extensions: readonly string[];

  /** Known //omni:code directive patterns (e.g., "--go -library"). */
  readonly knownCodeDirectives: readonly string[];

  /**
   * Classify a raw source line into a content kind string.
   *
   * Go: "package_decl", "import_decl", "func_decl", etc.
   * Rust: "use_decl", "struct_decl", "impl_block", etc.
   *
   * The returned string is used for content placement checking —
   * each kind maps to a block and/or subsection via the schema's
   * content_kind_mapping.
   */
  classifyLine(rawLine: string): string;

  /**
   * Parse identity fields from a structured variable/static declaration.
   *
   * Go: [][2]string slice syntax — `Pragma = [][2]string{ {"I1.key", "..."}, ... }`
   * Rust: &[(&str, &str)] static syntax — `static PRAGMA: &[(&str, &str)] = &[("I1.key", "...")];`
   */
  parseIdentityFields(lines: string[], varName: string): IdentityField[];

  /**
   * Find //omni: directives in the file.
   *
   * Each language has different scanning rules — Go stops at block markers
   * or code, Rust stops at block markers, use statements, or pub items.
   * Returns a Map<directiveName, DirectiveInfo>.
   */
  findOmniDirectives(lines: string[]): Map<string, DirectiveInfo>;

  /**
   * Find the test zone within a line range.
   *
   * Go: looks for `func Test...` declarations.
   * Rust: looks for `#[cfg(test)]` module attribute.
   *
   * Returns null if no test zone found in the range.
   */
  findTestZone(
    lines: string[],
    rangeStart: number,
    rangeEnd: number,
  ): { start: number; end: number } | null;

  /**
   * Find the main function zone within a line range.
   *
   * Go: looks for `func main()`.
   * Rust: looks for `fn main()`.
   *
   * Returns null if no main zone found in the range.
   */
  findMainZone(
    lines: string[],
    rangeStart: number,
    rangeEnd: number,
  ): { start: number; end: number } | null;

  /**
   * Enrich SETUP subsection patterns with legacy alternatives.
   *
   * Go: adds `//--- X.N` prefixes for backward compatibility.
   * Rust: returns subsections as-is (no legacy patterns).
   *
   * If undefined, subsections are used directly from the schema.
   */
  enrichSubsectionPatterns?(subsections: SubsectionDef[]): SubsectionDef[];

  /**
   * Build language-specific context fields.
   *
   * Go: `{ isDocGo: boolean, isTestFile: boolean }`
   * Rust: `{ isCrateRoot: boolean, isModuleFile: boolean }`
   *
   * These are merged into the handler's file context.
   */
  buildContextExtras(
    filePath: string,
    lines: string[],
  ): Record<string, unknown>;
}

// ============================================================================
// CLOSING
// ============================================================================
//
// Shared types and constants — the single source of truth for what 4-block
// structure looks like. When go.ts and rust.ts both need the same interface
// or constant, it lives here. DRY without abstraction gymnastics.
//
// The LanguageAdapter interface opens the generalization path: new format =
// new adapter + new schema. The engine doesn't need to change.
//
// "One Lord, one faith, one baptism." — Ephesians 4:5
// ============================================================================
