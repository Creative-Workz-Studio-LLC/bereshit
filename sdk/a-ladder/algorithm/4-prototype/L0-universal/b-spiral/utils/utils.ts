// #!omni:code -typescript\--> module
// ╠═==================================================================================================================═╣
//
// ╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
// ║ [BLOCK:ROOT] Diagnostic & IO Utilities                                                                             ║
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
// ║ ║ │ │ //omni:key B-dsa-utils                                                  │ │ ║ ║                              ║
// ║ ║ │ │ //omni:code --typescript -module                                        │ │ ║ ║                              ║
// ║ ║ │ │ //omni:version a-02.00                                                  │ │ ║ ║                              ║
// ║ ║ │ └─────────────────────────────────────────────────────────────────────────┘ │ ║ ║                              ║
// ║ ║ └─────────────────────────────────────────────────────────────────────────────┘ ║ ║                              ║
// ║ ║                                                                                 ║ ║                              ║
// ║ ║ ┌─────────────────────────────────────────────────────────────────────────────┐ ║ ║                              ║
// ║ ║ │ [HUMAN:ANCHOR] Architectural Intent & Scriptural Grounding                  │ ║ ║                              ║
// ║ ║ ├─────────────────────────────────────────────────────────────────────────────┤ ║ ║                              ║
//!║ ║ │ ┌─────────────────────────────────────────────────────────────────────────┐ │ ║ ║                              ║
//!║ ║ │ │ # The Witness Record                                                    │ │ ║ ║                              ║
//!║ ║ │ │                                                                         │ │ ║ ║                              ║
//!║ ║ │ │ "For there are three that bear record in heaven... and there are      │ │ ║ ║                              ║
//!║ ║ │ │ three that bear witness in earth..." — 1 John 5:7-8 (KJV)             │ │ ║ ║                              ║
//!║ ║ │ │                                                                         │ │ ║ ║                              ║
//!║ ║ │ │ This file manages the internal I/O streams and logging for the          │ │ ║ ║                              ║
//!║ ║ │ │ prototype engine. It ensures that every rendering execution leaves an   │ │ ║ ║                              ║
//!║ ║ │ │ immutable trail for debugging and structural verification.              │ │ ║ ║                              ║
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
// ║ ║ ┌─────────────────────────────────────────────────────────────────────────────┐ ║ ║                              ║
// ║ ║ │ [SETUP:CONSTANTS] Universal Resolution                                      │ ║ ║                              ║
// ║ ║ ├─────────────────────────────────────────────────────────────────────────────┤ ║ ║                              ║
/** The designated output path for immutable execution witnesses. */                  // ║ ║                              ║
const LOG_DIR = "a-ladder/algorithm/3-proof/logs/archive";                          // ║ ║                              ║
/** The absolute limit for the active log directory before rotation (50 MB). */      // ║ ║                              ║
const MAX_LOG_DIR_SIZE_BYTES = 50 * 1024 * 1024;                                    // ║ ║                              ║
/** Staging area for logs that exceed the weight limit. */                          // ║ ║                              ║
const FACTORY_PROCESSING_DIR = "a-ladder/algorithm/factory/processing";             // ║ ║                              ║
// ║ ║                                                                                 ║ ║                              ║
// ║ ║ ┌─────────────────────────────────────────────────────────────────────────────┐ ║ ║                              ║
// ║ ║ │ [SETUP:STATE] Mutable In-Memory Trackers                                  │ ║ ║                              ║
// ║ ║ ├─────────────────────────────────────────────────────────────────────────────┤ ║ ║                              ║
let logBuffer: string[] = [];                                                       // ║ ║                              ║
let logFilePath: string;                                                            // ║ ║                              ║
// ║ ║                                                                                 ║ ║                              ║
/** Preserving native console.log before interception. */                           // ║ ║                              ║
const originalConsoleLog = console.log;                                             // ║ ║                              ║
// ║ ║                                                                                 ║ ║                              ║
// ║ ╚═════════════════════════════════════════════════════════════════════════════════╝ ║                              ║
// ║                                                                                     ║                              ║
// ╠═════════════════════════════════════════════════════════════════════════════════════╬══════════════════════════════╣
// ║                                                                                     ║                              ║
// ║ ╔═════════════════════════════════════════════════════════════════════════════════╗ ║                              ║
// ║ ║ [BLOCK:BODY] Operational Logic & Turns                                          ║ ║                              ║
// ║ ╠═════════════════════════════════════════════════════════════════════════════════╣ ║                              ║
// ║ ║                                                                                 ║ ║                              ║
/**                                                                                 // ║ ║ [Sub-Block: Initialization]  ║
 * Initializes the log interceptor and generates a structured file path.            // ║ ║                              ║
 * @param {string} missionLabel - Identifier for the current execution run.          // ║ ║                              ║
 * @async                                                                           // ║ ║                              ║
 */                                                                                 // ║ ║                              ║
