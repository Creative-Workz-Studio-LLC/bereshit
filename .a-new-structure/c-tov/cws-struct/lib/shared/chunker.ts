// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/shared/chunker.ts
// key:     B-tov-cws-struct-lib-shared-chunker
// title:   CWS Struct — Universal Code Chunking Engine
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-22
// authors: Nova Dawn (CPI-SI)
// purpose: Config-driven code chunking engine — walks source lines, groups them
//          into semantic GenericChunk[] based on a language-specific config.
//
//          Handlers provide:
//            - classifyLine():  kind string for each source line
//            - patterns:        how to detect pragma, doc comments, separators, etc.
//            - balancedKinds:   which kinds trigger delimiter-balanced capture
//            - importKinds:     which kinds trigger import-like grouping
//            - kindToCategory:  map kind string → chunk category string
//
//          The engine provides:
//            - Brace/paren tracking (balanced delimiter capture)
//            - Doc comment and attribute attachment to following item
//            - Contiguous same-kind grouping
//            - Blank/separator/comment grouping
//
//          Extracted from Rust's parseCodeChunks() — same algorithm, now
//          parameterized. Rust and Go both delegate to this engine.
//
// biblical_foundation: "For we are labourers together with God."
//   — 1 Corinthians 3:9
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import type { GenericChunk } from "./scaffold.ts";

// ---------------------------------------------------------------------------
// Types — chunker configuration
// ---------------------------------------------------------------------------

/**
 * Language-specific configuration for the universal chunking engine.
 *
 * Each handler supplies one of these. The engine applies the same walk/group/
 * balance algorithm regardless of language — only the pattern matchers and
 * classifier change.
 */
export interface ChunkerConfig {
  /** Classify a trimmed source line into a language-specific kind string. */
  classifyLine: (trimmed: string) => string;

  /** Structural pattern detectors. */
  patterns: {
    /**
     * Pragma directive detection.
     * Both Go and Rust: `//omni:` or `// #!omni`.
     */
    pragma: (trimmed: string) => boolean;

    /**
     * File-level doc comment.
     * Rust: `//!` crate-level docs.
     * Go: returns false (no file-level doc syntax).
     */
    fileDoc: (trimmed: string) => boolean;

    /**
     * Separator banner line.
     * Universal: `// ───────...` or `// ═══════...` or `// -------...`
     */
    separator: (trimmed: string) => boolean;

    /**
     * Regular comment line (excludes pragma, fileDoc, separator).
     * Used for grouping contiguous comment blocks.
     */
    comment: (trimmed: string) => boolean;

    /**
     * Item-level doc comment that attaches to the NEXT item.
     * Rust: `///` (attaches to next pub item).
     * Go: returns false (Go doc comments are just regular comments).
     */
    itemDoc: (trimmed: string) => boolean;

    /**
     * Attribute/annotation before an item.
     * Rust: `#[derive(...)]`, `#![allow(...)]`, etc.
     * Go: returns false (Go has no attributes).
     */
    attribute: (trimmed: string) => boolean;

    /**
     * Start of a complete test block (captured with delimiter tracking).
     * Rust: `#[cfg(test)]` — captures the entire test module.
     * Go: returns false (Go tests are individual funcs, not blocks).
     */
    testBlock: (trimmed: string) => boolean;
  };

  /**
   * Classified kinds that trigger delimiter-balanced capture.
   *
   * When a line classifies as one of these kinds, the engine captures
   * all lines until delimiters balance (or the declaration terminates).
   * Preceding doc comments and attributes are attached.
   *
   * Supports both `{ }` (Go funcs, Rust items) and `( )` (Go import/const/var).
   * The engine auto-detects which delimiter pair to track from the first
   * open delimiter it encounters.
   *
   * Examples:
   *   Rust: fn_decl, impl_block, struct_decl, enum_decl, trait_decl, ...
   *   Go:   func_decl, method_decl, type_decl, import_decl, const_decl, var_decl
   */
  balancedKinds: ReadonlySet<string>;

