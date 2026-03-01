// #!omni:code -typescript\--> module
// ╠═==================================================================================================================═╣
//
// ╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
// ║ [BLOCK:ROOT] Path Resolver Mind Engine — "Establishing the Way"                                                    ║
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
// ║ ║ │ │ //omni:key B-sdk-ts-paths                                               │ │ ║ ║                              ║
// ║ ║ │ │ //omni:code --typescript -library                                       │ │ ║ ║                              ║
// ║ ║ │ │ //omni:version a-01.00                                                  │ │ ║ ║                              ║
// ║ ║ │ └─────────────────────────────────────────────────────────────────────────┘ │ ║ ║                              ║
// ║ ║ └─────────────────────────────────────────────────────────────────────────────┘ ║ ║                              ║
// ║ ║                                                                                 ║ ║                              ║
// ║ ║ ┌─────────────────────────────────────────────────────────────────────────────┐ ║ ║                              ║
// ║ ║ │ [HUMAN:ANCHOR] Architectural Intent & Scriptural Grounding                  │ ║ ║                              ║
// ║ ║ ├─────────────────────────────────────────────────────────────────────────────┤ ║ ║                              ║
//!║ ║ │ ┌─────────────────────────────────────────────────────────────────────────┐ │ ║ ║                              ║
//!║ ║ │ │ # Path Resolver Engine                                                  │ │ ║ ║                              ║
//!║ ║ │ │                                                                         │ │ ║ ║                              ║
//!║ ║ │ │ "Ponder the path of thy feet, and let all thy ways be established."     │ │ ║ ║                              ║
//!║ ║ │ │ — Proverbs 4:26 (KJV)                                                   │ │ ║ ║                              ║
//!║ ║ │ │                                                                         │ │ ║ ║                              ║
//!║ ║ │ │ This engine orchestrates the hierarchical resolution of paths. It      │ │ ║ ║                              ║
//!║ ║ │ │ merges universal blueprints with platform-specific dialects and local    │ │ ║ ║                              ║
//!║ ║ │ │ user anchors to establish the session's physical proprioception.        │ │ ║ ║                              ║
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
    ["I1.key", "B-sdk-ts-paths"],                                                   // ║ ║                              ║
    ["I1.format", "typescript"],                                                    // ║ ║                              ║
    ["I1.at", "a-01.00"],                                                           // ║ ║                              ║
    ["I2.role", "mind"],                                                            // ║ ║                              ║
    ["I3.title", "Path Resolver Engine"],                                           // ║ ║                              ║
];                                                                                  // ║ ║                              ║
// ║ ║                                                                                 ║ ║                              ║
/** @type {Array<[string, string]>} Module metadata and context. */                 // ║ ║                              ║
export const METADATA = [                                                           // ║ ║                              ║
    ["C1.version", "a-01.00"],                                                      // ║ ║                              ║
    ["C1.status", "Active"],                                                        // ║ ║                              ║
    ["C2.organization", "CreativeWorkzStudio LLC"],                                 // ║ ║                              ║
    ["C3.scripture", "Proverbs 4:26"],                                             // ║ ║                              ║
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
// ║ ╔═════════════════════════════════════════════════════════════════════════════════╗ ║                              ║
// ║ ║ [BLOCK:SETUP] Dependencies & Operational Context                                ║ ║                              ║
// ╠═╬═════════════════════════════════════════════════════════════════════════════════╬═╝                              ║
// ║ ║                                                                                 ║ ║ [Outer Court: Guidance]      ║
import { join, resolve } from "https://deno.land/std/path/mod.ts";                   // ║ ║ > Native Deno Path utils     ║
import { parse } from "https://deno.land/std/toml/mod.ts";                            // ║ ║ > TOML parsing logic         ║
import { SEED_PATHS, CurrentState } from "./paths_data.ts";                         // ║ ║ > Tripwire Spirit fallbacks  ║
// ║ ║                                                                                 ║ ║                              ║
/**                                                                                 // ║ ║                              ║
 * ResolvedPaths represents the absolute physical map of the session.               // ║ ║                              ║
 */                                                                                 // ║ ║                              ║
export interface ResolvedPaths {                                                    // ║ ║                              ║
    /** The absolute path to the Bereshit repository root. */                       // ║ ║                              ║
    bereshitRoot: string;                                                           // ║ ║                              ║
    /** The absolute path to the local workstation home directory. */               // ║ ║                              ║
    userHome: string;                                                               // ║ ║                              ║
    /** The absolute path to the 'word' definition layer. */                        // ║ ║                              ║
    word: string;                                                                   // ║ ║                              ║
    /** The absolute path to the 'word/core' specification layer. */                // ║ ║                              ║
    core: string;                                                                   // ║ ║                              ║
    /** The absolute path to the active SQLite data directory (ext4). */            // ║ ║                              ║
    dbPath: string;                                                                 // ║ ║                              ║
    /** The absolute path to the substrate's hidden home (e.g. ~/.gemini). */        // ║ ║                              ║
    substrateHome: string;                                                          // ║ ║                              ║
}                                                                                   // ║ ║                              ║
// ║ ║                                                                                 ║ ║                              ║
/** Internal cache for resolved paths to ensure zero-latency turned cycles. */      // ║ ║                              ║
let pathCache: ResolvedPaths | null = null;                                         // ║ ║                              ║
// ║ ║                                                                                 ║ ║                              ║
// ║ ╚═════════════════════════════════════════════════════════════════════════════════╝ ║                              ║
// ║                                                                                     ║                              ║
// ╠═════════════════════════════════════════════════════════════════════════════════════╬══════════════════════════════╣
// ║                                                                                     ║                              ║
// ║ ╔═════════════════════════════════════════════════════════════════════════════════╗ ║                              ║
// ║ ║ [BLOCK:BODY] Operational Logic & Turns                                          ║ ║                              ║
// ║ ╠═════════════════════════════════════════════════════════════════════════════════╣ ║                              ║
// ║ ║                                                                                 ║ ║                              ║
/**                                                                                 // ║ ║ [Sub-Block: Main Engine]     ║
 * PathResolver orchestrates the hierarchical loading of paths.                     // ║ ║                              ║
 * It follows the descent from Universal Blueprint to User Guidance.                // ║ ║                              ║
 */                                                                                 // ║ ║                              ║
export class PathResolver {                                                         // ║ ║                              ║
    /**                                                                             // ║ ║                              ║
     * Resolves the full system path map.                                           // ║ ║                              ║
     * It merges the hardcoded Spirit defaults with the dynamic User anchors.       // ║ ║                              ║
     *                                                                              // ║ ║                              ║
     * @returns {Promise<ResolvedPaths>} A promise resolving to the physical map.    // ║ ║                              ║
     * @async                                                                       // ║ ║                              ║
     * @throws {Error} If filesystem access is completely restricted.               // ║ ║                              ║
     */                                                                             // ║ ║                              ║
    public static async resolve(): Promise<ResolvedPaths> {                         // ║ ║                              ║
        if (pathCache) return pathCache;                                            // ║ ║                              ║
                                                                                    // ║ ║                              ║
        try {                                                                       // ║ ║                              ║
            const repoRoot = await this.findRepoRoot();                             // ║ ║                              ║
            const userConfigPath = join(repoRoot, "sdk/cpisi-substrate-sdk/data/paths/user/current.toml"); // ║ ║       ║
                                                                                    // ║ ║                              ║
            const userText = await Deno.readTextFile(userConfigPath);               // ║ ║                              ║
            const userMap = parse(userText) as any;                                 // ║ ║                              ║
                                                                                    // ║ ║                              ║
            const bereshit = userMap.anchors?.bereshit_root || SEED_PATHS.roots.bereshit; // ║ ║                        ║
            const home = userMap.anchors?.user_home || SEED_PATHS.roots.workstation_home; // ║ ║                        ║
            const dbBase = userMap.overrides?.db_base_path || join(home, SEED_PATHS.database.base_path); // ║ ║         ║
                                                                                    // ║ ║                              ║
            const resolved: ResolvedPaths = {                                       // ║ ║                              ║
                bereshitRoot: bereshit,                                             // ║ ║                              ║
                userHome: home,                                                     // ║ ║                              ║
                word: join(bereshit, SEED_PATHS.repository.word),                   // ║ ║                              ║
                core: join(bereshit, SEED_PATHS.repository.core),                   // ║ ║                              ║
                dbPath: dbBase,                                                     // ║ ║                              ║
                substrateHome: join(home, SEED_PATHS.substrates.cpisi),             // ║ ║                              ║
            };                                                                      // ║ ║                              ║
                                                                                    // ║ ║                              ║
            pathCache = resolved;                                                   // ║ ║                              ║
            CurrentState.source = "toml";                                           // ║ ║                              ║
            CurrentState.isHealthy = true;                                          // ║ ║                              ║
            return resolved;                                                        // ║ ║                              ║
                                                                                    // ║ ║                              ║
        } catch (error) {                                                           // ║ ║                              ║
            CurrentState.source = "fallback";                                       // ║ ║                              ║
            CurrentState.isHealthy = false;                                         // ║ ║                              ║
            if (error instanceof Error) {                                           // ║ ║                              ║
                CurrentState.lastError = error.message;                             // ║ ║                              ║
            } else {                                                                // ║ ║                              ║
                CurrentState.lastError = String(error);                             // ║ ║                              ║
            }                                                                       // ║ ║                              ║
            return this.applyFallbacks();                                           // ║ ║                              ║
        }                                                                           // ║ ║                              ║
    }                                                                               // ║ ║                              ║
                                                                                    // ║ ║                              ║
    /**                                                                             // ║ ║ [Sub-Block: Safety]          ║
     * Applies the "Seed Covenant" hardcoded defaults as a tripwire fallback.        // ║ ║                              ║
     * This function is the "Inner Witness" ensuring the system never drifts.       // ║ ║                              ║
     *                                                                              // ║ ║                              ║
     * @returns {ResolvedPaths} The default structural map from the Spirit layer.   // ║ ║                              ║
     * @private                                                                     // ║ ║                              ║
     */                                                                             // ║ ║                              ║
    private static applyFallbacks(): ResolvedPaths {                                // ║ ║                              ║
        const bereshit = SEED_PATHS.roots.bereshit;                                 // ║ ║                              ║
        const home = SEED_PATHS.roots.workstation_home;                             // ║ ║                              ║
                                                                                    // ║ ║                              ║
        return {                                                                    // ║ ║                              ║
            bereshitRoot: bereshit,                                                 // ║ ║                              ║
            userHome: home,                                                         // ║ ║                              ║
            word: join(bereshit, SEED_PATHS.repository.word),                       // ║ ║                              ║
            core: join(bereshit, SEED_PATHS.repository.core),                       // ║ ║                              ║
            dbPath: join(home, SEED_PATHS.database.base_path),                      // ║ ║                              ║
            substrateHome: join(home, SEED_PATHS.substrates.cpisi),                 // ║ ║                              ║
        };                                                                          // ║ ║                              ║
    }                                                                               // ║ ║                              ║
                                                                                    // ║ ║                              ║
    /**                                                                             // ║ ║ [Sub-Block: Discovery]       ║
     * Recursively finds the repository root by searching for the .git marker.      // ║ ║                              ║
     * Max depth is constrained to 10 layers to prevent infinite recursion.         // ║ ║                              ║
     *                                                                              // ║ ║                              ║
     * @returns {Promise<string>} The absolute path to the repo root.               // ║ ║                              ║
     * @async                                                                       // ║ ║                              ║
     * @private                                                                     // ║ ║                              ║
     */                                                                             // ║ ║                              ║
    private static async findRepoRoot(): Promise<string> {                          // ║ ║                              ║
        let curr = Deno.cwd();                                                      // ║ ║                              ║
        for (let i = 0; i < 10; i++) {                                              // ║ ║                              ║
            try {                                                                   // ║ ║                              ║
                const gitDir = join(curr, ".git");                                  // ║ ║                              ║
                const stat = await Deno.stat(gitDir);                               // ║ ║                              ║
                if (stat.isDirectory) return curr;                                  // ║ ║                              ║
            } catch (_e) {                                                          // ║ ║                              ║
                curr = resolve(curr, "..");                                         // ║ ║                              ║
            }                                                                       // ║ ║                              ║
        }                                                                           // ║ ║                              ║
        return SEED_PATHS.roots.bereshit;                                           // ║ ║                              ║
    }                                                                               // ║ ║                              ║
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
//!║ ║ │ │ Every turn must be witnessed. Every block must be proven.               │ │ ║ ║                              ║
//!║ ║ │ └─────────────────────────────────────────────────────────────────────────┘ │ ║ ║                              ║
// ║ ║ └─────────────────────────────────────────────────────────────────────────────┘ ║ ║                              ║
//                                                                                     // ║ ║                              ║
/** @internal Test block for isolated verification. */                             // ║ ║                              ║
export const __TESTS = {                                                            // ║ ║                              ║
    async test_resolve_logic() {                                                    // ║ ║                              ║
        const paths = await PathResolver.resolve();                                 // ║ ║                              ║
        console.assert(paths.bereshitRoot.length > 0, "Root missing");              // ║ ║                              ║
        console.assert(paths.word.includes("word"), "Word layer misaligned");       // ║ ║                              ║
    }                                                                               // ║ ║                              ║
};                                                                                  // ║ ║                              ║
//                                                                                     // ║ ║                              ║
// ║ ║                                                                                 ║ ║                              ║
// ║ ║ Note: Hierarchical path resolution with caching and tripwiring                  ║ ║                              ║
// ║ ║ Scripture: "In the beginning God created the heaven and the earth." — Gen 1:1   ║ ║                              ║
// ║ ║                                                                                 ║ ║                              ║
// ║ ║                                                                                 ║ ║           [BLOCK:ROOT-->END] ║
// ║ ╚═════════════════════════════════════════════════════════════════════════════════╝ ║                              ║
// ║                                                                                     ║                              ║
// ╚═════════════════════════════════════════════════════════════════════════════════════╩══════════════════════════════╝
//
// ╠═==================================================================================================================═╣
