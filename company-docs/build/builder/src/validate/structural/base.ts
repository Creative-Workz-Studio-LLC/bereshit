/**
 * CWS Manual Builder — Base Validation
 *
 * Skeleton checks that always run regardless of book type.
 * Maps to the Makefile's base validation layer.
 *
 * Checks:
 *   - book.adoc exists
 *   - Spine folders exist (A-frontmatter, B-content, C-appendices, D-backmatter)
 *   - Theme file exists
 *   - Shared attributes file exists
 *
 * "Prove all things; hold fast that which is good."
 *  — 1 Thessalonians 5:21
 */

import { existsSync } from 'fs';
import { join } from 'path';
import type { ValidationFinding } from '../types.js';
import { SCOPE_DIRS, type SpineScope } from '../../spine/types.js';

// =============================================================================
// Base Skeleton Checks
// =============================================================================

/**
 * Path configuration for validation.
 */
export interface BaseValidationPaths {
  /** Book root directory */
  bookDir: string;

  /** Theme file path (relative to company-docs/) */
  themeFile: string;

  /** Shared attributes file path */
  attrsFile: string;
}

/**
 * Run base skeleton validation.
 * Returns findings — caller decides what to do with them.
 */
export function validateBase(paths: BaseValidationPaths): ValidationFinding[] {
  const findings: ValidationFinding[] = [];

  // Master assembly
  const masterPath = join(paths.bookDir, 'book.adoc');
  if (existsSync(masterPath)) {
    findings.push({
      severity: 'pass',
      layer: 'base',
      category: 'skeleton',
      message: `Master assembly: ${masterPath}`,
    });
  } else {
    findings.push({
      severity: 'fail',
      layer: 'base',
      category: 'skeleton',
      message: `Master assembly: ${masterPath} not found`,
      path: masterPath,
    });
  }

  // Spine directories
  const scopes: SpineScope[] = ['A', 'B', 'C', 'D'];
  for (const scope of scopes) {
    const dirName = SCOPE_DIRS[scope];
    const dirPath = join(paths.bookDir, dirName);

    if (existsSync(dirPath)) {
      findings.push({
        severity: 'pass',
        layer: 'base',
        category: 'skeleton',
        message: `Spine: ${dirPath}/`,
      });
    } else {
      findings.push({
        severity: 'fail',
        layer: 'base',
        category: 'skeleton',
        message: `Spine: ${dirPath}/ not found`,
        path: dirPath,
      });
    }
  }

  // Theme file
  if (existsSync(paths.themeFile)) {
    findings.push({
      severity: 'pass',
      layer: 'base',
      category: 'skeleton',
      message: `Theme: ${paths.themeFile}`,
    });
  } else {
    findings.push({
      severity: 'fail',
      layer: 'base',
      category: 'skeleton',
      message: `Theme: ${paths.themeFile} not found`,
      path: paths.themeFile,
    });
  }

  // Shared attributes
  if (existsSync(paths.attrsFile)) {
    findings.push({
      severity: 'pass',
      layer: 'base',
      category: 'skeleton',
      message: `Attributes: ${paths.attrsFile}`,
    });
  } else {
    findings.push({
      severity: 'fail',
      layer: 'base',
      category: 'skeleton',
      message: `Attributes: ${paths.attrsFile} not found`,
      path: paths.attrsFile,
    });
  }

  return findings;
}
