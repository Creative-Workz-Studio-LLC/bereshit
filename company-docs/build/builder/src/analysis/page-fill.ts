/**
 * Page Fill Analysis — Measured content density from rendered PDF pages.
 *
 * Data source: rendered PDF (authoritative — this is what the reader sees).
 *
 * Workflow position:
 *   Source → build PDF → [page-fill] → measured fill per page
 *   Measured fill → annotate source ("// end p.N — XX%, N lines remaining")
 *   Measured fill → compare with predicted fill (accuracy validation)
 *
 * Measures how much of each page's available content area is used,
 * enabling automated detection of "full" pages where adding content
 * would cause bleed to the next page.
 *
 * Uses pdftotext (poppler-utils) for text position extraction.
 * Configurable threshold determines what constitutes "full."
 *
 * "Write the vision, and make it plain upon tables,
 *  that he may run that readeth it." — Habakkuk 2:2
 */

import { execSync } from 'child_process';
import { existsSync } from 'fs';
import type { PageLayoutConfig, PageFillConfig, FillRange } from '../config/types.js';

// =============================================================================
// Types
// =============================================================================

/** Fill status classification — derived from config range names */
export type FillStatus = string;

/** Bleed risk level — how close the page is to overflowing */
export type BleedRisk = 'safe' | 'tight' | 'bleed-risk';

/** Result for a single page */
export interface PageFillResult {
  /** 1-based page number */
  page: number;
  /** Content fill ratio (0.0 to 1.0+) */
  fillRatio: number;
  /** Classification based on thresholds */
  status: FillStatus;
  /** Lowest content Y position in points */
  lowestContentY: number;
  /** Available content height in points (between margins, excluding header/footer) */
  availableHeight: number;
  /** Used content height in points */
  usedHeight: number;
  /** Percentage headroom remaining before 100% fill */
  headroom: number;
  /** Estimated lines that could fit in remaining space */
  linesRemaining: number;
  /** Bleed risk assessment */
  risk: BleedRisk;
}

/** Fill analysis configuration */
export interface FillConfig {
  /** Named ranges with min/max as ratios (0.0-1.0) */
  ranges: Record<string, { min: number; max: number }>;
  /** Minimum Y position to consider as content (filters header area, in points) */
  headerCutoff: number;
  /** Maximum Y position to consider as content (filters footer area, in points) */
  footerCutoff: number;
  /** Line height in points (for estimating remaining lines) */
  lineHeight: number;
  /** Fill ratio at or above which bleed is imminent (default: 0.95) */
  bleedThreshold: number;
  /** Fill ratio at or above which the page is tight (default: same as overfull.min) */
  tightThreshold: number;
}

/** Complete analysis result */
export interface FillAnalysis {
  /** Source PDF file */
  file: string;
  /** Total pages analyzed */
  totalPages: number;
  /** Per-page results */
  pages: PageFillResult[];
  /** Configuration used */
  config: FillConfig;
  /** Summary counts per status */
  summary: Record<string, number>;
}

// =============================================================================
// Constants
// =============================================================================

/** Points per inch */
const PPI = 72;

/** US Letter page dimensions in points */
const LETTER_HEIGHT = 792;

// =============================================================================
// Helpers
// =============================================================================

/**
 * Parse an "inches" string (e.g., "0.875in") to points.
 */
function inchesToPoints(value: string): number {
  const match = value.match(/^([\d.]+)\s*in$/);
  if (!match || !match[1]) return 0;
  return parseFloat(match[1]) * PPI;
}

/** Default fill ranges (used when no config provided) */
const DEFAULT_RANGES: Record<string, { min: number; max: number }> = {
  sparse:   { min: 0.00, max: 0.40 },
  normal:   { min: 0.40, max: 0.70 },
  full:     { min: 0.70, max: 0.90 },
  overfull: { min: 0.90, max: 1.00 },
};

/**
 * Convert config ranges (percentages) to ratio ranges (0.0-1.0).
 */