export async function initLogging(missionLabel: string = "unlabeled") {             // ║ ║                              ║
    await Deno.mkdir(LOG_DIR, { recursive: true });                                 // ║ ║                              ║
    await Deno.mkdir(FACTORY_PROCESSING_DIR, { recursive: true });                  // ║ ║                              ║
                                                                                    // ║ ║                              ║
    const now = new Date();                                                         // ║ ║                              ║
    const dateStr = now.toISOString().split("T")[0].replace(/-/g, "");              // ║ ║                              ║
    const timeStr = now.toISOString().split("T")[1].replace(/[:.]/g, "").slice(0, 6); // ║ ║                            ║
    const structuredTime = `${dateStr}_${timeStr}`;                                 // ║ ║                              ║
                                                                                    // ║ ║                              ║
    logFilePath = `${LOG_DIR}/dsa_run_${structuredTime}_${missionLabel}.log`;       // ║ ║                              ║
    logBuffer.push(`--- DSA REPL Session Log: ${structuredTime} ---\n`);            // ║ ║                              ║
    logBuffer.push(`--- Mission Label: ${missionLabel} ---\n`);                     // ║ ║                              ║
                                                                                    // ║ ║                              ║
    await rotateLogsByWeight();                                                     // ║ ║                              ║
                                                                                    // ║ ║                              ║
    console.log = (...args: any[]) => {                                             // ║ ║                              ║
        const message = args.map(arg => String(arg)).join(" ");                     // ║ ║                              ║
        logBuffer.push(message);                                                    // ║ ║                              ║
        originalConsoleLog(...args);                                                // ║ ║                              ║
    };                                                                              // ║ ║                              ║
                                                                                    // ║ ║                              ║
    console.log(`Logging active: ${logFilePath}`);                                  // ║ ║                              ║
}                                                                                   // ║ ║                              ║
// ║ ║                                                                                 ║ ║                              ║
/**                                                                                 // ║ ║ [Sub-Block: Rotation]        ║
 * Moves old logs to factory processing if the total weight exceeds 50MB.           // ║ ║                              ║
 * @private                                                                         // ║ ║                              ║
 * @async                                                                           // ║ ║                              ║
 */                                                                                 // ║ ║                              ║
async function rotateLogsByWeight() {                                               // ║ ║                              ║
    try {                                                                           // ║ ║                              ║
        const entries = [];                                                         // ║ ║                              ║
        let totalSizeBytes = 0;                                                     // ║ ║                              ║
                                                                                    // ║ ║                              ║
        for await (const dirEntry of Deno.readDir(LOG_DIR)) {                       // ║ ║                              ║
            if (dirEntry.isFile && dirEntry.name.startsWith("dsa_run_")) {          // ║ ║                              ║
                const stat = await Deno.stat(`${LOG_DIR}/${dirEntry.name}`);        // ║ ║                              ║
                totalSizeBytes += stat.size;                                        // ║ ║                              ║
                entries.push({ name: dirEntry.name, size: stat.size });             // ║ ║                              ║
            }                                                                       // ║ ║                              ║
        }                                                                           // ║ ║                              ║
                                                                                    // ║ ║                              ║
        if (totalSizeBytes > MAX_LOG_DIR_SIZE_BYTES) {                              // ║ ║                              ║
            entries.sort((a, b) => a.name.localeCompare(b.name));                   // ║ ║                              ║
            let bytesToRemove = totalSizeBytes - (MAX_LOG_DIR_SIZE_BYTES * 0.8);    // ║ ║                              ║
            let removedBytes = 0;                                                   // ║ ║                              ║
                                                                                    // ║ ║                              ║
            for (const file of entries) {                                           // ║ ║                              ║
                if (removedBytes >= bytesToRemove) break;                           // ║ ║                              ║
                await Deno.rename(`${LOG_DIR}/${file.name}`, `${FACTORY_PROCESSING_DIR}/${file.name}`); // ║ ║          ║
                removedBytes += file.size;                                          // ║ ║                              ║
            }                                                                       // ║ ║                              ║
        }                                                                           // ║ ║                              ║
    } catch (e) {                                                                   // ║ ║                              ║
        originalConsoleLog(`Weight rotation failed: ${e}`);                         // ║ ║                              ║
    }                                                                               // ║ ║                              ║
}                                                                                   // ║ ║                              ║
// ║ ║                                                                                 ║ ║                              ║
/**                                                                                 // ║ ║ [Sub-Block: Persistence]     ║
 * Flushes memory log buffer to the file system.                                    // ║ ║                              ║
 * @async                                                                           // ║ ║                              ║
 * @throws {Error} [CWS-T01-06] If disk write fails.                                // ║ ║                              ║
 */                                                                                 // ║ ║                              ║
