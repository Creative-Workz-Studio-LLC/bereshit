/**
 * CWS Manual Builder
 *
 * Build system for the Creative Workz Studio Company Identity Manual.
 * Compiles AsciiDoc source to HTML, PDF, EPUB, and DocBook formats.
 *
 * @packageDocumentation
 */

// Configuration
export {
  type BuildConfig,
  type OutputFormat,
  type FormatOptions,
  type HtmlOptions,
  type PdfOptions,
  type EpubOptions,
  type DocbookOptions,
  defaultConfig,
  loadConfig,
} from './config.js';

// Format handlers
export {
  type BuildResult,
  buildFormat,
  buildFormats,
  checkToolInstalled,
  getInstallHint,
  getSupportedFormats,
} from './formats.js';

// Builder
export {
  ManualBuilder,
  createBuilder,
  formatFileSize,
  formatDuration,
  type BuildEvent,
  type BuildEventCallback,
} from './builder.js';
