// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/R05/rust/concept_test.ts
// purpose: Rust R[5] concept/detail tests — field content values, header docs,
//          subtype subsections, typing arrows, closing zone content/depth,
//          form-aware validation, and ternary policy severity thresholding.
// source:  Split from tests/handlers/rust_test.ts
// layer:   R[5] — Concept/detail detection
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert, assertGreater } from "jsr:@std/assert";
import {
  fixture, lazyFormat, errors, warnings, infos, byRule, hasRule, hasMessage,
  hasErrorCode, errorCodeReport,
} from "../../helpers.ts";
import { setGlobalPolicy } from "../../../lib/foundation/mod.ts";

// ============================================================================
// BODY
// ============================================================================

const rust = lazyFormat("rust");

// ---------------------------------------------------------------------------
// metadata/ — Field content value checks (PRAGMA + METADATA)
// ---------------------------------------------------------------------------

Deno.test("[CWS-R05-080] R05/metadata/bad-content-values: detects invalid PRAGMA field values", async () => {
  const results = await rust.lint(fixture("rust/metadata/bad-content-values.rs"));
  const warns = warnings(results);
  const contentWarns = warns.filter((r) => r.rule.startsWith("value/PRAGMA/"));

  // 8 Pragma content checks: I1.key, I1.format, I1.at, I2.type, I2.structure, I3.file, I3.title
  // (I1.from is info, not warning)
  assert(hasRule(results, "value/PRAGMA/I1.key"), "Should catch bad key format");
  assert(hasRule(results, "value/PRAGMA/I1.format"), "Should catch unknown format");
  assert(hasRule(results, "value/PRAGMA/I1.at"), "Should catch bad version");
  assert(hasRule(results, "value/PRAGMA/I2.type"), "Should catch unknown type");
  assert(hasRule(results, "value/PRAGMA/I2.structure"), "Should catch unknown structure");
  assert(hasRule(results, "value/PRAGMA/I3.file"), "Should catch empty file");
  assert(hasRule(results, "value/PRAGMA/I3.title"), "Should catch empty title");
  assertGreater(contentWarns.length, 6, `Expected 7+ PRAGMA content warnings, got ${contentWarns.length}`);
  // Error code: CWS-R05-080 (value/{{varName}}/{{group}}.{{field}} — Layer 3)
  assert(
    hasErrorCode(results, "CWS-R05-080"),
    `Expected CWS-R05-080 (value validation), got: ${errorCodeReport(results)}`,
  );
});

Deno.test("[CWS-R05-080] R05/metadata/bad-content-values: detects invalid METADATA field values", async () => {
  const results = await rust.lint(fixture("rust/metadata/bad-content-values.rs"));

  assert(hasRule(results, "value/METADATA/C1.version"), "Should catch bad version");
  assert(hasRule(results, "value/METADATA/C1.status"), "Should catch unknown status");
  assert(hasRule(results, "value/METADATA/C2.organization"), "Should catch empty organization");
  // Error code: CWS-R05-080 (value/{{varName}}/{{group}}.{{field}} — Layer 3)
  assert(
    hasErrorCode(results, "CWS-R05-080"),
    `Expected CWS-R05-080 (value validation), got: ${errorCodeReport(results)}`,
  );
});

Deno.test("[CWS-R05-080] R05/metadata/bad-content-values: info-level checks for dates and paths", async () => {
  const results = await rust.lint(fixture("rust/metadata/bad-content-values.rs"));
  const infoResults = infos(results);

  // Info-level content checks: I1.from (path), C1.created, C1.updated (dates), C3.scripture (empty)
  const contentInfos = infoResults.filter((r) =>
    r.rule.startsWith("value/PRAGMA/") || r.rule.startsWith("value/METADATA/"));
  assertGreater(contentInfos.length, 3, `Expected 4+ content info results, got ${contentInfos.length}`);

  // Verify severity levels
  assert(hasRule(results, "value/PRAGMA/I1.from"), "Should flag path-like check on I1.from");
  assert(hasRule(results, "value/METADATA/C1.created"), "Should flag date format on C1.created");
  assert(hasRule(results, "value/METADATA/C1.updated"), "Should flag date format on C1.updated");
  assert(hasRule(results, "value/METADATA/C3.scripture"), "Should flag empty scripture");

  // Verify all content infos are actually info severity
  for (const r of contentInfos) {
    assertEquals(r.severity, "info", `${r.rule} should be info, got ${r.severity}`);
  }
  // Error code: CWS-R05-080 (value/{{varName}}/{{group}}.{{field}} — Layer 3)
  assert(
    hasErrorCode(results, "CWS-R05-080"),
    `Expected CWS-R05-080 (value validation), got: ${errorCodeReport(results)}`,
  );
});

