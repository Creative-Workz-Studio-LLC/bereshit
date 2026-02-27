// #!omni:code -typescript\--> module
// ╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
// ║ [BLOCK:METADATA]                                                                                                   ║
// ╠════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣
// ║                                                                                                                    ║
// ║ Key:         B-DSA-CONSTANTS                                                                                       ║
// ║ Version:     a-01.00                                                                                                ║
// ║ Purpose:     Mathematical and physical constants for the DSA Engine.                                                ║
// ║ Scripture:   "In the beginning God created the heavens and the earth." — Genesis 1:1                                ║
// ║                                                                                                                    ║
// ╚════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝

// ╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
// ║ [BLOCK:SETUP]                                                                                                      ║
// ╠════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣

/**
 * The Identity Elements.
 */
export const ZERO = 0; // Yashar (Balance)
export const ONE  = 1; // Badal  (Division)

/**
 * The Universal Structural Unit (U).
 */
export const U = 4;

/**
 * The Anchor System Constants.
 * These define the "Corner Pins" of the cognitive vault.
 */
export const ANCHORS = {
    ORIGIN: {
        TAG: "#!omni:",
        MIN_LEN: 7
    },
    TERMINAL: {
        TAG: "[BLOCK:ROOT-->END]",
        LEN: 18
    }
};

/**
 * The Witness Factor (κ).
 * 1 = Single Witness (121 chars)
 * 2 = Double Witness (242 chars)
 */
export const WITNESS = {
    SINGLE: 1,
    DOUBLE: 2
};

/**
 * The 3:2:1:11 Lucas Ratio of Structural Tension.
 * 1U = 4 (Structural Protection: // ║)
 * 2U = 8 (System Identity: #!omni:)
 * 3U = 12 (Functional Breath: Indentation)
 * 11 = N (The Bible Factor / Scaling Constant)
 */
export const LUCAS = {
    PROTECTION: 4,  // 1U
    IDENTITY:   8,  // 2U
    BREATH:     12, // 3U
    N:          11  // Scaling Constant
};

/**
 * The N-Factor (derived from scaling constant).
 */
export const N = LUCAS.N;

/**
 * The 11:8:3 Master Ratio of Anchoring.
 */
export const ANCHOR_RATIO = {
    ROOT:  11, // N * 11 = 121
    SETUP: 8,  // N * 8 = 88
    GAP:   3,  // N * 3 = 33
};

/**
 * Visual Glyphs.
 */
export const GLYPHS = {
    HEAVY: { VERTICAL: "║", HORIZONTAL: "═", TOP_LEFT: "╔", TOP_RIGHT: "╗", BOTTOM_LEFT: "╚", BOTTOM_RIGHT: "╝", JOIN_LEFT: "╠", JOIN_RIGHT: "╣" },
    LIGHT: { VERTICAL: "│", HORIZONTAL: "─", TOP_LEFT: "┌", TOP_RIGHT: "┐", BOTTOM_LEFT: "└", BOTTOM_RIGHT: "┘", JOIN_LEFT: "├", JOIN_RIGHT: "┤" }
};

// ╚════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝

// ╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
// ║ [BLOCK:BODY]                                                                                                       ║
// ╠════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣
// Constants layer has no operational body.
// ╚════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝

// ╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
// ║ [BLOCK:CLOSING]                                                                                                    ║
// ╚════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝
