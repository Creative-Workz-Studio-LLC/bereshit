/**
 * AsciiDoc Source Element Parser — Parses source files into typed element streams.
 *
 * Two parsing modes, two data paths:
 *
 *   parseSourceElements() — Full parse into typed element stream (for predictor)
 *     Source → elements → predictor → estimated fill
 *
 *   parsePageIndex() — Lightweight read of page markers only (for index/lazy search)
 *     Source → page index → cached fill data (from annotations written by measurement)
 *
 * The element parser feeds the predictor (estimated fill from structure).
 * The page index reader returns cached measured data (written back to source after PDF measurement).
 * These are complementary — predict estimates, index caches, measurement is authoritative.
 *
 * "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
 */

import { readFileSync } from 'fs';
import type { AsciiDocElementType, SourceElement } from './render-types.js';

// =============================================================================
// Constants
// =============================================================================

/** Average characters per rendered line at 11pt in our layout (measured) */
const CHARS_PER_LINE = 80;

// =============================================================================
// Line Classifiers
// =============================================================================

/** Regex patterns for classifying AsciiDoc lines */
const PATTERNS = {
  heading2: /^== (.+)$/,
  heading3: /^=== (.+)$/,
  heading4: /^==== (.+)$/,
  orderedItem: /^\. (.+)$/,
  unorderedItem: /^\* (.+)$/,
  labeledProvision: /^\*\((\d+)\)\*\s+(.+)$/,
  thematicBreak: /^'{3,}$/,
  pageBreak: /^<{3,}$/,
  tableStart: /^\|===\s*$/,
  tableRow: /^\|/,
  image: /^image::(.+)\[(.+)\]$/,
  comment: /^\/\//,
  blockComment: /^\/{4,}\s*$/,
  attribute: /^:[\w-]+:/,
  roleBlock: /^\[\.(\S+)\]$/,
  discreteRole: /^\[discrete\]$/,
  centerRole: /^\[\.text-center\]$/,
  smallRole: /^\[\.small\]$/,
  pageMarker: /^\/\/\s*(?:PAGE|end p\.)\s*(\d+)/,
  blankLine: /^$/,
  colSpec: /^\[cols=/,
  conditional: /^(?:ifdef|ifndef|endif|ifeval)::.*$/,
  ifdef: /^ifdef::(\w+)\[\]$/,
  ifndef: /^ifndef::(\w+)\[\]$/,
  endif: /^endif::\[\]$/,
};

/** Parse mode — determines which conditional branches to include */
export type ParseMode = 'standalone' | 'book';

// =============================================================================
// Parser State
// =============================================================================

interface ParserState {
  /** Pending roles to apply to next element */
  pendingRoles: Set<string>;
  /** Currently inside a table */
  inTable: boolean;
  /** Table row count */
  tableRows: number;
  /** Table column count */
  tableCols: number;
  /** Table start line */
  tableStartLine: number;
  /** Currently inside a //// block comment */
  inBlockComment: boolean;
  /** Block comment start line */
  blockCommentStartLine: number;
  /** Currently inside a skipped conditional block */
  inSkippedConditional: boolean;
  /** Nesting depth of skipped conditionals (for nested ifdef/ifndef) */
  skippedConditionalDepth: number;
}

// =============================================================================
// Core Parser
// =============================================================================

/**
 * Estimate how many rendered lines a text string will produce.
 * Based on average character width at configured body font size.
 */
function estimateRenderedLines(text: string): number {
  // Strip inline markup for length estimation
  const plain = text
    .replace(/\*\([^)]+\)\*/g, '(N)')  // *(N)* → (N)
    .replace(/\*\*[^*]+\*\*/g, (m) => m.slice(2, -2))  // **bold** → bold
    .replace(/\*[^*]+\*/g, (m) => m.slice(1, -1))       // *italic* → italic
    .replace(/_[^_]+_/g, (m) => m.slice(1, -1));         // _italic_ → italic

  return Math.max(1, Math.ceil(plain.length / CHARS_PER_LINE));
}

