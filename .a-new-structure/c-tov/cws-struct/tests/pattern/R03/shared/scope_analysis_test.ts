// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/T00/R03/shared/scope_analysis_test.ts
// key:     B-tov-cws-struct-tests-T00-R03-shared-scope-analysis
// title:   CWS Struct — R[3] Scope Analysis Unit Tests
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-23
// authors: Nova Dawn (CPI-SI)
// purpose: Unit tests for function-scope-aware concept detection (R[3] layer).
//
//          Tests the scope analysis module directly — parseFunctionScopes,
//          detectConceptMultiline, detectConceptByScope, hasR3Detection.
//
//          Gate | Verifies
//          -----|----------------------------------------
//          G1   | parseFunctionScopes extracts Rust function names + bodies
//          G2   | parseFunctionScopes extracts Go function names + receiver params
//          G3   | detectConceptByScope("recursion") finds self-calling functions
//          G4   | detectConceptByScope("recursion") rejects non-recursive functions
//          G5   | detectConceptByScope("self_access") finds Go receiver usage
//          G6   | detectConceptByScope("self_access") rejects non-receiver usage
//          G7   | detectConceptMultiline joins lines and matches regex
//          G8   | detectConceptMultiline rejects when no pattern spans lines
//          G9   | hasR3Detection returns true for recursion (any language)
//          G10  | hasR3Detection returns true for self_access (Go only)
//          G11  | hasR3Detection returns false for concepts without scope detection
//          G12  | parseFunctionScopes handles nested braces correctly
//          G13  | parseFunctionScopes skips string literals containing braces
//          G14  | parseFunctionScopes handles multiple functions in sequence
//          G15  | detectConceptByScope returns false for empty input
//          G16  | detectConceptByScope returns false for unknown concepts
//
// biblical_foundation: "Day unto day uttereth speech, and night unto night
//   sheweth knowledge." — Psalm 19:2
//   Each detection layer speaks what it can see.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert } from "jsr:@std/assert";

import {
  parseFunctionScopes,
  detectConceptMultiline,
  detectConceptByScope,
  hasR3Detection,
} from "@lib/shared/scope-analysis.ts";

import type { FunctionScope } from "@lib/shared/scope-analysis.ts";
import type { ConceptDetector } from "@lib/data/concept-detectors.ts";

// ---------------------------------------------------------------------------
// Test helpers
// ---------------------------------------------------------------------------

