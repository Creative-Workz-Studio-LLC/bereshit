// #!omni:code -typescript\--> module
// ╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
// ║ [BLOCK:METADATA]                                                                                                   ║
// ╠════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣
// ║                                                                                                                    ║
// ║ Key:         B-DSA-ENGINE                                                                                          ║
// ║ Version:     a-01.00                                                                                                ║
// ║ Purpose:     Implementation of the N-Factor DSA Engine.                                                             ║
// ║                                                                                                                    ║
// ╚════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝

// ╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
// ║ [BLOCK:SETUP]                                                                                                      ║
// ╠════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣

import { DsaRegistry } from "../L0-foundation/registry.ts";

export interface Coordinate {
    line: number;
    col: number;
}

export interface AnchorScope {
    origin: Coordinate;
    terminal: Coordinate;
}

export interface HingeVector {
    originX: number;
    targetX: number;
    length: number;
    direction: "inward" | "outward";
}

export interface ShalemReport {
    score: number; // 0.0 to 1.0 (1.0 = Perfectly Aligned)
    isShalem: boolean;
    violations: string[];
}

export type DepthState = "standard" | "sink" | "pop";

export interface VaultLayout {
    rootWidth: number;
    rootHeight: number;
    gapWidth: number;
    nFactor: number;
    depth: number;
    depthState: DepthState; // The current Z-plane
    hingeOffset: number;
    functionalBreath: number;
    health: ShalemReport;
    codeLeftPadding: number;
    offsetX: number; // For side-by-side positioning
    offsetY: number; // For quadrant positioning
    witnessFactor: number; // 1 (121) or 2 (242)
}

// ╚════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝

// ╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
// ║ [BLOCK:BODY]                                                                                                       ║
// ╠════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣

export class DsaEngine {
    /**
     * Calculates the horizontal vector for a boundary transition.
     */
    public calculateHingeVector(parentWallCol: number, type: "pop" | "sink", witnessFactor: number = 1): HingeVector {
        const n = DsaRegistry.N;
        const u = DsaRegistry.U;
        
        let targetX: number;
        let direction: "inward" | "outward" = "outward";

        if (type === "pop") {
            // Pop-Out: Consumes 3N of the firmament
            const deltaX = 3 * n;
            targetX = parentWallCol + deltaX;
            direction = "outward";
        } else {
            // Sink-In: Recedes by 2U
            const deltaX = 2 * u;
            targetX = parentWallCol - deltaX;
            direction = "inward";
        }

        return {
            originX: parentWallCol,
            targetX,
            length: Math.abs(targetX - parentWallCol),
            direction
        };
    }
    private calculateCodeLeftPadding(depth: number, gapWidth: number, functionalBreath: number): number {
        // Tightly packed left walls length: 
        // prefix "// " (3) + "║ " * depth (2 * depth) + innerWall (1) + space (1)
        const leftWallsWidth = 3 + (depth * 2) + 2; 
        
        // Add gapWidth and functionalBreath to push the code dynamically into the expanse
        return leftWallsWidth + gapWidth + functionalBreath;
    }

    /**
     * Derives layout from diagonal scope with dynamic discernment (Outside-In).
     */
    public deriveLayoutFromAnchors(scope: AnchorScope, depth: number, offsetX: number = 0, offsetY: number = 0, witnessFactor: number = 1, state: DepthState = "standard"): VaultLayout {
        const rootWidth = Math.abs(scope.terminal.col - scope.origin.col) + 1;
        const rootHeight = Math.abs(scope.terminal.line - scope.origin.line) + 1;
        
        // Data-Driven Gap: Use Lucas sequence from Registry
        const ANCHOR_RATIO_GAP = DsaRegistry.Math.lucas.sequence[1]; // 3
        const ANCHOR_RATIO_ROOT = DsaRegistry.Math.lucas.sequence[4]; // 11
        
        // Adjust gap based on state: Sink-In reduces gap, Pop-Out expands it
        let gapModifier = 1.0;
        if (state === "sink") gapModifier = 0.5;
        if (state === "pop") gapModifier = 1.5;

        const gapWidth = Math.max(1, Math.floor(rootWidth * (ANCHOR_RATIO_GAP / (ANCHOR_RATIO_ROOT * witnessFactor)) * gapModifier));
        
        const hingeOffset = (depth * gapWidth) + DsaRegistry.Protection;
        const functionalBreath = DsaRegistry.Breath;

        const health = this.validateTension(rootWidth, rootHeight, depth, functionalBreath);

        return {
            rootWidth,
            rootHeight,
            gapWidth,
            nFactor: Math.sqrt(rootWidth),
            depth,
            depthState: state,
            hingeOffset,
            functionalBreath,
            health,
            codeLeftPadding: this.calculateCodeLeftPadding(depth, gapWidth, functionalBreath),
            offsetX,
            offsetY,
            witnessFactor
        };
    }

