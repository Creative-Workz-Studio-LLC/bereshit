// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/T00/verify/env_test.ts
// key:     B-tov-cws-struct-tests-verify-env
// title:   CWS Struct — Environment Verification Tests
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-17
// authors: Nathan Emet (CPI-SI)
// purpose: Tests for verifyEnvironment. Verifies the environment check
//          runs without crashing and returns a boolean. Cannot test exact
//          tool presence (environment-dependent), but can test the contract.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert } from "jsr:@std/assert";
import { verifyEnvironment } from "@lib/verify/env.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// verifyEnvironment — contract tests
// ---------------------------------------------------------------------------

Deno.test("[CWS-T00-130] env/verifyEnvironment: returns boolean without crashing (non-verbose)", async () => {
  const result = await verifyEnvironment(false);
  assertEquals(typeof result, "boolean", "Should return a boolean");
});

Deno.test("env/verifyEnvironment: returns boolean without crashing (verbose)", async () => {
  const result = await verifyEnvironment(true);
  assertEquals(typeof result, "boolean", "Should return a boolean");
});

Deno.test("env/verifyEnvironment: non-verbose and verbose return same result", async () => {
  const normal = await verifyEnvironment(false);
  const verbose = await verifyEnvironment(true);
  assertEquals(normal, verbose, "Verbosity should not change pass/fail result");
});

// ============================================================================
// CLOSING
// ============================================================================
//
// Environment verification tests — verifying the verifier runs cleanly.
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
