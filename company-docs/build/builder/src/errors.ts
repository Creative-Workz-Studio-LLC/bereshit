/**
 * CWS Manual Builder — Error Handling Module
 *
 * Typed error codes, severity levels, and consistent error formatting.
 * Mirrors the Makefile error code system (E0-E4x) with TypeScript typing.
 *
 * Error categories:
 *   E1x — Usage (arguments, scope, input)
 *   E2x — Dependency (files, tools, paths)
 *   E3x — Build (compilation, rendering, extraction)
 *   E4x — Validation (structural, editorial, data)
 *   E5x — Theme (typography, page layout, elements)
 *   E6x — Scripture (verse lookup, text matching)
 *   E7x — Report (witness generation, config drift)
 *
 * "Surely the Lord GOD will do nothing, but he revealeth
 *  his secret unto his servants the prophets." — Amos 3:7
 */

// =============================================================================
// Error Code Definitions
// =============================================================================

/** Error categories matching exit code groups. */
export type ErrorCategory = 'usage' | 'dependency' | 'build' | 'validation' | 'theme' | 'scripture' | 'report';

/** Error severity levels. */
export type ErrorSeverity = 'error' | 'warn' | 'info';

/** All defined error codes. */
export type ErrorCode =
  // Success
  | 'E0'
  // Usage errors (exit 1)
  | 'E10' | 'E11' | 'E12'
  // Dependency errors (exit 2)
  | 'E20' | 'E21' | 'E22' | 'E23'
  // Build errors (exit 3)
  | 'E30' | 'E31' | 'E32' | 'E33' | 'E34'
  // Validation errors (exit 4)
  | 'E40' | 'E41' | 'E42' | 'E43'
  // Theme validation errors (exit 5)
  | 'E50' | 'E51' | 'E52' | 'E53'
  // Scripture/Bible validation errors (exit 6)
  | 'E60' | 'E61'
  // Report/config management errors (exit 7)
  | 'E70' | 'E71' | 'E72';

/** Metadata for each error code. */
interface ErrorCodeDef {
  /** Numeric code string */
  code: ErrorCode;

  /** Human-readable label */
  label: string;

  /** Category for grouping */
  category: ErrorCategory;

  /** Process exit code */
  exitCode: number;

  /** Default severity */
  severity: ErrorSeverity;

  /** Description of when this error occurs */
  description: string;
}

/** Complete error code registry. */
export const ERROR_CODES: Record<ErrorCode, ErrorCodeDef> = {
  // Success
  E0:  { code: 'E0',  label: 'Success',             category: 'usage',      exitCode: 0, severity: 'info',  description: 'Target completed without errors' },

  // Usage errors (exit 1)
  E10: { code: 'E10', label: 'Missing argument',     category: 'usage',      exitCode: 1, severity: 'error', description: 'Required argument not provided (FILE=, PART=, ARTICLE=, PAGES=)' },
  E11: { code: 'E11', label: 'Invalid value',        category: 'usage',      exitCode: 1, severity: 'error', description: 'Argument value is invalid (bad scope, unknown part, out-of-range pages)' },
  E12: { code: 'E12', label: 'Conflicting arguments', category: 'usage',     exitCode: 1, severity: 'error', description: 'Mutually exclusive arguments provided' },

  // Dependency errors (exit 2)
  E20: { code: 'E20', label: 'Source not found',     category: 'dependency', exitCode: 2, severity: 'error', description: 'Resolved source file does not exist' },
  E21: { code: 'E21', label: 'Tool not found',       category: 'dependency', exitCode: 2, severity: 'error', description: 'Required tool not installed (asciidoctor-pdf, qpdf, entr, node)' },
  E22: { code: 'E22', label: 'Directory not found',  category: 'dependency', exitCode: 2, severity: 'error', description: 'Required directory does not exist' },
  E23: { code: 'E23', label: 'No matching files',    category: 'dependency', exitCode: 2, severity: 'warn',  description: 'Scope or glob matched zero files' },

  // Build errors (exit 3)
  E30: { code: 'E30', label: 'PDF build failed',     category: 'build',      exitCode: 3, severity: 'error', description: 'asciidoctor-pdf returned non-zero exit code' },
  E31: { code: 'E31', label: 'Asset generation failed', category: 'build',   exitCode: 3, severity: 'error', description: 'Mermaid, ImageMagick, or other asset tool failed' },
  E32: { code: 'E32', label: 'Extraction failed',    category: 'build',      exitCode: 3, severity: 'error', description: 'qpdf page extraction or PDF read failed' },
  E33: { code: 'E33', label: 'TS build failed',      category: 'build',      exitCode: 3, severity: 'error', description: 'TypeScript compilation or npm build failed' },
  E34: { code: 'E34', label: 'Partial batch failure', category: 'build',     exitCode: 3, severity: 'warn',  description: 'Some files failed in a batch build (others succeeded)' },

  // Validation errors (exit 4)
  E40: { code: 'E40', label: 'Validation failed',    category: 'validation', exitCode: 4, severity: 'error', description: 'Structure validation checks found errors' },
  E41: { code: 'E41', label: 'Page validation failed', category: 'validation', exitCode: 4, severity: 'error', description: 'Page marker gaps, missing banners, sequence errors' },
  E42: { code: 'E42', label: 'TOC validation failed', category: 'validation', exitCode: 4, severity: 'error', description: 'TOC structure issues (missing sections, placeholders)' },
  E43: { code: 'E43', label: 'Lint failure',          category: 'validation', exitCode: 4, severity: 'warn',  description: 'Editorial style violations found' },

  // Theme validation errors (exit 5)
  E50: { code: 'E50', label: 'Theme validation failed',   category: 'theme',     exitCode: 5, severity: 'error', description: 'Theme YAML fails typography spec validation' },
  E51: { code: 'E51', label: 'Page layout mismatch',      category: 'theme',     exitCode: 5, severity: 'error', description: 'Theme page layout diverges from config spec' },
  E52: { code: 'E52', label: 'Theme parse error',         category: 'theme',     exitCode: 5, severity: 'error', description: 'Theme YAML cannot be parsed' },
  E53: { code: 'E53', label: 'Theme element mismatch',    category: 'theme',     exitCode: 5, severity: 'warn',  description: 'Theme element settings diverge from spec' },

  // Scripture/Bible validation errors (exit 6)
  E60: { code: 'E60', label: 'Verse not found',           category: 'scripture', exitCode: 6, severity: 'error', description: 'Cited verse not found in scripture data' },
  E61: { code: 'E61', label: 'Verse text mismatch',       category: 'scripture', exitCode: 6, severity: 'warn',  description: 'Cited text differs from canonical scripture' },

  // Report/config management errors (exit 7)
  E70: { code: 'E70', label: 'Report generation failed',  category: 'report',    exitCode: 7, severity: 'error', description: 'Witness report could not be generated' },
  E71: { code: 'E71', label: 'Partial validation failure', category: 'report',   exitCode: 7, severity: 'warn',  description: 'Some validation domains failed to run' },
  E72: { code: 'E72', label: 'Config version mismatch',   category: 'report',    exitCode: 7, severity: 'warn',  description: 'Config version does not match builder version' },
};

