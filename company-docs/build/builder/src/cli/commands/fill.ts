/**
 * CLI Command: fill — Page content fill analysis, prediction, and cross-checking.
 *
 * Data flow (the editorial page-fill cycle):
 *
 *   Source (.adoc)
 *     │
 *     ├──→ build PDF ──→ MEASURE (actual fill from rendered PDF)
 *     │                       │
 *     │                       ├──→ annotate source: // end p.N — XX%, N lines remaining
 *     │                       │
 *     │                       └──→ compare with prediction (accuracy check)
 *     │
 *     ├──→ PREDICT (estimated fill from source structure — no PDF needed)
 *     │
 *     └──→ INDEX (read cached annotations from source markers — instant)
 *
 * Three data sources, one page model:
 *   - Measured:  from rendered PDF   → authoritative, requires PDF build
 *   - Predicted: from source parsing → estimated, no PDF needed
 *   - Indexed:   from source markers → cached measured data, instant lookup
 *
 * Modes:
 *   cws-build fill <pdf>                         Measure actual fill from PDF
 *   cws-build fill <pdf> --pages 5-10            Measure specific page range
 *   cws-build fill <pdf> --mark                  Suggest // content full markers
 *   cws-build fill <pdf> --cross-check <source>  Cross-check source markers vs PDF
 *   cws-build fill --predict <source>            Predict fill from source (no PDF)
 *   cws-build fill <pdf> --predict <source>      Compare predicted vs measured
 *   cws-build fill --index <source>              Read page index from annotations
 *
 * "The heart of the prudent getteth knowledge;
 *  and the ear of the wise seeketh knowledge." — Proverbs 18:15
 */

import { Command } from 'commander';
import chalk from 'chalk';
import { resolve } from 'path';

import type { BuildConfig, RuntimePaths } from '../../config/types.js';
import {
  analyzePageFill,
  buildFillConfig,
  type FillAnalysis,
  type FillConfig,
} from '../../analysis/page-fill.js';
import {
  crossCheckFill,
  type CrossCheckResult,
  type CrossCheckFinding,
} from '../../analysis/cross-check.js';
import { parseSourceElements, parsePageIndex, type ParseMode, type PageIndexEntry } from '../../analysis/source-parser.js';
import { loadCalibration } from '../../analysis/calibration.js';
import {
  predictFill,
  comparePredictionToActual,
} from '../../analysis/predictor.js';
import type { FillPrediction, PredictionAccuracy } from '../../analysis/render-types.js';

// =============================================================================
// Display Helpers
// =============================================================================

import type { BleedRisk, PageFillResult } from '../../analysis/page-fill.js';

/** Status display config — symbols and colors by range name */
const STATUS_STYLES: Record<string, { symbol: string; color: (s: string) => string }> = {
  sparse:   { symbol: '○', color: chalk.gray },
  normal:   { symbol: '●', color: chalk.green },
  full:     { symbol: '★', color: chalk.yellow },
  overfull: { symbol: '▲', color: chalk.red },
};

const DEFAULT_STYLE = { symbol: '?', color: chalk.white };

function getStyle(status: string) {
  return STATUS_STYLES[status] ?? DEFAULT_STYLE;
}

function pct(ratio: number): string {
  return `${Math.round(ratio * 100)}%`;
}

/** Risk display — symbol and color for bleed risk level */
const RISK_STYLES: Record<BleedRisk, { label: string; color: (s: string) => string }> = {
  'safe':       { label: 'safe',  color: chalk.green },
  'tight':      { label: 'tight', color: chalk.yellow },
  'bleed-risk': { label: 'BLEED', color: chalk.red },
};

function riskLabel(risk: BleedRisk): string {
  const style = RISK_STYLES[risk];
  return style.color(style.label);
}