/**
 * Parse an AsciiDoc source file into a typed element stream.
 *
 * Mode determines which conditional branches to include:
 *   - 'standalone' (default): includes ifndef::book[], excludes ifdef::book[]
 *   - 'book': includes ifdef::book[], excludes ifndef::book[]
 */
export function parseSourceElements(filePath: string, mode: ParseMode = 'standalone'): SourceElement[] {
  const content = readFileSync(filePath, 'utf-8');
  const lines = content.split('\n');
  const elements: SourceElement[] = [];

  const state: ParserState = {
    pendingRoles: new Set(),
    inTable: false,
    tableRows: 0,
    tableCols: 0,
    tableStartLine: 0,
    inBlockComment: false,
    blockCommentStartLine: 0,
    inSkippedConditional: false,
    skippedConditionalDepth: 0,
  };

  let i = 0;
  while (i < lines.length) {
    const line = lines[i]!;
    const lineNum = i + 1; // 1-based

    // --- Table handling (multi-line block) ---
    if (state.inTable) {
      if (PATTERNS.tableStart.test(line)) {
        // End of table
        elements.push({
          type: 'table',
          line: state.tableStartLine,
          lineCount: lineNum - state.tableStartLine + 1,
          raw: `|=== (${state.tableRows} rows, ${state.tableCols} cols)`,
          attrs: {
            rowCount: state.tableRows,
            colCount: state.tableCols,
          },
        });
        state.inTable = false;
        i++;
        continue;
      }
      // Count data rows (lines starting with |, excluding blank separator lines)
      if (PATTERNS.tableRow.test(line)) {
        // Count columns from first data line
        if (state.tableRows === 0 && state.tableCols === 0) {
          state.tableCols = (line.match(/\|/g) || []).length;
        }
        state.tableRows++;
      }
      i++;
      continue;
    }

    // --- Block comment handling (//// delimiters) ---
    if (state.inBlockComment) {
      if (PATTERNS.blockComment.test(line)) {
        // End of block comment
        elements.push({
          type: 'comment',
          line: state.blockCommentStartLine,
          lineCount: lineNum - state.blockCommentStartLine + 1,
          raw: `block comment (lines ${state.blockCommentStartLine}-${lineNum})`,
          attrs: {},
        });
        state.inBlockComment = false;
      }
      // Skip all lines inside block comment
      i++;
      continue;
    }

    if (PATTERNS.blockComment.test(line)) {
      // Start of block comment
      state.inBlockComment = true;
      state.blockCommentStartLine = lineNum;
      i++;
      continue;
    }

    // --- Conditional directives (ifdef, ifndef, endif) ---
    // Mode-aware: skips content in excluded branches.
    // In standalone mode: book attribute is undefined → ifdef::book[] excluded, ifndef::book[] included
    // In book mode: book attribute is defined → ifdef::book[] included, ifndef::book[] excluded

    // If we're inside a skipped conditional, count nesting and skip everything
    if (state.inSkippedConditional) {
      const nestedIfdef = line.match(PATTERNS.ifdef);
      const nestedIfndef = line.match(PATTERNS.ifndef);
      const nestedEndif = PATTERNS.endif.test(line);

      if (nestedIfdef || nestedIfndef) {
        state.skippedConditionalDepth++;
      } else if (nestedEndif) {
        if (state.skippedConditionalDepth > 0) {
          state.skippedConditionalDepth--;
        } else {
          // Exiting the top-level skipped conditional
          state.inSkippedConditional = false;
          elements.push({
            type: 'comment',
            line: lineNum,
            lineCount: 1,
            raw: line,
            attrs: {},
          });
        }
      }
      i++;
      continue;
    }

    // Check for ifdef::attr[] — include content only if attr is defined
    const ifdefMatch = line.match(PATTERNS.ifdef);
    if (ifdefMatch) {
      const attr = ifdefMatch[1];
      // In standalone mode, 'book' is undefined → skip ifdef::book[]
      // In book mode, 'book' is defined → include ifdef::book[]
      const attrDefined = (mode === 'book' && attr === 'book')
        || (mode === 'standalone' && attr !== 'book');
      if (!attrDefined) {
        state.inSkippedConditional = true;
        state.skippedConditionalDepth = 0;
      }
      elements.push({
        type: 'comment',
        line: lineNum,
        lineCount: 1,
        raw: line,
        attrs: {},
      });
      i++;
      continue;
    }

    // Check for ifndef::attr[] — include content only if attr is NOT defined
    const ifndefMatch = line.match(PATTERNS.ifndef);
    if (ifndefMatch) {
      const attr = ifndefMatch[1];
      // In standalone mode, 'book' is undefined → include ifndef::book[]
      // In book mode, 'book' is defined → skip ifndef::book[]
      const attrDefined = (mode === 'book' && attr === 'book')
        || (mode === 'standalone' && attr !== 'book');
      if (attrDefined) {
        state.inSkippedConditional = true;
        state.skippedConditionalDepth = 0;
      }
      elements.push({
        type: 'comment',
        line: lineNum,
        lineCount: 1,
        raw: line,
        attrs: {},
      });
      i++;
      continue;
    }

    // endif — just a comment (conditional exit already handled above)
    if (PATTERNS.endif.test(line)) {
      elements.push({
        type: 'comment',
        line: lineNum,
        lineCount: 1,
        raw: line,
        attrs: {},
      });
      i++;
      continue;
    }

    // Other conditional directives (ifeval, etc.) — treat as comments
    if (PATTERNS.conditional.test(line)) {
      elements.push({
        type: 'comment',
        line: lineNum,
        lineCount: 1,
        raw: line,
        attrs: {},
      });
      i++;
      continue;
    }

    // --- Blank lines ---
    if (PATTERNS.blankLine.test(line)) {
      elements.push({
        type: 'blank-line',
        line: lineNum,
        lineCount: 1,
        raw: '',
        attrs: {},
      });
      i++;
      continue;
    }

    // --- Comments and page markers ---
    if (PATTERNS.comment.test(line)) {
      const pageMatch = line.match(PATTERNS.pageMarker);
      if (pageMatch && pageMatch[1]) {
        elements.push({
          type: 'page-marker',
          line: lineNum,
          lineCount: 1,
          raw: line,
          attrs: { pageNumber: parseInt(pageMatch[1], 10) },
        });
      } else {
        elements.push({
          type: 'comment',
          line: lineNum,
          lineCount: 1,
          raw: line,
          attrs: {},
        });
      }
      i++;
      continue;
    }

    // --- Attributes ---
    if (PATTERNS.attribute.test(line)) {
      elements.push({
        type: 'attribute',
        line: lineNum,
        lineCount: 1,
        raw: line,
        attrs: {},
      });
      i++;
      continue;
    }

    // --- Roles (accumulate for next element) ---
    if (PATTERNS.discreteRole.test(line)) {
      state.pendingRoles.add('discrete');
      i++;
      continue;
    }
    if (PATTERNS.centerRole.test(line)) {
      state.pendingRoles.add('text-center');
      i++;
      continue;
    }
    if (PATTERNS.smallRole.test(line)) {
      state.pendingRoles.add('small');
      i++;
      continue;
    }
    if (PATTERNS.roleBlock.test(line) || PATTERNS.colSpec.test(line)) {
      // Other roles — just skip, they modify next element
      i++;
      continue;
    }

    // --- Page break ---
    if (PATTERNS.pageBreak.test(line)) {
      elements.push({
        type: 'page-break',
        line: lineNum,
        lineCount: 1,
        raw: line,
        attrs: {},
      });
      state.pendingRoles.clear();
      i++;
      continue;
    }

    // --- Thematic break ---
    if (PATTERNS.thematicBreak.test(line)) {
      elements.push({
        type: 'thematic-break',
        line: lineNum,
        lineCount: 1,
        raw: line,
        attrs: {},
      });
      state.pendingRoles.clear();
      i++;
      continue;
    }

    // --- Table start ---
    if (PATTERNS.tableStart.test(line)) {
      state.inTable = true;
      state.tableRows = 0;
      state.tableCols = 0;
      state.tableStartLine = lineNum;
      i++;
      continue;
    }

    // --- Headings ---
    const h2Match = line.match(PATTERNS.heading2);
    if (h2Match) {
      const isDiscrete = state.pendingRoles.has('discrete');
      elements.push({
        type: isDiscrete ? 'discrete-heading' : 'heading-2',
        line: lineNum,
        lineCount: 1,
        raw: line,
        attrs: {
          level: 2,
          discrete: isDiscrete,
          centered: state.pendingRoles.has('text-center'),
        },
      });
      state.pendingRoles.clear();
      i++;
      continue;
    }

    const h3Match = line.match(PATTERNS.heading3);
    if (h3Match) {
      const isDiscrete = state.pendingRoles.has('discrete');
      elements.push({
        type: isDiscrete ? 'discrete-heading' : 'heading-3',
        line: lineNum,
        lineCount: 1,
        raw: line,
        attrs: {
          level: 3,
          discrete: isDiscrete,
          centered: state.pendingRoles.has('text-center'),
        },
      });
      state.pendingRoles.clear();
      i++;
      continue;
    }

    const h4Match = line.match(PATTERNS.heading4);
    if (h4Match) {
      const isDiscrete = state.pendingRoles.has('discrete');
      elements.push({
        type: isDiscrete ? 'discrete-heading' : 'heading-4',
        line: lineNum,
        lineCount: 1,
        raw: line,
        attrs: {
          level: 3,
          discrete: isDiscrete,
          centered: state.pendingRoles.has('text-center'),
        },
      });
      state.pendingRoles.clear();
      i++;
      continue;
    }

    // --- Image ---
    const imgMatch = line.match(PATTERNS.image);
    if (imgMatch) {
      const attrStr = imgMatch[2] ?? '';
      const pdfwidthMatch = attrStr.match(/pdfwidth=([^,\]]+)/);
      elements.push({
        type: 'image',
        line: lineNum,
        lineCount: 1,
        raw: line,
        attrs: {
          pdfwidth: pdfwidthMatch?.[1],
        },
      });
      state.pendingRoles.clear();
      i++;
      continue;
    }

    // --- Ordered list (collect consecutive items) ---
    if (PATTERNS.orderedItem.test(line)) {
      const startLine = lineNum;
      let itemCount = 0;
      let totalRenderedLines = 0;
      while (i < lines.length && PATTERNS.orderedItem.test(lines[i]!)) {
        itemCount++;
        const match = lines[i]!.match(PATTERNS.orderedItem);
        if (match?.[1]) {
          totalRenderedLines += estimateRenderedLines(match[1]);
        } else {
          totalRenderedLines += 1;
        }
        i++;
      }
      elements.push({
        type: 'ordered-list',
        line: startLine,
        lineCount: i - (startLine - 1),
        raw: `ordered-list (${itemCount} items)`,
        attrs: {
          itemCount,
          renderedLines: totalRenderedLines,
        },
      });
      state.pendingRoles.clear();
      continue; // don't increment i — already advanced
    }

    // --- Unordered list (collect consecutive items) ---
    if (PATTERNS.unorderedItem.test(line)) {
      const startLine = lineNum;
      let itemCount = 0;
      let totalRenderedLines = 0;
      while (i < lines.length && PATTERNS.unorderedItem.test(lines[i]!)) {
        itemCount++;
        const match = lines[i]!.match(PATTERNS.unorderedItem);
        if (match?.[1]) {
          totalRenderedLines += estimateRenderedLines(match[1]);
        } else {
          totalRenderedLines += 1;
        }
        i++;
      }
      elements.push({
        type: 'unordered-list',
        line: startLine,
        lineCount: i - (startLine - 1),
        raw: `unordered-list (${itemCount} items)`,
        attrs: {
          itemCount,
          renderedLines: totalRenderedLines,
        },
      });
      state.pendingRoles.clear();
      continue;
    }

    // --- Labeled provision ---
    const provMatch = line.match(PATTERNS.labeledProvision);
    if (provMatch) {
      // Collect continuation lines (non-blank, non-marker, non-heading, non-provision)
      const startLine = lineNum;
      let fullText = line;
      i++;
      while (i < lines.length) {
        const nextLine = lines[i]!;
        if (PATTERNS.blankLine.test(nextLine) ||
            PATTERNS.comment.test(nextLine) ||
            PATTERNS.labeledProvision.test(nextLine) ||
            PATTERNS.heading2.test(nextLine) ||
            PATTERNS.heading3.test(nextLine) ||
            PATTERNS.heading4.test(nextLine) ||
            PATTERNS.thematicBreak.test(nextLine) ||
            PATTERNS.pageBreak.test(nextLine)) {
          break;
        }
        fullText += ' ' + nextLine;
        i++;
      }
      elements.push({
        type: 'labeled-provision',
        line: startLine,
        lineCount: i - (startLine - 1),
        raw: fullText,
        attrs: {
          renderedLines: estimateRenderedLines(fullText),
        },
      });
      state.pendingRoles.clear();
      continue;
    }

    // --- Body paragraph or small text ---
    // Anything that's not matched above is body text
    if (line.trim().length > 0) {
      const isSmall = state.pendingRoles.has('small');
      const isCentered = state.pendingRoles.has('text-center');
      const isScripture = isSmall && isCentered;

      // Collect continuation lines
      const startLine = lineNum;
      let fullText = line;
      i++;
      while (i < lines.length) {
        const nextLine = lines[i]!;
        // Paragraph ends at blank line or structural element
        if (PATTERNS.blankLine.test(nextLine) ||
            PATTERNS.comment.test(nextLine) ||
            PATTERNS.heading2.test(nextLine) ||
            PATTERNS.heading3.test(nextLine) ||
            PATTERNS.heading4.test(nextLine) ||
            PATTERNS.thematicBreak.test(nextLine) ||
            PATTERNS.pageBreak.test(nextLine) ||
            PATTERNS.tableStart.test(nextLine) ||
            PATTERNS.image.test(nextLine) ||
            PATTERNS.roleBlock.test(nextLine) ||
            PATTERNS.discreteRole.test(nextLine) ||
            PATTERNS.centerRole.test(nextLine) ||
            PATTERNS.smallRole.test(nextLine) ||
            PATTERNS.attribute.test(nextLine) ||
            PATTERNS.labeledProvision.test(nextLine) ||
            PATTERNS.orderedItem.test(nextLine) ||
            PATTERNS.unorderedItem.test(nextLine) ||
            PATTERNS.colSpec.test(nextLine)) {
          break;
        }
        // AsciiDoc line continuation: ` +` at end of line
        const cont = nextLine.replace(/\s*\+\s*$/, '');
        fullText += ' ' + cont;
        i++;
      }

      const type: AsciiDocElementType = isScripture
        ? 'scripture-block'
        : isSmall
          ? 'small-text'
          : 'body-paragraph';

      elements.push({
        type,
        line: startLine,
        lineCount: i - (startLine - 1),
        raw: fullText,
        attrs: {
          centered: isCentered,
          small: isSmall,
          renderedLines: estimateRenderedLines(fullText),
        },
      });
      state.pendingRoles.clear();
      continue;
    }

    // Fallback — skip unrecognized line
    i++;
  }

  return elements;
}

