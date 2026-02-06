/**
 * CWS Manual Builder — Banner Display
 *
 * Config-driven startup banner for the CLI.
 */

import chalk from 'chalk';
import type { BuildConfig } from '../config/types.js';

// =============================================================================
// Banner
// =============================================================================

/**
 * Create the startup banner from config values.
 */
export function getBanner(config: BuildConfig): string {
  const companyName = config.attributes['company-name'] || 'CWS';
  const tagline = config.attributes['tagline'] || '';

  return `
${chalk.blue('╔═══════════════════════════════════════════════════════════════════╗')}
${chalk.blue('║')}  ${chalk.bold.white(`${companyName} Manual Builder`)}${' '.repeat(Math.max(0, 46 - companyName.length))}${chalk.blue('║')}
${chalk.blue('║')}  ${chalk.gray(tagline.substring(0, 55).padEnd(55))} ${chalk.blue('║')}
${chalk.blue('╚═══════════════════════════════════════════════════════════════════╝')}
`;
}
