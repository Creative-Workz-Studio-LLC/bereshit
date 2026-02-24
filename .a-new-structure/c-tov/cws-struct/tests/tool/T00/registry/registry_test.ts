// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/T00/tool/registry_test.ts
// purpose: T00 tool-layer tests for format registry error paths.
//          Tests ToolError for duplicate registration (CWS-T00-040).
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertThrows } from "jsr:@std/assert";
import { registerFormat } from "@lib/engine/registry.ts";
import { ToolError } from "@lib/foundation/tool-error.ts";
import type { FormatHandler } from "@lib/foundation/types.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// T00-040: Duplicate format registration
// ---------------------------------------------------------------------------

Deno.test("[CWS-T00-040] T00/registry/duplicate-format: ToolError for double-registering format", () => {
  // Create a dummy format handler with a known-unique name
  const dummyName = `__test_duplicate_${Date.now()}`;
  const dummy: FormatHandler = {
    name: dummyName,
    description: "test-only dummy handler",
    extensions: [".test"],
    lint: async () => [],
  };

  // First registration should succeed
  registerFormat(dummy);

  // Second registration with same name should throw T00-040
  assertThrows(
    () => registerFormat(dummy),
    ToolError,
  );
});

// ============================================================================
// CLOSING
// ============================================================================
//
// T00 registry tests — tooling error path coverage.
// 1 test covering T00-040.
//
// "The light shineth in darkness." — John 1:5
// ============================================================================
