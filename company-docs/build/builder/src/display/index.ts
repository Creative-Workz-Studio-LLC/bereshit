/**
 * CWS Manual Builder — Display Module
 *
 * Re-exports all display components: formatters, progress, banner,
 * and result display functions for CLI output.
 */

// Types
export type { EventHandler, EventHandlerFactory } from './types.js';

// Banner
export { getBanner } from './banner.js';

// Progress
export { createEventHandler } from './progress.js';

// Formatting utilities
export { formatFileSize, formatDuration, pluralize } from './format.js';

// Result display — validation
export { displayFindings } from './validation.js';

// Result display — editorial lint
export { displayEditorialResults } from './editorial.js';

// Result display — Bible validation
export { displayBibleFindings } from './bible.js';

// Result display — TOC & page structure
export { displayTocResults, displayTocFillResult, displayTocCheckResult } from './toc.js';

// Result display — spine tree
export { displaySpineTree } from './spine.js';

// Result display — asset generation
export { displayAssetReport } from './assets-report.js';
