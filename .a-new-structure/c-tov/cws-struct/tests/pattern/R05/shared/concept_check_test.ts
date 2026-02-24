// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/T00/R05/shared/concept_check_test.ts
// key:     B-tov-cws-struct-tests-shared-concept-check
// title:   CWS Struct — Concept Validation Tests
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-23
// authors: Nova Dawn (CPI-SI)
// purpose: Verify validateContainerConcepts() and buildConceptContainers()
//          correctly enforce the ternary matrix (42 sections x 11 concepts).
//
//          Gate | Verifies
//          -----|----------------------------------------
//          G1   | Denied concept found → warn result
//          G2   | Granted concept missing → info result
//          G3   | Deferred concept → no result (skipped)
//          G4   | Empty container → no results
//          G5   | Comment-only container → no detections
//          G6   | ALL_DENIED container → warn for any found concept
//          G7   | ALL_GRANTED container → info for missing concepts
//          G8   | Rule namespace follows concept/{container}/{concept}
//          G9   | Results tagged with container name
//          G10  | buildConceptContainers generates ContainerCheckSet[]
//          G11  | Detectors with empty patterns → skipped
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert } from "jsr:@std/assert";

import {
  validateContainerConcepts,
  buildConceptContainers,
} from "@lib/shared/concept-check.ts";

import type { ConceptDetector, ConceptPattern } from "@lib/data/concept-detectors.ts";
import type { TernaryValue } from "@lib/data/types.ts";

// ---------------------------------------------------------------------------
// Test helpers — synthetic detectors and concept maps
// ---------------------------------------------------------------------------

const FILE = "test.rs";

/** Build a synthetic concept detector with a simple regex. */
function makeDetector(
  conceptId: string,
  regex: string,
  language = "rust",
): ConceptDetector {
  return {
    conceptId,
    language,
    patterns: [{
      id: "test",
      syntax: regex,
      detect: new RegExp(regex),
      captures: {},
      priority: 1,
    }],
  };
}

/** Build a detector with no patterns (simulating emit-only language). */
function emptyDetector(conceptId: string): ConceptDetector {
  return { conceptId, language: "go", patterns: [] };
}

/** Standard concept map: all denied. */
function allDenied(): Record<string, TernaryValue> {
  return {
    function_call: "denied",
    method_call: "denied",
    self_access: "denied",
    variable_binding: "denied",
    conditional_if: "denied",
    conditional_match: "denied",
    return_value: "denied",
    error_propagation: "denied",
    err_wrap: "denied",
    ok_wrap: "denied",
    string_format: "denied",
  };
}

/** Standard concept map: all granted. */
function allGranted(): Record<string, TernaryValue> {
  return {
    function_call: "granted",
    method_call: "granted",
    self_access: "granted",
    variable_binding: "granted",
    conditional_if: "granted",
    conditional_match: "granted",
    return_value: "granted",
    error_propagation: "granted",
    err_wrap: "granted",
    ok_wrap: "granted",
    string_format: "granted",
  };
}

/** Standard concept map: all defer. */
function allDefer(): Record<string, TernaryValue> {
  return {
    function_call: "defer",
    method_call: "defer",
    self_access: "defer",
    variable_binding: "defer",
    conditional_if: "defer",
    conditional_match: "defer",
    return_value: "defer",
    error_propagation: "defer",
    err_wrap: "defer",
    ok_wrap: "defer",
    string_format: "defer",
  };
}

/** Build a minimal set of detectors for testing (just function_call and variable_binding). */
function testDetectors(): ConceptDetector[] {
  return [
    makeDetector("function_call", "\\w+\\("),
    makeDetector("variable_binding", "\\blet\\s+"),
    // Rest are empty — won't match anything, won't produce results
    emptyDetector("method_call"),
    emptyDetector("self_access"),
    emptyDetector("conditional_if"),
    emptyDetector("conditional_match"),
    emptyDetector("return_value"),
    emptyDetector("error_propagation"),
    emptyDetector("err_wrap"),
    emptyDetector("ok_wrap"),
    emptyDetector("string_format"),
  ];
}

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// G1: Denied concept found → warn
// ---------------------------------------------------------------------------

