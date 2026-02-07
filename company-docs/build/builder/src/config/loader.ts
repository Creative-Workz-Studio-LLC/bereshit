/**
 * CWS Manual Builder — Configuration Loader
 *
 * Loads and validates build.config.yaml. All builder behavior
 * is defined in the config file, not in code.
 *
 * Specification: editorial-style-guide.adoc (CWS-GDE-001)
 *
 * "For which of you, intending to build a tower, sitteth not down first,
 *  and counteth the cost, whether he have sufficient to finish it?"
 *  — Luke 14:28
 */

import { readFileSync, existsSync } from 'fs';
import { resolve, dirname } from 'path';
import { fileURLToPath } from 'url';
import { parse as parseYaml } from 'yaml';
import type {
  BuildConfig,
  RuntimePaths,
  FormatConfig,
  ScopeDefinition,
  BrandColorsConfig,
} from './types.js';

// =============================================================================
// Setup
// =============================================================================

/** ES Module __dirname equivalent */
const __filename = fileURLToPath(import.meta.url);
const __dirname = dirname(__filename);

/** Default config filename */
const CONFIG_FILENAME = 'build.config.yaml';

// =============================================================================
// Config Discovery
// =============================================================================

/**
 * Find the config file by walking up from the builder directory.
 * Expects: builder/dist/config/ → builder/ → build/ → company-docs/
 */
function findConfigFile(): string {
  // From dist/config/, go up to company-docs/build/
  const buildDir = resolve(__dirname, '../../..');
  const configPath = resolve(buildDir, CONFIG_FILENAME);

  if (!existsSync(configPath)) {
    // Fallback: try from src/ location (dev mode)
    const devBuildDir = resolve(__dirname, '../..');
    const devConfigPath = resolve(devBuildDir, CONFIG_FILENAME);

    if (!existsSync(devConfigPath)) {
      throw new Error(
        `Configuration file not found: ${configPath}\n` +
        `The build system requires a ${CONFIG_FILENAME} file in the company-docs/build/ directory.`
      );
    }

    return devConfigPath;
  }

  return configPath;
}

// =============================================================================
// Config Validation
// =============================================================================

/**
 * Validate that required config sections are present.
 */
function validateConfig(config: BuildConfig): void {
  const required: Array<keyof BuildConfig> = ['document', 'attributes', 'formats', 'watch', 'display'];

  for (const section of required) {
    if (!(section in config)) {
      throw new Error(`Missing required config section: ${section}`);
    }
  }

  // Validate document section
  if (!config.document.master) {
    throw new Error('Config missing: document.master');
  }
  if (!config.document.output_name) {
    throw new Error('Config missing: document.output_name');
  }
  if (!config.document.output_dir) {
    throw new Error('Config missing: document.output_dir');
  }

  // Validate at least one format exists
  if (Object.keys(config.formats).length === 0) {
    throw new Error('Config must define at least one format');
  }
}

// =============================================================================
// Config Loading
// =============================================================================

/**
 * Load and parse the YAML configuration file.
 */
export function loadConfig(configPath?: string): BuildConfig {
  const resolvedPath = configPath || findConfigFile();
  const yamlContent = readFileSync(resolvedPath, 'utf-8');
  const config = parseYaml(yamlContent) as BuildConfig;

  validateConfig(config);
  return config;
}

/**
 * Compute runtime paths from configuration.
 * Resolves all relative paths to absolute.
 *
 * When config.paths is present (v3.0.0+), uses its values.
 * Otherwise falls back to convention-based defaults.
 */
export function computePaths(config: BuildConfig, configPath?: string): RuntimePaths {
  const configFile = configPath || findConfigFile();
  const sourceDir = dirname(configFile);

  // company-docs/ is one level up from build/ (where config lives)
  const projectDir = resolve(sourceDir, '..');

  // Use config.paths when available, fall back to conventions
  const bookDir = config.paths?.book_dir
    ? resolve(projectDir, config.paths.book_dir)
    : resolve(projectDir, 'book');

  return {
    configFile,
    sourceDir,
    outputDir: resolve(sourceDir, config.document.output_dir),
    masterDocument: resolve(sourceDir, config.document.master),
    bookDir,
  };
}

// =============================================================================
// Config Queries
// =============================================================================

/**
 * Get names of all enabled formats.
 */
export function getEnabledFormats(config: BuildConfig): string[] {
  return Object.entries(config.formats)
    .filter(([, format]) => format.enabled)
    .map(([name]) => name);
}

/**
 * Get names of all defined formats.
 */
export function getAllFormats(config: BuildConfig): string[] {
  return Object.keys(config.formats);
}

/**
 * Get a specific format's configuration.
 */
export function getFormatConfig(config: BuildConfig, format: string): FormatConfig | undefined {
  return config.formats[format];
}

/**
 * Get a display icon from config.
 */
export function getIcon(config: BuildConfig, key: string): string {
  return config.display.icons[key] || '';
}

// =============================================================================
// Scope Queries (§9)
// =============================================================================

/**
 * Get all scope keys (e.g., ["A", "C", "D"]).
 */
export function getScopeKeys(config: BuildConfig): string[] {
  return config.scopes ? Object.keys(config.scopes) : [];
}

/**
 * Get a specific scope definition by key.
 */
export function getScopeConfig(config: BuildConfig, key: string): ScopeDefinition | undefined {
  return config.scopes?.[key];
}

// =============================================================================
// Brand Queries (§10)
// =============================================================================

/**
 * Get brand color palette.
 * Returns undefined if brand config is absent.
 */
export function getBrandColors(config: BuildConfig): BrandColorsConfig | undefined {
  return config.brand?.colors;
}

// =============================================================================
// Scripture Queries (§7)
// =============================================================================

/**
 * Get the scripture base path for Bible verse validation.
 */
export function getScriptureBasePath(config: BuildConfig): string | undefined {
  return config.scripture?.base_path;
}

/**
 * Get the default Bible translation.
 */
export function getDefaultTranslation(config: BuildConfig): string {
  return config.scripture?.default_translation ?? 'KJV';
}