/** Build a synthetic detector with a regex pattern. */
function makeDetector(conceptId: string, regex: string, language = "rust"): ConceptDetector {
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

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// G1: parseFunctionScopes — Rust extraction
// ---------------------------------------------------------------------------

Deno.test("[CWS-T00-106] scope-analysis/G1: Rust — extracts function name and body lines", () => {
  const lines = [
    "/// Compute factorial.",
    "fn factorial(n: u64) -> u64 {",
    "    if n <= 1 {",
    "        return 1;",
    "    }",
    "    n * factorial(n - 1)",
    "}",
  ];

  const scopes = parseFunctionScopes(lines, "rust");
  assertEquals(scopes.length, 1);
  assertEquals(scopes[0]!.name, "factorial");
  assertEquals(scopes[0]!.receiverParam, undefined);
  assertEquals(scopes[0]!.declarationLine, 1); // 0-indexed, skipping doc comment
  assert(scopes[0]!.bodyLines.length > 0, "should have body lines");
});

Deno.test("scope-analysis/G1: Rust — extracts pub(crate) fn", () => {
  const lines = [
    "pub(crate) fn build_graph(data: &str) -> Graph {",
    "    let g = Graph::new();",
    "    g",
    "}",
  ];

  const scopes = parseFunctionScopes(lines, "rust");
  assertEquals(scopes.length, 1);
  assertEquals(scopes[0]!.name, "build_graph");
});

Deno.test("scope-analysis/G1: Rust — extracts pub fn", () => {
  const lines = [
    "pub fn walk_tree(node: &str) {",
    "    println!(\"{}\", node);",
    "}",
  ];

  const scopes = parseFunctionScopes(lines, "rust");
  assertEquals(scopes.length, 1);
  assertEquals(scopes[0]!.name, "walk_tree");
});

// ---------------------------------------------------------------------------
// G2: parseFunctionScopes — Go extraction with receiver
// ---------------------------------------------------------------------------

Deno.test("scope-analysis/G2: Go — extracts function with receiver param", () => {
  const lines = [
    "func (n *Node) Print() {",
    "    fmt.Println(n.Name)",
    "}",
  ];

  const scopes = parseFunctionScopes(lines, "go");
  assertEquals(scopes.length, 1);
  assertEquals(scopes[0]!.name, "Print");
  assertEquals(scopes[0]!.receiverParam, "n");
  assert(scopes[0]!.bodyLines.length > 0);
});

Deno.test("scope-analysis/G2: Go — extracts free function (no receiver)", () => {
  const lines = [
    "func CountNodes(node *Node) int {",
    "    return 1",
    "}",
  ];

  const scopes = parseFunctionScopes(lines, "go");
  assertEquals(scopes.length, 1);
  assertEquals(scopes[0]!.name, "CountNodes");
  assertEquals(scopes[0]!.receiverParam, undefined);
});

// ---------------------------------------------------------------------------
// G3: detectConceptByScope("recursion") — positive
// ---------------------------------------------------------------------------

Deno.test("scope-analysis/G3: recursion — Rust function calling itself", () => {
  const lines = [
    "fn has_cycle(spec: &str, graph: &Graph) -> bool {",
    "    if visited.contains(spec) {",
    "        return true;",
    "    }",
    "    for dep in &node.depends_on {",
    "        if has_cycle(dep, graph) {",
    "            return true;",
    "        }",
    "    }",
    "    false",
    "}",
  ];

  const result = detectConceptByScope("recursion", lines, "rust");
  assertEquals(result, true, "should detect recursive call to has_cycle");
});

Deno.test("scope-analysis/G3: recursion — Go function calling itself", () => {
  const lines = [
    "func CountNodes(node *Node) int {",
    "    if node == nil {",
    "        return 0",
    "    }",
    "    count := 1",
    "    for _, child := range node.Children {",
    "        count += CountNodes(child)",
    "    }",
    "    return count",
    "}",
  ];

  const result = detectConceptByScope("recursion", lines, "go");
  assertEquals(result, true, "should detect recursive call to CountNodes");
});

Deno.test("scope-analysis/G3: recursion — Go receiver method calling itself", () => {
  const lines = [
    "func (n *Node) Walk(visit func(string)) {",
    "    visit(n.Name)",
    "    for _, child := range n.Children {",
    "        child.Walk(visit)",
    "    }",
    "}",
  ];

  // Note: child.Walk() is NOT n.Walk() — this is a method call on a different
  // receiver. The current scope analysis doesn't track this as recursion because
  // it checks for `Walk(` as a standalone call, not `*.Walk(`.
  // This documents the current behavior — method recursion via different
  // receivers is a future enhancement.
  const result = detectConceptByScope("recursion", lines, "go");
  // The regex `\bWalk\s*\(` WILL match `child.Walk(visit)` because
  // \b matches at the boundary between . and W. So this IS detected.
  assertEquals(result, true, "should detect Walk called in Walk body");
});

// ---------------------------------------------------------------------------
// G4: detectConceptByScope("recursion") — negative
// ---------------------------------------------------------------------------

Deno.test("scope-analysis/G4: recursion — non-recursive function", () => {
  const lines = [
    "fn add(a: i32, b: i32) -> i32 {",
    "    a + b",
    "}",
  ];

  const result = detectConceptByScope("recursion", lines, "rust");
  assertEquals(result, false, "simple add should not be detected as recursive");
});

Deno.test("scope-analysis/G4: recursion — function calling different function", () => {
  const lines = [
    "fn process(data: &str) -> Result<()> {",
    "    let parsed = parse(data)?;",
    "    validate(parsed)",
    "}",
  ];

  const result = detectConceptByScope("recursion", lines, "rust");
  assertEquals(result, false, "calling different functions is not recursion");
});

// ---------------------------------------------------------------------------
// G5: detectConceptByScope("self_access") — positive (Go)
// ---------------------------------------------------------------------------

Deno.test("scope-analysis/G5: self_access — Go receiver field access", () => {
  const lines = [
    "func (n *Node) Print() {",
    "    fmt.Println(n.Name)",
    "}",
  ];

  const result = detectConceptByScope("self_access", lines, "go");
  assertEquals(result, true, "n.Name should be detected as self_access");
});

Deno.test("scope-analysis/G5: self_access — Go receiver method chain", () => {
  const lines = [
    "func (s *Server) Start() error {",
    "    s.Logger.Info(\"starting\")",
    "    return s.Listen()",
    "}",
  ];

  const result = detectConceptByScope("self_access", lines, "go");
  assertEquals(result, true, "s.Logger and s.Listen should be detected as self_access");
});

// ---------------------------------------------------------------------------
// G6: detectConceptByScope("self_access") — negative
// ---------------------------------------------------------------------------

Deno.test("scope-analysis/G6: self_access — Go function without receiver", () => {
  const lines = [
    "func CountNodes(node *Node) int {",
    "    return node.Count()",
    "}",
  ];

  const result = detectConceptByScope("self_access", lines, "go");
  assertEquals(result, false, "free function has no receiver — not self_access");
});

Deno.test("scope-analysis/G6: self_access — Rust (not applicable)", () => {
  const lines = [
    "fn process(data: &str) -> bool {",
    "    data.len() > 0",
    "}",
  ];

  // self_access scope detection is Go-only (Rust uses self.field regex)
  const result = detectConceptByScope("self_access", lines, "rust");
  assertEquals(result, false, "Rust self_access is regex-based, not scope-based");
});

// ---------------------------------------------------------------------------
// G7: detectConceptMultiline — positive
// ---------------------------------------------------------------------------

Deno.test("scope-analysis/G7: multiline — regex matches across joined lines", () => {
  // Simulate a recursion regex that needs to see fn declaration + call
  const detector = makeDetector("recursion", "fn\\s+(\\w+)\\s*\\([^)]*\\)[^{]*\\{[\\s\\S]*?\\1\\s*\\(");

  const lines = [
    "fn factorial(n: u64) -> u64 {",
    "    if n <= 1 { return 1; }",
    "    n * factorial(n - 1)",
    "}",
  ];

  const result = detectConceptMultiline(detector, lines);
  assertEquals(result, true, "joined lines should match fn...factorial...factorial(");
});

// ---------------------------------------------------------------------------
// G8: detectConceptMultiline — negative
// ---------------------------------------------------------------------------

Deno.test("scope-analysis/G8: multiline — no match when pattern absent", () => {
  const detector = makeDetector("recursion", "fn\\s+(\\w+)\\s*\\([^)]*\\)[^{]*\\{[\\s\\S]*?\\1\\s*\\(");

  const lines = [
    "fn add(a: i32, b: i32) -> i32 {",
    "    a + b",
    "}",
  ];

  const result = detectConceptMultiline(detector, lines);
  assertEquals(result, false, "non-recursive function should not match");
});

Deno.test("scope-analysis/G8: multiline — empty patterns return false", () => {
  const detector: ConceptDetector = {
    conceptId: "test",
    language: "rust",
    patterns: [],
  };

  const result = detectConceptMultiline(detector, ["let x = 1;"]);
  assertEquals(result, false, "empty patterns should return false");
});

// ---------------------------------------------------------------------------
// G9-G11: hasR3Detection
// ---------------------------------------------------------------------------

Deno.test("scope-analysis/G9: hasR3Detection — recursion (any language)", () => {
  assertEquals(hasR3Detection("recursion", "rust"), true);
  assertEquals(hasR3Detection("recursion", "go"), true);
  assertEquals(hasR3Detection("recursion", "unknown"), true);
});

Deno.test("scope-analysis/G10: hasR3Detection — self_access (Go only)", () => {
  assertEquals(hasR3Detection("self_access", "go"), true);
  assertEquals(hasR3Detection("self_access", "rust"), false);
});

Deno.test("scope-analysis/G11: hasR3Detection — other concepts return false", () => {
  assertEquals(hasR3Detection("function_call", "rust"), false);
  assertEquals(hasR3Detection("variable_binding", "go"), false);
  assertEquals(hasR3Detection("conditional_if", "rust"), false);
  assertEquals(hasR3Detection("error_propagation", "rust"), false);
});

// ---------------------------------------------------------------------------
// G12: parseFunctionScopes — nested braces
// ---------------------------------------------------------------------------

Deno.test("scope-analysis/G12: Rust — handles nested braces correctly", () => {
  const lines = [
    "fn complex(data: &str) -> Result<()> {",
    "    if data.is_empty() {",
    "        return Err(\"empty\".into());",
    "    }",
    "    match data {",
    "        \"a\" => {",
    "            println!(\"a\");",
    "        }",
    "        _ => {}",
    "    }",
    "    Ok(())",
    "}",
  ];

  const scopes = parseFunctionScopes(lines, "rust");
  assertEquals(scopes.length, 1, "should parse as one function despite nested braces");
  assertEquals(scopes[0]!.name, "complex");
});

// ---------------------------------------------------------------------------
// G13: parseFunctionScopes — string literals with braces
// ---------------------------------------------------------------------------

Deno.test("scope-analysis/G13: Rust — skips braces in string literals", () => {
  const lines = [
    'fn fmt_output(name: &str) -> String {',
    '    format!("Hello, {}!", name)',
    '}',
  ];

  const scopes = parseFunctionScopes(lines, "rust");
  assertEquals(scopes.length, 1, "string braces should not break parsing");
  assertEquals(scopes[0]!.name, "fmt_output");
});

// ---------------------------------------------------------------------------
// G14: parseFunctionScopes — multiple functions in sequence
// ---------------------------------------------------------------------------

Deno.test("scope-analysis/G14: Rust — extracts multiple sequential functions", () => {
  const lines = [
    "fn first() -> i32 {",
    "    1",
    "}",
    "",
    "fn second() -> i32 {",
    "    2",
    "}",
    "",
    "pub fn third(x: i32) -> bool {",
    "    x > 0",
    "}",
  ];

  const scopes = parseFunctionScopes(lines, "rust");
  assertEquals(scopes.length, 3);
  assertEquals(scopes[0]!.name, "first");
  assertEquals(scopes[1]!.name, "second");
  assertEquals(scopes[2]!.name, "third");
});

Deno.test("scope-analysis/G14: Go — extracts mixed receiver and free functions", () => {
  const lines = [
    "func (n *Node) Name() string {",
    "    return n.name",
    "}",
    "",
    "func Helper(x int) int {",
    "    return x + 1",
    "}",
  ];

  const scopes = parseFunctionScopes(lines, "go");
  assertEquals(scopes.length, 2);
  assertEquals(scopes[0]!.name, "Name");
  assertEquals(scopes[0]!.receiverParam, "n");
  assertEquals(scopes[1]!.name, "Helper");
  assertEquals(scopes[1]!.receiverParam, undefined);
});

// ---------------------------------------------------------------------------
// G15: detectConceptByScope — empty input
// ---------------------------------------------------------------------------

Deno.test("scope-analysis/G15: empty input returns false", () => {
  assertEquals(detectConceptByScope("recursion", [], "rust"), false);
  assertEquals(detectConceptByScope("self_access", [], "go"), false);
});

// ---------------------------------------------------------------------------
// G16: detectConceptByScope — unknown concept
// ---------------------------------------------------------------------------

Deno.test("scope-analysis/G16: unknown concept returns false", () => {
  const lines = [
    "fn something() {",
    "    something()",
    "}",
  ];

  assertEquals(detectConceptByScope("nonexistent", lines, "rust"), false);
});

// ============================================================================
// CLOSING
// ============================================================================
//
// R[3] scope analysis unit tests — 16 gates proving function-scope-aware
// concept detection works correctly.
//
// parseFunctionScopes: Rust fn/pub fn/pub(crate) fn, Go func/receiver func
// detectConceptByScope: recursion (both), self_access (Go only)
// detectConceptMultiline: regex-over-joined-text fallback
// hasR3Detection: concept → layer routing
//
// "Day unto day uttereth speech." — Psalm 19:2
// Each layer speaks what it can see.
// ============================================================================
