// #!omni:code -typescript\--> module
// ╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
// ║ [BLOCK:METADATA]                                                                                                   ║
// ╠════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣
// ║                                                                                                                    ║
// ║ Key:         B-DSA-REGISTRY                                                                                        ║
// ║ Version:     a-02.10                                                                                               ║
// ║ Purpose:     Self-contained data registry (no external URL dependencies).                                          ║
// ║                                                                                                                    ║
// ╚════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝

// ╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
// ║ [BLOCK:SETUP]                                                                                                      ║
// ╠════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣

import { SEED_MATH, SEED_TOPOLOGY, CurrentState } from "../../data/state.ts";

export interface DsaRawData {
    metadata: { substrate: string; encoding: string };
    raw_samples: Record<string, string>;
}

export interface DsaMath {
    constants: {
        u: number;
        n: number;
        breath_ratio: number;
        identity_ratio: number;
        protection_ratio: number;
    };
    witness: {
        single: number;
        double: number;
        full: number;
    };
    lucas: {
        sequence: number[];
        origin_idx: number;
        n_idx: number;
        terminal_idx: number;
    };
}

export interface DsaTopology {
    topology: {
        types: string[];
        spine_x: number;
        exclusive_doors: boolean;
    };
    joints: {
        cross: string;
        t_up: string;
        t_down: string;
        l_left: string;
        l_right: string;
        spine_heavy: string;
        spine_light: string;
        horizon_heavy: string;
        horizon_light: string;
    };
    transitions: {
        pop_out_top: string;
        pop_out_bottom: string;
        sink_in_top: string;
        sink_in_bottom: string;
        breach_vertical: string;
        sunk_vertical: string;
    };
}

/**
 * Lightweight TOML-to-Object parser for simple KV structures.
 * (Self-contained to avoid URL imports)
 */
function simpleParse(text: string): any {
    const obj: any = {};
    let currentSection: string | null = null;
    
    text.split("\n").forEach(line => {
        const lineWithoutComment = line.split("#")[0].trim();
        if (!lineWithoutComment) return;
        
        if (lineWithoutComment.startsWith("[") && lineWithoutComment.endsWith("]")) {
            currentSection = lineWithoutComment.slice(1, -1);
            obj[currentSection] = {};
            return;
        }
        
        const [key, val] = lineWithoutComment.split("=").map(s => s.trim());
        if (key && val) {
            let parsedVal: any = val;
            if (val.startsWith("[") && val.endsWith("]")) {
                parsedVal = val.slice(1, -1).split(",").map(s => s.trim().replace(/\"/g, ""));
            } else if (!isNaN(Number(val))) {
                parsedVal = Number(val);
            } else if (val === "true" || val === "false") {
                parsedVal = val === "true";
            } else {
                parsedVal = val.replace(/\"/g, "");
            }
            
            if (currentSection) obj[currentSection][key] = parsedVal;
            else obj[key] = parsedVal;
        }
    });
    return obj;
}

// ╚════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝

// ╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
// ║ [BLOCK:BODY]                                                                                                       ║
// ╠════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣

export class DsaRegistry {
    private static math: DsaMath;
    private static topology: DsaTopology;
    private static raw: DsaRawData;

    public static async load() {
        try {
            // Use local relative paths without URL/path module dependencies
            const mathText = await Deno.readTextFile("sdk/algorithm/src/data/math.toml");
            const topologyText = await Deno.readTextFile("sdk/algorithm/src/data/topology.toml");
            const rawText = await Deno.readTextFile("sdk/algorithm/src/data/raw_terminal.toml");
            
            this.math = simpleParse(mathText) as unknown as DsaMath;
            this.topology = simpleParse(topologyText) as unknown as DsaTopology;
            this.raw = simpleParse(rawText) as unknown as DsaRawData;

            CurrentState.source = "toml";
            CurrentState.isHealthy = true;
        } catch (error) {
            this.applyFallbacks();
            CurrentState.source = "fallback";
            CurrentState.isHealthy = false;
            CurrentState.lastError = error.message;
        }
    }
// ... [rest of class remains same] ...

    private static applyFallbacks() {
        // Fallback for Raw Terminal Mapping
        this.raw = {
            metadata: { substrate: "fallback", encoding: "UTF-8" },
            raw_samples: { frame_start: "// ", frame_end: " //", spine_heavy: "║", spine_light: "│" }
        };

        // Map SEED_MATH to DsaMath interface
        this.math = {
            constants: {
                u: SEED_MATH.u,
                n: SEED_MATH.n,
                breath_ratio: SEED_MATH.ratios.breath,
                identity_ratio: SEED_MATH.ratios.identity,
                protection_ratio: SEED_MATH.ratios.protection
            },
            witness: SEED_MATH.witness,
            lucas: { sequence: [1, 3, 4, 7, 11, 18], origin_idx: 3, n_idx: 4, terminal_idx: 5 }
        };

        // Map SEED_TOPOLOGY to DsaTopology
        this.topology = {
            topology: { types: ["spine"], spine_x: SEED_TOPOLOGY.spine_x, exclusive_doors: true },
            joints: {
                cross: SEED_TOPOLOGY.joints.heavy.cross,
                t_up: SEED_TOPOLOGY.joints.heavy.t_up,
                t_down: SEED_TOPOLOGY.joints.heavy.t_down,
                l_left: SEED_TOPOLOGY.joints.heavy.bl,
                l_right: SEED_TOPOLOGY.joints.heavy.br,
                spine_heavy: SEED_TOPOLOGY.joints.heavy.v,
                spine_light: SEED_TOPOLOGY.joints.light.v,
                horizon_heavy: SEED_TOPOLOGY.joints.heavy.h,
                horizon_light: SEED_TOPOLOGY.joints.light.h
            },
            transitions: {
                pop_out_top: "└────────────────────────────────┐",
                pop_out_bottom: "┌────────────────────────────────┘",
                sink_in_top: "┌────────────────────────────────┘",
                sink_in_bottom: "└────────────────────────────────┐",
                breach_vertical: "│",
                sunk_vertical: "╏"
            }
        };
    }

    public static get Math() { return this.math; }
    public static get Topology() { return this.topology; }
    public static get Raw() { return this.raw; }
    
    // Quick-access calculated constants
    public static get U() { return this.math.constants.u; }
    public static get N() { return this.math.constants.n; }
    public static get Breath() { return this.U * this.math.constants.breath_ratio; }
    public static get Identity() { return this.U * this.math.constants.identity_ratio; }
    public static get Protection() { return this.U * this.math.constants.protection_ratio; }
}

// ╚════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝

// ╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
// ║ [BLOCK:CLOSING]                                                                                                    ║
// ╚════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝
