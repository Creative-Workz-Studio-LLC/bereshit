/**
 * CWS Manual Builder — Witness Recommendations
 *
 * Analyzes domain results and generates prioritized, actionable
 * recommendations. This is where observation becomes action —
 * the bridge between the witness report and the CPI-SI feedback loop.
 *
 * Priority levels:
 *   1 — Critical (system can't build)
 *   2 — High (output will be incorrect)
 *   3 — Medium (quality issues)
 *   4 — Low (minor improvements)
 *   5 — Info (nice-to-have)
 *
 * "Where there is no vision, the people perish: but he that
 *  keepeth the law, happy is he." — Proverbs 29:18
 */

import type {
  DomainResult,
  Recommendation,
  StructuralDomainResult,
  EditorialDomainResult,
  ThemeDomainResult,
  BibleDomainResult,
  TocDomainResult,
  SkippedDomainResult,
} from './types.js';

// =============================================================================
// Recommendation Generation
// =============================================================================

/**
 * Generate prioritized recommendations from domain results.
 *
 * Each domain contributes recommendations based on its findings.
 * Results are sorted by priority (1 = most critical).
 */
export function generateRecommendations(domains: DomainResult[]): Recommendation[] {
  const recs: Recommendation[] = [];

  for (const domain of domains) {
    // Skipped domains handled separately — guard narrows the union
    if (domain.health === 'skipped') {
      recs.push(...fromSkipped(domain));
      continue;
    }

    // Now TypeScript knows domain is not SkippedDomainResult
    switch (domain.domain) {
      case 'structural':
        recs.push(...fromStructural(domain));
        break;
      case 'editorial':
        recs.push(...fromEditorial(domain));
        break;
      case 'theme':
        recs.push(...fromTheme(domain));
        break;
      case 'bible':
        recs.push(...fromBible(domain));
        break;
      case 'toc':
        recs.push(...fromToc(domain));
        break;
    }
  }

  // Sort by priority (lowest number = highest priority)
  recs.sort((a, b) => a.priority - b.priority);

  return recs;
}

// =============================================================================
// Per-Domain Recommendation Generators
// =============================================================================

/** Priority 1: Structural failures prevent the book from building. */
function fromStructural(domain: StructuralDomainResult): Recommendation[] {
  const recs: Recommendation[] = [];
  const { result } = domain;

  if (result.counts.fail > 0) {
    recs.push({
      priority: 1,
      domain: 'structural',
      message: `${result.counts.fail} structural failure(s) — book may not build correctly`,
      action: 'cws-build validate --type manual-l',
    });

    // Surface specific failures
    const failures = result.findings.filter(f => f.severity === 'fail');
    for (const f of failures.slice(0, 3)) {
      recs.push({
        priority: 1,
        domain: 'structural',
        message: f.message,
        file: f.path,
      });
    }
  }

  if (result.counts.warn > 0) {
    recs.push({
      priority: 3,
      domain: 'structural',
      message: `${result.counts.warn} structural warning(s) — review naming and conventions`,
      action: 'cws-build validate --type manual-l --bonus operational',
    });
  }

  return recs;
}

/** Priority 3: Editorial warnings indicate style/quality issues. */
function fromEditorial(domain: EditorialDomainResult): Recommendation[] {
  const recs: Recommendation[] = [];
  const { result } = domain;

  if (result.totals.warn === 0 && result.totals.info === 0) return recs;

  // Aggregate by check ID to find the most impactful
  const checkCounts = new Map<string, { warn: number; info: number; label: string }>();

  for (const file of result.files) {
    for (const finding of file.findings) {
      const key = `${finding.check}`;
      const existing = checkCounts.get(key) ?? { warn: 0, info: 0, label: finding.message };
      if (finding.severity === 'warn') existing.warn++;
      else existing.info++;
      checkCounts.set(key, existing);
    }
  }

  // Sort by warning count descending, take top 5
  const sorted = [...checkCounts.entries()]
    .sort(([, a], [, b]) => b.warn - a.warn)
    .slice(0, 5);

  for (const [checkId, counts] of sorted) {
    if (counts.warn > 0) {
      recs.push({
        priority: 3,
        domain: 'editorial',
        message: `Check ${checkId}: ${counts.warn} warning(s) — ${counts.label}`,
        action: `cws-build lint --category editorial`,
      });
    }
  }

  // Overall summary if many warnings
  if (result.totals.warn > 10) {
    recs.push({
      priority: 3,
      domain: 'editorial',
      message: `${result.totals.warn} editorial warnings across ${result.filesChecked} files`,
      action: 'cws-build lint',
    });
  }

  return recs;
}

/** Priority 2: Theme failures mean PDF won't render correctly. */
function fromTheme(domain: ThemeDomainResult): Recommendation[] {
  const recs: Recommendation[] = [];

  for (const theme of domain.themes) {
    const errorCount = (theme.counts['fail'] ?? 0) + (theme.counts['error'] ?? 0);
    const warnCount = theme.counts['warn'] ?? 0;

    if (errorCount > 0) {
      recs.push({
        priority: 2,
        domain: 'theme',
        message: `Theme '${theme.file}' has ${errorCount} error(s) — PDF may render incorrectly`,
        file: theme.file,
        action: 'cws-build validate --type manual-l',
      });
    }

    if (warnCount > 0) {
      recs.push({
        priority: 4,
        domain: 'theme',
        message: `Theme '${theme.file}' has ${warnCount} warning(s)`,
        file: theme.file,
      });
    }
  }

  return recs;
}

/** Priority 4: Bible mismatches affect citation accuracy. */
function fromBible(domain: BibleDomainResult): Recommendation[] {
  const recs: Recommendation[] = [];
  const { result } = domain;

  if (result.counts['not-found'] > 0) {
    recs.push({
      priority: 4,
      domain: 'bible',
      message: `${result.counts['not-found']} verse(s) not found in scripture data`,
      action: 'cws-build lint --bible',
    });
  }

  if (result.counts['mismatch'] > 0) {
    recs.push({
      priority: 4,
      domain: 'bible',
      message: `${result.counts['mismatch']} verse text mismatch(es) — check citations against KJV`,
      action: 'cws-build lint --bible',
    });
  }

  return recs;
}

/** Priority 5: TOC staleness — page numbers need updating. */
function fromToc(domain: TocDomainResult): Recommendation[] {
  const recs: Recommendation[] = [];
  const { result } = domain;

  const errorIssues = result.issues.filter(i => i.severity === 'error');
  const warnIssues = result.issues.filter(i => i.severity === 'warn');

  if (errorIssues.length > 0) {
    recs.push({
      priority: 3,
      domain: 'toc',
      message: `${errorIssues.length} TOC error(s) — page marker issues`,
      action: 'cws-build lint --toc',
    });
  }

  if (warnIssues.length > 0) {
    recs.push({
      priority: 5,
      domain: 'toc',
      message: `${warnIssues.length} page marker warning(s) — consider running toc --fill`,
      action: 'cws-build toc --fill',
    });
  }

  if (result.totals.placeholders > 0) {
    recs.push({
      priority: 5,
      domain: 'toc',
      message: `${result.totals.placeholders} placeholder page number(s) remain`,
      action: 'cws-build toc --fill',
    });
  }

  return recs;
}

/** Skipped domains always get a recommendation to investigate. */
function fromSkipped(domain: SkippedDomainResult): Recommendation[] {
  return [{
    priority: 4,
    domain: domain.domain,
    message: `Domain '${domain.domain}' was skipped: ${domain.reason}`,
  }];
}
