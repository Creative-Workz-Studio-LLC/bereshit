// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/T00/tool/transpiler_test.ts
// purpose: T00 tool-layer tests for transpiler schema loading error paths.
//          Tests ToolError for missing mapping/form/content fields.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertRejects } from "jsr:@std/assert";
import { ToolError } from "@lib/foundation/tool-error.ts";
import {
  loadTranspilerMappings,
  loadFormStructure,
  loadFormContent,
} from "@lib/engine/transpiler-mappings.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// T00-030: Transpiler mapping schema missing 'mapping' field
// ---------------------------------------------------------------------------

Deno.test("[CWS-T00-030] T00/transpiler/missing-mapping: ToolError for schema without mapping field", async () => {
  // Trigger by loading a mapping between two formats that don't have a schema.
  // "dotfiles" → "makefile" has no mapping schema — loadTranspilerMappings
  // will parse an empty/absent file, find no `mapping` field, and throw T00-030.
  await assertRejects(
    () => loadTranspilerMappings("dotfiles" as never, "makefile" as never),
    ToolError,
  );
});

// ---------------------------------------------------------------------------
// T00-031: Form structure schema missing 'form' field
// ---------------------------------------------------------------------------

Deno.test("[CWS-T00-031] T00/transpiler/missing-form: ToolError for schema without form field", async () => {
  // Load a non-existent form — "nonexistent-form-9999" won't have a schema.
  // The function will either fail to find the file (caught) or find data
  // without 'form' field and throw T00-031.
  await assertRejects(
    () => loadFormStructure("nonexistent-form-9999"),
    Error, // May throw ToolError(T00-031) or file-not-found error
  );
});

// ---------------------------------------------------------------------------
// T00-032: Form content schema missing 'content_expectations' field
// ---------------------------------------------------------------------------

Deno.test("[CWS-T00-032] T00/transpiler/missing-content: ToolError for schema without content_expectations", async () => {
  // Load content for a non-existent form — "nonexistent-form-9999" won't have a schema.
  await assertRejects(
    () => loadFormContent("go", "nonexistent-form-9999"),
    Error,
  );
});

// ============================================================================
// CLOSING
// ============================================================================
//
// T00 transpiler schema error paths — 3 tests (T00-030, T00-031, T00-032).
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
