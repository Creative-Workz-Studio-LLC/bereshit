/**
 * CWS Manual Builder — Page & TOC Validation
 *
 * Parses AsciiDoc files for editorial page markers, content-full annotations,
 * and § section entries. Provides structured page data for:
 *   - Editorial review (which pages exist, which are locked/content-full)
 *   - TOC navigation (§ sections with page placeholders)
 *   - Publication (filling p. __ placeholders with real page numbers)
 *
 * Page marker patterns recognized:
 *   // end p.N                    Page boundary marker
 *   // end p.N — content full     Page is full (adding <<< creates blank page)
 *   // PAGE N — TITLE             Page title banner (in comment blocks)
 *   § N.N — Title                 Section entry (in TOC frameless tables)
 *   p. __                         Page number placeholder (to be filled)
 *
 * "Write the vision, and make it plain upon tables,
 *  that he may run that readeth it." — Habakkuk 2:2
 */

import { readFileSync, readdirSync, existsSync } from 'fs';
import { join, basename, dirname } from 'path';
import type { RuntimePaths } from '../config/types.js';
import { detectSectionType, type SectionType } from '../errors.js';

// =============================================================================
// Types
// =============================================================================

/** A single page boundary found in an AsciiDoc source file. */
export interface PageMarker {
  /** Page number (from // end p.N) */
  number: number;

  /** Page title (from // PAGE N — TITLE banner, if present) */
  title: string;

  /** Line number of the // PAGE N banner (0 if no banner found) */
  bannerLine: number;

  /** Line number of the // end p.N marker */
  endLine: number;

  /** Whether this page is content-full (locked — do not add <<<) */
  contentFull: boolean;

  /** In-book page number (computed from cross-file offset, 0 if not computed) */
  inBookNumber: number;
}

/** A § section entry extracted from a TOC page. */
export interface TocSection {
  /** Full section reference (e.g., "§ 1.1") */
  ref: string;

  /** Article number (e.g., 1) */
  article: number;

  /** Section number within article (e.g., 1) */
  section: number;

  /** Section title (e.g., "Legal Structure") */
  title: string;

  /** Description from [.small]# ... # (if present) */
  description: string;

  /** Whether a page placeholder (p. __) follows this entry */
  hasPlaceholder: boolean;

  /** Line number in source file */
  line: number;
}

/** An Article header entry from a TOC page. */
export interface TocArticle {
  /** Article number */
  number: number;

  /** Article title (e.g., "Entity Overview") */
  title: string;

  /** Subtitle/description */
  subtitle: string;

  /** Sections within this article */
  sections: TocSection[];

  /** Page number where this article appears in the TOC */
  tocPage: number;

  /** Line number in source file */
  line: number;
}

/** Options for page/TOC parsing. */
export interface PageParseOptions {
  /** Single file to parse */
  file?: string;

  /** Scope letter (A, B, C, D) */
  scope?: string;
}

/** Complete parse result for a single file. */
export interface FilePageResult {
  /** Source file path */
  file: string;

  /** Source file basename */
  filename: string;

  /** Section type (cover, toc, dedication, preface, glossary, content, etc.) */
  sectionType: SectionType;

  /** All page markers found */
  pages: PageMarker[];

  /** All § section entries found */
  sections: TocSection[];

  /** All Article headers found */
  articles: TocArticle[];

  /** Total pages (highest page number) */
  totalPages: number;

  /** Number of content-full (locked) pages */
  contentFullCount: number;

  /** Number of editable pages */
  editableCount: number;

  /** Page number placeholders (p. __) count */
  placeholderCount: number;

  /** In-book page range (computed in multi-file mode) */
  inBookStart: number;

  /** In-book page range end (0 if not computed) */
  inBookEnd: number;
}

/** Complete parse result across all files. */
export interface PageParseResult {
  /** Results per file */
  files: FilePageResult[];

