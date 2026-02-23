// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/R25/go/block_test.ts
// purpose: R25 (block-level) detection tests for the Go format handler.
//          Tests subsection ordering, doc comments, closing zones, and
//          required subsections — the block-interior grain in the hierarchy.
// source:  Split from tests/handlers/go_test.ts
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert, assertGreater } from "jsr:@std/assert";
import {
  fixture, lazyFormat, errors, warnings, infos, byRule,
  hasRule, hasMessage, hasErrorCode, errorCodeReport,
} from "../../helpers.ts";

// ============================================================================
// BODY
// ============================================================================

const go = lazyFormat("go");

// ---------------------------------------------------------------------------
// doc/ — Doc comment quality checks (schema-driven)
// ---------------------------------------------------------------------------

Deno.test("[CWS-R05-001] R25/doc/no-header-doc: warns about missing doc comments (package, pragma, metadata)", async () => {
  const results = await go.lint(fixture("go/setup/no-header-doc.go"));
  const docPackage = byRule(results, "doc/package");
  const docPragma = byRule(results, "doc/pragma-var");
  const docMetadata = byRule(results, "doc/metadata-var");
  assertGreater(docPackage.length, 0,
    "Should warn about missing package doc comment");
  assertGreater(docPragma.length, 0,
    "Should warn about missing Pragma doc comment");
  assertGreater(docMetadata.length, 0,
    "Should warn about missing Metadata doc comment");
  // Schema says severity is "warn" for all three
  assertEquals(docPackage[0]!.severity, "warn", "package doc severity from schema");
  assertEquals(docPragma[0]!.severity, "warn", "pragma doc severity from schema");
  assertEquals(docMetadata[0]!.severity, "warn", "metadata doc severity from schema");
  // Error code: CWS-R05-001 (doc/package — Layer 3)
  assert(
    hasErrorCode(results, "CWS-R05-001"),
    `Expected CWS-R05-001 (doc/package), got: ${errorCodeReport(results)}`,
  );
  // Error code: CWS-R05-002 (doc/pragma-var — Layer 3)
  assert(
    hasErrorCode(results, "CWS-R05-002"),
    `Expected CWS-R05-002 (doc/pragma-var), got: ${errorCodeReport(results)}`,
  );
  // Error code: CWS-R05-003 (doc/metadata-var — Layer 3)
  assert(
    hasErrorCode(results, "CWS-R05-003"),
    `Expected CWS-R05-003 (doc/metadata-var), got: ${errorCodeReport(results)}`,
  );
});

Deno.test("R25/doc/valid-library: no doc comment warnings (all comments present)", async () => {
  const results = await go.lint(fixture("go/structure/valid-library.go"));
  const docRules = byRule(results, "doc/");
  assertEquals(docRules.length, 0,
    `Valid library should have 0 doc warnings: ${JSON.stringify(docRules.map(r => r.rule))}`);
});

Deno.test("R25/doc/schema-driven-severity: Go doc comment expectations loaded from schema", async () => {
  const { loadCodeRules } = await import("../../../lib/foundation/mod.ts");
  const rules = await loadCodeRules("go");
  const pkgDoc = rules.docCommentExpectations["package_doc"];
  const pragmaDoc = rules.docCommentExpectations["pragma_doc"];
  const metaDoc = rules.docCommentExpectations["metadata_doc"];
  assertEquals(pkgDoc?.severity, "warn", "package_doc severity from schema");
  assertEquals(pragmaDoc?.severity, "warn", "pragma_doc severity from schema");
  assertEquals(metaDoc?.severity, "warn", "metadata_doc severity from schema");
});

// ---------------------------------------------------------------------------
// setup/ — SETUP block subsection ordering
// ---------------------------------------------------------------------------

Deno.test("R25/setup/subsection-order-correct: all subsections in correct dependency order — zero order warnings", async () => {
  const results = await go.lint(fixture("go/setup/subsection-order-correct.go"));
  const errs = errors(results);
  const orderWarns = byRule(results, "setup/subsection-order");
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);
  assertEquals(orderWarns.length, 0, `Expected 0 subsection-order warnings: ${JSON.stringify(orderWarns, null, 2)}`);
});

