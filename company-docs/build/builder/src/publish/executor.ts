/**
 * CWS Manual Builder — Format Executor
 *
 * Executes format builds based on configuration.
 * Contains NO hardcoded format definitions — all behavior comes from config.
 *
 * The config defines what command to run, what arguments to pass,
 * and what the output extension is. This code just runs it.
 *
 * "Whatsoever thy hand findeth to do, do it with thy might."
 *  — Ecclesiastes 9:10
 */

import { spawn } from 'child_process';
import { existsSync } from 'fs';
import { mkdir } from 'fs/promises';
import { join } from 'path';
import type { BuildConfig, FormatConfig, RuntimePaths } from '../config/types.js';
import type { BuildResult } from './types.js';

// =============================================================================
// Command Execution (Pure utility)
// =============================================================================

/**
 * Execute a shell command and return a promise.
 */
function executeCommand(
  command: string,
  args: string[],
  cwd: string
): Promise<{ stdout: string; stderr: string; code: number }> {
  return new Promise((resolve) => {
    const proc = spawn(command, args, { cwd, shell: true });
    let stdout = '';
    let stderr = '';

    proc.stdout.on('data', (data: Buffer) => {
      stdout += data.toString();
    });

    proc.stderr.on('data', (data: Buffer) => {
      stderr += data.toString();
    });

    proc.on('close', (code) => {
      resolve({ stdout, stderr, code: code ?? 1 });
    });

    proc.on('error', (err) => {
      resolve({ stdout, stderr: err.message, code: 1 });
    });
  });
}

// =============================================================================
// Argument Building (Config-driven)
// =============================================================================

/**
 * Build attribute arguments from config.
 * Transforms config.attributes into -a key=value arguments.
 */
function buildAttributeArgs(attributes: Record<string, string>): string[] {
  const args: string[] = [];
  for (const [key, value] of Object.entries(attributes)) {
    if (value === '') {
      // Boolean attribute (just the key)
      args.push('-a', key);
    } else {
      args.push('-a', `${key}=${value}`);
    }
  }
  return args;
}

/**
 * Build command arguments from format config.
 * All logic is driven by config values.
 */
function buildCommandArgs(
  formatConfig: FormatConfig,
  attributes: Record<string, string>,
  outputPath: string,
  masterDocument: string
): string[] {
  const args: string[] = [];

  // Backend (if specified in config)
  if (formatConfig.backend) {
    args.push('-b', formatConfig.backend);
  }

  // Document type is always book for this manual
  args.push('-d', 'book');

  // Output path
  args.push('-o', outputPath);

  // Attributes from config
  args.push(...buildAttributeArgs(attributes));

  // Additional args from format config
  if (formatConfig.args && formatConfig.args.length > 0) {
    args.push(...formatConfig.args);
  }

  // Source document (always last)
  args.push(masterDocument);

  return args;
}

// =============================================================================
// Tool Checking (Config-driven)
// =============================================================================

/**
 * Check if a format's required tool is installed.
 */
export async function checkToolInstalled(
  formatConfig: FormatConfig
): Promise<boolean> {
  const checkCommand = `${formatConfig.command} --version`;
  const result = await executeCommand(checkCommand, [], process.cwd());
  return result.code === 0;
}

/**
 * Get installation hint from config.
 */
export function getInstallHint(formatConfig: FormatConfig): string {
  return formatConfig.install;
}

// =============================================================================
// Build Execution (Config-driven)
// =============================================================================

/**
 * Build a single format.
 * All behavior is driven by config — this function just executes.
 */
export async function buildFormat(
  formatName: string,
  config: BuildConfig,
  paths: RuntimePaths
): Promise<BuildResult> {
  const startTime = Date.now();

  // Get format config
  const formatConfig = config.formats[formatName];
  if (!formatConfig) {
    return {
      format: formatName,
      success: false,
      outputPath: '',
      duration: Date.now() - startTime,
      error: `Unknown format: ${formatName}. Check build.config.yaml`,
    };
  }

  // Compute output path from config
  const outputPath = join(
    paths.outputDir,
    `${config.document.output_name}${formatConfig.extension}`
  );

  // Ensure output directory exists
  if (!existsSync(paths.outputDir)) {
    await mkdir(paths.outputDir, { recursive: true });
  }

  // Check if tool is installed
  const isInstalled = await checkToolInstalled(formatConfig);
  if (!isInstalled) {
    return {
      format: formatName,
      success: false,
      outputPath,
      duration: Date.now() - startTime,
      error: `${formatConfig.command} not found. Install with: ${formatConfig.install}`,
    };
  }

  // Build command arguments from config
  const args = buildCommandArgs(
    formatConfig,
    config.attributes,
    outputPath,
    paths.masterDocument
  );

  // Execute the command
  const result = await executeCommand(
    formatConfig.command,
    args,
    paths.sourceDir
  );

  const duration = Date.now() - startTime;

  if (result.code !== 0) {
    return {
      format: formatName,
      success: false,
      outputPath,
      duration,
      error: result.stderr || 'Unknown error during build',
    };
  }

  // Verify output was created
  if (!existsSync(outputPath)) {
    return {
      format: formatName,
      success: false,
      outputPath,
      duration,
      error: 'Build completed but output file not found',
    };
  }

  return {
    format: formatName,
    success: true,
    outputPath,
    duration,
  };
}

/**
 * Build multiple formats in parallel.
 */
export async function buildFormats(
  formatNames: string[],
  config: BuildConfig,
  paths: RuntimePaths
): Promise<BuildResult[]> {
  const promises = formatNames.map((format) => buildFormat(format, config, paths));
  return Promise.all(promises);
}