  /**
   * Classified kinds that trigger import-like grouping.
   *
   * When a line classifies as one of these kinds, the engine absorbs
   * following lines of the same kind plus interleaved blanks and comments.
   * Stops at doc comments, pragmas, or different kinds.
   *
   * Examples:
   *   Rust: use_decl
   *   Go:   (empty — Go imports are balanced via `import (`)
   */
  importKinds: ReadonlySet<string>;

  /**
   * Map a classified kind + trimmed line to a chunk category string.
   *
   * This is the final category assigned to the GenericChunk.
   * The scaffold pipeline uses categories through ScaffoldAdapter
   * for block/section routing.
   */
  kindToCategory: (kind: string, trimmed: string) => string;

  /**
   * If true, balanced-kind lines with no open delimiter are treated as
   * single-line declarations (no Case 3 multi-line signature scan).
   *
   * Go (true):  `const X = 1` has no `{` or `(` → single-line chunk.
   * Rust (false): `fn foo()` may continue to `-> Result<T> {` → Case 3 scans.
   *
   * Default: false (Rust behavior — multi-line signatures allowed).
   */
  singleLineIfNoDelimiter?: boolean;
}

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Core — universal chunk parser
// ---------------------------------------------------------------------------

/**
 * Parse source lines into semantic GenericChunk[] using a language config.
 *
 * Algorithm (mirrors the proven Rust parseCodeChunks flow):
 *   1. Walk lines sequentially
 *   2. Match structural patterns: pragma, fileDoc, blank, separator, comment
 *   3. For test blocks: capture with delimiter tracking
 *   4. For attributes: collect and rewind (attach to next item)
 *   5. Classify line → route:
 *      - balanced kinds → delimiter-tracked capture + doc/attr attachment
 *      - import kinds → absorb contiguous same-kind lines
 *      - single-line declarations → individual chunks
 *   6. Catch-all → "other" chunk
 *
 * @param lines Raw source lines (preserving original whitespace)
 * @param config Language-specific chunker configuration
 * @returns Ordered array of semantic chunks
 */