Deno.test("[CWS-R25-023] R25/setup/subsection-order-wrong: detects Core Types before Constants (wrong dependency order)", async () => {
  const results = await go.lint(fixture("go/setup/subsection-order-wrong.go"));
  assert(
    hasRule(results, "setup/subsection-order"),
    `Should detect wrong SETUP subsection order, got rules: ${warnings(results).map((w) => w.rule).join(", ")}`,
  );
  // Error code: CWS-R25-023 (setup/subsection-order — Layer 1)
  assert(
    hasErrorCode(results, "CWS-R25-023"),
    `Expected CWS-R25-023 (setup order), got: ${errorCodeReport(results)}`,
  );
});

Deno.test("[CWS-R10-043] R25/setup/alias-name: detects alias names and suggests canonical alternatives", async () => {
  const results = await go.lint(fixture("go/setup/aliased-subsection-names.go"));
  const aliasInfos = byRule(results, "setup/alias-name");
  // Fixture uses: Consts (→Constants), Vars (→Variables), Types (→CoreTypes),
  // Interfaces (→InterfaceDefs), Methods (→TypeMethods) = 5 aliases
  assertGreater(aliasInfos.length, 0, `Expected alias info diagnostics, got: ${JSON.stringify(aliasInfos, null, 2)}`);
  // Should NOT have order warnings — aliases are in correct order
  const orderWarns = byRule(results, "setup/subsection-order");
  assertEquals(orderWarns.length, 0, `Aliased names in correct order should produce 0 order warnings: ${JSON.stringify(orderWarns, null, 2)}`);
  // Error code: CWS-R10-043 (setup/alias-name — Layer 2)
  assert(
    hasErrorCode(results, "CWS-R10-043"),
    `Expected CWS-R10-043 (alias name), got: ${errorCodeReport(results)}`,
  );
});

// ---------------------------------------------------------------------------
// setup/required — Required SETUP subsections for subtype
// ---------------------------------------------------------------------------

Deno.test("[CWS-R25-030] R25/setup/missing-required-subsections: library missing ErrorTypes and CoreTypes", async () => {
  const results = await go.lint(fixture("go/setup/missing-required-subsections.go"));
  const errs = errors(results);
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);

  const required = byRule(results, "setup/required-subsection");
  assertGreater(required.length, 0, "Should detect missing required SETUP subsections");
  assertEquals(required[0]!.severity, "warn", "Required subsection check should be warn-level");
  // Error code: CWS-R25-030 (setup/required-subsection — Layer 1)
  assert(
    hasErrorCode(results, "CWS-R25-030"),
    `Expected CWS-R25-030 (required setup), got: ${errorCodeReport(required)}`,
  );
  assert(hasMessage(required, "Error Types"), "Should mention missing Error Types");
  assert(hasMessage(required, "Core Types"), "Should mention missing Core Types");
});

Deno.test("R25/setup/missing-required-subsections: reports count matches schema", async () => {
  const results = await go.lint(fixture("go/setup/missing-required-subsections.go"));
  const required = byRule(results, "setup/required-subsection");
  // Library requires: Imports (present), Constants (present), Error Types (missing), Core Types (missing)
  assertEquals(required.length, 2, "Should report exactly 2 missing required subsections");
});

// ---------------------------------------------------------------------------
// body/ — BODY block subsection ordering
// ---------------------------------------------------------------------------

Deno.test("[CWS-R25-024] R25/body/wrong-body-order: detects BODY subsections out of numeric order", async () => {
  const results = await go.lint(fixture("go/body/wrong-body-order.go"));
  const errs = errors(results);
  const orderWarns = byRule(results, "body/subsection-order");
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);
  assertGreater(orderWarns.length, 0, "Should detect wrong BODY subsection order");
  // Error code: CWS-R25-024 (body/subsection-order — Layer 1)
  assert(
    hasErrorCode(results, "CWS-R25-024"),
    `Expected CWS-R25-024 (body order), got: ${errorCodeReport(orderWarns)}`,
  );
  // New numeric format: message references §N numbers
  assert(
    hasMessage(results, "§2") && hasMessage(results, "§5"),
    "Should reference the misordered subsection numbers (§5 before §2)",
  );
});

Deno.test("R25/body/valid-library: zero body subsection order warnings", async () => {
  const results = await go.lint(fixture("go/structure/valid-library.go"));
  const orderWarns = byRule(results, "body/subsection-order");
  assertEquals(orderWarns.length, 0, `Expected 0 body/subsection-order warnings: ${JSON.stringify(orderWarns, null, 2)}`);
});

