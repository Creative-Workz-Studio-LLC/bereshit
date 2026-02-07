/**
 * CWS Manual Builder — TOC Command
 *
 * Command: toc — parse, fill placeholders, verify page numbers
 */

import type { Command } from 'commander';
import type { BuildConfig, RuntimePaths } from '../../config/types.js';
import { displayTocResults, displayTocFillResult, displayTocCheckResult } from '../../display/toc.js';
import { exitWithError } from '../utils.js';

// =============================================================================
// Command Registration
// =============================================================================

export function registerTocCommand(
  program: Command,
  _config: BuildConfig,
  paths: RuntimePaths,
): void {
  program
    .command('toc [file]')
    .description('Parse and display TOC structure, fill placeholders, or check page numbers')
    .option('-s, --scope <scope>', 'Scope letter (A, B, C, D)')
    .option('--status', 'Show page status only (no § sections)')
    .option('--fill', 'Fill p. __ placeholders with computed page numbers')
    .option('--verify', 'Check existing page numbers against computed values')
    .option('--dry-run', 'With --fill: report changes without writing')
    .option('--json', 'Output as JSON for programmatic use')
    .action(async (file: string | undefined, opts: { scope?: string; status?: boolean; fill?: boolean; verify?: boolean; dryRun?: boolean; json?: boolean }) => {
      if (opts.fill) {
        // TOC fill pipeline: compute page numbers → replace p. __ placeholders
        const { runTocFill } = await import('../../validate/pages.js');
        const result = await runTocFill(paths, { dryRun: opts.dryRun });
        displayTocFillResult(result, opts.dryRun);
      } else if (opts.verify) {
        // TOC verify pipeline: compare existing page numbers against computed values
        const { runTocCheck } = await import('../../validate/pages.js');
        const result = await runTocCheck(paths);
        displayTocCheckResult(result);

        if (result.stale > 0) {
          exitWithError('E41', `TOC verify: ${result.stale} page number(s) are stale`);
        }
      } else {
        // Default: parse and display TOC structure
        const { runPageParse } = await import('../../validate/pages.js');
        const result = await runPageParse(paths, {
          file: file || undefined,
          scope: opts.scope,
        });

        if (opts.json) {
          console.log(JSON.stringify(result, null, 2));
        } else {
          displayTocResults(result);
        }
      }
    });
}
