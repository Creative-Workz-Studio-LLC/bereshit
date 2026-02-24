// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/foundation/types.ts
// key:     B-tov-cws-struct-lib-foundation-types
// title:   CWS Struct — Shared Types & Result Constructors
// type:    Code (Library)
// version: a-03.00
// created: 2026-02-11
// updated: 2026-02-24
// authors: Nova Dawn (CPI-SI)
// purpose: Re-exports pure type definitions from lib/types/ (canonical source)
//          and provides runtime result constructors + global state management.
//
//          a-03.00: Type definitions extracted to lib/types/ during Phase 0
//          reorganization. This file now re-exports types for backward
//          compatibility and keeps runtime-only code: policySeverity,
//          error/warn/info constructors, summarize, global policy/focus state.
//
// biblical_foundation: "A just weight and balance are the LORD's: all the
//   weights of the bag are his work." — Proverbs 16:11
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// ---------------------------------------------------------------------------
// Type re-exports — canonical source is lib/types/
// ---------------------------------------------------------------------------
//
// All pure type definitions live in lib/types/. This file re-exports them
// for backward compatibility — existing imports from "./types.ts" or
// "../foundation/types.ts" continue to work unchanged.

export type {
  Severity,
  LintPolicy,
  LintLayer,
  BlockName,
  LintGridOptions,
  FixAction,
  FixSuggestion,
  LintResult,
  LintSummary,
  AnatomyBlock,
  AnatomySection,
  ParsedAnatomy,
  TransformOptions,
  InspectBlock,
  InspectSection,
  SubsectionDef,
  InspectContent,
  InspectResult,
  FormatHandler,
  CliOptions,
} from "../types/mod.ts";

// Types needed by runtime functions in this file
import type {
  Severity,
  LintPolicy,
  LintResult,
  FixSuggestion,
  LintSummary,
  LintGridOptions,
} from "../types/mod.ts";

import type { HealthScore } from "../types/mod.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Lint Policy — runtime functions
// ---------------------------------------------------------------------------
//
// Maps to CPI-SI's 3 keys:
//   -1 (strict)   — toward anchor — production standard, tighten severity
//    0 (balanced)  — maintain — development default, severity as-declared
//   +1 (growth)    — toward fullness — scaffold/learning, loosen severity
//
// Policy shifts severity one step per level:
//   strict:   info → warn,  warn → error,  error → error
//   balanced: info → info,  warn → warn,   error → error  (identity)
//   growth:   info → info,  warn → info,   error → warn

/**
 * Map a base severity through a policy lens.
 *
 * The policy shifts severity by one step on the ternary scale:
 * - strict (-1): tighten (warn → error, info → warn)
 * - balanced (0): identity (no change)
 * - growth (+1): loosen (error → warn, warn → info)
 *
 * Floor: info never becomes nothing. Ceiling: error stays error under strict.
 * This only applies to form-aware checks — structural checks bypass policy.
 */
export function policySeverity(base: Severity, policy: LintPolicy): Severity {
  if (policy === "balanced") return base;

  if (policy === "strict") {
    // Tighten: one step toward error
    switch (base) {
      case "info": return "warn";
      case "warn": return "error";
      case "error": return "error";
    }
  }

  // growth: one step toward info
  switch (base) {
    case "error": return "warn";
    case "warn": return "info";
    case "info": return "info";
  }
}

// ---------------------------------------------------------------------------
// Global Policy State — set by CLI, read by handlers
// ---------------------------------------------------------------------------
//
// The FormatHandler.lint() contract takes only (filePath) — no options bag.
// Until the interface evolves, policy flows through module-level state.
// One policy per session. CLI sets it before dispatching to handlers.

let _globalPolicy: LintPolicy = "balanced";

/** Set the session-wide ternary lint policy. Call before dispatching to handlers. */
export function setGlobalPolicy(p: LintPolicy): void { _globalPolicy = p; }

/** Get the session-wide ternary lint policy. Handlers call this in form-aware checks. */
export function getGlobalPolicy(): LintPolicy { return _globalPolicy; }

// ---------------------------------------------------------------------------
// Lint Grid Focus — block × layer slicing (global state)
// ---------------------------------------------------------------------------

let _lintFocus: LintGridOptions | undefined;

/** Set the session-wide lint focus. Call before dispatching to handlers. */
export function setLintFocus(opts?: LintGridOptions): void { _lintFocus = opts; }

/** Get the session-wide lint focus. Handlers call this in runLintGrid(). */
export function getLintFocus(): LintGridOptions | undefined { return _lintFocus; }

// ---------------------------------------------------------------------------
// Result constructors — convenience builders
// ---------------------------------------------------------------------------

/** Options for result constructors. Pass line, fix, and/or block as needed. */
interface ResultOpts {
  line?: number;
  fix?: FixSuggestion;
  /** Which block this result belongs to. */
  block?: string;
  /** Which container (subsection/zone) within the block. */
  container?: string;
}

export function error(
  file: string, rule: string, message: string, opts?: ResultOpts | FixSuggestion,
): LintResult {
  const o = normalizeOpts(opts);
  return { file, severity: "error", rule, message, ...o };
}

export function warn(
  file: string, rule: string, message: string, opts?: ResultOpts | FixSuggestion,
): LintResult {
  const o = normalizeOpts(opts);
  return { file, severity: "warn", rule, message, ...o };
}

export function info(
  file: string, rule: string, message: string, opts?: ResultOpts | FixSuggestion,
): LintResult {
  const o = normalizeOpts(opts);
  return { file, severity: "info", rule, message, ...o };
}

/** Normalize opts: accept either ResultOpts or bare FixSuggestion for backward compat. */
function normalizeOpts(opts?: ResultOpts | FixSuggestion): Partial<LintResult> {
  if (!opts) return {};
  // Duck-type: FixSuggestion has 'description' + (toml | actions), ResultOpts has 'line' or 'fix'
  if ("description" in opts && ("toml" in opts || "actions" in opts)) {
    return { fix: opts as FixSuggestion };
  }
  const result: Partial<LintResult> = {};
  const ro = opts as ResultOpts;
  if (ro.line !== undefined) result.line = ro.line;
  if (ro.fix !== undefined) result.fix = ro.fix;
  if (ro.block !== undefined) result.block = ro.block;
  if (ro.container !== undefined) result.container = ro.container;
  return result;
}

/** Tally a results array into a summary. Single-pass counting. */
export function summarize(
  file: string,
  results: LintResult[],
  health?: HealthScore,
): LintSummary {
  let errors = 0, warnings = 0, infos = 0;
  for (const r of results) {
    switch (r.severity) {
      case "error": errors++; break;
      case "warn": warnings++; break;
      case "info": infos++; break;
    }
  }
  return { file, errors, warnings, infos, results, ...(health ? { health } : {}) };
}

// ============================================================================
// CLOSING
// ============================================================================
//
// Type definitions → lib/types/ (single source of truth).
// Runtime functions → here (policy, constructors, summarize, global state).
// Backward-compatible re-exports → all existing import paths still work.
//
// "A just weight and balance are the LORD's." — Proverbs 16:11
// ============================================================================
