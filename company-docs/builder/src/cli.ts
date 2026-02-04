#!/usr/bin/env node
/**
 * CWS Manual Builder - CLI Interface
 *
 * Command-line interface for building the Company Identity Manual.
 *
 * Usage:
 *   cws-build                    Build all formats
 *   cws-build --format html      Build HTML only
 *   cws-build --format pdf       Build PDF only
 *   cws-build --format epub      Build EPUB only
 *   cws-build --format all       Build all formats
 *   cws-build --watch            Watch mode (HTML only)
 *   cws-build --clean            Clean output directory
 *   cws-build --info             Show output file info
 */

import { Command } from 'commander';
import chalk from 'chalk';
import { defaultConfig, loadConfig, type OutputFormat } from './config.js';
import { createBuilder, formatFileSize, formatDuration, type BuildEvent } from './builder.js';
import { getSupportedFormats, checkToolInstalled, getInstallHint } from './formats.js';

// ASCII art banner
const banner = `
${chalk.blue('╔═══════════════════════════════════════════════════════════════════╗')}
${chalk.blue('║')}  ${chalk.bold.white('CWS Manual Builder')}                                              ${chalk.blue('║')}
${chalk.blue('║')}  ${chalk.gray('Creative Workz Studio - Company Identity Manual')}                 ${chalk.blue('║')}
${chalk.blue('╚═══════════════════════════════════════════════════════════════════╝')}
`;

// Format icons
const formatIcons: Record<OutputFormat, string> = {
  html: '🌐',
  pdf: '📄',
  epub: '📚',
  docbook: '📋',
};

/**
 * Event handler for build events
 */
function handleEvent(event: BuildEvent): void {
  switch (event.type) {
    case 'start':
      console.log(chalk.cyan('\n▶ Starting build...'));
      console.log(chalk.gray(`  Formats: ${event.formats.join(', ')}`));
      break;

    case 'format-start':
      process.stdout.write(
        chalk.white(`  ${formatIcons[event.format]} Building ${event.format.toUpperCase()}... `)
      );
      break;

    case 'format-complete':
      if (event.result.success) {
        console.log(
          chalk.green('✓') +
            chalk.gray(` (${formatDuration(event.result.duration)})`)
        );
      } else {
        console.log(chalk.red('✗'));
        console.log(chalk.red(`     Error: ${event.result.error}`));
      }
      break;

    case 'complete':
      const successful = event.results.filter((r) => r.success).length;
      const total = event.results.length;
      console.log();
      if (successful === total) {
        console.log(chalk.green.bold(`✓ Build complete: ${successful}/${total} formats`));
      } else if (successful > 0) {
        console.log(chalk.yellow.bold(`⚠ Build partial: ${successful}/${total} formats`));
      } else {
        console.log(chalk.red.bold(`✗ Build failed: 0/${total} formats`));
      }

      // Show output paths
      console.log(chalk.gray('\nOutput files:'));
      for (const result of event.results) {
        if (result.success) {
          console.log(chalk.gray(`  ${formatIcons[result.format]} ${result.outputPath}`));
        }
      }
      break;

    case 'watch-start':
      console.log(chalk.cyan('\n👁  Watching for changes...'));
      console.log(chalk.gray('   Press Ctrl+C to stop\n'));
      break;

    case 'file-change':
      console.log(chalk.yellow(`\n📝 File changed: ${event.path}`));
      break;

    case 'error':
      console.log(chalk.red(`\n✗ Error: ${event.error}`));
      break;
  }
}

/**
 * Check prerequisites for building
 */
async function checkPrerequisites(formats: OutputFormat[]): Promise<boolean> {
  console.log(chalk.cyan('\n🔍 Checking prerequisites...\n'));

  let allInstalled = true;

  for (const format of formats) {
    const installed = await checkToolInstalled(format);
    const icon = installed ? chalk.green('✓') : chalk.red('✗');
    const status = installed
      ? chalk.gray('installed')
      : chalk.red(`not found - ${getInstallHint(format)}`);
    console.log(`  ${icon} ${format.toUpperCase()}: ${status}`);

    if (!installed) {
      allInstalled = false;
    }
  }

  console.log();
  return allInstalled;
}

/**
 * Parse format argument
 */
function parseFormat(format: string): OutputFormat[] {
  if (format === 'all') {
    return getSupportedFormats();
  }

  const validFormats = getSupportedFormats();
  const requested = format.split(',').map((f) => f.trim().toLowerCase());

  for (const f of requested) {
    if (!validFormats.includes(f as OutputFormat)) {
      console.log(chalk.red(`Invalid format: ${f}`));
      console.log(chalk.gray(`Valid formats: ${validFormats.join(', ')}`));
      process.exit(1);
    }
  }

  return requested as OutputFormat[];
}

/**
 * Main CLI program
 */
async function main(): Promise<void> {
  const program = new Command();

  program
    .name('cws-build')
    .description('Build the CWS Company Identity Manual in various formats')
    .version('1.0.0')
    .option('-f, --format <format>', 'Output format (html, pdf, epub, docbook, all)', 'all')
    .option('-w, --watch', 'Watch for changes and rebuild (HTML only)')
    .option('-c, --clean', 'Clean output directory before building')
    .option('-i, --info', 'Show output file information')
    .option('-p, --parallel', 'Build formats in parallel')
    .option('--check', 'Check prerequisites without building')
    .option('-o, --output <dir>', 'Output directory')
    .option('-q, --quiet', 'Suppress banner and non-essential output');

  program.parse();

  const opts = program.opts();

  // Show banner unless quiet mode
  if (!opts.quiet) {
    console.log(banner);
  }

  // Load configuration with any overrides
  const config = loadConfig({
    outputDir: opts.output || defaultConfig.outputDir,
  });

  // Create builder
  const builder = createBuilder(config, handleEvent);

  // Handle --check
  if (opts.check) {
    const formats = parseFormat(opts.format);
    await checkPrerequisites(formats);
    return;
  }

  // Handle --info
  if (opts.info) {
    const files = await builder.getOutputInfo();
    if (files.length === 0) {
      console.log(chalk.yellow('No output files found.'));
      console.log(chalk.gray(`Output directory: ${config.outputDir}`));
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
    console.log(chalk.cyan('🧹 Cleaning output directory...'));
    await builder.clean();
    console.log(chalk.green('✓ Output directory cleaned'));

    // If only cleaning, exit
    if (!opts.format || opts.format === 'all') {
      return;
    }
  }

  // Parse formats
  const formats = parseFormat(opts.format);

  // Check prerequisites
  const prereqOk = await checkPrerequisites(formats);
  if (!prereqOk) {
    console.log(chalk.yellow('⚠ Some tools are missing. Build may fail for those formats.'));
  }

  // Handle --watch
  if (opts.watch) {
    // Initial build
    await builder.build(['html']);
    // Start watching
    builder.startWatch(['html']);

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
