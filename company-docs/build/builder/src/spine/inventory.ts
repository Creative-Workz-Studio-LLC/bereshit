/**
 * CWS Manual Builder — Inventory Builder
 *
 * Takes a SpineTree (from the scanner) and builds structured
 * inventory reports — the parts list that tells you exactly
 * what the book contains.
 *
 * "For which of you, intending to build a tower, sitteth not down first,
 *  and counteth the cost?" — Luke 14:28
 */

import type {
  SpineTree,
  SpineScope,
  InventoryReport,
  InventorySummary,
  PartInventory,
} from './types.js';
import { SCOPE_NAMES } from './types.js';

// =============================================================================
// Inventory Building
// =============================================================================

/**
 * Build a complete inventory report from a spine tree.
 */
export function buildInventory(tree: SpineTree): InventoryReport {
  const summary = buildSummary(tree);
  const parts = buildPartsList(tree);
  const scopeFiles = buildScopeFiles(tree);

  return { summary, parts, scopeFiles };
}

// =============================================================================
// Summary
// =============================================================================

/**
 * Build summary statistics from the spine tree.
 */
function buildSummary(tree: SpineTree): InventorySummary {
  const scopes: SpineScope[] = ['A', 'B', 'C', 'D'];
  let scopeCount = 0;
  let partCount = 0;
  let articleCount = 0;
  let sectionCount = 0;
  let totalFiles = 0;
  let totalSize = 0;
  const emptyArticles: string[] = [];

  for (const scope of scopes) {
    const node = tree.scopes[scope];

    // Count scope if it has content
    if (node.files.length > 0 || node.parts.length > 0) {
      scopeCount++;
    }

    // Scope files (non-content)
    for (const file of node.files) {
      totalFiles++;
      totalSize += file.size;
    }

    // Parts and articles (content scope)
    for (const part of node.parts) {
      partCount++;

      for (const article of part.articles) {
        articleCount++;

        if (article.sections.length === 0) {
          emptyArticles.push(`${part.dirname}/${article.dirname}`);
        }

        for (const section of article.sections) {
          sectionCount++;
          totalFiles++;
          totalSize += section.size;
        }
      }
    }
  }

  return {
    scopeCount,
    partCount,
    articleCount,
    sectionCount,
    totalFiles,
    totalSize,
    emptyArticles,
  };
}

// =============================================================================
// Parts List
// =============================================================================

/**
 * Build per-part inventory entries.
 */
function buildPartsList(tree: SpineTree): PartInventory[] {
  return tree.scopes.B.parts.map(part => {
    const articles = part.articles.map(article => {
      const count = article.sections.length;
      let letterRange = '';

      if (count > 0) {
        const first = article.sections[0]?.letter || '?';
        const last = article.sections[count - 1]?.letter || '?';
        letterRange = count === 1 ? first : `${first}–${last}`;
      }

      return {
        id: article.id,
        name: article.name,
        sectionCount: count,
        letterRange,
      };
    });

    return {
      id: part.id,
      name: part.name,
      articleCount: part.articles.length,
      sectionCount: articles.reduce((sum, a) => sum + a.sectionCount, 0),
      articles,
    };
  });
}

// =============================================================================
// Scope Files
// =============================================================================

/**
 * Build scope file counts for non-content scopes.
 */
function buildScopeFiles(tree: SpineTree): Record<string, number> {
  const result: Record<string, number> = {};

  for (const scope of ['A', 'C', 'D'] as SpineScope[]) {
    const node = tree.scopes[scope];
    result[SCOPE_NAMES[scope]] = node.files.length;
  }

  return result;
}

// =============================================================================
// Display Helpers
// =============================================================================

/**
 * Format inventory as a human-readable tree string.
 */
export function formatInventoryTree(report: InventoryReport): string {
  const lines: string[] = [];

  lines.push('Book Inventory');
  lines.push('==============');
  lines.push('');

  // Non-content scopes
  for (const [scope, count] of Object.entries(report.scopeFiles)) {
    lines.push(`  ${scope}: ${count} files`);
  }
  lines.push('');

  // Content parts
  lines.push('  Content:');
  for (const part of report.parts) {
    lines.push(`    Part ${part.id} (${part.name}): ${part.articleCount} articles, ${part.sectionCount} sections`);

    for (const article of part.articles) {
      if (article.sectionCount === 0) {
        lines.push(`      Art. ${article.id} (${article.name}): EMPTY`);
      } else {
        lines.push(`      Art. ${article.id} (${article.name}): ${article.sectionCount} sections (${article.letterRange})`);
      }
    }
  }

  lines.push('');
  lines.push('  Summary:');
  lines.push(`    ${report.summary.partCount} parts, ${report.summary.articleCount} articles, ${report.summary.sectionCount} sections`);
  lines.push(`    ${report.summary.totalFiles} total files (${formatSize(report.summary.totalSize)})`);

  if (report.summary.emptyArticles.length > 0) {
    lines.push(`    ${report.summary.emptyArticles.length} empty articles`);
  }

  return lines.join('\n');
}

/**
 * Simple size formatting for inventory display.
 */
function formatSize(bytes: number): string {
  if (bytes < 1024) return `${bytes} B`;
  if (bytes < 1024 * 1024) return `${(bytes / 1024).toFixed(1)} KB`;
  return `${(bytes / (1024 * 1024)).toFixed(1)} MB`;
}