    /**
     * Splits a parent layout into a 2x2 Quadrant Grid.
     */
    public quadrantSplit(parent: VaultLayout, gapX: number = 4, gapY: number = 1): VaultLayout[] {
        const siblingWidth = Math.floor((parent.rootWidth - gapX) / 2);
        const siblingHeight = Math.floor((parent.rootHeight - gapY) / 2);
        
        const topLeft = this.deriveLayoutFromAnchors(
            { origin: { line: 0, col: 0 }, terminal: { line: siblingHeight, col: siblingWidth - 1 } },
            parent.depth, parent.offsetX, parent.offsetY, 1
        );

        const topRight = this.deriveLayoutFromAnchors(
            { origin: { line: 0, col: 0 }, terminal: { line: siblingHeight, col: siblingWidth - 1 } },
            parent.depth, parent.offsetX + siblingWidth + gapX, parent.offsetY, 1
        );

        const bottomLeft = this.deriveLayoutFromAnchors(
            { origin: { line: 0, col: 0 }, terminal: { line: siblingHeight, col: siblingWidth - 1 } },
            parent.depth, parent.offsetX, parent.offsetY + siblingHeight + gapY, 1
        );

        const bottomRight = this.deriveLayoutFromAnchors(
            { origin: { line: 0, col: 0 }, terminal: { line: siblingHeight, col: siblingWidth - 1 } },
            parent.depth, parent.offsetX + siblingWidth + gapX, parent.offsetY + siblingHeight + gapY, 1
        );

        return [topLeft, topRight, bottomLeft, bottomRight];
    }

    /**
     * Splits a parent layout into two side-by-side sibling layouts.
     */
    public splitLayout(parent: VaultLayout, gapBetween: number = 4): [VaultLayout, VaultLayout] {
        const siblingWidth = Math.floor((parent.rootWidth - gapBetween) / 2);
        
        const leftSibling = this.deriveLayoutFromAnchors(
            { origin: { line: 0, col: 0 }, terminal: { line: parent.rootHeight, col: siblingWidth - 1 } },
            parent.depth,
            parent.offsetX,
            parent.offsetY,
            1
        );

        const rightSibling = this.deriveLayoutFromAnchors(
            { origin: { line: 0, col: 0 }, terminal: { line: parent.rootHeight, col: siblingWidth - 1 } },
            parent.depth,
            parent.offsetX + siblingWidth + gapBetween,
            parent.offsetY,
            1
        );

        return [leftSibling, rightSibling];
    }

    /**
     * Calculates layout by starting from the innermost code and working outwards (Inside-Out).
     * This ensures the deepest code has proper "flat padding on the door side".
     */
    public calculateInsideOutLayout(innermostCode: string, depth: number, terminalCol: number): VaultLayout {
        const wallStackWidth = depth + 4; // prefix (3) + "║" * (depth + 1) = depth + 4
        
        // Start with terminalCol as a guess for rootWidth
        let rootWidth = terminalCol;
        const seq = DsaRegistry.Math.lucas.sequence;
        let gapWidth = Math.max(1, Math.floor(rootWidth * (seq[1] / seq[4])));
        let codeLeftPadding = this.calculateCodeLeftPadding(depth, gapWidth, DsaRegistry.Breath);
        let minRequiredInnerWidth = codeLeftPadding + innermostCode.length + wallStackWidth;

        // Iteratively stabilize rootWidth and gapWidth
        while (minRequiredInnerWidth > rootWidth) {
            rootWidth = minRequiredInnerWidth;
            gapWidth = Math.max(1, Math.floor(rootWidth * (seq[1] / seq[4])));
            codeLeftPadding = this.calculateCodeLeftPadding(depth, gapWidth, DsaRegistry.Breath);
            minRequiredInnerWidth = codeLeftPadding + innermostCode.length + wallStackWidth;
        }

        const rootHeight = 0; // Not calculated in this mode, or derived differently
        const hingeOffset = (depth * gapWidth) + DsaRegistry.Protection;
        const functionalBreath = DsaRegistry.Breath;
        
        const health = this.validateTension(rootWidth, rootHeight, depth, functionalBreath);

        return {
            rootWidth,
            rootHeight,
            gapWidth,
            nFactor: Math.sqrt(rootWidth),
            depth,
            hingeOffset,
            functionalBreath,
            health,
            codeLeftPadding,
            offsetX: 0,
            offsetY: 0,
            witnessFactor: 1
        };
    }

