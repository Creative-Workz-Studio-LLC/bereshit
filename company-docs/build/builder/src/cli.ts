#!/usr/bin/env node
/**
 * CWS Manual Builder — CLI Interface
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
 *
 * "Write the vision, and make it plain upon tables,
 *  that he may run that readeth it." — Habakkuk 2:2
 */

import { Command } from 'commander';
import chalk from 'chalk';

// Config
import {
  loadConfig,
  computePaths,
  getEnabledFormats,
  getAllFormats,
  getFormatConfig,
  type BuildConfig,
  type RuntimePaths,
} from './config/index.js';

// Display
import { getBanner } from './display/banner.js';
import { createEventHandler } from './display/progress.js';
import { formatFileSize } from './display/format.js';

// Publish
import { createBuilder } from './publish/orchestrator.js';
import { checkToolInstalled, getInstallHint } from './publish/executor.js';

// Spine
import { scanSpine } from './spine/scanner.js';
import { buildInventory, formatInventoryTree } from './spine/inventory.js';
import { SCOPE_DIRS, SCOPE_NAMES, type SpineScope } from './spine/types.js';

// Validate
import { runValidation } from './validate/runner.js';
import type { BookType, BonusScope, ValidationFinding } from './validate/types.js';

// Assets
import { generateAssets } from './assets/generator.js';
import type { GenerationReport } from './assets/types.js';

// =============================================================================
// Prerequisites Check (Config-driven)
// =============================================================================

/**
 * Check prerequisites for building.
 * Format info comes from config.
 */
async function checkPrerequisites(
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
// Makefile Delegation (Editorial commands)
// =============================================================================

/**
 * Delegate an editorial command to the Makefile (primary build system).
 * The Makefile handles PDF editorial workflow — caching, cascading,
 * status, and targeted operations. The TS builder calls through to it
 * for a unified CLI experience.
 */
async function delegateToMake(target: string, sourceDir: string): Promise<void> {
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
      console.error(chalk.red(`Failed to run make: ${err.message}`));
      process.exit(1);
    });
  });
}

// =============================================================================
// Format Parsing (Config-driven)
// =============================================================================

/**
 * Parse format argument against config-defined formats.
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
      console.log(chalk.gray('(Formats are defined in build.config.yaml)'));
      process.exit(1);
    }
  }

  return requested;
}

// =============================================================================
// Validation Display
// =============================================================================

/**
 * Display validation findings with colored output.
 */
function displayFindings(findings: ValidationFinding[]): void {
  let currentLayer = '';

  for (const finding of findings) {
    // Section headers
    const layerLabel = finding.layer === 'base' ? 'Base (skeleton)'
      : finding.layer === 'type' ? `Type: ${finding.category}`
      : `Bonus: ${finding.category}`;

    if (layerLabel !== currentLayer) {
      currentLayer = layerLabel;
      console.log(chalk.gray(`\n--- ${currentLayer} ---\n`));
    }

    // Severity icon
    const prefix = finding.severity === 'pass' ? chalk.green('  PASS')
      : finding.severity === 'fail' ? chalk.red('  FAIL')
      : finding.severity === 'warn' ? chalk.yellow('  WARN')
      : chalk.gray('  INFO');

    console.log(`${prefix}  ${finding.message}`);

    // Details
    if (finding.details) {
      for (const detail of finding.details) {
        console.log(chalk.gray(`          ${detail}`));
      }
    }
  }
}

// =============================================================================
// Asset Generation Display
// =============================================================================

/**
 * Display asset generation results with colored output.
 */
function displayAssetReport(report: GenerationReport): void {
  for (const phase of report.phases) {
    console.log(chalk.gray(`\n--- ${phase.name} ---\n`));

    for (const result of phase.results) {
      const prefix = result.status === 'generated' ? chalk.green('  GEN ')
        : result.status === 'up-to-date' ? chalk.gray('  OK  ')
        : result.status === 'failed' ? chalk.red('  FAIL')
        : chalk.yellow('  SKIP');

      const suffix = result.status === 'up-to-date' ? chalk.gray(' (up to date)')
        : result.error ? chalk.red(` — ${result.error}`)
        : '';

      console.log(`${prefix}  ${result.path}${suffix}`);
    }
  }

  const t = report.totals;
  console.log(chalk.gray('\n--- Summary ---\n'));
  console.log(`  Generated: ${t.generated}  Up to date: ${t.upToDate}  Failed: ${t.failed}  Skipped: ${t.skipped}`);
  if (t.failed > 0) {
    console.log(chalk.yellow(`  Warning: ${t.failed} asset(s) failed to generate.`));
  }
}

