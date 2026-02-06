/**
 * CWS Manual Builder — Validation Runner
 *
 * Orchestrates the three-layer validation system:
 *   Base       → skeleton checks (always runs)
 *   Type       → book type minimums (manual, manual-l)
 *   Bonus      → operational/editorial checks
 *
 * "Prove all things; hold fast that which is good."
 *  — 1 Thessalonians 5:21
 */

import { join } from 'path';
import type { RuntimePaths } from '../config/types.js';
import type { SpineTree } from '../spine/types.js';
import { scanSpine } from '../spine/scanner.js';
import type {
  ValidationResult,
  ValidationOptions,
  ValidationFinding,
  ValidationSeverity,
} from './types.js';
import { validateBase, type BaseValidationPaths } from './base.js';
import { validateManual } from './manual.js';
import { validateManualL } from './manual-l.js';
import { validateOperational } from './operational.js';

// =============================================================================
// Validation Runner
// =============================================================================

/**
 * Run full validation pipeline.
 * Scans the spine, then runs requested validation layers.
 */
export async function runValidation(
  paths: RuntimePaths,
  options: ValidationOptions = {}
): Promise<ValidationResult> {
  const startTime = Date.now();
  const findings: ValidationFinding[] = [];

  // Compute validation paths
  const basePaths: BaseValidationPaths = {
    bookDir: paths.bookDir,
    themeFile: join(paths.sourceDir, 'themes', 'cws-manual-theme.yml'),
    attrsFile: join(paths.sourceDir, '_previews', '_shared-attributes.adoc'),
  };

  // Layer 1: Base (always runs)
  findings.push(...validateBase(basePaths));

  // Scan spine (needed for type and bonus layers)
  let tree: SpineTree | undefined;
  if (options.type || options.bonus) {
    tree = await scanSpine(paths.bookDir);
  }

  // Layer 2: Type-specific validation
  if (options.type && tree) {
    switch (options.type) {
      case 'manual':
        findings.push(...validateManual(tree));
        break;
      case 'manual-l':
        findings.push(...validateManualL(tree));
        break;
    }
  }

  // Layer 3: Bonus validation
  if (options.bonus && tree) {
    switch (options.bonus) {
      case 'operational': {
        const previewDir = join(paths.sourceDir, '_previews');
        findings.push(...validateOperational(tree, previewDir));
        break;
      }
    }
  }

  // Build scope label
  let scopeLabel = 'base';
  if (options.type) {
    scopeLabel = options.type;
  }
  if (options.bonus) {
    scopeLabel = `${scopeLabel}+${options.bonus}`;
  }

  // Count severities
  const counts: Record<ValidationSeverity, number> = {
    pass: 0,
    fail: 0,
    warn: 0,
    info: 0,
  };

  for (const finding of findings) {
    counts[finding.severity]++;
  }

  return {
    valid: counts.fail === 0,
    bookType: options.type,
    bonusScope: options.bonus,
    scopeLabel,
    findings,
    counts,
    duration: Date.now() - startTime,
  };
}
