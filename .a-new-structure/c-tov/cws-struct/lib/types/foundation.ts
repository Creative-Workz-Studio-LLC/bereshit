// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/types/foundation.ts
// key:     B-tov-cws-struct-lib-types-foundation
// title:   CWS Struct — Foundation Types (Lint, Handler, CLI)
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-24
// authors: Nova Dawn (CPI-SI)
// purpose: Pure type definitions extracted from foundation/types.ts.
//          Lint results, format handler contract, CLI options, anatomy.
//          Runtime functions (policySeverity, error, warn, info, summarize,
//          global state setters) remain in foundation/types.ts.
//
// biblical_foundation: "A just weight and balance are the LORD's: all the
//   weights of the bag are his work." — Proverbs 16:11
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import type { Severity, LintPolicy, BlockName, LintLayer } from "./core.ts";
import type { HealthScore } from "./health.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Lint Grid Focus — block x layer slicing
// ---------------------------------------------------------------------------

/** Lint grid focus options — which slice of the block x layer grid to execute. */
export interface LintGridOptions {
  /** Vertical slice — only lint this block (+ pre-gate). */
  focusBlock?: BlockName;
  /** Horizontal slice — only lint this layer across all blocks. */
  focusLayer?: LintLayer;
  /** Container slice — only lint this container within the focused block. */
  focusContainer?: string;
}

// ---------------------------------------------------------------------------
// Lint Results
// ---------------------------------------------------------------------------

/** A single atomic action within a fix suggestion (for multi-step code fixes). */
export interface FixAction {
  type: "remove" | "insert";
  /** For remove: first line to remove (1-based). */
  startLine?: number;
  /** For remove: last line to remove (1-based, inclusive). */
  endLine?: number;
  /** For insert: line number to insert after (1-based). */
  afterLine?: number;
  /** For insert: lines of code to add. */
  content?: string[];
}

/** Suggested correction for a lint finding. */
export interface FixSuggestion {
  /** Human description of the fix (e.g., "Add missing [_metadata.I1_core] table"). */
  description: string;
  /** TOML snippet to insert or replace (TOML handler). */
  toml?: string;
  /** Insertion hint (e.g., "after [_metadata]", "in _metadata.I1_core"). */
  location?: string;
  /** Ordered list of atomic actions for code fixes (Go/Rust handlers). */
  actions?: FixAction[];
}

/** Single finding from a lint check. */
export interface LintResult {
  file: string;
  severity: Severity;
  rule: string;
  message: string;
  /** 1-based line number where the finding occurs (if applicable). */
  line?: number;
  /** Optional auto-correction suggestion. */
  fix?: FixSuggestion;
  /** Resolved CWS error code (e.g., "CWS-R50-020"). Populated by engine post-lint. */
  errorCode?: string;
  /** Detection layer name (e.g., "R50", "R25"). Populated alongside errorCode. */
  layerName?: string;
  /** Which block this result belongs to ("metadata"|"setup"|"body"|"closing"|"structural"). */
  block?: string;
  /** Which container (subsection/zone) within the block (e.g., "Imports", "Cv", "X5"). */
  container?: string;
}

// ---------------------------------------------------------------------------
// Parsed Anatomy — what the handler actually sees in the file
// ---------------------------------------------------------------------------

/** A block's physical location in the file. */
export interface AnatomyBlock {
  /** Block name: "METADATA", "SETUP", "BODY", "CLOSING". */
  name: string;
  /** Start line (1-indexed). */
  startLine: number;
  /** End line (1-indexed, inclusive). */
  endLine: number;
  /** Line count. */
  lines: number;
}

/** A section detected within a block. */
export interface AnatomySection {
  /** Section name (e.g., "Imports", "Free Functions", "Cv"). */
  name: string;
  /** Parent block. */
  block: string;
  /** Start line (1-indexed). */
  line: number;
}

