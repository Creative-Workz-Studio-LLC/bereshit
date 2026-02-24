// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/foundation/tool-error.ts
// key:     B-tov-cws-struct-lib-foundation-tool-error
// title:   CWS Struct — Structured Tool Error
// type:    Code (Library)
// version: a-02.00
// created: 2026-02-22
// updated: 2026-02-24
// authors: Nova Dawn (CPI-SI)
// purpose: Structured error class for tool infrastructure failures. Every
//          throw site in the tooling now carries a CWS-T00-xxx code, a
//          formatted message from the catalog template, and machine-readable
//          context fields. No more bare "throw new Error(msg)" in the dark.
//
//          ToolError extends Error — existing catch blocks still work.
//          The code field enables --why mode, --debug tracing, and log
//          correlation. The context record enables structured diagnostics.
//
//          a-02.00: Lazy catalog binding via setter injection. The error
//          catalog (data/errors.ts) is bound at runtime via bindErrorCatalog()
//          instead of a static import, breaking the foundation→data circular
//          dependency at import time. The catalog is always bound before any
//          ToolError is constructed — data/mod.ts calls bindErrorCatalog()
//          during module evaluation.
//
// biblical_foundation: "The light shineth in darkness; and the darkness
//   comprehended it not." — John 1:5
//   (Structured errors bring light to infrastructure failures.)
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// ---------------------------------------------------------------------------
// Lazy catalog binding — breaks foundation→data circular dependency
// ---------------------------------------------------------------------------
//
// Previously: import { getByCode } from "../data/errors.ts"
// Problem: foundation/ importing from data/ creates a bidirectional cycle
//   since data/ also imports from foundation/ (ToolError, pipeline, cache).
//
// Solution: setter injection. data/mod.ts binds the catalog lookup after
// both modules finish loading. By the time any ToolError is constructed
// (always inside function bodies, never at module level), the catalog is
// available.

/** Error catalog entry shape — what we need from the lookup. */
interface CatalogEntry {
  messageTemplate: string;
  suggestionTemplate: string;
}

/** Catalog lookup function type. */
type ErrorCatalogLookup = (code: string) => CatalogEntry | undefined;

/** Bound catalog lookup — set by bindErrorCatalog() during app init. */
let _getByCode: ErrorCatalogLookup | undefined;

/**
 * Bind the error catalog lookup function.
 *
 * Called by data/mod.ts during module evaluation. This wires up the
 * catalog without creating a static import from foundation/ → data/.
 *
 * @param fn The getByCode function from data/errors.ts
 */
export function bindErrorCatalog(fn: ErrorCatalogLookup): void {
  _getByCode = fn;
}

// ============================================================================
// BODY
// ============================================================================

/**
 * Structured tool error with CWS-T00-xxx error code.
 *
 * Replaces bare `throw new Error(msg)` throughout tool infrastructure.
 * The error code links to the catalog for --why mode explanations and
 * structured logging. The context record provides substitution values
 * for the message template.
 *
 * @example
 * ```typescript
 * throw new ToolError("CWS-T00-001", { schemaId: "code/forms/module/go.jsonc" });
 * // → "[CWS-T00-001] Schema not found: code/forms/module/go.jsonc"
 * ```
 *
 * @example
 * ```typescript
 * try {
 *   await pipeline.getText(schemaId);
 * } catch (e) {
 *   if (e instanceof ToolError) {
 *     console.error(`Tool error ${e.code}: ${e.message}`);
 *   }
 * }
 * ```
 */
export class ToolError extends Error {
  /** CWS-T00-xxx error code from the catalog. */
  readonly code: string;

  /** Context fields used for message template substitution. */
  readonly context: Readonly<Record<string, string>>;

  constructor(code: string, context: Record<string, string> = {}) {
    const entry = _getByCode?.(code);
    let message: string;

    if (entry) {
      // Replace {{placeholder}} with context values
      message = entry.messageTemplate.replace(
        /\{\{(\w+)\}\}/g,
        (_, key) => context[key] ?? `{{${key}}}`,
      );
      message = `[${code}] ${message}`;
    } else {
      // Catalog not bound yet or unregistered code — format from context
      const detail = Object.entries(context)
        .map(([k, v]) => `${k}=${v}`)
        .join(", ");
      message = `[${code}] Tool error${detail ? `: ${detail}` : ""}`;
    }

    super(message);
    this.name = "ToolError";
    this.code = code;
    this.context = Object.freeze({ ...context });
  }

  /**
   * Get the suggestion for this error from the catalog.
   *
   * @returns Suggestion string, or undefined if code not in catalog
   */
  get suggestion(): string | undefined {
    return _getByCode?.(this.code)?.suggestionTemplate;
  }
}

// ============================================================================
// CLOSING
// ============================================================================
//
// ToolError — structured light in the infrastructure darkness.
// Every CWS-T00-xxx code traces to the catalog. Every context field
// enables diagnostics. Every throw site is now visible.
//
// a-02.00: Lazy catalog binding via bindErrorCatalog() breaks the
// foundation→data import cycle. The pattern follows the existing lazy
// import precedent (mod.ts uses dynamic import for database.ts).
//
// "The light shineth in darkness; and the darkness comprehended it not."
//   — John 1:5
// ============================================================================
