/**
 * CWS Manual Builder — Bible Validation Display
 *
 * Formats Bible verse validation findings for terminal output.
 */

import chalk from 'chalk';
import type { BibleValidationResult } from '../validate/bible.js';

// =============================================================================
// Bible Validation Display
// =============================================================================

/**
 * Display Bible verse validation findings with colored output.
 */
export function displayBibleFindings(result: BibleValidationResult): void {
  console.log();
  console.log(chalk.cyan('Bible Verse Validation'));
  console.log(chalk.cyan('======================'));

  if (result.citations.length === 0) {
    console.log(chalk.gray('\n  No Scripture citations found.\n'));
    return;
  }

  for (const cite of result.citations) {
    const ref = `${cite.book} ${cite.chapter}:${cite.verse}${cite.verseEnd ? '-' + cite.verseEnd : ''}`;
    const trans = cite.translation || 'KJV';
    const label = chalk.gray(`[${trans}]`);

    if (cite.status === 'verified') {
      console.log(chalk.green(`  PASS  ${ref} ${label}`));
    } else if (cite.status === 'mismatch') {
      console.log(chalk.yellow(`  WARN  ${ref} ${label} — text differs from canonical`));
      if (cite.details) {
        for (const d of cite.details) {
          console.log(chalk.gray(`          ${d}`));
        }
      }
    } else if (cite.status === 'not-found') {
      console.log(chalk.red(`  FAIL  ${ref} ${label} — verse not found in scripture data`));
    } else if (cite.status === 'ref-only') {
      console.log(chalk.gray(`  INFO  ${ref} ${label} — reference only (no quoted text to verify)`));
    }
  }

  console.log(chalk.gray('\n--- Summary ---\n'));
  console.log(
    `  Citations: ${result.citations.length}  ` +
    `Verified: ${result.counts.verified}  ` +
    `Mismatch: ${result.counts.mismatch}  ` +
    `Not found: ${result.counts['not-found']}  ` +
    `Ref-only: ${result.counts['ref-only']}`
  );

  if (result.filesChecked > 0) {
    console.log(chalk.gray(`  Files checked: ${result.filesChecked}`));
  }
  console.log();
}
