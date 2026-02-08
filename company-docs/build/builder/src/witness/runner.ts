/**
 * CWS Manual Builder — Witness Runner
 *
 * Orchestrates all 5 validation domains into a unified WitnessReport.
 * Each domain runs independently (try/caught) — if one throws,
 * the rest continue and the failing domain is marked as skipped.
 *
 * Order: structural → theme → editorial → bible → toc
 *
 * "Surely the Lord GOD will do nothing, but he revealeth
 *  his secret unto his servants the prophets." — Amos 3:7
 */

import { readdir } from 'node:fs/promises';
import { basename, join } from 'node:path';

import type { BuildConfig, RuntimePaths } from '../config/types.js';
import type { BookType, BonusScope, ValidationFinding } from '../validate/types.js';
import type { DomainName, ThemeFileResult, WitnessReport } from './types.js';
import { ALL_DOMAINS } from './types.js';
import { WitnessCollector } from './collector.js';

// =============================================================================
// Options
// =============================================================================

/** Options for the witness runner. */
export interface WitnessOptions {
  /** Book type for structural validation (manual, manual-l) */
  bookType?: BookType;

  /** Bonus validation scope (operational) */
  bonus?: BonusScope;

  /** Which domains to run (default: all) */
  domains?: DomainName[];

  /** Editorial lint category filter */
  category?: string;

  /** Document type context for editorial lint */
  documentType?: string;

  /** Register override for editorial lint */
  register?: string;

  /** File scope for editorial/bible/toc lint */
  file?: string;

  /** Scope letter (A, B, C, D) */
  scope?: string;
}

// =============================================================================
// Runner
// =============================================================================

/**
 * Run the full witness validation — all requested domains.
 *
 * Each domain is independently try/caught. If a domain throws,
 * the error is captured and the domain marked as skipped.
 * Other domains continue regardless.
 */
export async function runWitness(
  config: BuildConfig,
  paths: RuntimePaths,
  options: WitnessOptions = {},
): Promise<WitnessReport> {
  const requestedDomains = options.domains ?? [...ALL_DOMAINS];
  const collector = new WitnessCollector('witness', options as Record<string, unknown>, requestedDomains);

  // --- Domain 1: Structural ---
  if (requestedDomains.includes('structural')) {
    await runStructuralDomain(collector, config, paths, options);
  }

  // --- Domain 2: Theme ---
  if (requestedDomains.includes('theme')) {
    await runThemeDomain(collector, config, paths);
  }

  // --- Domain 3: Editorial ---
  if (requestedDomains.includes('editorial')) {
    await runEditorialDomain(collector, config, paths, options);
  }

  // --- Domain 4: Bible ---
  if (requestedDomains.includes('bible')) {
    await runBibleDomain(collector, paths, options);
  }

  // --- Domain 5: TOC ---
  if (requestedDomains.includes('toc')) {
    await runTocDomain(collector, paths, options);
  }

  return collector.finalize(
    config.version ?? 'unknown',
    '3.0.0', // Builder version from package.json
  );
}

// =============================================================================
// Individual Domain Runners
// =============================================================================

async function runStructuralDomain(
  collector: WitnessCollector,
  config: BuildConfig,
  paths: RuntimePaths,
  options: WitnessOptions,
): Promise<void> {
  try {
    const start = Date.now();
    const { runValidation } = await import('../validate/runner.js');
    const result = await runValidation(paths, {
      type: options.bookType,
      bonus: options.bonus,
    }, config.typography, config.page_layout);
    collector.addStructural(result, Date.now() - start);
  } catch (error) {
    collector.failDomain('structural', error as Error);
  }
}