export function parseChunks(
  lines: string[],
  config: ChunkerConfig,
): GenericChunk[] {
  const chunks: GenericChunk[] = [];
  let i = 0;

  while (i < lines.length) {
    const trimmed = lines[i]!.trim();

    // ── Phase 1: Pragma directives ──────────────────────────────────
    if (config.patterns.pragma(trimmed)) {
      const chunk: GenericChunk = { lines: [lines[i]!], category: "pragma" };
      i++;
      // Collect contiguous pragma lines
      while (i < lines.length && config.patterns.pragma(lines[i]!.trim())) {
        chunk.lines.push(lines[i]!);
        i++;
      }
      chunks.push(chunk);
      continue;
    }

    // ── Phase 2: File-level doc comments ────────────────────────────
    if (config.patterns.fileDoc(trimmed)) {
      const chunk: GenericChunk = { lines: [lines[i]!], category: "doc_comment" };
      i++;
      while (i < lines.length && config.patterns.fileDoc(lines[i]!.trim())) {
        chunk.lines.push(lines[i]!);
        i++;
      }
      chunks.push(chunk);
      continue;
    }

    // ── Phase 3: Blank lines ────────────────────────────────────────
    if (trimmed === "") {
      const chunk: GenericChunk = { lines: [lines[i]!], category: "blank" };
      i++;
      while (i < lines.length && lines[i]!.trim() === "") {
        chunk.lines.push(lines[i]!);
        i++;
      }
      chunks.push(chunk);
      continue;
    }

    // ── Phase 4: Separator banners ──────────────────────────────────
    if (config.patterns.separator(trimmed)) {
      const chunk: GenericChunk = { lines: [lines[i]!], category: "separator" };
      i++;
      // Collect title line + closing separator
      while (i < lines.length) {
        const next = lines[i]!.trim();
        if (config.patterns.separator(next) ||
            (config.patterns.comment(next) && !config.patterns.separator(next))) {
          // Comment line between separators = title/subtitle
          chunk.lines.push(lines[i]!);
          i++;
          if (config.patterns.separator(next)) break; // closing separator
        } else {
          break;
        }
      }
      chunks.push(chunk);
      continue;
    }

    // ── Phase 5: Regular comments ───────────────────────────────────
    if (config.patterns.comment(trimmed) &&
        !config.patterns.fileDoc(trimmed) &&
        !config.patterns.pragma(trimmed) &&
        !config.patterns.separator(trimmed)) {
      const chunk: GenericChunk = { lines: [lines[i]!], category: "comment" };
      i++;
      while (i < lines.length) {
        const next = lines[i]!.trim();
        if (config.patterns.comment(next) &&
            !config.patterns.fileDoc(next) &&
            !config.patterns.pragma(next) &&
            !config.patterns.separator(next)) {
          chunk.lines.push(lines[i]!);
          i++;
        } else {
          break;
        }
      }
      chunks.push(chunk);
      continue;
    }

    // ── Phase 6: Test blocks (complete brace-delimited) ─────────────
    if (config.patterns.testBlock(trimmed)) {
      const chunk: GenericChunk = { lines: [lines[i]!], category: "test_module" };
      i++;
      const depth = trackDelimiters(chunk, lines, i);
      i = depth.nextIndex;
      chunks.push(chunk);
      continue;
    }

    // ── Phase 7: Attributes (collect and attempt attachment) ────────
    if (config.patterns.attribute(trimmed)) {
      const attrLines: string[] = [lines[i]!];
      i++;
      while (i < lines.length && config.patterns.attribute(lines[i]!.trim())) {
        attrLines.push(lines[i]!);
        i++;
      }
      // Rewind — the next iteration handles the item; attributes will be
      // picked up by the doc/attr attachment phase in balanced capture.
      i -= attrLines.length;
      // Fall through to classification below
    }

    // ── Phase 8: Classify and route ─────────────────────────────────
    const kind = config.classifyLine(trimmed);

    // --- 8a: Balanced-delimiter items ──────────────────────────────
    if (config.balancedKinds.has(kind)) {
      // Collect attached doc comments + attributes from previous chunks
      const itemLines = collectAttachedPreceding(chunks, config);

      itemLines.push(lines[i]!);
      i++;

      // Track delimiters until balanced
      i = captureBalancedItem(itemLines, lines, i, trimmed, config);

      // Determine category
      const category = config.kindToCategory(kind, trimmed);
      chunks.push({ lines: itemLines, category });
      continue;
    }

    // --- 8b: Import-like grouping ─────────────────────────────────
    if (config.importKinds.has(kind)) {
      const chunk: GenericChunk = { lines: [lines[i]!], category: "import" };
      i++;
      while (i < lines.length) {
        const next = lines[i]!.trim();
        const nextKind = config.classifyLine(next);
        // Absorb: same kind, blank lines, regular comments.
        // Do NOT absorb: item-level docs, file docs, or pragmas.
        if (nextKind === kind || next === "" ||
            (config.patterns.comment(next) &&
             !config.patterns.fileDoc(next) &&
             !config.patterns.itemDoc(next) &&
             !config.patterns.pragma(next) &&
             !config.patterns.separator(next))) {
          chunk.lines.push(lines[i]!);
          i++;
        } else {
          break;
        }
      }
      chunks.push(chunk);
      continue;
    }

    // --- 8c: Single-line declarations ─────────────────────────────
    {
      const category = config.kindToCategory(kind, trimmed);
      chunks.push({ lines: [lines[i]!], category });
      i++;
    }
  }

  return chunks;
}

// ---------------------------------------------------------------------------
// Helpers — delimiter tracking
// ---------------------------------------------------------------------------

/**
 * Track balanced delimiters from current position, appending to chunk.
 * Handles both `{ }` and `( )` — auto-detects which pair from content.
 *
 * @returns Object with nextIndex (where to resume parsing)
 */