Deno.test("R05/metadata/valid-library: zero content value warnings", async () => {
  const results = await rust.lint(fixture("rust/structure/valid-library.rs"));
  const warns = warnings(results);
  const contentWarns = warns.filter((r) =>
    r.rule.startsWith("value/PRAGMA/") || r.rule.startsWith("value/METADATA/"));
  assertEquals(contentWarns.length, 0,
    `Expected 0 content warnings on valid-library, got: ${JSON.stringify(contentWarns.map(r => r.rule))}`);
});

Deno.test("R05/metadata/placeholder-values: content checks skip placeholders", async () => {
  const results = await rust.lint(fixture("rust/metadata/placeholder-values.rs"));
  // Content checks should NOT fire for placeholder values like [YOUR-KEY-HERE]
  const contentWarns = warnings(results).filter((r) =>
    r.rule.startsWith("value/PRAGMA/") || r.rule.startsWith("value/METADATA/"));
  assertEquals(contentWarns.length, 0,
    `Content checks should skip placeholders: ${JSON.stringify(contentWarns.map(r => r.rule))}`);
});

Deno.test("[CWS-R05-080] R05/metadata/bad-content-values: total of 14 content check results", async () => {
  const results = await rust.lint(fixture("rust/metadata/bad-content-values.rs"));
  const contentResults = results.filter((r) =>
    r.rule.startsWith("value/PRAGMA/") || r.rule.startsWith("value/METADATA/"));
  assertEquals(contentResults.length, 14,
    `Expected 14 total content results (8 PRAGMA + 6 METADATA), got ${contentResults.length}: ${JSON.stringify(contentResults.map(r => r.rule))}`);
  // Error code: CWS-R05-080 (value/{{varName}}/{{group}}.{{field}} — Layer 3)
  assert(
    hasErrorCode(results, "CWS-R05-080"),
    `Expected CWS-R05-080 (value validation), got: ${errorCodeReport(results)}`,
  );
});

// ---------------------------------------------------------------------------
// setup/ — Header documentation checks
// ---------------------------------------------------------------------------

Deno.test("[CWS-R05-007] R05/setup/header-doc: no documentation before first subsection triggers info", async () => {
  const f = fixture("rust/setup/no-header-doc.rs");
  const results = await rust.lint(f);
  const headerDoc = byRule(results, "setup/header-doc");
  assertGreater(headerDoc.length, 0, "Should detect missing header documentation");
  assertEquals(headerDoc[0]!.severity, "info");
  // Error code: CWS-R05-007 (setup/header-doc — Layer 3)
  assert(
    hasErrorCode(results, "CWS-R05-007"),
    `Expected CWS-R05-007 (setup header doc), got: ${errorCodeReport(results)}`,
  );
  assert(hasMessage(headerDoc, "header documentation"), "Should mention header documentation");
});

Deno.test("R05/setup/header-doc: good documentation before first subsection is clean", async () => {
  const f = fixture("rust/setup/good-header-doc.rs");
  const results = await rust.lint(f);
  const headerDoc = byRule(results, "setup/header-doc");
  assertEquals(headerDoc.length, 0, "Good header doc should produce no findings");
});

// ---------------------------------------------------------------------------
// body/ — Subtype subsections and typing arrows
// ---------------------------------------------------------------------------

Deno.test("[CWS-R05-008] R05/body/subtype-subsections: wrong subsection names for library triggers info", async () => {
  const f = fixture("rust/body/wrong-subtype-subsections.rs");
  const results = await rust.lint(f);
  const subtypeSubs = byRule(results, "body/subtype-subsections");
  assertGreater(subtypeSubs.length, 0, "Should detect wrong subsection names");
  assertEquals(subtypeSubs[0]!.severity, "info");
  assert(hasMessage(subtypeSubs, "library"), "Should mention the subtype");

  // Error code: CWS-R05-008 (subtype subsection mismatch — Layer 3)
  assert(
    hasErrorCode(results, "CWS-R05-008"),
    `Expected CWS-R05-008 (subtype subsections), got: ${errorCodeReport(byRule(results, "body/subtype"))}`,
  );
});

