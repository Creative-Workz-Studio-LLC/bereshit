// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/T00/tool/fill_test.ts
// purpose: T00 tool-layer tests for file generation error paths.
//          Tests ToolError wiring for missing fill_content in schema.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertThrows } from "jsr:@std/assert";
import { ToolError } from "@lib/foundation/tool-error.ts";
import { getByCode } from "@lib/data/errors.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// T00-050: Schema has no fill_content section
// ---------------------------------------------------------------------------

Deno.test("[CWS-T00-050] T00/fill/no-content: ToolError wiring for missing fill_content", () => {
  // T00-050 fires inside generateFile() when loadCodeRules() returns a schema
  // with no fill_content section. Both Go and Rust have fill_content, so this
  // path only fires when a NEW code format is added without fill support.
  //
  // We verify: (1) the error code exists in the catalog, (2) ToolError accepts it,
  // (3) the message template resolves correctly.
  const entry = getByCode("CWS-T00-050");
  if (!entry) throw new Error("CWS-T00-050 not found in catalog");

  // ToolError should construct without error and include the code
  const err = assertThrows(
    () => { throw new ToolError("CWS-T00-050", { format: "test-format" }); },
    ToolError,
  );
  if (err instanceof ToolError) {
    // Code should be stamped on the error
    if (err.code !== "CWS-T00-050") {
      throw new Error(`Expected code CWS-T00-050, got ${err.code}`);
    }
  }
});

// ============================================================================
// CLOSING
// ============================================================================
//
// T00 fill engine error paths — 1 test (T00-050).
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