  /** Aggregate totals */
  totals: {
    files: number;
    pages: number;
    contentFull: number;
    editable: number;
    sections: number;
    articles: number;
    placeholders: number;
  };

  /** Validation issues found */
  issues: PageIssue[];

  /** Duration in milliseconds */
  duration: number;
}

/** A validation issue found during parsing. */
export interface PageIssue {
  /** Severity */
  severity: 'error' | 'warn' | 'info';

  /** Source file */
  file: string;

  /** Line number (0 if file-level) */
  line: number;

  /** Issue description */
  message: string;
}

// =============================================================================
// Regex Patterns
// =============================================================================

/** Matches: // end p.N  or  // end p.N — content full */
const END_PAGE_REGEX = /^\/\/\s*end\s+p\.(\d+)(?:\s*—?\s*content\s+full)?$/;

/** Matches: // end p.N — content full (specifically) */
const CONTENT_FULL_REGEX = /^\/\/\s*end\s+p\.(\d+)\s*—?\s*content\s+full$/;

/** Matches: // PAGE N — TITLE (banner comment) */
const PAGE_BANNER_REGEX = /^\/\/\s*PAGE\s+(\d+)\s*—\s*(.+)$/;

/** Matches: § N.N — Title (section entry in TOC) */
const SECTION_REGEX = /\|?§\s+(\d+)\.(\d+)\s*—\s*(.+)/;

/** Matches: [.small]#description text# */
const SMALL_DESC_REGEX = /\[\.small\]#([^#]+)#/;

/** Matches: p. __ (page placeholder) */
const PAGE_PLACEHOLDER_REGEX = /p\.\s*__/;

/** Matches: **Article N: Title** or **Article N — Title** */
const ARTICLE_HEADER_REGEX = /\*\*Article\s+(\d+)[:\s—]+(.+?)\*\*/;

// =============================================================================
// Parser
// =============================================================================

/**
 * Parse a single AsciiDoc file for page markers, TOC sections, and articles.
 */
