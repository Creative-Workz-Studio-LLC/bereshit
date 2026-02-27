// #!omni:code -typescript\--> module
import { DsaEngine } from "../../L1-core/engine.ts";
import { VaultDrawer } from "../../L1-core/drawer.ts";
import { prompt } from "../../L0-foundation/utils.ts";

const engine = new DsaEngine();

export async function runRender3DCube() {
    console.clear();
    console.log("--- Generating 3D Hierarchical Vault ---");
    const rootScope = { origin: { line: 1, col: 1 }, terminal: { line: 50, col: 121 } };
    const rootLayout = engine.deriveLayoutFromAnchors(rootScope, 3);
    const drawer = new VaultDrawer(rootLayout);

    console.log(drawer.drawOmniShebang("--typescript -executable"));
    console.log(drawer.drawHorizontalJoint());
    console.log("//");
    console.log(drawer.drawMasterBar(true));
    console.log(drawer.drawBoundaryLine("Master Vault", 0, "ROOT"));
    console.log(drawer.drawBlockConnector(0));
    
    // Normal Block
    console.log(drawer.drawNestedBar(1, true));
    console.log(drawer.drawBoundaryLine("Standard Flow", 1, "NORM"));
    console.log(drawer.drawVaultLine("console.log('Action');", 1));
    console.log(drawer.drawNestedBar(1, false));

    console.log(drawer.drawTerminalTag("BLOCK:ROOT-->END", 0));
    console.log(drawer.drawMasterBar(false));

    if (Deno.args.length === 0) {
        await prompt("\nRender Complete. Press Enter to return...");
    }
}

export async function runRenderSiblings() {
    console.clear();
    console.log("--- Generating 2-Block Unit (Side-by-Side Siblings) ---");
    const rootScope = { origin: { line: 1, col: 1 }, terminal: { line: 20, col: 121 } };
    const rootLayout = engine.deriveLayoutFromAnchors(rootScope, 0);
    const [leftLayout, rightLayout] = engine.splitLayout(rootLayout, 5);
    
    const leftDrawer = new VaultDrawer(leftLayout);
    const rightDrawer = new VaultDrawer(rightLayout);
    
    console.log(VaultDrawer.mergeSiblingLines(leftDrawer.drawMasterBar(true), rightDrawer.drawMasterBar(true)));
    console.log(VaultDrawer.mergeSiblingLines(
        leftDrawer.drawVaultLine("console.log('A');", 1),
        rightDrawer.drawVaultLine("console.log('B');", 1)
    ));
    console.log(VaultDrawer.mergeSiblingLines(leftDrawer.drawMasterBar(false), rightDrawer.drawMasterBar(false)));

    if (Deno.args.length === 0) {
        await prompt("\nRender Complete. Press Enter to return...");
    }
}

export async function runRenderDoubleWitness() {
    console.clear();
    console.log("--- Generating Double Witness (242-char) Standard ---");
    const rootScope = { origin: { line: 1, col: 1 }, terminal: { line: 40, col: 242 } };
    const rootLayout = engine.deriveLayoutFromAnchors(rootScope, 1, 0, 0, 2);
    const drawer = new VaultDrawer(rootLayout);

    console.log(drawer.drawMasterBar(true));
    console.log(drawer.drawUnifiedLine("[BLOCK:METADATA] The Hierarchical Master", "", 0, "t-top"));
    console.log(drawer.drawJointHorizon("t-down"));
    console.log(drawer.drawUnifiedLine("[BLOCK:SETUP]", "[BLOCK:BODY]", 0, "l-left"));
    console.log(drawer.drawJointHorizon("t-up"));
    console.log(drawer.drawUnifiedLine("[BLOCK:CLOSING]", "", 0, "t-bottom"));
    console.log(drawer.drawMasterBar(false));

    if (Deno.args.length === 0) {
        await prompt("\nRender Complete. Press Enter to return...");
    }
}

export async function runRenderPolish() {
    console.clear();
    console.log("--- Generating Polish Render: The Unitary Proper Box ---");
    const engine = new DsaEngine();
    const rootScope = { origin: { line: 1, col: 1 }, terminal: { line: 40, col: 121 } };
    const rootLayout = engine.deriveLayoutFromAnchors(rootScope, 0);
    const drawer = new VaultDrawer(rootLayout);

    console.log(drawer.drawMasterBar(true));
    
    // Pass 1: Standard Box Body
    console.log(drawer.drawBoundaryLine("The Unitary Proper Box", 1, "POLISH"));
    console.log(drawer.drawVaultLine("const state = 'sound';", 1));
    
    // Pass 2: POP-OUT Breach (Foregrounding)
    const currentRCol = (drawer as any).getRightWallCol(1);
    const popVector = engine.calculateHingeVector(currentRCol, "pop");
    console.log(drawer.drawTransitionLine(popVector, true, 1));
    
    // Popped State
    console.log(drawer.drawBoundaryLine("Popped Breach", 1, "POP"));
    console.log(drawer.drawVaultLine("match priority {", 1));
    console.log(drawer.drawVaultLine("  Self::Elastic => true,", 1));
    console.log(drawer.drawVaultLine("}", 1));
    
    // Pass 3: Return to Standard
    const returnVector = { originX: popVector.targetX, targetX: popVector.originX, length: popVector.length, direction: "inward" } as any;
    console.log(drawer.drawTransitionLine(returnVector, false, 1));

    // Pass 4: Final Closure with Internal Tag
    console.log(drawer.drawBoundaryLine("Returning to Covenant", 1, "NORM"));
    console.log(drawer.drawTerminalTag("BLOCK:ROOT-->END"));
    console.log(drawer.drawMasterBar(false));

    if (Deno.args.length === 0) {
        await prompt("\nRender Complete. Press Enter to return...");
    }
}
