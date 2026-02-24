// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/R50/toml/schema_test.ts
// purpose: R50 system-level tests for TOML handler schema paths.
//          Tests error code wiring for schema load failure (R50-006).
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assert } from "jsr:@std/assert";
import { matchRule, getByCode } from "@lib/data/errors.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// R50-006: schema/load failure path
// ---------------------------------------------------------------------------

Deno.test("[CWS-R50-006] R50/schema/load-wiring: schema/load rule resolves to R50-006", () => {
  // R50-006 fires when TOML handler's loadRules() throws.
  // This is a defensive guard that requires a broken schema environment to
  // trigger in practice. We verify the wiring: rule string → catalog entry.
  const entry = matchRule("schema/load");
  assert(entry !== undefined, "schema/load should match a catalog entry");
  assert(entry!.code === "CWS-R50-006",
    `Expected CWS-R50-006, got ${entry!.code}`);

  // Also verify the entry exists directly
  const direct = getByCode("CWS-R50-006");
  assert(direct !== undefined, "CWS-R50-006 should exist in error catalog");
});

// ============================================================================
// CLOSING
// ============================================================================
//
// R50 TOML schema tests — 1 test (R50-006).
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
