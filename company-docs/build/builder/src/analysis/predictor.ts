/**
 * Predictive Fill Calculator — Estimates page fill from AsciiDoc source.
 *
 * Data source: source file structure + calibration ratios.
 * Produces ESTIMATED fill — not measured. Use `page-fill.ts` for measured fill from PDF.
 *
 * Workflow position:
 *   Source → [parser] → elements → [predictor] → estimated fill per page
 *   PDF   → [page-fill] → measured fill per page
 *   Estimated + Measured → [comparePredictionToActual] → accuracy delta
 *
 * Walks a parsed element stream, applies calibrated render ratios,
 * and predicts how content will flow across pages — without building a PDF.
 *
 * "The heart of the prudent getteth knowledge;
 *  and the ear of the wise seeketh knowledge." — Proverbs 18:15
 */

import type {
  RenderCalibration,
  SourceElement,
  PredictedElement,
  PredictedPage,
  FillPrediction,
  PredictionAccuracy,
} from './render-types.js';
import type { BleedRisk } from './page-fill.js';
import type { FillConfig } from './page-fill.js';
import { renderableElements } from './source-parser.js';

// =============================================================================
// Height Calculators
// =============================================================================

/**
 * Calculate the predicted render height for a single element.
 * Returns total height including the element's own spacing contribution.
 */
function elementHeight(el: SourceElement, cal: RenderCalibration, isFirst: boolean): number {
  switch (el.type) {
    case 'heading-2':
      // == forces new page in doctype:book — handled at page level, not here
      return cal.headings.h2.height + cal.headings.h2.gapAfter;

    case 'heading-3':
      return (isFirst ? 0 : cal.headings.h3.gapBefore)
        + cal.headings.h3.height
        + cal.headings.h3.gapAfter;

    case 'heading-4':
      return (isFirst ? 0 : cal.headings.h4.gapBefore)
        + cal.headings.h4.height
        + cal.headings.h4.gapAfter;

    case 'discrete-heading':
      return (isFirst ? 0 : cal.headings.discrete.gapBefore)
        + cal.headings.discrete.height
        + cal.headings.discrete.gapAfter;

    case 'body-paragraph': {
      const lines = el.attrs.renderedLines ?? 1;
      const textHeight = cal.body.lineHeight + (lines - 1) * cal.body.linePitch;
      const spacing = isFirst ? 0 : cal.body.paragraphSpacing;
      return spacing + textHeight;
    }

    case 'labeled-provision': {
      const lines = el.attrs.renderedLines ?? 2;
      const textHeight = cal.body.lineHeight + (lines - 1) * cal.body.linePitch;
      const spacing = isFirst ? 0 : cal.body.paragraphSpacing;
      return spacing + textHeight;
    }

    case 'ordered-list':
    case 'unordered-list': {
      const items = el.attrs.itemCount ?? 1;
      const renderedLines = el.attrs.renderedLines ?? items;
      // Multi-line list model:
      //   - Each item: itemHeight (first line) + (extraLines) * bodyLinePitch
      //   - Between items: itemPitch - itemHeight (inter-item gap)
      //   - Total = items * itemHeight + (renderedLines - items) * bodyLinePitch
      //           + (items - 1) * (itemPitch - itemHeight)
      const perItemBase = items * cal.lists.itemHeight;
      const extraLineHeight = (renderedLines - items) * cal.body.linePitch;
      const interItemGap = (items - 1) * (cal.lists.itemPitch - cal.lists.itemHeight);
      const listHeight = perItemBase + extraLineHeight + interItemGap;
      const spacing = cal.lists.firstItemGap;
      return spacing + listHeight;
    }

    case 'small-text': {
      const lines = el.attrs.renderedLines ?? 1;
      const textHeight = cal.smallText.lineHeight + (lines - 1) * cal.smallText.linePitch;
      const spacing = isFirst ? 0 : cal.smallText.gapBefore;
      return spacing + textHeight;
    }

    case 'scripture-block': {
      const lines = el.attrs.renderedLines ?? 1;
      const textHeight = cal.scripture.lineHeight + (lines - 1) * cal.smallText.linePitch;
      const spacing = isFirst ? 0 : cal.scripture.interBlockGap;
      return spacing + textHeight;
    }

    case 'thematic-break':
      return cal.thematicBreak.totalHeight;

    case 'table': {
      const rows = el.attrs.rowCount ?? 1;
      const tableHeight = cal.tables.headerHeight
        + rows * cal.tables.rowPitch;
      const spacing = isFirst ? 0 : cal.tables.gapBefore;
      return spacing + tableHeight;
    }

    case 'image': {
      // Rough estimate: images at pdfwidth=2.5in ≈ 180pt height
      // This should be refined with actual image dimension measurement
      const pdfwidth = el.attrs.pdfwidth;
      let imageHeight = 180; // default
      if (pdfwidth) {
        const inMatch = pdfwidth.match(/([\d.]+)in/);
        if (inMatch?.[1]) {
          // Assume roughly square aspect ratio for logos
          imageHeight = parseFloat(inMatch[1]) * 72;
        }
      }
      const spacing = isFirst ? 0 : cal.body.paragraphSpacing;
      return spacing + imageHeight;
    }

    case 'page-break':
      return 0; // handled at page level

    default:
      return 0;
  }
}

