/**
 * CWS Manual Builder — Publish Types
 *
 * Type definitions for the multi-format publishing pipeline.
 * The publisher executes format builds based on configuration.
 *
 * "Whatsoever thy hand findeth to do, do it with thy might."
 *  — Ecclesiastes 9:10
 */

// =============================================================================
// Build Results
// =============================================================================

/**
 * Result of building a single format.
 */
export interface BuildResult {
  /** Format name (e.g., "pdf", "html") */
  format: string;

  /** Whether the build succeeded */
  success: boolean;

  /** Absolute path to the output file */
  outputPath: string;

  /** Build duration in milliseconds */
  duration: number;

  /** Error message (if build failed) */
  error?: string;
}

// =============================================================================
// Build Events
// =============================================================================

/**
 * Events emitted during the build process.
 * Used by the display module for progress reporting.
 */
export type BuildEvent =
  | { type: 'start'; formats: string[] }
  | { type: 'format-start'; format: string }
  | { type: 'format-complete'; result: BuildResult }
  | { type: 'complete'; results: BuildResult[] }
  | { type: 'error'; error: string }
  | { type: 'watch-start' }
  | { type: 'file-change'; path: string };

/**
 * Callback for build events.
 */
export type BuildEventCallback = (event: BuildEvent) => void;

// =============================================================================
// Output Info
// =============================================================================

/**
 * Information about an output file.
 */
export interface OutputFileInfo {
  /** Absolute path to the file */
  path: string;

  /** File size in bytes */
  size: number;

  /** Last modification time */
  modified: Date;
}
