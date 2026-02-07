/**
 * CWS Manual Builder — Editorial Commands (Make delegation)
 *
 * Commands: status, editorial, section, article, part, remove, pages
 * All delegate to the Makefile — the primary editorial build system.
 */

import type { Command } from 'commander';
import type { BuildConfig, RuntimePaths } from '../../config/types.js';
import { delegateToMake } from '../utils.js';

// =============================================================================
// Command Registration
// =============================================================================

export function registerEditorialCommands(
  program: Command,
  _config: BuildConfig,
  paths: RuntimePaths,
): void {
  // cws-build status
  program
    .command('status')
    .description('Show build dashboard (current/stale PDFs, sizes, timestamps)')
    .action(async () => {
      await delegateToMake('status', paths.sourceDir);
    });

  // cws-build editorial
  program
    .command('editorial')
    .description('Full editorial build: validate → scopes → articles → sections → master')
    .option('--force', 'Force rebuild (ignore cache)')
    .action(async (opts: { force?: boolean }) => {
      const args = opts.force ? 'book FORCE=1' : 'book';
      await delegateToMake(args, paths.sourceDir);
    });

  // cws-build remove
  program
    .command('remove')
    .description('Safe targeted removal of build outputs')
    .option('-s, --scope <scope>', 'Scope letter (A, B, C, D, assets, master)')
    .option('-p, --part <part>', 'Part ID or search term')
    .option('-a, --article <article>', 'Article ID or search term')
    .option('-f, --file <file>', 'Single file name or path')
    .action(async (opts: { scope?: string; part?: string; article?: string; file?: string }) => {
      const makeArgs: string[] = ['remove'];
      if (opts.scope) makeArgs.push(`SCOPE=${opts.scope}`);
      if (opts.part) makeArgs.push(`PART=${opts.part}`);
      if (opts.article) makeArgs.push(`ARTICLE=${opts.article}`);
      if (opts.file) makeArgs.push(`FILE=${opts.file}`);
      await delegateToMake(makeArgs.join(' '), paths.sourceDir);
    });

  // cws-build section
  program
    .command('section <file>')
    .description('Build a single section PDF (themed)')
    .option('--force', 'Force rebuild (ignore cache)')
    .action(async (file: string, opts: { force?: boolean }) => {
      const force = opts.force ? ' FORCE=1' : '';
      await delegateToMake(`section FILE=${file}${force}`, paths.sourceDir);
    });

  // cws-build article
  program
    .command('article <id>')
    .description('Build all sections in an article, then combined PDF')
    .option('--force', 'Force rebuild (ignore cache)')
    .action(async (id: string, opts: { force?: boolean }) => {
      const force = opts.force ? ' FORCE=1' : '';
      await delegateToMake(`article ARTICLE=${id}${force}`, paths.sourceDir);
    });

  // cws-build part
  program
    .command('part <id>')
    .description('Build a part (cascades to articles and sections)')
    .option('--force', 'Force rebuild (ignore cache)')
    .action(async (id: string, opts: { force?: boolean }) => {
      const force = opts.force ? ' FORCE=1' : '';
      await delegateToMake(`part PART=${id}${force}`, paths.sourceDir);
    });

  // cws-build pages
  program
    .command('pages <file>')
    .description('Extract page range from a section PDF (requires qpdf)')
    .requiredOption('--pages <range>', 'Page range (e.g., "5", "17-19", "1-3,17-19")')
    .option('--force', 'Force rebuild section PDF first')
    .action(async (file: string, opts: { pages: string; force?: boolean }) => {
      const force = opts.force ? ' FORCE=1' : '';
      await delegateToMake(`pages FILE=${file} PAGES=${opts.pages}${force}`, paths.sourceDir);
    });
}
