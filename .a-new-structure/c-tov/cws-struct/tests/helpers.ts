// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/helpers.ts
// key:     B-tov-cws-struct-tests-helpers
// title:   CWS Struct — Test Helpers
// type:    Code (Test Support)
// version: b-01.50
// created: 2026-02-17
// authors: Nova Dawn (CPI-SI)
// purpose: Shared test utilities for format handler tests.
//          Provides fixture path resolution and result filtering.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import type { LintResult, Severity } from "../lib/foundation/mod.ts";
import { matchRule, getByCode, ERROR_CODES } from "../lib/data/errors.ts";
import type { ErrorCodeEntry } from "../lib/data/types.ts";
import type {
  BaseFileContext,
  BlockPosition,
  SubsectionRange,
} from "../lib/shared/types.ts";

// Side-effect imports — register all format handlers before tests run.
import "../lib/handlers/go.ts";
import "../lib/handlers/rust.ts";
import "../lib/handlers/toml.ts";

// Re-export registry for test files.
export { getFormat } from "../lib/engine/mod.ts";

import { getFormat as _getFormat } from "../lib/engine/mod.ts";
import type { FormatHandler } from "../lib/foundation/mod.ts";

/**
 * Get a format handler, throwing if not registered.
 * Use this INSIDE test bodies (not at module level) to avoid top-level throws
 * that can cause Deno to silently drop entire test files during parallel loading.
 */
export function requireFormat(name: string): FormatHandler {
  const handler = _getFormat(name);
  if (!handler) {
    throw new Error(`${name} format handler not registered — check imports in helpers.ts`);
  }
  return handler;
}

/**
 * Return a lazy Proxy that initializes the handler on first property access.
 * Use at module level instead of `let` + setup test — enables `--filter` targeting
 * without requiring the setup test to run first.
 *
 * @example const go = lazyFormat("go");
 *          // go.lint(...) works — proxy calls requireFormat("go") on first access
 */
export function lazyFormat(name: string): FormatHandler {
  let cached: FormatHandler | undefined;
  return new Proxy({} as FormatHandler, {
    get(_target, prop, receiver) {
      if (!cached) cached = requireFormat(name);
      const val = Reflect.get(cached, prop, receiver);
      return typeof val === "function" ? (val as (...args: unknown[]) => unknown).bind(cached) : val;
    },
  });
}

// Re-export error catalog functions for test assertions.
export { matchRule, getByCode, getByRule, getByLayer, getByCategory } from "../lib/data/errors.ts";
export type { ErrorCodeEntry } from "../lib/data/types.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Fixture resolution
// ---------------------------------------------------------------------------

/**
 * Resolve a fixture path relative to the test file calling this.
 * Usage: `fixture("rust/valid-library.rs")`
 *
 * Uses import.meta.url of the calling module for proper Deno resolution.
 */
export function fixture(relativePath: string): string {
  return new URL(`./fixtures/${relativePath}`, import.meta.url).pathname;
}

// ---------------------------------------------------------------------------
// Result filtering
// ---------------------------------------------------------------------------

/** Filter results to only errors. */
export function errors(results: LintResult[]): LintResult[] {
  return results.filter((r) => r.severity === "error");
}

/** Filter results to only warnings. */
export function warnings(results: LintResult[]): LintResult[] {
  return results.filter((r) => r.severity === "warn");
}

/** Filter results to only infos. */
export function infos(results: LintResult[]): LintResult[] {
  return results.filter((r) => r.severity === "info");
}

/** Filter results by severity. */
export function bySeverity(results: LintResult[], severity: Severity): LintResult[] {
  return results.filter((r) => r.severity === severity);
}

/** Filter results by rule name (partial match). */
export function byRule(results: LintResult[], ruleFragment: string): LintResult[] {
  return results.filter((r) => r.rule.includes(ruleFragment));
}

/** Check if any result matches a rule pattern. */
export function hasRule(results: LintResult[], ruleFragment: string): boolean {
  return results.some((r) => r.rule.includes(ruleFragment));
}