function parseFile(filePath: string): FilePageResult {
  const filename = basename(filePath);
  const dirPath = dirname(filePath);
  const result: FilePageResult = {
    file: filePath,
    filename,
    sectionType: detectSectionType(filename, dirPath),
    pages: [],
    sections: [],
    articles: [],
    totalPages: 0,
    contentFullCount: 0,
    editableCount: 0,
    placeholderCount: 0,
    inBookStart: 0,
    inBookEnd: 0,
  };

  let content: string;
  try {
    content = readFileSync(filePath, 'utf-8');
  } catch {
    return result;
  }

  const lines = content.split('\n');

  // Track page banners for back-linking
  const banners = new Map<number, { line: number; title: string }>();

  // First pass: collect page banners
  for (let i = 0; i < lines.length; i++) {
    const line = (lines[i] ?? '').trim();
    const bannerMatch = line.match(PAGE_BANNER_REGEX);
    if (bannerMatch) {
      const pageNum = parseInt(bannerMatch[1] ?? '0', 10);
      const title = (bannerMatch[2] ?? '').trim();
      banners.set(pageNum, { line: i + 1, title });
    }
  }

  // Second pass: collect end markers, sections, articles, placeholders
  let currentArticle: TocArticle | null = null;

  for (let i = 0; i < lines.length; i++) {
    const line = (lines[i] ?? '').trim();

    // Page end markers
    const endMatch = line.match(END_PAGE_REGEX);
    if (endMatch) {
      const pageNum = parseInt(endMatch[1] ?? '0', 10);
      const contentFull = CONTENT_FULL_REGEX.test(line);
      const banner = banners.get(pageNum);

      result.pages.push({
        number: pageNum,
        title: banner?.title ?? '',
        bannerLine: banner?.line ?? 0,
        endLine: i + 1,
        contentFull,
        inBookNumber: 0,  // Computed in multi-file mode by runPageParse
      });

      if (contentFull) {
        result.contentFullCount++;
      }

      continue;
    }

    // Article headers
    const articleMatch = line.match(ARTICLE_HEADER_REGEX);
    if (articleMatch) {
      const artNum = parseInt(articleMatch[1] ?? '0', 10);
      const artTitle = (articleMatch[2] ?? '').trim()
        .replace(/\*\*$/, '')  // Remove trailing bold marker
        .replace(/\s*\+\s*$/, '');  // Remove trailing line continuation

      // Find which page this article is on
      let tocPage = 0;
      for (const page of result.pages) {
        if (page.endLine > i + 1) break;
        tocPage = page.number;
      }
      // If no page ended yet, check banners
      if (tocPage === 0) {
        for (const [pNum, banner] of banners) {
          if (banner.line <= i + 1) {
            tocPage = pNum;
          }
        }
      }

      // Get subtitle from next line if it has italic formatting
      let subtitle = '';
      const nextLine = (lines[i + 1] ?? '').trim();
      if (nextLine.startsWith('_') && nextLine.includes('_')) {
        subtitle = nextLine.replace(/^_/, '').replace(/_\s*\+?\s*$/, '').trim();
      }

      currentArticle = {
        number: artNum,
        title: artTitle,
        subtitle,
        sections: [],
        tocPage: tocPage + 1,  // Article appears on the page AFTER current
        line: i + 1,
      };
      result.articles.push(currentArticle);
      continue;
    }

    // § section entries
    const sectionMatch = line.match(SECTION_REGEX);
    if (sectionMatch) {
      const artNum = parseInt(sectionMatch[1] ?? '0', 10);
      const secNum = parseInt(sectionMatch[2] ?? '0', 10);
      const secTitle = (sectionMatch[3] ?? '').trim()
        .replace(/\s*\+\s*$/, '');  // Remove line continuation

      // Look for [.small]# description # on next line
      let description = '';
      const nextLine = (lines[i + 1] ?? '').trim();
      const descMatch = nextLine.match(SMALL_DESC_REGEX);
      if (descMatch) {
        description = (descMatch[1] ?? '').trim();
      }

      // Check if page placeholder follows (on same or nearby lines)
      let hasPlaceholder = false;
      for (let j = i; j < Math.min(i + 4, lines.length); j++) {
        if (PAGE_PLACEHOLDER_REGEX.test(lines[j] ?? '')) {
          hasPlaceholder = true;
          break;
        }
      }

      const section: TocSection = {
        ref: `§ ${artNum}.${secNum}`,
        article: artNum,
        section: secNum,
        title: secTitle,
        description,
        hasPlaceholder,
        line: i + 1,
      };

      result.sections.push(section);

      // Attach to current article if matching
      if (currentArticle && currentArticle.number === artNum) {
        currentArticle.sections.push(section);
      }

      if (hasPlaceholder) {
        result.placeholderCount++;
      }

      continue;
    }

    // Standalone page placeholders (not part of § entries)
    if (PAGE_PLACEHOLDER_REGEX.test(line) && !SECTION_REGEX.test(line)) {
      // Already counted in section entries above — only count standalone ones
      const prevLine = (lines[i - 1] ?? '').trim();
      if (!SECTION_REGEX.test(prevLine) && !SMALL_DESC_REGEX.test(prevLine)) {
        result.placeholderCount++;
      }
    }
  }

  // Compute totals
  result.totalPages = result.pages.length > 0
    ? Math.max(...result.pages.map(p => p.number))
    : 0;
  result.editableCount = result.totalPages - result.contentFullCount;

  return result;
}

// =============================================================================
// Validation
// =============================================================================

/**
 * Validate page markers for a parsed file.
 * Returns issues found.
 *
 * Checks:
 *   1. Sequential page numbers (1, 2, 3, ...)
 *   2. Pages without banners (// PAGE N — TITLE)
 *   3. § section entries without placeholders
 *   4. Standalone header using :title-page: (should be :notitle:)
 *   5. Book heading guard: [discrete] == heading without ifndef::book[] wrapper
 */
