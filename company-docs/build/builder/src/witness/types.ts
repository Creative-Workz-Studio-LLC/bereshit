/**
 * CWS Manual Builder — Witness Report Schema
 *
 * Unified structured output capturing ALL validation domains.
 * The system bears witness to its own state — observation layer
 * of the CPI-SI feedback loop.
 *
 * Five domains:
 *   structural  — Book structure (base → type → bonus)
 *   editorial   — 57-check editorial lint (7 categories)
 *   theme       — Theme YAML validation (typography, colors, layout)
 *   bible       — Scripture citation verification (KJV/WEB)
 *   toc         — Page markers and TOC pipeline
 *
 * Health maps to Hebrew states:
 *   healthy  = tov (+1)
 *   degraded = yashar (0)
 *   failing  = shavar (-1)
 *
 * "Surely the Lord GOD will do nothing, but he revealeth
 *  his secret unto his servants the prophets." — Amos 3:7
 */

import type { ValidationResult, ValidationFinding } from '../validate/types.js';
import type { EditorialResult } from '../validate/editorial/index.js';
import type { BibleValidationResult } from '../validate/bible/index.js';
import type { PageParseResult } from '../validate/toc/index.js';

// =============================================================================
// Schema Version
// =============================================================================

/** Schema version for forward compatibility. Consumers check this to handle evolution. */
export const WITNESS_SCHEMA_VERSION = '1.0.0';

// =============================================================================
// Health Types
// =============================================================================

/** Overall system health — tov/yashar/shavar. */
export type SystemHealth = 'healthy' | 'degraded' | 'failing';

/** Per-domain health status (all possible values). */
export type DomainHealth = 'pass' | 'warn' | 'fail' | 'skipped';

/** Health for domains that actually executed (never 'skipped'). */
export type ActiveHealth = 'pass' | 'warn' | 'fail';

/** Domain identifiers. */
export type DomainName = 'structural' | 'editorial' | 'theme' | 'bible' | 'toc';

/** All domain names as a constant array (for iteration). */
export const ALL_DOMAINS: readonly DomainName[] = [
  'structural', 'editorial', 'theme', 'bible', 'toc',
] as const;

// =============================================================================
// Metadata
// =============================================================================

/** Metadata about the report itself — who, when, what. */
export interface WitnessMetadata {
  /** Schema version for forward compatibility */
  schemaVersion: string;

  /** ISO 8601 timestamp when report was generated */
  timestamp: string;

  /** Build config version from build.config.yaml */
  configVersion: string;

  /** Builder version from package.json */
  builderVersion: string;

  /** Total wall-clock duration in milliseconds */
  duration: number;

  /** Which domains were requested */
  domainsRequested: DomainName[];

  /** Which domains actually ran (subset of requested) */
  domainsExecuted: DomainName[];

  /** Triggering command: 'witness', 'lint', 'validate' */
  trigger: string;

  /** Options that were passed to the triggering command */
  options: Record<string, unknown>;
}

// =============================================================================
// Domain Summaries
// =============================================================================

/** Summary of a single validation domain — the overview row. */
export interface DomainSummary {
  /** Domain identifier */
  domain: DomainName;

  /** Human-readable domain name */
  label: string;

  /** Overall domain health */
  health: DomainHealth;

  /** Count of findings by severity */
  counts: Record<string, number>;

  /** Duration of this domain's execution in ms */
  duration: number;

  /** Error message if domain failed to execute */
  error?: string;
}

// =============================================================================
// Domain Results — Each wraps its native result type
// =============================================================================

/** Structural validation domain (base → type → bonus). */
export interface StructuralDomainResult {
  domain: 'structural';
  health: ActiveHealth;
  duration: number;
  result: ValidationResult;
}

/** Editorial lint domain (57 checks, 7 categories). */
export interface EditorialDomainResult {
  domain: 'editorial';
  health: ActiveHealth;
  duration: number;
  result: EditorialResult;
}

/** Theme validation domain — per-theme-file findings. */
export interface ThemeFileResult {
  file: string;
  findings: ValidationFinding[];
  counts: Record<string, number>;
}

export interface ThemeDomainResult {
  domain: 'theme';
  health: ActiveHealth;
  duration: number;
  themes: ThemeFileResult[];
}

/** Bible/scripture validation domain. */
export interface BibleDomainResult {
  domain: 'bible';
  health: ActiveHealth;
  duration: number;
  result: BibleValidationResult;
}

/** TOC/pages validation domain. */
export interface TocDomainResult {
  domain: 'toc';
  health: ActiveHealth;
  duration: number;
  result: PageParseResult;
}

/** Skipped domain — requested but could not execute. */
export interface SkippedDomainResult {
  domain: DomainName;
  health: 'skipped';
  duration: 0;
  reason: string;
}

/** Union of all possible domain results. */
export type DomainResult =
  | StructuralDomainResult
  | EditorialDomainResult
  | ThemeDomainResult
  | BibleDomainResult
  | TocDomainResult
  | SkippedDomainResult;

// =============================================================================
// Recommendations
// =============================================================================

/** A single actionable recommendation — where observation becomes action. */
export interface Recommendation {
  /** Priority: 1 (critical) to 5 (nice-to-have) */
  priority: number;

  /** Which domain generated this recommendation */
  domain: DomainName;

  /** Human-readable recommendation */
  message: string;

  /** File path if applicable */
  file?: string;

  /** Suggested command or action */
  action?: string;
}

// =============================================================================
// The Witness Report
// =============================================================================

/** The complete Witness Report — unified observation of system state. */
export interface WitnessReport {
  /** Report metadata */
  metadata: WitnessMetadata;

  /** Overall system health assessment */
  health: SystemHealth;

  /** Per-domain summaries (ordered by severity, worst first) */
  summary: DomainSummary[];

  /** Full domain results — the detail behind the summary */
  domains: DomainResult[];

  /** Aggregated counts across all domains */
  totals: {
    errors: number;
    warnings: number;
    info: number;
    passes: number;
  };

  /** Actionable recommendations sorted by priority */
  recommendations: Recommendation[];
}