// =============================================================================
// Error Construction
// =============================================================================

/** A structured error with code, message, and context. */
export interface BuildError {
  /** Error code from the registry */
  code: ErrorCode;

  /** Human-readable message */
  message: string;

  /** Source file (if applicable) */
  file?: string;

  /** Source line (if applicable) */
  line?: number;

  /** Severity override (defaults to code's severity) */
  severity?: ErrorSeverity;

  /** Additional context for debugging */
  context?: Record<string, string | number | boolean>;
}

/**
 * Create a structured build error.
 */
export function createError(
  code: ErrorCode,
  message: string,
  options?: {
    file?: string;
    line?: number;
    severity?: ErrorSeverity;
    context?: Record<string, string | number | boolean>;
  }
): BuildError {
  return {
    code,
    message,
    file: options?.file,
    line: options?.line,
    severity: options?.severity ?? ERROR_CODES[code].severity,
    context: options?.context,
  };
}

// =============================================================================
// Error Formatting
// =============================================================================

/**
 * Format an error for terminal output.
 *
 * Format: "Error [E20]: Source file 'A-COVER.adoc' not found."
 *         "  WARN [E34]: Build failed for B-TOC.adoc (continuing batch)"
 *         "  INFO [E23]: No files matched scope 'C'"
 */
export function formatError(error: BuildError): string {
  const def = ERROR_CODES[error.code];
  const severity = error.severity ?? def.severity;

  let prefix: string;
  switch (severity) {
    case 'error': prefix = `Error [${error.code}]`; break;
    case 'warn':  prefix = `  WARN [${error.code}]`; break;
    case 'info':  prefix = `  INFO [${error.code}]`; break;
  }

  let location = '';
  if (error.file) {
    location = error.line ? ` (${error.file}:${error.line})` : ` (${error.file})`;
  }

  return `${prefix}: ${error.message}${location}`;
}

/**
 * Format multiple errors for terminal output.
 */
export function formatErrors(errors: BuildError[]): string {
  return errors.map(formatError).join('\n');
}

// =============================================================================
// Error Collection
// =============================================================================

/** Collects errors during a multi-step operation. */
export class ErrorCollector {
  private errors: BuildError[] = [];

  /** Add an error to the collection. */
  add(code: ErrorCode, message: string, options?: Parameters<typeof createError>[2]): void {
    this.errors.push(createError(code, message, options));
  }

  /** Get all collected errors. */
  all(): readonly BuildError[] {
    return this.errors;
  }

