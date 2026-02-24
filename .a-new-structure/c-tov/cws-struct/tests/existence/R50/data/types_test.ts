// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/T00/R50/data/types_test.ts
// key:     B-tov-cws-struct-tests-data-types
// title:   CWS Struct — Data Layer Type Guard Tests
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-22
// authors: Nathan Emet (CPI-SI)
// purpose: Verify type guards and validation helpers in lib/data/types.ts.
//          These are the building blocks — if the guards are wrong,
//          every registry test built on top of them is unreliable.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert, assertFalse } from "jsr:@std/assert";

import {
  isValidTernary,
  isValidBlock,
  isValidFormStatus,
  isValidLayer,
  isValidWeight,
  validateConceptMap,
  verifyTranspose,
} from "@lib/data/types.ts";

import type {
  SectionEntry,
  ConceptEntry,
  TernaryValue,
} from "@lib/data/types.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// isValidTernary
// ---------------------------------------------------------------------------

Deno.test("[CWS-T00-073] data/types/isValidTernary: granted is valid", () => {
  assert(isValidTernary("granted"));
});

Deno.test("data/types/isValidTernary: denied is valid", () => {
  assert(isValidTernary("denied"));
});

Deno.test("data/types/isValidTernary: defer is valid", () => {
  assert(isValidTernary("defer"));
});

Deno.test("data/types/isValidTernary: empty string is invalid", () => {
  assertFalse(isValidTernary(""));
});

Deno.test("data/types/isValidTernary: 'true' is not a ternary value", () => {
  assertFalse(isValidTernary("true"));
});

Deno.test("data/types/isValidTernary: 'GRANTED' (uppercase) is invalid", () => {
  assertFalse(isValidTernary("GRANTED"));
});

Deno.test("data/types/isValidTernary: 'allow' is not a ternary value", () => {
  assertFalse(isValidTernary("allow"));
});

// ---------------------------------------------------------------------------
// isValidBlock
// ---------------------------------------------------------------------------

Deno.test("data/types/isValidBlock: metadata is valid", () => {
  assert(isValidBlock("metadata"));
});

Deno.test("data/types/isValidBlock: setup is valid", () => {
  assert(isValidBlock("setup"));
});

Deno.test("data/types/isValidBlock: body is valid", () => {
  assert(isValidBlock("body"));
});

Deno.test("data/types/isValidBlock: closing is valid", () => {
  assert(isValidBlock("closing"));
});

Deno.test("data/types/isValidBlock: 'METADATA' (uppercase) is invalid", () => {
  assertFalse(isValidBlock("METADATA"));
});

Deno.test("data/types/isValidBlock: 'header' is not a valid block", () => {
  assertFalse(isValidBlock("header"));
});

Deno.test("data/types/isValidBlock: empty string is invalid", () => {
  assertFalse(isValidBlock(""));
});

// ---------------------------------------------------------------------------
// isValidFormStatus
// ---------------------------------------------------------------------------

Deno.test("data/types/isValidFormStatus: required is valid", () => {
  assert(isValidFormStatus("required"));
});

Deno.test("data/types/isValidFormStatus: available is valid", () => {
  assert(isValidFormStatus("available"));
});

Deno.test("data/types/isValidFormStatus: reserved is valid", () => {
  assert(isValidFormStatus("reserved"));
});

Deno.test("data/types/isValidFormStatus: 'REQUIRED' (uppercase) is invalid", () => {
  assertFalse(isValidFormStatus("REQUIRED"));
});

Deno.test("data/types/isValidFormStatus: 'optional' is not a valid status", () => {
  assertFalse(isValidFormStatus("optional"));
});

// ---------------------------------------------------------------------------
// isValidLayer
// ---------------------------------------------------------------------------

Deno.test("data/types/isValidLayer: 0 through 3 are valid", () => {
  assert(isValidLayer(0));
  assert(isValidLayer(1));
  assert(isValidLayer(2));
  assert(isValidLayer(3));
});

Deno.test("data/types/isValidLayer: negative is invalid", () => {
  assertFalse(isValidLayer(-1));
});

Deno.test("data/types/isValidLayer: 4 is out of range", () => {
  assertFalse(isValidLayer(4));
});

Deno.test("data/types/isValidLayer: 1.5 (non-integer) is invalid", () => {
  assertFalse(isValidLayer(1.5));
});

// ---------------------------------------------------------------------------
// isValidWeight
// ---------------------------------------------------------------------------

Deno.test("data/types/isValidWeight: 0 is valid (minimum)", () => {
  assert(isValidWeight(0));
});

Deno.test("data/types/isValidWeight: 1 is valid (maximum)", () => {
  assert(isValidWeight(1));
});

Deno.test("data/types/isValidWeight: 0.73 is valid (mid-range)", () => {
  assert(isValidWeight(0.73));
});

Deno.test("data/types/isValidWeight: -0.01 is invalid (below range)", () => {
  assertFalse(isValidWeight(-0.01));
});

Deno.test("data/types/isValidWeight: 1.01 is invalid (above range)", () => {
  assertFalse(isValidWeight(1.01));
});

// ---------------------------------------------------------------------------
// validateConceptMap
// ---------------------------------------------------------------------------

Deno.test("data/types/validateConceptMap: all valid values returns empty array", () => {
  const map: Record<string, string> = {
    function_call: "granted",
    method_call: "denied",
    self_access: "defer",
    variable_binding: "granted",
    conditional_if: "denied",
    conditional_match: "denied",
    return_value: "granted",
    error_propagation: "denied",
    err_wrap: "denied",
    ok_wrap: "denied",
    string_format: "defer",
  };
  const invalid = validateConceptMap(map);
  assertEquals(invalid.length, 0);
});

