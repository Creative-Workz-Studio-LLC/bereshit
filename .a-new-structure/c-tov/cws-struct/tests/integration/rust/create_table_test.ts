// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/integration/rust/create_table_test.ts
// key:     B-tov-cws-struct-tests-integration-rust-create-table
// title:   Rust Create Tests — Table-Driven
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-24
// authors: Nova Dawn (CPI-SI)
// purpose: Table-driven Rust file creation tests using _create_runner.ts.
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

const rustCreateSpec: CreateFormatSpec = {
  format: "rust",
  label: "rust/create",
  cases: [
    // ── Library: pub crate root ─────────────────────────────────────
    {
      subtype: "library",
      desc: "generate library file — lints clean",
      packageOrCrate: "testcrate",
      expectedBlocks: ["METADATA", "SETUP", "BODY", "CLOSING"],
      contentContains: ["PRAGMA"],
      contentAbsent: ["{{key}}", "{{title}}", "{{purpose}}"],
    },

    // ── Executable: fn main() in CLOSING Ce ─────────────────────────
    {
      subtype: "executable",
      desc: "generate executable file — lints clean",
      packageOrCrate: "testcrate",
      expectedBlocks: ["METADATA", "SETUP", "BODY", "CLOSING"],
      contentAbsent: ["{{key}}", "{{title}}"],
    },

    // ── Module: general-purpose mod ─────────────────────────────────
    {
      subtype: "module",
      desc: "generate module file — lints clean",
      packageOrCrate: "testcrate",
      expectedBlocks: ["METADATA", "SETUP", "BODY", "CLOSING"],
      contentAbsent: ["{{key}}"],
    },

    // ── Bare-bone: minimal skeleton ─────────────────────────────────
    {
      subtype: "bare-bone",
      desc: "generate bare-bone file — lints clean",
      packageOrCrate: "testcrate",
      expectedBlocks: ["METADATA", "SETUP", "BODY", "CLOSING"],
      contentAbsent: ["{{key}}", "{{title}}"],
    },

    // ── Demo-test: test file skeleton ───────────────────────────────
    {
      subtype: "demo-test",
      desc: "generate demo-test file — lints clean",
      packageOrCrate: "testcrate",
      expectedBlocks: ["METADATA", "SETUP", "BODY", "CLOSING"],
      contentAbsent: ["{{key}}", "{{title}}"],
    },
  ],
};

runCreateSuite(rustCreateSpec);

// ============================================================================
// CLOSING
// ============================================================================
//
// Table-driven Rust create tests — 5 subtypes generating up to 5 tests.
// Each generated file verified: non-empty, lints clean, blocks present,
// no unfilled placeholders. Schema = specification proven for Rust.
//
// "And God saw every thing that he had made, and, behold, it was very good."
//   — Genesis 1:31
// ============================================================================
