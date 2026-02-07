/**
 * CWS Manual Builder — Build Command (Default Action)
 *
 * Default action when no subcommand given — multi-format publishing.
 * Handles: --format, --watch, --clean, --info, --check, --parallel, --quiet
 */

import chalk from 'chalk';
import type { Command } from 'commander';
import type { BuildConfig, RuntimePaths } from '../../config/types.js';
import { getBanner } from '../../display/banner.js';
import { createEventHandler } from '../../display/progress.js';
import { formatFileSize } from '../../display/format.js';
import { createBuilder } from '../../publish/orchestrator.js';
import { exitWithError, checkPrerequisites, parseFormat } from '../utils.js';

// =============================================================================
// Command Registration
// =============================================================================

export function registerBuildCommand(
  program: Command,
  config: BuildConfig,
  paths: RuntimePaths,
): void {
  // Program-level options for the default build action
  program
    .option('-f, --format <format>', 'Output format (or "all", "enabled")', 'enabled')
    .option('-w, --watch', 'Watch for changes and rebuild')
    .option('-c, --clean', 'Clean output directory before building')
    .option('-i, --info', 'Show output file information')
    .option('-p, --parallel', 'Build formats in parallel')
    .option('--check', 'Check prerequisites without building')
    .option('-q, --quiet', 'Suppress banner and non-essential output');

  // Default action (runs when no subcommand is given)
  program.action(async () => {
    const opts = program.opts<{
      format: string;
      watch?: boolean;
      clean?: boolean;
      info?: boolean;
      parallel?: boolean;
      check?: boolean;
      quiet?: boolean;
    }>();

    // Show banner unless quiet mode
    if (!opts.quiet && config.display.banner) {
      console.log(getBanner(config));
    }

    // Create event handler with config
    const handleEvent = createEventHandler(config);

    // Create builder with config and paths
    const builder = createBuilder(config, paths, handleEvent);

    // Handle --check
    if (opts.check) {
      const formats = parseFormat(opts.format, config);
      await checkPrerequisites(formats, config);
      return;
    }

    // Handle --info
    if (opts.info) {
      const files = await builder.getOutputInfo();
      if (files.length === 0) {
        console.log(chalk.yellow('No output files found.'));
        console.log(chalk.gray(`Output directory: ${paths.outputDir}`));
      } else {
        console.log(chalk.cyan('Output files:\n'));
        for (const file of files) {
          console.log(
            chalk.white(`  ${file.path}`) +
              chalk.gray(` (${formatFileSize(file.size)}, ${file.modified.toLocaleString()})`)
          );
        }
      }
      return;
    }

    // Handle --clean
    if (opts.clean) {
      const icons = config.display.icons;
      console.log(chalk.cyan(`${icons['clean'] || '🧹'} Cleaning output directory...`));
      await builder.clean();
      console.log(chalk.green(`${icons['success'] || '✓'} Output directory cleaned`));

      // If only cleaning (no specific format requested), exit
      if (opts.format === 'enabled') {
        return;
      }
    }

    // Parse formats from config
    const formats = parseFormat(opts.format, config);

    // Check prerequisites
    const prereqOk = await checkPrerequisites(formats, config);
    if (!prereqOk) {
      console.log(chalk.yellow('⚠ Some tools are missing. Build may fail for those formats.'));
    }

    // Handle --watch (uses watch config)
    if (opts.watch) {
      // Initial build with watch formats from config
      const watchFormats = config.watch.formats;
      await builder.build(watchFormats);
      // Start watching
      builder.startWatch();

      // Handle exit
      process.on('SIGINT', () => {
        console.log(chalk.gray('\n\nStopping watcher...'));
        builder.stopWatch();
        process.exit(0);
      });

      // Keep process alive
      return new Promise(() => {});
    }

    // Normal build
    const results = opts.parallel
      ? await builder.buildParallel(formats)
      : await builder.build(formats);

    // Exit with error if any builds failed
    const failures = results.filter((r) => !r.success);
    if (failures.length > 0) {
      const failedFormats = failures.map(r => r.format).join(', ');
      exitWithError('E30', `Build failed for format(s): ${failedFormats}`);
    }
  });
}