function configRangesToRatios(configRanges: Record<string, FillRange>): Record<string, { min: number; max: number }> {
  const result: Record<string, { min: number; max: number }> = {};
  for (const [name, range] of Object.entries(configRanges)) {
    result[name] = { min: range.min / 100, max: range.max / 100 };
  }
  return result;
}

/** Default line height when typography not available */
const DEFAULT_LINE_HEIGHT = 14.4; // 12pt × 1.2 leading

/**
 * Build fill config from page layout config and optional page fill config.
 * Derives header/footer cutoffs from margins and header/footer heights.
 */
export function buildFillConfig(
  pageLayout: PageLayoutConfig,
  pageFillConfig?: PageFillConfig,
  cliOverrides?: { fullThreshold?: number; overfullThreshold?: number },
  lineHeight?: number,
): FillConfig {
  const marginTop = inchesToPoints(pageLayout.margin_top);
  const marginBottom = inchesToPoints(pageLayout.margin_bottom);
  const headerHeight = inchesToPoints(pageLayout.header_height);
  const footerHeight = inchesToPoints(pageLayout.footer_height);

  const headerCutoff = marginTop + headerHeight;
  const footerCutoff = LETTER_HEIGHT - marginBottom - footerHeight;

  // Build ranges: config → CLI overrides → defaults
  let ranges = pageFillConfig?.ranges
    ? configRangesToRatios(pageFillConfig.ranges)
    : { ...DEFAULT_RANGES };

  // CLI overrides adjust the 'full' and 'overfull' boundaries
  if (cliOverrides?.fullThreshold !== undefined) {
    const ft = cliOverrides.fullThreshold;
    if (ranges.normal) ranges.normal.max = ft;
    if (ranges.full) ranges.full.min = ft;
  }
  if (cliOverrides?.overfullThreshold !== undefined) {
    const ot = cliOverrides.overfullThreshold;
    if (ranges.full) ranges.full.max = ot;
    if (ranges.overfull) ranges.overfull.min = ot;
  }

  // Bleed risk thresholds
  const bleedThreshold = pageFillConfig?.bleed_threshold
    ? pageFillConfig.bleed_threshold / 100
    : 0.95;
  const tightThreshold = ranges.overfull?.min ?? 0.90;

  return {
    ranges,
    headerCutoff,
    footerCutoff,
    lineHeight: lineHeight ?? DEFAULT_LINE_HEIGHT,
    bleedThreshold,
    tightThreshold,
  };
}

/**
 * Classify fill ratio into a status based on configured ranges.
 * Ranges are checked from highest to lowest — first match wins.
 */
function classifyFill(ratio: number, config: FillConfig): FillStatus {
  // Sort ranges by min descending — check highest first
  const sorted = Object.entries(config.ranges)
    .sort(([, a], [, b]) => b.min - a.min);

  for (const [name, range] of sorted) {
    if (ratio >= range.min) return name;
  }

  return sorted[sorted.length - 1]?.[0] ?? 'unknown';
}

// =============================================================================
// Core Analysis
// =============================================================================

/**
 * Extract word bounding boxes from a PDF page using pdftotext -bbox.
 * Returns array of yMax values for all words on the page.
 */
function extractWordPositions(pdfPath: string, page: number): number[] {
  try {
    const output = execSync(
      `pdftotext -bbox -f ${page} -l ${page} "${pdfPath}" -`,
      { encoding: 'utf-8', stdio: ['pipe', 'pipe', 'pipe'] },
    );

    const yMaxValues: number[] = [];
    const regex = /yMax="([\d.]+)"/g;
    let match: RegExpExecArray | null;

    while ((match = regex.exec(output)) !== null) {
      if (match[1]) yMaxValues.push(parseFloat(match[1]));
    }

    return yMaxValues;
  } catch {
    return [];
  }
}

/**
 * Get total page count from a PDF using qpdf.
 */
