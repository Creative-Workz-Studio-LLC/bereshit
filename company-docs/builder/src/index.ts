/**
 * CWS Manual Builder
 *
 * Config-driven build system for the Creative Workz Studio Company Identity Manual.
 * All behavior is defined in build.config.yaml - this code just executes.
 *
 * @packageDocumentation
 */

// Configuration (loaded from build.config.yaml)
export {
  type BuildConfig,
  type FormatConfig,
  type WatchConfig,
  type DisplayConfig,
  type DocumentConfig,
  type RuntimePaths,
  loadConfig,
  computePaths,
  getEnabledFormats,
  getAllFormats,
  getFormatConfig,
  getIcon,
} from './config.js';

// Format execution
export {
  type BuildResult,
  buildFormat,
  buildFormats,
  checkToolInstalled,
  getInstallHint,
} from './formats.js';

// Builder orchestration
export {
  ManualBuilder,
  createBuilder,
  formatFileSize,
  formatDuration,
  type BuildEvent,
  type BuildEventCallback,
} from './builder.js';