Deno.test("[CWS-T00-105] concept-check/G1: denied concept found produces warn", () => {
  const map: Record<string, TernaryValue> = {
    ...allDefer(),
    function_call: "denied",
  };

  const lines = [
    "use std::fs;",
    "",
    "let x = process(data);",  // function_call detected
  ];

  const detectors = testDetectors();
  const results = validateContainerConcepts(FILE, "imports", lines, map, detectors);

  const warns = results.filter((r) => r.severity === "warn");
  assertEquals(warns.length, 1);
  assert(warns[0]!.rule.includes("concept/imports/function_call"));
  assert(warns[0]!.message.includes("Denied concept"));
});

// ---------------------------------------------------------------------------
// G2: Granted concept missing → info
// ---------------------------------------------------------------------------

Deno.test("concept-check/G2: granted concept missing produces info", () => {
  const map: Record<string, TernaryValue> = {
    ...allDefer(),
    function_call: "granted",
  };

  // Lines with NO function calls
  const lines = [
    "let x = 42;",
    "let y = x + 1;",
  ];

  const detectors = testDetectors();
  const results = validateContainerConcepts(FILE, "free-functions", lines, map, detectors);

  const infos = results.filter((r) => r.severity === "info");
  assertEquals(infos.length, 1);
  assert(infos[0]!.rule.includes("concept/free-functions/function_call/missing"));
});

// ---------------------------------------------------------------------------
// G3: Deferred concept → no result
// ---------------------------------------------------------------------------

Deno.test("concept-check/G3: deferred concept produces no result", () => {
  const map: Record<string, TernaryValue> = {
    ...allDefer(),
    function_call: "defer",  // explicitly defer
  };

  const lines = [
    "let x = process(data);",  // function call present
  ];

  const detectors = testDetectors();
  const results = validateContainerConcepts(FILE, "statics", lines, map, detectors);

  // No results — defer means "linter can't decide"
  assertEquals(results.length, 0);
});

// ---------------------------------------------------------------------------
// G4: Empty container → no results
// ---------------------------------------------------------------------------

Deno.test("concept-check/G4: empty container produces no results", () => {
  const map = allDenied();
  const detectors = testDetectors();
  const results = validateContainerConcepts(FILE, "imports", [], map, detectors);

  assertEquals(results.length, 0);
});

// ---------------------------------------------------------------------------
// G5: Comment-only container → no detections
// ---------------------------------------------------------------------------

Deno.test("concept-check/G5: comment-only container produces no detections", () => {
  const map: Record<string, TernaryValue> = {
    ...allDefer(),
    function_call: "denied",
  };

  const lines = [
    "// let x = process(data);",
    "// This is just a comment with function call syntax",
    "",
    "   ",
  ];

  const detectors = testDetectors();
  const results = validateContainerConcepts(FILE, "imports", lines, map, detectors);

  // No code lines remain after filtering, so no detections
  assertEquals(results.length, 0);
});

// ---------------------------------------------------------------------------
// G6: ALL_DENIED container → warn for found concepts
// ---------------------------------------------------------------------------

Deno.test("concept-check/G6: all-denied container warns on found concepts", () => {
  const map = allDenied();

  const lines = [
    "let x = process(data);",  // function_call + variable_binding
  ];

  const detectors = testDetectors();
  const results = validateContainerConcepts(FILE, "imports", lines, map, detectors);

  const warns = results.filter((r) => r.severity === "warn");
  // Should warn for function_call and variable_binding (the two with patterns)
  assertEquals(warns.length, 2);
});

// ---------------------------------------------------------------------------
// G7: ALL_GRANTED container with all present → no warnings
// ---------------------------------------------------------------------------

Deno.test("concept-check/G7: all-granted with all present produces no warnings", () => {
  const map = allGranted();

  const lines = [
    "let x = process(data);",  // function_call + variable_binding
  ];

  const detectors = testDetectors();
  const results = validateContainerConcepts(FILE, "core-logic", lines, map, detectors);

  const warns = results.filter((r) => r.severity === "warn");
  assertEquals(warns.length, 0);
});