function validatePages(result: FilePageResult): PageIssue[] {
  const issues: PageIssue[] = [];

  // --- File-level editorial checks (read content once) ---
  let content: string;
  try {
    content = readFileSync(result.file, 'utf-8');
  } catch {
    content = '';
  }

  if (content) {
    const lines = content.split('\n');

    // Check 4: :title-page: in standalone header (should be :notitle:)
    for (let i = 0; i < lines.length; i++) {
      if ((lines[i] ?? '').trim() === ':title-page:') {
        issues.push({
          severity: 'warn',
          file: result.filename,
          line: i + 1,
          message: 'Standalone header uses :title-page: (should be :notitle: for frontmatter)',
        });
      }
    }

    // Check 5: Book heading guard — ifdef::book[] exists but [discrete] == heading
    //          may lack ifndef::book[] wrapper
    const hasIfdefBook = lines.some(l => (l ?? '').trim().startsWith('ifdef::book[]'));
    if (hasIfdefBook) {
      // Find [discrete] headings followed by == Title
      let inIfndef = false;
      for (let i = 0; i < lines.length; i++) {
        const line = (lines[i] ?? '').trim();
        if (line === 'ifndef::book[]') inIfndef = true;
        if (line === 'endif::[]' && inIfndef) { inIfndef = false; continue; }

        // Outside ifndef::book[] guard — check for [discrete] + == heading pair
        if (!inIfndef && line === '[discrete]') {
          const nextLine = (lines[i + 1] ?? '').trim();
          if (nextLine.startsWith('== ') && !nextLine.startsWith('=== ')) {
            issues.push({
              severity: 'info',
              file: result.filename,
              line: i + 2,
              message: `[discrete] == heading "${nextLine.slice(3).trim()}" may need ifndef::book[] guard (file has ifdef::book[])`,
            });
          }
        }
      }
    }
  }

  if (result.pages.length === 0) {
    return issues;  // No page markers — skip page-specific validation
  }

  // Check 1: Sequential page numbers (should be 1, 2, 3, ...)
  let expected = 1;
  for (const page of result.pages.sort((a, b) => a.number - b.number)) {
    if (page.number !== expected) {
      issues.push({
        severity: 'error',
        file: result.filename,
        line: page.endLine,
        message: `Page sequence gap: expected p.${expected}, found p.${page.number}`,
      });
    }
    expected = page.number + 1;
  }

  // Check 2: Pages without banners
  for (const page of result.pages) {
    if (!page.title && page.number > 0) {
      issues.push({
        severity: 'warn',
        file: result.filename,
        line: page.endLine,
        message: `Page ${page.number} has no // PAGE N — TITLE banner`,
      });
    }
  }

  // Check 3: § section entries have placeholders
  for (const section of result.sections) {
    if (!section.hasPlaceholder) {
      issues.push({
        severity: 'info',
        file: result.filename,
        line: section.line,
        message: `${section.ref} has no p. __ placeholder`,
      });
    }
  }

  return issues;
}

// =============================================================================
// File Discovery
// =============================================================================

/**
 * Find AsciiDoc files based on options.
 * Reuses the same resolution pattern as bible.ts.
 */
