// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/data/placement.ts
// key:     B-tov-cws-struct-lib-data-placement
// title:   CWS Struct — Placement Registry
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-22
// updated: 2026-02-22
// authors: Nova Dawn (CPI-SI)
// purpose: Hand-compiled registry of per-language placement rules.
//          Maps language constructs → target container/block.
//
//          Source: schemas/code/R50_codefile/languages/go/go.jsonc, schemas/code/R50_codefile/languages/rust/rust.jsonc
//          (content_kinds + metadata_forbidden sections)
//
//          SETUP makes things EXIST (declarations).
//          BODY makes things HAPPEN (implementations).
//          METADATA is declaration-free (comments + identity only).
//
// biblical_foundation: "A place for everything, and everything in its place."
//   — not Scripture, but the principle is: "Let all things be done decently
//   and in order." — 1 Corinthians 14:40
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import type { PlacementRule, BlockName } from "./types.ts";

// ---------------------------------------------------------------------------
// Placement rule builder
// ---------------------------------------------------------------------------

function rule(
  construct: string,
  targetContainer: string,
  targetBlock: BlockName,
  reason: string,
): PlacementRule {
  return { construct, targetContainer, targetBlock, reason };
}

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Go placement rules (7 content kinds)
// ---------------------------------------------------------------------------
//
// Source: schemas/code/R50_codefile/languages/go/go.jsonc → content_kinds
// Pattern: SETUP = declarations, BODY = implementations
//
// Go has 5 SETUP subsections: Imports, Constants, Variables, CoreTypes, TypeMethods
// Go has 2 BODY kinds: func_decl (standalone), method_decl (type-attached in SETUP)
//
// Note: method_decl targets SETUP/type-methods (structural completers),
// not BODY. Go's String(), Error() etc. complete type identity in SETUP.

const GO_PLACEMENT: PlacementRule[] = [
  rule(
    "import_decl", "imports", "setup",
    "Import declarations establish dependencies — SETUP makes things exist.",
  ),
  rule(
    "const_decl", "constants", "setup",
    "Constants are compile-time fixed values — SETUP establishes invariants.",
  ),
  rule(
    "var_decl", "variables", "setup",
    "Package-level variables are runtime state — SETUP declares scope.",
  ),
  rule(
    "type_decl", "core-types", "setup",
    "Type declarations define data structures — SETUP creates vocabulary.",
  ),
  rule(
    "func_decl", "free-functions", "body",
    "Standalone functions implement behavior — BODY makes things happen.",
  ),
  rule(
    "method_decl", "type-methods", "setup",
    "Type methods complete structural identity (String, Error) — SETUP structural completers.",
  ),
  rule(
    "init_func", "core-logic", "body",
    "init() functions execute at startup — BODY entry-point behavior.",
  ),
];

// ---------------------------------------------------------------------------
// Rust placement rules (12 content kinds)
// ---------------------------------------------------------------------------
//
// Source: schemas/code/R50_codefile/languages/rust/rust.jsonc → content_kinds
// Pattern: SETUP = 10 declaration kinds, BODY = 2 implementation kinds
//
// Rust has 8 SETUP subsections: Imports, Modules, Constants, Statics,
// TypeAliases, CoreTypes, TraitDefs, Macros
// Rust has 2 BODY kinds: fn_decl (standalone), impl_block (trait/inherent)

const RUST_PLACEMENT: PlacementRule[] = [
  rule(
    "use_decl", "imports", "setup",
    "Use declarations import items into scope — SETUP dependency declaration.",
  ),
  rule(
    "reexport_decl", "modules", "setup",
    "Re-export declarations expose items to parent — SETUP module hierarchy.",
  ),
  rule(
    "mod_decl", "modules", "setup",
    "Module declarations define hierarchy — SETUP structural organization.",
  ),
  rule(
    "const_decl", "constants", "setup",
    "Constants are compile-time values — SETUP establishes invariants.",
  ),
  rule(
    "static_decl", "statics", "setup",
    "Statics are runtime-initialized fixed values — SETUP runtime constants.",
  ),
  rule(
    "type_alias", "type-aliases", "setup",
    "Type aliases create vocabulary — SETUP shorthand for complex signatures.",
  ),
  rule(
    "struct_decl", "core-types", "setup",
    "Struct declarations define data structures — SETUP creates vocabulary.",
  ),
  rule(
    "enum_decl", "core-types", "setup",
    "Enum declarations define value sets — SETUP creates vocabulary.",
  ),
  rule(
    "trait_decl", "trait-defs", "setup",
    "Trait declarations define behavioral contracts — SETUP establishes shape.",
  ),
  rule(
    "macro_decl", "macros", "setup",
    "Macro declarations define code generation — SETUP meta-code.",
  ),
  rule(
    "fn_decl", "free-functions", "body",
    "Standalone functions implement behavior — BODY makes things happen.",
  ),
  rule(
    "impl_block", "trait-implementations", "body",
    "Impl blocks fulfill contracts — BODY contract fulfillment.",
  ),
];

// ---------------------------------------------------------------------------
// Metadata-forbidden kinds — content that must NEVER appear in METADATA
// ---------------------------------------------------------------------------

/** Go content kinds forbidden in METADATA block. */
export const GO_METADATA_FORBIDDEN: readonly string[] = [
  "import_decl", "const_decl", "var_decl", "type_decl",
  "func_decl", "method_decl", "init_func",
];

/** Rust content kinds forbidden in METADATA block. */
export const RUST_METADATA_FORBIDDEN: readonly string[] = [
  "use_decl", "reexport_decl", "mod_decl", "const_decl", "static_decl",
  "type_alias", "struct_decl", "enum_decl", "trait_decl", "macro_decl",
  "fn_decl", "impl_block",
];

// ---------------------------------------------------------------------------
// Exports
// ---------------------------------------------------------------------------

/**
 * Per-language placement rules, keyed by language name.
 *
 * Each entry is an ordered array of PlacementRules mapping
 * language constructs to their target container and block.
 */
export const PLACEMENT_REGISTRY: Record<string, PlacementRule[]> = {
  go: GO_PLACEMENT,
  rust: RUST_PLACEMENT,
};

/**
 * Per-language metadata-forbidden kinds, keyed by language name.
 *
 * Content kinds that must NEVER appear in the METADATA block.
 * METADATA is comments + identity only.
 */
export const METADATA_FORBIDDEN: Record<string, readonly string[]> = {
  go: GO_METADATA_FORBIDDEN,
  rust: RUST_METADATA_FORBIDDEN,
};

// ============================================================================
// CLOSING
// ============================================================================
//
// Placement registry — where language constructs belong.
// SETUP makes things EXIST. BODY makes things HAPPEN.
// METADATA is sacred ground — identity only, no code.
//
// "Let all things be done decently and in order."
// — 1 Corinthians 14:40
// ============================================================================
