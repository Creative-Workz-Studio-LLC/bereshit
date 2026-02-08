/**
 * CWS Manual Builder — Bible Verse Validation
 *
 * Parses AsciiDoc files for Scripture citations, looks up canonical
 * verse text from the Bereshit scripture data (KJV and WEB), and
 * validates that cited text matches the source.
 *
 * Citation patterns recognized:
 *   — Book Chapter:Verse              Plain attribution
 *   — _Book Chapter:Verse (KJV)_      Italic with translation
 *   — _Book Chapter:Verse-End (KJV)_  Verse range, italic
 *   _"text"_ — Book Chapter:Verse     Inline quote + reference
 *
 * Scripture data: Bereshit/word/scripture/{KJV,WEB}/Book/Chapter_N/Verse_N.txt
 *
 * "All scripture is given by inspiration of God, and is profitable
 *  for doctrine, for reproof, for correction, for instruction in
 *  righteousness." — 2 Timothy 3:16
 */

import { readFileSync, existsSync, readdirSync } from 'fs';
import { join, resolve } from 'path';
import type { RuntimePaths } from '../../config/types.js';

// =============================================================================
// Types
// =============================================================================

/** Supported Bible translations in the scripture data. */
export type BibleTranslation = 'KJV' | 'WEB';

/** Status of a verse citation check. */
export type CitationStatus = 'verified' | 'mismatch' | 'not-found' | 'ref-only';

/** A single parsed citation from an AsciiDoc file. */
export interface ParsedCitation {
  /** Source file path */
  file: string;

  /** Line number in source file */
  line: number;

  /** Book name as written in the source */
  book: string;

  /** Normalized book name for filesystem lookup (e.g., "1_Corinthians") */
  bookNormalized: string;

  /** Chapter number */
  chapter: number;

  /** Starting verse number */
  verse: number;

  /** Ending verse number (for ranges like 3:23-24) */
  verseEnd?: number;

  /** Translation (KJV or WEB), defaults to KJV */
  translation: BibleTranslation;

  /** Quoted text from the document (if present) */
  quotedText?: string;

  /** Canonical text from scripture data */
  canonicalText?: string;

  /** Validation status */
  status: CitationStatus;

  /** Additional detail messages */
  details?: string[];
}

/** Options for Bible validation. */
export interface BibleValidationOptions {
  /** Single file to validate */
  file?: string;

  /** Scope letter (A, B, C, D) */
  scope?: string;
}

/** Complete Bible validation result. */
export interface BibleValidationResult {
  /** All citations found and their validation status */
  citations: ParsedCitation[];

  /** Counts per status */
  counts: Record<CitationStatus, number>;

  /** Number of files checked */
  filesChecked: number;

  /** Duration in milliseconds */
  duration: number;
}

// =============================================================================
// Constants
// =============================================================================

/**
 * Bible book name mappings — display name → filesystem directory name.
 * Handles numbered books, alternate names, and common abbreviations.
 */