function fillBar(ratio: number, config: FillConfig, width: number = 20): string {
  const filled = Math.min(Math.round(ratio * width), width);
  const empty = width - filled;
  const bar = '\u2588'.repeat(filled) + '\u2591'.repeat(empty);

  // Color based on which range the ratio falls in
  const overfullMin = config.ranges.overfull?.min ?? 0.90;
  const fullMin = config.ranges.full?.min ?? 0.70;
  const sparseMax = config.ranges.sparse?.max ?? 0.40;

  if (ratio >= overfullMin) return chalk.red(bar);
  if (ratio >= fullMin) return chalk.yellow(bar);
  if (ratio < sparseMax) return chalk.gray(bar);
  return chalk.green(bar);
}

/** Generate a proximity suggestion for a page */
function pageSuggestion(page: PageFillResult): string {
  const headPct = Math.round(page.headroom * 100);
  const lines = page.linesRemaining;

  if (page.risk === 'bleed-risk') {
    if (lines <= 0) return 'at capacity \u2014 any addition bleeds to next page';
    return `~${lines} line${lines === 1 ? '' : 's'} before bleed \u2014 2-pager risk`;
  }
  if (page.risk === 'tight') {
    return `${headPct}% headroom (~${lines} line${lines === 1 ? '' : 's'}) \u2014 content complete`;
  }
  if (page.status === 'full') {
    return `${headPct}% headroom (~${lines} line${lines === 1 ? '' : 's'}) \u2014 near capacity`;
  }
  if (page.status === 'sparse') {
    return `${headPct}% open (~${lines} line${lines === 1 ? '' : 's'}) \u2014 significant room`;
  }
  // normal
  return `${headPct}% headroom (~${lines} line${lines === 1 ? '' : 's'})`;
}

// =============================================================================
// Measured Fill Display (data source: rendered PDF)
// =============================================================================

function displayResults(analysis: FillAnalysis): void {
  const { pages, summary, config } = analysis;

  // Show configured ranges
  const rangeStr = Object.entries(config.ranges)
    .map(([name, r]) => `${name}: ${Math.round(r.min * 100)}-${Math.round(r.max * 100)}%`)
    .join(', ');

  console.log('');
  console.log(chalk.bold('Measured Page Fill') + chalk.gray('  (from rendered PDF)'));
  console.log(chalk.gray(`  PDF: ${analysis.file}`));
  console.log(chalk.gray(`  Pages: ${analysis.totalPages} total, ${pages.length} analyzed`));
  console.log(chalk.gray(`  Ranges: ${rangeStr}`));
  console.log(chalk.gray(`  Bleed threshold: ${Math.round(config.bleedThreshold * 100)}%`));
  console.log('');

  console.log(chalk.gray('  Page  Fill    Bar                  Status     Lines  Risk'));
  console.log(chalk.gray('  ' + '\u2500'.repeat(68)));

  for (const page of pages) {
    const { symbol, color } = getStyle(page.status);
    const pageNum = String(page.page).padStart(4);
    const fill = pct(page.fillRatio).padStart(5);
    const bar = fillBar(page.fillRatio, config);
    const label = color(`${symbol} ${page.status.padEnd(8)}`);
    const lines = String(page.linesRemaining).padStart(5);
    const risk = riskLabel(page.risk);

    console.log(`  ${pageNum}  ${fill}  ${bar}  ${label}  ${lines}  ${risk}`);
  }

  console.log('');
  console.log(chalk.gray('  ' + '\u2500'.repeat(68)));

  const summaryParts = Object.entries(summary).map(([name, count]) => {
    const { color } = getStyle(name);
    return color(`${name}: ${count}`);
  });

  // Risk summary
  const bleedCount = pages.filter((p) => p.risk === 'bleed-risk').length;
  const tightCount = pages.filter((p) => p.risk === 'tight').length;
  const riskParts: string[] = [];
  if (bleedCount > 0) riskParts.push(chalk.red(`${bleedCount} bleed-risk`));
  if (tightCount > 0) riskParts.push(chalk.yellow(`${tightCount} tight`));

  console.log(`  ${summaryParts.join('  ')}`);
  if (riskParts.length > 0) {
    console.log(`  ${riskParts.join('  ')}`);
  }
  console.log('');

  // Proximity suggestions for pages that need attention
  const needsAttention = pages.filter((p) => p.risk !== 'safe');
  if (needsAttention.length > 0) {
    console.log(chalk.bold('  Proximity'));
    console.log('');
    for (const page of needsAttention) {
      const risk = riskLabel(page.risk);
      const suggestion = pageSuggestion(page);
      console.log(`  p.${String(page.page).padEnd(3)} ${risk.padEnd(14)} ${suggestion}`);
    }
    console.log('');
  }
}

