// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/foundation/cache-registry.ts
// key:     B-tov-cws-struct-lib-foundation-cache-registry
// title:   CWS Struct — Cache Registry
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-22
// authors: Nova Dawn (CPI-SI)
// purpose: Central registry for all caches across the tool.
//          Modules register their clear functions at import time.
//          clearAllCaches() resets everything — used by tests and CLI.
//
//          No cross-layer imports — registration flows inward (toward foundation).
//          The registry doesn't know what it clears. It just calls functions.
//
// biblical_foundation: "Behold, I make all things new." — Revelation 21:5
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

/** A registered cache entry. */
interface CacheEntry {
  /** Human-readable name for diagnostics. */
  name: string;
  /** Function that clears this cache. */
  clear: () => void;
}

// ============================================================================
// BODY
// ============================================================================

const _registry: CacheEntry[] = [];

/**
 * Register a cache's clear function with the central registry.
 * Called at module level — registration happens on first import.
 *
 * @param name   Human-readable cache identifier (e.g., "schema-pipeline/text")
 * @param clear  Function that resets the cache to its initial state
 */
export function registerCache(name: string, clear: () => void): void {
  _registry.push({ name, clear });
}

/**
 * Clear ALL registered caches. Calls every registered clear function.
 * Used by test setup and CLI reset operations.
 *
 * @param opts.verbose  When true, logs each cache name as it's cleared.
 * @returns The count of caches cleared (for diagnostics).
 */
export function clearAllCaches(opts?: { verbose?: boolean }): number {
  const count = _registry.length;
  for (const entry of _registry) {
    if (opts?.verbose) {
      console.log(`  [cache] clearing: ${entry.name}`);
    }
    entry.clear();
  }
  return count;
}

/**
 * List all registered cache names. For diagnostics and debugging.
 */
export function listCaches(): string[] {
  return _registry.map((e) => e.name);
}

// ============================================================================
// CLOSING
// ============================================================================
//
// Central cache registry — modules register, clearAllCaches() resets all.
// Registration is additive. Clear functions are idempotent.
//
// "Behold, I make all things new." — Revelation 21:5
// ============================================================================
