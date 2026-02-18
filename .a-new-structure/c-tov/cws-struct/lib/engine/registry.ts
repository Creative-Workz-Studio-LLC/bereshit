// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/engine/registry.ts
// key:     B-tov-cws-struct-lib-engine-registry
// title:   CWS Struct — Format Registry
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-11
// authors: Nova Dawn (CPI-SI)
// purpose: Central registry for format handlers. New formats register here.
//          The tool grows by addition — add a handler, register it, done.
//          No modification to CLI or core logic needed.
//
// biblical_foundation: "For as the body is one, and hath many members, and
//   all the members of that one body, being many, are one body." — 1 Cor 12:12
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import type { FormatHandler } from "../foundation/mod.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Registry — the single map of all known formats
// ---------------------------------------------------------------------------

const registry = new Map<string, FormatHandler>();

/** Register a format handler. */
export function registerFormat(handler: FormatHandler): void {
  if (registry.has(handler.name)) {
    throw new Error(`Format "${handler.name}" already registered`);
  }
  registry.set(handler.name, handler);
}

/** Get a handler by name. */
export function getFormat(name: string): FormatHandler | undefined {
  return registry.get(name);
}

/** List all registered format names. */
export function listFormats(): string[] {
  return [...registry.keys()].sort();
}

/** List all registered handlers with descriptions. */
export function listFormatDetails(): Array<{ name: string; description: string; extensions: string[] }> {
  return [...registry.values()]
    .sort((a, b) => a.name.localeCompare(b.name))
    .map((h) => ({
      name: h.name,
      description: h.description,
      extensions: h.extensions,
    }));
}

/**
 * Auto-detect format from a file path by checking registered handlers.
 * Returns the first matching handler name, or undefined.
 */
export function detectFormat(filePath: string): string | undefined {
  // Extract basename first, then find extension within it.
  // Prevents dots in directory names from producing garbage extensions
  // (e.g., "/path/my.project/Makefile" would wrongly yield ".project/Makefile").
  const base = filePath.slice(filePath.lastIndexOf("/") + 1);
  const dotIdx = base.lastIndexOf(".");
  const ext = dotIdx > 0 ? base.slice(dotIdx) : "";

  for (const [name, handler] of registry) {
    if (ext && handler.extensions.includes(ext)) return name;
    if (handler.basenames?.includes(base)) return name;
  }

  return undefined;
}

// ============================================================================
// CLOSING
// ============================================================================
//
// The registry pattern: one body, many members. Each format is a member
// with its own purpose, all serving the same mission — structural alignment.
//
// "For as the body is one, and hath many members." — 1 Corinthians 12:12
// ============================================================================