function displayMarkerSuggestions(analysis: FillAnalysis): void {
  const fullPages = analysis.pages.filter((p) => p.status === 'full' || p.status === 'overfull');

  if (fullPages.length === 0) {
    console.log(chalk.green('  No pages at fill threshold \u2014 no markers needed.'));
    return;
  }

  console.log(chalk.bold('  Suggested Markers'));
  console.log('');

  for (const page of fullPages) {
    const { color } = getStyle(page.status);
    const status = color(page.status.toUpperCase());
    console.log(`  Page ${page.page} (${pct(page.fillRatio)}) ${status}`);
    console.log(chalk.gray(`    // end p.${page.page} \u2014 content full`));
  }

  console.log('');
}

// =============================================================================
// Cross-Check Display (validates source annotations against measured PDF)
// =============================================================================

const FINDING_STYLES: Record<CrossCheckFinding['type'], { symbol: string; color: (s: string) => string }> = {
  stale_marker:     { symbol: '\u26A0', color: chalk.yellow },
  missing_marker:   { symbol: '\u26A0', color: chalk.yellow },
  page_count_drift: { symbol: '\u2716', color: chalk.red },
  marker_confirmed: { symbol: '\u2714', color: chalk.green },
};

function displayCrossCheck(result: CrossCheckResult): void {
  console.log('');
  console.log(chalk.bold('Cross-Check: Source Markers vs. PDF Fill'));
  console.log(chalk.gray(`  Source: ${result.sourceFile}`));
  console.log(chalk.gray(`  PDF:    ${result.pdfFile}`));
  console.log(chalk.gray(`  Markers found: ${result.markers.length} (${result.markers.filter((m) => m.markedFull).length} marked full)`));
  console.log('');

  // Show non-info findings first (errors and warnings)
  const actionable = result.findings.filter((f) => f.severity !== 'info');
  const confirmed = result.findings.filter((f) => f.severity === 'info');

  if (actionable.length > 0) {
    console.log(chalk.bold('  Findings'));
    console.log('');

    for (const finding of actionable) {
      const style = FINDING_STYLES[finding.type];
      const lineRef = finding.line ? chalk.gray(` (line ${finding.line})`) : '';
      console.log(`  ${style.color(style.symbol)} ${finding.message}${lineRef}`);
    }

    console.log('');
  }

  if (confirmed.length > 0) {
    console.log(chalk.gray(`  ${confirmed.length} marker(s) confirmed accurate`));
  }

  // Summary line
  const { summary } = result;
  const parts: string[] = [];
  if (summary.pageCountDrift) parts.push(chalk.red('page count drift'));
  if (summary.staleMarkers > 0) parts.push(chalk.yellow(`${summary.staleMarkers} stale`));
  if (summary.missingMarkers > 0) parts.push(chalk.yellow(`${summary.missingMarkers} missing`));
  if (summary.confirmed > 0) parts.push(chalk.green(`${summary.confirmed} confirmed`));

  console.log(`  ${parts.join('  |  ')}`);
  console.log('');
}

// =============================================================================
// Page Index Display (data source: cached annotations in source comments)
// =============================================================================