// =============================================================================
// Spine Display
// =============================================================================

/**
 * Display spine tree with colored output.
 */
function displaySpineTree(bookDir: string, scopes: Record<SpineScope, { files: Array<{ filename: string }>; parts: Array<{ id: string; dirname: string; articles: Array<{ id: string; dirname: string; sections: Array<{ filename: string }> }> }> }>): void {
  console.log(chalk.cyan('\nBook Structure'));
  console.log(chalk.cyan('==============\n'));

  const scopeKeys: SpineScope[] = ['A', 'B', 'C', 'D'];

  for (const scope of scopeKeys) {
    const node = scopes[scope];
    const dirName = SCOPE_DIRS[scope];
    const scopeName = SCOPE_NAMES[scope];

    console.log(chalk.white.bold(`  ${dirName}/`) + chalk.gray(` (${scopeName})`));

    if (node.files.length > 0) {
      for (const file of node.files) {
        console.log(chalk.gray(`    ${file.filename}`));
      }
    }

    for (const part of node.parts) {
      console.log(chalk.yellow(`    ${part.dirname}/`));

      for (const article of part.articles) {
        const count = article.sections.length;
        const label = count === 0 ? chalk.red('EMPTY') : chalk.gray(`${count} sections`);
        console.log(chalk.blue(`      ${article.dirname}/`) + ` ${label}`);

        for (const section of article.sections) {
          console.log(chalk.gray(`        ${section.filename}`));
        }
      }
    }

    console.log();
  }

  console.log(chalk.gray(`  Book root: ${bookDir}`));
}

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
    console.error(chalk.red('Configuration error:'), (error as Error).message);
    console.error(chalk.gray('\nEnsure build.config.yaml exists in the company-docs/build/ directory.'));
    process.exit(1);
  }

  const program = new Command();

  // Version comes from config
  program
    .name('cws-build')
    .description('Build the CWS Company Identity Manual — editorial review and multi-format publishing')
    .version(config.version);

  // -------------------------------------------------------------------------
  // Default command: build
  // -------------------------------------------------------------------------

  program
    .option('-f, --format <format>', 'Output format (or "all", "enabled")', 'enabled')
    .option('-w, --watch', 'Watch for changes and rebuild')
    .option('-c, --clean', 'Clean output directory before building')
    .option('-i, --info', 'Show output file information')
    .option('-p, --parallel', 'Build formats in parallel')
    .option('--check', 'Check prerequisites without building')
    .option('-q, --quiet', 'Suppress banner and non-essential output');

  // -------------------------------------------------------------------------
  // Editorial commands
  // -------------------------------------------------------------------------

  // cws-build inventory
  program
    .command('inventory')
    .description('Show book parts list and file inventory')
    .action(async () => {
      const tree = await scanSpine(paths.bookDir);
      const report = buildInventory(tree);
      console.log();
      console.log(formatInventoryTree(report));
      console.log();
    });

  // cws-build spine
  program
    .command('spine')
    .description('Show book structure tree')
    .action(async () => {
      const tree = await scanSpine(paths.bookDir);
      displaySpineTree(tree.bookDir, tree.scopes);
    });

  // cws-build validate
  program
    .command('validate')
    .description('Validate book structure')
    .option('-t, --type <type>', 'Book type (manual, manual-l)')
    .option('-b, --bonus <scope>', 'Bonus validation (operational)')
    .action(async (opts: { type?: string; bonus?: string }) => {
      console.log();
      console.log(chalk.cyan('Book Structure Validation'));
      console.log(chalk.cyan('========================='));

      const result = await runValidation(paths, {
        type: opts.type as BookType | undefined,
        bonus: opts.bonus as BonusScope | undefined,
      });

      displayFindings(result.findings);

      console.log(chalk.gray('\n--- Summary ---\n'));

      if (result.valid) {
        console.log(chalk.green(`  ${result.scopeLabel}: VALID (${result.counts.warn} warnings)`));
      } else {
        console.log(chalk.red(`  ${result.scopeLabel}: INVALID (${result.counts.fail} errors, ${result.counts.warn} warnings)`));
      }
      console.log();

      if (!result.valid) {
        process.exit(4);
      }
    });

  // -------------------------------------------------------------------------
  // Editorial review commands (delegate to Makefile — primary build system)
  // -------------------------------------------------------------------------
  // The Makefile is the primary editorial build system for PDF review.
  // These commands provide a unified CLI interface by delegating to Make.

  // cws-build status
  program
    .command('status')
    .description('Show build dashboard (current/stale PDFs, sizes, timestamps)')
    .action(async () => {
      await delegateToMake('status', paths.sourceDir);
    });

  // cws-build editorial
  program
    .command('editorial')
    .description('Full editorial build: validate → scopes → articles → sections → master')
    .option('--force', 'Force rebuild (ignore cache)')
    .action(async (opts: { force?: boolean }) => {
      const args = opts.force ? 'book FORCE=1' : 'book';
      await delegateToMake(args, paths.sourceDir);
    });

  // cws-build remove
  program
    .command('remove')
    .description('Safe targeted removal of build outputs')
    .option('-s, --scope <scope>', 'Scope letter (A, B, C, D, assets, master)')
    .option('-p, --part <part>', 'Part ID or search term')
    .option('-a, --article <article>', 'Article ID or search term')
    .option('-f, --file <file>', 'Single file name or path')
    .action(async (opts: { scope?: string; part?: string; article?: string; file?: string }) => {
      const makeArgs: string[] = ['remove'];
      if (opts.scope) makeArgs.push(`SCOPE=${opts.scope}`);
      if (opts.part) makeArgs.push(`PART=${opts.part}`);
      if (opts.article) makeArgs.push(`ARTICLE=${opts.article}`);
      if (opts.file) makeArgs.push(`FILE=${opts.file}`);
      await delegateToMake(makeArgs.join(' '), paths.sourceDir);
    });

  // cws-build section
  program
    .command('section <file>')
    .description('Build a single section PDF (themed)')
    .option('--force', 'Force rebuild (ignore cache)')
    .action(async (file: string, opts: { force?: boolean }) => {
      const force = opts.force ? ' FORCE=1' : '';
      await delegateToMake(`section FILE=${file}${force}`, paths.sourceDir);
    });

  // cws-build article
  program
    .command('article <id>')
    .description('Build all sections in an article, then combined PDF')
    .option('--force', 'Force rebuild (ignore cache)')
    .action(async (id: string, opts: { force?: boolean }) => {
      const force = opts.force ? ' FORCE=1' : '';
      await delegateToMake(`article ARTICLE=${id}${force}`, paths.sourceDir);
    });

  // cws-build part
  program
    .command('part <id>')
    .description('Build a part (cascades to articles and sections)')
    .option('--force', 'Force rebuild (ignore cache)')
    .action(async (id: string, opts: { force?: boolean }) => {
      const force = opts.force ? ' FORCE=1' : '';
      await delegateToMake(`part PART=${id}${force}`, paths.sourceDir);
    });

  // -------------------------------------------------------------------------
  // Asset generation
  // -------------------------------------------------------------------------

  // cws-build assets
  program
    .command('assets')
    .description('Generate derived visual assets (diagrams, grayscale variants, palette swatch)')
    .option('--force', 'Force regeneration (ignore timestamps)')
    .action(async (opts: { force?: boolean }) => {
      console.log();
      console.log(chalk.cyan('Asset Generation'));
      console.log(chalk.cyan('================'));

      const assetsDir = `${paths.bookDir}/00-assets`;
      const pngScale = config.assets?.figures?.png_scale ?? 3;

      const report: GenerationReport = await generateAssets({
        assetsDir,
        force: opts.force,
        pngScale,
      });

      displayAssetReport(report);
      console.log();

      if (report.totals.failed > 0) {
        process.exit(3);
      }
    });

  // -------------------------------------------------------------------------
  // Default action: publish (runs when no subcommand is given)
  // -------------------------------------------------------------------------

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
      process.exit(1);
    }
  });

  // -------------------------------------------------------------------------
  // Parse and route
  // -------------------------------------------------------------------------

  await program.parseAsync();
}

// =============================================================================
// Run CLI
// =============================================================================

main().catch((error) => {
  console.error(chalk.red('Fatal error:'), error);
  process.exit(1);
});