function findFiles(bookDir: string, options: PageParseOptions): string[] {
  if (options.file) {
    // Try direct, with extension, then search
    const candidates = [
      join(bookDir, options.file),
      join(bookDir, options.file.endsWith('.adoc') ? options.file : `${options.file}.adoc`),
    ];

    for (const path of candidates) {
      if (existsSync(path)) return [path];
    }

    // Search scope dirs

    const scopeDirs = ['A-frontmatter', 'B-content', 'C-appendices', 'D-backmatter'];
    for (const dir of scopeDirs) {
      const searchDir = join(bookDir, dir);
      if (!existsSync(searchDir)) continue;

      const targetName: string = options.file.endsWith('.adoc') ? options.file : `${options.file}.adoc`;
      try {
        const entries = readdirSync(searchDir);
        for (const entry of entries) {
          if (entry === targetName || entry === options.file) {
            return [join(searchDir, entry)];
          }
        }
      } catch { /* skip */ }
    }

    return [];
  }

  if (options.scope) {
    const scopeMap: Record<string, string> = {
      'A': 'A-frontmatter',
      'B': 'B-content',
      'C': 'C-appendices',
      'D': 'D-backmatter',
    };

    const dirName = scopeMap[options.scope.toUpperCase()];
    if (!dirName) return [];

    const scopeDir = join(bookDir, dirName);
    if (!existsSync(scopeDir)) return [];

    return findAdocFiles(scopeDir);
  }

  // Default: all frontmatter files (most likely to have TOC data)
  const frontmatter = join(bookDir, 'A-frontmatter');
  if (existsSync(frontmatter)) {
    return findAdocFiles(frontmatter);
  }

  return [];
}

/**
 * Find .adoc files in a directory (non-recursive for scope dirs).
 */
function findAdocFiles(dir: string): string[] {
  try {
    return readdirSync(dir)
      .filter((f: string) => f.endsWith('.adoc'))
      .sort()
      .map((f: string) => join(dir, f));
  } catch {
    return [];
  }
}

// =============================================================================
// Main Entry Points
// =============================================================================

/**
 * Parse page markers and TOC structure from AsciiDoc files.
 *
 * Returns structured data for editorial review, navigation, and publication.
 */
export async function runPageParse(
  paths: RuntimePaths,
  options: PageParseOptions = {}
): Promise<PageParseResult> {
  const startTime = Date.now();
  const files = findFiles(paths.bookDir, options);

  const fileResults: FilePageResult[] = [];
  const allIssues: PageIssue[] = [];
  const isMultiFile = files.length > 1;

  for (const filePath of files) {
    const result = parseFile(filePath);
    fileResults.push(result);

    // Validate and collect issues
    const issues = validatePages(result);
    allIssues.push(...issues);
  }

  // Compute in-book page numbers across files (multi-file mode)
  if (isMultiFile) {
    let bookOffset = 0;
    for (const fileResult of fileResults) {
      if (fileResult.totalPages === 0) continue;

      fileResult.inBookStart = bookOffset + 1;
      fileResult.inBookEnd = bookOffset + fileResult.totalPages;

      for (const page of fileResult.pages) {
        page.inBookNumber = bookOffset + page.number;
      }

      bookOffset += fileResult.totalPages;
    }
  }

  // Compute aggregate totals
  const totals = {
    files: fileResults.length,
    pages: fileResults.reduce((sum, f) => sum + f.totalPages, 0),
    contentFull: fileResults.reduce((sum, f) => sum + f.contentFullCount, 0),
    editable: fileResults.reduce((sum, f) => sum + f.editableCount, 0),
    sections: fileResults.reduce((sum, f) => sum + f.sections.length, 0),
    articles: fileResults.reduce((sum, f) => sum + f.articles.length, 0),
    placeholders: fileResults.reduce((sum, f) => sum + f.placeholderCount, 0),
  };

  return {
    files: fileResults,
    totals,
    issues: allIssues,
    duration: Date.now() - startTime,
  };
}

/**
 * Quick page status check — just markers and content-full status.
 * Lighter than full parse — skips § section extraction.
 */
export async function runPageStatus(
  paths: RuntimePaths,
  options: PageParseOptions = {}
): Promise<PageParseResult> {
  // Full parse includes everything — caller can use just the page data
  return runPageParse(paths, options);
}

// =============================================================================
// TOC Pipeline — Fill & Check
// =============================================================================