const BOOK_MAP: Record<string, string> = {
  // Old Testament
  'Genesis': 'Genesis',
  'Exodus': 'Exodus',
  'Leviticus': 'Leviticus',
  'Numbers': 'Numbers',
  'Deuteronomy': 'Deuteronomy',
  'Joshua': 'Joshua',
  'Judges': 'Judges',
  'Ruth': 'Ruth',
  '1 Samuel': '1_Samuel',
  '2 Samuel': '2_Samuel',
  '1 Kings': '1_Kings',
  '2 Kings': '2_Kings',
  '1 Chronicles': '1_Chronicles',
  '2 Chronicles': '2_Chronicles',
  'Ezra': 'Ezra',
  'Nehemiah': 'Nehemiah',
  'Esther': 'Esther',
  'Job': 'Job',
  'Psalm': 'Psalms',
  'Psalms': 'Psalms',
  'Proverbs': 'Proverbs',
  'Ecclesiastes': 'Ecclesiastes',
  'Song of Solomon': 'Song_of_Solomon',
  'Isaiah': 'Isaiah',
  'Jeremiah': 'Jeremiah',
  'Lamentations': 'Lamentations',
  'Ezekiel': 'Ezekiel',
  'Daniel': 'Daniel',
  'Hosea': 'Hosea',
  'Joel': 'Joel',
  'Amos': 'Amos',
  'Obadiah': 'Obadiah',
  'Jonah': 'Jonah',
  'Micah': 'Micah',
  'Nahum': 'Nahum',
  'Habakkuk': 'Habakkuk',
  'Zephaniah': 'Zephaniah',
  'Haggai': 'Haggai',
  'Zechariah': 'Zechariah',
  'Malachi': 'Malachi',

  // New Testament
  'Matthew': 'Matthew',
  'Mark': 'Mark',
  'Luke': 'Luke',
  'John': 'John',
  'Acts': 'Acts',
  'Romans': 'Romans',
  '1 Corinthians': '1_Corinthians',
  '2 Corinthians': '2_Corinthians',
  'Galatians': 'Galatians',
  'Ephesians': 'Ephesians',
  'Philippians': 'Philippians',
  'Colossians': 'Colossians',
  '1 Thessalonians': '1_Thessalonians',
  '2 Thessalonians': '2_Thessalonians',
  '1 Timothy': '1_Timothy',
  '2 Timothy': '2_Timothy',
  'Titus': 'Titus',
  'Philemon': 'Philemon',
  'Hebrews': 'Hebrews',
  'James': 'James',
  '1 Peter': '1_Peter',
  '2 Peter': '2_Peter',
  '1 John': '1_John',
  '2 John': '2_John',
  '3 John': '3_John',
  'Jude': 'Jude',
  'Revelation': 'Revelation',
};

/**
 * Regex for Bible citation attributions in AsciiDoc.
 *
 * Matches patterns like:
 *   — Genesis 1:1
 *   — _Genesis 1:1 (KJV)_
 *   — _Habakkuk 2:2-3 (KJV)_
 *   — 1 Corinthians 13:4-7 (WEB)
 */
const CITATION_REGEX = /—\s*_?((?:\d\s+)?[A-Z][a-z]+(?:\s+of\s+[A-Z][a-z]+)?)\s+(\d+):(\d+)(?:-(\d+))?\s*(?:\((\w+)\))?\s*_?/g;

// =============================================================================
// Scripture Filesystem Lookup
// =============================================================================

/**
 * Resolve the Bereshit scripture base path.
 * Walks up from company-docs/build/ to find Bereshit/word/scripture/.
 */
function resolveScripturePath(sourceDir: string): string | null {
  // sourceDir is company-docs/build/
  // Bereshit root is two levels up: company-docs/../
  const bereshitRoot = resolve(sourceDir, '..', '..');
  const scripturePath = join(bereshitRoot, 'word', 'scripture');

  if (existsSync(scripturePath)) {
    return scripturePath;
  }

  return null;
}

/**
 * Read a single verse from the filesystem.
 * Path: scripture/{translation}/Book/Chapter_N/Verse_N.txt
 */
function readVerse(
  scripturePath: string,
  translation: BibleTranslation,
  bookDir: string,
  chapter: number,
  verse: number
): string | null {
  const versePath = join(
    scripturePath,
    translation,
    bookDir,
    `Chapter_${chapter}`,
    `Verse_${verse}.txt`
  );

  if (!existsSync(versePath)) {
    return null;
  }

  try {
    // Remove BOM and trim whitespace
    return readFileSync(versePath, 'utf-8').replace(/^\uFEFF/, '').trim();
  } catch {
    return null;
  }
}

/**
 * Read a verse range and concatenate.
 */
