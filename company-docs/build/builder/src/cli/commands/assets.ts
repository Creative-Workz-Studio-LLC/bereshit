/**
 * CWS Manual Builder — Assets Command
 *
 * Command: assets — generate derived visual assets
 */

import chalk from 'chalk';
import type { Command } from 'commander';
import type { BuildConfig, RuntimePaths } from '../../config/types.js';
import { generateAssets } from '../../assets/generator.js';
import type { GenerationReport } from '../../assets/types.js';
import { displayAssetReport } from '../../display/assets-report.js';
import { exitWithError } from '../utils.js';

// =============================================================================
// Command Registration
// =============================================================================

export function registerAssetsCommand(
  program: Command,
  config: BuildConfig,
  paths: RuntimePaths,
): void {
  program
    .command('assets')
    .description('Generate derived visual assets (diagrams, grayscale variants, palette swatch)')
    .option('--force', 'Force regeneration (ignore timestamps)')
    .action(async (opts: { force?: boolean }) => {
      console.log();
      console.log(chalk.cyan('Asset Generation'));
      console.log(chalk.cyan('================'));

      const assetsDir = `${paths.bookDir}/00-assets`;
      const pngScale = config.assets?.figures?.png_scale ?? 3;

      const report: GenerationReport = await generateAssets({
        assetsDir,
        force: opts.force,
        pngScale,
      });

      displayAssetReport(report);
      console.log();

      if (report.totals.failed > 0) {
        exitWithError('E31', `Asset generation failed: ${report.totals.failed} asset(s) failed`);
      }
    });
}
