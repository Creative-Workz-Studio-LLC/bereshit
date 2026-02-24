// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/engine/transpiler-types.ts
// key:     B-tov-cws-struct-lib-engine-transpiler-types
// title:   CWS Struct — Transpiler Types (Semantic IR)
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-21
// authors: Nova Dawn (CPI-SI)
// purpose: Type definitions for the three-cord transpiler. The Semantic IR
//          (intermediate representation) sits between source extraction and
//          target generation. Source → IR → Target. The IR captures what a
//          4-block file MEANS — not its syntax.
//
// biblical_foundation: "There is one body, and one Spirit, even as ye are
//   called in one hope of your calling." — Ephesians 4:4
//   One structure, many expressions. The IR is the one body.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import type { CodeFormat } from "../foundation/mod.ts";
import type { CompositionTarget } from "../foundation/mod.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// 1. Identity — extracted from METADATA block
// ---------------------------------------------------------------------------

/** Extracted identity from a source file's METADATA block. */
export interface ExtractedIdentity {
  /** OmniCode key (I1.key), e.g., "B-tov-my-crate-lib" */
  key: string;
  /** File format (I1.format), e.g., "rust" */
  format: CodeFormat;
  /** File path (I1.from) */
  from: string;
  /** Version (I1.at), e.g., "a-01.00" */
  version: string;
  /** File title (I3.title), e.g., "My Library" */
  title: string;
  /** File purpose (I3.purpose) */
  purpose: string;
  /** Detected subtype, e.g., "module", "library", "executable" */
  subtype: string;
  /** Raw pragma directive, e.g., "//omni:code --rust -module" */
  pragma: string;
  /** All extracted fields for passthrough (field → value) */
  raw: Map<string, string>;
}

// ---------------------------------------------------------------------------
// 2. Semantic chunks — extracted from SETUP/BODY/CLOSING blocks
// ---------------------------------------------------------------------------

/**
 * A semantic chunk — one logical unit of code from a source section.
 *
 * Chunks are the atoms of transpilation. A `use` block, a `struct` definition,
 * a function — each becomes a chunk. The transpiler doesn't parse AST; it
 * captures structural meaning at the section level.
 */
export interface SemanticChunk {
  /** Source section tag, e.g., "Imports", "CoreTypes", "CoreLogic" */
  sectionTag: string;
  /** Source block, e.g., "SETUP", "BODY", "CLOSING" */
  sourceBlock: "SETUP" | "BODY" | "CLOSING";
  /** Target section tag in destination format (mapped by section map) */
  targetTag: string;
  /** Raw source lines (for reference / doc comments) */
  sourceLines: string[];
  /** Extracted items — the semantic content */
  items: ChunkItem[];
}

/**
 * A single item within a semantic chunk.
 *
 * The transpiler operates at this granularity: each item is one "thing"
 * that maps from source to target format. The item knows what it IS
 * (kind) and carries enough info to generate a stub in the target.
 */
export interface ChunkItem {
  /** What kind of construct: import, type, function, constant, etc. */
  kind: ItemKind;
  /** Name of the construct (e.g., function name, type name) */
  name: string;
  /** Visibility: public/private (Rust pub → Go Uppercase) */
  visibility: "public" | "private";
  /** Doc comment lines (/// in Rust, // in Go) */
  docComment: string[];
  /** The raw source lines for this item */
  sourceLines: string[];
  /** Additional metadata (varies by kind) */
  meta: ItemMeta;
}

/** The kinds of items the transpiler recognizes. */
export type ItemKind =
  | "import"
  | "constant"
  | "variable"
  | "type_alias"
  | "struct"
  | "enum"
  | "trait"          // Rust trait → Go interface
  | "interface"      // Go interface (target)
  | "function"
  | "method"         // impl method
  | "constructor"    // New* function
  | "error_type"
  | "closing_zone"   // Cv, Ce, Cc, X1-X6
  | "comment_block"  // Section-level documentation
  | "unknown";

/** Metadata bag — different item kinds carry different details. */
export interface ItemMeta {
  /** For functions/methods: parameter signature (source syntax) */
  params?: string;
  /** For functions/methods: return type (source syntax) */
  returnType?: string;
  /** For types: the type this implements/extends */
  implements?: string;
  /** For methods: the receiver type */
  receiver?: string;
  /** For imports: the module/package path */
  importPath?: string;
  /** For constants: the value expression */
  value?: string;
  /** For closing zones: the zone tag (Cv, X1, etc.) */
  zoneTag?: string;
  /** For closing zones: field lines (Never:, Note:, etc.) */
  zoneFields?: string[];
  /** For functions/methods: captured body lines (inside braces). */
  bodyLines?: string[];
  /** For functions/methods: translated body lines (target language). Set by AST pass. */
  translatedBody?: string[];
  /** For functions/methods: Go packages used in translated body. Set by AST pass. */
  bodyPackagesUsed?: string[];
}

