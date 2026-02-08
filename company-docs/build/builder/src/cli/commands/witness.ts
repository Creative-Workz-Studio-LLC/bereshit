/**
 * CWS Manual Builder — Witness Command
 *
 * Command: witness — comprehensive system health check across
 * all 5 validation domains. Produces a unified WitnessReport
 * as terminal dashboard or JSON output.
 *
 * The witness concept is theological: the system observes, records,
 * and reports its own health. This is the observation layer of the
 * CPI-SI feedback loop.
 *
 * "Surely the Lord GOD will do nothing, but he revealeth
 *  his secret unto his servants the prophets." — Amos 3:7
 */

import type { Command } from 'commander';
import type { BuildConfig, RuntimePaths } from '../../config/types.js';
import type { BookType, BonusScope } from '../../validate/types.js';
import type { DomainName } from '../../witness/types.js';
import { ALL_DOMAINS } from '../../witness/types.js';
import { runWitness } from '../../witness/runner.js';
import { serializeWitnessReport, writeWitnessReport } from '../../witness/serializer.js';
import { displayWitnessReport } from '../../display/formatters/witness.js';
import { exitWithError } from '../utils.js';

// =============================================================================
// Command Registration
// =============================================================================

interface WitnessOpts {
  type?: string;
  bonus?: string;
  json?: boolean;
  out?: string;
  domains?: string;
  category?: string;
  documentType?: string;
  register?: string;
  file?: string;
  scope?: string;
  verbose?: boolean;
}

export function registerWitnessCommand(
  program: Command,
  config: BuildConfig,
  paths: RuntimePaths,
): void {
  program
    .command('witness')
    .description('Comprehensive system health check — all validation domains')
    .option('-t, --type <type>', 'Book type for structural validation (manual, manual-l)')
    .option('-b, --bonus <scope>', 'Bonus validation scope (operational)')
    .option('--json', 'Output JSON report to stdout')
    .option('--out <file>', 'Write JSON report to file')
    .option('--domains <list>', 'Comma-separated domains: structural,editorial,theme,bible,toc')
    .option('--category <category>', 'Editorial lint category filter')
    .option('--document-type <type>', 'Document type context for editorial lint')
    .option('--register <register>', 'Expected register for editorial lint')
    .option('-f, --file <file>', 'Single file scope for editorial/bible/toc')
    .option('-s, --scope <scope>', 'Scope letter (A, B, C, D)')
    .option('-v, --verbose', 'Show all recommendations (default: top 10)')
    .action(async (opts: WitnessOpts) => {
      // Parse domain filter
      let domains: DomainName[] | undefined;
      if (opts.domains) {
        const requested = opts.domains.split(',').map(s => s.trim()) as DomainName[];
        const valid = requested.filter(d => (ALL_DOMAINS as readonly string[]).includes(d));
        if (valid.length === 0) {
          exitWithError('E11', `Invalid domain(s): ${opts.domains}. Valid: ${ALL_DOMAINS.join(', ')}`);
        }
        domains = valid;
      }

      // Run the witness
      const report = await runWitness(config, paths, {
        bookType: opts.type as BookType | undefined,
        bonus: opts.bonus as BonusScope | undefined,
        domains,
        category: opts.category,
        documentType: opts.documentType,
        register: opts.register,
        file: opts.file,
        scope: opts.scope,
      });

      // JSON output
      if (opts.json) {
        process.stdout.write(serializeWitnessReport(report) + '\n');
      }

      // File output
      if (opts.out) {
        await writeWitnessReport(report, opts.out);
      }

      // Terminal display (always unless --json without --out)
      if (!opts.json) {
        displayWitnessReport(report, { verbose: opts.verbose });
      } else if (opts.out) {
        // When writing to file + JSON, show a brief summary
        displayWitnessReport(report, { verbose: opts.verbose });
      }

      // Exit with error if system is failing
      if (report.health === 'failing') {
        exitWithError('E70', `System health: FAILING — ${report.totals.errors} error(s) across ${report.metadata.domainsExecuted.length} domain(s)`);
      }
    });
}
