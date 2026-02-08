/**
 * CWS Manual Builder — Display Module
 *
 * Re-exports all display components: shared utilities, formatters,
 * progress, banner, and result display functions for CLI output.
 */

// Shared — types, banner, progress, formatting
export type { EventHandler, EventHandlerFactory } from './shared/index.js';
export { getBanner } from './shared/index.js';
export { createEventHandler } from './shared/index.js';
export { formatFileSize, formatDuration, pluralize } from './shared/index.js';

// Formatters — domain-specific display
export { displayFindings } from './formatters/index.js';
export { displayEditorialResults } from './formatters/index.js';
export type { EditorialDisplayOptions } from './formatters/index.js';
export { displayBibleFindings } from './formatters/index.js';
export { displayTocResults, displayTocFillResult, displayTocCheckResult } from './formatters/index.js';
export { displaySpineTree } from './formatters/index.js';
export { displayAssetReport } from './formatters/index.js';
export { displayWitnessReport } from './formatters/index.js';
export type { WitnessDisplayOptions } from './formatters/index.js';