function trackDelimiters(
  chunk: GenericChunk,
  lines: string[],
  startIdx: number,
): { nextIndex: number } {
  let i = startIdx;
  let braceDepth = 0;
  let parenDepth = 0;
  let foundOpen = false;

  // Count delimiters in existing chunk lines
  for (const line of chunk.lines) {
    for (const ch of line) {
      if (ch === "{") { braceDepth++; foundOpen = true; }
      if (ch === "}") braceDepth--;
      if (ch === "(") { parenDepth++; foundOpen = true; }
      if (ch === ")") parenDepth--;
    }
  }

  // Continue capturing until all delimiters balance
  while (i < lines.length) {
    chunk.lines.push(lines[i]!);
    for (const ch of lines[i]!) {
      if (ch === "{") { braceDepth++; foundOpen = true; }
      if (ch === "}") braceDepth--;
      if (ch === "(") { parenDepth++; foundOpen = true; }
      if (ch === ")") parenDepth--;
    }
    i++;
    if (foundOpen && braceDepth === 0 && parenDepth === 0) break;
  }

  return { nextIndex: i };
}

/**
 * Capture a balanced-delimiter item starting from the first line.
 *
 * Handles three cases:
 *   1. First line has open delimiter → track until balanced
 *   2. Line ends with `;` → single-line declaration
 *   3. Multi-line signature → keep reading until delimiter or `;`
 *
 * @param itemLines Lines collected so far (including first line of item)
 * @param lines All source lines
 * @param currentIdx Index of NEXT line to read
 * @param firstTrimmed Trimmed first line of the item
 * @returns Next index to resume parsing
 */
function captureBalancedItem(
  itemLines: string[],
  lines: string[],
  currentIdx: number,
  firstTrimmed: string,
  config: ChunkerConfig,
): number {
  let i = currentIdx;

  // Count delimiters in all collected lines so far
  let braceDepth = 0;
  let parenDepth = 0;
  for (const line of itemLines) {
    for (const ch of line) {
      if (ch === "{") braceDepth++;
      if (ch === "}") braceDepth--;
      if (ch === "(") parenDepth++;
      if (ch === ")") parenDepth--;
    }
  }

  const hasOpenDelimiter = braceDepth > 0 || parenDepth > 0;

  // Case 1: Has open delimiter → track until balanced
  if (hasOpenDelimiter) {
    while (i < lines.length && (braceDepth > 0 || parenDepth > 0)) {
      itemLines.push(lines[i]!);
      for (const ch of lines[i]!) {
        if (ch === "{") braceDepth++;
        if (ch === "}") braceDepth--;
        if (ch === "(") parenDepth++;
        if (ch === ")") parenDepth--;
      }
      i++;
    }
    return i;
  }

  // Case 2: Ends with `;` → single-line declaration
  if (firstTrimmed.endsWith(";")) {
    return i;
  }

  // Case 2b: No-semicolon language (Go) — no open delimiter means single-line.
  // Go's `const X = 1`, `type Foo int`, `var X string` have no delimiters.
  // Without this, Case 3 would consume the next line looking for `{` or `;`.
  if (config.singleLineIfNoDelimiter) {
    return i;
  }

  // Case 3: Multi-line signature — keep reading until delimiters found or `;`
  while (i < lines.length) {
    itemLines.push(lines[i]!);
    const line = lines[i]!;
    for (const ch of line) {
      if (ch === "{") braceDepth++;
      if (ch === "}") braceDepth--;
      if (ch === "(") parenDepth++;
      if (ch === ")") parenDepth--;
    }
    i++;

    // Found open delimiter — continue until balanced
    if (braceDepth > 0 || parenDepth > 0) {
      while (i < lines.length && (braceDepth > 0 || parenDepth > 0)) {
        itemLines.push(lines[i]!);
        for (const ch of lines[i]!) {
          if (ch === "{") braceDepth++;
          if (ch === "}") braceDepth--;
          if (ch === "(") parenDepth++;
          if (ch === ")") parenDepth--;
        }
        i++;
      }
      return i;
    }

    // Terminated by `;`
    if (line.trim().endsWith(";")) return i;
  }

  return i;
}