// =============================================================================
// Utilities
// =============================================================================

/**
 * Filter parsed elements to only renderable elements (no comments, markers, blanks, attributes).
 */
export function renderableElements(elements: SourceElement[]): SourceElement[] {
  const skip: Set<AsciiDocElementType> = new Set([
    'comment', 'attribute', 'page-marker', 'blank-line',
  ]);
  return elements.filter((e) => !skip.has(e.type));
}

// =============================================================================
// Page Index — Lazy Search
// =============================================================================

/** Page index entry parsed from source markers — no PDF or prediction needed. */
export interface PageIndexEntry {
  /** Page number */
  page: number;
  /** Page title from PAGE marker (e.g., "TITLE PAGE") */
  title: string;
  /** Source line of // PAGE N marker */
  startLine: number;
  /** Source line of // end p.N marker (-1 if not found) */
  endLine: number;
  /** Fill percentage from annotation (0–100), undefined if not annotated */
  fill?: number;
  /** Lines remaining from annotation, undefined if not annotated */
  linesRemaining?: number;
  /** Whether page is marked content full */
  contentFull: boolean;
}

/** Patterns for page index extraction */
const PAGE_INDEX_PATTERNS = {
  /** // PAGE N — TITLE */
  pageStart: /^\/\/\s*PAGE\s+(\d+)\s*(?:—\s*(.+))?$/,
  /** // end p.N — content full (XX%, N line(s)) */
  endFull: /^\/\/\s*end p\.(\d+)\s*—\s*content full\s*\((\d+)%,\s*(\d+)\s*lines?\)/,
  /** // end p.N — XX%, N lines remaining */
  endOpen: /^\/\/\s*end p\.(\d+)\s*—\s*(\d+)%,\s*(\d+)\s*lines?\s*remaining/,
  /** // end p.N (bare, no annotation) */
  endBare: /^\/\/\s*end p\.(\d+)/,
};