function readVerseRange(
  scripturePath: string,
  translation: BibleTranslation,
  bookDir: string,
  chapter: number,
  verseStart: number,
  verseEnd: number
): string | null {
  const parts: string[] = [];

  for (let v = verseStart; v <= verseEnd; v++) {
    const text = readVerse(scripturePath, translation, bookDir, chapter, v);
    if (text === null) {
      return null;  // If any verse in range is missing, report not-found
    }
    parts.push(text);
  }

  return parts.join(' ');
}

// =============================================================================
// Quote Extraction
// =============================================================================

/**
 * Extract quoted text that precedes a citation attribution.
 * Looks backward from the citation line to find the quoted content.
 *
 * Common patterns:
 *   > "In the beginning..." — Genesis 1:1
 *   _"In the beginning..."_ — Genesis 1:1
 *   [quote block content]
 */
function extractQuotedText(lines: string[], citationLineIndex: number): string | null {
  // Check if citation line itself has inline quote: _"text"_ — Book...
  const citLine = lines[citationLineIndex] ?? '';
  const inlineMatch = citLine.match(/_?"([^"]+)"_?\s*—/);
  if (inlineMatch && inlineMatch[1]) {
    return cleanQuoteText(inlineMatch[1]);
  }

  // Look backward for quoted/italic text preceding the attribution
  const collected: string[] = [];
  for (let i = citationLineIndex - 1; i >= 0 && i >= citationLineIndex - 10; i--) {
    const line = (lines[i] ?? '').trim();

    // Stop at blank lines, comment lines, or block boundaries
    if (line === '' || line.startsWith('//') || line === '____' || line === '----') {
      break;
    }

    // Skip AsciiDoc role markers
    if (line.startsWith('[') && line.endsWith(']')) {
      continue;
    }

    collected.unshift(line);
  }

  if (collected.length === 0) {
    return null;
  }

  const text = collected.join(' ');
  return cleanQuoteText(text);
}

/**
 * Clean up extracted quote text — remove AsciiDoc formatting markers.
 */
