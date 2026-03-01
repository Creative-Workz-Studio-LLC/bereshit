// #!omni:code -typescript\--> module
// ╠═==================================================================================================================═╣
//
// ╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
// ║ [BLOCK:ROOT] Core Geometric Renderer                                                                               ║
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
// ║ ║ │ │ //omni:key B-dsa-drawer                                                 │ │ ║ ║                              ║
// ║ ║ │ │ //omni:code --typescript -module                                        │ │ ║ ║                              ║
// ║ ║ │ │ //omni:version a-02.50                                                  │ │ ║ ║                              ║
// ║ ║ │ └─────────────────────────────────────────────────────────────────────────┘ │ ║ ║                              ║
// ║ ║ └─────────────────────────────────────────────────────────────────────────────┘ ║ ║                              ║
// ║ ║                                                                                 ║ ║                              ║
// ║ ║ ┌─────────────────────────────────────────────────────────────────────────────┐ ║ ║                              ║
// ║ ║ │ [HUMAN:ANCHOR] Architectural Intent & Scriptural Grounding                  │ ║ ║                              ║
// ║ ║ ├─────────────────────────────────────────────────────────────────────────────┤ ║ ║                              ║
//!║ ║ │ ┌─────────────────────────────────────────────────────────────────────────┐ │ ║ ║                              ║
//!║ ║ │ │ # The Physics Renderer                                                  │ │ ║ ║                              ║
//!║ ║ │ │                                                                         │ │ ║ ║                              ║
//!║ ║ │ │ "And the building of the wall of it was of jasper: and the city was   │ │ ║ ║                              ║
//!║ ║ │ │ pure gold, like unto clear glass." — Revelation 21:18 (KJV)             │ │ ║ ║                              ║
//!║ ║ │ │                                                                         │ │ ║ ║                              ║
//!║ ║ │ │ This is the hand that draws the vault. It takes the calculated        │ │ ║ ║                              ║
//!║ ║ │ │ coordinates from the engine and applies the ASCII glyphs defined in L0. │ │ ║ ║                              ║
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
// ║ ║ │ [SETUP:IMPORTS] Dimensional Dependencies                                    │ ║ ║                              ║
// ║ ║ ├─────────────────────────────────────────────────────────────────────────────┤ ║ ║                              ║
import { SEED_TOPOLOGY } from "../../../L0-universal/b-spiral/state/state.ts";      // ║ ║                              ║
import { HingeVector, EngineVaultLayout } from "../engine/engine.ts";               // ║ ║                              ║
// ║ ║                                                                                 ║ ║                              ║
// ║ ╚═════════════════════════════════════════════════════════════════════════════════╝ ║                              ║
// ║                                                                                     ║                              ║
// ╠═════════════════════════════════════════════════════════════════════════════════════╬══════════════════════════════╣
// ║                                                                                     ║                              ║
// ║ ╔═════════════════════════════════════════════════════════════════════════════════╗ ║                              ║
// ║ ║ [BLOCK:BODY] Operational Logic & Turns                                          ║ ║                              ║
// ║ ╠═════════════════════════════════════════════════════════════════════════════════╣ ║                              ║
// ║ ║                                                                                 ║ ║                              ║
/**                                                                                 // ║ ║ [Sub-Block: Drawer Logic]    ║
 * Refined, state-aware drawer implementing Hinge Physics and Unitary Containers.   // ║ ║                              ║
 */                                                                                 // ║ ║                              ║