Deno.test("data/types/validateConceptMap: invalid value is reported", () => {
  const map: Record<string, string> = {
    function_call: "yes",
  };
  const invalid = validateConceptMap(map);
  assertEquals(invalid.length, 1);
  assert(invalid[0]!.includes("function_call"));
  assert(invalid[0]!.includes("yes"));
});

Deno.test("data/types/validateConceptMap: multiple invalid values all reported", () => {
  const map: Record<string, string> = {
    function_call: "true",
    method_call: "false",
    self_access: "granted", // valid
  };
  const invalid = validateConceptMap(map);
  assertEquals(invalid.length, 2);
});

Deno.test("data/types/validateConceptMap: empty map returns empty (no values to validate)", () => {
  const invalid = validateConceptMap({});
  assertEquals(invalid.length, 0);
});

// ---------------------------------------------------------------------------
// verifyTranspose
// ---------------------------------------------------------------------------

Deno.test("data/types/verifyTranspose: consistent data returns empty array", () => {
  const sections: Record<string, SectionEntry> = {
    "imports": {
      tag: "imports",
      block: "setup",
      position: 1,
      nature: "dependency-declaration",
      description: "Import declarations",
      conceptMap: { "function_call": "denied" as TernaryValue },
      detectionWeight: 1.0,
      formStatus: {},
    },
  };
  const concepts: Record<string, ConceptEntry> = {
    "function_call": {
      id: "function_call",
      dirName: "function-call",
      description: "Function call pattern",
      containerMap: { "imports": "denied" as TernaryValue },
    },
  };
  const mismatches = verifyTranspose(sections, concepts);
  assertEquals(mismatches.length, 0);
});

Deno.test("data/types/verifyTranspose: mismatch is detected", () => {
  const sections: Record<string, SectionEntry> = {
    "imports": {
      tag: "imports",
      block: "setup",
      position: 1,
      nature: "dependency-declaration",
      description: "Import declarations",
      conceptMap: { "function_call": "denied" as TernaryValue },
      detectionWeight: 1.0,
      formStatus: {},
    },
  };
  const concepts: Record<string, ConceptEntry> = {
    "function_call": {
      id: "function_call",
      dirName: "function-call",
      description: "Function call pattern",
      containerMap: { "imports": "granted" as TernaryValue }, // MISMATCH
    },
  };
  const mismatches = verifyTranspose(sections, concepts);
  assertEquals(mismatches.length, 1);
  assert(mismatches[0]!.includes("transpose mismatch"));
  assert(mismatches[0]!.includes("imports"));
  assert(mismatches[0]!.includes("function_call"));
});

Deno.test("data/types/verifyTranspose: unknown concept is reported", () => {
  const sections: Record<string, SectionEntry> = {
    "imports": {
      tag: "imports",
      block: "setup",
      position: 1,
      nature: "dependency-declaration",
      description: "Import declarations",
      conceptMap: { "nonexistent_concept": "granted" as TernaryValue },
      detectionWeight: 1.0,
      formStatus: {},
    },
  };
  const concepts: Record<string, ConceptEntry> = {};
  const mismatches = verifyTranspose(sections, concepts);
  assertEquals(mismatches.length, 1);
  assert(mismatches[0]!.includes("unknown concept"));
});

Deno.test("data/types/verifyTranspose: missing container in concept is reported", () => {
  const sections: Record<string, SectionEntry> = {
    "imports": {
      tag: "imports",
      block: "setup",
      position: 1,
      nature: "dependency-declaration",
      description: "Import declarations",
      conceptMap: { "function_call": "denied" as TernaryValue },
      detectionWeight: 1.0,
      formStatus: {},
    },
  };
  const concepts: Record<string, ConceptEntry> = {
    "function_call": {
      id: "function_call",
      dirName: "function-call",
      description: "Function call pattern",
      containerMap: {}, // missing "imports" key
    },
  };
  const mismatches = verifyTranspose(sections, concepts);
  assertEquals(mismatches.length, 1);
  assert(mismatches[0]!.includes("missing container"));
});

Deno.test("data/types/verifyTranspose: multiple sections and concepts verified", () => {
  const sections: Record<string, SectionEntry> = {
    "imports": {
      tag: "imports",
      block: "setup",
      position: 1,
      nature: "dependency-declaration",
      description: "Import declarations",
      conceptMap: {
        "function_call": "denied" as TernaryValue,
        "method_call": "denied" as TernaryValue,
      },
      detectionWeight: 1.0,
      formStatus: {},
    },
    "constructors": {
      tag: "constructors",
      block: "body",
      position: 3,
      nature: "creation",
      description: "Constructor functions",
      conceptMap: {
        "function_call": "granted" as TernaryValue,
        "method_call": "granted" as TernaryValue,
      },
      detectionWeight: 0.91,
      formStatus: {},
    },
  };
  const concepts: Record<string, ConceptEntry> = {
    "function_call": {
      id: "function_call",
      dirName: "function-call",
      description: "Function call pattern",
      containerMap: {
        "imports": "denied" as TernaryValue,
        "constructors": "granted" as TernaryValue,
      },
    },
    "method_call": {
      id: "method_call",
      dirName: "method-call",
      description: "Method call pattern",
      containerMap: {
        "imports": "denied" as TernaryValue,
        "constructors": "granted" as TernaryValue,
      },
    },
  };
  const mismatches = verifyTranspose(sections, concepts);
  assertEquals(mismatches.length, 0);
});

// ============================================================================
// CLOSING
// ============================================================================
//
// Type guard tests — the foundation of data layer verification.
// If these pass, the guards work. If the guards work, the registry
// tests built on them are trustworthy.
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
