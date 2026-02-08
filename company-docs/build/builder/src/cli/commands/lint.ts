/**
 * CWS Manual Builder — Lint Command
 *
 * Command: lint — editorial, style, structure, conversion, legal,
 *                  tone, industry, Bible, TOC checks
 *
 * Flag combinations:
 *   lint                            — all categories
 *   lint --category legal           — legal checks only (18-26)
 *   lint --category tone            — tone/voice checks only (27-29)
 *   lint --category industry        — industry checks only (30-31)
 *   lint --tone                     — shorthand for --category tone
 *   lint --industry                 — shorthand for --category industry
 *   lint --editorial                — shorthand for --category editorial
 *   lint --document-type eula       — all checks with register context
 *   lint --document-type eula --category tone — tone checks in EULA context
 *   lint --report                   — JSON witness report output
 */

import type { Command } from 'commander';
import type { BuildConfig, RuntimePaths } from '../../config/types.js';
import { displayBibleFindings } from '../../display/formatters/bible.js';
import { displayTocResults } from '../../display/formatters/toc.js';
import { displayEditorialResults } from '../../display/formatters/editorial.js';
import { exitWithError } from '../utils.js';

// =============================================================================
// Command Registration
// =============================================================================

interface LintOpts {
  file?: string;
  scope?: string;
  bible?: boolean;
  toc?: boolean;
  editorial?: boolean;
  tone?: boolean;
  industry?: boolean;
  category?: string;
  documentType?: string;
  register?: string;
  report?: boolean;
  verbose?: boolean;
}

export function registerLintCommand(
  program: Command,
  config: BuildConfig,
  paths: RuntimePaths,
): void {
  program
    .command('lint')
    .description('Run editorial content validation (editorial/style/structure/conversion/legal/tone/industry/Bible/TOC)')
    .option('-f, --file <file>', 'Single file to lint')
    .option('-s, --scope <scope>', 'Scope letter (A, B, C, D)')
    .option('--bible', 'Run Bible verse validation only (check citations against KJV/WEB)')
    .option('--toc', 'Run TOC/page marker validation only (sequence, banners, content-full)')
    .option('--editorial', 'Run editorial lint only (checks 1-8: shall, spacers, markers, etc.)')
    .option('--tone', 'Run tone/voice checks only (checks 27-29: compulsion, plain language, sentence length)')
    .option('--industry', 'Run industry-specific checks only (checks 30-31: register, provisions)')
    .option('--category <category>', 'Check category: editorial, style, structure, conversion, legal, tone, industry, all (default: all)')
    .option('--document-type <type>', 'Document type context (eula, creator_agreement, board_resolution, internal_memo, etc.)')
    .option('--register <register>', 'Expected register (formal_traditional, formal_covenant, plain, informal, warm)')
    .option('--report', 'Output JSON witness report instead of terminal display')
    .option('-v, --verbose', 'Show all findings (default: top 5 per file)')
    .action(async (opts: LintOpts) => {
      if (opts.bible) {
        // Native TS Bible verse validation
        const { runBibleValidation } = await import('../../validate/bible/index.js');
        const start = Date.now();
        const result = await runBibleValidation(paths, {
          file: opts.file,
          scope: opts.scope,
        });
        const duration = Date.now() - start;

        if (opts.report) {
          const { WitnessCollector } = await import('../../witness/collector.js');
          const { serializeWitnessReport } = await import('../../witness/serializer.js');
          const collector = new WitnessCollector('lint', opts as Record<string, unknown>, ['bible']);
          collector.addBible(result, duration);
          const report = collector.finalize(config.version ?? 'unknown', '3.0.0');
          process.stdout.write(serializeWitnessReport(report) + '\n');
        } else {
          displayBibleFindings(result);
        }
      } else if (opts.toc) {
        // Native TS page/TOC validation
        const { runPageParse } = await import('../../validate/toc/index.js');
        const start = Date.now();
        const result = await runPageParse(paths, {
          file: opts.file,
          scope: opts.scope,
        });
        const duration = Date.now() - start;

        if (opts.report) {
          const { WitnessCollector } = await import('../../witness/collector.js');
          const { serializeWitnessReport } = await import('../../witness/serializer.js');
          const collector = new WitnessCollector('lint', opts as Record<string, unknown>, ['toc']);
          collector.addToc(result, duration);
          const report = collector.finalize(config.version ?? 'unknown', '3.0.0');
          process.stdout.write(serializeWitnessReport(report) + '\n');
        } else {
          displayTocResults(result);

          // Exit with error if there are errors
          const pageErrors = result.issues.filter(i => i.severity === 'error');
          if (pageErrors.length > 0) {
            exitWithError('E41', `Page validation failed: ${pageErrors.length} error(s)`);
          }
        }
      } else {
        // Native TS editorial lint (replaces Makefile grep chains)
        const { runEditorialLint } = await import('../../validate/editorial/index.js');

        // Determine check category — shorthand flags override --category
        let checkCategory = opts.category ?? 'all';
        if (opts.editorial) checkCategory = 'editorial';
        if (opts.tone) checkCategory = 'tone';
        if (opts.industry) checkCategory = 'industry';

        const start = Date.now();
        const result = await runEditorialLint(
          paths,
          {
            file: opts.file,
            scope: opts.scope,
            check: checkCategory,
            documentType: opts.documentType,
            register: opts.register,
          },
          config.legal_language,
          config.tone_and_voice,
          config.industry_specific,
          config.document_types,
          config.asciidoc_publishing,
          config.company_conventions,
        );
        const duration = Date.now() - start;

        if (opts.report) {
          const { WitnessCollector } = await import('../../witness/collector.js');
          const { serializeWitnessReport } = await import('../../witness/serializer.js');
          const collector = new WitnessCollector('lint', opts as Record<string, unknown>, ['editorial']);
          collector.addEditorial(result, duration);
          const report = collector.finalize(config.version ?? 'unknown', '3.0.0');
          process.stdout.write(serializeWitnessReport(report) + '\n');
        } else {
          displayEditorialResults(result, { verbose: opts.verbose });

          // Exit with error code if warnings found (matches Makefile behavior)
          if (result.totals.warn > 0) {
            exitWithError('E43', `Editorial lint: ${result.totals.warn} warning(s) found`);
          }
        }
      }
    });
}