// ---------------------------------------------------------------------------
// 3. Section mapping — how source sections map to target sections
// ---------------------------------------------------------------------------

/** Maps source section tags to target section tags. */
export interface SectionMapping {
  /** Source section tag → target section tag */
  setup: Map<string, string>;
  body: Map<string, string>;
  closing: Map<string, string>;
}

// ---------------------------------------------------------------------------
// 4. Extracted semantics — full IR from a source file
// ---------------------------------------------------------------------------

/**
 * Complete semantic extraction from a source file.
 * This is the IR — everything the transpiler knows about the source.
 */
export interface ExtractedSemantics {
  /** Source identity */
  identity: ExtractedIdentity;
  /** Source composition target (what recipe built this file) */
  sourceTarget: CompositionTarget;
  /** All extracted chunks, organized by block */
  chunks: SemanticChunk[];
  /** Source file path */
  sourceFile: string;
  /** Source line count */
  sourceLineCount: number;
}

// ---------------------------------------------------------------------------
// 5. Transpile options and result
// ---------------------------------------------------------------------------

/** Options for a transpilation run. */
export interface TranspileOptions {
  /** Source file path — the file to transpile */
  sourceFile: string;
  /** Source format, e.g., "rust" */
  sourceFormat: CodeFormat;
  /** Target format, e.g., "go" */
  targetFormat: CodeFormat;
  /** Source form, e.g., "module" — auto-detected from pragma if not given */
  sourceForm?: string;
  /** Target form, e.g., "library" — mapped from source form if not given */
  targetForm?: string;
  /** Preview mode — output to stdout, don't write file */
  dryRun: boolean;
  /** Output file path (if not given, derived from source path) */
  outputPath?: string;
  /** Override key for the generated file */
  key?: string;
  /** Override title for the generated file */
  title?: string;
}

/** Result of a transpilation. */
export interface TranspileResult {
  /** Whether transpilation succeeded */
  success: boolean;
  /** Source file path */
  sourceFile: string;
  /** Target file path (if written) */
  targetFile?: string;
  /** Generated output as string */
  output: string;
  /** Line count of generated file */
  lineCount: number;
  /** Diagnostic messages */
  diagnostics: TranspileDiagnostic[];
  /** Section mapping used */
  sectionMapping: SectionMapping;
  /** Statistics */
  stats: TranspileStats;
}

/** A diagnostic message from the transpiler. */
export interface TranspileDiagnostic {
  severity: "error" | "warn" | "info";
  message: string;
  /** Source line number (if applicable) */
  sourceLine?: number;
}

/** Statistics about a transpilation. */
export interface TranspileStats {
  /** Chunks extracted from source */
  chunksExtracted: number;
  /** Items extracted from source */
  itemsExtracted: number;
  /** Sections mapped to target */
  sectionsMapped: number;
  /** Items that got stubs (TODO body) */
  stubsGenerated: number;
  /** Items that transferred directly (comments, constants) */
  directTransfers: number;
}

// ---------------------------------------------------------------------------
// 6. Format-specific mapping registry
// ---------------------------------------------------------------------------

/**
 * A format mapping defines how one format's forms relate to another's.
 *
 * The mapping lives at the form level — "rust module → go library" —
 * because different forms have different section inventories.
 */
export interface FormatFormMapping {
  /** Source format */
  sourceFormat: CodeFormat;
  /** Source form */
  sourceForm: string;
  /** Target format */
  targetFormat: CodeFormat;
  /** Target form */
  targetForm: string;
  /** Section mapping (source tag → target tag) */
  sections: SectionMapping;
  /** Notes about the mapping — why these map to each other */
  notes: string;
}

// ============================================================================
// CLOSING
// ============================================================================
//
// The Semantic IR is the bridge between formats. Source extraction produces
// IR; target generation consumes IR. The IR doesn't care about syntax — it
// captures what the code MEANS structurally.
//
// Rust `pub fn new()` and Go `func New()` are the same THING — a public
// constructor. The IR captures "constructor, public, name: new/New" and
// lets each format express it in its own syntax.
//
// "There is one body, and one Spirit." — Ephesians 4:4
// ============================================================================
