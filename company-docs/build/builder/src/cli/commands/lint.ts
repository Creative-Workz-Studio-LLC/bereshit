/**
 * CWS Manual Builder — Lint Command
 *
 * Command: lint — editorial, style, structure, conversion, Bible, TOC checks
 */

import type { Command } from 'commander';
import type { BuildConfig, RuntimePaths } from '../../config/types.js';
import { displayBibleFindings } from '../../display/bible.js';
import { displayTocResults } from '../../display/toc.js';
import { displayEditorialResults } from '../../display/editorial.js';
import { exitWithError } from '../utils.js';

// =============================================================================
// Command Registration
// =============================================================================

export function registerLintCommand(
  program: Command,
  _config: BuildConfig,
  paths: RuntimePaths,
): void {
  program
    .command('lint')
    .description('Run editorial content validation (editorial/style/structure/conversion/Bible/TOC)')
    .option('-f, --file <file>', 'Single file to lint')
    .option('-s, --scope <scope>', 'Scope letter (A, B, C, D)')
    .option('--bible', 'Run Bible verse validation only (check citations against KJV/WEB)')
    .option('--toc', 'Run TOC/page marker validation only (sequence, banners, content-full)')
    .option('--editorial', 'Run editorial lint only (checks 1-8: shall, spacers, markers, etc.)')
    .option('--category <category>', 'Check category: editorial, style, structure, conversion, all (default: all)')
    .action(async (opts: { file?: string; scope?: string; bible?: boolean; toc?: boolean; editorial?: boolean; category?: string }) => {
      if (opts.bible) {
        // Native TS Bible verse validation
        const { runBibleValidation } = await import('../../validate/bible.js');
        const result = await runBibleValidation(paths, {
          file: opts.file,
          scope: opts.scope,
        });
        displayBibleFindings(result);
      } else if (opts.toc) {
        // Native TS page/TOC validation
        const { runPageParse } = await import('../../validate/pages.js');
        const result = await runPageParse(paths, {
          file: opts.file,
          scope: opts.scope,
        });
        displayTocResults(result);

        // Exit with error if there are errors
        const pageErrors = result.issues.filter(i => i.severity === 'error');
        if (pageErrors.length > 0) {
          exitWithError('E41', `Page validation failed: ${pageErrors.length} error(s)`);
        }
      } else {
        // Native TS editorial lint (replaces Makefile grep chains)
        const { runEditorialLint } = await import('../../validate/editorial.js');

        // Determine check category
        let checkCategory = opts.category ?? 'all';
        if (opts.editorial) checkCategory = 'editorial';

        const result = await runEditorialLint(paths, {
          file: opts.file,
          scope: opts.scope,
          check: checkCategory,
        });
        displayEditorialResults(result);

        // Exit with error code if warnings found (matches Makefile behavior)
        if (result.totals.warn > 0) {
          exitWithError('E43', `Editorial lint: ${result.totals.warn} warning(s) found`);
        }
      }
    });
}
