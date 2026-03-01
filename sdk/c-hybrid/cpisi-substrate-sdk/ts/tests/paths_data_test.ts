// #!omni:code -typescript\--> test
// ╠═==================================================================================================================═╣
//
// ╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
// ║ [BLOCK:ROOT] Path Data Verification — "Proving the Landmarks"                                                      ║
// ╠════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣
// ║                                                                                                                    ║
// ║ ╔═════════════════════════════════════════════════════════════════════════════════╗                                ║
// ║ ║ [FILE:IDENTITY] Application Genesis Block                                       ║                                ║
// ╠═╬═════════════════════════════════════════════════════════════════════════════════╬═╗                              ║
// ║ ║                                                                                 ║ ║                              ║
// ║ ║ ┌─────────────────────────────────────────────────────────────────────────────┐ ║ ║                              ║
// ║ ║ │ [OMNI:PRAGMA] CPI-SI Code System Declarations                               │ ║ ║                              ║
// ║ ║ ├─────────────────────────────────────────────────────────────────────────────┤ ║ ║                              ║
// ║ ║ │ ┌─────────────────────────────────────────────────────────────────────────┐ │ ║ ║                              ║
// ║ ║ │ │ //omni:key B-sdk-ts-paths-data-test                                     │ │ ║ ║                              ║
// ║ ║ │ │ //omni:code --typescript -test                                          │ │ ║ ║                              ║
// ║ ║ │ │ //omni:version a-01.00                                                  │ │ ║ ║                              ║
// ║ ║ │ └─────────────────────────────────────────────────────────────────────────┘ │ ║ ║                              ║
// ║ ║ └─────────────────────────────────────────────────────────────────────────────┘ ║ ║                              ║
// ║ ║                                                                                 ║ ║                              ║
// ║ ║ ┌─────────────────────────────────────────────────────────────────────────────┐ ║ ║                              ║
// ║ ║ │ [HUMAN:ANCHOR] Architectural Intent & Scriptural Grounding                  │ ║ ║                              ║
// ║ ║ ├─────────────────────────────────────────────────────────────────────────────┤ ║ ║                              ║
//!║ ║ │ ┌─────────────────────────────────────────────────────────────────────────┐ │ ║ ║                              ║
//!║ ║ │ │ # Path Data Tests                                                       │ │ ║ ║                              ║
//!║ ║ │ │                                                                         │ │ ║ ║                              ║
//!║ ║ │ │ "Prove all things; hold fast that which is good."                       │ │ ║ ║                              ║
//!║ ║ │ │ — 1 Thessalonians 5:21 (KJV)                                            │ │ ║ ║                              ║
//!║ ║ │ │                                                                         │ │ ║ ║                              ║
//!║ ║ │ │ This test suite verifies that the immutable Seed Covenant landmarks     │ │ ║ ║                              ║
//!║ ║ │ │ are correctly defined and that the tripwire state defaults are sound.    │ │ ║ ║                              ║
//!║ ║ │ └─────────────────────────────────────────────────────────────────────────┘ │ ║ ║                              ║
// ║ ║ └─────────────────────────────────────────────────────────────────────────────┘ ║ ║                              ║
// ║ ╚═════════════════════════════════════════════════════════════════════════════════╝ ║                              ║
// ║                                                                                     ║                              ║
// ╠═════════════════════════════════════════════════════════════════════════════════════╬══════════════════════════════╣
// ║                                                                                     ║                              ║
// ║ ╔═════════════════════════════════════════════════════════════════════════════════╗ ║                              ║
// ║ ║ [BLOCK:METADATA] Kingdom Technology Structural Covenant                         ║ ║                              ║
// ║ ╠═════════════════════════════════════════════════════════════════════════════════╣ ║                              ║
// ║ ║                                                                                 ║ ║                              ║
/** @type {Array<[string, string]>} Module pragma identity. */                      // ║ ║                              ║
export const PRAGMA = [                                                             // ║ ║                              ║
    ["I1.key", "B-sdk-ts-paths-data-test"],                                         // ║ ║                              ║
    ["I1.format", "typescript"],                                                    // ║ ║                              ║
    ["I1.from", "ts/tests/paths_data_test.ts"],                                     // ║ ║                              ║
    ["I1.at", "a-01.00"],                                                           // ║ ║                              ║
    ["I2.role", "test"],                                                            // ║ ║                              ║
    ["I3.title", "Path Data Verification"],                                         // ║ ║                              ║
];                                                                                  // ║ ║                              ║
// ║ ║                                                                                 ║ ║                              ║
/** @type {Array<[string, string]>} Module metadata and context. */                 // ║ ║                              ║
export const METADATA = [                                                           // ║ ║                              ║
    ["C1.version", "a-01.00"],                                                      // ║ ║                              ║
    ["C1.status", "Active"],                                                        // ║ ║                              ║
    ["C2.organization", "CreativeWorkzStudio LLC"],                                 // ║ ║                              ║
    ["C3.scripture", "1 Thessalonians 5:21"],                                       // ║ ║                              ║
    ["C3.anchor", "Genesis 1:1"],                                                   // ║ ║                              ║
];                                                                                  // ║ ║                              ║
// ║ ║                                                                                 ║ ║                              ║
/** @returns {Array<[string, string]>} The module's OmniCode pragma identity. */    // ║ ║                              ║
export function getPragma() { return PRAGMA; }                                      // ║ ║                              ║
/** @returns {Array<[string, string]>} The module's OmniCode metadata. */           // ║ ║                              ║
export function getMetadata() { return METADATA; }                                  // ║ ║                              ║
// ║ ║                                                                                 ║ ║                              ║
// ║ ╚═════════════════════════════════════════════════════════════════════════════════╝ ║                              ║
// ║                                                                                     ║                              ║
// ╠═════════════════════════════════════════════════════════════════════════════════════╬══════════════════════════════╣
// ║                                                                                     ║                              ║
// ╔═════════════════════════════════════════════════════════════════════════════════╗ ║                              ║
// ║ ║ [BLOCK:SETUP] Dependencies & Test Fixtures                                      ║ ║                              ║
// ╠═╬═════════════════════════════════════════════════════════════════════════════════╬═╝                              ║
// ║ ║                                                                                 ║ ║ [Outer Court: Guidance]      ║
import { assertEquals, assert } from "https://deno.land/std/assert/mod.ts";           // ║ ║ > Native Assertions          ║
import { SEED_PATHS, CurrentState } from "../lib/paths_data.ts";                    // ║ ║ > Target Data Layer          ║
// ║ ║                                                                                 ║ ║                              ║
// ║ ╚═════════════════════════════════════════════════════════════════════════════════╝ ║                              ║
// ║                                                                                     ║                              ║
// ╠═════════════════════════════════════════════════════════════════════════════════════╬══════════════════════════════╣
// ║                                                                                     ║                              ║
// ╔═════════════════════════════════════════════════════════════════════════════════╗ ║                              ║
// ║ ║ [BLOCK:BODY] Verification Logic                                                 ║ ║                              ║
// ║ ╠═════════════════════════════════════════════════════════════════════════════════╣ ║                              ║
// ║ ║                                                                                 ║ ║                              ║
/**                                                                                 // ║ ║ [Sub-Block: Logic Test]      ║
 * Verifies that the absolute root anchors for the workstation are correctly        // ║ ║                              ║
 * defined and traceable.                                                           // ║ ║                              ║
 */                                                                                 // ║ ║                              ║
