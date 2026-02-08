/**
 * Render Prediction Types — Data structures for source-to-PDF fill prediction.
 *
 * Maps AsciiDoc source elements to measured render heights, enabling
 * page fill prediction without building a PDF.
 *
 * Calibrated from: build/_calibration/calibration-ratios.json
 *
 * "A just weight and balance are the LORD's:
 *  all the weights of the bag are his work." — Proverbs 16:11
 */

// =============================================================================
// Element Types
// =============================================================================

/** AsciiDoc element types recognized by the source parser */
export type AsciiDocElementType =
  | 'heading-2'           // == (forces new page in doctype:book)
  | 'heading-3'           // ===
  | 'heading-4'           // ====
  | 'discrete-heading'    // [discrete] === (no section, no page break)
  | 'body-paragraph'      // Regular paragraph text
  | 'ordered-list'        // `. ` items (the list as a whole)
  | 'unordered-list'      // `* ` items (the list as a whole)
  | 'labeled-provision'   // `*(N)*` inline-numbered paragraph
  | 'small-text'          // [.small] block
  | 'scripture-block'     // [.text-center][.small] citation
  | 'thematic-break'      // '''
  | 'page-break'          // <<<
  | 'table'               // |=== ... |===
  | 'image'               // image:: directive
  | 'blank-line'          // Inter-element spacing
  | 'comment'             // // comment lines (no render height)
  | 'attribute'           // :attr: value (no render height)
  | 'page-marker';        // // PAGE N or // end p.N (no render height)

// =============================================================================
// Render Ratios
// =============================================================================

/** Height measurements for a heading level */
export interface HeadingRatio {
  /** Rendered height of the heading text itself (points) */
  height: number;
  /** Spacing above the heading from previous content (points) */
  gapBefore: number;
  /** Spacing below the heading to first body content (points) */
  gapAfter: number;
}

/** Height measurements for list elements */
export interface ListRatio {
  /** Height of a single list item line (points) */
  itemHeight: number;
  /** Y-distance between consecutive item starts (points) */
  itemPitch: number;
  /** Gap from preceding element to first item (points) */
  firstItemGap: number;
  /** Gap after last item to next element (points) */
  spacingAfter: number;
}

/** Height measurements for table elements */
export interface TableRatio {
  /** Header row height (points) */
  headerHeight: number;
  /** Body row height (points) */
  rowHeight: number;
  /** Y-distance between consecutive row starts (points) */
  rowPitch: number;
  /** Gap from preceding element to table start (points) */
  gapBefore: number;
}

/** Complete calibration data — measured render heights for all element types */
export interface RenderCalibration {
  /** Body text metrics */
  body: {
    /** Height of a single text line (points) */
    lineHeight: number;
    /** Y-distance between consecutive lines in same paragraph (points) */
    linePitch: number;
    /** Gap between paragraphs (points) */
    paragraphSpacing: number;
  };

  /** Heading metrics by level */
  headings: {
    h2: HeadingRatio;
    h3: HeadingRatio;
    h4: HeadingRatio;
    discrete: HeadingRatio;
  };

  /** List metrics (ordered and unordered are identical) */
  lists: ListRatio;

  /** Small text metrics ([.small] blocks) */
  smallText: {
    lineHeight: number;
    linePitch: number;
    gapBefore: number;
    gapAfter: number;
  };

  /** Scripture block metrics ([.text-center][.small]) */
  scripture: {
    lineHeight: number;
    interBlockGap: number;
    gapAfter: number;
  };

  /** Thematic break (''') total vertical consumption */
  thematicBreak: {
    totalHeight: number;
  };

  /** Table metrics */
  tables: TableRatio;

  /** Page structure */
  page: {
    height: number;
    headerCutoff: number;
    footerCutoff: number;
    usableHeight: number;
    contentStartChapter: number;
    contentStartSection: number;
  };

  /** Calibration metadata */
  meta: {
    source: string;
    measuredDate: string;
    theme: string;
  };
}

// =============================================================================
// Source Elements
// =============================================================================

/** A parsed element from an AsciiDoc source file */
export interface SourceElement {
  /** Element type */
  type: AsciiDocElementType;
  /** 1-based line number in source file */
  line: number;
  /** Number of source lines this element spans */
  lineCount: number;
  /** Raw source text */
  raw: string;
  /** Detected attributes/roles */
  attrs: {
    /** Heading level (2, 3, 4) */
    level?: number;
    /** Has [discrete] role */
    discrete?: boolean;
    /** Has [.text-center] role */
    centered?: boolean;
    /** Has [.small] role */
    small?: boolean;
    /** Number of list items (for list elements) */
    itemCount?: number;
    /** Number of rendered lines (estimated from text width) */
    renderedLines?: number;
    /** Number of table rows (for table elements) */
    rowCount?: number;
    /** Number of table columns */
    colCount?: number;
    /** Image pdfwidth (for image elements) */
    pdfwidth?: string;
    /** Page number from marker */
    pageNumber?: number;
  };
}

// =============================================================================
// Prediction Output
// =============================================================================

/** A predicted element with its height contribution */
export interface PredictedElement {
  /** Source element reference */
  source: SourceElement;
  /** Predicted height including spacing (points) */
  predictedHeight: number;
  /** Cumulative Y position on the predicted page */
  yPosition: number;
}

/** Predicted fill for a single page */
export interface PredictedPage {
  /** 1-based page number */
  page: number;
  /** Elements predicted on this page */
  elements: PredictedElement[];
  /** Total predicted content height (points) */
  predictedContentHeight: number;
  /** Predicted fill ratio (0.0-1.0+) */
  predictedFill: number;
  /** Estimated lines remaining */
  predictedLinesRemaining: number;
  /** Predicted bleed risk */
  predictedRisk: 'safe' | 'tight' | 'bleed-risk';
  /** Whether this page was forced by a page break */
  forcedBreak: boolean;
  /** Declared page number from source marker (if any) */
  declaredPage?: number;
}

/** Complete fill prediction for a source file */
export interface FillPrediction {
  /** Source file analyzed */
  file: string;
  /** Predicted pages */
  pages: PredictedPage[];
  /** Total predicted page count */
  totalPages: number;
  /** Calibration used */
  calibration: RenderCalibration;
  /** Accuracy metrics (populated when compared to actual) */
  accuracy?: PredictionAccuracy;
}

/** Accuracy metrics from comparing prediction to actual PDF */
export interface PredictionAccuracy {
  /** Mean absolute error in fill percentage */
  meanAbsoluteError: number;
  /** Maximum error on any single page */
  maxError: number;
  /** Pages within 5% accuracy */
  pagesWithin5pct: number;
  /** Total pages compared */
  totalPages: number;
  /** Per-page deltas */
  deltas: Array<{
    page: number;
    predicted: number;
    actual: number;
    delta: number;
  }>;
}