/** Parsed file anatomy — what the handler sees before judging. */
export interface ParsedAnatomy {
  /** Directive key-value pairs from pragma/omni parsing. */
  directives: Record<string, string>;
  /** Block positions — physical boundaries. */
  blocks: AnatomyBlock[];
  /** Identity fields grouped by source (PRAGMA, METADATA, etc.). */
  identity: Record<string, Array<{ key: string; value: string }>>;
  /** Detected sections within each block. */
  sections: Record<string, AnatomySection[]>;
  /** Content summary per block (e.g., imports, functions, types found). */
  content?: Record<string, string[]>;
}

/** Aggregated results for one file. */
export interface LintSummary {
  file: string;
  errors: number;
  warnings: number;
  infos: number;
  results: LintResult[];
  /** Computed health score — sections as containers, atomic action scoring. */
  health?: HealthScore;
  /** The //omni: pragma directive from line 1 (e.g. "//omni:code --go -library"). */
  pragma?: string;
  /** Parsed file anatomy — the handler's full view of file structure and content. */
  anatomy?: ParsedAnatomy;
}

// ---------------------------------------------------------------------------
// Format Handler — the contract for adding new formats
// ---------------------------------------------------------------------------

/** Options passed to transform handlers. */
export interface TransformOptions {
  /** Preview mode — report changes without writing. */
  dryRun: boolean;
  /** Scaffold extension sections (I4, C5-C7, _contract, X2-X4, etc.). */
  extensions: boolean;
  /** Force re-scaffold even if file already has block structure. */
  force: boolean;
  /** Step-by-step mode — write each build phase to a .steps/ directory. */
  steps: boolean;
}

/** Block detection entry — what the parser found. */
export interface InspectBlock {
  /** Block name: "METADATA", "SETUP", "BODY", "CLOSING". */
  name: string;
  /** Start line (1-indexed). */
  startLine: number;
  /** End line (1-indexed, inclusive). */
  endLine: number;
  /** Separator line number (if detected). */
  separatorLine?: number;
}

/** Section detection entry — subsection within a block. */
export interface InspectSection {
  /** Section name (e.g., "Imports", "Constants", "CoreTypes"). */
  name: string;
  /** Parent block name. */
  block: string;
  /** Start line. */
  line: number;
  /** Position within block (1-indexed). */
  position: number;
  /** Expected position from schema (if known). */
  expectedPosition?: number;
  /** Whether position matches expected. */
  aligned?: boolean;
}

// ---------------------------------------------------------------------------
// Subsection definitions — how blocks are subdivided
// ---------------------------------------------------------------------------

/** A single entry in a SETUP/BODY subsection ordering table. */
export interface SubsectionDef {
  tag: string;
  pattern: RegExp;
  /** Regex matching ONLY alias names (not canonical). When present and
   *  `pattern` matches, test this regex to detect alias usage and
   *  emit info suggesting the canonical name. */
  aliases?: RegExp;
}

/** Content classification entry. */
export interface InspectContent {
  /** Category (e.g., "function", "import", "type", "const", "var"). */
  category: string;
  /** Count of items in this category. */
  count: number;
  /** Which block(s) they appear in. */
  blocks: string[];
}

/** Result of inspecting a file — what the parser sees, no judgment. */
export interface InspectResult {
  /** File path. */
  filePath: string;
  /** Detected format name. */
  format: string;
  /** Detected subtype (e.g., "library", "executable", "demo-test"). */
  subtype?: string;
  /** Whether this is a template file. */
  isTemplate: boolean;
  /** Total line count. */
  lineCount: number;
  /** Detected blocks. */
  blocks: InspectBlock[];
  /** Detected sections within blocks. */
  sections: InspectSection[];
  /** Content classification. */
  content: InspectContent[];
  /** Detected directives (key=value). */
  directives: Record<string, string>;
  /** OmniCode pragma line (if present). */
  pragma?: string;
  /** Identity fields grouped by source (e.g., "PRAGMA", "METADATA"). */
  identity?: Record<string, Array<{ key: string; value: string }>>;
}

