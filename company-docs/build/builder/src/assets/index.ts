/**
 * CWS Manual Builder — Assets Module
 *
 * Re-exports all asset generation types and functions.
 */

// Types
export type {
  BrandColor,
  AssetResult,
  PhaseSummary,
  GenerationReport,
  GenerateOptions,
} from './types.js';

// Constants
export { BRAND_COLORS } from './types.js';

// Generator
export { generateAssets } from './generator.js';