// =============================================================================
// Risk Classification
// =============================================================================

/**
 * Classify bleed risk from fill ratio.
 */
function classifyRisk(fillRatio: number, config?: FillConfig): BleedRisk {
  const bleedThreshold = config?.bleedThreshold ?? 0.95;
  const tightThreshold = config?.tightThreshold ?? 0.92;

  if (fillRatio >= bleedThreshold) return 'bleed-risk';
  if (fillRatio >= tightThreshold) return 'tight';
  return 'safe';
}

// =============================================================================
// Core Predictor
// =============================================================================

/**
 * Predict page fill from parsed source elements.
 *
 * Walks the element stream, accumulates predicted heights per page,
 * and handles page breaks, chapter headings, and PAGE markers as page boundaries.
 *
 * The `// PAGE N` markers in the source are the authoritative page boundaries —
 * they tell us exactly where the author says each page starts. The predictor
 * uses these markers to align with actual PDF pages and fills in height-based
 * predictions between markers.
 */
export function predictFill(
  elements: SourceElement[],
  calibration: RenderCalibration,
  fillConfig?: FillConfig,
  sourceFile: string = 'unknown',
): FillPrediction {
  const usableHeight = calibration.page.usableHeight;

  // First pass: find PAGE markers and build a line→declaredPage map
  // This tells us which source lines belong to which declared page
  const pageMarkers: Array<{ line: number; pageNumber: number }> = [];
  for (const el of elements) {
    if (el.type === 'page-marker' && el.attrs.pageNumber !== undefined) {
      if (el.raw.includes('PAGE')) {
        pageMarkers.push({ line: el.line, pageNumber: el.attrs.pageNumber });
      }
    }
  }

  // If we have PAGE markers, use them as authoritative page boundaries.
  // Otherwise fall back to height-based prediction only.
  const hasMarkers = pageMarkers.length > 0;

  // Find the PDF content boundary — "// *** START OF PDF" marker tells us
  // where preamble ends and renderable content begins.
  // Falls back to first PAGE marker, or 0 if neither exists.
  let contentStartLine = 0;
  for (const el of elements) {
    if (el.type === 'comment' && el.raw.includes('START OF PDF')) {
      contentStartLine = el.line;
      break;
    }
  }
  if (contentStartLine === 0 && hasMarkers) {
    contentStartLine = pageMarkers[0]!.line;
  }

  // Filter renderable elements — skip preamble content before PDF boundary
  const allRenderable = renderableElements(elements);
  const renderable = contentStartLine > 0
    ? allRenderable.filter((el) => el.line >= contentStartLine)
    : allRenderable;
  const pages: PredictedPage[] = [];
  let currentPageElements: PredictedElement[] = [];
  let currentY = 0;
  let pageNum = 1;
  let isFirstOnPage = true;
  let forcedBreak = false;
  let currentDeclaredPage: number | undefined = undefined;
  let markerIndex = 0;

  // Assign declared page based on which PAGE marker region this element falls in
  function getDeclaredPage(sourceLine: number): number | undefined {
    if (!hasMarkers) return undefined;
    // Find the last PAGE marker at or before this source line
    let declared: number | undefined;
    for (const m of pageMarkers) {
      if (m.line <= sourceLine) declared = m.pageNumber;
      else break;
    }
    return declared;
  }

  function finalizePage(): void {
    const fillRatio = usableHeight > 0 ? currentY / usableHeight : 0;
    const linesRemaining = Math.floor((usableHeight - currentY) / calibration.body.linePitch);

    pages.push({
      page: currentDeclaredPage ?? pageNum,
      elements: currentPageElements,
      predictedContentHeight: currentY,
      predictedFill: fillRatio,
      predictedLinesRemaining: Math.max(0, linesRemaining),
      predictedRisk: classifyRisk(fillRatio, fillConfig),
      forcedBreak,
      declaredPage: currentDeclaredPage,
    });

    pageNum++;
    currentPageElements = [];
    currentY = 0;
    isFirstOnPage = true;
    forcedBreak = false;
  }

  for (const el of renderable) {
    // Check if we've crossed into a new declared page region
    if (hasMarkers) {
      const declared = getDeclaredPage(el.line);
      if (declared !== undefined && declared !== currentDeclaredPage) {
        // New page marker region — finalize current page if we have content
        if (currentPageElements.length > 0 || currentY > 0) {
          finalizePage();
        }
        currentDeclaredPage = declared;
        // Check if this was from a forced break (<<<) preceding the marker
        if (markerIndex < pageMarkers.length && pageMarkers[markerIndex]!.pageNumber === declared) {
          forcedBreak = markerIndex > 0; // first page isn't forced
          markerIndex++;
        }
      }
    }

    // Page breaks force a new page (only if not already handled by marker)
    if (el.type === 'page-break') {
      if (!hasMarkers && (currentPageElements.length > 0 || currentY > 0)) {
        finalizePage();
      }
      forcedBreak = true;
      continue;
    }

    // Chapter headings (==) force a new page in doctype:book
    if (el.type === 'heading-2' && !hasMarkers) {
      if (currentPageElements.length > 0 || currentY > 0) {
        finalizePage();
      }
      forcedBreak = true;
    }

    // Calculate element height
    const height = elementHeight(el, calibration, isFirstOnPage);

    // In marker mode, don't split pages by height — markers are authoritative
    // In no-marker mode, overflow triggers a new page
    if (!hasMarkers && currentY + height > usableHeight && !isFirstOnPage) {
      finalizePage();
      const newHeight = elementHeight(el, calibration, true);
      currentPageElements.push({
        source: el,
        predictedHeight: newHeight,
        yPosition: currentY,
      });
      currentY += newHeight;
      isFirstOnPage = false;
      continue;
    }

    currentPageElements.push({
      source: el,
      predictedHeight: height,
      yPosition: currentY,
    });
    currentY += height;
    isFirstOnPage = false;
  }

  // Finalize last page
  if (currentPageElements.length > 0 || currentY > 0) {
    finalizePage();
  }

  return {
    file: sourceFile,
    pages,
    totalPages: pages.length,
    calibration,
  };
}

