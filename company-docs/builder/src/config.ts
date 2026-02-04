/**
 * CWS Manual Builder - Configuration Loader
 *
 * This module ONLY loads configuration from build.config.yaml.
 * All behavior is defined in the config file, not in code.
 *
 * Biblical Foundation:
 *   "For which of you, intending to build a tower, sitteth not down first,
 *    and counteth the cost, whether he have sufficient to finish it?"
 *   — Luke 14:28
 */

import { readFileSync, existsSync } from 'fs';
import { resolve, dirname } from 'path';
import { fileURLToPath } from 'url';
import { parse as parseYaml } from 'yaml';

// ES Module __dirname equivalent
const __filename = fileURLToPath(import.meta.url);
const __dirname = dirname(__filename);

// -----------------------------------------------------------------------------
// Type Definitions (derived from config structure)
// -----------------------------------------------------------------------------

/**
 * Format configuration from YAML
 */
export interface FormatConfig {
  enabled: boolean;
  command: string;
  backend: string;
  extension: string;
  install: string;
  options: Record<string, unknown>;
  args: string[];
}

/**
 * Watch configuration from YAML
 */
export interface WatchConfig {
  patterns: string[];
  ignore: string[];
  debounce_ms: number;
  formats: string[];
}

/**
 * Display configuration from YAML
 */
export interface DisplayConfig {
  banner: boolean;
  colors: boolean;
  icons: Record<string, string>;
}

/**
 * Document configuration from YAML
 */
export interface DocumentConfig {
  master: string;
  output_name: string;
  output_dir: string;
}

/**
 * Complete build configuration (mirrors YAML structure)
 */
export interface BuildConfig {
  document: DocumentConfig;
  attributes: Record<string, string>;
  formats: Record<string, FormatConfig>;
  watch: WatchConfig;
  display: DisplayConfig;
  version: string;
}

/**
 * Runtime paths (computed from config)
 */
export interface RuntimePaths {
  configFile: string;
  sourceDir: string;
  outputDir: string;
  masterDocument: string;
}

// -----------------------------------------------------------------------------
// Config Loading
// -----------------------------------------------------------------------------

/**
 * Default config file location (relative to company-docs/)
 */
const CONFIG_FILENAME = 'build.config.yaml';

/**
 * Find the config file by walking up from the builder directory
 */
function findConfigFile(): string {
  // Start from builder/src, go up to company-docs
  const companyDocsDir = resolve(__dirname, '../..');
  const configPath = resolve(companyDocsDir, CONFIG_FILENAME);

  if (!existsSync(configPath)) {
    throw new Error(
      `Configuration file not found: ${configPath}\n` +
      `The build system requires a ${CONFIG_FILENAME} file in the company-docs directory.`
    );
  }

  return configPath;
}

/**
 * Load and parse the YAML configuration file
 */
export function loadConfig(configPath?: string): BuildConfig {
  const resolvedPath = configPath || findConfigFile();

  const yamlContent = readFileSync(resolvedPath, 'utf-8');
  const config = parseYaml(yamlContent) as BuildConfig;

  // Validate required sections exist
  validateConfig(config);

  return config;
}

/**
 * Validate that required config sections are present
 */
function validateConfig(config: BuildConfig): void {
  const required = ['document', 'attributes', 'formats', 'watch', 'display'];

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

/**
 * Compute runtime paths from configuration
 */
export function computePaths(config: BuildConfig, configPath?: string): RuntimePaths {
  const configFile = configPath || findConfigFile();
  const sourceDir = dirname(configFile);

  return {
    configFile,
    sourceDir,
    outputDir: resolve(sourceDir, config.document.output_dir),
    masterDocument: resolve(sourceDir, config.document.master),
  };
}

/**
 * Get enabled formats from config
 */
export function getEnabledFormats(config: BuildConfig): string[] {
  return Object.entries(config.formats)
    .filter(([_, format]) => format.enabled)
    .map(([name, _]) => name);
}

/**
 * Get all format names from config
 */
export function getAllFormats(config: BuildConfig): string[] {
  return Object.keys(config.formats);
}

/**
 * Get a specific format's configuration
 */
export function getFormatConfig(config: BuildConfig, format: string): FormatConfig | undefined {
  return config.formats[format];
}

/**
 * Get display icon from config
 */
export function getIcon(config: BuildConfig, key: string): string {
  return config.display.icons[key] || '';
}
