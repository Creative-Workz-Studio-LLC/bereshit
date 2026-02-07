/**
 * CWS Manual Builder — Validation Module
 *
 * Re-exports all validation components.
 */

// Types
export type {
  BookType,
  BonusScope,
  ValidationSeverity,
  ValidationFinding,
  ValidationResult,
  ValidationOptions,
} from './types.js';

// Runner (primary entry point)
export { runValidation } from './runner.js';

// Individual validators (for direct use)
export { validateBase, type BaseValidationPaths } from './base.js';
export { validateManual } from './manual.js';
export { validateManualL } from './manual-l.js';
export { validateOperational } from './operational.js';

// Page & TOC validation
export { runPageParse, runPageStatus, runTocFill, runTocCheck } from './pages.js';
export type {
  PageMarker,
  TocSection,
  TocArticle,
  FilePageResult,
  PageParseResult,
  PageIssue,
  PageParseOptions,
  TocPipelineResult,
} from './pages.js';

// Editorial lint validation
export { runEditorialLint } from './editorial.js';
export type {
  EditorialSeverity,
  CheckCategory,
  EditorialFinding,
  EditorialFileResult,
  EditorialResult,
  EditorialLintOptions,
} from './editorial.js';