Deno.test("R05/body/subtype-subsections: RO-acknowledged absent subsections are not reported", async () => {
  const f = fixture("rust/body/ro-acknowledged-subsections.rs");
  const results = await rust.lint(f);
  const subtypeSubs = byRule(results, "body/subtype-subsections");
  assertEquals(subtypeSubs.length, 0,
    "Subsections acknowledged in Reserved Omission should not be reported as missing");
});

// ── Typing arrow tests (R[5] refinement) ─────────────────────────────────

Deno.test("R05/body/typing-arrow: module->utility narrows canonical — irrelevant sections not flagged", async () => {
  const f = fixture("rust/body/typing-arrow-utility.rs");
  const results = await rust.lint(f);

  // subtype-subsections: canonical list should be narrowed to required + available.
  // Irrelevant sections (Trait Implementations, Constructors, Queries) should NOT appear.
  const subtypeSubs = byRule(results, "body/subtype-subsections");
  const irrelevantSections = ["trait implementations", "constructors", "queries"];
  const flaggedIrrelevant = subtypeSubs.filter((r) =>
    irrelevantSections.some((s) => r.message.toLowerCase().includes(s)));
  assertEquals(flaggedIrrelevant.length, 0,
    "module->utility should NOT flag irrelevant sections (Trait Implementations, Constructors, Queries)");

  // required-subsection: only typing-required sections should produce warnings.
  // utility requires only FreeFunctions (BODY) and Imports (SETUP).
  const requiredBody = byRule(results, "body/required-subsection");
  const irrelevantRequired = requiredBody.filter((r) =>
    irrelevantSections.some((s) => r.message.toLowerCase().includes(s)));
  assertEquals(irrelevantRequired.length, 0,
    "module->utility should NOT warn about irrelevant sections as required");

  // Verify available sections (Identity Access, Core Logic) don't produce required warnings either
  const availableSections = ["identity access", "core logic"];
  const availableRequired = requiredBody.filter((r) =>
    availableSections.some((s) => r.message.toLowerCase().includes(s)));
  assertEquals(availableRequired.length, 0,
    "module->utility should NOT warn about available (optional) sections as required");
});

Deno.test("R05/body/typing-arrow: module->service with all 6 sections — clean", async () => {
  const f = fixture("rust/body/typing-arrow-service.rs");
  const results = await rust.lint(f);
  const subtypeSubs = byRule(results, "body/subtype-subsections");
  assertEquals(subtypeSubs.length, 0,
    "module->service with all 6 canonical sections should be clean");

  const requiredBody = byRule(results, "body/required-subsection");
  assertEquals(requiredBody.length, 0,
    "module->service with all required sections present should have no required warnings");
});

// ---------------------------------------------------------------------------
// closing/ — Zone content and depth checks
// ---------------------------------------------------------------------------

Deno.test("[CWS-R05-030] R05/closing/X1-content: X1 zone missing required fields triggers info", async () => {
  const f = fixture("rust/closing/x1-missing-fields.rs");
  const results = await rust.lint(f);
  const x1Content = byRule(results, "closing/X1-content");
  // X1 zone present but missing Never, Careful, Safe fields
  assertGreater(x1Content.length, 0, "Should detect missing X1 fields");
  assertEquals(x1Content[0]!.severity, "info");
  assert(hasMessage(x1Content, "never"), "Should mention missing 'never' field");

  // Error code: CWS-R05-030 (closing zone content — Layer 3)
  assert(
    hasErrorCode(results, "CWS-R05-030"),
    `Expected CWS-R05-030 (closing content), got: ${errorCodeReport(x1Content)}`,
  );
});

