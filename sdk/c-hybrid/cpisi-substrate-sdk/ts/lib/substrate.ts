// #!omni:code -typescript\--> module
// ╠═==================================================================================================================═╣
//
// ╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
// ║ [BLOCK:ROOT] Substrate SDK FFI Bridge — "Connecting the Mind and Spirit"                                           ║
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
// ║ ║ │ │ //omni:key B-sdk-ts-substrate-ffi                                       │ │ ║ ║                              ║
// ║ ║ │ │ //omni:code --typescript -library                                       │ │ ║ ║                              ║
// ║ ║ │ │ //omni:version a-01.00                                                  │ │ ║ ║                              ║
// ║ ║ │ └─────────────────────────────────────────────────────────────────────────┘ │ ║ ║                              ║
// ║ ║ └─────────────────────────────────────────────────────────────────────────────┘ ║ ║                              ║
// ║ ║                                                                                 ║ ║                              ║
// ║ ║ ┌─────────────────────────────────────────────────────────────────────────────┐ ║ ║                              ║
// ║ ║ │ [HUMAN:ANCHOR] Architectural Intent & Scriptural Grounding                  │ ║ ║                              ║
// ║ ║ ├─────────────────────────────────────────────────────────────────────────────┤ ║ ║                              ║
//!║ ║ │ ┌─────────────────────────────────────────────────────────────────────────┐ │ ║ ║                              ║
//!║ ║ │ │ # Substrate SDK FFI Wrapper                                             │ │ ║ ║                              ║
//!║ ║ │ │                                                                         │ │ ║ ║                              ║
//!║ ║ │ │ "For as the body without the spirit is dead, so faith without works     │ │ ║ ║                              ║
//!║ ║ │ │ is dead also." — James 2:26 (KJV)                                       │ │ ║ ║                              ║
//!║ ║ │ │                                                                         │ │ ║ ║                              ║
//!║ ║ │ │ This module bridges the high-level Mind (TypeScript) to the             │ │ ║ ║                              ║
//!║ ║ │ │ foundational Spirit (Rust) using Deno's FFI capabilities.                │ │ ║ ║                              ║
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
    ["I1.key", "B-sdk-ts-substrate-ffi"],                                           // ║ ║                              ║
    ["I1.format", "typescript"],                                                    // ║ ║                              ║
    ["I1.from", "ts/lib/substrate.ts"],                                             // ║ ║                              ║
    ["I1.at", "a-01.00"],                                                           // ║ ║                              ║
    ["I2.role", "mind-bridge"],                                                     // ║ ║                              ║
    ["I3.title", "Substrate SDK FFI Wrapper"],                                      // ║ ║                              ║
];                                                                                  // ║ ║                              ║
// ║ ║                                                                                 ║ ║                              ║
/** @type {Array<[string, string]>} Module metadata and context. */                 // ║ ║                              ║
export const METADATA = [                                                           // ║ ║                              ║
    ["C1.version", "a-01.00"],                                                      // ║ ║                              ║
    ["C1.status", "Active"],                                                        // ║ ║                              ║
    ["C2.organization", "CreativeWorkzStudio LLC"],                                 // ║ ║                              ║
    ["C3.scripture", "James 2:26"],                                                 // ║ ║                              ║
    ["C3.anchor", "Genesis 1:1"],                                                   // ║ ║                              ║
];                                                                                  // ║ ║                              ║
// ║ ║                                                                                 ║ ║                              ║
// ║ ╚═════════════════════════════════════════════════════════════════════════════════╝ ║                              ║
// ║                                                                                     ║                              ║
// ╠═════════════════════════════════════════════════════════════════════════════════════╬══════════════════════════════╣
// ║                                                                                     ║                              ║
// ╔═════════════════════════════════════════════════════════════════════════════════╗ ║                              ║
// ║ ║ [BLOCK:SETUP] Dependencies & Operational Context                                ║ ║                              ║
// ╠═╬═════════════════════════════════════════════════════════════════════════════════╬═╝                              ║
// ║ ║                                                                                 ║ ║ [Outer Court: Guidance]      ║
import { join } from "https://deno.land/std/path/mod.ts";                           // ║ ║ > Native Path utilities      ║
import { PathResolver } from "./paths.ts";                                          // ║ ║ > Hierarchical Pathing       ║
// ║ ║                                                                                 ║ ║                              ║
/**                                                                                 // ║ ║ [Sub-Layer: Definitions]   ║
 * Machine-readable mission record produced at the end of a session.                // ║ ║                              ║
 */                                                                                 // ║ ║                              ║