function displayPageIndex(index: PageIndexEntry[], sourcePath: string): void {
  const fullCount = index.filter((e) => e.contentFull).length;
  const annotated = index.filter((e) => e.fill !== undefined).length;

  console.log('');
  console.log(chalk.bold('Page Index') + chalk.gray('  (from source annotations — run measure to refresh)'));
  console.log(chalk.gray(`  Source: ${sourcePath}`));
  console.log(chalk.gray(`  Pages: ${index.length} (${annotated} annotated, ${fullCount} full)`));
  console.log('');

  console.log(chalk.gray('  Page  Fill   Lines  Status       Title'));
  console.log(chalk.gray('  ' + '\u2500'.repeat(70)));

  for (const entry of index) {
    const pageNum = String(entry.page).padStart(4);
    const fill = entry.fill !== undefined ? `${entry.fill}%`.padStart(5) : '   —'.padStart(5);
    const lines = entry.linesRemaining !== undefined
      ? String(entry.linesRemaining).padStart(5)
      : '    —';

    let status: string;
    if (entry.fill === undefined) {
      status = chalk.gray('no data     ');
    } else if (entry.contentFull) {
      status = chalk.red('content full');
    } else if (entry.fill >= 85) {
      status = chalk.yellow('near full   ');
    } else {
      status = chalk.green('has room    ');
    }

    const title = entry.title ? chalk.gray(entry.title) : '';
    const lineRange = chalk.gray(`L${entry.startLine}-${entry.endLine > 0 ? entry.endLine : '?'}`);

    console.log(`  ${pageNum}  ${fill}  ${lines}  ${status}  ${title}  ${lineRange}`);
  }

  console.log('');

  // Summary: pages with room for content
  const withRoom = index.filter((e) => e.fill !== undefined && !e.contentFull);
  if (withRoom.length > 0) {
    const totalLines = withRoom.reduce((sum, e) => sum + (e.linesRemaining ?? 0), 0);
    console.log(chalk.green(`  ${withRoom.length} page(s) with room — ${totalLines} total lines available`));
    console.log('');
  }
}

// =============================================================================
// Predicted Fill Display (data source: source structure + calibration ratios)
// =============================================================================

function displayPrediction(prediction: FillPrediction): void {
  console.log('');
  console.log(chalk.bold('Predicted Page Fill') + chalk.gray('  (estimated from source — no PDF needed)'));
  console.log(chalk.gray(`  Source: ${prediction.file}`));
  console.log(chalk.gray(`  Calibration: ${prediction.calibration.meta.source}`));
  console.log(chalk.gray(`  Predicted pages: ${prediction.totalPages}`));
  console.log('');

  console.log(chalk.gray('  Page  Fill    Bar                  Risk        Lines Left'));
  console.log(chalk.gray('  ' + '\u2500'.repeat(62)));

  for (const page of prediction.pages) {
    const pageNum = String(page.page).padStart(4);
    const fill = pct(page.predictedFill).padStart(5);
    const ratio = page.predictedFill;

    // Simple bar for predicted fill
    const width = 20;
    const filled = Math.min(Math.round(ratio * width), width);
    const empty = width - filled;
    const barStr = '\u2588'.repeat(filled) + '\u2591'.repeat(empty);
    const bar = ratio >= 0.95 ? chalk.red(barStr)
      : ratio >= 0.92 ? chalk.yellow(barStr)
        : ratio < 0.35 ? chalk.gray(barStr)
          : chalk.green(barStr);

    const risk = RISK_STYLES[page.predictedRisk];
    const riskStr = risk.color(risk.label.padEnd(10));
    const lines = String(page.predictedLinesRemaining).padStart(5);
    const forced = page.forcedBreak ? chalk.gray(' [forced]') : '';
    const declared = page.declaredPage ? chalk.gray(` (p.${page.declaredPage})`) : '';

    console.log(`  ${pageNum}  ${fill}  ${bar}  ${riskStr}  ${lines}${forced}${declared}`);
  }

  console.log('');
  console.log(chalk.gray('  ' + '\u2500'.repeat(62)));

  // Risk summary
  const bleedCount = prediction.pages.filter((p) => p.predictedRisk === 'bleed-risk').length;
  const tightCount = prediction.pages.filter((p) => p.predictedRisk === 'tight').length;
  const safeCount = prediction.pages.filter((p) => p.predictedRisk === 'safe').length;
  const parts: string[] = [];
  parts.push(chalk.green(`${safeCount} safe`));
  if (tightCount > 0) parts.push(chalk.yellow(`${tightCount} tight`));
  if (bleedCount > 0) parts.push(chalk.red(`${bleedCount} bleed-risk`));
  console.log(`  ${parts.join('  ')}`);
  console.log('');
}

