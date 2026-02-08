/**
 * CWS Manual Builder — Editorial Lint Validation
 *
 * All editorial style checks in one module. Replaces the Makefile grep chains
 * with structured, categorized validation. Each check produces findings with
 * severity, line numbers, and actionable messages.
 *
 * Check categories:
 *   editorial   — Core editorial lint (checks 1-8, 36, 47)
 *                  1-8: shall, spacers, markers, old-comments, lead, title-page, heading guard
 *                  36 (attribute-usage), 47 (board-reference)
 *   style       — Anti-pattern detection (checks 9-13, 37-40, 53, 55-56)
 *                  9-13: company-name, chapter-language, all-caps, gold-text, register-mixing
 *                  37 (closing-filler), 38 (paragraph-density), 39 (redundant-logo),
 *                  40 (color-accessibility), 53 (section-ref-format),
 *                  55 (lead-statement), 56 (em-dash-listing)
 *   structure   — Structural integrity checks (checks 14-15, 32-35, 41-44, 50-51, 54, 57)
 *                  14-15: unlabeled breaks, xref resolution
 *                  32-35: prohibited attributes, redundant page breaks,
 *                         include guards, section role placement (config-driven)
 *                  41 (frontmatter-alignment), 42 (toc-table),
 *                  43 (subordinate-heading), 44 (appendix-duplication),
 *                  50 (orphaned-heading), 51 (image-attributes),
 *                  54 (hebrew-font-config), 57 (reading-guide-parties)
 *   conversion  — Article editorial conversion checklist (checks 16-17, 48-49)
 *                  17 (admonition-blocks — all scopes), 48 (sidebar-blocks),
 *                  49 (icon-tables)
 *   legal       — Legal/governance language checks (checks 18-26, 45, 52, config-driven)
 *                  52 (defined-terms-bold)
 *                  Per-file: 18 (definition verbs), 21 (prohibited terms),
 *                            23 (numbering), 24 (enumeration), 25 (self-reference),
 *                            45 (archaic-context)
 *                  Cross-file: 19 (binding), 20 (boilerplate),
 *                              22 (recitals style + whereas-operative), 26 (document anatomy)
 *   tone        — Tone & voice checks (checks 27-29, 46, config-driven)
 *                  Per-file: 27 (compulsion language), 28 (plain language words),
 *                            29 (sentence length), 46 (passive-governance)
 *   industry    — Industry-specific checks (checks 30-31, config-driven)
 *                  Per-file: 30 (document register consistency)
 *                  Cross-file: 31 (creator agreement provisions)
 *
 * Total: 57 checks across 7 categories (config version 5.2.0)
 *
 * Specification: editorial-style-guide.adoc (CWS-GDE-001) — FULL COVERAGE
 *   §Company Conventions + §Anti-Patterns + §Legal & Governance Language +
 *   §Tone & Voice + §Industry-Specific + §AsciiDoc Publishing Reference +
 *   §CIM Section Patterns + §Editorial Conversion Checklist + §Build Verification
 *
 * "Prove all things; hold fast that which is good."
 *  — 1 Thessalonians 5:21
 */

import { readFileSync, readdirSync, existsSync } from 'fs';
import { execSync } from 'child_process';
import { join, basename, dirname, relative } from 'path';
import type {
  RuntimePaths,
  LegalLanguageConfig,
  ToneAndVoiceConfig,
  IndustrySpecificConfig,
  DocumentTypesConfig,
  AsciidocPublishingConfig,
  CompanyConventionsConfig,
} from '../../config/types.js';
import { detectSectionType, detectScope } from '../../errors.js';


// =============================================================================
// Types
// =============================================================================

/** Severity of an editorial finding. */
export type EditorialSeverity = 'warn' | 'info';

/** Check category for filtering. */
export type CheckCategory = 'editorial' | 'style' | 'structure' | 'conversion' | 'legal' | 'industry' | 'tone';

/** A single editorial finding. */
export interface EditorialFinding {
  /** Severity level */
  severity: EditorialSeverity;

  /** Check identifier (e.g., "shall", "company-name", "unlabeled-breaks") */
  check: string;

  /** Check category */
  category: CheckCategory;

  /** Human-readable message */
  message: string;

  /** Source line number (1-indexed, 0 if not applicable) */
  line: number;

  /** Source text that triggered the finding (for context) */
  source?: string;
}

/** Results for a single file. */
export interface EditorialFileResult {
  /** Filename (basename) */
  file: string;

  /** Full path to the file */
  path: string;

  /** All findings for this file */
  findings: EditorialFinding[];

  /** Count by severity */
  counts: { warn: number; info: number };
}

/** Complete editorial lint result. */
export interface EditorialResult {
  /** Results per file */
  files: EditorialFileResult[];

  /** Number of files checked */
  filesChecked: number;

  /** Totals across all files */
  totals: { warn: number; info: number };

  /** Which categories were run */
  categories: CheckCategory[];
}

/** Options for running editorial lint. */
export interface EditorialLintOptions {
  /** Single file to lint (filename or path) */
  file?: string;

  /** Scope letter (A, B, C, D) */
  scope?: string;

  /** Check category filter (omit for all) */
  check?: string;

  /** Document type context (e.g., eula, creator_agreement, board_resolution) */
  documentType?: string;

  /** Expected register (e.g., formal_traditional, plain, informal, warm) */
  register?: string;
}


// =============================================================================
// File Resolution
// =============================================================================

/**
 * Resolve the list of files to lint based on options.
 */
function resolveFiles(paths: RuntimePaths, options: EditorialLintOptions): string[] {
  const bookDir = paths.bookDir;

  if (options.file) {
    // Single file — resolve using same logic as Makefile
    let target = options.file;

    // Strip book/ prefix
    if (target.startsWith('book/')) target = target.slice(5);

    // Add .adoc if missing
    if (!target.endsWith('.adoc')) target += '.adoc';

    // If has slash, treat as path relative to book/
    if (target.includes('/')) {
      const full = join(bookDir, target);
      if (existsSync(full)) return [full];
      return [];
    }

    // Search all of book/ for this filename
    return findFileRecursive(bookDir, target);
  }

  if (options.scope) {
    const scopeMap: Record<string, string> = {
      'A': 'A-frontmatter',
      'B': 'B-content',
      'C': 'C-appendices',
      'D': 'D-backmatter',
      'frontmatter': 'A-frontmatter',
      'content': 'B-content',
      'appendices': 'C-appendices',
      'backmatter': 'D-backmatter',
    };
    const dir = scopeMap[options.scope];
    if (!dir) return [];
    return findAdocFiles(join(bookDir, dir));
  }

  // All files (exclude 00-assets)
  return findAdocFiles(bookDir).filter(f => !f.includes('/00-assets/'));
}

/** Recursively find all .adoc files in a directory. */
function findAdocFiles(dir: string): string[] {
  if (!existsSync(dir)) return [];
  const results: string[] = [];

  function walk(d: string): void {
    for (const entry of readdirSync(d, { withFileTypes: true })) {
      const full = join(d, entry.name);
      if (entry.isDirectory()) {
        walk(full);
      } else if (entry.name.endsWith('.adoc')) {
        results.push(full);
      }
    }
  }

  walk(dir);
  return results.sort();
}

/** Find a file by name recursively. */
function findFileRecursive(dir: string, filename: string): string[] {
  const results: string[] = [];

  function walk(d: string): void {
    for (const entry of readdirSync(d, { withFileTypes: true })) {
      const full = join(d, entry.name);
      if (entry.isDirectory()) {
        walk(full);
      } else if (entry.name === filename || entry.name.toLowerCase() === filename.toLowerCase()) {
        results.push(full);
      }
    }
  }

  walk(dir);
  return results;
}


// =============================================================================
// Check Implementations — Editorial (1-8)
// =============================================================================

/**
 * Check 1: "shall" in governance text (skip Bible quotes, comments).
 * Config-driven: when legal_language config is available, uses the obligation
 * word lists to provide register-aware messaging.
 */
function checkShall(lines: string[], findings: EditorialFinding[], legalConfig?: LegalLanguageConfig): void {
  // Build modern word list from config (or fall back to defaults)
  const modernWords = legalConfig
    ? [legalConfig.obligation.modern.duty, legalConfig.obligation.modern.future,
      legalConfig.obligation.modern.permission, legalConfig.obligation.modern.recommendation]
    : ['must', 'will', 'may'];
  const modernAlts = modernWords.join('/');

  for (let i = 0; i < lines.length; i++) {
    const line = lines[i]!;

    // Skip comments
    if (line.trimStart().startsWith('//')) continue;

    // Skip lines with Bible/scripture indicators
    if (/KJV|WEB|scripture|verse|Bible/i.test(line)) continue;

    // Skip quote blocks and attributions
    if (line.includes('[quote') || line.startsWith('—') || line.startsWith('_—')) continue;

    // Skip lines in literal/listing blocks
    if (line.startsWith('----') || line.startsWith('....')) continue;

    // Check for "shall" — skip underscored variants (like _shall_)
    if (/\bshall\b/i.test(line) && !/_/.test(line.match(/shall/i)?.[0] ?? '')) {
      // Also skip if "shall" is inside an italic or bold marker used in examples
      if (line.includes('|*') || line.includes('|_')) continue;

      findings.push({
        severity: 'warn',
        check: 'shall',
        category: 'editorial',
        message: `'shall' — traditional register word; verify context is appropriate (modern: ${modernAlts})`,
        line: i + 1,
        source: line.trim(),
      });
    }
  }
}

/** Check 2: {empty} + spacer count. */
function checkSpacers(lines: string[], findings: EditorialFinding[]): void {
  let count = 0;
  for (let i = 0; i < lines.length; i++) {
    if ((lines[i] ?? '').includes('{empty} +')) {
      count++;
    }
  }
  if (count > 0) {
    findings.push({
      severity: 'info',
      check: 'spacers',
      category: 'editorial',
      message: `{empty} + spacers: ${count} found (review if intentional)`,
      line: 0,
    });
  }
}

/**
 * Get PDF page count using qpdf --show-npages.
 * Returns 0 if qpdf is unavailable or the command fails.
 */
function getPdfPageCount(pdfPath: string): number {
  try {
    const output = execSync(`qpdf --show-npages "${pdfPath}"`, {
      encoding: 'utf-8',
      timeout: 5000,
      stdio: ['pipe', 'pipe', 'pipe'],
    }).trim();
    const count = parseInt(output, 10);
    return isNaN(count) ? 0 : count;
  } catch {
    return 0;
  }
}

