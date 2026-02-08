/**
 * CWS Manual Builder — Display Shared Utilities
 *
 * Re-exports shared display components: types, formatting, banner, progress.
 */

export type { EventHandler, EventHandlerFactory } from './types.js';
export { getBanner } from './banner.js';
export { createEventHandler } from './progress.js';
export { formatFileSize, formatDuration, pluralize } from './format.js';
