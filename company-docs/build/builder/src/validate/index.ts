/**
 * CWS Manual Builder — Validation Module
 *
 * Re-exports all validation components.
 */

// Types
export type {
  BookType,
  BonusScope,
  ValidationSeverity,
  ValidationFinding,
  ValidationResult,
  ValidationOptions,
} from './types.js';

// Runner (primary entry point)
export { runValidation } from './runner.js';

// Individual validators (for direct use)
export { validateBase, type BaseValidationPaths } from './base.js';
export { validateManual } from './manual.js';
export { validateManualL } from './manual-l.js';
export { validateOperational } from './operational.js';
