/**
 * CWS Manual Builder — Display Module
 *
 * Re-exports all display components.
 */

// Types
export type { EventHandler, EventHandlerFactory } from './types.js';

// Banner
export { getBanner } from './banner.js';

// Progress
export { createEventHandler } from './progress.js';

// Formatting utilities
export { formatFileSize, formatDuration, pluralize } from './format.js';
