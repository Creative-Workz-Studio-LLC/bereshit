/**
 * CWS Manual Builder — Spine Tree Display
 *
 * Formats the book structure spine tree for terminal output.
 */

import chalk from 'chalk';
import { SCOPE_DIRS, SCOPE_NAMES, type SpineScope, type SpineScopeNode } from '../spine/types.js';

// =============================================================================
// Spine Tree Display
// =============================================================================

/**
 * Display spine tree with colored output.
 */
export function displaySpineTree(
  bookDir: string,
  scopes: Record<SpineScope, SpineScopeNode>,
): void {
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
