/**
 * CWS Manual Builder
 *
 * Config-driven build system for the CreativeWorkzStudio LLC
 * Company Identity Manual. Supports editorial review (spine, inventory,
 * validation) and multi-format publishing (PDF, HTML, EPUB, DocBook).
 *
 * All behavior is defined in build.config.yaml — this code just executes.
 *
 * Specification: editorial-style-guide.adoc (CWS-GDE-001)
 *
 * @packageDocumentation
 */

// =============================================================================
// Configuration
// =============================================================================

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
} from './config/index.js';

export {
  loadConfig,
  computePaths,
  getEnabledFormats,
  getAllFormats,
  getFormatConfig,
  getIcon,
} from './config/index.js';

// =============================================================================
// Spine (Book Structure Discovery)
// =============================================================================

export type {
  SpineScope,
  SpineTree,
  SpineScopeNode,
  SpinePart,
  SpineArticle,
  SpineSection,
  InventoryReport,
  InventorySummary,
  PartInventory,
} from './spine/index.js';

export {
  SCOPE_NAMES,
  SCOPE_DIRS,
  scanSpine,
  getPartIds,
  getPart,
  getScopeDir,
  scopeFromDir,
  resolveBookDir,
  buildInventory,
  formatInventoryTree,
} from './spine/index.js';

// =============================================================================
// Validation
// =============================================================================

export type {
  BookType,
  BonusScope,
  ValidationSeverity,
  ValidationFinding,
  ValidationResult,
  ValidationOptions,
} from './validate/index.js';

export {
  runValidation,
  validateBase,
  validateManual,
  validateManualL,
  validateOperational,
} from './validate/index.js';

export type { BaseValidationPaths } from './validate/index.js';

// =============================================================================
// Publishing
// =============================================================================

export type {
  BuildResult,
  BuildEvent,
  BuildEventCallback,
  OutputFileInfo,
} from './publish/index.js';

export {
  buildFormat,
  buildFormats,
  checkToolInstalled,
  getInstallHint,
  ManualBuilder,
  createBuilder,
  BuildWatcher,
} from './publish/index.js';

// =============================================================================
// Assets (Derived Asset Generation)
// =============================================================================

export type {
  BrandColor,
  AssetResult,
  PhaseSummary,
  GenerationReport,
  GenerateOptions,
} from './assets/index.js';

export {
  BRAND_COLORS,
  generateAssets,
} from './assets/index.js';

// =============================================================================
// Display
// =============================================================================

export type { EventHandler, EventHandlerFactory } from './display/index.js';

export {
  getBanner,
  createEventHandler,
  formatFileSize,
  formatDuration,
  pluralize,
} from './display/index.js';