Deno.test("[B-sdk-ts-paths-data] Verify Seed Covenant Roots", () => {               // ║ ║                              ║
    assert(SEED_PATHS.roots.bereshit.includes("Bereshit"));                         // ║ ║                              ║
    assert(SEED_PATHS.roots.workstation_home.startsWith("/home/"));                 // ║ ║                              ║
});                                                                                 // ║ ║                              ║
// ║ ║                                                                                 ║ ║                              ║
/**                                                                                 // ║ ║ [Sub-Block: Structural]     ║
 * Verifies that the internal repository mapping follows the Bereshit standard.     // ║ ║                              ║
 */                                                                                 // ║ ║                              ║
Deno.test("[B-sdk-ts-paths-data] Verify Repository Layers", () => {                 // ║ ║                              ║
    assertEquals(SEED_PATHS.repository.word, "word");                               // ║ ║                              ║
    assertEquals(SEED_PATHS.repository.core, "word/core");                          // ║ ║                              ║
});                                                                                 // ║ ║                              ║
// ║ ║                                                                                 ║ ║                              ║
/**                                                                                 // ║ ║ [Sub-Block: Diagnostic]      ║
 * Verifies that the initial tripwire state is healthy and pointing to fallbacks.   // ║ ║                              ║
 */                                                                                 // ║ ║                              ║
Deno.test("[B-sdk-ts-paths-data] Verify Initial Tripwire State", () => {            // ║ ║                              ║
    assertEquals(CurrentState.source, "fallback");                                  // ║ ║                              ║
    assertEquals(CurrentState.isHealthy, true);                                     // ║ ║                              ║
});                                                                                 // ║ ║                              ║
// ║ ║                                                                                 ║ ║                              ║
// ║ ╚═════════════════════════════════════════════════════════════════════════════════╝ ║                              ║
// ║                                                                                     ║                              ║
// ╠═════════════════════════════════════════════════════════════════════════════════════╬══════════════════════════════╣
// ║                                                                                     ║                              ║
// ╔═════════════════════════════════════════════════════════════════════════════════╗ ║                              ║
// ║ ║ [BLOCK:CLOSING] Finalization & Validation                                       ║ ║                              ║
// ║ ╠═════════════════════════════════════════════════════════════════════════════════╣ ║                              ║
// ║ ║                                                                                 ║ ║                              ║
// ║ ║ Note: External provers for the Spirit Layer.                                    ║ ║                              ║
// ║ ║ Scripture: "In the beginning God created the heaven and the earth." — Gen 1:1   ║ ║                              ║
// ║ ║                                                                                 ║ ║                              ║
// ║ ║                                                                                 ║ ║           [BLOCK:ROOT-->END] ║
// ║ ╚═════════════════════════════════════════════════════════════════════════════════╝ ║                              ║
// ║                                                                                     ║                              ║
// ╚═════════════════════════════════════════════════════════════════════════════════════╩══════════════════════════════╝
//
// ╠═==================================================================================================================═╣