export interface SessionTestimony {                                                 // ║ ║                              ║
    session_id: string;                                                             // ║ ║                              ║
    substrate: string;                                                              // ║ ║                              ║
    engine: string;                                                                 // ║ ║                              ║
    started_at: string;                                                             // ║ ║                              ║
    ended_at: string;                                                               // ║ ║                              ║
    duration_seconds: number;                                                       // ║ ║                              ║
    metrics: {                                                                      // ║ ║                              ║
        exchanges: number;                                                          // ║ ║                              ║
        insights: number;                                                           // ║ ║                              ║
        tools_used: number;                                                         // ║ ║                              ║
        peak_context_tokens: number;                                                // ║ ║                              ║
        compactions: number;                                                        // ║ ║                              ║
    };                                                                              // ║ ║                              ║
    state: {                                                                        // ║ ║                              ║
        trajectory: string;                                                         // ║ ║                              ║
        health: number;                                                             // ║ ║                              ║
        hebrew_state: string;                                                       // ║ ║                              ║
        alignment: number;                                                          // ║ ║                              ║
        anchor: string;                                                             // ║ ║                              ║
    };                                                                              // ║ ║                              ║
    summary: string;                                                                // ║ ║                              ║
}                                                                                   // ║ ║                              ║
// ║ ║                                                                                 ║ ║                              ║
/** Internal FFI handle. Lazy-initialized via getLib(). */                          // ║ ║                              ║
let lib: Deno.DynamicLibrary<any> | null = null;                                    // ║ ║                              ║
// ║ ║                                                                                 ║ ║                              ║
// ║ ╚═════════════════════════════════════════════════════════════════════════════════╝ ║                              ║
// ║                                                                                     ║                              ║
// ╠═════════════════════════════════════════════════════════════════════════════════════╬══════════════════════════════╣
// ║                                                                                     ║                              ║
// ╔═════════════════════════════════════════════════════════════════════════════════╗ ║                              ║
// ║ ║ [BLOCK:BODY] Operational Logic & Turns                                          ║ ║                              ║
// ║ ╠═════════════════════════════════════════════════════════════════════════════════╣ ║                              ║
// ║ ║                                                                                 ║ ║                              ║
/**                                                                                 // ║ ║ [Sub-Block: FFI Core]       ║
 * Initializes the FFI library using resolved system paths.                         // ║ ║                              ║
 *                                                                                  // ║ ║                              ║
 * @returns {Promise<Deno.DynamicLibrary<any>>} The open library handle.             // ║ ║                              ║
 * @async                                                                           // ║ ║                              ║
 * @private                                                                         // ║ ║                              ║
 */                                                                                 // ║ ║                              ║
async function getLib(): Promise<Deno.DynamicLibrary<any>> {                        // ║ ║                              ║
    if (lib) return lib;                                                            // ║ ║                              ║
                                                                                    // ║ ║                              ║
    const paths = await PathResolver.resolve();                                     // ║ ║                              ║
    const libPath = join(paths.bereshitRoot, "sdk/cpisi-substrate-sdk/lib/libcpisi_substrate_sdk.so"); // ║ ║           ║
                                                                                    // ║ ║                              ║
    lib = Deno.dlopen(libPath, {                                                    // ║ ║                              ║
        cpisi_sdk_load_map: { parameters: ["buffer"], result: "i32" },              // ║ ║                              ║
        cpisi_sdk_process_event: { parameters: ["buffer", "buffer", "buffer"], result: "pointer" }, // ║ ║              ║
        cpisi_sdk_render_output: { parameters: ["buffer", "buffer", "buffer", "buffer"], result: "pointer" }, // ║ ║    ║
        cpisi_sdk_get_last_testimony: { parameters: [], result: "pointer" },        // ║ ║                              ║
        cpisi_sdk_generate_sabbath_artifacts: { parameters: ["buffer"], result: "i32" }, // ║ ║                         ║
        cpisi_sdk_free_string: { parameters: ["pointer"], result: "void" },          // ║ ║                              ║
    });                                                                             // ║ ║                              ║
                                                                                    // ║ ║                              ║
    return lib;                                                                     // ║ ║                              ║
}                                                                                   // ║ ║                              ║
// ║ ║                                                                                 ║ ║                              ║
/** Converts a TS string to a null-terminated C string buffer. */                   // ║ ║                              ║
function toCString(s: string): Uint8Array {                                         // ║ ║                              ║
    return new TextEncoder().encode(s + "\0");                                      // ║ ║                              ║
}                                                                                   // ║ ║                              ║
// ║ ║                                                                                 ║ ║                              ║
/** Converts a C string pointer back to TS and frees the C allocation. */           // ║ ║                              ║
async function fromCString(ptr: Deno.PointerValue): Promise<string> {               // ║ ║                              ║
    if (!ptr) return "";                                                            // ║ ║                              ║
    const s = new Deno.UnsafePointerView(ptr).getCString();                         // ║ ║                              ║
    const l = await getLib();                                                       // ║ ║                              ║
    (l.symbols.cpisi_sdk_free_string as any)(ptr);                                  // ║ ║                              ║
    return s;                                                                       // ║ ║                              ║
}                                                                                   // ║ ║                              ║
// ║ ║                                                                                 ║ ║                              ║
// ╠═════════════════════════════════════════════════════════════════════════════════╣ ║                              ║
// ║ ║                                                                                 ║ ║ [Sub-Block: Public APIs]    ║
// ║ ║                                                                                 ║ ║                              ║
/**                                                                                 // ║ ║                              ║
 * Loads a substrate map into the Spirit engine.                                    // ║ ║                              ║
 *                                                                                  // ║ ║ @param {string} path - Path to the TOML map.                                // ║ ║                              ║
 * @returns {Promise<number>} 0 on success, negative on error.                      // ║ ║                              ║
 */                                                                                 // ║ ║                              ║
