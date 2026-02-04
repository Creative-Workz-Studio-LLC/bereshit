#!/usr/bin/env node
/**
 * CWS Manual Builder - CLI Interface
 *
 * Command-line interface driven by build.config.yaml.
 * Display settings, icons, and available formats all come from config.
 *
 * Usage:
 *   cws-build                    Build enabled formats from config
 *   cws-build --format html      Build HTML only
 *   cws-build --format pdf       Build PDF only
 *   cws-build --format all       Build all defined formats
 *   cws-build --watch            Watch mode (formats from config)
 *   cws-build --clean            Clean output directory
 *   cws-build --info             Show output file info
 */

import { Command } from 'commander';
import chalk from 'chalk';
import {
  loadConfig,
  computePaths,
  getEnabledFormats,
  getAllFormats,
  getFormatConfig,
  getIcon,
  type BuildConfig,
  type RuntimePaths,
} from './config.js';
import { createBuilder, formatFileSize, formatDuration, type BuildEvent } from './builder.js';
import { checkToolInstalled, getInstallHint } from './formats.js';

// -----------------------------------------------------------------------------
// Display (Config-driven)
// -----------------------------------------------------------------------------

/**
 * Create banner from config
 */
function getBanner(config: BuildConfig): string {
  const companyName = config.attributes['company-name'] || 'CWS';
  const tagline = config.attributes['tagline'] || '';

  return `
${chalk.blue('╔═══════════════════════════════════════════════════════════════════╗')}
${chalk.blue('║')}  ${chalk.bold.white('CWS Manual Builder')}                                              ${chalk.blue('║')}
${chalk.blue('║')}  ${chalk.gray(tagline.substring(0, 55).padEnd(55))} ${chalk.blue('║')}
${chalk.blue('╚═══════════════════════════════════════════════════════════════════╝')}
`;
}

/**
 * Create event handler that uses config for display
 */
function createEventHandler(config: BuildConfig) {
  return function handleEvent(event: BuildEvent): void {
    const icons = config.display.icons;

    switch (event.type) {
      case 'start':
        console.log(chalk.cyan(`\n${icons.start || '▶'} Starting build...`));
        console.log(chalk.gray(`  Formats: ${event.formats.join(', ')}`));
        break;

      case 'format-start': {
        const icon = icons[event.format] || '📄';
        process.stdout.write(
          chalk.white(`  ${icon} Building ${event.format.toUpperCase()}... `)
        );
        break;
      }

      case 'format-complete':
        if (event.result.success) {
          console.log(
            chalk.green(icons.success || '✓') +
              chalk.gray(` (${formatDuration(event.result.duration)})`)
          );
        } else {
          console.log(chalk.red(icons.failure || '✗'));
          console.log(chalk.red(`     Error: ${event.result.error}`));
        }
        break;

      case 'complete': {
        const successful = event.results.filter((r) => r.success).length;
        const total = event.results.length;
        console.log();
        if (successful === total) {
          console.log(chalk.green.bold(`${icons.success || '✓'} Build complete: ${successful}/${total} formats`));
        } else if (successful > 0) {
          console.log(chalk.yellow.bold(`⚠ Build partial: ${successful}/${total} formats`));
        } else {
          console.log(chalk.red.bold(`${icons.failure || '✗'} Build failed: 0/${total} formats`));
        }

        // Show output paths
        console.log(chalk.gray('\nOutput files:'));
        for (const result of event.results) {
          if (result.success) {
            const icon = icons[result.format] || '📄';
            console.log(chalk.gray(`  ${icon} ${result.outputPath}`));
          }
        }
        break;
      }

      case 'watch-start':
        console.log(chalk.cyan(`\n${icons.watch || '👁'}  Watching for changes...`));
        console.log(chalk.gray('   Press Ctrl+C to stop\n'));
        break;

      case 'file-change':
        console.log(chalk.yellow(`\n${icons.file_change || '📝'} File changed: ${event.path}`));
        break;

      case 'error':
        console.log(chalk.red(`\n${icons.failure || '✗'} Error: ${event.error}`));
        break;
    }
  };
}

// -----------------------------------------------------------------------------
// Prerequisites Check (Config-driven)
// -----------------------------------------------------------------------------

/**
 * Check prerequisites for building
 * Format info comes from config
 */
async function checkPrerequisites(
  formats: string[],
  config: BuildConfig
): Promise<boolean> {
  const icons = config.display.icons;
  console.log(chalk.cyan(`\n${icons.check || '🔍'} Checking prerequisites...\n`));

  let allInstalled = true;

  for (const formatName of formats) {
    const formatConfig = getFormatConfig(config, formatName);
    if (!formatConfig) {
      console.log(chalk.red(`  ? ${formatName.toUpperCase()}: not defined in config`));
      allInstalled = false;
      continue;
    }

    const installed = await checkToolInstalled(formatConfig);
    const icon = installed ? chalk.green(icons.success || '✓') : chalk.red(icons.failure || '✗');
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

// -----------------------------------------------------------------------------
// Format Parsing (Config-driven)
// -----------------------------------------------------------------------------

/**
 * Parse format argument against config-defined formats
 */
function parseFormat(format: string, config: BuildConfig): string[] {
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
      console.log(chalk.red(`Invalid format: ${f}`));
      console.log(chalk.gray(`Valid formats: ${allFormats.join(', ')}`));
      console.log(chalk.gray(`(Formats are defined in build.config.yaml)`));
      process.exit(1);
    }
  }

  return requested;
}

// -----------------------------------------------------------------------------
// Main CLI
// -----------------------------------------------------------------------------

/**
 * Main CLI program
 */
async function main(): Promise<void> {
  // Load configuration first - everything depends on it
  let config: BuildConfig;
  let paths: RuntimePaths;

  try {
    config = loadConfig();
    paths = computePaths(config);
  } catch (error) {
    console.error(chalk.red('Configuration error:'), (error as Error).message);
    console.error(chalk.gray('\nEnsure build.config.yaml exists in the company-docs directory.'));
    process.exit(1);
  }

  const program = new Command();

  // Version comes from config
  program
    .name('cws-build')
    .description('Build the CWS Company Identity Manual in various formats')
    .version(config.version)
    .option('-f, --format <format>', 'Output format (or "all", "enabled")', 'enabled')
    .option('-w, --watch', 'Watch for changes and rebuild')
    .option('-c, --clean', 'Clean output directory before building')
    .option('-i, --info', 'Show output file information')
    .option('-p, --parallel', 'Build formats in parallel')
    .option('--check', 'Check prerequisites without building')
    .option('-q, --quiet', 'Suppress banner and non-essential output');

  program.parse();

  const opts = program.opts();

  // Show banner unless quiet mode (controlled by config.display.banner)
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
    console.log(chalk.cyan(`${icons.clean || '🧹'} Cleaning output directory...`));
    await builder.clean();
    console.log(chalk.green(`${icons.success || '✓'} Output directory cleaned`));

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
    process.exit(1);
  }
}

// Run CLI
main().catch((error) => {
  console.error(chalk.red('Fatal error:'), error);
  process.exit(1);
});