  /** Get errors by severity. */
  bySeverity(severity: ErrorSeverity): BuildError[] {
    return this.errors.filter(e => (e.severity ?? ERROR_CODES[e.code].severity) === severity);
  }

  /** Get errors by category. */
  byCategory(category: ErrorCategory): BuildError[] {
    return this.errors.filter(e => ERROR_CODES[e.code].category === category);
  }

  /** Count errors by severity. */
  counts(): { errors: number; warnings: number; info: number; total: number } {
    return {
      errors: this.bySeverity('error').length,
      warnings: this.bySeverity('warn').length,
      info: this.bySeverity('info').length,
      total: this.errors.length,
    };
  }

  /** Whether any error-severity issues exist. */
  hasErrors(): boolean {
    return this.bySeverity('error').length > 0;
  }

  /** Whether any issues exist at all. */
  hasAny(): boolean {
    return this.errors.length > 0;
  }

  /** Get the highest exit code from all errors. */
  exitCode(): number {
    if (this.errors.length === 0) return 0;
    return Math.max(...this.errors.map(e => ERROR_CODES[e.code].exitCode));
  }

  /** Format all errors for terminal output. */
  format(): string {
    return formatErrors(this.errors);
  }

  /** Clear all collected errors. */
  clear(): void {
    this.errors = [];
  }
}

// =============================================================================
// Section Type Detection
// =============================================================================

/** Section types based on filename prefix and position in book spine. */
export type SectionType =
  | 'cover'
  | 'toc'
  | 'dedication'
  | 'preface'
  | 'glossary'
  | 'content'
  | 'appendix'
  | 'backmatter'
  | 'unknown';

/** Scope categories (A-D spine directories). */
export type ScopeCategory = 'frontmatter' | 'content' | 'appendices' | 'backmatter';

/** Mapping from filename patterns to section types. */
const SECTION_TYPE_PATTERNS: Array<{ pattern: RegExp; type: SectionType; scope: ScopeCategory }> = [
  // Frontmatter (A-*) — order matters, checked sequentially
  { pattern: /^A-COVER/i,                  type: 'cover',      scope: 'frontmatter' },
  { pattern: /TABLE.?OF.?CONTENTS|^B-T/i,  type: 'toc',        scope: 'frontmatter' },
  { pattern: /DEDICATION|^C-DED/i,         type: 'dedication',  scope: 'frontmatter' },
  { pattern: /PREFACE|^D-PRE/i,            type: 'preface',     scope: 'frontmatter' },
  { pattern: /GLOSSARY|^E-GLOS/i,          type: 'glossary',    scope: 'frontmatter' },

  // Content (B-*)
  { pattern: /^Article-|^Part-/i,          type: 'content',     scope: 'content' },

  // Appendices (C-*)
  { pattern: /^Appendix-|^App-/i,          type: 'appendix',    scope: 'appendices' },

  // Backmatter (D-*)
  { pattern: /^Index|^Colophon|^Back/i,    type: 'backmatter',  scope: 'backmatter' },
];

/**
 * Detect section type from filename and/or directory path.
 */
export function detectSectionType(filename: string, dirPath?: string): SectionType {
  // Try filename patterns first
  for (const { pattern, type } of SECTION_TYPE_PATTERNS) {
    if (pattern.test(filename)) return type;
  }

  // Fallback: detect from parent directory
  if (dirPath) {
    if (dirPath.includes('A-frontmatter')) return 'cover';  // unknown frontmatter file
    if (dirPath.includes('B-content')) return 'content';
    if (dirPath.includes('C-appendices')) return 'appendix';
    if (dirPath.includes('D-backmatter')) return 'backmatter';
  }

  return 'unknown';
}

/**
 * Get scope category from directory path.
 */
export function detectScope(dirPath: string): ScopeCategory | null {
  if (dirPath.includes('A-frontmatter')) return 'frontmatter';
  if (dirPath.includes('B-content')) return 'content';
  if (dirPath.includes('C-appendices')) return 'appendices';
  if (dirPath.includes('D-backmatter')) return 'backmatter';
  return null;
}

/**
 * Get display label for a section type.
 */
export function sectionTypeLabel(type: SectionType): string {
  const labels: Record<SectionType, string> = {
    cover: 'Cover',
    toc: 'Table of Contents',
    dedication: 'Dedication',
    preface: 'Preface',
    glossary: 'Glossary',
    content: 'Content',
    appendix: 'Appendix',
    backmatter: 'Back Matter',
    unknown: 'Unknown',
  };
  return labels[type];
}

/**
 * Get short flag for a section type (for compact display).
 */
export function sectionTypeFlag(type: SectionType): string {
  const flags: Record<SectionType, string> = {
    cover: 'CVR',
    toc: 'TOC',
    dedication: 'DED',
    preface: 'PRF',
    glossary: 'GLS',
    content: 'CNT',
    appendix: 'APX',
    backmatter: 'BKM',
    unknown: '???',
  };
  return flags[type];
}
