/**
 * CWS Manual Builder — Editorial Lint Display
 *
 * Formats editorial lint results for terminal output.
 * Default: concise mode (top 5 findings per file + per-check summary).
 * Verbose: full output (all findings per file).
 */

import chalk from 'chalk';
import type { EditorialResult, EditorialFinding } from '../../validate/editorial/index.js';

export interface EditorialDisplayOptions {
  /** Show all findings per file (default: top 5 per file) */
  verbose?: boolean;
  /** Max findings shown per file in concise mode (default: 5) */
  maxPerFile?: number;
}

// =============================================================================
// Editorial Lint Display
// =============================================================================

/**
 * Display editorial lint results with colored output.
 *
 * Concise mode (default): shows top N findings per file, then a
 * "...and N more" line, followed by a per-check breakdown summary.
 *
 * Verbose mode (--verbose): shows every finding in every file.
 */
export function displayEditorialResults(result: EditorialResult, options: EditorialDisplayOptions = {}): void {
  const verbose = options.verbose ?? false;
  const maxPerFile = options.maxPerFile ?? 5;

  console.log();
  console.log(chalk.cyan('Editorial Lint'));
  console.log(chalk.cyan('=============='));
  console.log(chalk.gray(`  Categories: ${result.categories.join(', ')}`));

  // Per-check aggregation (built in concise mode for the summary)
  const checkCounts = new Map<string, { warn: number; info: number; message: string }>();

  for (const fileResult of result.files) {
    console.log();
    console.log(chalk.white(`--- ${fileResult.file} ---`));

    if (fileResult.findings.length === 0) {
      console.log(chalk.green('  PASS  No issues found'));
      continue;
    }

    // Always aggregate per-check counts
    for (const finding of fileResult.findings) {
      const entry = checkCounts.get(finding.check) ?? { warn: 0, info: 0, message: '' };
      if (finding.severity === 'warn') entry.warn++;
      else entry.info++;
      if (!entry.message) entry.message = finding.message;
      checkCounts.set(finding.check, entry);
    }

    if (verbose) {
      // Full output — show every finding
      for (const finding of fileResult.findings) {
        printFinding(finding);
      }
    } else {
      // Concise mode — top N findings, then overflow count
      const shown = fileResult.findings.slice(0, maxPerFile);
      const remaining = fileResult.findings.length - shown.length;

      for (const finding of shown) {
        printFinding(finding);
      }

      if (remaining > 0) {
        const remWarn = fileResult.findings.slice(maxPerFile).filter(f => f.severity === 'warn').length;
        const remInfo = remaining - remWarn;
        const parts: string[] = [];
        if (remWarn > 0) parts.push(`${remWarn} warning(s)`);
        if (remInfo > 0) parts.push(`${remInfo} info`);
        console.log(chalk.gray(`          ...and ${remaining} more (${parts.join(', ')})`));
      }
    }
  }

  // Per-check breakdown (concise mode only — gives the birds-eye view)
  if (!verbose && checkCounts.size > 0) {
    console.log(chalk.gray('\n--- By Check ---\n'));

    // Sort by warn count descending
    const sorted = [...checkCounts.entries()].sort((a, b) => b[1].warn - a[1].warn);
    for (const [check, counts] of sorted) {
      const parts: string[] = [];
      if (counts.warn > 0) parts.push(chalk.yellow(`${counts.warn} warn`));
      if (counts.info > 0) parts.push(chalk.gray(`${counts.info} info`));
      const sample = counts.message.length > 80
        ? counts.message.substring(0, 77) + '...'
        : counts.message;
      console.log(`  ${chalk.white(check.padEnd(28))} ${parts.join(', ')}  ${chalk.gray(sample)}`);
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

// =============================================================================
// Helpers
// =============================================================================

function printFinding(finding: EditorialFinding): void {
  const prefix = finding.severity === 'warn'
    ? chalk.yellow('  WARN')
    : chalk.gray('  INFO');
  const cat = chalk.gray(`[${finding.check}]`);
  const loc = finding.line > 0 ? chalk.gray(` (line ${finding.line})`) : '';
  console.log(`${prefix}  ${cat} ${finding.message}${loc}`);

  if (finding.source) {
    console.log(chalk.gray(`          ${finding.source.substring(0, 120)}`));
  }
}
