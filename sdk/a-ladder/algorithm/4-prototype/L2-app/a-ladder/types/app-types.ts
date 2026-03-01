// #!omni:code -typescript\--> module
// ╠═==================================================================================================================═╣
//
// ╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
// ║ [BLOCK:ROOT] Application-Layer Shared Types                                                                        ║
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
// ║ ║ │ │ //omni:key B-dsa-app-types                                              │ │ ║ ║                              ║
// ║ ║ │ │ //omni:code --typescript -module                                        │ │ ║ ║                              ║
// ║ ║ │ │ //omni:version a-01.00                                                  │ │ ║ ║                              ║
// ║ ║ │ └─────────────────────────────────────────────────────────────────────────┘ │ ║ ║                              ║
// ║ ║ └─────────────────────────────────────────────────────────────────────────────┘ ║ ║                              ║
// ║ ║                                                                                 ║ ║                              ║
// ║ ║ ┌─────────────────────────────────────────────────────────────────────────────┐ ║ ║                              ║
// ║ ║ │ [HUMAN:ANCHOR] Architectural Intent & Scriptural Grounding                  │ ║ ║                              ║
// ║ ║ ├─────────────────────────────────────────────────────────────────────────────┤ ║ ║                              ║
//!║ ║ │ ┌─────────────────────────────────────────────────────────────────────────┐ │ ║ ║                              ║
//!║ ║ │ │ # The Shared Language of the Body                                       │ │ ║ ║                              ║
//!║ ║ │ │                                                                         │ │ ║ ║                              ║
//!║ ║ │ │ "Two are better than one; because they have a good reward for their   │ │ ║ ║                              ║
//!║ ║ │ │ labour." — Ecclesiastes 4:9 (KJV)                                       │ │ ║ ║                              ║
//!║ ║ │ │                                                                         │ │ ║ ║                              ║
//!║ ║ │ │ This file defines the shared data structures for the L2 layer. It      │ │ ║ ║                              ║
//!║ ║ │ │ ensures that the Go Body and TypeScript Mind share a single schema.     │ │ ║ ║                              ║
//!║ ║ │ └─────────────────────────────────────────────────────────────────────────┘ │ ║ ║                              ║
// ║ ║ └─────────────────────────────────────────────────────────────────────────────┘ ║ ║                              ║
// ║ ╚═════════════════════════════════════════════════════════════════════════════════╝ ║                              ║
// ║                                                                                     ║                              ║
// ╠═════════════════════════════════════════════════════════════════════════════════════╬══════════════════════════════╣
// ║                                                                                     ║                              ║
// ║ ╔═════════════════════════════════════════════════════════════════════════════════╗ ║                              ║
// ║ ║ [BLOCK:SETUP] Initial Foundations & Context Window                              ║ ║                              ║
// ║ ╠═════════════════════════════════════════════════════════════════════════════════╣ ║                              ║
// ║ ║                                                                                 ║ ║                              ║
/**                                                                                 // ║ ║ [Sub-Block: Application]    ║
 * Represents the primary visual viewport and active state.                          // ║ ║                              ║
 */                                                                                 // ║ ║                              ║
export interface CpiSiViewport {                                                    // ║ ║                              ║
    /** The identity of the active CPI-SI instance (e.g. Nova Dawn). */             // ║ ║                              ║
    instanceName: string;                                                           // ║ ║                              ║
    /** The current state within the 27-position cube. */                           // ║ ║                              ║
    currentState: number;                                                           // ║ ║                              ║
    /** True if the teardown sequence has been initiated. */                        // ║ ║                              ║
    isShuttingDown: boolean;                                                        // ║ ║                              ║
}                                                                                   // ║ ║                              ║
// ║ ║                                                                                 ║ ║                              ║
/**                                                                                 // ║ ║ [Sub-Block: Orchestration]  ║
 * Represents a command request received from the user or substrate.                // ║ ║                              ║
 */                                                                                 // ║ ║                              ║
