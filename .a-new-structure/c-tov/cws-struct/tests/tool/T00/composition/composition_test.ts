// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/T00/tool/composition_test.ts
// purpose: T00 tool-layer tests for composition parsing error paths.
//          Tests ToolError throws with CWS-T00-020 through T00-022.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertThrows } from "jsr:@std/assert";
import { parseCompositionTarget } from "@lib/foundation/composition-loader.ts";
import { ToolError } from "@lib/foundation/tool-error.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// T00-020: Composition missing target field
// ---------------------------------------------------------------------------

Deno.test("[CWS-T00-020] T00/composition/missing-target: ToolError for missing target", () => {
  assertThrows(
    () => parseCompositionTarget('{ "foo": "bar" }'),
    ToolError,
  );
});

// ---------------------------------------------------------------------------
// T00-021: Composition target missing required fields
// ---------------------------------------------------------------------------

Deno.test("[CWS-T00-021] T00/composition/missing-fields: ToolError for missing pragma/format/form", () => {
  assertThrows(
    () => parseCompositionTarget('{ "target": { "name": "test" } }'),
    ToolError,
  );
});

// ---------------------------------------------------------------------------
// T00-022: Composition target missing structure/content sides
// ---------------------------------------------------------------------------

Deno.test("[CWS-T00-022] T00/composition/missing-sides: ToolError for missing composition sides", () => {
  assertThrows(
    () => parseCompositionTarget(JSON.stringify({
      target: {
        pragma: "test",
        format: "go",
        form: "module",
        composition: {},
      },
    })),
    ToolError,
  );
});

// ============================================================================
// CLOSING
// ============================================================================
//
// T00 composition tests — tooling error path coverage.
// 3 tests covering T00-020, T00-021, T00-022.
//
// "The light shineth in darkness." — John 1:5
// ============================================================================