/** Check 3-4: Page marker integrity and sequence. */
function checkPageMarkers(lines: string[], findings: EditorialFinding[], filePath: string, outputDir: string): void {
  const pageNumbers: { num: number; line: number }[] = [];

  for (let i = 0; i < lines.length; i++) {
    const match = (lines[i] ?? '').match(/\/\/ end p\.(\d+)/);
    if (match) {
      pageNumbers.push({ num: parseInt(match[1]!, 10), line: i + 1 });
    }
  }

  if (pageNumbers.length === 0) return;

  const sorted = [...pageNumbers].sort((a, b) => a.num - b.num);
  const highest = sorted[sorted.length - 1]!.num;

  findings.push({
    severity: 'info',
    check: 'page-markers',
    category: 'editorial',
    message: `Page markers: ${pageNumbers.length} boundaries (pages 1-${highest})`,
    line: 0,
  });

  // Check sequence (should be 1, 2, 3, ...)
  let prev = 0;
  let seqOk = true;
  for (const p of sorted) {
    const expected = prev + 1;
    if (p.num !== expected) {
      findings.push({
        severity: 'warn',
        check: 'page-markers',
        category: 'editorial',
        message: `Page sequence gap: expected p.${expected}, found p.${p.num}`,
        line: p.line,
      });
      seqOk = false;
    }
    prev = p.num;
  }

  if (seqOk) {
    findings.push({
      severity: 'info',
      check: 'page-markers',
      category: 'editorial',
      message: `Page sequence: 1 through ${highest} (sequential)`,
      line: 0,
    });
  }

  // PDF page count cross-validation — compare marker count against actual PDF
  const bookRoot = dirname(dirname(filePath));
  const relPath = relative(bookRoot, filePath);
  const pdfName = relPath.replace(/\.adoc$/, '.pdf').replace(/^book\//, '');
  const pdfPath = join(outputDir, pdfName);

  if (existsSync(pdfPath)) {
    const pdfPageCount = getPdfPageCount(pdfPath);
    if (pdfPageCount > 0) {
      if (pdfPageCount === highest) {
        findings.push({
          severity: 'info',
          check: 'page-markers',
          category: 'editorial',
          message: `PDF cross-validation: ${basename(pdfPath)} — ${pdfPageCount} pages (matches markers)`,
          line: 0,
        });
      } else {
        findings.push({
          severity: 'warn',
          check: 'page-markers',
          category: 'editorial',
          message: `PDF cross-validation MISMATCH: ${basename(pdfPath)} has ${pdfPageCount} pages but markers indicate ${highest}`,
          line: 0,
        });
      }
    } else {
      findings.push({
        severity: 'info',
        check: 'page-markers',
        category: 'editorial',
        message: `PDF exists but page count unavailable: ${basename(pdfPath)} (marker count: ${highest})`,
        line: 0,
      });
    }
  }
}

/** Check 5: Old-format page comments. */
function checkOldComments(lines: string[], findings: EditorialFinding[]): void {
  let count = 0;
  for (let i = 0; i < lines.length; i++) {
    if ((lines[i] ?? '').includes('// --- PAGE BREAK')) {
      count++;
    }
  }
  if (count > 0) {
    findings.push({
      severity: 'warn',
      check: 'old-comments',
      category: 'editorial',
      message: `Old-format page comments: ${count} (convert to // end p.N block format)`,
      line: 0,
    });
  }
}

/** Check 6: [.lead] density. */
function checkLeadDensity(lines: string[], findings: EditorialFinding[]): void {
  let count = 0;
  for (let i = 0; i < lines.length; i++) {
    if (/^\[\.lead\]/.test(lines[i] ?? '')) {
      count++;
    }
  }
  if (count > 2) {
    findings.push({
      severity: 'info',
      check: 'lead-density',
      category: 'editorial',
      message: `[.lead] usage: ${count} instances (check for font size consistency)`,
      line: 0,
    });
  }
}

/** Check 7: :title-page: in standalone header. */
function checkTitlePage(lines: string[], findings: EditorialFinding[]): void {
  for (let i = 0; i < lines.length; i++) {
    if ((lines[i] ?? '').includes(':title-page:')) {
      findings.push({
        severity: 'warn',
        check: 'title-page',
        category: 'editorial',
        message: 'Standalone header uses :title-page: (should be :notitle: for frontmatter)',
        line: i + 1,
      });
      break;
    }
  }
}

/** Check 8: Book heading guard — [discrete] == without ifndef::book[] wrapper. */
function checkBookHeadingGuard(lines: string[], findings: EditorialFinding[]): void {
  let hasIfdef = false;
  for (const l of lines) {
    if (l.includes('ifdef::book[]')) {
      hasIfdef = true;
      break;
    }
  }

  if (!hasIfdef) return;

  // Count [discrete] headings and guarded ones
  let bareDiscrete = 0;
  let guarded = 0;

  for (let i = 0; i < lines.length; i++) {
    const cur = lines[i] ?? '';
    const next = lines[i + 1] ?? '';
    const prev = lines[i - 1] ?? '';

    if (cur.trim() === '[discrete]' && next.startsWith('== ')) {
      bareDiscrete++;

      // Check if preceded by ifndef::book[]
      if (prev.includes('ifndef::book[]')) {
        guarded++;
      }
    }
  }

  if (bareDiscrete > guarded) {
    findings.push({
      severity: 'info',
      check: 'book-heading',
      category: 'editorial',
      message: `Book heading guard: file has ifdef::book[] but some [discrete] == headings may lack ifndef::book[] wrapper`,
      line: 0,
    });
  }
}


// =============================================================================
// Check Implementations — Style Anti-Patterns (9-13)
// =============================================================================

/** Check 9: "Creative Workz Studio" (spaced company name). */
function checkCompanyName(lines: string[], findings: EditorialFinding[]): void {
  for (let i = 0; i < lines.length; i++) {
    const line = lines[i]!;

    // Skip comments and attribute definitions
    if (line.trimStart().startsWith('//')) continue;
    if (line.startsWith(':company-name:')) continue;
    if (line.startsWith(':company-short:')) continue;

    if (/Creative\s+Workz\s+Studio/i.test(line)) {
      findings.push({
        severity: 'warn',
        check: 'company-name',
        category: 'style',
        message: '"Creative Workz Studio" (spaced) — use "CreativeWorkzStudio LLC"',
        line: i + 1,
        source: line.trim(),
      });
    }
  }
}

/** Check 10: "Chapter" language in governance text. */
function checkChapterLanguage(lines: string[], findings: EditorialFinding[]): void {
  for (let i = 0; i < lines.length; i++) {
    const line = lines[i]!;

    // Skip comments
    if (line.trimStart().startsWith('//')) continue;

    // Skip Bible/scripture references (e.g., "Chapter 3 of Romans")
    if (/KJV|WEB|scripture|verse|Bible|Romans|Genesis|Exodus|Psalms/i.test(line)) continue;

    // Skip quote blocks
    if (line.includes('[quote')) continue;

    // Skip lines where "Chapter" refers to a chapter label (asciidoctor attribute)
    if (line.includes('chapter-label')) continue;

    // Detect standalone "Chapter" usage (not "chapter-label" or "Chapter_N")
    if (/\bChapter\b/.test(line) && !/chapter-/.test(line)) {
      findings.push({
        severity: 'warn',
        check: 'chapter-language',
        category: 'style',
        message: '"Chapter" — use "Article", "Section", or "§" (governance terminology)',
        line: i + 1,
        source: line.trim(),
      });
    }
  }
}

/** Check 11: ALL CAPS for emphasis (4+ consecutive uppercase letters). */
function checkAllCaps(lines: string[], findings: EditorialFinding[]): void {
  // Known acronyms that are valid ALL CAPS
  const knownAcronyms = new Set([
    'CWS', 'LLC', 'CEO', 'CFO', 'COO', 'CCO', 'CTO',
    'PDF', 'HTML', 'EPUB', 'XML', 'YAML', 'TOML', 'JSON', 'JSONC',
    'API', 'CLI', 'GUI', 'URL', 'WCAG', 'EULA', 'SOP', 'MSCD',
    'KJV', 'WEB', 'TOC', 'DPI', 'SVG', 'PNG', 'JPEG',
    'PASS', 'FAIL', 'WARN', 'INFO', 'SKIP',
    'NOTE', 'TIP', 'IMPORTANT', 'WARNING', 'CAUTION',
    'WHEREAS', 'THEREFORE', 'WITNESSETH', 'RESOLVED',
    'COVER', 'TABLE', 'CONTENTS', 'DEDICATION', 'PREFACE',
    'ENTITY', 'OVERVIEW', 'LEGAL', 'STRUCTURE', 'MISSION',
    'GOD', 'LORD', 'AMEN',
    'GEM_FONTS_DIR', 'CURDIR',
    'PHONY', 'FORCE', 'TRUE', 'FALSE',
    'README', 'REFACTOR',
  ]);

  for (let i = 0; i < lines.length; i++) {
    const line = lines[i]!;

    // Skip comments, headings, attributes, literal blocks
    if (line.trimStart().startsWith('//')) continue;
    if (line.startsWith('=')) continue;
    if (line.startsWith(':')) continue;
    if (line.startsWith('----') || line.startsWith('....')) continue;
    if (line.startsWith('|')) continue; // table rows often have caps

    // Find ALL CAPS words (4+ letters)
    const capsWords = line.match(/\b[A-Z]{4,}\b/g);
    if (!capsWords) continue;

    for (const word of capsWords) {
      if (knownAcronyms.has(word)) continue;
      // Skip if it's a filename pattern (like A-COVER.adoc)
      if (/^[A-Z]-[A-Z]+/.test(word)) continue;

      findings.push({
        severity: 'info',
        check: 'all-caps',
        category: 'style',
        message: `ALL CAPS "${word}" — use bold for emphasis instead`,
        line: i + 1,
        source: line.trim(),
      });
    }
  }
}

/** Check 12: [.gold] role on text content. */
function checkGoldText(lines: string[], findings: EditorialFinding[]): void {
  for (let i = 0; i < lines.length; i++) {
    if ((lines[i] ?? '').includes('[.gold]')) {
      findings.push({
        severity: 'info',
        check: 'gold-text',
        category: 'style',
        message: '[.gold] role used — gold (#D69E2E) fails WCAG on white; use for borders only',
        line: i + 1,
        source: (lines[i] ?? '').trim(),
      });
    }
  }
}

/**
 * Check 13: Register mixing — traditional + modern obligation words in same paragraph.
 * Config-driven: when legal_language config is available, builds word sets from
 * both registers to detect any cross-register mixing (not just shall/must).
 */
function checkRegisterMixing(lines: string[], findings: EditorialFinding[], legalConfig?: LegalLanguageConfig): void {
  // Build word sets from config or use defaults
  const traditionalWords = legalConfig
    ? [legalConfig.obligation.traditional.duty,
      legalConfig.obligation.traditional.performative,
      legalConfig.obligation.traditional.recitals,
      legalConfig.obligation.traditional.opening,
      ...legalConfig.obligation.traditional.self_reference]
    : ['shall', 'hereby', 'whereas', 'witnesseth', 'hereunder', 'herein', 'hereof'];

  const modernWords = legalConfig
    ? [legalConfig.obligation.modern.duty,
      legalConfig.obligation.modern.future,
      legalConfig.obligation.modern.permission,
      legalConfig.obligation.modern.recommendation,
      legalConfig.obligation.modern.prohibition]
    : ['must', 'will', 'may', 'should', 'may not'];

  // Build regex patterns (case-insensitive word boundaries)
  const traditionalRe = new RegExp(`\\b(${traditionalWords.join('|')})\\b`, 'i');
  const modernDutyRe = new RegExp(`\\b(${modernWords.filter(w => w !== 'will' && w !== 'may').join('|')})\\b`, 'i');

  // Build paragraphs (groups of lines separated by blank lines)
  let paragraphStart = -1;
  let paragraphLines: string[] = [];

  function checkParagraph(start: number, pLines: string[]): void {
    const text = pLines.join(' ');

    // Skip comments and Bible references
    if (/KJV|WEB|scripture|Bible/i.test(text)) return;
    if (pLines.every(l => l.trimStart().startsWith('//'))) return;

    // Skip lines inside tables (contain |)
    if (pLines.some(l => l.startsWith('|'))) return;

    const hasTraditional = traditionalRe.test(text);
    const hasModern = modernDutyRe.test(text);

    if (hasTraditional && hasModern) {
      // Find which specific words are mixing
      const tradFound = traditionalWords.filter(w => new RegExp(`\\b${w}\\b`, 'i').test(text));
      const modFound = modernWords.filter(w => w !== 'will' && w !== 'may' && new RegExp(`\\b${w}\\b`, 'i').test(text));

      findings.push({
        severity: 'warn',
        check: 'register-mixing',
        category: 'style',
        message: `Register mixing: traditional (${tradFound.join(', ')}) + modern (${modFound.join(', ')}) in same paragraph`,
        line: start + 1,
      });
    }
  }

  for (let i = 0; i < lines.length; i++) {
    const line = lines[i]!;

    if (line.trim() === '') {
      // End of paragraph
      if (paragraphLines.length > 0) {
        checkParagraph(paragraphStart, paragraphLines);
        paragraphLines = [];
        paragraphStart = -1;
      }
    } else {
      if (paragraphStart === -1) paragraphStart = i;
      paragraphLines.push(line);
    }
  }

  // Check final paragraph
  if (paragraphLines.length > 0) {
    checkParagraph(paragraphStart, paragraphLines);
  }
}


// =============================================================================
// Check Implementations — Structure (14-15)
// =============================================================================

/** Check 14: Unlabeled page breaks — <<< without // PDF p.N within 5 lines. */
function checkUnlabeledBreaks(lines: string[], findings: EditorialFinding[]): void {
  for (let i = 0; i < lines.length; i++) {
    if ((lines[i] ?? '').trim() !== '<<<') continue;

    // Look for a page comment within 5 lines above
    let found = false;
    const searchStart = Math.max(0, i - 5);
    for (let j = searchStart; j < i; j++) {
      if (/\/\/ (PDF p\.|end p\.|PAGE \d)/.test(lines[j] ?? '')) {
        found = true;
        break;
      }
    }

    // Also check the line immediately after (sometimes the comment is below)
    if (!found && i + 1 < lines.length) {
      if (/\/\/ (PDF p\.|end p\.|PAGE \d)/.test(lines[i + 1] ?? '')) {
        found = true;
      }
    }

    if (!found) {
      findings.push({
        severity: 'warn',
        check: 'unlabeled-breaks',
        category: 'structure',
        message: '<<< without page comment — every page break needs // PDF p.N',
        line: i + 1,
      });
    }
  }
}

/** Check 15: Cross-reference resolution — <<anchor>> without [[anchor]]. */
function checkXrefResolution(
  lines: string[],
  findings: EditorialFinding[],
  allAnchors: Set<string>,
): void {
  for (let i = 0; i < lines.length; i++) {
    const line = lines[i]!;

    // Skip comments
    if (line.trimStart().startsWith('//')) continue;

    // Find all <<anchor-id>> and <<anchor-id,text>> references
    const refs = line.matchAll(/<<([a-zA-Z0-9_-]+)(?:,[^>]*)?>>/g);
    for (const match of refs) {
      const anchor = match[1]!;

      // Skip known AsciiDoc built-in anchors
      if (anchor === 'toc' || anchor === 'top') continue;

      if (!allAnchors.has(anchor)) {
        findings.push({
          severity: 'warn',
          check: 'xref-resolution',
          category: 'structure',
          message: `Broken cross-reference <<${anchor}>> — no [[${anchor}]] anchor found`,
          line: i + 1,
        });
      }
    }
  }
}


// =============================================================================
// Check Implementations — Conversion Checklist (16-17)
// =============================================================================

/** Check 16: icon: macro usage (should be removed per conversion checklist). */
function checkIconMacros(lines: string[], findings: EditorialFinding[]): void {
  for (let i = 0; i < lines.length; i++) {
    const line = lines[i]!;

    // Skip comments
    if (line.trimStart().startsWith('//')) continue;

    if (/icon:[a-z-]+\[/.test(line)) {
      findings.push({
        severity: 'warn',
        check: 'icon-macros',
        category: 'conversion',
        message: 'icon: macro — replace with bold labels in centered text (conversion step 1)',
        line: i + 1,
        source: line.trim(),
      });
    }
  }
}

/** Check 17: Admonition blocks — all scopes (conversion step 3). */
function checkAdmonitionBlocks(lines: string[], findings: EditorialFinding[]): void {
  for (let i = 0; i < lines.length; i++) {
    const line = lines[i]!;

    // Skip comments
    if (line.trimStart().startsWith('//')) continue;

    if (/^(NOTE|TIP|IMPORTANT|WARNING|CAUTION):/.test(line)) {
      findings.push({
        severity: 'info',
        check: 'admonition-blocks',
        category: 'conversion',
        message: 'Admonition block — convert to centered text with bold labels (conversion step 3)',
        line: i + 1,
        source: line.trim(),
      });
    }
  }
}


// =============================================================================
// Check Implementations — Legal/Governance (18-20)
// =============================================================================

/**
 * Check 18: Definition verbs — validate that definitions use approved verbs.
 * Config-driven: reads approved verbs from legal_language.definition_verbs.
 *
 * Matches the editorial guide pattern: *"Term"* verb [definition].
 * Only triggers when a bold-quoted term is immediately followed by a
 * definition-like verb (means, includes, is, refers, denotes, etc.).
 * Flags verbs NOT in the approved list.
 */
function checkDefinitionVerbs(lines: string[], findings: EditorialFinding[], legalConfig: LegalLanguageConfig): void {
  if (!legalConfig) return;

  // Approved definition verbs from config (natural language forms)
  const approvedVerbs = ['means', 'includes', 'means and includes', 'does not include', 'refers to'];

  // Tight pattern: bold-quoted or quoted term followed immediately by a verb
  // Matches: *"Term"* verb, "*Term*" verb, "Term" verb
  // The verb capture is limited to 1-3 words to avoid runaway matches
  const defPattern = /(?:\*"[^"]+"\*|"\*[^*]+\*")\s+((?:means and includes|does not include|refers to|means|includes|is|are|denotes|designates|constitutes|shall mean|shall include))\b/i;

  for (let i = 0; i < lines.length; i++) {
    const line = lines[i]!;

    // Skip comments
    if (line.trimStart().startsWith('//')) continue;

    // Skip table rows, literal blocks
    if (line.startsWith('|') || line.startsWith('----') || line.startsWith('....')) continue;

    const match = defPattern.exec(line);
    if (!match) continue;

    const verb = match[1]!.trim().toLowerCase();

    // Check if the verb is in the approved list
    if (!approvedVerbs.some(v => verb === v)) {
      findings.push({
        severity: 'info',
        check: 'definition-verbs',
        category: 'legal',
        message: `Definition verb "${verb}" — approved: ${approvedVerbs.join(', ')}`,
        line: i + 1,
        source: line.trim(),
      });
    }
  }
}

/**
 * Check 19: Binding provisions — verify required binding language exists.
 * Config-driven: reads required patterns from legal_language.binding.locations.
 * Runs across ALL files in the lint scope (not per-file).
 *
 * Content is normalized: AsciiDoc line continuations (" +\n") and newlines
 * collapsed to spaces so multi-line phrases can be matched.
 */
function checkBindingProvisions(allContent: string, findings: EditorialFinding[], legalConfig: LegalLanguageConfig): void {
  if (!legalConfig?.binding) return;

  const binding = legalConfig.binding;
  let foundCount = 0;

  // Normalize content: collapse AsciiDoc line continuations and newlines
  const normalized = allContent
    .replace(/ \+\n/g, ' ')   // AsciiDoc line continuation
    .replace(/\n/g, ' ')       // Remaining newlines
    .replace(/\s+/g, ' ')      // Collapse whitespace
    .toLowerCase();

  for (const location of binding.locations) {
    const pattern = location.language.toLowerCase();
    if (normalized.includes(pattern)) {
      foundCount++;
      findings.push({
        severity: 'info',
        check: 'binding-provisions',
        category: 'legal',
        message: `Binding language found: "${location.language}" (${location.context})`,
        line: 0,
      });
    } else {
      findings.push({
        severity: 'warn',
        check: 'binding-provisions',
        category: 'legal',
        message: `Binding language MISSING: "${location.language}" (expected: ${location.context}, p.${location.page})`,
        line: 0,
      });
    }
  }

  // Summary
  if (foundCount < binding.required_instances) {
    findings.push({
      severity: 'warn',
      check: 'binding-provisions',
      category: 'legal',
      message: `Binding provisions: ${foundCount}/${binding.required_instances} required instances found`,
      line: 0,
    });
  } else {
    findings.push({
      severity: 'info',
      check: 'binding-provisions',
      category: 'legal',
      message: `Binding provisions: ${foundCount}/${binding.required_instances} instances — complete`,
      line: 0,
    });
  }
}

/**
 * Check 20: Boilerplate provisions — verify required general clauses exist.
 * Config-driven: reads required provision names from legal_language.boilerplate.required.
 * Searches for provision headings or key phrases across all content.
 */
function checkBoilerplate(allContent: string, findings: EditorialFinding[], legalConfig: LegalLanguageConfig): void {
  if (!legalConfig?.boilerplate) return;

  const contentLower = allContent.toLowerCase();
  let foundCount = 0;

  for (const provision of legalConfig.boilerplate.required) {
    // Convert snake_case to human-readable and search
    const searchTerm = provision.replace(/_/g, ' ');
    if (contentLower.includes(searchTerm)) {
      foundCount++;
      findings.push({
        severity: 'info',
        check: 'boilerplate',
        category: 'legal',
        message: `Boilerplate provision present: ${searchTerm}`,
        line: 0,
      });
    } else {
      findings.push({
        severity: 'warn',
        check: 'boilerplate',
        category: 'legal',
        message: `Boilerplate provision MISSING: ${searchTerm} — add to Document Governance`,
        line: 0,
      });
    }
  }

  // Summary
  const total = legalConfig.boilerplate.required.length;
  findings.push({
    severity: foundCount === total ? 'info' : 'warn',
    check: 'boilerplate',
    category: 'legal',
    message: `Boilerplate provisions: ${foundCount}/${total} present`,
    line: 0,
  });
}


// =============================================================================
// Check Implementations — Legal/Governance: Structure (21-26)
// =============================================================================

/**
 * Check 21: Governance prohibited terms — config-driven, per-file.
 * Reads from governance_notation.prohibited_terms.
 * Detects structural terminology that governance documents should avoid
 * (e.g., "Chapter" → use "Article" instead).
 */
function checkProhibitedTerms(lines: string[], findings: EditorialFinding[], legalConfig: LegalLanguageConfig): void {
  if (!legalConfig?.governance_notation?.prohibited_terms) return;

  for (const prohibited of legalConfig.governance_notation.prohibited_terms) {
    const termRe = new RegExp(`\\b${prohibited.term}\\b`);
    for (let i = 0; i < lines.length; i++) {
      const line = lines[i]!;

      // Skip comments, scripture, quotes, attributes, tables
      if (line.trimStart().startsWith('//')) continue;
      if (/KJV|WEB|scripture|Bible/i.test(line)) continue;
      if (line.includes('[quote') || line.includes('chapter-label')) continue;
      if (line.startsWith(':') || line.startsWith('|')) continue;

      if (termRe.test(line)) {
        findings.push({
          severity: 'warn',
          check: 'prohibited-terms',
          category: 'legal',
          message: `"${prohibited.term}" — ${prohibited.reason}`,
          line: i + 1,
          source: line.trim(),
        });
      }
    }
  }
}

/**
 * Check 22: Recitals style detection — cross-file.
 * Detects WHEREAS clauses and "Background" sections in document content.
 * Reports which recitals style is in use and whether it matches cim_default.
 */
function checkRecitalsStyle(allContent: string, findings: EditorialFinding[], legalConfig: LegalLanguageConfig): void {
  if (!legalConfig?.recitals) return;

  const hasWhereas = /\bWHEREAS\b/.test(allContent);
  const hasBackground = /^Background\b/m.test(allContent);
  const cimDefault = legalConfig.recitals.cim_default;

  if (hasWhereas) {
    findings.push({
      severity: 'info',
      check: 'recitals-style',
      category: 'legal',
      message: `Recitals: Traditional (WHEREAS) style detected${cimDefault === 'traditional' ? ' — matches CIM default' : ''}`,
      line: 0,
    });
  }

  if (hasBackground) {
    findings.push({
      severity: 'info',
      check: 'recitals-style',
      category: 'legal',
      message: `Recitals: Modern (Background) style detected${cimDefault === 'modern' ? ' — matches CIM default' : ''}`,
      line: 0,
    });
  }

  // WHEREAS-operative pairing (enhanced check 22)
  // If WHEREAS found, check for "NOW, THEREFORE" operative clause
  if (hasWhereas && legalConfig?.whereas_operative?.require_now_therefore) {
    const hasNowTherefore = /\bNOW,?\s*THEREFORE\b/i.test(allContent);
    if (!hasNowTherefore) {
      findings.push({
        severity: 'warn',
        check: 'whereas-operative',
        category: 'legal',
        message: 'WHEREAS recitals without "NOW, THEREFORE" — recitals need operative clauses',
        line: 0,
      });
    } else {
      findings.push({
        severity: 'info',
        check: 'whereas-operative',
        category: 'legal',
        message: 'WHEREAS-operative pairing: "NOW, THEREFORE" present — recitals properly concluded',
        line: 0,
      });
    }
  }

  if (!hasWhereas && !hasBackground) {
    findings.push({
      severity: 'info',
      check: 'recitals-style',
      category: 'legal',
      message: 'Recitals: No WHEREAS or Background recitals detected',
      line: 0,
    });
  }
}

/**
 * Check 23: Numbering convention detection — per-file.
 * Detects traditional (ARTICLE I, Section 1.01, romanettes) vs modern
 * (Article 1, § 1.1, Arabic sub-subsections) numbering patterns.
 * Reports which system is in use and whether it matches config default.
 */
function checkNumberingConvention(lines: string[], findings: EditorialFinding[], legalConfig: LegalLanguageConfig): void {
  if (!legalConfig?.numbering) return;

  let traditionalCount = 0;
  let modernCount = 0;

  for (let i = 0; i < lines.length; i++) {
    const line = lines[i]!;

    // Skip comments
    if (line.trimStart().startsWith('//')) continue;

    // Traditional patterns: ARTICLE I (Roman, caps), Section 1.01 (zero-padded)
    if (/\bARTICLE\s+[IVXLCDM]+\b/.test(line)) traditionalCount++;
    if (/\bSection\s+\d+\.\d{2}\b/.test(line)) traditionalCount++;

    // Modern patterns: Article 1 (Arabic), § 1.1
    if (/\bArticle\s+\d+\b/.test(line) && !/ARTICLE/.test(line)) modernCount++;
    if (/§\s*\d+\.\d+/.test(line)) modernCount++;
  }

  if (traditionalCount > 0 || modernCount > 0) {
    const expected = legalConfig.numbering.default;
    const detected = traditionalCount > modernCount ? 'traditional' : 'modern';
    const match = detected === expected;

    findings.push({
      severity: match ? 'info' : 'warn',
      check: 'numbering-convention',
      category: 'legal',
      message: `Numbering: ${detected} detected (${traditionalCount} traditional, ${modernCount} modern)${match ? ' — matches default' : ` — expected ${expected}`}`,
      line: 0,
    });
  }
}

/**
 * Check 24: Enumeration punctuation — per-file.
 * Validates tabulated enumeration lists using (a), (b), (c) patterns.
 * Checks: semicolons between items, conjunction before final item,
 * period after final item — per editorial guide enumeration conventions.
 */
function checkEnumerationPunctuation(lines: string[], findings: EditorialFinding[], legalConfig: LegalLanguageConfig): void {
  if (!legalConfig?.enumeration) return;

  const listPattern = /^\s*\([a-z]\)\s+/;
  let inList = false;
  let listItems: { line: number; text: string }[] = [];
  const threshold = legalConfig.enumeration.tabulated_threshold;
  const punct = legalConfig.enumeration.punctuation;

  function validateList(): void {
    if (listItems.length < threshold) return;

    // Check intermediate items end with semicolons
    for (let j = 0; j < listItems.length - 1; j++) {
      const item = listItems[j]!;
      const trimmed = item.text.trimEnd();
      const isSecondToLast = j === listItems.length - 2;

      if (isSecondToLast) {
        // Penultimate item should end with "; and" or "; or"
        if (punct.conjunction_before_last && !/;\s*(and|or)\s*$/.test(trimmed)) {
          findings.push({
            severity: 'info',
            check: 'enumeration-punct',
            category: 'legal',
            message: 'Enumeration: penultimate item should end with "; and" or "; or"',
            line: item.line + 1,
            source: item.text.trim().slice(0, 80),
          });
        }
      } else if (punct.separator === 'semicolon' && !trimmed.endsWith(';')) {
        findings.push({
          severity: 'info',
          check: 'enumeration-punct',
          category: 'legal',
          message: 'Enumeration: item should end with semicolon',
          line: item.line + 1,
          source: item.text.trim().slice(0, 80),
        });
      }
    }

    // Check final item ends with period
    const lastItem = listItems[listItems.length - 1]!;
    if (punct.final_period && !lastItem.text.trimEnd().endsWith('.')) {
      findings.push({
        severity: 'info',
        check: 'enumeration-punct',
        category: 'legal',
        message: 'Enumeration: final item should end with period',
        line: lastItem.line + 1,
        source: lastItem.text.trim().slice(0, 80),
      });
    }
  }

  for (let i = 0; i < lines.length; i++) {
    const line = lines[i]!;

    // Skip comments
    if (line.trimStart().startsWith('//')) continue;

    if (listPattern.test(line)) {
      if (!inList) {
        inList = true;
        listItems = [];
      }
      listItems.push({ line: i, text: line });
    } else if (inList && line.trim() === '') {
      // End of list — validate
      validateList();
      inList = false;
      listItems = [];
    } else if (inList) {
      // Continuation line — append to current item
      if (listItems.length > 0) {
        listItems[listItems.length - 1]!.text += ' ' + line;
      }
    }
  }

  // Check final list if file ends while in list
  if (inList) {
    validateList();
  }
}

/**
 * Check 25: Self-reference register consistency — per-file.
 * Detects traditional self-reference terms (hereunder, herein, hereof,
 * hereinafter) from cross_reference.traditional_self_reference.
 * Reports usage for register awareness — traditional terms are appropriate
 * in recitals and founding docs but should be flagged elsewhere.
 */
function checkSelfReference(lines: string[], findings: EditorialFinding[], legalConfig: LegalLanguageConfig): void {
  if (!legalConfig?.cross_reference?.traditional_self_reference) return;

  const traditionalTerms = legalConfig.cross_reference.traditional_self_reference;
  const termRe = new RegExp(`\\b(${traditionalTerms.join('|')})\\b`, 'i');
  const found: string[] = [];

  for (let i = 0; i < lines.length; i++) {
    const line = lines[i]!;

    // Skip comments and scripture
    if (line.trimStart().startsWith('//')) continue;
    if (/KJV|WEB|scripture|Bible/i.test(line)) continue;
    if (line.includes('[quote')) continue;

    const match = termRe.exec(line);
    if (match && !found.includes(match[1]!.toLowerCase())) {
      found.push(match[1]!.toLowerCase());
    }
  }

  if (found.length > 0) {
    findings.push({
      severity: 'info',
      check: 'self-reference',
      category: 'legal',
      message: `Traditional self-reference terms: ${found.join(', ')} — verify register is appropriate`,
      line: 0,
    });
  }
}

/**
 * Check 26: Document anatomy sequence — cross-file.
 * Verifies CIM subset elements (recitals → operative_provisions →
 * general_provisions) exist across the file set. Uses file paths for
 * ordering (A-frontmatter/ < B-content/ < C-appendices/) since directory
 * structure mirrors document anatomy.
 *
 * Reads from document_anatomy.cim_elements.
 */
function checkDocumentAnatomy(fileResults: EditorialFileResult[], findings: EditorialFinding[], legalConfig: LegalLanguageConfig): void {
  if (!legalConfig?.document_anatomy) return;

  const cimElements = legalConfig.document_anatomy.cim_elements;
  if (!cimElements || cimElements.length === 0) return;

  // Map CIM element names to file-based indicators
  // recitals: [preface] in a frontmatter file
  // operative_provisions: content files in B-content/
  // general_provisions: Document Governance heading in content
  const elementDetectors: Record<string, (results: EditorialFileResult[]) => string | null> = {
    'recitals': (results: EditorialFileResult[]): string | null => {
      for (const r of results) {
        if (r.path.includes('frontmatter') && r.path.includes('PREFACE')) {
          return r.file;
        }
      }
      return null;
    },
    'operative_provisions': (results: EditorialFileResult[]): string | null => {
      for (const r of results) {
        if (r.path.includes('B-content/')) {
          return r.file;
        }
      }
      return null;
    },
    'general_provisions': (results: EditorialFileResult[]): string | null => {
      // Look for Document Governance content in any file
      for (const r of results) {
        try {
          const content = readFileSync(r.path, 'utf-8');
          if (/Document Governance/i.test(content) && r.path.includes('B-content/')) {
            return r.file;
          }
        } catch { /* skip */ }
      }
      return null;
    },
  };

  const detected: string[] = [];
  for (const element of cimElements) {
    const detector = elementDetectors[element];
    if (!detector) continue;

    const foundFile = detector(fileResults);
    if (foundFile) {
      detected.push(element);
      findings.push({
        severity: 'info',
        check: 'document-anatomy',
        category: 'legal',
        message: `CIM anatomy: "${element}" present (${foundFile})`,
        line: 0,
      });
    } else {
      findings.push({
        severity: 'info',
        check: 'document-anatomy',
        category: 'legal',
        message: `CIM anatomy: "${element}" not detected`,
        line: 0,
      });
    }
  }

  // Summary
  if (detected.length === cimElements.length) {
    findings.push({
      severity: 'info',
      check: 'document-anatomy',
      category: 'legal',
      message: `CIM anatomy: all ${cimElements.length} elements present (${cimElements.join(' → ')})`,
      line: 0,
    });
  } else {
    findings.push({
      severity: 'info',
      check: 'document-anatomy',
      category: 'legal',
      message: `CIM anatomy: ${detected.length}/${cimElements.length} elements detected`,
      line: 0,
    });
  }
}


// =============================================================================
// Tone & Voice Checks (27-29)
// =============================================================================

/**
 * Check 27: Compulsion Language (tone, per-file)
 *
 * Flags compulsion patterns like "must comply", "failure to comply" and
 * suggests covenant alternatives from config. CWS governance uses relational
 * language, not fear-based.
 */
function checkCompulsionLanguage(lines: string[], findings: EditorialFinding[], toneConfig?: ToneAndVoiceConfig): void {
  if (!toneConfig?.covenant_language?.replacements) return;

  const replacements = toneConfig.covenant_language.replacements;
  for (const replacement of replacements) {
    const pattern = new RegExp(replacement.compulsion.replace(/[.*+?^${}()|[\]\\]/g, '\\$&'), 'i');
    for (let i = 0; i < lines.length; i++) {
      const line = lines[i]!;

      // Skip comments and attribute lines
      if (line.startsWith('//') || line.startsWith(':')) continue;

      if (pattern.test(line)) {
        findings.push({
          severity: 'warn',
          check: 'compulsion-language',
          category: 'tone',
          message: `Compulsion language: "${replacement.compulsion}" → consider covenant: "${replacement.covenant}"`,
          line: i + 1,
        });
      }
    }
  }
}

/**
 * Check 28: Plain Language Words (tone, per-file)
 *
 * Flags words like "utilize", "facilitate", "notwithstanding" and suggests
 * simpler alternatives from config.
 */
function checkPlainLanguageWords(lines: string[], findings: EditorialFinding[], toneConfig?: ToneAndVoiceConfig): void {
  if (!toneConfig?.plain_language?.word_replacements) return;

  const replacements = toneConfig.plain_language.word_replacements;
  for (const replacement of replacements) {
    // Match whole word/phrase (case-insensitive)
    const escaped = replacement.avoid.replace(/[.*+?^${}()|[\]\\]/g, '\\$&');
    const pattern = new RegExp(`\\b${escaped}\\b`, 'i');
    for (let i = 0; i < lines.length; i++) {
      const line = lines[i]!;

      // Skip comments and attribute lines
      if (line.startsWith('//') || line.startsWith(':')) continue;

      if (pattern.test(line)) {
        findings.push({
          severity: 'warn',
          check: 'plain-language-words',
          category: 'tone',
          message: `Plain language: "${replacement.avoid}" → prefer "${replacement.prefer}"`,
          line: i + 1,
        });
      }
    }
  }
}

/**
 * Check 29: Sentence Length (tone, per-file)
 *
 * Flags sentences exceeding max_sentence_words (default 50). Counts words
 * in prose lines, handling AsciiDoc line continuation (` +` suffix).
 */
function checkSentenceLength(lines: string[], findings: EditorialFinding[], toneConfig?: ToneAndVoiceConfig): void {
  const maxWords = toneConfig?.plain_language?.max_sentence_words ?? 50;

  // Accumulate logical lines (handle AsciiDoc line continuation ` +`)
  let currentSentence = '';
  let sentenceStartLine = 0;
  let inComment = false;

  for (let i = 0; i < lines.length; i++) {
    const line = lines[i]!;

    // Track AsciiDoc block comment boundaries
    if (line.trim() === '////') {
      inComment = !inComment;
      continue;
    }
    if (inComment) continue;

    // Skip structural lines
    if (line.startsWith('//') || line.startsWith(':') ||
      line.startsWith('=') || line.startsWith('[') ||
      line.startsWith('|') || line.startsWith('.') ||
      line.startsWith('image:') || line.startsWith('include:') ||
      line.startsWith('ifdef::') || line.startsWith('endif::') ||
      line.startsWith('<<<') || line.trim() === '') {
      // End of prose block — check accumulated sentence
      if (currentSentence) {
        checkSentenceWordCount(currentSentence, sentenceStartLine, maxWords, findings);
        currentSentence = '';
      }
      continue;
    }

    // Prose line — accumulate
    if (!currentSentence) {
      sentenceStartLine = i + 1;
    }

    // Handle line continuation (` +` at end)
    if (line.endsWith(' +')) {
      currentSentence += ' ' + line.slice(0, -2).trim();
    } else {
      currentSentence += ' ' + line.trim();
    }

    // Check for sentence-ending punctuation
    const sentences = currentSentence.split(/[.!?]+/);

    // Check all complete sentences (all but the last fragment)
    for (let s = 0; s < sentences.length - 1; s++) {
      const sentence = sentences[s]!.trim();
      if (sentence) {
        checkSentenceWordCount(sentence, sentenceStartLine, maxWords, findings);
      }
    }

    // Keep the last fragment (incomplete sentence)
    const lastFragment = sentences[sentences.length - 1]!;
    if (lastFragment.trim()) {
      currentSentence = lastFragment;
    } else {
      currentSentence = '';
    }
  }

  // Check any remaining accumulated sentence
  if (currentSentence) {
    checkSentenceWordCount(currentSentence, sentenceStartLine, maxWords, findings);
  }
}

/** Helper: check word count for a single sentence. */
function checkSentenceWordCount(sentence: string, line: number, maxWords: number, findings: EditorialFinding[]): void {
  const words = sentence.trim().split(/\s+/).filter(w => w.length > 0);
  if (words.length > maxWords) {
    const preview = words.slice(0, 8).join(' ') + '...';
    findings.push({
      severity: 'warn',
      check: 'sentence-length',
      category: 'tone',
      message: `Sentence too long: ${words.length} words (max ${maxWords}). Starts: "${preview}"`,
      line,
    });
  }
}


// =============================================================================
// Industry-Specific Checks (30-31)
// =============================================================================

/**
 * Check 30: Document Register Consistency (industry, per-file)
 *
 * When --document-type is specified, checks that the file's register
 * is consistent with the expected register for that document type.
 * For example, a formal_traditional document shouldn't use "we" or "our team",
 * while an informal document shouldn't use "WHEREAS" or "hereby".
 */
function checkDocumentRegister(lines: string[], findings: EditorialFinding[], register?: string): void {
  if (!register) return;

  // Register-specific patterns that indicate mismatch
  const formalPatterns = [
    { pattern: /\bWHEREAS\b/, label: 'WHEREAS' },
    { pattern: /\bhereby\b/i, label: 'hereby' },
    { pattern: /\bhereinafter\b/i, label: 'hereinafter' },
    { pattern: /\baforesaid\b/i, label: 'aforesaid' },
  ];

  const informalPatterns = [
    { pattern: /\bour team\b/i, label: 'our team' },
    { pattern: /\bhey\b/i, label: 'hey' },
    { pattern: /\bawesome\b/i, label: 'awesome' },
    { pattern: /\bcool\b/i, label: 'cool' },
  ];

  for (let i = 0; i < lines.length; i++) {
    const line = lines[i]!;
    if (line.startsWith('//') || line.startsWith(':')) continue;

    // Informal/warm/plain docs shouldn't have formal legal language
    if (register === 'informal' || register === 'warm' || register === 'plain') {
      for (const fp of formalPatterns) {
        if (fp.pattern.test(line)) {
          findings.push({
            severity: 'warn',
            check: 'document-register',
            category: 'industry',
            message: `Register mismatch: "${fp.label}" is formal language in a ${register} document`,
            line: i + 1,
          });
        }
      }
    }

    // Formal docs shouldn't have casual language
    if (register === 'formal_traditional' || register === 'formal_covenant') {
      for (const ip of informalPatterns) {
        if (ip.pattern.test(line)) {
          findings.push({
            severity: 'warn',
            check: 'document-register',
            category: 'industry',
            message: `Register mismatch: "${ip.label}" is informal language in a ${register} document`,
            line: i + 1,
          });
        }
      }
    }
  }
}

/**
 * Check 31: Creator Agreement Provisions (industry, cross-file)
 *
 * When --document-type is creator_agreement, verifies that all required
 * provisions from config are present in the document. Non-negotiable
 * provisions get error severity.
 */
function checkCreatorProvisions(allContent: string, findings: EditorialFinding[], industryConfig?: IndustrySpecificConfig): void {
  if (!industryConfig?.publishing_house?.creator_agreements) return;

  const provisions = industryConfig.publishing_house.creator_agreements;

  // Normalize content for matching (collapse line continuations)
  const normalized = allContent.replace(/ \+\n/g, ' ');

  for (const provision of provisions) {
    // Convert provision name to searchable patterns
    const provisionLabel = provision.provision.replace(/_/g, ' ');
    const pattern = new RegExp(provisionLabel.replace(/[.*+?^${}()|[\]\\]/g, '\\$&'), 'i');

    if (pattern.test(normalized)) {
      findings.push({
        severity: 'info',
        check: 'creator-provisions',
        category: 'industry',
        message: `Creator provision present: ${provisionLabel}`,
        line: 0,
      });
    } else {
      findings.push({
        severity: provision.non_negotiable ? 'warn' : 'info',
        check: 'creator-provisions',
        category: 'industry',
        message: `Creator provision ${provision.non_negotiable ? 'REQUIRED' : 'recommended'} but not found: ${provisionLabel}`,
        line: 0,
      });
    }
  }
}


// =============================================================================
// Check Implementations — AsciiDoc Publishing Structure (32-35)
// =============================================================================

/**
 * Check 32: Prohibited attributes — per-file.
 * Config-driven: reads from asciidoc_publishing.prohibited_attributes.
 * Flags document attributes that must not appear (e.g., :preface-title:).
 */
function checkProhibitedAttributes(lines: string[], findings: EditorialFinding[], publishingConfig?: AsciidocPublishingConfig): void {
  if (!publishingConfig?.prohibited_attributes) return;

  for (const prohibited of publishingConfig.prohibited_attributes) {
    for (let i = 0; i < lines.length; i++) {
      const line = lines[i]!;

      // Skip comments
      if (line.trimStart().startsWith('//')) continue;

      if (line.includes(prohibited.attribute)) {
        findings.push({
          severity: 'warn',
          check: 'prohibited-attribute',
          category: 'structure',
          message: `Prohibited attribute ${prohibited.attribute} — ${prohibited.reason} (${prohibited.guide_ref})`,
          line: i + 1,
          source: line.trim(),
        });
      }
    }
  }
}

/**
 * Check 33: Redundant page break — per-file.
 * Config-driven: reads from asciidoc_publishing.page_flow.
 * In doctype:book, == headings force page breaks automatically.
 * A <<< adjacent to a non-discrete == heading is redundant.
 *
 * Looks forward from <<< for next non-blank/non-comment line.
 * If it's `== ` (not `=== `), flags as warn (redundant before chapter).
 * Also looks backward — if `== ` preceded the `<<<`, flags as info
 * (redundant after chapter).
 * Skips [discrete] headings — they don't force page breaks.
 */
function checkRedundantPageBreak(lines: string[], findings: EditorialFinding[], publishingConfig?: AsciidocPublishingConfig): void {
  if (!publishingConfig?.page_flow) return;

  const flow = publishingConfig.page_flow;
  for (let i = 0; i < lines.length; i++) {
    if ((lines[i] ?? '').trim() !== '<<<') continue;

    // --- Forward check: <<< before == heading ---
    if (flow.redundant_break_before_chapter) {
      let nextContentIdx = -1;
      for (let j = i + 1; j < lines.length && j <= i + 5; j++) {
        const l = (lines[j] ?? '').trim();
        if (l === '' || l.startsWith('//')) continue;
        nextContentIdx = j;
        break;
      }

      if (nextContentIdx >= 0) {
        const nextLine = lines[nextContentIdx]!;
        // Check for == heading (chapter level) but NOT === or deeper
        if (/^== [^=]/.test(nextLine)) {
          // Verify it's not preceded by [discrete] on the line before
          const lineBefore = (lines[nextContentIdx - 1] ?? '').trim();
          if (lineBefore !== '[discrete]') {
            findings.push({
              severity: 'warn',
              check: 'redundant-page-break',
              category: 'structure',
              message: '<<< before == heading — redundant in doctype:book (== forces page break)',
              line: i + 1,
            });
          }
        }
      }
    }

    // --- Backward check: <<< after == heading ---
    if (flow.redundant_break_after_chapter) {
      let prevContentIdx = -1;
      for (let j = i - 1; j >= 0 && j >= i - 5; j--) {
        const l = (lines[j] ?? '').trim();
        if (l === '' || l.startsWith('//')) continue;
        prevContentIdx = j;
        break;
      }

      if (prevContentIdx >= 0) {
        const prevLine = lines[prevContentIdx]!;
        // Check for == heading (chapter level) but NOT === or deeper
        if (/^== [^=]/.test(prevLine)) {
          // Verify it's not preceded by [discrete]
          const lineBeforePrev = (lines[prevContentIdx - 1] ?? '').trim();
          if (lineBeforePrev !== '[discrete]') {
            findings.push({
              severity: 'info',
              check: 'redundant-page-break',
              category: 'structure',
              message: '<<< after == heading — may be redundant (== already started a new page)',
              line: i + 1,
            });
          }
        }
      }
    }
  }
}

/**
 * Check 34: Include guard — per-file.
 * Config-driven: reads from asciidoc_publishing.include_guard.
 * Files in required scopes should have ifndef::book[] wrapper
 * for standalone rendering compatibility.
 *
 * Uses detectScope() from errors.ts to determine file scope.
 * Checks against include_guard.required_scopes and exclude_patterns.
 */
function checkIncludeGuard(lines: string[], findings: EditorialFinding[], filePath: string, publishingConfig?: AsciidocPublishingConfig): void {
  if (!publishingConfig?.include_guard) return;

  const guard = publishingConfig.include_guard;
  const fileName = basename(filePath);
  const dirPath = dirname(filePath);

  // Check if file is excluded
  for (const pattern of guard.exclude_patterns) {
    // Simple glob matching: * → .*, handle literal matches
    const regex = new RegExp('^' + pattern.replace(/\*/g, '.*') + '$');
    if (regex.test(fileName)) return;
  }

  // Check if file is in a required scope
  const scope = detectScope(dirPath);
  if (!scope || !guard.required_scopes.includes(scope)) return;

  // Look for ifndef::book[] in the file
  const hasGuard = lines.some(l => l.includes('ifndef::book[]'));
  if (!hasGuard) {
    findings.push({
      severity: 'info',
      check: 'include-guard',
      category: 'structure',
      message: `File in ${scope} scope lacks ifndef::book[] — needed for standalone rendering`,
      line: 0,
    });
  }
}

/**
 * Check 35: Section role placement — per-file.
 * Config-driven: reads from asciidoc_publishing.section_roles.
 *
 * For each configured role (dedication, preface):
 *   1. Finds [role] lines in the file
 *   2. Validates filename matches file_pattern
 *   3. If requires_ifdef_book, checks that ifdef::book[] appears within 5 lines above
 */
function checkSectionRolePlacement(lines: string[], findings: EditorialFinding[], filePath: string, publishingConfig?: AsciidocPublishingConfig): void {
  if (!publishingConfig?.section_roles) return;

  const fileName = basename(filePath);
  for (const roleDef of publishingConfig.section_roles) {
    const roleMarker = `[${roleDef.role}]`;
    for (let i = 0; i < lines.length; i++) {
      const line = (lines[i] ?? '').trim();

      // Skip comments
      if (line.startsWith('//')) continue;

      if (line === roleMarker) {
        // Found role marker — validate filename match
        const filePattern = new RegExp('^' + roleDef.file_pattern.replace(/\*/g, '.*') + '$', 'i');
        if (!filePattern.test(fileName)) {
          findings.push({
            severity: 'warn',
            check: 'section-role-placement',
            category: 'structure',
            message: `[${roleDef.role}] found in ${fileName} — expected in files matching ${roleDef.file_pattern}`,
            line: i + 1,
          });
        }

        // Check for ifdef::book[] guard within 5 lines above
        if (roleDef.requires_ifdef_book) {
          let hasIfdef = false;
          const searchStart = Math.max(0, i - 5);
          for (let j = searchStart; j < i; j++) {
            if ((lines[j] ?? '').includes('ifdef::book[]')) {
              hasIfdef = true;
              break;
            }
          }
          if (!hasIfdef) {
            findings.push({
              severity: 'info',
              check: 'section-role-placement',
              category: 'structure',
              message: `[${roleDef.role}] without ifdef::book[] guard within 5 lines — role may need book context`,
              line: i + 1,
            });
          }
        }
      }
    }
  }
}


// =============================================================================
// Check Implementations — Company Conventions (36, 47)
// =============================================================================

/**
 * Check 36: Attribute usage — raw company name should use {company-name} attribute.
 * Config-driven: reads raw_name_variants from company_conventions.
 */
function checkAttributeUsage(lines: string[], findings: EditorialFinding[], companyConfig?: CompanyConventionsConfig): void {
  if (!companyConfig?.raw_name_variants) return;

  for (let i = 0; i < lines.length; i++) {
    const line = lines[i]!;

    // Skip comments and attribute definitions
    if (line.trimStart().startsWith('//')) continue;
    if (line.startsWith(':company-name:')) continue;
    if (line.startsWith(':company-short:')) continue;
    if (line.startsWith(':')) continue;

    // Skip table rows and listing blocks
    if (line.startsWith('|') || line.startsWith('----')) continue;

    for (const variant of companyConfig.raw_name_variants) {
      if (line.includes(variant)) {
        findings.push({
          severity: 'info',
          check: 'attribute-usage',
          category: 'editorial',
          message: `Raw company name "${variant}" — use ${companyConfig.attribute_name} attribute instead`,
          line: i + 1,
          source: line.trim(),
        });
        break; // One finding per line
      }
    }
  }
}

/**
 * Check 47: Board member reference — verify names match canonical list.
 * Config-driven: reads board_members from company_conventions.
 * When a board member name appears, checks that canonical title is nearby.
 */
function checkBoardReference(lines: string[], findings: EditorialFinding[], companyConfig?: CompanyConventionsConfig): void {
  if (!companyConfig?.board_members) return;

  for (const member of companyConfig.board_members) {
    for (let i = 0; i < lines.length; i++) {
      const line = lines[i]!;

      // Skip comments
      if (line.trimStart().startsWith('//')) continue;

      if (line.includes(member.name)) {
        // Check if title appears on same line or within 3 lines
        let titleFound = false;
        const searchEnd = Math.min(lines.length, i + 4);
        for (let j = i; j < searchEnd; j++) {
          if ((lines[j] ?? '').includes(member.title)) {
            titleFound = true;
            break;
          }
        }

        if (!titleFound) {
          findings.push({
            severity: 'info',
            check: 'board-reference',
            category: 'editorial',
            message: `Board member "${member.name}" — canonical title: "${member.title}"`,
            line: i + 1,
          });
        }
      }
    }
  }
}


// =============================================================================
// Check Implementations — Style Anti-Patterns (37-40)
// =============================================================================

/**
 * Check 37: Closing filler — generic transition phrases.
 * Config-driven: reads phrases from tone_and_voice.closing_filler.
 */
function checkClosingFiller(lines: string[], findings: EditorialFinding[], toneConfig?: ToneAndVoiceConfig): void {
  if (!toneConfig?.closing_filler?.phrases) return;

  for (const phrase of toneConfig.closing_filler.phrases) {
    const pattern = new RegExp(phrase.replace(/[.*+?^${}()|[\]\\]/g, '\\$&'), 'i');
    for (let i = 0; i < lines.length; i++) {
      const line = lines[i]!;

      // Skip comments, attributes, tables
      if (line.trimStart().startsWith('//')) continue;
      if (line.startsWith(':') || line.startsWith('|')) continue;

      if (pattern.test(line)) {
        findings.push({
          severity: 'info',
          check: 'closing-filler',
          category: 'style',
          message: `Closing filler: "${phrase}" — use meaningful governance transition`,
          line: i + 1,
          source: line.trim(),
        });
      }
    }
  }
}

/**
 * Check 38: Paragraph density — consecutive non-blank content lines.
 * Config-driven: reads max_consecutive_lines from tone_and_voice.paragraph_density.
 */
function checkParagraphDensity(lines: string[], findings: EditorialFinding[], toneConfig?: ToneAndVoiceConfig): void {
  const maxLines = toneConfig?.paragraph_density?.max_consecutive_lines ?? 6;
  let runStart = -1;
  let runLength = 0;

  for (let i = 0; i < lines.length; i++) {
    const line = lines[i]!;

    // Structural lines break a run
    const isStructural = line.trim() === '' ||
      line.startsWith('=') ||
      line.startsWith('[') ||
      line.startsWith('|') ||
      line.startsWith(':') ||
      line.startsWith('.') ||
      line.startsWith('//') ||
      line.startsWith('<<<') ||
      line.startsWith('----') ||
      line.startsWith('....') ||
      line.startsWith('image:') ||
      line.startsWith('include:') ||
      line.startsWith('ifdef::') ||
      line.startsWith('endif::');

    if (isStructural) {
      // End of run — check if it exceeded threshold
      if (runLength > maxLines) {
        findings.push({
          severity: 'info',
          check: 'paragraph-density',
          category: 'style',
          message: `Dense paragraph block (${runLength} lines) — consider breaking into shorter units`,
          line: runStart + 1,
        });
      }
      runStart = -1;
      runLength = 0;
    } else {
      if (runStart === -1) runStart = i;
      runLength++;
    }
  }

  // Check final run
  if (runLength > maxLines) {
    findings.push({
      severity: 'info',
      check: 'paragraph-density',
      category: 'style',
      message: `Dense paragraph block (${runLength} lines) — consider breaking into shorter units`,
      line: runStart + 1,
    });
  }
}

/**
 * Check 39: Redundant logo — more than one logo image reference.
 * Logo should appear once on title page only.
 */
function checkRedundantLogo(lines: string[], findings: EditorialFinding[]): void {
  let logoCount = 0;
  for (let i = 0; i < lines.length; i++) {
    const line = lines[i]!;

    // Skip comments
    if (line.trimStart().startsWith('//')) continue;

    if (/image::.*logo/i.test(line)) {
      logoCount++;
    }
  }
  if (logoCount > 1) {
    findings.push({
      severity: 'info',
      check: 'redundant-logo',
      category: 'style',
      message: `Multiple logo references (${logoCount}) — one logo on title page only`,
      line: 0,
    });
  }
}

/**
 * Check 40: Color accessibility — color role without companion formatting.
 * Color alone is insufficient signal; pair with bold, border, or text label.
 */
function checkColorAccessibility(lines: string[], findings: EditorialFinding[]): void {
  const colorRoles = ['gold', 'blue', 'green', 'red', 'purple'];
  for (let i = 0; i < lines.length; i++) {
    const line = lines[i]!;

    // Skip comments
    if (line.trimStart().startsWith('//')) continue;

    for (const role of colorRoles) {
      const rolePattern = new RegExp(`\\[\\.${role}\\]`);
      if (!rolePattern.test(line)) continue;

      // Check if same line or next line has companion formatting
      const context = line + ' ' + (lines[i + 1] ?? '');
      const hasCompanion = /\*[^*]+\*|_[^_]+_|\*\*[^*]+\*\*/.test(context);

      if (!hasCompanion) {
        findings.push({
          severity: 'info',
          check: 'color-accessibility',
          category: 'style',
          message: `Color role [.${role}] without companion signal — pair with bold, border, or text label`,
          line: i + 1,
          source: line.trim(),
        });
      }
    }
  }
}


// =============================================================================
// Check Implementations — Structure (41-44)
// =============================================================================

/**
 * Check 41: Frontmatter alignment — ceremonial content should use [.text-center].
 * Only applies to frontmatter cover and dedication files.
 */
function checkFrontmatterAlignment(lines: string[], findings: EditorialFinding[], filePath: string): void {
  const scope = detectScope(dirname(filePath));
  if (scope !== 'frontmatter') return;

  const sectionType = detectSectionType(basename(filePath), dirname(filePath));
  const ceremonialTypes = new Set(['cover', 'dedication']);
  if (!ceremonialTypes.has(sectionType)) return;

  // Count plain text lines and [.text-center] occurrences
  let plainTextCount = 0;
  let centerCount = 0;

  for (let i = 0; i < lines.length; i++) {
    const line = lines[i]!;
    if (line.includes('[.text-center]')) {
      centerCount++;
      continue;
    }

    // Skip structural lines
    if (line.trim() === '' || line.startsWith('=') || line.startsWith('[') ||
      line.startsWith(':') || line.startsWith('//') || line.startsWith('|') ||
      line.startsWith('<<<') || line.startsWith('image:') ||
      line.startsWith('include:') || line.startsWith('ifdef::') ||
      line.startsWith('endif::') || line.startsWith('ifndef::') ||
      line.startsWith('----') || line.startsWith('....')) {
      continue;
    }

    plainTextCount++;
  }

  if (plainTextCount > 3 && centerCount === 0) {
    findings.push({
      severity: 'info',
      check: 'frontmatter-alignment',
      category: 'structure',
      message: 'Frontmatter file without [.text-center] — ceremonial content should be centered',
      line: 0,
    });
  }
}

/**
 * Check 42: TOC table — table markup in TOC/navigation files.
 * TOC should use centered paragraph-style listings, not tables.
 */
function checkTocTable(lines: string[], findings: EditorialFinding[], filePath: string): void {
  const sectionType = detectSectionType(basename(filePath), dirname(filePath));
  if (sectionType !== 'toc') return;

  for (let i = 0; i < lines.length; i++) {
    const line = lines[i]!;

    // Skip comments
    if (line.trimStart().startsWith('//')) continue;

    if (line.trim() === '|===') {
      findings.push({
        severity: 'warn',
        check: 'toc-table',
        category: 'structure',
        message: 'Table markup in TOC file — use centered paragraph-style listings instead',
        line: i + 1,
      });
    }
  }
}

/**
 * Check 43: Subordinate heading — multiple == headings in sequence.
 * In doctype:book, == forces a page break. Sequential == headings
 * may indicate some should be === (subordinate sections).
 */
function checkSubordinateHeading(lines: string[], findings: EditorialFinding[]): void {
  let lastChapterLine = -1;

  for (let i = 0; i < lines.length; i++) {
    const line = lines[i]!;

    // Skip comments
    if (line.trimStart().startsWith('//')) continue;

    // Check for == heading (chapter level, not deeper)
    if (/^== [^=]/.test(line)) {
      // Check if preceded by [discrete] — discrete headings don't force page breaks
      const prevLine = (lines[i - 1] ?? '').trim();
      if (prevLine === '[discrete]') continue;

      // If we already saw a == heading in this file, flag this one
      if (lastChapterLine >= 0) {
        findings.push({
          severity: 'info',
          check: 'subordinate-heading',
          category: 'structure',
          message: 'Multiple == headings in sequence — consider === for subordinate sections (== forces page break in book)',
          line: i + 1,
          source: line.trim(),
        });
      }

      lastChapterLine = i;
    }

    // Part headings (= ) reset the counter — multiple == under different parts is fine
    if (/^= [^=]/.test(line)) {
      lastChapterLine = -1;
    }
  }
}

/**
 * Check 44: Appendix duplication — include directives pointing to appendix/backmatter
 * in TOC files. Appendix content belongs in its own scope.
 */
function checkAppendixDuplication(lines: string[], findings: EditorialFinding[], filePath: string): void {
  const sectionType = detectSectionType(basename(filePath), dirname(filePath));
  if (sectionType !== 'toc') return;

  for (let i = 0; i < lines.length; i++) {
    const line = lines[i]!;

    // Skip comments
    if (line.trimStart().startsWith('//')) continue;

    if (/include::.*(?:C-appendices|D-backmatter)/i.test(line)) {
      findings.push({
        severity: 'warn',
        check: 'appendix-duplication',
        category: 'structure',
        message: 'Include of appendix/backmatter content in TOC file — appendix content belongs in its own scope',
        line: i + 1,
        source: line.trim(),
      });
    }
  }
}


// =============================================================================
// Check Implementations — Legal (45) + Tone (46)
// =============================================================================

/**
 * Check 45: Archaic context — archaic legal terms outside traditional register.
 * Config-driven: reads archaic_terms from legal_language.
 */
function checkArchaicContext(lines: string[], findings: EditorialFinding[], legalConfig: LegalLanguageConfig, register?: string): void {
  if (!legalConfig?.archaic_terms) return;

  for (const entry of legalConfig.archaic_terms) {
    const termRe = new RegExp(`\\b${entry.term.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')}\\b`, 'i');
    for (let i = 0; i < lines.length; i++) {
      const line = lines[i]!;

      // Skip comments, scripture, tables
      if (line.trimStart().startsWith('//')) continue;
      if (/KJV|WEB|scripture|Bible/i.test(line)) continue;
      if (line.startsWith('|') || line.startsWith(':')) continue;

      if (termRe.test(line)) {
        const registerNote = register && register !== 'formal_traditional'
          ? ` (document register: ${register} — not traditional)`
          : '';
        findings.push({
          severity: 'info',
          check: 'archaic-context',
          category: 'legal',
          message: `Archaic term "${entry.term}" — ${entry.reason}${registerNote}`,
          line: i + 1,
          source: line.trim(),
        });
      }
    }
  }
}

/**
 * Check 46: Passive governance — passive voice patterns in governance text.
 * Config-driven: reads patterns from tone_and_voice.passive_voice.
 */
function checkPassiveGovernance(lines: string[], findings: EditorialFinding[], toneConfig?: ToneAndVoiceConfig): void {
  if (!toneConfig?.passive_voice?.enabled) return;

  const patterns = toneConfig.passive_voice.patterns.map((p: string) => {
    try {
      return new RegExp(p, 'i');
    } catch {
      return null;
    }
  }).filter((p: RegExp | null): p is RegExp => p !== null);

  for (let i = 0; i < lines.length; i++) {
    const line = lines[i]!;

    // Skip comments, scripture, tables, attributes
    if (line.trimStart().startsWith('//')) continue;
    if (/KJV|WEB|scripture|Bible/i.test(line)) continue;
    if (line.startsWith('|') || line.startsWith(':')) continue;
    if (line.includes('[quote')) continue;

    for (const pattern of patterns) {
      const match = pattern.exec(line);
      if (match) {
        findings.push({
          severity: 'info',
          check: 'passive-governance',
          category: 'tone',
          message: `Passive voice: "${match[0]}" — use active: name the actor`,
          line: i + 1,
          source: line.trim(),
        });
        break; // One finding per line
      }
    }
  }
}


// =============================================================================
// Check Implementations — Conversion Completion (48-49)
// =============================================================================

/** Check 48: Sidebar blocks — conversion step 2. */
function checkSidebarBlocks(lines: string[], findings: EditorialFinding[]): void {
  for (let i = 0; i < lines.length; i++) {
    const line = lines[i]!;
    if (line.trimStart().startsWith('//')) continue;

    if (line.trim() === '[sidebar]') {
      findings.push({
        severity: 'warn',
        check: 'sidebar-blocks',
        category: 'conversion',
        message: 'Sidebar block — convert to centered flowing text with thematic breaks (conversion step 2)',
        line: i + 1,
        source: line.trim(),
      });
    }
  }
}

/** Check 49: Icon tables — icon macros inside table blocks (conversion step 4). */
function checkIconTables(lines: string[], findings: EditorialFinding[]): void {
  let inTable = false;
  let tableStartLine = 0;
  let iconCount = 0;

  for (let i = 0; i < lines.length; i++) {
    const line = lines[i]!;
    if (line.trimStart().startsWith('//')) continue;

    if (line.trim() === '|===') {
      if (!inTable) {
        inTable = true;
        tableStartLine = i + 1;
        iconCount = 0;
      } else {
        // End of table — report if icons found
        if (iconCount > 0) {
          findings.push({
            severity: 'info',
            check: 'icon-tables',
            category: 'conversion',
            message: `Icon table (${iconCount} icon macros) — convert to centered paragraph listing (conversion step 4)`,
            line: tableStartLine,
          });
        }
        inTable = false;
      }
      continue;
    }

    if (inTable && /icon:[a-z-]+\[/.test(line)) {
      iconCount++;
    }
  }
}


// =============================================================================
// Check Implementations — Structure: Orphans & Images (50-51)
// =============================================================================

/** Check 50: Orphaned heading — heading with no content before page break or EOF. */
function checkOrphanedHeading(lines: string[], findings: EditorialFinding[]): void {
  for (let i = 0; i < lines.length; i++) {
    const line = lines[i]!;
    if (line.trimStart().startsWith('//')) continue;

    if (!/^=+\s+\S/.test(line)) continue;

    // Look ahead up to 3 lines for content or <<<
    let foundBreak = false;
    let foundContent = false;
    for (let j = 1; j <= 3 && (i + j) < lines.length; j++) {
      const next = lines[i + j]!;
      // Skip blank lines, comments, and ifdef guards
      if (next.trim() === '' || next.trimStart().startsWith('//')) continue;
      if (next.trim().startsWith('ifdef::') || next.trim().startsWith('ifndef::')
        || next.trim().startsWith('endif::')) continue;

      if (next.trim() === '<<<') {
        foundBreak = true;
        break;
      }
      foundContent = true;
      break;
    }

    // Also check if heading is last non-blank, non-comment line in file
    const remaining = lines.slice(i + 1).filter(l => l.trim() !== '' && !l.trimStart().startsWith('//'));
    const atEof = remaining.length === 0;

    if (!foundContent && (foundBreak || atEof)) {
      findings.push({
        severity: 'warn',
        check: 'orphaned-heading',
        category: 'structure',
        message: 'Orphaned heading — heading with no content before page break or end of file',
        line: i + 1,
        source: line.trim(),
      });
    }
  }
}

/** Check 51: Image attributes — image directives should specify pdfwidth= or width=. */
function checkImageAttributes(lines: string[], findings: EditorialFinding[]): void {
  for (let i = 0; i < lines.length; i++) {
    const line = lines[i]!;
    if (line.trimStart().startsWith('//')) continue;

    // Only check block image macros (image::), not inline (image:)
    if (!/^image::/.test(line)) continue;
    if (/pdfwidth=/.test(line) || /width=/.test(line)) continue;

    findings.push({
      severity: 'info',
      check: 'image-attributes',
      category: 'structure',
      message: 'Image directive without size — add pdfwidth= or width= for predictable PDF rendering',
      line: i + 1,
      source: line.trim(),
    });
  }
}


// =============================================================================
// Check Implementations — Legal: Defined Terms (52)
// =============================================================================

/**
 * Check 52: Defined terms bold — defined terms should be bold on first use.
 * Config-driven: reads first_occurrence from legal_language.defined_terms.
 */
function checkDefinedTermsBold(lines: string[], findings: EditorialFinding[], legalConfig: LegalLanguageConfig): void {
  if (!legalConfig?.defined_terms) return;
  if (legalConfig.defined_terms.first_occurrence !== 'bold') return;

  // Phase 1: Extract defined terms from *"Term"* patterns
  const definedTerms = new Map<string, number>();
  for (let i = 0; i < lines.length; i++) {
    const line = lines[i]!;
    if (line.trimStart().startsWith('//')) continue;

    const defMatch = line.match(/\*"([^"]+)"\*/);
    if (defMatch?.[1] && !definedTerms.has(defMatch[1])) {
      definedTerms.set(defMatch[1], i + 1);
    }
  }

  if (definedTerms.size === 0) return;

  // Phase 2: Check for unbolded first occurrence after definition
  for (const [term, defLine] of definedTerms) {
    const escaped = term.replace(/[.*+?^${}()|[\]\\]/g, '\\$&');
    const boldRe = new RegExp(`\\*${escaped}\\*|\\*\\*${escaped}\\*\\*`);
    const plainRe = new RegExp(`\\b${escaped}\\b`);

    for (let i = 0; i < lines.length; i++) {
      if (i + 1 === defLine) continue; // Skip the definition line itself
      const line = lines[i]!;
      if (line.trimStart().startsWith('//')) continue;
      if (line.startsWith('|') || line.startsWith(':')) continue; // Skip tables and attributes

      if (plainRe.test(line) && !boldRe.test(line)) {
        findings.push({
          severity: 'info',
          check: 'defined-terms-bold',
          category: 'legal',
          message: `Defined term "${term}" — first occurrence should be bold per §Defined Terms`,
          line: i + 1,
          source: line.trim(),
        });
        break; // Only report first unbolded occurrence per term
      }
    }
  }
}


// =============================================================================
// Check Implementations — CIM Section Patterns (53-57)
// =============================================================================

/** Check 53: Section reference format — § N.M consistency. */
function checkSectionRefFormat(lines: string[], findings: EditorialFinding[]): void {
  for (let i = 0; i < lines.length; i++) {
    const line = lines[i]!;
    if (line.trimStart().startsWith('//')) continue;
    if (line.startsWith(':')) continue; // Skip document attributes

    // §N.M without space after §
    if (/§\d/.test(line)) {
      findings.push({
        severity: 'info',
        check: 'section-ref-format',
        category: 'style',
        message: 'Section reference format — use "§ N.M" (space after §)',
        line: i + 1,
        source: line.trim(),
      });
    }
  }
}

/** Check 54: Hebrew font config — Hebrew text needs font fallback declaration. */
function checkHebrewFontConfig(lines: string[], findings: EditorialFinding[], filePath: string): void {
  let hasHebrew = false;
  let hasFontDir = false;

  for (const line of lines) {
    // Detect Hebrew Unicode range (0x0590-0x05FF)
    if (/[\u0590-\u05FF]/.test(line)) hasHebrew = true;
    if (/:pdf-fontsdir:/.test(line)) hasFontDir = true;
  }

  if (hasHebrew && !hasFontDir) {
    // Check if shared-attributes include provides font dir (common pattern)
    const hasInclude = lines.some(l => /include::.*_shared-attributes/.test(l));
    if (!hasInclude) {
      findings.push({
        severity: 'info',
        check: 'hebrew-font-config',
        category: 'structure',
        message: 'Hebrew text detected without :pdf-fontsdir: — ensure font fallback via shared attributes or direct declaration',
        line: 0,
        source: basename(filePath),
      });
    }
  }
}

/** Check 55: Lead statement — Mission/Vision statements should use [.lead]. */
function checkLeadStatement(lines: string[], findings: EditorialFinding[]): void {
  for (let i = 0; i < lines.length; i++) {
    const line = lines[i]!;
    if (line.trimStart().startsWith('//')) continue;

    // Detect heading containing "Mission Statement" or "Vision Statement"
    if (!/^=+\s+.*\b(Mission|Vision)\s+Statement\b/i.test(line)) continue;

    // Look ahead up to 5 lines for [.lead]
    let foundLead = false;
    for (let j = 1; j <= 5 && (i + j) < lines.length; j++) {
      if (lines[i + j]!.includes('[.lead]')) {
        foundLead = true;
        break;
      }
      if (/^=+\s/.test(lines[i + j]!)) break; // Hit another heading — stop
    }

    if (!foundLead) {
      findings.push({
        severity: 'info',
        check: 'lead-statement',
        category: 'style',
        message: 'Mission/Vision statement section without [.lead] — key declarations deserve maximum visual weight',
        line: i + 1,
        source: line.trim(),
      });
    }
  }
}

/** Check 56: Em-dash listing — bold-name listings should use em-dash (—), not hyphen or en-dash. */
function checkEmDashListing(lines: string[], findings: EditorialFinding[]): void {
  for (let i = 0; i < lines.length; i++) {
    const line = lines[i]!;
    if (line.trimStart().startsWith('//')) continue;

    // Detect pattern: **Bold Text** followed by a separator that isn't em-dash
    const match = line.match(/\*\*[^*]+\*\*\s*([-\u2013])\s/);
    if (match) {
      const sep = match[1]!;
      if (sep === '-' || sep === '\u2013') {
        findings.push({
          severity: 'info',
          check: 'em-dash-listing',
          category: 'style',
          message: `Listing separator "${sep}" — use em-dash "\u2014" for bold-name listings (§CIM patterns)`,
          line: i + 1,
          source: line.trim(),
        });
      }
    }
  }
}

/**
 * Check 57: Reading guide parties — reading guide files must define all 4 required parties.
 * Config-driven: reads party names from company_conventions.reading_guide_parties.
 */
function checkReadingGuideParties(lines: string[], findings: EditorialFinding[], filePath: string, companyConfig?: CompanyConventionsConfig): void {
  if (!companyConfig?.reading_guide_parties) return;

  // Only fire on files that contain reading guide content
  const content = lines.join('\n');
  const isReadingGuide = /reading\s+(guide|path)/i.test(content)
    && /\*\*Board\s+Members?\*\*/i.test(content);
  if (!isReadingGuide) return;

  const requiredParties = companyConfig.reading_guide_parties;
  for (const party of requiredParties) {
    const partyRe = new RegExp(`\\*\\*${party.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')}\\*\\*`, 'i');
    if (!partyRe.test(content)) {
      findings.push({
        severity: 'info',
        check: 'reading-guide-parties',
        category: 'structure',
        message: `Reading guide missing required party: "${party}" — all 4 parties must be defined`,
        line: 0,
        source: basename(filePath),
      });
    }
  }
}


// =============================================================================
// Anchor Collection (for cross-reference check)
// =============================================================================

/** Collect all [[anchor-id]] definitions across all files. */
function collectAnchors(files: string[]): Set<string> {
  const anchors = new Set<string>();

  for (const filePath of files) {
    try {
      const content = readFileSync(filePath, 'utf-8');
      const anchorMatches = content.matchAll(/\[\[([a-zA-Z0-9_-]+)\]\]/g);
      for (const match of anchorMatches) {
        anchors.add(match[1]!);
      }

      // Also collect section IDs from headings (=== heading generates automatic ID)
      const headingMatches = content.matchAll(/^=+\s+(.+)$/gm);
      for (const match of headingMatches) {
        // AsciiDoc auto-generates IDs: lowercase, spaces→hyphens, strip non-alpha
        const autoId = match[1]!
          .toLowerCase()
          .replace(/[^a-z0-9\s-]/g, '')
          .replace(/\s+/g, '-')
          .replace(/^-+|-+$/g, '');
        if (autoId) anchors.add(autoId);
      }
    } catch {
      // Skip unreadable files
    }
  }

  return anchors;
}


// =============================================================================
// Main Entry Point
// =============================================================================

/**
 * Run editorial lint on the specified files.
 *
 * Config-driven: when legalConfig is provided, checks 1 (shall) and 13
 * (register-mixing) use obligation word lists from build.config.yaml §13.
 * Legal category checks (18-20) require legalConfig to function.
 *
 * Produces output matching the Makefile lint format:
 *   --- FILENAME ---
 *     WARN  message
 *     INFO  message
 *     PASS  No issues found
 *   --- Summary ---
 *     Files: N  Warnings: N  Info: N
 */
export async function runEditorialLint(
  paths: RuntimePaths,
  options: EditorialLintOptions = {},
  legalConfig?: LegalLanguageConfig,
  toneConfig?: ToneAndVoiceConfig,
  industryConfig?: IndustrySpecificConfig,
  documentTypesConfig?: DocumentTypesConfig,
  publishingConfig?: AsciidocPublishingConfig,
  companyConfig?: CompanyConventionsConfig,
): Promise<EditorialResult> {
  // Determine which categories to run
  const categoryMap: Record<string, CheckCategory[]> = {
    'editorial': ['editorial'],
    'style': ['style'],
    'structure': ['structure'],
    'conversion': ['conversion'],
    'legal': ['legal'],
    'industry': ['industry'],
    'tone': ['tone'],
    'all': ['editorial', 'style', 'structure', 'conversion', 'legal', 'industry', 'tone'],
  };

  const requestedCategories: CheckCategory[] = categoryMap[options.check ?? 'all']
    ?? ['editorial', 'style', 'structure', 'conversion', 'legal', 'industry', 'tone'];

  // Resolve files
  const filePaths = resolveFiles(paths, options);

  if (filePaths.length === 0) {
    return {
      files: [],
      filesChecked: 0,
      totals: { warn: 0, info: 0 },
      categories: requestedCategories,
    };
  }

  // Collect all anchors for cross-reference check (if structure category is active)
  let allAnchors = new Set<string>();
  if (requestedCategories.includes('structure')) {
    // Collect from all book files, not just the ones being linted
    const allFiles = findAdocFiles(paths.bookDir).filter(f => !f.includes('/00-assets/'));
    allAnchors = collectAnchors(allFiles);
  }

  const results: EditorialFileResult[] = [];
  let totalWarn = 0;
  let totalInfo = 0;

  // Resolve register from document type if not explicitly provided
  let resolvedRegister = options.register;
  if (!resolvedRegister && options.documentType && documentTypesConfig?.types) {
    const docType = documentTypesConfig.types.find(t => t.type === options.documentType);
    if (docType) resolvedRegister = docType.register;
  }

  // Accumulate all content for cross-file checks (legal + industry)
  let allContent = '';
  const needsCrossFile = (requestedCategories.includes('legal') && legalConfig) ||
    (requestedCategories.includes('industry') && options.documentType === 'creator_agreement');

  for (const filePath of filePaths) {
    const content = readFileSync(filePath, 'utf-8');
    const lines = content.split('\n');
    const findings: EditorialFinding[] = [];

    if (needsCrossFile) {
      allContent += content + '\n';
    }

    // Run enabled categories
    if (requestedCategories.includes('editorial')) {
      checkShall(lines, findings, legalConfig);
      checkSpacers(lines, findings);
      checkPageMarkers(lines, findings, filePath, paths.outputDir);
      checkOldComments(lines, findings);
      checkLeadDensity(lines, findings);
      checkTitlePage(lines, findings);
      checkBookHeadingGuard(lines, findings);
      checkAttributeUsage(lines, findings, companyConfig);
      checkBoardReference(lines, findings, companyConfig);
    }

    if (requestedCategories.includes('style')) {
      checkCompanyName(lines, findings);
      checkChapterLanguage(lines, findings);
      checkAllCaps(lines, findings);
      checkGoldText(lines, findings);
      checkRegisterMixing(lines, findings, legalConfig);
      checkClosingFiller(lines, findings, toneConfig);
      checkParagraphDensity(lines, findings, toneConfig);
      checkRedundantLogo(lines, findings);
      checkColorAccessibility(lines, findings);
      checkSectionRefFormat(lines, findings);
      checkLeadStatement(lines, findings);
      checkEmDashListing(lines, findings);
    }

    if (requestedCategories.includes('structure')) {
      checkUnlabeledBreaks(lines, findings);
      checkXrefResolution(lines, findings, allAnchors);
      checkProhibitedAttributes(lines, findings, publishingConfig);
      checkRedundantPageBreak(lines, findings, publishingConfig);
      checkIncludeGuard(lines, findings, filePath, publishingConfig);
      checkSectionRolePlacement(lines, findings, filePath, publishingConfig);
      checkFrontmatterAlignment(lines, findings, filePath);
      checkTocTable(lines, findings, filePath);
      checkSubordinateHeading(lines, findings);
      checkAppendixDuplication(lines, findings, filePath);
      checkOrphanedHeading(lines, findings);
      checkImageAttributes(lines, findings);
      checkHebrewFontConfig(lines, findings, filePath);
      checkReadingGuideParties(lines, findings, filePath, companyConfig);
    }

    if (requestedCategories.includes('conversion')) {
      checkIconMacros(lines, findings);
      checkAdmonitionBlocks(lines, findings);
      checkSidebarBlocks(lines, findings);
      checkIconTables(lines, findings);
    }

    // Per-file legal checks
    if (requestedCategories.includes('legal') && legalConfig) {
      checkDefinitionVerbs(lines, findings, legalConfig);
      checkProhibitedTerms(lines, findings, legalConfig);
      checkNumberingConvention(lines, findings, legalConfig);
      checkEnumerationPunctuation(lines, findings, legalConfig);
      checkSelfReference(lines, findings, legalConfig);
      checkArchaicContext(lines, findings, legalConfig, resolvedRegister);
      checkDefinedTermsBold(lines, findings, legalConfig);
    }

    // Per-file tone checks (27-29, 46)
    if (requestedCategories.includes('tone')) {
      checkCompulsionLanguage(lines, findings, toneConfig);
      checkPlainLanguageWords(lines, findings, toneConfig);
      checkSentenceLength(lines, findings, toneConfig);
      checkPassiveGovernance(lines, findings, toneConfig);
    }

    // Per-file industry checks (30)
    if (requestedCategories.includes('industry')) {
      checkDocumentRegister(lines, findings, resolvedRegister);
    }

    const warnCount = findings.filter(f => f.severity === 'warn').length;
    const infoCount = findings.filter(f => f.severity === 'info').length;

    results.push({
      file: basename(filePath),
      path: filePath,
      findings,
      counts: { warn: warnCount, info: infoCount },
    });

    totalWarn += warnCount;
    totalInfo += infoCount;
  }

  // Cross-file legal checks (run once across all content, not per-file)
  if (requestedCategories.includes('legal') && legalConfig && allContent) {
    const crossFindings: EditorialFinding[] = [];
    checkBindingProvisions(allContent, crossFindings, legalConfig);
    checkBoilerplate(allContent, crossFindings, legalConfig);
    checkRecitalsStyle(allContent, crossFindings, legalConfig);
    checkDocumentAnatomy(results, crossFindings, legalConfig);

    if (crossFindings.length > 0) {
      const warnCount = crossFindings.filter(f => f.severity === 'warn').length;
      const infoCount = crossFindings.filter(f => f.severity === 'info').length;
      results.push({
        file: '(cross-file legal checks)',
        path: '',
        findings: crossFindings,
        counts: { warn: warnCount, info: infoCount },
      });
      totalWarn += warnCount;
      totalInfo += infoCount;
    }
  }

  // Cross-file industry checks (31 — creator agreement provisions)
  if (requestedCategories.includes('industry') && options.documentType === 'creator_agreement' && allContent) {
    const crossFindings: EditorialFinding[] = [];
    checkCreatorProvisions(allContent, crossFindings, industryConfig);

    if (crossFindings.length > 0) {
      const warnCount = crossFindings.filter(f => f.severity === 'warn').length;
      const infoCount = crossFindings.filter(f => f.severity === 'info').length;
      results.push({
        file: '(cross-file industry checks)',
        path: '',
        findings: crossFindings,
        counts: { warn: warnCount, info: infoCount },
      });
      totalWarn += warnCount;
      totalInfo += infoCount;
    }
  }

  return {
    files: results,
    filesChecked: filePaths.length,
    totals: { warn: totalWarn, info: totalInfo },
    categories: requestedCategories,
  };
}