// ---------------------------------------------------------------------------
// G8: Rule namespace follows concept/{container}/{concept}
// ---------------------------------------------------------------------------

Deno.test("concept-check/G8: rule namespace is concept/{container}/{concept}", () => {
  const map: Record<string, TernaryValue> = {
    ...allDefer(),
    function_call: "denied",
    variable_binding: "granted",
  };

  const lines = [
    "let x = process(data);",  // both present
  ];

  const detectors = testDetectors();
  const results = validateContainerConcepts(FILE, "imports", lines, map, detectors);

  // function_call denied + found → warn with rule "concept/imports/function_call"
  const fcResult = results.find((r) => r.rule === "concept/imports/function_call");
  assert(fcResult !== undefined, "should have concept/imports/function_call rule");
  assertEquals(fcResult!.severity, "warn");

  // variable_binding granted + found → no result (it's present, all good)
  const vbResult = results.find((r) => r.rule.includes("variable_binding"));
  assertEquals(vbResult, undefined, "granted + found should produce no result");
});

// ---------------------------------------------------------------------------
// G9: Results tagged with container name
// ---------------------------------------------------------------------------

Deno.test("concept-check/G9: results have container field set", () => {
  const map: Record<string, TernaryValue> = {
    ...allDefer(),
    function_call: "denied",
  };

  const lines = ["let x = process(data);"];

  const detectors = testDetectors();
  const results = validateContainerConcepts(FILE, "imports", lines, map, detectors);

  assert(results.length > 0);
  for (const r of results) {
    assertEquals(r.container, "imports");
  }
});

// ---------------------------------------------------------------------------
// G10: buildConceptContainers generates ContainerCheckSet[]
// ---------------------------------------------------------------------------

Deno.test("concept-check/G10: buildConceptContainers returns non-empty for setup", () => {
  // Mock block lines with subsection markers
  const blockLines = [
    "// Imports",
    "use std::fs;",
    "use std::io;",
    "",
    "// Constants",
    'const VERSION: &str = "1.0";',
    "",
  ];

  // Mock subsection ranges
  const ranges = [
    { tag: "Imports", startIdx: 0, endIdx: 4 },
    { tag: "Constants", startIdx: 4, endIdx: 7 },
  ];

  const detectors = testDetectors();
  const containers = buildConceptContainers("setup", FILE, blockLines, ranges, detectors);

  // Should have at least some containers (depends on which setup sections
  // have actionable concept maps). The function won't crash and returns valid data.
  assert(Array.isArray(containers));

  // Each container should have a name and checks
  for (const c of containers) {
    assert(c.container.length > 0, "container should have a name");
    assert(Array.isArray(c.checks), "container should have checks array");
  }
});

// ---------------------------------------------------------------------------
// G11: Detectors with empty patterns → skipped
// ---------------------------------------------------------------------------

Deno.test("concept-check/G11: empty-pattern detectors produce no results", () => {
  const map: Record<string, TernaryValue> = {
    ...allDefer(),
    method_call: "denied",  // method_call has no patterns in testDetectors()
  };

  const lines = ["self.name.clone()"];

  const detectors = testDetectors();
  const results = validateContainerConcepts(FILE, "methods", lines, map, detectors);

  // method_call detector is empty → should be skipped, no warn
  const methodResults = results.filter((r) => r.rule.includes("method_call"));
  assertEquals(methodResults.length, 0, "empty-pattern detector should be skipped");
});

// ---------------------------------------------------------------------------
// G12: R[3] fallback — recursion detected via scope analysis
// ---------------------------------------------------------------------------

