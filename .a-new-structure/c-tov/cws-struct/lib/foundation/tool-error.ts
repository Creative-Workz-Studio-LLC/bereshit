// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/foundation/tool-error.ts
// key:     B-tov-cws-struct-lib-foundation-tool-error
// title:   CWS Struct — Structured Tool Error
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-22
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
// biblical_foundation: "The light shineth in darkness; and the darkness
//   comprehended it not." — John 1:5
//   (Structured errors bring light to infrastructure failures.)
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { getByCode } from "../data/errors.ts";

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
    const entry = getByCode(code);
    let message: string;

    if (entry) {
      // Replace {{placeholder}} with context values
      message = entry.messageTemplate.replace(
        /\{\{(\w+)\}\}/g,
        (_, key) => context[key] ?? `{{${key}}}`,
      );
      message = `[${code}] ${message}`;
    } else {
      // Unregistered code — shouldn't happen, but be graceful
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
    return getByCode(this.code)?.suggestionTemplate;
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
// "The light shineth in darkness; and the darkness comprehended it not."
//   — John 1:5
// ============================================================================