function getPageCount(pdfPath: string): number {
  try {
    const output = execSync(`qpdf --show-npages "${pdfPath}"`, {
      encoding: 'utf-8',
      stdio: ['pipe', 'pipe', 'pipe'],
    });
    return parseInt(output.trim(), 10);
  } catch {
    return 0;
  }
}

/**
 * Classify bleed risk based on fill ratio and config thresholds.
 */
function classifyBleedRisk(fillRatio: number, config: FillConfig): BleedRisk {
  if (fillRatio >= config.bleedThreshold) return 'bleed-risk';
  if (fillRatio >= config.tightThreshold) return 'tight';
  return 'safe';
}

/**
 * Analyze content fill for a single page.
 */
function analyzePage(pdfPath: string, page: number, config: FillConfig): PageFillResult {
  const yMaxValues = extractWordPositions(pdfPath, page);

  // Filter to content area only (exclude header/footer)
  const contentYValues = yMaxValues.filter(
    (y) => y > config.headerCutoff && y < config.footerCutoff,
  );

  const availableHeight = config.footerCutoff - config.headerCutoff;
  const lowestContentY = contentYValues.length > 0 ? Math.max(...contentYValues) : config.headerCutoff;
  const usedHeight = lowestContentY - config.headerCutoff;
  const fillRatio = availableHeight > 0 ? usedHeight / availableHeight : 0;

  // Headroom and line estimation
  const remainingHeight = Math.max(availableHeight - usedHeight, 0);
  const headroom = Math.max(1 - fillRatio, 0);
  const linesRemaining = config.lineHeight > 0
    ? Math.floor(remainingHeight / config.lineHeight)
    : 0;

  return {
    page,
    fillRatio,
    status: classifyFill(fillRatio, config),
    lowestContentY,
    availableHeight,
    usedHeight,
    headroom,
    linesRemaining,
    risk: classifyBleedRisk(fillRatio, config),
  };
}

/**
 * Parse a page range string (e.g., "1-5", "3", "1-3,7-9") into an array of page numbers.
 */
export function parsePageRange(range: string, maxPage: number): number[] {
  const pages: Set<number> = new Set();

  for (const part of range.split(',')) {
    const trimmed = part.trim();
    const dashMatch = trimmed.match(/^(\d+)-(\d+)$/);

    if (dashMatch && dashMatch[1] && dashMatch[2]) {
      const start = parseInt(dashMatch[1], 10);
      const end = Math.min(parseInt(dashMatch[2], 10), maxPage);
      for (let i = start; i <= end; i++) pages.add(i);
    } else {
      const num = parseInt(trimmed, 10);
      if (num >= 1 && num <= maxPage) pages.add(num);
    }
  }

  return Array.from(pages).sort((a, b) => a - b);
}

// =============================================================================
// Public API
// =============================================================================

/**
 * Analyze content fill for a PDF file.
 *
 * @param pdfPath - Path to the PDF file
 * @param config - Fill configuration (from buildFillConfig)
 * @param pageRange - Optional page range string (default: all pages)
 * @returns Complete fill analysis
 */
export function analyzePageFill(
  pdfPath: string,
  config: FillConfig,
  pageRange?: string,
): FillAnalysis {
  if (!existsSync(pdfPath)) {
    throw new Error(`PDF not found: ${pdfPath}`);
  }

  const totalPages = getPageCount(pdfPath);
  if (totalPages === 0) {
    throw new Error(`Could not read page count from: ${pdfPath}`);
  }

  const pagesToAnalyze = pageRange
    ? parsePageRange(pageRange, totalPages)
    : Array.from({ length: totalPages }, (_, i) => i + 1);

  const pages = pagesToAnalyze.map((p) => analyzePage(pdfPath, p, config));

  // Build summary from configured range names
  const summary: Record<string, number> = {};
  for (const name of Object.keys(config.ranges)) {
    summary[name] = 0;
  }
  for (const page of pages) {
    summary[page.status] = (summary[page.status] ?? 0) + 1;
  }

  return {
    file: pdfPath,
    totalPages,
    pages,
    config,
    summary,
  };
}
