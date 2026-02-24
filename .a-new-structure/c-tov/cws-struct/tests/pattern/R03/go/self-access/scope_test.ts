// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/R03/go/self-access/scope_test.ts
// key:     B-tov-cws-struct-tests-R03-go-self-access
// title:   CWS Struct — R[3] Go Self-Access Detection Tests
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-23
// authors: Nova Dawn (CPI-SI)
// purpose: Format-level tests proving R[3] scope analysis detects Go receiver
//          parameter usage (self_access) that R[5] regex cannot detect.
//
//          Go receiver access (n.Name) is syntactically identical to any
//          variable.Field — regex can't distinguish receiver from local var.
//          R[3] parses function signatures to identify receiver params.
//
//          Gate | Verifies
//          -----|----------------------------------------
//          G1   | Go receiver method — self_access detected via R[3]
//          G2   | Go free function — no false self_access detection
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals } from "jsr:@std/assert";
import { fixture, lazyFormat, byRule } from "@test/helpers.ts";

// ============================================================================
// BODY
// ============================================================================

const go = lazyFormat("go");

// ---------------------------------------------------------------------------
// G1: Receiver method fixture — self_access detected via R[3]
// ---------------------------------------------------------------------------

Deno.test("[CWS-T00-106] R03/go/self-access/G1: receiver method — self_access concept detected", async () => {
  const results = await go.lint(fixture("go/scope/receiver-method.go"));

  // The fixture has receiver methods (n.Name, n.Children, s.Logger, etc.).
  // Go has NO R[5] regex for self_access (removed in Phase 2 — too many false positives).
  // R[3] scope analysis parses func (n *Node) and checks for n.Field usage.
  //
  // If self_access IS detected, there should be NO "self_access/missing" info
  // in containers where it's granted.
  const selfAccessMissing = byRule(results, "concept/").filter(
    (r) => r.rule.includes("self_access/missing"),
  );

  // Note: self_access may be granted in some containers and not others.
  // Core Logic container should have self_access detected (receiver methods live there).
  const coreLogicMissing = selfAccessMissing.filter(
    (r) => r.rule.includes("core-logic"),
  );
  assertEquals(coreLogicMissing.length, 0,
    "Receiver methods in Core Logic should have self_access detected by R[3]");
});

// ---------------------------------------------------------------------------
// G2: Free function — no false self_access detection
// ---------------------------------------------------------------------------

Deno.test("R03/go/self-access/G2: free function only — no false self_access in free-functions", async () => {
  const results = await go.lint(fixture("go/scope/receiver-method.go"));

  // Free Functions section has CountNodes(node *Node) — no receiver.
  // self_access should NOT be detected in the Free Functions container
  // (CountNodes uses node.Children, but node is a parameter, not a receiver).
  const freeFnSelfAccessDenied = byRule(results, "concept/free-functions/self_access")
    .filter((r) => r.severity === "warn");
  assertEquals(freeFnSelfAccessDenied.length, 0,
    "Free function parameter usage should not trigger self_access denied warning");
});

// ============================================================================
// CLOSING
// ============================================================================
//
// R[3] Go self_access format-level tests — 2 gates proving scope analysis
// correctly detects receiver parameter usage in Go files.
//
// Go self_access is the ONE concept that has zero R[5] regex patterns.
// R[3] is the ONLY detection path. This is the proof it works.
//
// "Day unto day uttereth speech." — Psalm 19:2
// ============================================================================