/** Every format registers a handler that satisfies this interface. */
export interface FormatHandler {
  /** Short name used in CLI (e.g., "toml", "json", "makefile"). */
  name: string;

  /** Human-readable description. */
  description: string;

  /** File extensions this handler processes (e.g., [".toml"]). */
  extensions: string[];

  /**
   * Optional basename patterns for files without meaningful extensions
   * (e.g., [".gitignore", ".editorconfig"] for dotfiles).
   */
  basenames?: string[];

  /** Maximum directory walk depth (default: 3). */
  maxDepth?: number;

  /** Lint a single file, return results. */
  lint(filePath: string): Promise<LintResult[]>;

  /**
   * Compute health score for a file from its lint results (optional).
   * Called after lint() with the same file path + results.
   * Each handler implements its own domain-specific scoring algorithm.
   */
  computeHealth?(filePath: string, results: LintResult[]): Promise<HealthScore>;

  /** Transform a single file in place (optional). */
  transform?(filePath: string, opts: TransformOptions): Promise<LintResult[]>;

  /**
   * Inspect a single file — show parsed structure without running checks (optional).
   * Returns a structured representation of what the parser sees: blocks, sections,
   * directives, content classification. "Here's what I see" — no judgment.
   */
  inspect?(filePath: string): Promise<InspectResult>;
}

// ---------------------------------------------------------------------------
// CLI Options — parsed from command line
// ---------------------------------------------------------------------------

export interface CliOptions {
  command: "lint" | "transform" | "create" | "transpile" | "query" | "watch" | "formats" | "verify" | "studio" | "help" | "version";
  format?: string;
  targets: string[];
  verbose: boolean;
  errorsOnly: boolean;
  summaryOnly: boolean;
  dryRun: boolean;
  extensions: boolean;
  json: boolean;
  failFast: boolean;
  /** Force re-scaffold even if file already has block structure. */
  force: boolean;
  /** Step-by-step mode — write each scaffold phase to a .steps/ directory. */
  steps: boolean;
  /** Ternary lint policy: strict (-1) | balanced (0) | growth (+1). Default: balanced. */
  policy: LintPolicy;
  /** Create command: subtype for code generation. */
  subtype?: string;
  /** Create command: OmniCode key. */
  key?: string;
  /** Create command: file title. */
  title?: string;
  /** Create command: file purpose. */
  purpose?: string;
  /** Log path: emit atomic health observations as structured log entries. */
  logPath?: string;
  /** Targeted check — focus on a specific block (metadata, setup, body, closing). */
  check?: string;
  /** Database path: enable SQLite persistence for health tracking. */
  dbPath?: string;
  /** Query subcommand: health, worst, patterns, trend, instance, session, stats. */
  queryCommand?: string;
  /** Debug mode: verbose diagnostic output showing internal state to stderr. */
  debug: boolean;
  /** Trace mode: step-by-step execution log (very verbose) to stderr. */
  trace: boolean;
  /** Inspect mode: show parsed structure without running checks. */
  inspect: boolean;
  /** Why mode: trace each result to its data layer source. */
  why: boolean;
  /** Deep mode (-vv/--deep): show per-action checks within each container. */
  deep: boolean;
  /** Diff-aware: only lint files changed since last commit (or --since ref). */
  changed: boolean;
  /** Git ref for diff-aware linting: `git diff --name-only <since>`. */
  since?: string;
  /** Lint only staged files: `git diff --name-only --cached`. */
  staged: boolean;
  /** Quick-fix suggestion count. undefined = off, 0 = default (5), N = show N. */
  suggest?: number;
  /** Help topic: lint, transform, create, transpile, query, options, layers, testing, witness, examples. */
  helpTopic?: string;
}

// ============================================================================
// CLOSING
// ============================================================================
//
// Foundation types — lint results, handler contract, CLI options.
// Pure type definitions only. Runtime functions remain in foundation/types.ts.
//
// "A just weight and balance are the LORD's." — Proverbs 16:11
// ============================================================================