function cleanQuoteText(text: string): string {
  return text
    .replace(/^_+|_+$/g, '')         // Remove italic markers
    .replace(/^\*+|\*+$/g, '')        // Remove bold markers
    .replace(/^"+|"+$/g, '')          // Remove quotes
    .replace(/^[>"]\s*/g, '')         // Remove blockquote markers
    .replace(/\s+/g, ' ')            // Normalize whitespace
    .replace(/^\.\.\.\s*/, '')        // Remove leading ellipsis
    .replace(/\s*\.\.\.$/,  '')       // Remove trailing ellipsis
    .trim();
}

// =============================================================================
// Text Comparison
// =============================================================================

/**
 * Normalize text for comparison — lowercase, strip punctuation variance.
 */
function normalizeForComparison(text: string): string {
  return text
    .toLowerCase()
    .replace(/['']/g, "'")            // Normalize smart quotes
    .replace(/[""]/g, '"')
    .replace(/\s+/g, ' ')
    .replace(/^\uFEFF/, '')           // Remove BOM
    .trim();
}

/**
 * Check if quoted text is a substring match of canonical text.
 * Handles partial quotes (common in documents — they quote a portion).
 */
function compareTexts(quoted: string, canonical: string): boolean {
  const q = normalizeForComparison(quoted);
  const c = normalizeForComparison(canonical);

  // Exact match
  if (q === c) return true;

  // Quoted is contained in canonical (partial quote)
  if (c.includes(q)) return true;

  // Canonical is contained in quoted (rare — quoted has more context)
  if (q.includes(c)) return true;

  // Word-level fuzzy: check if 80%+ of quoted words appear in canonical
  const qWords = q.split(/\s+/).filter(w => w.length > 2);
  const cWords = new Set(c.split(/\s+/));
  const matches = qWords.filter(w => cWords.has(w)).length;

  return qWords.length > 0 && (matches / qWords.length) >= 0.8;
}

// =============================================================================
// File Discovery
// =============================================================================

/**
 * Find AsciiDoc files to validate based on options.
 */
function findFiles(bookDir: string, options: BibleValidationOptions): string[] {
  if (options.file) {
    // Resolve a single file — try direct path, then search
    const direct = join(bookDir, options.file);
    if (existsSync(direct)) return [direct];

    const withExt = direct.endsWith('.adoc') ? direct : `${direct}.adoc`;
    if (existsSync(withExt)) return [withExt];

    // Search in scope dirs
    const scopeDirs = ['A-frontmatter', 'B-content', 'C-appendices', 'D-backmatter'];
    for (const dir of scopeDirs) {
      const searchDir = join(bookDir, dir);
      if (!existsSync(searchDir)) continue;

      const found = findFileRecursive(searchDir, options.file);
      if (found) return [found];
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

    return findAdocFilesRecursive(scopeDir);
  }

  // All files
  const files: string[] = [];
  const scopeDirs = ['A-frontmatter', 'B-content', 'C-appendices', 'D-backmatter'];
  for (const dir of scopeDirs) {
    const scopeDir = join(bookDir, dir);
    if (existsSync(scopeDir)) {
      files.push(...findAdocFilesRecursive(scopeDir));
    }
  }
  return files.sort();
}

/**
 * Recursively find .adoc files in a directory.
 */
function findAdocFilesRecursive(dir: string): string[] {
  const results: string[] = [];

  try {
    const entries = readdirSync(dir, { withFileTypes: true });
    for (const entry of entries) {
      const fullPath = join(dir, entry.name);
      if (entry.isDirectory()) {
        results.push(...findAdocFilesRecursive(fullPath));
      } else if (entry.name.endsWith('.adoc')) {
        results.push(fullPath);
      }
    }
  } catch {
    // Skip unreadable directories
  }

  return results;
}

/**
 * Search recursively for a file by name (with or without extension).
 */
function findFileRecursive(dir: string, name: string): string | null {
  const target = name.endsWith('.adoc') ? name : `${name}.adoc`;

  try {
    const entries = readdirSync(dir, { withFileTypes: true });
    for (const entry of entries) {
      const fullPath = join(dir, entry.name);
      if (entry.isDirectory()) {
        const found = findFileRecursive(fullPath, name);
        if (found) return found;
      } else if (entry.name === target || entry.name === name) {
        return fullPath;
      }
    }
  } catch {
    // Skip unreadable
  }

  return null;
}

// =============================================================================
// Main Validation
// =============================================================================

/**
 * Run Bible verse validation across AsciiDoc files.
 *
 * Parses citation patterns, looks up canonical text from the Bereshit
 * scripture data, and compares quoted text to source.
 */
export async function runBibleValidation(
  paths: RuntimePaths,
  options: BibleValidationOptions = {}
): Promise<BibleValidationResult> {
  const startTime = Date.now();
  const citations: ParsedCitation[] = [];

  // Resolve scripture data path
  const scripturePath = resolveScripturePath(paths.sourceDir);
  if (!scripturePath) {
    return {
      citations: [{
        file: '',
        line: 0,
        book: '',
        bookNormalized: '',
        chapter: 0,
        verse: 0,
        translation: 'KJV',
        status: 'not-found',
        details: ['Scripture data not found. Expected at: Bereshit/word/scripture/'],
      }],
      counts: { verified: 0, mismatch: 0, 'not-found': 1, 'ref-only': 0 },
      filesChecked: 0,
      duration: Date.now() - startTime,
    };
  }

  // Find files to check
  const files = findFiles(paths.bookDir, options);

  for (const filePath of files) {
    let content: string;
    try {
      content = readFileSync(filePath, 'utf-8');
    } catch {
      continue;
    }

    const lines = content.split('\n');

    // Track block comment state (between //// markers)
    let inBlockComment = false;

    // Find all citation patterns
    for (let lineIdx = 0; lineIdx < lines.length; lineIdx++) {
      const line = lines[lineIdx] ?? '';
      const trimmed = line.trim();

      // Toggle block comment state on //// boundaries
      if (trimmed === '////') {
        inBlockComment = !inBlockComment;
        continue;
      }

      // Skip line comments and block comment content
      if (inBlockComment || trimmed.startsWith('//')) {
        continue;
      }

      // Skip lines inside AsciiDoc file header comments (/* ... */ style)
      if (trimmed.startsWith('*') && !trimmed.startsWith('**')) {
        continue;
      }

      // Reset regex lastIndex for each line
      CITATION_REGEX.lastIndex = 0;
      let match: RegExpExecArray | null;

      while ((match = CITATION_REGEX.exec(line)) !== null) {
        const bookRaw = (match[1] ?? '').trim();
        const chapter = parseInt(match[2] ?? '0', 10);
        const verseStart = parseInt(match[3] ?? '0', 10);
        const verseEnd = match[4] ? parseInt(match[4], 10) : undefined;
        const translationRaw = match[5]?.toUpperCase() ?? 'KJV';
        const translation: BibleTranslation =
          translationRaw === 'WEB' ? 'WEB' : 'KJV';

        // Normalize book name for filesystem
        const bookNormalized = BOOK_MAP[bookRaw];
        if (!bookNormalized) {
          citations.push({
            file: filePath,
            line: lineIdx + 1,
            book: bookRaw,
            bookNormalized: bookRaw.replace(/\s+/g, '_'),
            chapter,
            verse: verseStart,
            verseEnd,
            translation,
            status: 'not-found',
            details: [`Unknown book name: "${bookRaw}"`],
          });
          continue;
        }

        // Read canonical text
        const canonical = verseEnd
          ? readVerseRange(scripturePath, translation, bookNormalized, chapter, verseStart, verseEnd)
          : readVerse(scripturePath, translation, bookNormalized, chapter, verseStart);

        if (canonical === null) {
          citations.push({
            file: filePath,
            line: lineIdx + 1,
            book: bookRaw,
            bookNormalized,
            chapter,
            verse: verseStart,
            verseEnd,
            translation,
            status: 'not-found',
            details: [`Verse file not found for ${translation}/${bookNormalized}/Chapter_${chapter}/Verse_${verseStart}.txt`],
          });
          continue;
        }

        // Extract quoted text from source
        const quotedText = extractQuotedText(lines, lineIdx);

        if (!quotedText || quotedText.length < 5) {
          // Reference-only citation (no quoted text to verify)
          citations.push({
            file: filePath,
            line: lineIdx + 1,
            book: bookRaw,
            bookNormalized,
            chapter,
            verse: verseStart,
            verseEnd,
            translation,
            canonicalText: canonical,
            status: 'ref-only',
          });
          continue;
        }

        // Compare quoted text to canonical
        const matches = compareTexts(quotedText, canonical);

        citations.push({
          file: filePath,
          line: lineIdx + 1,
          book: bookRaw,
          bookNormalized,
          chapter,
          verse: verseStart,
          verseEnd,
          translation,
          quotedText,
          canonicalText: canonical,
          status: matches ? 'verified' : 'mismatch',
          details: matches ? undefined : [
            `Quoted:    "${quotedText.substring(0, 80)}${quotedText.length > 80 ? '...' : ''}"`,
            `Canonical: "${canonical.substring(0, 80)}${canonical.length > 80 ? '...' : ''}"`,
          ],
        });
      }
    }
  }

  // Count results
  const counts: Record<CitationStatus, number> = {
    verified: 0,
    mismatch: 0,
    'not-found': 0,
    'ref-only': 0,
  };

  for (const cite of citations) {
    counts[cite.status]++;
  }

  return {
    citations,
    counts,
    filesChecked: files.length,
    duration: Date.now() - startTime,
  };
}
