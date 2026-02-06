/**
 * CWS Manual Builder — Asset Generation Types
 *
 * Type definitions for derived asset generation:
 *   - Mermaid diagram rendering (source → PNG + SVG)
 *   - Grayscale figure variants (color PNG → grayscale PNG)
 *   - Brand color palette swatch (theme colors → labeled PNG)
 *
 * Specification: editorial-style-guide.adoc (CWS-GDE-001)
 *
 * "Every good gift and every perfect gift is from above."
 *   — James 1:17
 */

// =============================================================================
// Brand Colors
// =============================================================================

/**
 * CWS brand color entry.
 * Canonical source: Brand Color Palette in Asset Catalog (README.adoc).
 */
export interface BrandColor {
  /** Display name (e.g., "Deep Blue") */
  name: string;

  /** Hex code (e.g., "#1A365D") */
  hex: string;

  /** Functional role (e.g., "Primary") */
  role: string;
}

/**
 * CWS brand color palette — the five canonical colors.
 * These are constants, not configuration. They change only when the
 * brand identity changes (which is a board-level decision).
 */
export const BRAND_COLORS: readonly BrandColor[] = [
  { name: 'Deep Blue',   hex: '#1A365D', role: 'Primary' },
  { name: 'Medium Blue', hex: '#2C5282', role: 'Secondary' },
  { name: 'Gold',        hex: '#D69E2E', role: 'Accent' },
  { name: 'Dark',        hex: '#2D3748', role: 'Borders' },
  { name: 'Light',       hex: '#F7FAFC', role: 'Backgrounds' },
] as const;

// =============================================================================
// Generation Results
// =============================================================================

/** Result of generating a single asset. */
export interface AssetResult {
  /** Output file path (relative to assets directory) */
  path: string;

  /** What happened */
  status: 'generated' | 'up-to-date' | 'failed' | 'skipped';

  /** Error message if failed */
  error?: string;
}

/** Summary of a generation phase. */
export interface PhaseSummary {
  /** Phase name (e.g., "Mermaid diagrams") */
  name: string;

  /** Individual asset results */
  results: AssetResult[];
}

/** Complete generation report. */
export interface GenerationReport {
  /** Results per phase */
  phases: PhaseSummary[];

  /** Aggregate counts */
  totals: {
    generated: number;
    upToDate: number;
    failed: number;
    skipped: number;
  };
}

// =============================================================================
// Generation Options
// =============================================================================

/** Options for asset generation. */
export interface GenerateOptions {
  /** Force regeneration (ignore cache/timestamps) */
  force?: boolean;

  /** Absolute path to book/00-assets/ directory */
  assetsDir: string;

  /** PNG scale factor for Mermaid rendering (default: 3) */
  pngScale?: number;
}
