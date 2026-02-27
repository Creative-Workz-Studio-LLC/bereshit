// #!omni:code -typescript\--> module
import { DsaEngine } from "../../L1-core/engine.ts";
import { VaultDrawer } from "../../L1-core/drawer.ts";
import { prompt } from "../../L0-foundation/utils.ts";

const engine = new DsaEngine();

export async function runInteractiveMode() {
    console.clear();
    console.log("--- DSA Diagonal Scope Analyzer ---");
    const oLine = parseInt(await prompt("Origin Line: ")) || 1;
    const tCol = parseInt(await prompt("Terminal Col: ")) || 121;
    
    const scope = { origin: { line: oLine, col: 1 }, terminal: { line: 10, col: tCol } };
    const layout = engine.deriveLayoutFromAnchors(scope, 0);
    const drawer = new VaultDrawer(layout);

    console.log(drawer.drawMasterBar(true));
    console.log(drawer.drawBoundaryLine("Interactive Analysis", 0, "ROOT"));
    console.log(drawer.drawMasterBar(false));

    await prompt("\nPress Enter to return...");
}
