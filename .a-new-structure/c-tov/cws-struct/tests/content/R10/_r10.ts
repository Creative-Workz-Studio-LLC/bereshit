// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/content/R10/_r10.ts
// key:     B-tov-cws-struct-tests-content-R10-runner
// title:   R10 Test Runner — Re-exports from Shared Runner
// type:    Code (Test Infrastructure)
// version: a-02.00
// created: 2026-02-23
// updated: 2026-02-24
// authors: Nova Dawn (CPI-SI)
// purpose: Backward-compatible re-export of shared test runner for R10 tests.
//          All logic now lives in tests/_runner.ts — this file is a thin shim.
//
// ============================================================================

// Re-export everything from the shared runner
export {
  type TestCase as R10TestCase,
  type FormatSpec as R10FormatSpec,
  type TestCase,
  type FormatSpec,
  runSuite as runR10Suite,
  runSuite,
  lintOnce,
  clearFixtureCache,
  noErrors,
  noWarnings,
  noPlacementWarnings,
  hasIdentityRegisterInfo,
  noContentPragmaWarnings,
  noDocWarnings,
  noClosingWarnings,
  noBodyOrderWarnings,
  noValueWarnings,
  noDepthWarnings,
} from "@test/_runner.ts";
