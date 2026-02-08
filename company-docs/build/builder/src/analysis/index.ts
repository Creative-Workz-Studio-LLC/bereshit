/**
 * Analysis module — PDF content analysis, source parsing, and fill prediction.
 */

export {
  analyzePageFill,
  buildFillConfig,
  parsePageRange,
  type BleedRisk,
  type FillAnalysis,
  type FillConfig,
  type FillStatus,
  type PageFillResult,
} from './page-fill.js';

export {
  crossCheckFill,
  parseSourceMarkers,
  countPageMarkers,
  type SourceMarker,
  type CrossCheckFinding,
  type CrossCheckResult,
} from './cross-check.js';

export {
  parseSourceElements,
  renderableElements,
  parsePageIndex,
  type ParseMode,
  type PageIndexEntry,
} from './source-parser.js';

export {
  loadCalibration,
  getDefaultCalibration,
  findCalibrationFile,
} from './calibration.js';

export {
  predictFill,
  comparePredictionToActual,
} from './predictor.js';

export type {
  AsciiDocElementType,
  RenderCalibration,
  SourceElement,
  PredictedElement,
  PredictedPage,
  FillPrediction,
  PredictionAccuracy,
} from './render-types.js';
