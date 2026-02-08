/**
 * CWS Manual Builder — Witness Collector
 *
 * Gathers results from each validation domain into a unified
 * WitnessReport. The collector receives — it does NOT run validators.
 * Separation of concerns: runner/commands run validators, collector
 * assembles the report.
 *
 * Pattern:
 *   1. Construct with trigger context
 *   2. Add domain results as they complete
 *   3. finalize() computes health, sorts, generates recommendations
 *
 * "For we must all appear before the judgment seat of Christ;
 *  that every one may receive the things done in his body."
 *  — 2 Corinthians 5:10
 */

import type {
  WitnessReport,
  WitnessMetadata,
  DomainResult,
  DomainSummary,
  DomainName,
  DomainHealth,
  ActiveHealth,
  SystemHealth,
  StructuralDomainResult,
  EditorialDomainResult,
  ThemeDomainResult,
  ThemeFileResult,
  BibleDomainResult,
  TocDomainResult,
  SkippedDomainResult,
} from './types.js';

import { WITNESS_SCHEMA_VERSION } from './types.js';
import { generateRecommendations } from './recommendations.js';

import type { ValidationResult } from '../validate/types.js';
import type { EditorialResult } from '../validate/editorial/index.js';
import type { BibleValidationResult } from '../validate/bible/index.js';
import type { PageParseResult } from '../validate/toc/index.js';

// =============================================================================
// Domain Labels
// =============================================================================

/** Human-readable labels for each domain. */
const DOMAIN_LABELS: Record<DomainName, string> = {
  structural: 'Structural Validation',
  editorial: 'Editorial Lint',
  theme: 'Theme Validation',
  bible: 'Scripture Verification',
  toc: 'TOC & Page Markers',
};

// =============================================================================
// Health Computation
// =============================================================================

/** Compute domain health from native severity counts. */
function computeStructuralHealth(result: ValidationResult): ActiveHealth {
  if (result.counts.fail > 0) return 'fail';
  if (result.counts.warn > 0) return 'warn';
  return 'pass';
}

function computeEditorialHealth(result: EditorialResult): ActiveHealth {
  if (result.totals.warn > 0) return 'warn';
  return 'pass';
}

function computeThemeHealth(themes: ThemeFileResult[]): ActiveHealth {
  let hasError = false;
  let hasWarn = false;

  for (const t of themes) {
    if ((t.counts['fail'] ?? 0) > 0 || (t.counts['error'] ?? 0) > 0) hasError = true;
    if ((t.counts['warn'] ?? 0) > 0) hasWarn = true;
  }

  if (hasError) return 'fail';
  if (hasWarn) return 'warn';
  return 'pass';
}

function computeBibleHealth(result: BibleValidationResult): ActiveHealth {
  if (result.counts['not-found'] > 0) return 'fail';
  if (result.counts['mismatch'] > 0) return 'warn';
  return 'pass';
}

function computeTocHealth(result: PageParseResult): ActiveHealth {
  const hasError = result.issues.some(i => i.severity === 'error');
  const hasWarn = result.issues.some(i => i.severity === 'warn');

  if (hasError) return 'fail';
  if (hasWarn) return 'warn';
  return 'pass';
}

/** Compute overall system health from domain summaries. */
function computeSystemHealth(summaries: DomainSummary[]): SystemHealth {
  const hasFailure = summaries.some(s => s.health === 'fail');
  const hasWarning = summaries.some(s => s.health === 'warn');

  if (hasFailure) return 'failing';   // shavar (-1)
  if (hasWarning) return 'degraded';  // yashar (0)
  return 'healthy';                    // tov (+1)
}

/** Health severity order for sorting (worst first). */
const HEALTH_ORDER: Record<DomainHealth, number> = {
  fail: 0,
  warn: 1,
  pass: 2,
  skipped: 3,
};

// =============================================================================
// WitnessCollector
// =============================================================================

/**
 * Collects domain results and assembles a WitnessReport.
 *
 * Usage:
 *   const collector = new WitnessCollector('witness', { type: 'manual-l' });
 *   collector.addStructural(result, duration);
 *   collector.addEditorial(result, duration);
 *   // ... add more domains ...
 *   const report = collector.finalize('5.2.0', '3.0.0');
 */
export class WitnessCollector {
  private readonly trigger: string;
  private readonly options: Record<string, unknown>;
  private readonly startTime: number;
  private readonly domainsRequested: DomainName[];
  private readonly results: DomainResult[] = [];

  constructor(
    trigger: string,
    options: Record<string, unknown>,
    domainsRequested?: DomainName[],
  ) {
    this.trigger = trigger;
    this.options = options;
    this.startTime = Date.now();
    this.domainsRequested = domainsRequested ?? [...(['structural', 'editorial', 'theme', 'bible', 'toc'] as const)];
  }

  // ---------------------------------------------------------------------------
  // Domain Adders
  // ---------------------------------------------------------------------------

  /** Add structural validation results. */
  addStructural(result: ValidationResult, duration: number): void {
    const domain: StructuralDomainResult = {
      domain: 'structural',
      health: computeStructuralHealth(result),
      duration,
      result,
    };
    this.results.push(domain);
  }

  /** Add editorial lint results. */
  addEditorial(result: EditorialResult, duration: number): void {
    const domain: EditorialDomainResult = {
      domain: 'editorial',
      health: computeEditorialHealth(result),
      duration,
      result,
    };
    this.results.push(domain);
  }

  /** Add theme validation results (accumulated per-file). */
  addTheme(themes: ThemeFileResult[], duration: number): void {
    const domain: ThemeDomainResult = {
      domain: 'theme',
      health: computeThemeHealth(themes),
      duration,
      themes,
    };
    this.results.push(domain);
  }

