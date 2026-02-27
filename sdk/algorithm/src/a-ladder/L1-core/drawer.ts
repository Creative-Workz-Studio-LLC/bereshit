// #!omni:code -typescript\--> module
// ╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
// ║ [BLOCK:METADATA]                                                                                                   ║
// ╠════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣
// ║                                                                                                                    ║
// ║ Key:         B-DSA-DRAWER-V2                                                                                       ║
// ║ Version:     a-02.50                                                                                               ║
// ║ Purpose:     Refined, state-aware drawer implementing Hinge Physics and Unitary Containers.                         ║
// ║                                                                                                                    ║
// ╚════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝

// ╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
// ║ [BLOCK:SETUP]                                                                                                      ║
// ╠════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣

import { DsaRegistry } from "../L0-foundation/registry.ts";
import { VaultLayout, HingeVector } from "./engine.ts";

// ╚════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝

// ╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
// ║ [BLOCK:BODY]                                                                                                       ║
// ╠════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣

export class VaultDrawer {
    private layout: VaultLayout;
    public flipX: boolean = false;
    private wallPositions: Map<number, number> = new Map();

    constructor(layout: VaultLayout) {
        this.layout = layout;
        this.initializeWallMap();
    }

    private initializeWallMap() {
        // Root (0) is absolute
        this.wallPositions.set(0, this.layout.rootWidth - 1);
        // Level 1 starts at standard position
        this.wallPositions.set(1, (this.layout.rootWidth - 1) - this.layout.gapWidth);
    }

    /**
     * The Frame Covenant: Every line starts with // and maintains Root Pillars.
     */
    private createBaseLine(): string[] {
        const line = new Array(this.layout.rootWidth).fill(" ");
        line[0] = "/"; line[1] = "/";
        
        // Root Pillars (Level 0)
        line[this.getLeftWallCol(0)] = "║";
        line[this.getRightWallCol(0)] = "║";
        
        return line;
    }

    private getLeftWallCol(depth: number): number {
        const col = 3 + (depth * 2);
        return this.flipX ? this.layout.rootWidth - 1 - col : col;
    }

    private getRightWallCol(depth: number): number {
        const standard = this.wallPositions.get(depth) || ((this.layout.rootWidth - 1) - this.layout.gapWidth);
        return this.flipX ? this.layout.rootWidth - 1 - standard : standard;
    }

    /**
     * Renders a boundary line for a block, respecting stateful wall positions.
     */
    public drawBoundaryLine(content: string, depth: number, tag?: string, style: "normal" | "sink" | "popout" = "normal"): string {
        const line = this.createBaseLine();
        const glyph = style === "sink" ? "╏" : "║";
        
        const lCol = this.getLeftWallCol(depth);
        const rCol = this.getRightWallCol(depth);
        
        line[lCol] = glyph;
        line[rCol] = glyph;

        const label = tag ? `[${tag}] ` : "";
        const text = `${label}${content}`;
        const start = lCol + 2;
        
        for (let i = 0; i < text.length && start + i < rCol; i++) {
            line[start + i] = text[i];
        }

        return line.join("");
    }

    /**
     * Renders a Vault Line where code is present (Opening the left wall).
     */
    public drawVaultLine(code: string, depth: number): string {
        const line = this.createBaseLine();
        const rCol = this.getRightWallCol(depth);
        const glyph = this.layout.depthState === "sink" ? "╏" : "║";

        // Solid Right Wall of the block
        line[rCol] = glyph;
        
        // Ensure parent wall is NOT rendered if this wall has "popped" past it
        const parentCol = this.wallPositions.get(depth - 1);
        if (parentCol !== undefined && rCol <= parentCol) {
            line[parentCol] = "║";
        }

        // Code breathes through the open left door
        const codeStart = this.layout.codeLeftPadding;
        for (let i = 0; i < code.length && codeStart + i < rCol; i++) {
            line[codeStart + i] = code[i];
        }

        return line.join("");
    }

    /**
     * Renders a physical transition hinge (Pop/Sink).
     */
    public drawTransitionLine(vector: HingeVector, isTop: boolean, depth: number): string {
        const line = this.createBaseLine();
        const bridge = "─";

        let startGlyph: string, endGlyph: string;
        
        // Correct corner orientation based on horizontal path direction
        if (vector.targetX > vector.originX) {
            // Outward (Pop)
            startGlyph = isTop ? "└" : "┌";
            endGlyph = isTop ? "┐" : "┘";
        } else {
            // Inward (Sink or Return)
            startGlyph = isTop ? "┐" : "┘";
            endGlyph = isTop ? "└" : "┌";
        }

        const start = Math.min(vector.originX, vector.targetX);
        const end = Math.max(vector.originX, vector.targetX);

        // Draw the horizontal bridge
        for (let i = start + 1; i < end; i++) line[i] = bridge;
        
        // PHYSICAL JOIN: Corners occupy the vertical wall columns
        line[vector.originX] = startGlyph;
        line[vector.targetX] = endGlyph;

        // UPDATE STATE: Subsequent lines will use the target coordinate
        this.wallPositions.set(depth, vector.targetX);

        return line.join("");
    }

    /**
     * Multi-Witness Support: Unified Line rendering for side-by-side logic.
     */
    public drawUnifiedLine(left: string, right: string, depth: number, topology: string = "spine"): string {
        const line = this.createBaseLine();
        const mid = 121; // Absolute Witness Point

        if (topology === "t-top") {
            const start = this.layout.codeLeftPadding;
            for (let i = 0; i < left.length && start + i < this.layout.rootWidth - 3; i++) {
                line[start + i] = left[i];
            }
        } else {
            line[mid] = "║"; // The Internal Spine
            // Left content
            const lStart = 3 + (depth * 2) + 2;
            for (let i = 0; i < left.length && lStart + i < mid; i++) {
                line[lStart + i] = left[i];
            }
            // Right content
            const rStart = mid + 3 + (depth * 2) + 2;
            for (let i = 0; i < right.length && rStart + i < this.layout.rootWidth - 3; i++) {
                line[rStart + i] = right[i];
            }
        }

        return line.join("");
    }

    public drawTerminalTag(tag: string): string {
        const line = this.createBaseLine();
        const rWall = this.getRightWallCol(0);
        const label = `[${tag}] `;
        
        // Position tag inside the right pillar of the Root
        const startCol = rWall - label.length - 1;
        for (let i = 0; i < label.length; i++) {
            line[startCol + i] = label[i];
        }
        
        return line.join("");
    }

    public drawMasterBar(isTop: boolean): string {
        const barLength = this.layout.rootWidth - 3; // Subtract // and space
        const bar = new Array(barLength).fill("═");
        bar[0] = isTop ? "╔" : "╚";
        bar[barLength - 1] = isTop ? "╗" : "╝";
        return "// " + bar.join("");
    }
}
