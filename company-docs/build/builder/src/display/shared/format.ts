/**
 * CWS Manual Builder — Display Formatting Utilities
 *
 * Pure formatting functions for file sizes, durations, and counts.
 * No side effects — just string transformations.
 */

// =============================================================================
// Size Formatting
// =============================================================================

/**
 * Format byte count for human display.
 */
export function formatFileSize(bytes: number): string {
  if (bytes < 1024) return `${bytes} B`;
  if (bytes < 1024 * 1024) return `${(bytes / 1024).toFixed(1)} KB`;
  return `${(bytes / (1024 * 1024)).toFixed(1)} MB`;
}

// =============================================================================
// Duration Formatting
// =============================================================================

/**
 * Format millisecond duration for human display.
 */
export function formatDuration(ms: number): string {
  if (ms < 1000) return `${ms}ms`;
  return `${(ms / 1000).toFixed(1)}s`;
}

// =============================================================================
// Count Formatting
// =============================================================================

/**
 * Pluralize a noun based on count.
 */
export function pluralize(count: number, singular: string, plural?: string): string {
  const word = count === 1 ? singular : (plural || `${singular}s`);
  return `${count} ${word}`;
}
