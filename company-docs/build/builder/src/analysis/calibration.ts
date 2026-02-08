/**
 * Calibration Engine — Loads measured render ratios for fill prediction.
 *
 * Workflow position:
 *   Calibration PDF → manual measurement → calibration-ratios.json
 *     → loadCalibration() → RenderCalibration → predictor uses these ratios
 *
 * Calibration data is measured once from a rendered calibration PDF, then
 * stored as JSON. The predictor applies these ratios to source elements
 * to estimate fill without building a PDF.
 *
 * "Divers weights are an abomination unto the LORD;
 *  and a false balance is not good." — Proverbs 20:23
 */

import { readFileSync, existsSync } from 'fs';
import { dirname, resolve } from 'path';
import { fileURLToPath } from 'url';
import type { RenderCalibration } from './render-types.js';

// ESM equivalent of __dirname (CommonJS doesn't have import.meta.url)
const __filename = fileURLToPath(import.meta.url);
const __dirname = dirname(__filename);

// =============================================================================
// Default Calibration
// =============================================================================

/**
 * Default calibration measured from cws-manual-legal-theme.yml.
 * Used when no calibration file is available.
 * All values in points (72pt = 1 inch).
 */
const DEFAULT_CALIBRATION: RenderCalibration = {
  body: {
    lineHeight: 15.0,
    linePitch: 20.5,
    paragraphSpacing: 17.5,
  },
  headings: {
    h2: { height: 30.0, gapBefore: 0, gapAfter: 17.0 },
    h3: { height: 23.2, gapBefore: 34.4, gapAfter: 12.5 },
    h4: { height: 19.1, gapBefore: 30.1, gapAfter: 10.1 },
    discrete: { height: 23.2, gapBefore: 34.4, gapAfter: 12.4 },
  },
  lists: {
    itemHeight: 15.0,
    itemPitch: 26.5,
    firstItemGap: 12.5,
    spacingAfter: 17.5,
  },
  smallText: {
    lineHeight: 12.3,
    linePitch: 16.8,
    gapBefore: 12.0,
    gapAfter: 16.5,
  },
  scripture: {
    lineHeight: 12.3,
    interBlockGap: 16.5,
    gapAfter: 17.0,
  },
  thematicBreak: {
    totalHeight: 41.5,
  },
  tables: {
    headerHeight: 15.0,
    rowHeight: 15.0,
    rowPitch: 30.5,
    gapBefore: 17.5,
  },
  page: {
    height: 792.0,
    headerCutoff: 40.0,
    footerCutoff: 750.0,
    usableHeight: 710.0,
    contentStartChapter: 65.2,
    contentStartSection: 64.7,
  },
  meta: {
    source: 'default (hardcoded from cws-manual-legal-theme.yml measurement)',
    measuredDate: '2026-02-08',
    theme: 'cws-manual-legal-theme.yml',
  },
};

// =============================================================================
// Public API
// =============================================================================

/**
 * Load calibration from a JSON file.
 * If no path given, tries auto-discovery relative to module location.
 * Falls back to default calibration if file not found.
 */
export function loadCalibration(calibrationPath?: string): RenderCalibration {
  // If no explicit path, try auto-discovery
  const targetPath = calibrationPath ?? findCalibrationFile();

  if (!existsSync(targetPath)) {
    return { ...DEFAULT_CALIBRATION };
  }

  try {
    const raw = JSON.parse(readFileSync(targetPath, 'utf-8'));
    return mapJsonToCalibration(raw, targetPath);
  } catch {
    return { ...DEFAULT_CALIBRATION };
  }
}

/**
 * Get the default calibration (no file needed).
 */
export function getDefaultCalibration(): RenderCalibration {
  return { ...DEFAULT_CALIBRATION };
}

/**
 * Find the calibration file relative to the builder source.
 * If no buildDir is provided, resolves relative to this module's location
 * (src/analysis/ → ../../_calibration/ in source, or dist/analysis/ → ../../build/_calibration/ at runtime).
 */
export function findCalibrationFile(buildDir?: string): string {
  if (buildDir) {
    return resolve(buildDir, '_calibration', 'calibration-ratios.json');
  }
  // Auto-discover: at runtime this file lives in dist/analysis/
  // (build/builder/dist/analysis/) and calibration data lives in
  // build/_calibration/ (three levels up from dist/analysis/)
  return resolve(__dirname, '..', '..', '..', '_calibration', 'calibration-ratios.json');
}

// =============================================================================
// Helpers
// =============================================================================

/**
 * Map raw JSON calibration data to typed RenderCalibration.
 */