async function runThemeDomain(
  collector: WitnessCollector,
  config: BuildConfig,
  paths: RuntimePaths,
): Promise<void> {
  try {
    const start = Date.now();
    const themesDir = join(paths.sourceDir, 'themes');

    // Discover all theme YAML files
    let themeFiles: string[];
    try {
      const entries = await readdir(themesDir);
      themeFiles = entries
        .filter(f => f.startsWith('cws-') && f.endsWith('-theme.yml'))
        .map(f => join(themesDir, f));
    } catch {
      collector.skipDomain('theme', `Theme directory not found: ${themesDir}`);
      return;
    }

    if (themeFiles.length === 0) {
      collector.skipDomain('theme', 'No theme files found');
      return;
    }

    // Import theme validators + merge utility
    const {
      validateThemeFontSizes,
      validateThemeFontFamilies,
      validateThemeAlignment,
      validateLegalTypography,
      validateThemeColors,
      validateThemeRoles,
      validateThemeStyles,
      validatePageLayout,
      validateThemeElements,
      mergeThemeOverrides,
    } = await import('../validate/theme/index.js');

    // Validate each theme file
    const themes: ThemeFileResult[] = [];

    for (const themeFile of themeFiles) {
      const findings: ValidationFinding[] = [];

      // Look up per-theme overrides (§19)
      const stem = basename(themeFile, '.yml');
      const overrides = config.theme_variants?.[stem];

      // Merge overrides into canonical config (or use canonical as-is)
      const merged = (overrides && config.typography && config.page_layout)
        ? mergeThemeOverrides(config.typography, config.page_layout, overrides)
        : { typography: config.typography, pageLayout: config.page_layout };

      // Typography checks (if config available)
      if (merged.typography) {
        findings.push(...validateThemeFontSizes(themeFile, merged.typography));
        findings.push(...validateThemeFontFamilies(themeFile, merged.typography));
        findings.push(...validateThemeAlignment(themeFile, merged.typography));
        findings.push(...validateThemeColors(themeFile, merged.typography));
        findings.push(...validateThemeRoles(themeFile, merged.typography));
        findings.push(...validateThemeStyles(themeFile, merged.typography));
        findings.push(...validateThemeElements(themeFile, merged.typography));

        // Legal typography only for legal themes
        const name = basename(themeFile);
        if (name.includes('legal') || name.includes('manual-legal')) {
          findings.push(...validateLegalTypography(themeFile, merged.typography));
        }
      }

      // Page layout checks (if config available)
      if (merged.pageLayout) {
        findings.push(...validatePageLayout(themeFile, merged.pageLayout));
      }

      // Compute per-file counts
      const counts: Record<string, number> = {};
      for (const f of findings) {
        counts[f.severity] = (counts[f.severity] ?? 0) + 1;
      }

      themes.push({
        file: basename(themeFile),
        findings,
        counts,
      });
    }

    collector.addTheme(themes, Date.now() - start);
  } catch (error) {
    collector.failDomain('theme', error as Error);
  }
}

async function runEditorialDomain(
  collector: WitnessCollector,
  config: BuildConfig,
  paths: RuntimePaths,
  options: WitnessOptions,
): Promise<void> {
  try {
    const start = Date.now();
    const { runEditorialLint } = await import('../validate/editorial/index.js');
    const result = await runEditorialLint(
      paths,
      {
        file: options.file,
        scope: options.scope,
        check: options.category ?? 'all',
        documentType: options.documentType,
        register: options.register,
      },
      config.legal_language,
      config.tone_and_voice,
      config.industry_specific,
      config.document_types,
      config.asciidoc_publishing,
      config.company_conventions,
    );
    collector.addEditorial(result, Date.now() - start);
  } catch (error) {
    collector.failDomain('editorial', error as Error);
  }
}

async function runBibleDomain(
  collector: WitnessCollector,
  paths: RuntimePaths,
  options: WitnessOptions,
): Promise<void> {
  try {
    const start = Date.now();
    const { runBibleValidation } = await import('../validate/bible/index.js');
    const result = await runBibleValidation(paths, {
      file: options.file,
      scope: options.scope,
    });
    collector.addBible(result, Date.now() - start);
  } catch (error) {
    collector.failDomain('bible', error as Error);
  }
}

async function runTocDomain(
  collector: WitnessCollector,
  paths: RuntimePaths,
  options: WitnessOptions,
): Promise<void> {
  try {
    const start = Date.now();
    const { runPageParse } = await import('../validate/toc/index.js');
    const result = await runPageParse(paths, {
      file: options.file,
      scope: options.scope,
    });
    collector.addToc(result, Date.now() - start);
  } catch (error) {
    collector.failDomain('toc', error as Error);
  }
}
