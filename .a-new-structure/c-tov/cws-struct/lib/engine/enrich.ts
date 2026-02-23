// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/engine/enrich.ts
// key:     B-tov-cws-struct-lib-engine-enrich
// title:   CWS Struct — Result Enrichment
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-22
// authors: Nova Dawn (CPI-SI)
// purpose: Post-lint enrichment — resolves rule strings to CWS error codes.
//          Called between handler.lint() and summarize() to populate errorCode
//          and layerName on each LintResult. Handlers stay clean (emit rule
//          strings); the engine enriches with catalog data.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import type { LintResult } from "../foundation/mod.ts";
import { matchRule } from "../data/mod.ts";

// ============================================================================
// BODY
// ============================================================================

/**
 * Resolve each LintResult's rule string to its CWS error code and layer name.
 * Mutates results in place for efficiency (lint produces fresh arrays).
 *
 * Rules without a catalog entry (info-only, uncataloged) are left as-is —
 * their errorCode and layerName remain undefined.
 */
export function resolveErrorCodes(results: LintResult[]): void {
  for (const r of results) {
    const entry = matchRule(r.rule);
    if (entry) {
      r.errorCode = entry.code;
      r.layerName = entry.layerName;
    }
  }
}

// ============================================================================
// CLOSING
// ============================================================================
//
// Single-purpose: resolve rule → error code. The matchRule() function handles
// both static rules (exact match) and dynamic patterns ({{placeholder}}).
//
// "A false balance is abomination to the LORD: but a just weight
//  is his delight." — Proverbs 11:1
// ============================================================================