function mapJsonToCalibration(raw: Record<string, unknown>, source: string): RenderCalibration {
  const body = raw.body as Record<string, number> | undefined;
  const headings = raw.headings as Record<string, Record<string, number>> | undefined;
  const lists = raw.lists as Record<string, number> | undefined;
  const smallText = raw.small_text as Record<string, number> | undefined;
  const scripture = raw.scripture as Record<string, number> | undefined;
  const thematicBreak = raw.thematic_break as Record<string, number> | undefined;
  const tables = raw.tables as Record<string, number> | undefined;
  const page = raw.page as Record<string, number> | undefined;

  return {
    body: {
      lineHeight: body?.line_height ?? DEFAULT_CALIBRATION.body.lineHeight,
      linePitch: body?.line_pitch ?? DEFAULT_CALIBRATION.body.linePitch,
      paragraphSpacing: body?.paragraph_spacing ?? DEFAULT_CALIBRATION.body.paragraphSpacing,
    },
    headings: {
      h2: {
        height: headings?.h2?.height ?? DEFAULT_CALIBRATION.headings.h2.height,
        gapBefore: headings?.h2?.gap_before ?? DEFAULT_CALIBRATION.headings.h2.gapBefore,
        gapAfter: headings?.h2?.gap_after ?? DEFAULT_CALIBRATION.headings.h2.gapAfter,
      },
      h3: {
        height: headings?.h3?.height ?? DEFAULT_CALIBRATION.headings.h3.height,
        gapBefore: headings?.h3?.gap_before ?? DEFAULT_CALIBRATION.headings.h3.gapBefore,
        gapAfter: headings?.h3?.gap_after ?? DEFAULT_CALIBRATION.headings.h3.gapAfter,
      },
      h4: {
        height: headings?.h4?.height ?? DEFAULT_CALIBRATION.headings.h4.height,
        gapBefore: headings?.h4?.gap_before ?? DEFAULT_CALIBRATION.headings.h4.gapBefore,
        gapAfter: headings?.h4?.gap_after ?? DEFAULT_CALIBRATION.headings.h4.gapAfter,
      },
      discrete: {
        height: headings?.discrete?.height ?? DEFAULT_CALIBRATION.headings.discrete.height,
        gapBefore: headings?.discrete?.gap_before ?? DEFAULT_CALIBRATION.headings.discrete.gapBefore,
        gapAfter: headings?.discrete?.gap_after ?? DEFAULT_CALIBRATION.headings.discrete.gapAfter,
      },
    },
    lists: {
      itemHeight: lists?.item_height ?? DEFAULT_CALIBRATION.lists.itemHeight,
      itemPitch: lists?.item_pitch ?? DEFAULT_CALIBRATION.lists.itemPitch,
      firstItemGap: lists?.first_item_gap ?? DEFAULT_CALIBRATION.lists.firstItemGap,
      spacingAfter: lists?.spacing_after ?? DEFAULT_CALIBRATION.lists.spacingAfter,
    },
    smallText: {
      lineHeight: smallText?.line_height ?? DEFAULT_CALIBRATION.smallText.lineHeight,
      linePitch: smallText?.line_pitch ?? DEFAULT_CALIBRATION.smallText.linePitch,
      gapBefore: smallText?.gap_before ?? DEFAULT_CALIBRATION.smallText.gapBefore,
      gapAfter: smallText?.gap_after ?? DEFAULT_CALIBRATION.smallText.gapAfter,
    },
    scripture: {
      lineHeight: scripture?.line_height ?? DEFAULT_CALIBRATION.scripture.lineHeight,
      interBlockGap: scripture?.inter_block_gap ?? DEFAULT_CALIBRATION.scripture.interBlockGap,
      gapAfter: scripture?.gap_after ?? DEFAULT_CALIBRATION.scripture.gapAfter,
    },
    thematicBreak: {
      totalHeight: thematicBreak?.total_height ?? DEFAULT_CALIBRATION.thematicBreak.totalHeight,
    },
    tables: {
      headerHeight: tables?.header_height ?? DEFAULT_CALIBRATION.tables.headerHeight,
      rowHeight: tables?.row_height ?? DEFAULT_CALIBRATION.tables.rowHeight,
      rowPitch: tables?.row_pitch ?? DEFAULT_CALIBRATION.tables.rowPitch,
      gapBefore: tables?.gap_before ?? DEFAULT_CALIBRATION.tables.gapBefore,
    },
    page: {
      height: page?.height ?? DEFAULT_CALIBRATION.page.height,
      headerCutoff: page?.header_cutoff ?? DEFAULT_CALIBRATION.page.headerCutoff,
      footerCutoff: page?.footer_cutoff ?? DEFAULT_CALIBRATION.page.footerCutoff,
      usableHeight: page?.usable_height ?? DEFAULT_CALIBRATION.page.usableHeight,
      contentStartChapter: page?.content_start_chapter ?? DEFAULT_CALIBRATION.page.contentStartChapter,
      contentStartSection: page?.content_start_section ?? DEFAULT_CALIBRATION.page.contentStartSection,
    },
    meta: {
      source,
      measuredDate: (raw._measured as string) ?? 'unknown',
      theme: (raw._source as string) ?? 'unknown',
    },
  };
}
