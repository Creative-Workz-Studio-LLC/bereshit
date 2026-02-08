/**
 * CWS Manual Builder — Validate Command
 *
 * Command: validate — structural book validation
 */

import chalk from 'chalk';
import type { Command } from 'commander';
import type { BuildConfig, RuntimePaths } from '../../config/types.js';
import { runValidation } from '../../validate/runner.js';
import type { BookType, BonusScope } from '../../validate/types.js';
import { displayFindings } from '../../display/formatters/validation.js';
import { exitWithError } from '../utils.js';

// =============================================================================
// Command Registration
// =============================================================================

export function registerValidateCommand(
  program: Command,
  config: BuildConfig,
  paths: RuntimePaths,
): void {
  program
    .command('validate')
    .description('Validate book structure')
    .option('-t, --type <type>', 'Book type (manual, manual-l)')
    .option('-b, --bonus <scope>', 'Bonus validation (operational)')
    .option('--report', 'Output JSON witness report instead of terminal display')
    .action(async (opts: { type?: string; bonus?: string; report?: boolean }) => {
      const start = Date.now();
      const result = await runValidation(paths, {
        type: opts.type as BookType | undefined,
        bonus: opts.bonus as BonusScope | undefined,
      }, config.typography, config.page_layout);
      const duration = Date.now() - start;

      if (opts.report) {
        // JSON witness report output
        const { WitnessCollector } = await import('../../witness/collector.js');
        const { serializeWitnessReport } = await import('../../witness/serializer.js');
        const collector = new WitnessCollector('validate', opts as Record<string, unknown>, ['structural']);
        collector.addStructural(result, duration);
        const report = collector.finalize(config.version ?? 'unknown', '3.0.0');
        process.stdout.write(serializeWitnessReport(report) + '\n');
      } else {
        // Terminal display
        console.log();
        console.log(chalk.cyan('Book Structure Validation'));
        console.log(chalk.cyan('========================='));

        displayFindings(result.findings);

        console.log(chalk.gray('\n--- Summary ---\n'));

        if (result.valid) {
          console.log(chalk.green(`  ${result.scopeLabel}: VALID (${result.counts.warn} warnings)`));
        } else {
          console.log(chalk.red(`  ${result.scopeLabel}: INVALID (${result.counts.fail} errors, ${result.counts.warn} warnings)`));
        }
        console.log();

        if (!result.valid) {
          exitWithError('E40', `Validation failed for ${result.scopeLabel}: ${result.counts.fail} errors, ${result.counts.warn} warnings`);
        }
      }
    });
}