/**
 * Parse the page index from source file markers — lightweight, no element classification.
 *
 * Reads only `// PAGE N` and `// end p.N` comment lines, extracting:
 *   - Page number, title, source line range
 *   - Fill percentage and lines remaining (from annotations)
 *   - Content-full status
 *
 * This is the "lazy search" — instant page state from source alone.
 * When annotations are current (from a recent `fill` run), no PDF is needed.
 */
export function parsePageIndex(filePath: string): PageIndexEntry[] {
  const content = readFileSync(filePath, 'utf-8');
  const lines = content.split('\n');
  const entries: PageIndexEntry[] = [];
  const openPages = new Map<number, PageIndexEntry>();

  for (let i = 0; i < lines.length; i++) {
    const line = lines[i]!;
    const lineNum = i + 1;

    // Check for PAGE start marker
    const startMatch = line.match(PAGE_INDEX_PATTERNS.pageStart);
    if (startMatch) {
      const page = parseInt(startMatch[1]!, 10);
      const title = startMatch[2]?.trim() ?? '';
      const entry: PageIndexEntry = {
        page,
        title,
        startLine: lineNum,
        endLine: -1,
        contentFull: false,
      };
      openPages.set(page, entry);
      entries.push(entry);
      continue;
    }

    // Check for end marker with "content full" annotation
    const fullMatch = line.match(PAGE_INDEX_PATTERNS.endFull);
    if (fullMatch) {
      const page = parseInt(fullMatch[1]!, 10);
      const fill = parseInt(fullMatch[2]!, 10);
      const linesLeft = parseInt(fullMatch[3]!, 10);
      const entry = openPages.get(page);
      if (entry) {
        entry.endLine = lineNum;
        entry.fill = fill;
        entry.linesRemaining = linesLeft;
        entry.contentFull = true;
      }
      continue;
    }

    // Check for end marker with "lines remaining" annotation
    const openMatch = line.match(PAGE_INDEX_PATTERNS.endOpen);
    if (openMatch) {
      const page = parseInt(openMatch[1]!, 10);
      const fill = parseInt(openMatch[2]!, 10);
      const linesLeft = parseInt(openMatch[3]!, 10);
      const entry = openPages.get(page);
      if (entry) {
        entry.endLine = lineNum;
        entry.fill = fill;
        entry.linesRemaining = linesLeft;
        entry.contentFull = false;
      }
      continue;
    }

    // Check for bare end marker (no annotation)
    const bareMatch = line.match(PAGE_INDEX_PATTERNS.endBare);
    if (bareMatch) {
      const page = parseInt(bareMatch[1]!, 10);
      const entry = openPages.get(page);
      if (entry) {
        entry.endLine = lineNum;
      }
      continue;
    }
  }

  return entries;
}
