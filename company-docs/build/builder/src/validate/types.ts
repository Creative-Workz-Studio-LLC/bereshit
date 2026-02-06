/**
 * CWS Manual Builder — Validation Types
 *
 * Type definitions for the book structure validation system.
 * Mirrors the Makefile's three-layer validation:
 *   Base       → skeleton checks (always runs)
 *   Type       → book type minimums (manual, manual-l)
 *   Bonus      → operational/editorial checks
 *
 * "Prove all things; hold fast that which is good."
 *  — 1 Thessalonians 5:21
 */

// =============================================================================
// Book Types
// =============================================================================

/**
 * Supported book types for validation.
 * Maps to Makefile's TYPE= parameter.
 */
export type BookType = 'manual' | 'manual-l';

/**
 * Bonus validation scopes.
 * Maps to Makefile's BONUS= parameter.
 */
export type BonusScope = 'operational';

// =============================================================================
// Validation Results
// =============================================================================

/**
 * Severity levels for validation findings.
 */
export type ValidationSeverity = 'pass' | 'fail' | 'warn' | 'info';

/**
 * A single validation finding.
 */
export interface ValidationFinding {
  /** Severity level */
  severity: ValidationSeverity;

  /** Validation layer that produced this finding */
  layer: 'base' | 'type' | 'bonus';

  /** Category within the layer (e.g., "skeleton", "legal-hierarchy") */
  category: string;

  /** Human-readable message */
  message: string;

  /** Related file or directory path (if applicable) */
  path?: string;

  /** Additional details (non-conforming items, etc.) */
  details?: string[];
}

/**
 * Complete validation result.
 */
export interface ValidationResult {
  /** Whether validation passed (no FAIL findings) */
  valid: boolean;

  /** Book type that was validated (if specified) */
  bookType?: BookType;

  /** Bonus scope that was validated (if specified) */
  bonusScope?: BonusScope;

  /** Scope label for display (e.g., "manual-l+operational") */
  scopeLabel: string;

  /** All findings, in order */
  findings: ValidationFinding[];

  /** Count of each severity */
  counts: Record<ValidationSeverity, number>;

  /** Validation duration in milliseconds */
  duration: number;
}

// =============================================================================
// Validation Options
// =============================================================================

/**
 * Options for running validation.
 */
export interface ValidationOptions {
  /** Book type to validate against (omit for base-only) */
  type?: BookType;

  /** Bonus validation scope */
  bonus?: BonusScope;
}
