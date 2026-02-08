/**
 * CWS Manual Builder — Display Formatters
 *
 * Re-exports all domain-specific display formatters.
 */

export { displayFindings } from './validation.js';
export { displayEditorialResults } from './editorial.js';
export type { EditorialDisplayOptions } from './editorial.js';
export { displayBibleFindings } from './bible.js';
export { displayTocResults, displayTocFillResult, displayTocCheckResult } from './toc.js';
export { displaySpineTree } from './spine.js';
export { displayAssetReport } from './assets-report.js';
export { displayWitnessReport } from './witness.js';
export type { WitnessDisplayOptions } from './witness.js';
