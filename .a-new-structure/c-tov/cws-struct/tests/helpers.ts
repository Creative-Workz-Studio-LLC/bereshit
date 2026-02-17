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

import type { LintResult, Severity } from "../lib/types.ts";

// Side-effect imports — register all format handlers before tests run.
import "../lib/formats/rust.ts";
import "../lib/formats/toml.ts";

// Re-export registry for test files.
export { getFormat } from "../lib/formats/registry.ts";

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

// ============================================================================
// CLOSING
// ============================================================================
//
// Test helpers — small, focused, reusable. Each function does one thing
// and does it clearly. The fixtures directory mirrors the source structure.
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