export interface CommandRequest {                                                   // ║ ║                              ║
    /** The target handler key (e.g. 'render'). */                                  // ║ ║                              ║
    commandName: string;                                                            // ║ ║                              ║
    /** Raw array of string arguments passed to the CLI. */                         // ║ ║                              ║
    arguments: string[];                                                            // ║ ║                              ║
    /** The substrate that generated the request. */                                 // ║ ║                              ║
    caller: "gemini" | "claude" | "native";                                         // ║ ║                              ║
}                                                                                   // ║ ║                              ║
// ║ ║                                                                                 ║ ║                              ║
/**                                                                                 // ║ ║ [Sub-Block: Testimony]      ║
 * Represents an immutable record of a successful execution turn.                   // ║ ║                              ║
 */                                                                                 // ║ ║                              ║
export interface ExecutionWitness {                                                 // ║ ║                              ║
    /** ISO8601 timestamp of the event. */                                          // ║ ║                              ║
    timestamp: string;                                                              // ║ ║                              ║
    /** Absolute physical path to the session log. */                               // ║ ║                              ║
    logPath: string;                                                                // ║ ║                              ║
    /** The calculated structural health score (0.0 to 1.0). */                     // ║ ║                              ║
    healthScore: number;                                                            // ║ ║                              ║
}                                                                                   // ║ ║                              ║
// ║ ║                                                                                 ║ ║                              ║
// ║ ╚═════════════════════════════════════════════════════════════════════════════════╝ ║                              ║
// ║                                                                                     ║                              ║
// ╠═════════════════════════════════════════════════════════════════════════════════════╬══════════════════════════════╣
// ║                                                                                     ║                              ║
// ║ ╔═════════════════════════════════════════════════════════════════════════════════╗ ║                              ║
// ║ ║ [BLOCK:CLOSING] Finalization & Validation                                       ║ ║                              ║
// ║ ╠═════════════════════════════════════════════════════════════════════════════════╣ ║                              ║
// ║ ║                                                                                 ║ ║                              ║
// ║ ║ ┌─────────────────────────────────────────────────────────────────────────────┐ ║ ║                              ║
// ║ ║ │ [CLOSING:TESTS] Automated Geometric & Functional Validation                 │ ║ ║                              ║
// ║ ║ ├─────────────────────────────────────────────────────────────────────────────┤ ║ ║                              ║
//!║ ║ │ ┌─────────────────────────────────────────────────────────────────────────┐ │ ║ ║                              ║
//!║ ║ │ │ # Immutable Inner Tests                                                 │ │ ║ ║                              ║
//!║ ║ │ │ Proving the 2D Application layer remains whole.                         │ │ ║ ║                              ║
//!║ ║ │ └─────────────────────────────────────────────────────────────────────────┘ │ ║ ║                              ║
// ║ ║ └─────────────────────────────────────────────────────────────────────────────┘ ║ ║                              ║
//                                                                                     // ║ ║                              ║
/** @internal Test block for app types. */                                          // ║ ║                              ║
export const __TESTS = {                                                            // ║ ║                              ║
    /** Basic structural verification. */                                           // ║ ║                              ║
    test_type_contract() {                                                          // ║ ║                              ║
        const witness: ExecutionWitness = {                                         // ║ ║                              ║
            timestamp: new Date().toISOString(),                                    // ║ ║                              ║
            logPath: "/test/path",                                                  // ║ ║                              ║
            healthScore: 1.0                                                        // ║ ║                              ║
        };                                                                          // ║ ║                              ║
        console.assert(witness.healthScore === 1.0, "Contract Drift");              // ║ ║                              ║
    }                                                                               // ║ ║                              ║
};                                                                                  // ║ ║                              ║
//                                                                                     // ║ ║                              ║
// ║ ║                                                                                 ║ ║                              ║
// ║ ║ Note: The TypeScript representation of the shared L2-app schemas.               ║ ║                              ║
// ║ ║ Scripture: "In the beginning God created the heaven and the earth." — Gen 1:1   ║ ║                              ║
// ║ ║                                                                                 ║ ║                              ║
// ║ ║                                                                                 ║ ║           [BLOCK:ROOT-->END] ║
// ║ ╚═════════════════════════════════════════════════════════════════════════════════╝ ║                              ║
// ║                                                                                     ║                              ║
// ╚═════════════════════════════════════════════════════════════════════════════════════╩══════════════════════════════╝
//
// ╠═==================================================================================================================═╣
