/**
 * CWS Manual Builder — Editorial Lint Display
 *
 * Formats editorial lint results for terminal output.
 * Matches Makefile lint output format for drop-in replacement.
 */

import chalk from 'chalk';
import type { EditorialResult } from '../validate/editorial.js';

// =============================================================================
// Editorial Lint Display
// =============================================================================

/**
 * Display editorial lint results with colored output.
 */
export function displayEditorialResults(result: EditorialResult): void {
  console.log();
  console.log(chalk.cyan('Editorial Lint'));
  console.log(chalk.cyan('=============='));
  console.log(chalk.gray(`  Categories: ${result.categories.join(', ')}`));

  for (const fileResult of result.files) {
    console.log();
    console.log(chalk.white(`--- ${fileResult.file} ---`));

    if (fileResult.findings.length === 0) {
      console.log(chalk.green('  PASS  No issues found'));
      continue;
    }

    for (const finding of fileResult.findings) {
      const prefix = finding.severity === 'warn'
        ? chalk.yellow('  WARN')
        : chalk.gray('  INFO');
      const loc = finding.line > 0 ? `:${finding.line}` : '';
      const cat = chalk.gray(`[${finding.check}]`);
      console.log(`${prefix}  ${cat} ${finding.message}${loc ? chalk.gray(` (line ${finding.line})`) : ''}`);

      if (finding.source) {
        console.log(chalk.gray(`          ${finding.source.substring(0, 120)}`));
      }
    }
  }

  // Summary
  console.log(chalk.gray('\n--- Summary ---\n'));
  console.log(
    `  Files: ${result.filesChecked}  ` +
    `Warnings: ${result.totals.warn}  ` +
    `Info: ${result.totals.info}`
  );
  console.log();
}
