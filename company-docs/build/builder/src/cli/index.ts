/**
 * CWS Manual Builder — CLI Main
 *
 * Command-line interface driven by build.config.yaml.
 * Supports both editorial review (spine, inventory, validate)
 * and multi-format publishing (build, watch, clean).
 *
 * Specification: editorial-style-guide.adoc (CWS-GDE-001)
 *
 * Usage:
 *   cws-build                          Build enabled formats from config
 *   cws-build --format html            Build HTML only
 *   cws-build --format pdf             Build PDF only
 *   cws-build --format all             Build all defined formats
 *   cws-build --watch                  Watch mode (formats from config)
 *   cws-build --clean                  Clean output directory
 *   cws-build --info                   Show output file info
 *   cws-build inventory                Show book parts list
 *   cws-build spine                    Show book structure tree
 *   cws-build validate [options]       Validate book structure
 *
 * Editorial Review (delegates to Makefile — primary build system):
 *   cws-build editorial [--force]      Full editorial build (cascading)
 *   cws-build status                   Build dashboard (PDFs, sizes, stale)
 *   cws-build section <file>           Build single section PDF
 *   cws-build article <id>             Build article (cascades to sections)
 *   cws-build part <id>                Build part (cascades to articles)
 *   cws-build remove [options]         Safe targeted output removal
 *   cws-build pages <file> -p <range>  Extract page range from section PDF
 *   cws-build lint [options]            Editorial lint — 17 checks, 4 categories
 *   cws-build lint --editorial          Editorial checks only (1-8: shall, spacers, markers)
 *   cws-build lint --category style     Style anti-pattern checks (9-13)
 *   cws-build lint --category structure Structural integrity checks (14-15)
 *   cws-build lint --category conversion Conversion checklist (16-17)
 *   cws-build lint --bible              Bible verse validation (KJV/WEB)
 *   cws-build lint --toc               Page marker validation (sequence, banners)
 *   cws-build toc [file]               Parse and display TOC structure
 *   cws-build toc --fill               Fill p. __ placeholders with computed page numbers
 *   cws-build toc --fill --dry-run     Report fills without writing
 *   cws-build toc --verify             Check existing page numbers against computed
 *   cws-build config [options]          Inspect, validate, or regenerate config
 *   cws-build config --validate         Validate config structure
 *   cws-build config --make             Regenerate config.mk from YAML
 *   cws-build config --types            Generate TypeScript types from YAML
 *   cws-build config --show --json      Dump full config as JSON
 *
 * "Write the vision, and make it plain upon tables,
 *  that he may run that readeth it." — Habakkuk 2:2
 */

import { Command } from 'commander';
import chalk from 'chalk';

import {
  loadConfig,
  computePaths,
  type BuildConfig,
  type RuntimePaths,
} from '../config/index.js';

import {
  createError,
  formatError,
  ERROR_CODES,
} from '../errors.js';

// Command registrars
import { registerBuildCommand } from './commands/build.js';
import { registerSpineCommands } from './commands/spine.js';
import { registerValidateCommand } from './commands/validate.js';
import { registerEditorialCommands } from './commands/editorial.js';
import { registerLintCommand } from './commands/lint.js';
import { registerTocCommand } from './commands/toc.js';
import { registerAssetsCommand } from './commands/assets.js';
import { registerConfigCommand } from './commands/config.js';

// =============================================================================
// Main CLI
// =============================================================================

/**
 * Main CLI program.
 */
async function main(): Promise<void> {
  // Load configuration first — everything depends on it
  let config: BuildConfig;
  let paths: RuntimePaths;

  try {
    config = loadConfig();
    paths = computePaths(config);
  } catch (error) {
    console.error(chalk.gray('\nEnsure build.config.yaml exists in the company-docs/build/ directory.'));
    const err = createError('E20', `Configuration error: ${(error as Error).message}`, { file: 'build.config.yaml' });
    console.error(chalk.red(formatError(err)));
    process.exit(ERROR_CODES['E20'].exitCode);
  }

  const program = new Command();

  // Version comes from config
  program
    .name('cws-build')
    .description('Build the CWS Company Identity Manual — editorial review and multi-format publishing')
    .version(config.version);

  // Register all commands
  registerSpineCommands(program, config, paths);
  registerValidateCommand(program, config, paths);
  registerEditorialCommands(program, config, paths);
  registerLintCommand(program, config, paths);
  registerTocCommand(program, config, paths);
  registerAssetsCommand(program, config, paths);
  registerConfigCommand(program, config, paths);
  registerBuildCommand(program, config, paths);  // Default action — must be last

  // Parse and route
  await program.parseAsync();
}

// =============================================================================
// Run CLI
// =============================================================================

main().catch((error) => {
  const err = createError('E30', `Fatal error: ${(error as Error).message ?? error}`);
  console.error(chalk.red(formatError(err)));
  process.exit(ERROR_CODES['E30'].exitCode);
});
