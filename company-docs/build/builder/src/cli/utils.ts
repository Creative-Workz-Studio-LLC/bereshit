/**
 * CWS Manual Builder — CLI Utilities
 *
 * Shared utilities for the CLI layer: error exit, Make delegation,
 * prerequisite checking, and format parsing.
 */

import chalk from 'chalk';
import {
  createError,
  formatError,
  ERROR_CODES,
  type ErrorCode,
} from '../errors.js';
import type { BuildConfig } from '../config/types.js';
import {
  getFormatConfig,
  getEnabledFormats,
  getAllFormats,
} from '../config/loader.js';
import { checkToolInstalled, getInstallHint } from '../publish/executor.js';

// =============================================================================
// Structured Error Exit
// =============================================================================

/**
 * Exit with a structured error code and message.
 * Matches Makefile error code system (E0-E4x).
 */
export function exitWithError(code: ErrorCode, message: string, opts?: { file?: string }): never {
  const error = createError(code, message, { file: opts?.file });
  console.error(chalk.red(formatError(error)));
  process.exit(ERROR_CODES[code].exitCode);
}

// =============================================================================
// Makefile Delegation
// =============================================================================

/**
 * Delegate an editorial command to the Makefile (primary build system).
 * The Makefile handles PDF editorial workflow — caching, cascading,
 * status, and targeted operations. The TS builder calls through to it
 * for a unified CLI experience.
 */
export async function delegateToMake(target: string, sourceDir: string): Promise<void> {
  const { spawn: spawnProc } = await import('child_process');
  const { resolve: resolvePath } = await import('path');

  // company-docs/ is one level up from build/
  const companyDocsDir = resolvePath(sourceDir, '..');

  return new Promise<void>((resolvePromise) => {
    const proc = spawnProc('make', target.split(' '), {
      cwd: companyDocsDir,
      stdio: 'inherit',
      shell: true,
    });

    proc.on('close', (code) => {
      if (code !== 0) {
        process.exit(code ?? 1);
      }
      resolvePromise();
    });

    proc.on('error', (err) => {
      const error = createError('E21', `Failed to run make: ${err.message}`);
      console.error(chalk.red(formatError(error)));
      process.exit(ERROR_CODES['E21'].exitCode);
    });
  });
}

// =============================================================================
// Prerequisites Check
// =============================================================================

/**
 * Check prerequisites for building.
 * Format info comes from config.
 */
export async function checkPrerequisites(
  formats: string[],
  config: BuildConfig
): Promise<boolean> {
  const icons = config.display.icons;
  console.log(chalk.cyan(`\n${icons['check'] || '🔍'} Checking prerequisites...\n`));

  let allInstalled = true;

  for (const formatName of formats) {
    const formatConfig = getFormatConfig(config, formatName);
    if (!formatConfig) {
      console.log(chalk.red(`  ? ${formatName.toUpperCase()}: not defined in config`));
      allInstalled = false;
      continue;
    }

    const installed = await checkToolInstalled(formatConfig);
    const icon = installed ? chalk.green(icons['success'] || '✓') : chalk.red(icons['failure'] || '✗');
    const status = installed
      ? chalk.gray('installed')
      : chalk.red(`not found - ${getInstallHint(formatConfig)}`);
    console.log(`  ${icon} ${formatName.toUpperCase()}: ${status}`);

    if (!installed) {
      allInstalled = false;
    }
  }

  console.log();
  return allInstalled;
}

// =============================================================================
// Format Parsing
// =============================================================================

/**
 * Parse format argument against config-defined formats.
 */
export function parseFormat(format: string, config: BuildConfig): string[] {
  const allFormats = getAllFormats(config);

  if (format === 'all') {
    return allFormats;
  }

  if (format === 'enabled') {
    return getEnabledFormats(config);
  }

  const requested = format.split(',').map((f) => f.trim().toLowerCase());

  for (const f of requested) {
    if (!allFormats.includes(f)) {
      console.log(chalk.gray(`Valid formats: ${allFormats.join(', ')}`));
      console.log(chalk.gray('(Formats are defined in build.config.yaml)'));
      exitWithError('E11', `Invalid format: '${f}'`);
    }
  }

  return requested;
}