  /** Add Bible/scripture validation results. */
  addBible(result: BibleValidationResult, duration: number): void {
    const domain: BibleDomainResult = {
      domain: 'bible',
      health: computeBibleHealth(result),
      duration,
      result,
    };
    this.results.push(domain);
  }

  /** Add TOC/pages validation results. */
  addToc(result: PageParseResult, duration: number): void {
    const domain: TocDomainResult = {
      domain: 'toc',
      health: computeTocHealth(result),
      duration,
      result,
    };
    this.results.push(domain);
  }

  // ---------------------------------------------------------------------------
  // Skip / Fail
  // ---------------------------------------------------------------------------

  /** Mark a domain as skipped (requested but could not execute). */
  skipDomain(domain: DomainName, reason: string): void {
    const skipped: SkippedDomainResult = {
      domain,
      health: 'skipped',
      duration: 0,
      reason,
    };
    this.results.push(skipped);
  }

  /** Mark a domain as failed (threw during execution). */
  failDomain(domain: DomainName, error: Error): void {
    const skipped: SkippedDomainResult = {
      domain,
      health: 'skipped',
      duration: 0,
      reason: `Domain threw: ${error.message}`,
    };
    this.results.push(skipped);
  }

  // ---------------------------------------------------------------------------
  // Finalize
  // ---------------------------------------------------------------------------

  /**
   * Assemble the final WitnessReport.
   *
   * Computes:
   *   - Per-domain health from native severity counts
   *   - System health: any fail → failing, any warn → degraded, else healthy
   *   - Summary sorted worst-first
   *   - Recommendations from domain analysis
   */
  finalize(configVersion: string, builderVersion: string): WitnessReport {
    const totalDuration = Date.now() - this.startTime;

    // Build summaries from results
    const summaries = this.results.map(r => this.buildSummary(r));

    // Sort worst-first
    summaries.sort((a, b) => HEALTH_ORDER[a.health] - HEALTH_ORDER[b.health]);

    // Compute system health
    const health = computeSystemHealth(summaries);

    // Compute aggregated totals
    const totals = this.computeTotals();

    // Executed domains (exclude skipped)
    const domainsExecuted = this.results
      .filter(r => r.health !== 'skipped')
      .map(r => r.domain);

    // Generate recommendations
    const recommendations = generateRecommendations(this.results);

    // Build metadata
    const metadata: WitnessMetadata = {
      schemaVersion: WITNESS_SCHEMA_VERSION,
      timestamp: new Date().toISOString(),
      configVersion,
      builderVersion,
      duration: totalDuration,
      domainsRequested: this.domainsRequested,
      domainsExecuted,
      trigger: this.trigger,
      options: this.options,
    };

    return {
      metadata,
      health,
      summary: summaries,
      domains: this.results,
      totals,
      recommendations,
    };
  }

  // ---------------------------------------------------------------------------
  // Internal Helpers
  // ---------------------------------------------------------------------------

  /** Build a DomainSummary from a DomainResult. */
  private buildSummary(result: DomainResult): DomainSummary {
    const base = {
      domain: result.domain,
      label: DOMAIN_LABELS[result.domain],
      health: result.health,
      duration: result.duration,
    };

    // Guard: skipped domains have no native result to summarize
    if (result.health === 'skipped') {
      return { ...base, counts: {}, error: result.reason };
    }

    // Now TypeScript knows result is NOT SkippedDomainResult
    switch (result.domain) {
      case 'structural':
        return { ...base, counts: { ...result.result.counts } };

      case 'editorial':
        return { ...base, counts: { ...result.result.totals, files: result.result.filesChecked } };

      case 'theme': {
        const counts: Record<string, number> = { themes: result.themes.length };
        for (const t of result.themes) {
          for (const [key, val] of Object.entries(t.counts)) {
            counts[key] = (counts[key] ?? 0) + (val as number);
          }
        }
        return { ...base, counts };
      }

      case 'bible':
        return { ...base, counts: { ...result.result.counts, files: result.result.filesChecked } };

      case 'toc':
        return { ...base, counts: { ...result.result.totals } };
    }
  }

  /** Compute aggregated totals across all domains. */
  private computeTotals(): { errors: number; warnings: number; info: number; passes: number } {
    let errors = 0;
    let warnings = 0;
    let info = 0;
    let passes = 0;

    for (const r of this.results) {
      // Skipped domains contribute nothing to totals
      if (r.health === 'skipped') continue;

      // Now TypeScript knows r is not SkippedDomainResult
      switch (r.domain) {
        case 'structural':
          errors += r.result.counts.fail ?? 0;
          warnings += r.result.counts.warn ?? 0;
          info += r.result.counts.info ?? 0;
          passes += r.result.counts.pass ?? 0;
          break;

        case 'editorial':
          warnings += r.result.totals.warn;
          info += r.result.totals.info;
          break;

        case 'theme':
          for (const t of r.themes) {
            errors += (t.counts['fail'] ?? 0) + (t.counts['error'] ?? 0);
            warnings += t.counts['warn'] ?? 0;
            info += t.counts['info'] ?? 0;
            passes += t.counts['pass'] ?? 0;
          }
          break;

        case 'bible':
          errors += r.result.counts['not-found'] ?? 0;
          warnings += r.result.counts['mismatch'] ?? 0;
          passes += r.result.counts['verified'] ?? 0;
          info += r.result.counts['ref-only'] ?? 0;
          break;

        case 'toc':
          for (const issue of r.result.issues) {
            if (issue.severity === 'error') errors++;
            else if (issue.severity === 'warn') warnings++;
            else info++;
          }
          break;
      }
    }

    return { errors, warnings, info, passes };
  }
}