export async function loadMap(path: string): Promise<number> {                      // ║ ║                              ║
    const l = await getLib();                                                       // ║ ║                              ║
    return (l.symbols.cpisi_sdk_load_map as any)(toCString(path));                  // ║ ║                              ║
}                                                                                   // ║ ║                              ║
// ║ ║                                                                                 ║ ║                              ║
/**                                                                                 // ║ ║                              ║
 * Processes a raw JSON event through the mapping engine.                           // ║ ║                              ║
 *                                                                                  // ║ ║ @param {string} substrate - Name of the dialect.                             // ║ ║                              ║
 * @param {string} event - The universal event key.                                 // ║ ║                              ║
 * @param {string} rawJson - The native CLI payload.                                // ║ ║                              ║
 * @returns {Promise<string>} The mapped Universal Event JSON.                      // ║ ║                              ║
 */                                                                                 // ║ ║                              ║
export async function processEvent(substrate: string, event: string, rawJson: string): Promise<string> { // ║ ║         ║
    const l = await getLib();                                                       // ║ ║                              ║
    const resPtr = (l.symbols.cpisi_sdk_process_event as any)(                      // ║ ║                              ║
        toCString(substrate),                                                       // ║ ║                              ║
        toCString(event),                                                           // ║ ║                              ║
        toCString(rawJson),                                                         // ║ ║                              ║
    );                                                                              // ║ ║                              ║
    return await fromCString(resPtr);                                               // ║ ║                              ║
}                                                                                   // ║ ║                              ║
// ║ ║                                                                                 ║ ║                              ║
/**                                                                                 // ║ ║                              ║
 * Retrieves the most recent mission record.                                        // ║ ║                              ║
 *                                                                                  // ║ ║ @returns {Promise<SessionTestimony | null>} The last testimony or null.      // ║ ║                              ║
 */                                                                                 // ║ ║                              ║
export async function getLastTestimony(): Promise<SessionTestimony | null> {        // ║ ║                              ║
    const l = await getLib();                                                       // ║ ║                              ║
    const resPtr = (l.symbols.cpisi_sdk_get_last_testimony as any)();               // ║ ║                              ║
    const json = await fromCString(resPtr);                                         // ║ ║                              ║
    if (!json) return null;                                                         // ║ ║                              ║
    return JSON.parse(json);                                                        // ║ ║                              ║
}                                                                                   // ║ ║                              ║
// ║ ║                                                                                 ║ ║                              ║
/**                                                                                 // ║ ║                              ║
 * Triggers the Sabbath artifact generation.                                        // ║ ║                              ║
 *                                                                                  // ║ ║ @param {SessionTestimony} testimony - The full mission record.               // ║ ║                              ║
 * @returns {Promise<number>} 0 on success.                                         // ║ ║                              ║
 */                                                                                 // ║ ║                              ║
export async function generateSabbathArtifacts(testimony: SessionTestimony): Promise<number> { // ║ ║                   ║
    const l = await getLib();                                                       // ║ ║                              ║
    return (l.symbols.cpisi_sdk_generate_sabbath_artifacts as any)(toCString(JSON.stringify(testimony))); // ║ ║        ║
}                                                                                   // ║ ║                              ║
// ║ ║                                                                                 ║ ║                              ║
// ║ ╚═════════════════════════════════════════════════════════════════════════════════╝ ║                              ║
// ║                                                                                     ║                              ║
// ╠═════════════════════════════════════════════════════════════════════════════════════╬══════════════════════════════╣
// ║                                                                                     ║                              ║
// ╔═════════════════════════════════════════════════════════════════════════════════╗ ║                              ║
// ║ ║ [BLOCK:CLOSING] Finalization & Validation                                       ║ ║                              ║
// ║ ╠═════════════════════════════════════════════════════════════════════════════════╣ ║                              ║
// ║ ║                                                                                 ║ ║                              ║
// ║ ║ Note: FFI bridge between Mind (TS) and Spirit (Rust).                           ║ ║                              ║
// ║ ║ Scripture: "The spirit indeed is willing, but the flesh is weak." — Mat 26:41   ║ ║                              ║
// ║ ║                                                                                 ║ ║                              ║
// ║ ║                                                                                 ║ ║           [BLOCK:ROOT-->END] ║
// ║ ╚═════════════════════════════════════════════════════════════════════════════════╝ ║                              ║
// ║                                                                                     ║                              ║
// ╚═════════════════════════════════════════════════════════════════════════════════════╩══════════════════════════════╝
//
// ╠═==================================================================================================================═╣