/**
 * Collect attached doc comments and attributes from preceding chunks.
 *
 * Walks backward through the chunk array, pulling off:
 *   - Item-level doc comments (e.g., `///` in Rust)
 *   - Attribute chunks (e.g., `#[derive(...)]` in Rust)
 *   - Single blanks between a doc comment and its item
 *
 * Returns lines to prepend to the current item. Modifies `chunks`
 * in place (pops consumed chunks).
 */
function collectAttachedPreceding(
  chunks: GenericChunk[],
  config: ChunkerConfig,
): string[] {
  const collected: string[] = [];

  while (chunks.length > 0) {
    const last = chunks[chunks.length - 1]!;

    // Item-level doc comments (/// in Rust)
    if (last.category === "comment" &&
        last.lines.some((l) => config.patterns.itemDoc(l.trim()))) {
      collected.unshift(...chunks.pop()!.lines);
      continue;
    }

    // Attribute lines — may have any category depending on how the language
    // classifier routes them. Check line content, not category.
    if (last.lines.every((l) => config.patterns.attribute(l.trim()))) {
      collected.unshift(...chunks.pop()!.lines);
      continue;
    }

    // Single blank line between doc comment and item — check if a doc
    // comment precedes the blank (formatting noise, attach to item)
    if (last.category === "blank" && last.lines.length <= 1) {
      const savedBlank = chunks.pop()!;
      if (chunks.length > 0 &&
          chunks[chunks.length - 1]!.category === "comment" &&
          chunks[chunks.length - 1]!.lines.some((l) =>
            config.patterns.itemDoc(l.trim()))) {
        // Doc comment found — attach it, skip the blank
        collected.unshift(...chunks.pop()!.lines);
      } else {
        // No doc comment — restore blank and stop
        chunks.push(savedBlank);
        break;
      }
      continue;
    }

    // Nothing else to attach
    break;
  }

  return collected;
}

// ---------------------------------------------------------------------------
// Preset patterns — shared by most C-family languages
// ---------------------------------------------------------------------------

/**
 * Common OmniCode pragma pattern — shared by Go, Rust, C, TypeScript.
 * Matches: `// #!omni ...` or `//omni:...`
 */
export function isOmniPragma(trimmed: string): boolean {
  return /^\s*\/\/\s+#!omni\b/.test(trimmed) || /^\s*\/\/omni:/.test(trimmed);
}

/**
 * Common separator pattern — matches block/section separator banners.
 * Matches: `// ──────...`, `// ══════...`, `// ------...`, `// ======...` (10+ chars)
 * Covers both Unicode box-drawing (─ U+2500, ═ U+2550) and ASCII (-, =).
 */
export function isSeparatorBanner(trimmed: string): boolean {
  return /^\/\/\s*[─═=\-]{10,}/.test(trimmed);
}

/**
 * Common comment pattern — double-slash comments, excluding special forms.
 * Returns true for `// ...` but NOT `//omni:`, `//!`, or separators.
 */
export function isRegularComment(trimmed: string): boolean {
  return trimmed.startsWith("//") &&
    !trimmed.startsWith("//omni:") &&
    !trimmed.startsWith("//!");
}

// ============================================================================
// CLOSING
// ============================================================================
//
// Universal chunking engine — the same walk/classify/group/balance algorithm
// that Rust's parseCodeChunks proved, now parameterized for all languages.
// Each handler provides its classifier and pattern matchers. The engine
// handles the structural complexity.
//
// The approach: simple patterns compose into complex behavior. The config
// says WHAT to look for. The engine says HOW to group it. Both are needed,
// neither alone suffices.
//
// "For we are labourers together with God." — 1 Corinthians 3:9
// ============================================================================
