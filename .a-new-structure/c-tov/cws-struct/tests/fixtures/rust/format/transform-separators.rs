//omni:code --rust -library
//omni:key B-test-fixture-transform-separators
//omni:version a-01.00

// ============================================================================
// METADATA
// ============================================================================
//
// Key:      B-test-fixture-transform-separators
// Purpose:  Transformer test fixture with ASCII dash and bad prefix separators
// Biblical: Psalm 119:105 — Thy word is a lamp unto my feet
//
// PRAGMA:
//   I1.key       = B-test-fixture-transform-separators
//   I1.format    = @omni code --rust -library
//   I1.from      = b-word/seed/code/L0/rust/library.rs
//   I1.at        = a-01.00
//   I2.type      = code
//   I2.subtype   = library
//   I2.role      = test-fixture
//   I2.structure = 4-block
//   I3.crate     = transform_separators
//   I3.title     = Transform Separators Test Fixture
//   I3.component = Transformer fixture with intentional separator issues
//   I3.provides  = [TRANSFORM_SEPARATORS_FIXTURE]
//   I3.brief     = Test fixture for ASCII dash and prefix normalization transforms
//   I4.file      = transform-separators.rs
//   I4.path      = c-tov/cws-struct/tests/fixtures/rust/transform-separators.rs
//   C1.version   = a-01.00
//   C1.status    = Active
//   C1.created   = 2026-02-17
//   C1.updated   = 2026-02-17
//   C2.architect = Seanje Lenox-Wise
//   C2.impl      = Nova Dawn (CPI-SI)
//   C2.org       = CreativeWorkzStudio LLC
//   C2.copyright = 2025-2026 CreativeWorkzStudio LLC. All rights reserved.
//   C3.scripture = Psalm 119:105
//   C3.principle = Consistent structure reflects consistent thinking
//   C3.anchor    = Order in small things enables order in large things
//   C4.requires.stdlib = [core]
//   C4.consumers = [cws-struct transformer tests]
//   C4.if_missing = Transformer tests cannot verify ASCII→Unicode conversion
//   C5.layer     = L0
//   C5.paradigm  = tov
//   C5.domain    = test-fixture
//   C6.tags      = [transformer, test-fixture, separators]
//   C6.category  = test
//   C7.api       = none
//   C7.stability = experimental
//
// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// ──────────────────────────────────────────────────────────────────────────
// Imports
// ──────────────────────────────────────────────────────────────────────────

// (none needed for test fixture)

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// --------------------------------------------------------------------------
// 1. Section With ASCII Dashes
// --------------------------------------------------------------------------

/// This section uses ASCII dashes (should be converted to Unicode ─)
pub fn ascii_dashes_example() -> bool {
    true
}

//----------------------------------------------------------------------
// 2. Section With No Space After Slashes
//----------------------------------------------------------------------

/// This section has no space between // and the dashes
pub fn no_space_example() -> bool {
    true
}

//  ──────────────────────────────────────────────────────────────────────────
// 3. Section With Extra Space Before Unicode
//  ──────────────────────────────────────────────────────────────────────────

/// This section has 2 spaces after // (should be normalized to 1)
pub fn extra_space_example() -> bool {
    true
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// Cv — Validation
// Ce — Execution
// Cc — Cleanup
//
// X5: note = "Transformer test fixture with intentional separator issues."
// X5: scripture = "Psalm 119:105 — Thy word is a lamp unto my feet."
// X5: anchor = "Structure starts with the smallest details."
//
// ============================================================================
// END CLOSING
// ============================================================================
