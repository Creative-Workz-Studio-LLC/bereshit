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

  /**
   * Pipeline strategy for multi-step conversions.
   * - undefined: Direct command execution (default)
   * - 'docbook-intermediate': AsciiDoc → DocBook XML → pandoc → target format
   *   Pandoc's DocBook reader is excellent; its AsciiDoc reader is not.
   */
  pipeline?: string;
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
 * Logos configuration — wraps logo entries with directory context.
 * Mirrors build.config.yaml assets.logos structure.
 */
export interface LogosConfig {
  /** Logos subdirectory within assets */
  directory: string;

  /** Individual logo entries */
  items: LogoEntry[];
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
 * Grayscale generation configuration.
 */
export interface GrayscaleConfig {
  /** Enable grayscale variant generation */
  enabled: boolean;

  /** Filenames to exclude from grayscale conversion */
  exclude: string[];
}

/**
 * Palette swatch generation configuration.
 */
export interface PaletteConfig {
  /** Enable palette swatch generation */
  enabled: boolean;

  /** Output path (relative to assets directory) */
  output: string;

  /** Swatch image width in pixels */
  width: number;

  /** Swatch image height in pixels */
  height: number;

  /** Bold font name for labels */
  font_bold: string;

  /** Regular font name for values */
  font_regular: string;
}

/**
 * Title page mockup generation configuration.
 */
export interface MockupConfig {
  /** Enable mockup generation */
  enabled: boolean;

  /** Output path (relative to assets directory) */
  output: string;
}

/**
 * Derived assets configuration — auto-generated from source assets.
 * Mirrors build.config.yaml assets.derived section.
 */
export interface DerivedAssetsConfig {
  /** Grayscale variant generation */
  grayscale: GrayscaleConfig;

  /** Brand color palette swatch */
  palette: PaletteConfig;

  /** Title page mockup */
  mockup: MockupConfig;
}

/**
 * Assets configuration — mirrors build.config.yaml assets section.
 */
export interface AssetsConfig {
  /** Assets directory name (relative to book/) */
  directory: string;

  /** Logo configuration (directory + entries) */
  logos: LogosConfig;

  /** Figures configuration and catalog */
  figures: FiguresConfig;

  /** Derived asset generation settings */
  derived?: DerivedAssetsConfig;
}

// =============================================================================
// Scripture Configuration (§7)
// =============================================================================

/**
 * Scripture validation configuration.
 * Used by `cws-build lint --bible` to validate cited verses against
 * canonical text from the Bereshit scripture data.
 *
 * "All scripture is given by inspiration of God..." — 2 Timothy 3:16
 */
export interface ScriptureConfig {
  /** Path to scripture data (relative to company-docs/) */
  base_path: string;

  /** Supported translation identifiers (e.g., ["KJV", "WEB"]) */
  translations: string[];

  /** Default translation when citation doesn't specify */
  default_translation: string;
}

// =============================================================================
// Paths Configuration (§8)
// =============================================================================

/**
 * Makefile project layout paths.
 * All paths relative to company-docs/ (Makefile working directory).
 * The config.mk generator translates these into Make variables.
 */
export interface PathsConfig {
  /** Book source directory */
  book_dir: string;

  /** Preview build directory */
  preview_dir: string;

  /** Shared attributes file path */
  shared_attributes: string;

  /** Theme file path */
  theme_file: string;

  /** Font directory (semicolon-separated for asciidoctor-pdf) */
  font_dir: string;
}

// =============================================================================
// Scopes Configuration (§9)
// =============================================================================

/**
 * Single scope definition — maps a book directory to an editorial preview build.
 */
export interface ScopeDefinition {
  /** Human-readable scope name */
  name: string;

  /** Book subdirectory for this scope */
  directory: string;

  /** Preview assembly file name */
  preview: string;

  /** Output PDF filename */
  output: string;
}

/**
 * All scope definitions, keyed by scope letter (A, B, C, D).
 */
export type ScopesConfig = Record<string, ScopeDefinition>;

// =============================================================================
// Brand Configuration (§10)
// =============================================================================

/**
 * Brand color definitions.
 * Hex values with # prefix.
 */
export interface BrandColorsConfig {
  /** Deep blue — primary brand color */
  deep_blue: string;

  /** Medium blue — secondary brand color */
  medium_blue: string;

  /** Gold — accent color */
  gold: string;

  /** Dark — text and dark backgrounds */
  dark: string;

  /** Light — light backgrounds */
  light: string;
}

/**
 * Brand configuration — visual identity parameters.
 */
export interface BrandConfig {
  /** Brand color palette */
  colors: BrandColorsConfig;
}

// =============================================================================
// Complete Build Configuration
// =============================================================================

/**
 * Complete build configuration — mirrors build.config.yaml structure (v3.0.0).
 * The TypeScript builder loads this and executes what it defines.
 *
 * Sections 1-6:  Core builder config (document, attributes, formats, watch, display, assets)
 * Section 7:     Scripture validation
 * Section 8:     Makefile paths
 * Section 9:     Scope definitions
 * Section 10:    Brand colors
 */
export interface BuildConfig {
  /** §1 — Document identity and paths */
  document: DocumentConfig;

  /** §2 — Asciidoctor attributes (passed as -a key=value) */
  attributes: Record<string, string>;

  /** §3 — Output format definitions */
  formats: Record<string, FormatConfig>;

  /** §4 — Watch mode configuration */
  watch: WatchConfig;

  /** §5 — CLI display settings */
  display: DisplayConfig;

  /** §6 — Asset requirements and catalog */
  assets?: AssetsConfig;

  /** §7 — Scripture validation (Bible verse checking) */
  scripture?: ScriptureConfig;

  /** §8 — Makefile project layout paths */
  paths?: PathsConfig;

  /** §9 — Flat scope definitions (A, C, D) */
  scopes?: ScopesConfig;

  /** §10 — Brand visual identity */
  brand?: BrandConfig;

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