function displayComparison(prediction: FillPrediction, analysis: FillAnalysis, accuracy: PredictionAccuracy): void {
  console.log('');
  console.log(chalk.bold('Predicted vs Measured') + chalk.gray('  (estimated from source vs actual from PDF)'));
  console.log(chalk.gray(`  Source: ${prediction.file}`));
  console.log(chalk.gray(`  PDF: ${analysis.file}`));
  console.log('');

  console.log(chalk.gray('  Page  Predicted  Measured  Delta   Risk'));
  console.log(chalk.gray('  ' + '\u2500'.repeat(55)));

  for (const delta of accuracy.deltas) {
    const pageNum = String(delta.page).padStart(4);
    const pred = `${delta.predicted}%`.padStart(8);
    const act = `${delta.actual}%`.padStart(8);
    const d = delta.delta;
    const deltaStr = (d >= 0 ? `+${d}` : `${d}`).padStart(5);

    // Highlight divergences >5%
    const deltaColor = Math.abs(d) > 5 ? chalk.red : Math.abs(d) > 2 ? chalk.yellow : chalk.green;

    const predictedPage = prediction.pages.find((p) => p.page === delta.page);
    const risk = predictedPage ? RISK_STYLES[predictedPage.predictedRisk] : RISK_STYLES.safe;
    const riskStr = risk.color(risk.label);

    console.log(`  ${pageNum}  ${pred}  ${act}  ${deltaColor(deltaStr)}   ${riskStr}`);
  }

  console.log('');
  console.log(chalk.gray('  ' + '\u2500'.repeat(58)));

  // Accuracy summary
  const mae = chalk.bold(`${accuracy.meanAbsoluteError}%`);
  const max = accuracy.maxError > 5 ? chalk.red(`${accuracy.maxError}%`) : chalk.green(`${accuracy.maxError}%`);
  const within5 = chalk.green(`${accuracy.pagesWithin5pct}/${accuracy.totalPages}`);

  console.log(`  Mean error: ${mae}  Max: ${max}  Within 5%: ${within5}`);
  console.log('');
}

// =============================================================================
// Command Registration
// =============================================================================