Deno.test("[CWS-R05-030] R05/closing/X5-content: X5 zone missing scripture field triggers info", async () => {
  const f = fixture("rust/closing/x5-missing-scripture.rs");
  const results = await rust.lint(f);
  const x5Content = byRule(results, "closing/X5-content");
  // X5 zone present but missing Scripture field
  assertGreater(x5Content.length, 0, "Should detect missing scripture in X5 zone");
  assertEquals(x5Content[0]!.severity, "info");
  assert(hasMessage(x5Content, "scripture"), "Should mention missing 'scripture' field");

  // Error code: CWS-R05-030 (closing zone content — Layer 3)
  assert(
    hasErrorCode(results, "CWS-R05-030"),
    `Expected CWS-R05-030 (closing content), got: ${errorCodeReport(x5Content)}`,
  );
});

Deno.test("[CWS-R05-031] R05/closing/X1-depth: detects empty and placeholder field values in X1", async () => {
  const results = await rust.lint(fixture("rust/closing/x1-empty-fields.rs"));
  const errs = errors(results);
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);

  const depth = byRule(results, "closing/X1-depth");
  assertGreater(depth.length, 0, "Should detect empty/placeholder X1 field values");
  assertEquals(depth[0]!.severity, "info", "X1-depth should be info-level");
  assert(hasMessage(depth, "never") || hasMessage(depth, "careful"),
    "Should mention the field with empty/placeholder value");

  // Error code: CWS-R05-031 (closing zone depth — Layer 3)
  assert(
    hasErrorCode(results, "CWS-R05-031"),
    `Expected CWS-R05-031 (closing depth), got: ${errorCodeReport(depth)}`,
  );
});

Deno.test("R05/closing/depth: valid-library has no depth warnings", async () => {
  const results = await rust.lint(fixture("rust/structure/valid-library.rs"));
  const depth = results.filter((r) => r.rule.includes("-depth"));
  assertEquals(depth.length, 0, "Valid library should produce no depth warnings");
});

// ---------------------------------------------------------------------------
// form/ — Form-aware validation (layer chain: format → variant)
// ---------------------------------------------------------------------------

// NOTE: form/module-reserved, form/module-missing-required, and form/template-reserved
// require form constraint schemas in form_identity/containers format (like TOML bare-bone).
// Rust code form schemas currently use content_expectations format instead.
// These tests are ignored until proper form constraint schemas are created for Rust code forms.

Deno.test({ name: "R05/form/module-reserved: detects S2 Modules in non-template module", ignore: true, fn: async () => {
  const f = fixture("rust/form/module-has-reserved-section.rs");
  const results = await rust.lint(f);
  const reserved = byRule(results, "form/reserved-section-present");
  assertGreater(reserved.length, 0, "Should detect reserved S2 Modules in module file");
  assertEquals(reserved[0]!.severity, "warn");
  assert(hasMessage(reserved, "Modules"), "Should mention the reserved section tag");
  assert(hasMessage(reserved, "RESERVED"), "Should indicate the section is reserved");
}});

Deno.test({ name: "R05/form/module-missing-required: detects absent S1 Imports", ignore: true, fn: async () => {
  const f = fixture("rust/form/module-missing-required.rs");
  const results = await rust.lint(f);
  const missing = byRule(results, "form/required-section-missing");
  assertGreater(missing.length, 0, "Should detect missing required section");
  assertEquals(missing[0]!.severity, "warn");
  assert(hasMessage(missing, "Imports"), "Should mention the missing Imports section");
}});

Deno.test({ name: "R05/form/template-reserved: reserved checks fire on templates (templates are live)", ignore: true, fn: async () => {
  const f = fixture("rust/form/template-with-reserved.rs");
  const results = await rust.lint(f);
  const reserved = byRule(results, "form/reserved-section-present");
  assertGreater(reserved.length, 0, "Templates should be checked for reserved sections");
  assert(hasMessage(reserved, "Modules"), "Should detect S2 Modules in module template");
}});

Deno.test("R05/form/template-required: required checks SKIP templates", async () => {
  const f = fixture("rust/form/template-with-reserved.rs");
  const results = await rust.lint(f);
  const missing = byRule(results, "form/required-section-missing");
  assertEquals(missing.length, 0, "Templates should not trigger required-section-missing");
});

Deno.test("R05/form/valid-module: template has no reserved violations (existing valid-module)", async () => {
  const f = fixture("rust/structure/valid-module.rs");
  const results = await rust.lint(f);
  const reserved = byRule(results, "form/reserved-section-present");
  assertEquals(reserved.length, 0, "Valid module template should have no reserved section violations");
});

