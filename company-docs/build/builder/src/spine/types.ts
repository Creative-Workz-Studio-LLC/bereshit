/**
 * CWS Manual Builder — Spine Types
 *
 * Type definitions for the book spine scanner and inventory system.
 * The spine is the structural skeleton of the book — Parts, Articles,
 * Sections discovered from the filesystem.
 *
 * Maps to Makefile's spine targets:
 *   A = Frontmatter    C = Appendices
 *   B = Content         D = Backmatter
 *
 * "Write the vision, and make it plain upon tables,
 *  that he may run that readeth it." — Habakkuk 2:2
 */

// =============================================================================
// Spine Scope
// =============================================================================

/**
 * The four spine scopes — top-level divisions of the book.
 * Maps directly to the Makefile's A/B/C/D targets.
 */
export type SpineScope = 'A' | 'B' | 'C' | 'D';

/**
 * Human-readable scope names.
 */
export const SCOPE_NAMES: Record<SpineScope, string> = {
  A: 'Frontmatter',
  B: 'Content',
  C: 'Appendices',
  D: 'Backmatter',
};

/**
 * Scope directory names as they appear on disk.
 */
export const SCOPE_DIRS: Record<SpineScope, string> = {
  A: 'A-frontmatter',
  B: 'B-content',
  C: 'C-appendices',
  D: 'D-backmatter',
};

// =============================================================================
// Spine Nodes
// =============================================================================

/**
 * A single section file within an article.
 */
export interface SpineSection {
  /** Section letter (A, B, C, ...) */
  letter: string;

  /** Section name derived from filename (e.g., "ENTITY-OVERVIEW") */
  name: string;

  /** Full filename (e.g., "A-ENTITY-OVERVIEW.adoc") */
  filename: string;

  /** Absolute path to the file */
  path: string;

  /** File size in bytes */
  size: number;
}

/**
 * An article directory containing sections.
 * Maps to Article-{NN}-{name}/ directories.
 */
export interface SpineArticle {
  /** Article number (e.g., "01", "02") */
  id: string;

  /** Article name derived from directory (e.g., "entity-overview") */
  name: string;

  /** Directory basename (e.g., "Article-01-entity-overview") */
  dirname: string;

  /** Absolute path to the article directory */
  path: string;

  /** Sections within this article, sorted by letter */
  sections: SpineSection[];
}

/**
 * A part directory containing articles.
 * Maps to Part-{N}-{name}/ directories.
 */
export interface SpinePart {
  /** Part number (e.g., "1", "2") */
  id: string;

  /** Part name derived from directory (e.g., "identity") */
  name: string;

  /** Directory basename (e.g., "Part-1-identity") */
  dirname: string;

  /** Absolute path to the part directory */
  path: string;

  /** Articles within this part, sorted by ID */
  articles: SpineArticle[];
}

/**
 * A scope-level directory (frontmatter, content, appendices, backmatter).
 * For scope B, contains Parts. For others, contains loose files.
 */
export interface SpineScopeNode {
  /** Scope letter (A, B, C, D) */
  scope: SpineScope;

  /** Human-readable name */
  name: string;

  /** Directory basename on disk */
  dirname: string;

  /** Absolute path */
  path: string;

  /** Parts (only populated for scope B) */
  parts: SpinePart[];

  /** Loose files (for non-content scopes) */
  files: SpineSection[];
}

// =============================================================================
// Spine Tree
// =============================================================================

/**
 * Complete spine tree — the full structural map of the book.
 */
export interface SpineTree {
  /** Absolute path to the book root directory */
  bookDir: string;

  /** All four scopes */
  scopes: Record<SpineScope, SpineScopeNode>;

  /** Whether the assets directory exists */
  hasAssets: boolean;

  /** Whether book.adoc exists */
  hasMaster: boolean;

  /** Scan timestamp */
  scannedAt: Date;
}

// =============================================================================
// Inventory Summary
// =============================================================================

/**
 * Summary statistics for the inventory report.
 */
export interface InventorySummary {
  /** Total scope directories found (out of 4) */
  scopeCount: number;

  /** Total parts found */
  partCount: number;

  /** Total articles found */
  articleCount: number;

  /** Total section files found */
  sectionCount: number;

  /** Total .adoc files across all scopes */
  totalFiles: number;

  /** Total size of all .adoc files in bytes */
  totalSize: number;

  /** Empty articles (directories with no .adoc files) */
  emptyArticles: string[];
}

/**
 * Per-part inventory entry for the parts list.
 */
export interface PartInventory {
  /** Part ID */
  id: string;

  /** Part name */
  name: string;

  /** Number of articles */
  articleCount: number;

  /** Number of sections across all articles */
  sectionCount: number;

  /** Articles with their section counts */
  articles: Array<{
    id: string;
    name: string;
    sectionCount: number;
    letterRange: string;
  }>;
}

/**
 * Complete inventory report.
 */
export interface InventoryReport {
  /** Summary statistics */
  summary: InventorySummary;

  /** Per-part inventory (content scope only) */
  parts: PartInventory[];

  /** Scope file counts for non-content scopes */
  scopeFiles: Record<string, number>;
}