/** Result of TOC fill or check operation. */
export interface TocPipelineResult {
  /** Number of placeholders successfully filled or verified */
  matched: number;

  /** Number of placeholders that couldn't be resolved (content lacks markers) */
  unresolved: number;

  /** Number of stale page numbers detected (check mode only) */
  stale: number;

  /** Content files missing page markers (need `// end p.N` markers) */
  missingMarkers: string[];

  /** Mapping of § ref → computed page number */
  pageMap: Map<string, number>;

  /** Issues encountered */
  issues: PageIssue[];

  /** Whether changes were written (fill mode) or just reported (check/dry-run) */
  written: boolean;
}

/**
 * Build a mapping from § N.M references to content file paths.
 *
 * Convention: Article-NN has sections lettered A (overview), B, C, ...
 * § N.1 maps to the B file, § N.2 to C, etc.
 */
async function buildSectionFileMap(bookDir: string): Promise<Map<string, string>> {
  const { scanSpine } = await import('../spine/scanner.js');
  const tree = await scanSpine(bookDir);

  const sectionMap = new Map<string, string>();

  for (const part of tree.scopes.B.parts) {
    for (const article of part.articles) {
      const artNum = parseInt(article.id, 10);

      // Skip the A-file (article overview), sections start at B
      const contentSections = article.sections.filter(s => s.letter !== 'A');

      for (let i = 0; i < contentSections.length; i++) {
        const section = contentSections[i];
        if (!section) continue;
        const ref = `§ ${artNum}.${i + 1}`;
        sectionMap.set(ref, section.path);
      }
    }
  }

  return sectionMap;
}

/**
 * Count pages in a content file by scanning for `// end p.N` markers.
 * Returns the highest page number found (0 if no markers).
 */
function countFilePages(filePath: string): number {
  let content: string;
  try {
    content = readFileSync(filePath, 'utf-8');
  } catch {
    return 0;
  }

  let maxPage = 0;
  for (const line of content.split('\n')) {
    const match = (line ?? '').trim().match(END_PAGE_REGEX);
    if (match) {
      const pageNum = parseInt(match[1] ?? '0', 10);
      if (pageNum > maxPage) maxPage = pageNum;
    }
  }

  return maxPage;
}

/**
 * Compute book-level page numbers for each § section reference.
 *
 * Flow: sectionFileMap (§ ref → path) → spine walk (order + A-files) → page counts → cumulative offset
 *
 * Uses sectionFileMap for path lookups, spine walk for structural order and A-file handling.
 * allRefs (sorted) validates that every mapped section was reached during traversal.
 *
 * Assumes frontmatter offset is provided (title page + auto-TOC + frontmatter pages).
 */
