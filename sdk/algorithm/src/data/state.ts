// #!omni:code -typescript\--> module
// ╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
// ║ [BLOCK:METADATA]                                                                                                   ║
// ╠════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣
// ║                                                                                                                    ║
// ║ Key:         B-DSA-STATE-TRIPWIRE                                                                                  ║
// ║ Version:     a-01.00                                                                                                ║
// ║ Purpose:     Fail-safe middle layer for mathematical and structural state.                                         ║
// ║                                                                                                                    ║
// ╚════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝

/**
 * THE SEED COVENANT: Immutable fallbacks for when external data is missing or corrupted.
 */
export const SEED_MATH = {
    u: 4,
    n: 11,
    ratios: { breath: 3, identity: 2, protection: 1 },
    witness: { single: 121, double: 242, full: 484 }
};

export const SEED_TOPOLOGY = {
    spine_x: 121,
    joints: {
        heavy: { v: "║", h: "═", tl: "╔", tr: "╗", bl: "╚", br: "╝", cross: "╬", t_up: "╩", t_down: "╦" },
        light: { v: "│", h: "─", tl: "┌", tr: "┐", bl: "└", br: "┘", cross: "┼", t_up: "┴", t_down: "┬" }
    }
};

/**
 * TRIPWIRE STATE: Tracks the health of the data-loading process.
 */
export interface TripwireState {
    source: "toml" | "fallback";
    isHealthy: boolean;
    lastError?: string;
}

export let CurrentState: TripwireState = {
    source: "fallback",
    isHealthy: true
};