// =============================================================================
// Accuracy Comparison
// =============================================================================

/**
 * Compare predicted fill against actual fill analysis.
 *
 * When the predictor uses PAGE markers, predicted pages already have
 * declared page numbers matching actual PDF pages — direct comparison.
 * Without markers, sequential page numbers are compared directly.
 */
export function comparePredictionToActual(
  prediction: FillPrediction,
  actualFills: Array<{ page: number; fillRatio: number }>,
): PredictionAccuracy {
  const deltas: PredictionAccuracy['deltas'] = [];

  for (const predicted of prediction.pages) {
    // Match by page number (declared page when markers present)
    const actual = actualFills.find((a) => a.page === predicted.page);

    if (actual) {
      const predPct = Math.round(predicted.predictedFill * 100);
      const actPct = Math.round(actual.fillRatio * 100);
      deltas.push({
        page: predicted.page,
        predicted: predPct,
        actual: actPct,
        delta: predPct - actPct,
      });
    }
  }

  const absDelta = deltas.map((d) => Math.abs(d.delta));
  const meanAbsoluteError = absDelta.length > 0
    ? absDelta.reduce((s, v) => s + v, 0) / absDelta.length
    : 0;
  const maxError = absDelta.length > 0 ? Math.max(...absDelta) : 0;
  const within5 = absDelta.filter((d) => d <= 5).length;

  return {
    meanAbsoluteError: Math.round(meanAbsoluteError * 10) / 10,
    maxError,
    pagesWithin5pct: within5,
    totalPages: deltas.length,
    deltas,
  };
}