/** Check if any result contains a message substring. */
export function hasMessage(results: LintResult[], messageFragment: string): boolean {
  return results.some((r) => r.message.includes(messageFragment));
}

// ---------------------------------------------------------------------------
// Error code resolution
// ---------------------------------------------------------------------------

/**
 * Resolve a LintResult's rule string to its ErrorCodeEntry.
 * Uses matchRule() for dynamic pattern matching (e.g., "identity/Pragma/I1.key"
 * matches "identity/{{varName}}/{{group}}.{{field}}" → CWS-R10-011).
 *
 * Returns undefined if the rule has no catalog entry (e.g., info-only rules
 * or rules not yet cataloged).
 */
export function resolveErrorCode(result: LintResult): ErrorCodeEntry | undefined {
  return matchRule(result.rule);
}

/**
 * Filter results to those whose rule resolves to a specific CWS error code.
 *
 * @example byErrorCode(results, "CWS-R50-020") — all block/order violations
 */
export function byErrorCode(results: LintResult[], code: string): LintResult[] {
  return results.filter((r) => {
    const entry = matchRule(r.rule);
    return entry !== undefined && entry.code === code;
  });
}

/**
 * Check if any result resolves to a specific CWS error code.
 *
 * @example hasErrorCode(results, "CWS-R50-020") — any block order violation?
 */
export function hasErrorCode(results: LintResult[], code: string): boolean {
  return results.some((r) => {
    const entry = matchRule(r.rule);
    return entry !== undefined && entry.code === code;
  });
}

/**
 * Filter results by detection layer (0=R50, 1=R25, 2=R10, 3=R05).
 * Only includes results whose rule resolves to a cataloged error code.
 */
export function byLayer(results: LintResult[], layer: 0 | 1 | 2 | 3): LintResult[] {
  return results.filter((r) => {
    const entry = matchRule(r.rule);
    return entry !== undefined && entry.layer === layer;
  });
}

/**
 * Filter results by error category (e.g., "block", "identity", "directive").
 */
export function byCategory(results: LintResult[], category: string): LintResult[] {
  return results.filter((r) => {
    const entry = matchRule(r.rule);
    return entry !== undefined && entry.category === category;
  });
}

/**
 * Build a summary map of error code → count for debugging.
 * Uncataloged rules appear under the key "UNCATALOGED".
 *
 * @example errorCodeSummary(results) → Map { "CWS-R50-020" => 1, "CWS-R25-010" => 3 }
 */
export function errorCodeSummary(results: LintResult[]): Map<string, number> {
  const summary = new Map<string, number>();
  for (const r of results) {
    const entry = matchRule(r.rule);
    const key = entry ? entry.code : "UNCATALOGED";
    summary.set(key, (summary.get(key) ?? 0) + 1);
  }
  return summary;
}

/**
 * Format error code summary as a human-readable string for test diagnostics.
 * Shows each code with its count and layer name.
 *
 * @example errorCodeReport(results) → "CWS-R50-020 (R50 block) ×1, CWS-R25-010 (R25 identity) ×3"
 */
export function errorCodeReport(results: LintResult[]): string {
  const summary = errorCodeSummary(results);
  const parts: string[] = [];
  for (const [code, count] of summary) {
    if (code === "UNCATALOGED") {
      parts.push(`UNCATALOGED ×${count}`);
    } else {
      const entry = getByCode(code);
      const label = entry ? `${entry.layerName} ${entry.category}` : "???";
      parts.push(`${code} (${label}) ×${count}`);
    }
  }
  return parts.join(", ");
}

// ---------------------------------------------------------------------------
// Mock builders — minimal contexts for unit testing
// ---------------------------------------------------------------------------

/**
 * Build a minimal BaseFileContext with sensible defaults for unit testing.
 * Override any field via the `overrides` parameter.
 *
 * Defaults: empty 10-line file, no blocks, no directives, not a template.
 */