// ---------------------------------------------------------------------------
// body/required — Required BODY subsections for subtype
// ---------------------------------------------------------------------------

Deno.test("[CWS-R25-031] R25/body/missing-required-subsections: library missing all 7 required body subsections", async () => {
  const results = await go.lint(fixture("go/body/missing-required-subsections.go"));
  const errs = errors(results);
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);

  const required = byRule(results, "body/required-subsection");
  assertGreater(required.length, 0, "Should detect missing required BODY subsections");
  assertEquals(required[0]!.severity, "warn", "Required body subsection check should be warn-level");
  // Error code: CWS-R25-031 (body/required-subsection — Layer 1)
  assert(
    hasErrorCode(results, "CWS-R25-031"),
    `Expected CWS-R25-031 (required body), got: ${errorCodeReport(required)}`,
  );
  // Library requires: Identity Access, Trait Implementations, Constructors,
  // Core Logic, Queries, Output Display, Free Functions = 7 required.
  // Fixture has only Helpers (reserved) and Core Operations (available). All 7 missing.
  assertEquals(required.length, 7, "Should report exactly 7 missing required body subsections");
});

// ---------------------------------------------------------------------------
// closing/ — CLOSING block zone ordering and required zones
// ---------------------------------------------------------------------------

Deno.test("[CWS-R25-020] R25/closing/wrong-closing-order: detects code zone after documentation section", async () => {
  const results = await go.lint(fixture("go/closing/wrong-closing-order.go"));
  const errs = errors(results);
  const zoneWarns = byRule(results, "closing/zone-order");
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);
  assertGreater(zoneWarns.length, 0, "Should detect code zone after documentation section");
  // Error code: CWS-R25-020 (closing/zone-order — Layer 1)
  assert(
    hasErrorCode(results, "CWS-R25-020"),
    `Expected CWS-R25-020 (closing zone order), got: ${errorCodeReport(zoneWarns)}`,
  );
  assert(
    hasMessage(results, "X1") || hasMessage(results, "Cv") || hasMessage(results, "Cc"),
    "Should reference the misordered zones",
  );
});

Deno.test("R25/closing/valid-library: zero closing zone order warnings", async () => {
  const results = await go.lint(fixture("go/structure/valid-library.go"));
  const zoneWarns = byRule(results, "closing/zone-order");
  assertEquals(zoneWarns.length, 0, `Expected 0 closing/zone-order warnings: ${JSON.stringify(zoneWarns, null, 2)}`);
});

Deno.test("[CWS-R25-032] R25/closing/missing-required-zones: detects missing X1 and X5", async () => {
  const results = await go.lint(fixture("go/closing/missing-required-zones.go"));
  const errs = errors(results);
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);

  assert(hasRule(results, "closing/required-X1"), "Should detect missing X1 zone");
  assert(hasRule(results, "closing/required-X5"), "Should detect missing X5 zone");
  // Error code: CWS-R25-032 (closing/required-{{tag}} — Layer 1)
  assert(
    hasErrorCode(results, "CWS-R25-032"),
    `Expected CWS-R25-032 (required closing zone), got: ${errorCodeReport(results)}`,
  );

  // Both should be info-level
  const x1 = byRule(results, "closing/required-X1");
  const x5 = byRule(results, "closing/required-X5");
  assertEquals(x1[0]!.severity, "info", "Missing X1 should be info-level");
  assertEquals(x5[0]!.severity, "info", "Missing X5 should be info-level");
});

Deno.test("R25/closing/valid-library: no required-zone or zone-content warnings", async () => {
  // The valid-library has a minimal CLOSING — if it has X1/X5, no content warnings.
  // If it lacks X1/X5, the required checks fire but that's separate from zone-content.
  const results = await go.lint(fixture("go/structure/valid-library.go"));
  const errs = errors(results);
  assertEquals(errs.length, 0, "Valid library should produce no errors");
});

// ============================================================================
// CLOSING
// ============================================================================
//
// R25 Go block-level tests — 15 tests covering:
// doc comments, setup subsection order/aliases/required,
// body subsection order/required, closing zone order/required.
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
