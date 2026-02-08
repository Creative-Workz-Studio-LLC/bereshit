/**
 * Page Fill Cross-Check — Validates source markers against PDF fill analysis.
 *
 * Compares human-authored `// end p.N — content full` markers in AsciiDoc
 * source files against machine-measured page content density from PDFs.
 * Flags discrepancies: stale markers, missing markers, page count drift.
 *
 * "Diverse weights and diverse measures — both of them alike
 *  are an abomination to the LORD." — Proverbs 20:10
 */

import { readFileSync } from 'fs';
import type { FillCrossCheck } from '../config/types.js';
import type { FillAnalysis, PageFillResult } from './page-fill.js';

// =============================================================================
// Types
// =============================================================================

/** A source marker parsed from an .adoc file */
export interface SourceMarker {
  /** 1-based page number from the marker */
  page: number;
  /** Whether the marker includes "content full" annotation */
  markedFull: boolean;
  /** Line number in the source file */
  line: number;
  /** Raw marker text */
  raw: string;
}

/** A single cross-check finding */
export interface CrossCheckFinding {
  /** Finding type */
  type: 'stale_marker' | 'missing_marker' | 'page_count_drift' | 'marker_confirmed';
  /** Severity */
  severity: 'error' | 'warning' | 'info';
  /** 1-based page number */
  page: number;
  /** Human-readable message */
  message: string;
  /** Measured fill ratio (if available) */
  fillRatio?: number;
  /** Source line number (if available) */
  line?: number;
}

/** Complete cross-check result */
export interface CrossCheckResult {
  /** Source file analyzed */
  sourceFile: string;
  /** PDF file analyzed */
  pdfFile: string;
  /** All findings */
  findings: CrossCheckFinding[];
  /** Markers found in source */
  markers: SourceMarker[];
  /** Summary counts */
  summary: {
    staleMarkers: number;
    missingMarkers: number;
    pageCountDrift: boolean;
    confirmed: number;
    total: number;
  };
}

// =============================================================================
// Source Parser
// =============================================================================

/**
 * Parse end-of-page markers from an AsciiDoc source file.
 * Matches patterns like:
 *   // end p.5
 *   // end p.5 — content full
 *   // end p.12 — content full
 */
export function parseSourceMarkers(sourceFile: string, markerPattern?: string): SourceMarker[] {
  const content = readFileSync(sourceFile, 'utf-8');
  const lines = content.split('\n');
  const markers: SourceMarker[] = [];

  // Default pattern matches: // end p.N [— content full]
  const pattern = markerPattern
    ? new RegExp(markerPattern)
    : /\/\/\s*end\s+p\.(\d+)(?:\s*—\s*content\s+full)?/;

  const fullPattern = /content\s+full/;

  for (let i = 0; i < lines.length; i++) {
    const line = lines[i]!;
    const match = line.match(pattern);
    if (match && match[1]) {
      markers.push({
        page: parseInt(match[1], 10),
        markedFull: fullPattern.test(line),
        line: i + 1,
        raw: line.trim(),
      });
    }
  }

  return markers;
}

/**
 * Count total PAGE markers (start markers) in source to get expected page count.
 */
export function countPageMarkers(sourceFile: string): number {
  const content = readFileSync(sourceFile, 'utf-8');
  const matches = content.match(/\/\/\s*PAGE\s+\d+/g);
  return matches ? matches.length : 0;
}

// =============================================================================
// Cross-Check Engine
// =============================================================================

/**
 * Cross-check source markers against fill analysis.
 */
export function crossCheckFill(
  sourceFile: string,
  analysis: FillAnalysis,
  settings: FillCrossCheck,
  markerPattern?: string,
): CrossCheckResult {
  const markers = parseSourceMarkers(sourceFile, markerPattern);
  const findings: CrossCheckFinding[] = [];

  // Build lookup: page number → fill result
  const fillByPage = new Map<number, PageFillResult>();
  for (const page of analysis.pages) {
    fillByPage.set(page.page, page);
  }

  // Build lookup: page number → source marker
  const markerByPage = new Map<number, SourceMarker>();
  for (const marker of markers) {
    markerByPage.set(marker.page, marker);
  }

  // 1. Check page count drift
  const sourcePageCount = countPageMarkers(sourceFile);
  if (settings.page_count_drift && sourcePageCount !== analysis.totalPages) {
    findings.push({
      type: 'page_count_drift',
      severity: 'error',
      page: 0,
      message: `Page count mismatch: source has ${sourcePageCount} PAGE markers, PDF has ${analysis.totalPages} pages`,
    });
  }

  // 2. Check each page
  for (const page of analysis.pages) {
    const marker = markerByPage.get(page.page);
    const fillPct = Math.round(page.fillRatio * 100);

    if (marker?.markedFull) {
      // Page IS marked "content full" — validate it
      if (fillPct < settings.stale_marker_below) {
        findings.push({
          type: 'stale_marker',
          severity: 'warning',
          page: page.page,
          message: `Page ${page.page} marked "content full" but only ${fillPct}% filled (threshold: ${settings.stale_marker_below}%)`,
          fillRatio: page.fillRatio,
          line: marker.line,
        });
      } else {
        findings.push({
          type: 'marker_confirmed',
          severity: 'info',
          page: page.page,
          message: `Page ${page.page} "content full" marker confirmed (${fillPct}% filled)`,
          fillRatio: page.fillRatio,
          line: marker.line,
        });
      }
    } else {
      // Page is NOT marked "content full" — check if it should be
      if (fillPct >= settings.missing_marker_above) {
        findings.push({
          type: 'missing_marker',
          severity: 'warning',
          page: page.page,
          message: `Page ${page.page} is ${fillPct}% filled but not marked "content full" (threshold: ${settings.missing_marker_above}%)`,
          fillRatio: page.fillRatio,
          line: marker?.line,
        });
      }
    }
  }

  // Summary
  const summary = {
    staleMarkers: findings.filter((f) => f.type === 'stale_marker').length,
    missingMarkers: findings.filter((f) => f.type === 'missing_marker').length,
    pageCountDrift: findings.some((f) => f.type === 'page_count_drift'),
    confirmed: findings.filter((f) => f.type === 'marker_confirmed').length,
    total: findings.length,
  };

  return {
    sourceFile,
    pdfFile: analysis.file,
    findings,
    markers,
    summary,
  };
}