export function registerFillCommand(
  program: Command,
  config: BuildConfig,
  _paths: RuntimePaths,
): void {
  program
    .command('fill [pdf]')
    .description('Measure fill from PDF, predict from source, or read cached page index')
    .option('--pages <range>', 'Page range to analyze (e.g., "5-10", "1,3,5")')
    .option('--threshold <percent>', 'Override full threshold percentage')
    .option('--warn <percent>', 'Override overfull/warning threshold percentage')
    .option('--json', 'Output as JSON')
    .option('--mark', 'Show suggested // content full markers')
    .option('--cross-check <source>', 'Cross-check source .adoc markers against PDF fill')
    .option('--predict <source>', 'Predict fill from AsciiDoc source (with or without PDF)')
    .option('--index <source>', 'Read page index from source markers (lazy search, no PDF)')
    .option('--mode <mode>', 'Parse mode: standalone (section preview) or book (full build)', 'standalone')
    .action((pdf: string | undefined, opts: {
      pages?: string;
      threshold?: string;
      warn?: string;
      json?: boolean;
      mark?: boolean;
      crossCheck?: string;
      predict?: string;
      index?: string;
      mode?: string;
    }) => {
      // --- Index mode — fast page map from source markers ---
      if (opts.index) {
        const sourcePath = resolve(process.cwd(), opts.index);
        const index = parsePageIndex(sourcePath);

        if (opts.json) {
          console.log(JSON.stringify(index, null, 2));
          return;
        }

        displayPageIndex(index, sourcePath);
        return;
      }

      // Require at least one input — PDF or --predict source
      if (!pdf && !opts.predict) {
        console.error(chalk.red('\n  Error: Provide a PDF path, --predict <source>, or --index <source>\n'));
        process.exit(1);
      }

      // Build config: §20 page_fill → CLI overrides → defaults
      const cliOverrides = {
        fullThreshold: opts.threshold ? parseInt(opts.threshold, 10) / 100 : undefined,
        overfullThreshold: opts.warn ? parseInt(opts.warn, 10) / 100 : undefined,
      };
      const lineHeight = config.typography?.line_height;
      const fillConfig = buildFillConfig(config.page_layout!, config.page_fill, cliOverrides, lineHeight);

      const parseMode: ParseMode = (opts.mode === 'book') ? 'book' : 'standalone';

      try {
        // --- Predict-only mode (no PDF) ---
        if (!pdf && opts.predict) {
          const sourcePath = resolve(process.cwd(), opts.predict);
          const elements = parseSourceElements(sourcePath, parseMode);
          const cal = loadCalibration();
          const prediction = predictFill(elements, cal, fillConfig, sourcePath);

          if (opts.json) {
            console.log(JSON.stringify(prediction, null, 2));
            return;
          }

          displayPrediction(prediction);
          return;
        }

        // --- PDF analysis (with optional prediction comparison) ---
        // Resolve PDF relative to CWD (user runs from company-docs/)
        const pdfPath = resolve(process.cwd(), pdf!);
        const analysis = analyzePageFill(pdfPath, fillConfig, opts.pages);

        // Predict + PDF = comparison mode
        if (opts.predict) {
          const sourcePath = resolve(process.cwd(), opts.predict);
          const elements = parseSourceElements(sourcePath, parseMode);
          const cal = loadCalibration();
          const prediction = predictFill(elements, cal, fillConfig, sourcePath);
          const actualFills = analysis.pages.map((p) => ({
            page: p.page,
            fillRatio: p.fillRatio,
          }));
          const accuracy = comparePredictionToActual(prediction, actualFills);

          if (opts.json) {
            console.log(JSON.stringify({ prediction, analysis, accuracy }, null, 2));
            return;
          }

          displayResults(analysis);
          displayComparison(prediction, analysis, accuracy);
          return;
        }

        // Standard fill analysis
        if (opts.json && !opts.crossCheck) {
          console.log(JSON.stringify(analysis, null, 2));
          return;
        }

        if (!opts.crossCheck) {
          displayResults(analysis);
          if (opts.mark) displayMarkerSuggestions(analysis);
          return;
        }

        // Cross-check mode
        const sourcePath = resolve(process.cwd(), opts.crossCheck);
        const markerPattern = config.page_fill?.marker_pattern;
        const crossCheckSettings = config.page_fill?.cross_check ?? {
          stale_marker_below: 70,
          missing_marker_above: 90,
          page_count_drift: true,
        };

        const result = crossCheckFill(sourcePath, analysis, crossCheckSettings, markerPattern);

        if (opts.json) {
          console.log(JSON.stringify({ analysis, crossCheck: result }, null, 2));
          return;
        }

        displayResults(analysis);
        displayCrossCheck(result);

      } catch (error) {
        console.error(chalk.red(`\n  Error: ${(error as Error).message}\n`));
        process.exit(1);
      }
    });
}