Deno.test("R05/form/bare-bone-default: files without subtype get bare-bone form constraints", async () => {
  // format/no-omni.rs has no subtype — should still get bare-bone form
  const f = fixture("rust/format/no-omni.rs");
  const results = await rust.lint(f);
  // Bare-bone form loads but with no reserved sections to violate in a minimal file,
  // we just verify no crash and no reserved-section-present results
  const reserved = byRule(results, "form/reserved-section-present");
  // A minimal file with no subsection headers won't trigger reserved checks
  assertEquals(reserved.length, 0, "Minimal file without subsection headers should have no reserved violations");
});

// ---------------------------------------------------------------------------
// policy/ — Ternary policy severity thresholding
// ---------------------------------------------------------------------------

// NOTE: policy/* tests depend on form/required-section-missing and form/reserved-section-present
// rules which require form constraint schemas in form_identity/containers format.
// Rust code form schemas currently use content_expectations format instead.
// Ignored until proper form constraint schemas are created for Rust code forms.

Deno.test({ name: "R05/policy/growth-required: required-missing becomes info under growth", ignore: true, fn: async () => {
  setGlobalPolicy("growth");
  try {
    const f = fixture("rust/form/module-missing-required.rs");
    const results = await rust.lint(f);
    const missing = byRule(results, "form/required-section-missing");
    assertGreater(missing.length, 0, "Should still detect missing required section");
    assertEquals(missing[0]!.severity, "info",
      "Under growth policy, form/required-section-missing should be info (not warn)");
  } finally {
    setGlobalPolicy("balanced");
  }
}});

Deno.test({ name: "R05/policy/strict-required: required-missing becomes error under strict", ignore: true, fn: async () => {
  setGlobalPolicy("strict");
  try {
    const f = fixture("rust/form/module-missing-required.rs");
    const results = await rust.lint(f);
    const missing = byRule(results, "form/required-section-missing");
    assertGreater(missing.length, 0, "Should still detect missing required section");
    assertEquals(missing[0]!.severity, "error",
      "Under strict policy, form/required-section-missing should be error (not warn)");
  } finally {
    setGlobalPolicy("balanced");
  }
}});

Deno.test({ name: "R05/policy/balanced-required: required-missing stays warn under balanced", ignore: true, fn: async () => {
  setGlobalPolicy("balanced");
  const f = fixture("rust/form/module-missing-required.rs");
  const results = await rust.lint(f);
  const missing = byRule(results, "form/required-section-missing");
  assertGreater(missing.length, 0, "Should detect missing required section");
  assertEquals(missing[0]!.severity, "warn",
    "Under balanced policy, form/required-section-missing stays warn");
}});

Deno.test({ name: "R05/policy/growth-reserved: reserved-present becomes info under growth", ignore: true, fn: async () => {
  setGlobalPolicy("growth");
  try {
    const f = fixture("rust/form/module-has-reserved-section.rs");
    const results = await rust.lint(f);
    const reserved = byRule(results, "form/reserved-section-present");
    assertGreater(reserved.length, 0, "Should still detect reserved section");
    assertEquals(reserved[0]!.severity, "info",
      "Under growth policy, form/reserved-section-present should be info");
  } finally {
    setGlobalPolicy("balanced");
  }
}});

Deno.test({ name: "R05/policy/strict-reserved: reserved-present becomes error under strict", ignore: true, fn: async () => {
  setGlobalPolicy("strict");
  try {
    const f = fixture("rust/form/module-has-reserved-section.rs");
    const results = await rust.lint(f);
    const reserved = byRule(results, "form/reserved-section-present");
    assertGreater(reserved.length, 0, "Should still detect reserved section");
    assertEquals(reserved[0]!.severity, "error",
      "Under strict policy, form/reserved-section-present should be error");
  } finally {
    setGlobalPolicy("balanced");
  }
}});

// ============================================================================
// CLOSING
// ============================================================================
//
// R[5] Rust concept/detail tests — field values, header docs, subtype
// subsections, typing arrows, closing zone content/depth, form-aware
// validation, ternary policy severity.
// 27 tests covering: metadata (6), setup (2), body (4), closing (4),
// form (6), policy (5).
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