export class VaultDrawer {                                                          // ║ ║                              ║
    private layout: EngineVaultLayout;                                              // ║ ║                              ║
    /** Flip rendering direction for mirror-witness modes. */                       // ║ ║                              ║
    public flipX: boolean = false;                                                  // ║ ║                              ║
    /** Tracking horizontal positions of vertical walls at each depth. */           // ║ ║                              ║
    private wallPositions: Map<number, number> = new Map();                         // ║ ║                              ║
                                                                                    // ║ ║                              ║
    /**                                                                             // ║ ║                              ║
     * Constructs a drawer anchored to a specific spatial layout.                   // ║ ║                              ║
     * @param {EngineVaultLayout} layout - Calculated spatial map from the engine.   // ║ ║                              ║
     */                                                                             // ║ ║                              ║
    constructor(layout: EngineVaultLayout) {                                        // ║ ║                              ║
        this.layout = layout;                                                       // ║ ║                              ║
        this.initializeWallMap();                                                   // ║ ║                              ║
    }                                                                               // ║ ║                              ║
                                                                                    // ║ ║                              ║
    /** Seeds the wall tracking map with foundational Level 0 and 1 boundaries. */  // ║ ║                              ║
    private initializeWallMap() {                                                   // ║ ║                              ║
        // Root (0) is absolute                                                     // ║ ║                              ║
        this.wallPositions.set(0, this.layout.rootWidth - 1);                       // ║ ║                              ║
        // Level 1 starts at standard position                                      // ║ ║                              ║
        this.wallPositions.set(1, (this.layout.rootWidth - 1) - this.layout.gapWidth); // ║ ║                           ║
    }                                                                               // ║ ║                              ║
                                                                                    // ║ ║                              ║
    /**                                                                             // ║ ║                              ║
     * The Frame Covenant: Every line starts with // and maintains Root Pillars.    // ║ ║                              ║
     * @returns {string[]} Base character buffer for a line.                        // ║ ║                              ║
     * @private                                                                     // ║ ║                              ║
     */                                                                             // ║ ║                              ║
    private createBaseLine(): string[] {                                            // ║ ║                              ║
        const line = new Array(this.layout.rootWidth).fill(" ");                    // ║ ║                              ║
        line[0] = "/"; line[1] = "/";                                               // ║ ║                              ║
                                                                                    // ║ ║                              ║
        // Root Pillars (Level 0)                                                   // ║ ║                              ║
        line[this.getLeftWallCol(0)] = SEED_TOPOLOGY.joints.heavy.v;                // ║ ║                              ║
        line[this.getRightWallCol(0)] = SEED_TOPOLOGY.joints.heavy.v;               // ║ ║                              ║
                                                                                    // ║ ║                              ║
        return line;                                                                // ║ ║                              ║
    }                                                                               // ║ ║                              ║
                                                                                    // ║ ║                              ║
    /** Resolves the left vertical column index for a given depth. */               // ║ ║                              ║
    private getLeftWallCol(depth: number): number {                                 // ║ ║                              ║
        const col = 3 + (depth * 2);                                                // ║ ║                              ║
        return this.flipX ? this.layout.rootWidth - 1 - col : col;                  // ║ ║                              ║
    }                                                                               // ║ ║                              ║
                                                                                    // ║ ║                              ║
    /** Resolves the right vertical column index for a given depth. */              // ║ ║                              ║
    private getRightWallCol(depth: number): number {                                // ║ ║                              ║
        const standard = this.wallPositions.get(depth) || ((this.layout.rootWidth - 1) - this.layout.gapWidth); // ║    ║
        return this.flipX ? this.layout.rootWidth - 1 - standard : standard;        // ║ ║                              ║
    }                                                                               // ║ ║                              ║
                                                                                    // ║ ║                              ║
    /**                                                                             // ║ ║                              ║
     * Renders a boundary line for a block, respecting stateful wall positions.     // ║ ║                              ║
     * @param {string} content - Text to display within the boundary.               // ║ ║                              ║
     * @param {number} depth - Nesting level.                                       // ║ ║                              ║
     * @param {string} [tag] - Optional block tag (e.g. METADATA).                  // ║ ║                              ║
     * @param {"normal" | "sink" | "popout"} style - Dimensional tension style.      // ║ ║                              ║
     * @returns {string} Rendered UTF-8 string.                                     // ║ ║                              ║
     */                                                                             // ║ ║                              ║
    public drawBoundaryLine(content: string, depth: number, tag?: string, style: "normal" | "sink" | "popout" = "normal"): string { // ║
        const line = this.createBaseLine();                                         // ║ ║                              ║
        const glyph = style === "sink" ? "╏" : SEED_TOPOLOGY.joints.heavy.v;        // ║ ║                              ║
                                                                                    // ║ ║                              ║
        const lCol = this.getLeftWallCol(depth);                                    // ║ ║                              ║
        const rCol = this.getRightWallCol(depth);                                   // ║ ║                              ║
                                                                                    // ║ ║                              ║
        line[lCol] = glyph;                                                         // ║ ║                              ║
        line[rCol] = glyph;                                                         // ║ ║                              ║
                                                                                    // ║ ║                              ║
        const label = tag ? `[${tag}] ` : "";                                       // ║ ║                              ║
        const text = `${label}${content}`;                                          // ║ ║                              ║
        const start = lCol + 2;                                                     // ║ ║                              ║
                                                                                    // ║ ║                              ║
        for (let i = 0; i < text.length && start + i < rCol; i++) {                 // ║ ║                              ║
            line[start + i] = text[i];                                              // ║ ║                              ║
        }                                                                           // ║ ║                              ║
                                                                                    // ║ ║                              ║
        return line.join("");                                                       // ║ ║                              ║
    }                                                                               // ║ ║                              ║
                                                                                    // ║ ║                              ║
    /**                                                                             // ║ ║                              ║
     * Renders a Vault Line where code is present (Opening the left wall).          // ║ ║                              ║
     * @param {string} code - The logic string to insert.                           // ║ ║                              ║
     * @param {number} depth - Nesting level.                                       // ║ ║                              ║
     * @returns {string} Rendered UTF-8 string with open door.                      // ║ ║                              ║
     */                                                                             // ║ ║                              ║
    public drawVaultLine(code: string, depth: number): string {                     // ║ ║                              ║
        const line = this.createBaseLine();                                         // ║ ║                              ║
        const rCol = this.getRightWallCol(depth);                                   // ║ ║                              ║
        const glyph = this.layout.depthState === "sink" ? "╏" : SEED_TOPOLOGY.joints.heavy.v; // ║ ║                      ║
                                                                                    // ║ ║                              ║
        line[rCol] = glyph;                                                         // ║ ║                              ║
                                                                                    // ║ ║                              ║
        const parentCol = this.wallPositions.get(depth - 1);                        // ║ ║                              ║
        if (parentCol !== undefined && rCol <= parentCol) {                         // ║ ║                              ║
            line[parentCol] = SEED_TOPOLOGY.joints.heavy.v;                         // ║ ║                              ║
        }                                                                           // ║ ║                              ║
                                                                                    // ║ ║                              ║
        const codeStart = this.layout.codeLeftPadding;                              // ║ ║                              ║
        for (let i = 0; i < code.length && codeStart + i < rCol; i++) {             // ║ ║                              ║
            line[codeStart + i] = code[i];                                          // ║ ║                              ║
        }                                                                           // ║ ║                              ║
                                                                                    // ║ ║                              ║
        return line.join("");                                                       // ║ ║                              ║
    }                                                                               // ║ ║                              ║
                                                                                    // ║ ║                              ║
    /**                                                                             // ║ ║                              ║
     * The Legal Hallway (Zero-Tension Variant). Drops both boundaries.             // ║ ║                              ║
     * @param {string} code - Raw string to pass through.                           // ║ ║                              ║
     * @returns {string} The unaltered code.                                         // ║ ║                              ║
     */                                                                             // ║ ║                              ║
    public drawHallwayLine(code: string): string {                                  // ║ ║                              ║
        return code;                                                                // ║ ║                              ║
    }                                                                               // ║ ║                              ║
                                                                                    // ║ ║                              ║
    /**                                                                             // ║ ║                              ║
     * Renders a physical transition hinge (Pop/Sink).                              // ║ ║                              ║
     * @param {HingeVector} vector - Coordinate start and end for the transition.   // ║ ║                              ║
     * @param {boolean} isTop - True if this is the start of a box.                 // ║ ║                              ║
     * @param {number} depth - Nesting level.                                       // ║ ║                              ║
     * @returns {string} Rendered horizontal bridge line.                           // ║ ║                              ║
     */                                                                             // ║ ║                              ║
    public drawTransitionLine(vector: HingeVector, isTop: boolean, depth: number): string { // ║ ║                      ║
        const line = this.createBaseLine();                                         // ║ ║                              ║
        const bridge = "─";                                                         // ║ ║                              ║
                                                                                    // ║ ║                              ║
        let startGlyph: string, endGlyph: string;                                   // ║ ║                              ║
                                                                                    // ║ ║                              ║
        if (vector.targetX > vector.originX) {                                      // ║ ║                              ║
            startGlyph = isTop ? "└" : "┌";                                         // ║ ║                              ║
            endGlyph = isTop ? "┐" : "┘";                                           // ║ ║                              ║
        } else {                                                                    // ║ ║                              ║
            startGlyph = isTop ? "┐" : "┘";                                         // ║ ║                              ║
            endGlyph = isTop ? "└" : "┌";                                           // ║ ║                              ║
        }                                                                           // ║ ║                              ║
                                                                                    // ║ ║                              ║
        const start = Math.min(vector.originX, vector.targetX);                     // ║ ║                              ║
        const end = Math.max(vector.originX, vector.targetX);                       // ║ ║                              ║
                                                                                    // ║ ║                              ║
        for (let i = start + 1; i < end; i++) line[i] = bridge;                     // ║ ║                              ║
                                                                                    // ║ ║                              ║
        line[vector.originX] = startGlyph;                                          // ║ ║                              ║
        line[vector.targetX] = endGlyph;                                            // ║ ║                              ║
                                                                                    // ║ ║                              ║
        this.wallPositions.set(depth, vector.targetX);                              // ║ ║                              ║
                                                                                    // ║ ║                              ║
        return line.join("");                                                       // ║ ║                              ║
    }                                                                               // ║ ║                              ║
                                                                                    // ║ ║                              ║
    /**                                                                             // ║ ║                              ║
     * Multi-Witness Support: Unified side-by-side rendering.                       // ║ ║                              ║
     * @param {string} left - Content for left quadrant.                            // ║ ║                              ║
     * @param {string} right - Content for right quadrant.                          // ║ ║                              ║
     * @param {number} depth - Nesting level.                                       // ║ ║                              ║
     * @param {string} [topology] - Joint pattern (e.g. spine, t-top).              // ║ ║                              ║
     * @returns {string} Rendered dual-content line.                                // ║ ║                              ║
     */                                                                             // ║ ║                              ║
    public drawUnifiedLine(left: string, right: string, depth: number, topology: string = "spine"): string { // ║ ║     ║
        const line = this.createBaseLine();                                         // ║ ║                              ║
        const mid = SEED_TOPOLOGY.spine_x;                                          // ║ ║                              ║
                                                                                    // ║ ║                              ║
        if (topology === "t-top") {                                                 // ║ ║                              ║
            const start = this.layout.codeLeftPadding;                              // ║ ║                              ║
            for (let i = 0; i < left.length && start + i < this.layout.rootWidth - 3; i++) { // ║ ║                     ║
                line[start + i] = left[i];                                          // ║ ║                              ║
            }                                                                       // ║ ║                              ║
        } else {                                                                    // ║ ║                              ║
            line[mid] = SEED_TOPOLOGY.joints.heavy.v;                               // ║ ║                              ║
            const lStart = 3 + (depth * 2) + 2;                                     // ║ ║                              ║
            for (let i = 0; i < left.length && lStart + i < mid; i++) {             // ║ ║                              ║
                line[lStart + i] = left[i];                                         // ║ ║                              ║
            }                                                                       // ║ ║                              ║
            const rStart = mid + 3 + (depth * 2) + 2;                               // ║ ║                              ║
            for (let i = 0; i < right.length && rStart + i < this.layout.rootWidth - 3; i++) { // ║ ║                   ║
                line[rStart + i] = right[i];                                        // ║ ║                              ║
            }                                                                       // ║ ║                              ║
        }                                                                           // ║ ║                              ║
                                                                                    // ║ ║                              ║
        return line.join("");                                                       // ║ ║                              ║
    }                                                                               // ║ ║                              ║
                                                                                    // ║ ║                              ║
    /** Renders the terminal block anchor. */                                       // ║ ║                              ║
    public drawTerminalTag(tag: string): string {                                   // ║ ║                              ║
        const line = this.createBaseLine();                                         // ║ ║                              ║
        const rWall = this.getRightWallCol(0);                                      // ║ ║                              ║
        const label = `[${tag}] `;                                                  // ║ ║                              ║
                                                                                    // ║ ║                              ║
        const startCol = rWall - label.length - 1;                                  // ║ ║                              ║
        for (let i = 0; i < label.length; i++) {                                    // ║ ║                              ║
            line[startCol + i] = label[i];                                          // ║ ║                              ║
        }                                                                           // ║ ║                              ║
                                                                                    // ║ ║                              ║
        return line.join("");                                                       // ║ ║                              ║
    }                                                                               // ║ ║                              ║
                                                                                    // ║ ║                              ║
    /** Renders the horizontal master boundary. */                                  // ║ ║                              ║
    public drawMasterBar(isTop: boolean): string {                                  // ║ ║                              ║
        const barLength = this.layout.rootWidth - 3;                                // ║ ║                              ║
        const bar = new Array(barLength).fill(SEED_TOPOLOGY.joints.heavy.h);        // ║ ║                              ║
        bar[0] = isTop ? SEED_TOPOLOGY.joints.heavy.tl : SEED_TOPOLOGY.joints.heavy.bl; // ║ ║                          ║
        bar[barLength - 1] = isTop ? SEED_TOPOLOGY.joints.heavy.tr : SEED_TOPOLOGY.joints.heavy.br; // ║ ║              ║
        return "// " + bar.join("");                                                // ║ ║                              ║
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
//!║ ║ │ │ Witnessing the integrity of the L1 Drawer logic.                        │ │ ║ ║                              ║
//!║ ║ │ └─────────────────────────────────────────────────────────────────────────┘ │ ║ ║                              ║
// ║ ║ └─────────────────────────────────────────────────────────────────────────────┘ ║ ║                              ║
//                                                                                     // ║ ║                              ║
/** @internal Test block for isolated verification of the drawer. */                // ║ ║                              ║
export const __TESTS = {                                                            // ║ ║                              ║
    /** Verifies that the master bar renders the correct corner glyphs. */          // ║ ║                              ║
    test_draw_master_bar() {                                                        // ║ ║                              ║
        const dummyLayout: any = { rootWidth: 140 };                                // ║ ║                              ║
        const drawer = new VaultDrawer(dummyLayout as EngineVaultLayout);           // ║ ║                              ║
        const topBar = drawer.drawMasterBar(true);                                  // ║ ║                              ║
        console.assert(topBar.startsWith("// ╔"), "Top bar start fail");            // ║ ║                              ║
        console.assert(topBar.endsWith("╗"), "Top bar end fail");                   // ║ ║                              ║
    },                                                                              // ║ ║                              ║
    /** Verifies that depth modification renders the light vertical pillar. */      // ║ ║                              ║
    test_boundary_line_sink() {                                                     // ║ ║                              ║
        const dummyLayout: any = { rootWidth: 140, gapWidth: 10 };                  // ║ ║                              ║
        const drawer = new VaultDrawer(dummyLayout as EngineVaultLayout);           // ║ ║                              ║
        const line = drawer.drawBoundaryLine("test", 1, undefined, "sink");         // ║ ║                              ║
        console.assert(line.includes("╏"), "Sink glyph missing");                   // ║ ║                              ║
    }                                                                               // ║ ║                              ║
};                                                                                  // ║ ║                              ║
//                                                                                     // ║ ║                              ║
// ║ ║                                                                                 ║ ║                              ║
// ║ ║ Note: The geometric renderer converting mathematical limits to ASCII structure. ║ ║                              ║
// ║ ║ Scripture: "In the beginning God created the heaven and the earth." — Gen 1:1   ║ ║                              ║
// ║ ║                                                                                 ║ ║                              ║
// ║ ║                                                                                 ║ ║           [BLOCK:ROOT-->END] ║
// ║ ╚═════════════════════════════════════════════════════════════════════════════════╝ ║                              ║
// ║                                                                                     ║                              ║
// ╚═════════════════════════════════════════════════════════════════════════════════════╩══════════════════════════════╝
//
// ╠═==================================================================================================================═╣