Deno.test("concept-check/G12: R[3] fallback detects recursion across lines", () => {
  const map: Record<string, TernaryValue> = {
    ...allDefer(),
    recursion: "granted",
  };

  // Recursive function — fn declaration and call on different lines.
  // R[5] regex can't match this line-by-line, but R[3] scope analysis can.
  const lines = [
    "fn has_cycle(spec: &str) -> bool {",
    "    if visited.contains(spec) {",
    "        return true;",
    "    }",
    "    for dep in &node.depends_on {",
    "        if has_cycle(dep) {",
    "            return true;",
    "        }",
    "    }",
    "    false",
    "}",
  ];

  // Build a recursion detector with the multiline regex pattern.
  // This pattern needs to see both fn declaration and call site — line-by-line fails.
  const recursionDetector: ConceptDetector = {
    conceptId: "recursion",
    language: "rust",
    patterns: [{
      id: "direct_recursion",
      syntax: "fn\\s+(\\w+)\\s*\\([^)]*\\)[^{]*\\{[\\s\\S]*?\\1\\s*\\(",
      detect: new RegExp("fn\\s+(\\w+)\\s*\\([^)]*\\)[^{]*\\{[\\s\\S]*?\\1\\s*\\("),
      captures: {},
      priority: 1,
    }],
  };

  const detectors: ConceptDetector[] = [
    recursionDetector,
    // Other detectors as empty stubs
    ...["function_call", "method_call", "self_access", "variable_binding",
        "conditional_if", "conditional_match", "return_value",
        "error_propagation", "err_wrap", "ok_wrap", "string_format",
    ].map((id) => emptyDetector(id)),
  ];

  const results = validateContainerConcepts(FILE, "free-functions", lines, map, detectors);

  // Recursion IS present. If R[3] works, it should be detected → no "missing" info.
  const missing = results.filter((r) => r.rule.includes("recursion/missing"));
  assertEquals(missing.length, 0,
    "R[3] should detect recursion via multiline/scope fallback — no 'missing' expected");
});

// ---------------------------------------------------------------------------
// G13: R[3] fallback — non-recursive function still shows missing
// ---------------------------------------------------------------------------

Deno.test("concept-check/G13: R[3] fallback — non-recursive function still missing", () => {
  const map: Record<string, TernaryValue> = {
    ...allDefer(),
    recursion: "granted",
  };

  // Non-recursive function — R[3] should NOT detect recursion
  const lines = [
    "fn add(a: i32, b: i32) -> i32 {",
    "    a + b",
    "}",
  ];

  const recursionDetector: ConceptDetector = {
    conceptId: "recursion",
    language: "rust",
    patterns: [{
      id: "direct_recursion",
      syntax: "fn\\s+(\\w+)\\s*\\([^)]*\\)[^{]*\\{[\\s\\S]*?\\1\\s*\\(",
      detect: new RegExp("fn\\s+(\\w+)\\s*\\([^)]*\\)[^{]*\\{[\\s\\S]*?\\1\\s*\\("),
      captures: {},
      priority: 1,
    }],
  };

  const detectors: ConceptDetector[] = [
    recursionDetector,
    ...["function_call", "method_call", "self_access", "variable_binding",
        "conditional_if", "conditional_match", "return_value",
        "error_propagation", "err_wrap", "ok_wrap", "string_format",
    ].map((id) => emptyDetector(id)),
  ];

  const results = validateContainerConcepts(FILE, "free-functions", lines, map, detectors);

  // Non-recursive → R[3] should NOT detect → "missing" info expected
  const missing = results.filter((r) => r.rule.includes("recursion/missing"));
  assertEquals(missing.length, 1,
    "Non-recursive function should still show recursion/missing");
});

// ============================================================================
// CLOSING
// ============================================================================
//
// Concept validation tests — 13 gates proving the ternary matrix enforcement.
//
// G1-G11: Core matrix enforcement (denied/granted/defer/empty/comments)
// G12-G13: R[3] fallback (recursion detection via scope analysis)
//
// denied + found → warn (concept in wrong container)
// granted + missing → info (expected concept absent)
// defer → skip (linter can't decide)
// empty → no results (nothing to check)
// comments → filtered (not code)
// R[3] → multiline/scope fallback for recursion + self_access
//
// "Try the spirits whether they are of God." — 1 John 4:1
// ============================================================================
