/**
 * CWS Manual Builder — Display Types
 *
 * Type definitions for CLI output and progress display.
 *
 * "Let your speech be alway with grace, seasoned with salt."
 *  — Colossians 4:6
 */

import type { BuildConfig } from '../../config/types.js';
import type { BuildEvent } from '../../publish/types.js';

// =============================================================================
// Display Types
// =============================================================================

/**
 * Event handler function signature.
 */
export type EventHandler = (event: BuildEvent) => void;

/**
 * Factory function that creates an event handler from config.
 */
export type EventHandlerFactory = (config: BuildConfig) => EventHandler;
