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
export { validateBase, type BaseValidationPaths } from './structural/index.js';
export { validateManual } from './structural/index.js';
export { validateManualL } from './structural/index.js';
export { validateOperational } from './structural/index.js';
export {
  validateThemeFontSizes,
  validateThemeFontFamilies,
  validateThemeAlignment,
  validateLegalTypography,
  validateThemeColors,
  validateThemeRoles,
  validateThemeStyles,
  validatePageLayout,
} from './theme/index.js';

// Page & TOC validation
export { runPageParse, runPageStatus, runTocFill, runTocCheck } from './toc/index.js';
export type {
  PageMarker,
  TocSection,
  TocArticle,
  FilePageResult,
  PageParseResult,
  PageIssue,
  PageParseOptions,
  TocPipelineResult,
} from './toc/index.js';

// Editorial lint validation
export { runEditorialLint } from './editorial/index.js';
export type {
  EditorialSeverity,
  CheckCategory,
  EditorialFinding,
  EditorialFileResult,
  EditorialResult,
  EditorialLintOptions,
} from './editorial/index.js';
