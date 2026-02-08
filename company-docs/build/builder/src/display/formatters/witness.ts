/**
 * CWS Manual Builder — Witness Report Display
 *
 * Terminal dashboard for the WitnessReport.
 * Shows system health, per-domain summaries, recommendations, and totals.
 *
 * "Surely the Lord GOD will do nothing, but he revealeth
 *  his secret unto his servants the prophets." — Amos 3:7
 */

import chalk from 'chalk';
import type { WitnessReport, DomainSummary, DomainHealth, SystemHealth } from '../../witness/types.js';
import { formatDuration } from '../shared/format.js';

export interface WitnessDisplayOptions {
  /** Show all recommendations (default: top 10) */
  verbose?: boolean;
  /** Max recommendations shown (default: 10) */
  maxRecommendations?: number;
}

/**
 * Display the witness report as a terminal dashboard.
 */
export function displayWitnessReport(report: WitnessReport, options: WitnessDisplayOptions = {}): void {
  const verbose = options.verbose ?? false;
  const maxRecs = options.maxRecommendations ?? 10;

  console.log();

  // Header
  console.log(chalk.cyan('Witness Report'));
  console.log(chalk.cyan('=============='));
  console.log(`  Config: ${chalk.white(report.metadata.configVersion)}`
    + `  |  Builder: ${chalk.white(report.metadata.builderVersion)}`
    + `  |  Health: ${formatSystemHealth(report.health)}`);
  console.log();

  // Domain Summary
  console.log(chalk.gray('--- Domain Summary ---'));
  console.log();
  for (const summary of report.summary) {
    console.log(formatDomainRow(summary));
  }

  // Recommendations
  if (report.recommendations.length > 0) {
    console.log();
    console.log(chalk.gray(`--- Recommendations (${report.recommendations.length}) ---`));
    console.log();
    const recsToShow = verbose
      ? report.recommendations
      : report.recommendations.slice(0, maxRecs);
    for (const rec of recsToShow) {
      const priority = chalk.gray(`[${rec.priority}]`);
      const domain = chalk.cyan(rec.domain);
      const message = rec.message;
      const action = rec.action ? chalk.gray(` — run: ${rec.action}`) : '';
      console.log(`  ${priority} ${domain}: ${message}${action}`);
    }
    const remaining = report.recommendations.length - recsToShow.length;
    if (remaining > 0) {
      console.log(chalk.gray(`  ...and ${remaining} more (use --verbose to see all)`));
    }
  }

  // Totals
  console.log();
  console.log(chalk.gray('--- Totals ---'));
  console.log();
  const { errors, warnings, info, passes } = report.totals;
  const parts = [
    errors > 0 ? chalk.red(`${errors} error(s)`) : chalk.gray(`${errors} errors`),
    warnings > 0 ? chalk.yellow(`${warnings} warning(s)`) : chalk.gray(`${warnings} warnings`),
    chalk.gray(`${info} info`),
    chalk.green(`${passes} pass(es)`),
  ];
  console.log(`  ${parts.join('  |  ')}`);
  console.log(`  Duration: ${formatDuration(report.metadata.duration)}`);
  console.log();
}

// =============================================================================
// Formatters
// =============================================================================

function formatSystemHealth(health: SystemHealth): string {
  switch (health) {
    case 'healthy': return chalk.green('HEALTHY (tov)');
    case 'degraded': return chalk.yellow('DEGRADED (yashar)');
    case 'failing': return chalk.red('FAILING (shavar)');
  }
}

function formatDomainHealth(health: DomainHealth): string {
  switch (health) {
    case 'pass': return chalk.green('PASS');
    case 'warn': return chalk.yellow('WARN');
    case 'fail': return chalk.red('FAIL');
    case 'skipped': return chalk.gray('SKIP');
  }
}

function formatDomainRow(summary: DomainSummary): string {
  const health = formatDomainHealth(summary.health);
  const label = summary.label.padEnd(24);
  const duration = chalk.gray(`(${formatDuration(summary.duration)})`);

  // Build counts string from domain-specific counts
  const countParts: string[] = [];
  for (const [key, val] of Object.entries(summary.counts)) {
    if ((val as number) > 0) {
      countParts.push(`${val} ${key}`);
    }
  }
  const counts = countParts.length > 0
    ? countParts.join(', ')
    : (summary.error ?? 'no findings');

  return `  ${health}  ${label} ${counts}  ${duration}`;
}