async function computePageMap(
  bookDir: string,
  frontmatterOffset: number
): Promise<{ pageMap: Map<string, number>; missingMarkers: string[] }> {
  // § ref → file path mapping (pre-built from spine)
  const sectionFileMap = await buildSectionFileMap(bookDir);

  // Sorted refs for validation pass after page computation
  const allRefs = Array.from(sectionFileMap.keys()).sort((a, b) => {
    const aParts = a.replace('§ ', '').split('.').map(Number);
    const bParts = b.replace('§ ', '').split('.').map(Number);
    const aArt = aParts[0] ?? 0;
    const aSec = aParts[1] ?? 0;
    const bArt = bParts[0] ?? 0;
    const bSec = bParts[1] ?? 0;
    return (aArt - bArt) || (aSec - bSec);
  });

  const pageMap = new Map<string, number>();
  const missingMarkers: string[] = [];

  // Cumulative page counter — starts after frontmatter
  let cumulativePages = frontmatterOffset;

  // Walk spine for structural order and A-file (article overview) handling
  const { scanSpine } = await import('../spine/scanner.js');
  const tree = await scanSpine(bookDir);

  for (const part of tree.scopes.B.parts) {
    for (const article of part.articles) {
      // Article overview (A-file) pages — not in sectionFileMap
      const overviewFile = article.sections.find(s => s.letter === 'A');
      if (overviewFile) {
        const overviewPages = countFilePages(overviewFile.path);
        if (overviewPages === 0) {
          missingMarkers.push(overviewFile.path);
        }
        cumulativePages += overviewPages;
      }

      // Content sections — spine gives order, sectionFileMap gives verified path
      const artNum = parseInt(article.id, 10);
      const contentSections = article.sections.filter(s => s.letter !== 'A');

      for (let i = 0; i < contentSections.length; i++) {
        const section = contentSections[i];
        if (!section) continue;

        const ref = `§ ${artNum}.${i + 1}`;
        // Use sectionFileMap for the canonical path (validates spine ↔ map agreement)
        const filePath = sectionFileMap.get(ref) ?? section.path;

        const sectionPages = countFilePages(filePath);
        if (sectionPages === 0) {
          missingMarkers.push(filePath);
        } else {
          // This section starts on the next page after all previous content
          pageMap.set(ref, cumulativePages + 1);
        }

        cumulativePages += sectionPages;
      }
    }
  }

  // Validation: ensure every mapped ref was reached during spine walk
  for (const ref of allRefs) {
    if (!pageMap.has(ref)) {
      const path = sectionFileMap.get(ref);
      if (path && !missingMarkers.includes(path)) {
        missingMarkers.push(path);
      }
    }
  }

  return { pageMap, missingMarkers };
}

/**
 * Fill `p. __` placeholders in the TOC file with computed page numbers.
 *
 * @param paths - Runtime paths
 * @param options - dryRun: report without writing
 */
export async function runTocFill(
  paths: RuntimePaths,
  options: { dryRun?: boolean } = {}
): Promise<TocPipelineResult> {
  const issues: PageIssue[] = [];

  // Parse frontmatter to get page count for offset computation
  const frontmatterResult = await runPageParse(paths, { scope: 'A' });
  const frontmatterPages = frontmatterResult.totals.pages;

  // PDF offset: title page (1) + auto-TOC (2) + frontmatter pages
  const pdfOffset = 3; // title + 2 auto-TOC pages
  const frontmatterOffset = pdfOffset + frontmatterPages;

  // Compute page map from content
  const { pageMap, missingMarkers } = await computePageMap(paths.bookDir, frontmatterOffset);

  // Find the TOC file
  const tocFile = join(paths.bookDir, 'A-frontmatter', 'B-TABLE-OF-CONTENTS.adoc');
  if (!existsSync(tocFile)) {
    issues.push({
      severity: 'error',
      file: 'B-TABLE-OF-CONTENTS.adoc',
      line: 0,
      message: 'TOC file not found',
    });
    return { matched: 0, unresolved: 0, stale: 0, missingMarkers, pageMap, issues, written: false };
  }

  let content = readFileSync(tocFile, 'utf-8');
  const lines = content.split('\n');
  let matched = 0;
  let unresolved = 0;

  // Process each § entry and its p. __ placeholder
  for (let i = 0; i < lines.length; i++) {
    const line = lines[i] ?? '';
    const sectionMatch = line.match(SECTION_REGEX);
    if (!sectionMatch) continue;

    const artNum = parseInt(sectionMatch[1] ?? '0', 10);
    const secNum = parseInt(sectionMatch[2] ?? '0', 10);
    const ref = `§ ${artNum}.${secNum}`;

    // Look for p. __ on nearby lines (same line or within 3 lines)
    for (let j = i; j < Math.min(i + 4, lines.length); j++) {
      if (PAGE_PLACEHOLDER_REGEX.test(lines[j] ?? '')) {
        const computedPage = pageMap.get(ref);
        if (computedPage !== undefined) {
          lines[j] = (lines[j] ?? '').replace(/p\.\s*__/, `p. ${computedPage}`);
          matched++;
        } else {
          unresolved++;
          issues.push({
            severity: 'info',
            file: 'B-TABLE-OF-CONTENTS.adoc',
            line: j + 1,
            message: `${ref}: cannot compute page — content file lacks page markers`,
          });
        }
        break;
      }
    }
  }

  // Write if not dry run and we have changes
  let written = false;
  if (!options.dryRun && matched > 0) {
    const { writeFileSync } = await import('fs');
    writeFileSync(tocFile, lines.join('\n'), 'utf-8');
    written = true;
  }

  return { matched, unresolved, stale: 0, missingMarkers, pageMap, issues, written };
}

