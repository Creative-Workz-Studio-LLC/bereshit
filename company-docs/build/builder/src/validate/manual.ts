/**
 * CWS Manual Builder — Manual Book Type Validation
 *
 * Validates minimum requirements for a "manual" book type:
 *   - At least 1 Part in B-content
 *   - At least 1 Article in any Part
 *   - At least 1 .adoc content file
 *
 * Maps to Makefile's `validate TYPE=manual` layer.
 */

import type { SpineTree } from '../spine/types.js';
import type { ValidationFinding } from './types.js';

// =============================================================================
// Manual Type Validation
// =============================================================================

/**
 * Validate manual book type minimums.
 * Requires a scanned spine tree.
 */
export function validateManual(tree: SpineTree): ValidationFinding[] {
  const findings: ValidationFinding[] = [];
  const content = tree.scopes.B;

  // Parts check
  const partCount = content.parts.length;
  if (partCount > 0) {
    findings.push({
      severity: 'pass',
      layer: 'type',
      category: 'manual',
      message: `Parts: ${partCount} found`,
    });
  } else {
    findings.push({
      severity: 'fail',
      layer: 'type',
      category: 'manual',
      message: `Parts: none found in ${content.path}`,
      path: content.path,
    });
  }

  // Articles check
  let articleCount = 0;
  for (const part of content.parts) {
    articleCount += part.articles.length;
  }

  if (articleCount > 0) {
    findings.push({
      severity: 'pass',
      layer: 'type',
      category: 'manual',
      message: `Articles: ${articleCount} found across all Parts`,
    });
  } else {
    findings.push({
      severity: 'fail',
      layer: 'type',
      category: 'manual',
      message: 'Articles: none found',
    });
  }

  // Content files check
  let fileCount = 0;
  for (const part of content.parts) {
    for (const article of part.articles) {
      fileCount += article.sections.length;
    }
  }

  if (fileCount > 0) {
    findings.push({
      severity: 'pass',
      layer: 'type',
      category: 'manual',
      message: `Content files: ${fileCount} .adoc files`,
    });
  } else {
    findings.push({
      severity: 'fail',
      layer: 'type',
      category: 'manual',
      message: 'Content files: no .adoc files in content',
    });
  }

  return findings;
}
