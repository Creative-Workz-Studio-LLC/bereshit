/**
 * CWS Manual Builder — Editorial Lint Validation
 *
 * All editorial style checks in one module. Replaces the Makefile grep chains
 * with structured, categorized validation. Each check produces findings with
 * severity, line numbers, and actionable messages.
 *
 * Check categories:
 *   editorial   — Core editorial lint (checks 1-8, migrated from Makefile)
 *   style       — Anti-pattern detection from editorial style guide (checks 9-13)
 *   structure   — Structural integrity checks (checks 14-15)
 *   conversion  — Article editorial conversion checklist (checks 16-17)
 *
 * Specification: editorial-style-guide.adoc (CWS-GDE-001), §Anti-Patterns
 *
 * "Prove all things; hold fast that which is good."
 *  — 1 Thessalonians 5:21
 */

import { readFileSync, readdirSync, existsSync } from 'fs';
import { execSync } from 'child_process';
import { join, basename, dirname, relative } from 'path';
import type { RuntimePaths } from '../config/types.js';
import { detectSectionType } from '../errors.js';

// =============================================================================
// Types
// =============================================================================

/** Severity of an editorial finding. */
export type EditorialSeverity = 'warn' | 'info';

/** Check category for filtering. */
export type CheckCategory = 'editorial' | 'style' | 'structure' | 'conversion';

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

/** Check 1: "shall" in governance text (skip Bible quotes, comments). */
function checkShall(lines: string[], findings: EditorialFinding[]): void {
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
        message: "'shall' in governance text (use must/will/may)",
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

/** Check 13: Register mixing — "shall" + "must" in same paragraph. */
function checkRegisterMixing(lines: string[], findings: EditorialFinding[]): void {
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

    const hasShall = /\bshall\b/i.test(text);
    const hasMust = /\bmust\b/i.test(text);

    if (hasShall && hasMust) {
      findings.push({
        severity: 'warn',
        check: 'register-mixing',
        category: 'style',
        message: '"shall" and "must" in same paragraph — pick one register and hold it',
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

/** Check 17: Admonition blocks in frontmatter. */
function checkAdmonitionFrontmatter(
  lines: string[],
  findings: EditorialFinding[],
  filePath: string,
): void {
  // Only check frontmatter files — use proper section type detection
  const sectionType = detectSectionType(basename(filePath), dirname(filePath));
  const frontmatterTypes = new Set(['cover', 'toc', 'dedication', 'preface', 'glossary']);
  if (!frontmatterTypes.has(sectionType)) return;

  for (let i = 0; i < lines.length; i++) {
    const line = lines[i]!;

    // Skip comments
    if (line.trimStart().startsWith('//')) continue;

    if (/^(NOTE|TIP|IMPORTANT|WARNING|CAUTION):/.test(line)) {
      findings.push({
        severity: 'info',
        check: 'admonition-frontmatter',
        category: 'conversion',
        message: `Admonition block in frontmatter — use centered text with bold labels instead (conversion step 3)`,
        line: i + 1,
        source: line.trim(),
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
): Promise<EditorialResult> {
  // Determine which categories to run
  const categoryMap: Record<string, CheckCategory[]> = {
    'editorial': ['editorial'],
    'style': ['style'],
    'structure': ['structure'],
    'conversion': ['conversion'],
    'all': ['editorial', 'style', 'structure', 'conversion'],
  };

  const requestedCategories: CheckCategory[] = categoryMap[options.check ?? 'all'] ?? ['editorial', 'style', 'structure', 'conversion'];

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

  for (const filePath of filePaths) {
    const content = readFileSync(filePath, 'utf-8');
    const lines = content.split('\n');
    const findings: EditorialFinding[] = [];

    // Run enabled categories
    if (requestedCategories.includes('editorial')) {
      checkShall(lines, findings);
      checkSpacers(lines, findings);
      checkPageMarkers(lines, findings, filePath, paths.outputDir);
      checkOldComments(lines, findings);
      checkLeadDensity(lines, findings);
      checkTitlePage(lines, findings);
      checkBookHeadingGuard(lines, findings);
    }

    if (requestedCategories.includes('style')) {
      checkCompanyName(lines, findings);
      checkChapterLanguage(lines, findings);
      checkAllCaps(lines, findings);
      checkGoldText(lines, findings);
      checkRegisterMixing(lines, findings);
    }

    if (requestedCategories.includes('structure')) {
      checkUnlabeledBreaks(lines, findings);
      checkXrefResolution(lines, findings, allAnchors);
    }

    if (requestedCategories.includes('conversion')) {
      checkIconMacros(lines, findings);
      checkAdmonitionFrontmatter(lines, findings, filePath);
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

  return {
    files: results,
    filesChecked: filePaths.length,
    totals: { warn: totalWarn, info: totalInfo },
    categories: requestedCategories,
  };
}