/**
 * Check existing page numbers in the TOC against computed values.
 * Reports stale numbers that have shifted.
 */
export async function runTocCheck(
  paths: RuntimePaths
): Promise<TocPipelineResult> {
  const issues: PageIssue[] = [];

  // Compute current page map
  const frontmatterResult = await runPageParse(paths, { scope: 'A' });
  const frontmatterPages = frontmatterResult.totals.pages;
  const pdfOffset = 3;
  const frontmatterOffset = pdfOffset + frontmatterPages;

  const { pageMap, missingMarkers } = await computePageMap(paths.bookDir, frontmatterOffset);

  // Find the TOC file
  const tocFile = join(paths.bookDir, 'A-frontmatter', 'B-TABLE-OF-CONTENTS.adoc');
  if (!existsSync(tocFile)) {
    issues.push({
      severity: 'error',
      file: 'B-TABLE-OF-CONTENTS.adoc',
      line: 0,
      message: 'TOC file not found',
    });
    return { matched: 0, unresolved: 0, stale: 0, missingMarkers, pageMap, issues, written: false };
  }

  const content = readFileSync(tocFile, 'utf-8');
  const lines = content.split('\n');
  let matched = 0;
  let unresolved = 0;
  let stale = 0;

  /** Matches: |p. 42 (explicit page number, not placeholder) */
  const PAGE_NUMBER_REGEX = /p\.\s*(\d+)/;

  for (let i = 0; i < lines.length; i++) {
    const line = lines[i] ?? '';
    const sectionMatch = line.match(SECTION_REGEX);
    if (!sectionMatch) continue;

    const artNum = parseInt(sectionMatch[1] ?? '0', 10);
    const secNum = parseInt(sectionMatch[2] ?? '0', 10);
    const ref = `§ ${artNum}.${secNum}`;

    // Look for page number on nearby lines
    for (let j = i; j < Math.min(i + 4, lines.length); j++) {
      const nearLine = lines[j] ?? '';

      // Check for placeholder (unfilled)
      if (PAGE_PLACEHOLDER_REGEX.test(nearLine) && !PAGE_NUMBER_REGEX.test(nearLine)) {
        unresolved++;
        break;
      }

      // Check for explicit page number
      const numMatch = nearLine.match(PAGE_NUMBER_REGEX);
      if (numMatch) {
        const currentPage = parseInt(numMatch[1] ?? '0', 10);
        const computedPage = pageMap.get(ref);

        if (computedPage !== undefined) {
          if (currentPage !== computedPage) {
            stale++;
            issues.push({
              severity: 'warn',
              file: 'B-TABLE-OF-CONTENTS.adoc',
              line: j + 1,
              message: `${ref}: page shifted — TOC says p. ${currentPage}, computed p. ${computedPage}`,
            });
          } else {
            matched++;
          }
        } else {
          issues.push({
            severity: 'info',
            file: 'B-TABLE-OF-CONTENTS.adoc',
            line: j + 1,
            message: `${ref}: has p. ${currentPage} but content file lacks page markers — cannot verify`,
          });
        }
        break;
      }
    }
  }

  return { matched, unresolved, stale, missingMarkers, pageMap, issues, written: false };
}