export async function writeLogFile() {                                              // ║ ║                              ║
    try {                                                                           // ║ ║                              ║
        await Deno.writeTextFile(logFilePath, logBuffer.join("\n"));                // ║ ║                              ║
        originalConsoleLog(`Session log saved: ${logFilePath}`);                    // ║ ║                              ║
    } catch (error) {                                                               // ║ ║                              ║
        const msg = error instanceof Error ? error.message : String(error);         // ║ ║                              ║
        originalConsoleLog(`[CWS-T01-06] IO Failure: ${msg}`);                      // ║ ║                              ║
    }                                                                               // ║ ║                              ║
}                                                                                   // ║ ║                              ║
// ║ ║                                                                                 ║ ║                              ║
/**                                                                                 // ║ ║ [Sub-Block: Input]           ║
 * Prompts user for text input and records interaction in the witness.              // ║ ║                              ║
 * @param {string} question - Text to display to user.                              // ║ ║                              ║
 * @returns {Promise<string>} The user's response.                                  // ║ ║                              ║
 * @async                                                                           // ║ ║                              ║
 */                                                                                 // ║ ║                              ║
export async function prompt(question: string): Promise<string> {                   // ║ ║                              ║
    logBuffer.push(`PROMPT: ${question}`);                                          // ║ ║                              ║
    const buf = new Uint8Array(1024);                                               // ║ ║                              ║
    originalConsoleLog(question);                                                   // ║ ║                              ║
    const n = await Deno.stdin.read(buf);                                           // ║ ║                              ║
    if (n === null) return "";                                                      // ║ ║                              ║
    const answer = new TextDecoder().decode(buf.subarray(0, n)).trim();             // ║ ║                              ║
    logBuffer.push(`ANSWER: ${answer}`);                                            // ║ ║                              ║
    return answer;                                                                  // ║ ║                              ║
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
//!║ ║ │ │ Witnessing the integrity of the IO utilities.                           │ │ ║ ║                              ║
//!║ ║ │ └─────────────────────────────────────────────────────────────────────────┘ │ ║ ║                              ║
// ║ ║ └─────────────────────────────────────────────────────────────────────────────┘ ║ ║                              ║
//                                                                                     // ║ ║                              ║
/** @internal Test block for utilities. */                                          // ║ ║                              ║
export const __TESTS = {                                                            // ║ ║                              ║
    /** Verifies that console.log interception is active. */                        // ║ ║                              ║
    test_interception() {                                                           // ║ ║                              ║
        logBuffer = [];                                                             // ║ ║                              ║
        console.log("Test Intercept");                                              // ║ ║                              ║
        console.assert(logBuffer.includes("Test Intercept"), "IO Interception failed"); // ║ ║                          ║
    }                                                                               // ║ ║                              ║
};                                                                                  // ║ ║                              ║
//                                                                                     // ║ ║                              ║
// ║ ║                                                                                 ║ ║                              ║
// ║ ║ Note: System-level intercepts for development diagnostics.                     ║ ║                              ║
// ║ ║ Scripture: "In the beginning God created the heaven and the earth." — Gen 1:1   ║ ║                              ║
// ║ ║                                                                                 ║ ║                              ║
// ║ ║                                                                                 ║ ║           [BLOCK:ROOT-->END] ║
// ║ ╚═════════════════════════════════════════════════════════════════════════════════╝ ║                              ║
// ║                                                                                     ║                              ║
// ╚═════════════════════════════════════════════════════════════════════════════════════╩══════════════════════════════╝
//
// ╠═==================================================================================================================═╣
