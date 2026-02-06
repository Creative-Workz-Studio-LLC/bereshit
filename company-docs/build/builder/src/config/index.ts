/**
 * CWS Manual Builder — Configuration Module
 *
 * Re-exports all configuration types and functions.
 */

// Types
export type {
  BuildConfig,
  FormatConfig,
  WatchConfig,
  DisplayConfig,
  DocumentConfig,
  AssetsConfig,
  LogoEntry,
  FigureCatalogEntry,
  FiguresConfig,
  RuntimePaths,
} from './types.js';

// Loader functions
export {
  loadConfig,
  computePaths,
  getEnabledFormats,
  getAllFormats,
  getFormatConfig,
  getIcon,
} from './loader.js';
