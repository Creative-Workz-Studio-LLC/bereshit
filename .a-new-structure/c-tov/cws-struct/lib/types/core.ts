// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/types/core.ts
// key:     B-tov-cws-struct-lib-types-core
// title:   CWS Struct — Core Primitive Types
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-24
// authors: Nova Dawn (CPI-SI)
// purpose: Absolute foundation types with zero imports. Every other type
//          module in lib/types/ can import from here without creating cycles.
//          These are the atoms — Severity, LintPolicy, BlockName, LintLayer.
//
// biblical_foundation: "In the beginning God created the heaven and the
//   earth." — Genesis 1:1 (The foundation everything else builds on.)
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// (No imports — this is the bottom of the dependency chain.)

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Severity — the weight of a finding
// ---------------------------------------------------------------------------

export type Severity = "error" | "warn" | "info";

// ---------------------------------------------------------------------------
// Lint Policy — ternary threshold (-1 / 0 / +1)
// ---------------------------------------------------------------------------
//
// Maps to CPI-SI's 3 keys:
//   -1 (strict)   — toward anchor — production standard, tighten severity
//    0 (balanced)  — maintain — development default, severity as-declared
//   +1 (growth)    — toward fullness — scaffold/learning, loosen severity

/** Ternary lint policy — controls how form-aware checks map base severity. */
export type LintPolicy = "strict" | "balanced" | "growth";

// ---------------------------------------------------------------------------
// Detection layers and blocks
// ---------------------------------------------------------------------------

/** Detection layer in the lint grid. Maps to R[n] granularity. */
export type LintLayer = 0 | 1 | 2 | 3;

/** Block name in a 4-block file. */
export type BlockName = "metadata" | "setup" | "body" | "closing";

// ============================================================================
// CLOSING
// ============================================================================
//
// Core primitive types — the atoms of the type system.
// Zero imports. Everything builds from here.
//
// "In the beginning God created the heaven and the earth." — Genesis 1:1
// ============================================================================