export function createMockContext(
  overrides?: Partial<BaseFileContext>,
): BaseFileContext {
  const defaults: BaseFileContext = {
    filePath: "test-fixture.rs",
    lines: Array.from({ length: 10 }, (_, i) => `// line ${i + 1}`),
    isTemplate: false,
    hasAnyOmni: false,
    hasAnyBlock: false,
    blocks: [],
    directives: new Map(),
    subtype: null,
    typing: null,
  };
  return { ...defaults, ...overrides };
}

/**
 * Build BlockPosition[] from simplified input.
 * Saves tests from manually constructing the full interface.
 *
 * @example createBlockPositions([
 *   { name: "METADATA", start: 1, end: 20 },
 *   { name: "SETUP", start: 22, end: 50 },
 * ])
 */
export function createBlockPositions(
  blocks: { name: string; start: number; end: number }[],
): BlockPosition[] {
  return blocks.map((b) => ({
    name: b.name,
    line: b.start,
    endLine: b.end,
  }));
}

/**
 * Build SubsectionRange[] from simplified input.
 * Uses array indices (0-based) matching what getSubsectionRanges() returns.
 *
 * @example createSubsectionRanges([
 *   { tag: "Imports", start: 0, end: 5 },
 *   { tag: "Constants", start: 5, end: 12 },
 * ])
 */
export function createSubsectionRanges(
  ranges: { tag: string; start: number; end: number }[],
): SubsectionRange[] {
  return ranges.map((r) => ({
    tag: r.tag,
    startIdx: r.start,
    endIdx: r.end,
  }));
}

// ---------------------------------------------------------------------------
// Witness registry — error code → test coverage tracking
// ---------------------------------------------------------------------------
//
// Convention: Tests that witness a specific error code include [CWS-XXX-NNN]
// in their test name. This enables:
//   deno test --filter='CWS-R50-020'   → run witnesses for that code
//   G13 gate: every code has a witness  → no untested error paths
//
// The registry is populated by witnessFor() calls at test registration time.
// It's a runtime structure — static analysis (scripts/witness.ts) also works.
// ---------------------------------------------------------------------------

/** Registry of error codes → test names that cover them. */
const WITNESS_REGISTRY = new Map<string, string[]>();

/**
 * Register this test as a witness for an error code.
 * Call at the top of any test that exercises a specific catalog entry.
 *
 * @example
 *   Deno.test("[CWS-R50-020] missing block fires error", () => {
 *     witnessFor("CWS-R50-020", "[CWS-R50-020] missing block fires error");
 *     // ... test body
 *   });
 */
export function witnessFor(code: string, testName: string): void {
  const existing = WITNESS_REGISTRY.get(code);
  if (existing) {
    if (!existing.includes(testName)) existing.push(testName);
  } else {
    WITNESS_REGISTRY.set(code, [testName]);
  }
}

/** Get all test names that witness a specific error code. */
export function witnessesFor(code: string): string[] {
  return WITNESS_REGISTRY.get(code) ?? [];
}

/**
 * Get all error codes in the catalog that have no witness tests.
 * The G13 gate calls this — empty array = full coverage.
 */
export function unwitnessedCodes(): string[] {
  const allCodes = [...ERROR_CODES.values()].map((e) => e.code);
  return allCodes.filter((code: string) => !WITNESS_REGISTRY.has(code));
}

/**
 * Get the `deno test --filter` command to run witnesses for a code.
 * Useful in diagnostic output — shows exactly how to reproduce.
 */
export function diagnosticCommand(code: string): string {
  return `deno test --filter='${code}'`;
}

/** Get the total witness count (unique code→test pairs). */
export function witnessCount(): { codes: number; tests: number } {
  let tests = 0;
  for (const names of WITNESS_REGISTRY.values()) {
    tests += names.length;
  }
  return { codes: WITNESS_REGISTRY.size, tests };
}

/** Reset the witness registry (for testing the witness system itself). */
export function clearWitnessRegistry(): void {
  WITNESS_REGISTRY.clear();
}

// ============================================================================
// CLOSING
// ============================================================================
//
// Test helpers — small, focused, reusable. Each function does one thing
// and does it clearly. The fixtures directory mirrors the source structure.
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
