/**
 * CWS Manual Builder — Validation Display
 *
 * Formats structural validation findings for terminal output.
 */

import chalk from 'chalk';
import type { ValidationFinding } from '../../validate/types.js';

// =============================================================================
// Validation Findings Display
// =============================================================================

/**
 * Display validation findings with colored output.
 */
export function displayFindings(findings: ValidationFinding[]): void {
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
