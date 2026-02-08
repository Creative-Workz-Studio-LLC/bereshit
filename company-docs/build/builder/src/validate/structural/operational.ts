/**
 * CWS Manual Builder — Operational Validation
 *
 * Full editorial validation (BONUS=operational):
 *   - Frontmatter/Appendices/Backmatter have content
 *   - Assets directory exists
 *   - Preview files exist per Part
 *   - No empty Article directories
 *   - Naming convention consistency across non-content scopes
 *   - Per-Part and per-Article structure details
 *
 * Maps to Makefile's `validate BONUS=operational` layer.
 */

import { existsSync } from 'fs';
import { join } from 'path';
import type { SpineTree, SpineScope } from '../../spine/types.js';
import { SCOPE_DIRS, SCOPE_NAMES } from '../../spine/types.js';
import type { ValidationFinding } from '../types.js';

// =============================================================================
// Constants
// =============================================================================

const LETTER_PATTERN = /^[A-Z]-/;
const NUMBER_PATTERN = /^\d/;

/**
 * Scope-to-preview-file mapping.
 * Scopes A, C, D have single preview files.
 * Scope B uses per-Part preview files (checked separately).
 */
const SCOPE_PREVIEWS: Partial<Record<SpineScope, string>> = {
  A: 'A-frontmatter-preview.adoc',
  C: 'C-appendices-preview.adoc',
  D: 'D-backmatter-preview.adoc',
};

// =============================================================================
// Operational Validation
// =============================================================================

/**
 * Run operational (bonus) validation.
 */
export function validateOperational(
  tree: SpineTree,
  previewDir: string
): ValidationFinding[] {
  const findings: ValidationFinding[] = [];

  // Non-content scope checks
  findings.push(...validateNonContentScopes(tree));

  // Scope preview files (A, C, D)
  findings.push(...validateScopePreviews(previewDir));

  // Assets directory
  if (tree.hasAssets) {
    findings.push({
      severity: 'pass',
      layer: 'bonus',
      category: 'operational',
      message: `Assets: ${join(tree.bookDir, '00-assets')}/`,
    });
  } else {
    findings.push({
      severity: 'warn',
      layer: 'bonus',
      category: 'operational',
      message: `Assets: ${join(tree.bookDir, '00-assets')}/ not found`,
      path: join(tree.bookDir, '00-assets'),
    });
  }

  // Structure details
  findings.push(...validateStructureDetails(tree, previewDir));

  // Naming convention consistency
  findings.push(...validateNamingConsistency(tree));

  return findings;
}

// =============================================================================
// Non-Content Scopes
// =============================================================================

/**
 * Check non-content scopes have content.
 */
function validateNonContentScopes(tree: SpineTree): ValidationFinding[] {
  const findings: ValidationFinding[] = [];
  const scopes: SpineScope[] = ['A', 'C', 'D'];

  for (const scope of scopes) {
    const node = tree.scopes[scope];
    const label = SCOPE_NAMES[scope].toLowerCase();
    const dirPath = join(tree.bookDir, SCOPE_DIRS[scope]);

    if (existsSync(dirPath)) {
      const fileCount = node.files.length;
      if (fileCount > 0) {
        findings.push({
          severity: 'pass',
          layer: 'bonus',
          category: 'operational',
          message: `${label}: ${fileCount} files`,
        });
      } else {
        findings.push({
          severity: 'warn',
          layer: 'bonus',
          category: 'operational',
          message: `${label}: directory exists but empty`,
          path: dirPath,
        });
      }
    } else {
      findings.push({
        severity: 'warn',
        layer: 'bonus',
        category: 'operational',
        message: `${label}: directory not found`,
        path: dirPath,
      });
    }
  }

  return findings;
}

// =============================================================================
// Scope Preview Files
// =============================================================================

/**
 * Check that scope-level preview files exist (A, C, D).
 * Scope B uses per-Part files, checked in validateStructureDetails.
 */
function validateScopePreviews(previewDir: string): ValidationFinding[] {
  const findings: ValidationFinding[] = [];

  for (const [scope, filename] of Object.entries(SCOPE_PREVIEWS)) {
    const filePath = join(previewDir, filename);
    const scopeName = SCOPE_NAMES[scope as SpineScope].toLowerCase();

    if (existsSync(filePath)) {
      findings.push({
        severity: 'pass',
        layer: 'bonus',
        category: 'previews',
        message: `${scopeName} preview: ${filename}`,
      });
    } else {
      findings.push({
        severity: 'warn',
        layer: 'bonus',
        category: 'previews',
        message: `${scopeName} preview: ${filename} not found`,
        path: filePath,
      });
    }
  }

  return findings;
}

// =============================================================================
// Structure Details
// =============================================================================

/**
 * Per-Part and per-Article structure validation.
 */
function validateStructureDetails(
  tree: SpineTree,
  previewDir: string
): ValidationFinding[] {
  const findings: ValidationFinding[] = [];

  for (const part of tree.scopes.B.parts) {
    const previewFile = join(previewDir, `B-part${part.id}-preview.adoc`);
    const hasPreview = existsSync(previewFile);
    const previewStatus = hasPreview ? 'preview: yes' : 'preview: MISSING';

    const details: string[] = [];

    for (const article of part.articles) {
      const sectionCount = article.sections.length;
      if (sectionCount === 0) {
        details.push(`${article.dirname}: EMPTY`);
      } else {
        details.push(`${article.dirname}: ${sectionCount} sections`);
      }
    }

    const severity = hasPreview ? 'info' as const : 'warn' as const;

    findings.push({
      severity,
      layer: 'bonus',
      category: 'structure',
      message: `Part ${part.id} (${part.dirname}): ${part.articles.length} articles, ${previewStatus}`,
      details,
    });
  }

  return findings;
}

// =============================================================================
// Naming Consistency
// =============================================================================

/**
 * Check naming convention consistency across non-content scopes.
 */
function validateNamingConsistency(tree: SpineTree): ValidationFinding[] {
  const findings: ValidationFinding[] = [];
  const scopes: SpineScope[] = ['A', 'C', 'D'];

  for (const scope of scopes) {
    const node = tree.scopes[scope];
    const label = SCOPE_NAMES[scope].toLowerCase();

    let letterCount = 0;
    let numberCount = 0;

    for (const file of node.files) {
      if (LETTER_PATTERN.test(file.filename)) {
        letterCount++;
      } else if (NUMBER_PATTERN.test(file.filename)) {
        numberCount++;
      }
    }

    if (letterCount > 0 && numberCount > 0) {
      findings.push({
        severity: 'warn',
        layer: 'bonus',
        category: 'naming',
        message: `${label}: mixed (${letterCount} letter, ${numberCount} number)`,
      });
    } else if (letterCount > 0) {
      findings.push({
        severity: 'pass',
        layer: 'bonus',
        category: 'naming',
        message: `${label}: letter-based (${letterCount} files)`,
      });
    } else if (numberCount > 0) {
      findings.push({
        severity: 'pass',
        layer: 'bonus',
        category: 'naming',
        message: `${label}: number-based (${numberCount} files)`,
      });
    }
  }

  return findings;
}
