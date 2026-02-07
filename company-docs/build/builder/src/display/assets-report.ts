/**
 * CWS Manual Builder — Asset Generation Display
 *
 * Formats asset generation results for terminal output.
 */

import chalk from 'chalk';
import type { GenerationReport } from '../assets/types.js';

// =============================================================================
// Asset Generation Display
// =============================================================================

/**
 * Display asset generation results with colored output.
 */
export function displayAssetReport(report: GenerationReport): void {
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
