/**
 * CWS Manual Builder — Configuration Types
 *
 * Type definitions for the build configuration system.
 * All interfaces mirror the structure of build.config.yaml.
 *
 * Specification: editorial-style-guide.adoc (CWS-GDE-001)
 *
 * "For which of you, intending to build a tower, sitteth not down first,
 *  and counteth the cost, whether he have sufficient to finish it?"
 *  — Luke 14:28
 */

// =============================================================================
// Format Configuration
// =============================================================================

/**
 * Single format configuration from build.config.yaml.
 * Each format defines how to invoke an Asciidoctor backend.
 */
export interface FormatConfig {
  /** Whether this format is built by default */
  enabled: boolean;

  /** CLI command to invoke (e.g., "asciidoctor-pdf") */
  command: string;

  /** Asciidoctor backend name (e.g., "pdf", "html5", "docbook5") */
  backend: string;

  /** File extension for output (e.g., ".pdf", ".html") */
  extension: string;

  /** Installation hint shown when tool is missing */
  install: string;

  /** Format-specific options (paper_size, compress, etc.) */
  options: Record<string, unknown>;

  /** Additional CLI arguments passed to the command */
  args: string[];
}

// =============================================================================
// Watch Configuration
// =============================================================================

/**
 * Watch mode configuration for the TS builder.
 * For editorial watch, use `make watch-pdf` instead.
 */
export interface WatchConfig {
  /** Glob patterns to watch for changes */
  patterns: string[];

  /** Glob patterns to ignore */
  ignore: string[];

  /** Debounce delay in milliseconds */
  debounce_ms: number;

  /** Formats to rebuild on change */
  formats: string[];
}

// =============================================================================
// Display Configuration
// =============================================================================

/**
 * CLI display settings.
 */
export interface DisplayConfig {
  /** Show startup banner */
  banner: boolean;

  /** Enable colored output */
  colors: boolean;

  /** Icon map (format names → emoji, status → emoji) */
  icons: Record<string, string>;
}

// =============================================================================
// Document Configuration
// =============================================================================

/**
 * Document identity and paths.
 */
export interface DocumentConfig {
  /** Master assembly file (relative to config directory) */
  master: string;

  /** Output filename stem (extensions added per format) */
  output_name: string;

  /** Output directory (relative to config directory) */
  output_dir: string;
}

// =============================================================================
// Asset Configuration
// =============================================================================

/**
 * Logo asset entry.
 */
export interface LogoEntry {
  /** Filename within the assets directory */
  filename: string;

  /** Purpose description */
  purpose: string;

  /** Specifications (dimensions, format, etc.) */
  specifications: string;

  /** Current status (present, missing, placeholder) */
  status: string;
}

/**
 * Figure catalog entry — a single diagram in the manual.
 */
export interface FigureCatalogEntry {
  /** Figure identifier (e.g., "fig-manual-structure") */
  id: string;

  /** Brief content description */
  content: string;

  /** Where this figure is used in the manual */
  used_in: string;
}

/**
 * Figures configuration — diagram rendering pipeline.
 *
 * Directory layout (post-reorganization):
 *   figures/png/        Color PNGs (Mermaid output + palette swatch)
 *   figures/svg/        SVG variants (Mermaid output)
 *   figures/grayscale/  Grayscale PNGs (derived from color PNGs)
 *   sources/diagrams/   Mermaid .mmd source files
 *   sources/config/     Rendering configs (puppeteer, mermaid theme)
 */
export interface FiguresConfig {
  /** PNG output directory (relative to assets directory) */
  png_directory: string;

  /** SVG output directory (relative to assets directory) */
  svg_directory: string;

  /** Grayscale output directory (relative to assets directory) */
  grayscale_directory: string;

  /** Mermaid source files directory (relative to assets directory) */
  sources: string;

  /** Rendering config directory (relative to assets directory) */
  config: string;

  /** Output format strategy: "dual" = SVG + PNG */
  format: string;

  /** PNG scale factor for print quality (e.g., 3 = 3x = 300+ DPI) */
  png_scale: number;

  /** Rendering tool command (e.g., "mmdc" for mermaid-cli) */
  renderer: string;

  /** Theme configuration file path (relative to assets directory) */
  theme: string;

  /** Complete catalog of all figures */
  catalog: FigureCatalogEntry[];
}

/**
 * Assets configuration — mirrors build.config.yaml assets section.
 */
export interface AssetsConfig {
  /** Assets directory name (relative to book/) */
  directory: string;

  /** Logo files */
  logos: LogoEntry[];

  /** Figures configuration and catalog */
  figures: FiguresConfig;
}

// =============================================================================
// Complete Build Configuration
// =============================================================================

/**
 * Complete build configuration — mirrors build.config.yaml structure.
 * The TypeScript builder loads this and executes what it defines.
 */
export interface BuildConfig {
  /** Document identity and paths */
  document: DocumentConfig;

  /** Asciidoctor attributes (passed as -a key=value) */
  attributes: Record<string, string>;

  /** Output format definitions */
  formats: Record<string, FormatConfig>;

  /** Watch mode configuration */
  watch: WatchConfig;

  /** CLI display settings */
  display: DisplayConfig;

  /** Asset requirements */
  assets?: AssetsConfig;

  /** Config version */
  version: string;
}

// =============================================================================
// Runtime Paths
// =============================================================================

/**
 * Computed paths for runtime use.
 * Resolved from config values + filesystem location.
 */
export interface RuntimePaths {
  /** Absolute path to the config file itself */
  configFile: string;

  /** Directory containing the config file (company-docs/build/) */
  sourceDir: string;

  /** Absolute path to the output directory */
  outputDir: string;

  /** Absolute path to the master document (book.adoc) */
  masterDocument: string;

  /** Absolute path to the book directory */
  bookDir: string;
}
