// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/integration/go/create_table_test.ts
// key:     B-tov-cws-struct-tests-integration-go-create-table
// title:   Go Create Tests — Table-Driven
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-24
// authors: Nova Dawn (CPI-SI)
// purpose: Table-driven Go file creation tests using _create_runner.ts.
//          Each row generates a file from schema, then verifies it lints
//          clean with 0 errors. The proof: schema = specification.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { runCreateSuite } from "@test/_create_runner.ts";
import type { CreateFormatSpec } from "@test/_create_runner.ts";

// ============================================================================
// BODY
// ============================================================================

const goCreateSpec: CreateFormatSpec = {
  format: "go",
  label: "go/create",
  cases: [
    // ── Library: the default form ───────────────────────────────────
    {
      subtype: "library",
      desc: "generate library file — lints clean",
      packageOrCrate: "testpkg",
      expectedBlocks: ["METADATA", "SETUP", "BODY", "CLOSING"],
      contentContains: ["package testpkg", "var Pragma"],
      contentAbsent: ["{{key}}", "{{title}}", "{{purpose}}"],
    },

    // ── Executable: package main + func main() ─────────────────────
    {
      subtype: "executable",
      desc: "generate executable file — lints clean",
      packageOrCrate: "main",
      expectedBlocks: ["METADATA", "SETUP", "BODY", "CLOSING"],
      contentContains: ["package main"],
      contentAbsent: ["{{key}}", "{{title}}"],
    },

    // ── Module: general-purpose ─────────────────────────────────────
    {
      subtype: "module",
      desc: "generate module file — lints clean",
      packageOrCrate: "testpkg",
      expectedBlocks: ["METADATA", "SETUP", "BODY", "CLOSING"],
      contentContains: ["package testpkg"],
      contentAbsent: ["{{key}}"],
    },

    // ── Bare-bone: minimal skeleton ─────────────────────────────────
    {
      subtype: "bare-bone",
      desc: "generate bare-bone file — lints clean",
      packageOrCrate: "testpkg",
      expectedBlocks: ["METADATA", "SETUP", "BODY", "CLOSING"],
      contentAbsent: ["{{key}}", "{{title}}"],
    },

    // ── Demo-test: test file skeleton ───────────────────────────────
    {
      subtype: "demo-test",
      desc: "generate demo-test file — lints clean",
      packageOrCrate: "testpkg",
      expectedBlocks: ["METADATA", "SETUP", "BODY", "CLOSING"],
      contentAbsent: ["{{key}}", "{{title}}"],
    },
  ],
};

runCreateSuite(goCreateSpec);

// ============================================================================
// CLOSING
// ============================================================================
//
// Table-driven Go create tests — 5 subtypes generating up to 5 tests.
// Each generated file verified: non-empty, lints clean, blocks present,
// no unfilled placeholders. The proof: schema IS specification.
//
// "And God saw every thing that he had made, and, behold, it was very good."
//   — Genesis 1:31
// ============================================================================
