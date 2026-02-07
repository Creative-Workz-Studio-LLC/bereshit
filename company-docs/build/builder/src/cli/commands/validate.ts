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
import { displayFindings } from '../../display/validation.js';
import { exitWithError } from '../utils.js';

// =============================================================================
// Command Registration
// =============================================================================

export function registerValidateCommand(
  program: Command,
  _config: BuildConfig,
  paths: RuntimePaths,
): void {
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
        exitWithError('E40', `Validation failed for ${result.scopeLabel}: ${result.counts.fail} errors, ${result.counts.warn} warnings`);
      }
    });
}
