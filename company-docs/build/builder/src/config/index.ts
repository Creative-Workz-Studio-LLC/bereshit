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
  LogosConfig,
  FigureCatalogEntry,
  FiguresConfig,
  GrayscaleConfig,
  PaletteConfig,
  MockupConfig,
  DerivedAssetsConfig,
  ScriptureConfig,
  PathsConfig,
  ScopeDefinition,
  ScopesConfig,
  BrandColorsConfig,
  BrandConfig,
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
  getScopeKeys,
  getScopeConfig,
  getBrandColors,
  getScriptureBasePath,
  getDefaultTranslation,
} from './loader.js';