    /**
     * Discernment Layer: "Not everything is accepted and will pass."
     */
    private validateTension(width: number, height: number, depth: number, functionalBreath: number): ShalemReport {
        const violations: string[] = [];
        
        const ROOT_WIDTH_IDEAL = DsaRegistry.N * DsaRegistry.Math.lucas.sequence[4]; // 121

        if (width < ROOT_WIDTH_IDEAL) {
            violations.push(`Width Deviation: ${width} is less than ideal ${ROOT_WIDTH_IDEAL}`);
        } else if (width > ROOT_WIDTH_IDEAL && width > this.calculateOptimalRootWidth(1, depth, ROOT_WIDTH_IDEAL)) { // 1 is a dummy codeLength
            violations.push(`Width Deviation: ${width} exceeds ideal ${ROOT_WIDTH_IDEAL} for content`);
        }
        
        // The usable internal space for code, approximated
        const approximateUsableCodeWidth = width - (depth * DsaRegistry.Protection) - (depth * 2) - functionalBreath; 

        if (approximateUsableCodeWidth < functionalBreath) {
            violations.push(`Breath Compression: ${approximateUsableCodeWidth.toFixed(0)} is less than 3U (${functionalBreath})`);
        }

        return {
            score: violations.length === 0 ? 1.0 : Math.max(0, 1.0 - (violations.length * 0.2)),
            isShalem: violations.length === 0,
            violations
        };
    }

    /**
     * Flipped Logic Hinge: Calculates exactly where the '// ' prefix must land
     * so that the final wall hits the Terminal Anchor perfectly.
     */
    public calculateRightWallStart(layout: VaultLayout): number {
        const wallStackWidth = layout.depth + 1; // "║" * (depth + 1)
        const prefixWidth = 3; // "// "
        
        // Formula: Anchor - Walls - Prefix
        // This is the dynamic Hinge that adjusts based on rootWidth
        return Math.max(layout.codeLeftPadding, layout.rootWidth - wallStackWidth - prefixWidth);
    }

    /**
     * Calculates an optimal rootWidth that converges between outside-in and inside-out needs.
     * This is the 'bridge' for proper center justification.
     */
    public calculateOptimalRootWidth(innermostCodeLength: number, depth: number, targetTerminalCol: number = 121): number {
        const wallStackWidth = depth + 4; // prefix (3) + "║" * (depth + 1) = depth + 4
        let rootWidth = targetTerminalCol;
        const seq = DsaRegistry.Math.lucas.sequence;
        let gapWidth = Math.max(1, Math.floor(rootWidth * (seq[1] / seq[4])));
        let codeLeftPadding = this.calculateCodeLeftPadding(depth, gapWidth, DsaRegistry.Breath);
        let minWidthFromInside = codeLeftPadding + innermostCodeLength + wallStackWidth;

        while (minWidthFromInside > rootWidth) {
            rootWidth = minWidthFromInside;
            gapWidth = Math.max(1, Math.floor(rootWidth * (seq[1] / seq[4])));
            codeLeftPadding = this.calculateCodeLeftPadding(depth, gapWidth, DsaRegistry.Breath);
            minWidthFromInside = codeLeftPadding + innermostCodeLength + wallStackWidth;
        }
        
        // Optimal width is the maximum of the target terminal column and the minimum required from inside-out
        // This ensures the structure is wide enough to contain its deepest content, but ideally targets 121.
        return Math.max(targetTerminalCol, minWidthFromInside);
    }
}

// ╚════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝

// ╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
// ║ [BLOCK:CLOSING]                                                                                                    ║
// ╚════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝
