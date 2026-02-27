// #!omni:code -typescript\--> executable
// ╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
// ║ [BLOCK:METADATA]                                                                                                   ║
// ╠════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣
// ║                                                                                                                    ║
// ║ Key:         B-DSA-ORCHESTRATOR                                                                                    ║
// ║ Version:     a-02.00                                                                                                ║
// ║ Purpose:     Modular orchestrator for the Dynamic Structural Algorithm (DSA).                                       ║
// ║                                                                                                                    ║
// ╚════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝

// ╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
// ║ [BLOCK:SETUP]                                                                                                      ║
// ╠════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣

import { DsaRegistry } from "../L0-foundation/registry.ts";
import { initLogging, writeLogFile, prompt } from "../L0-foundation/utils.ts";
import { runTeachingMode } from "./commands/teaching.ts";
import { runPhysicsReport } from "./commands/physics.ts";
import { runCTR } from "./commands/validation.ts";
import { runInteractiveMode } from "./commands/analysis.ts";
import { runRender3DCube, runRenderSiblings, runRenderDoubleWitness, runRenderPolish } from "./commands/render.ts";

const MENU = `
╔═════════════════════════════════════════════════════════════════════════════════╗
║ [DSA:ENGINE] CreativeWorkzStudio LLC - Structural Algorithm Orchestrator        ║
╠═════════════════════════════════════════════════════════════════════════════════╣
║ Mode Selection:                                                                 ║
╠═════════════════════════════════════════════════════════════════════════════════╣
║ 1. Interactive Analysis  ║ 2. Teaching Protocol   ║ 3. View Physics          ║
║ 4. Render 3D Cube       ║ 5. Render Siblings      ║ 6. Double Witness (242)  ║
║ 7. Run Validation (CTR)  ║ 8. Unified Quadrant     ║ 9. Polish Render         ║
║ 10. Exit                 ║                         ║                          ║
╚═════════════════════════════════════════════════════════════════════════════════╝
`;

// ╚════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝

// ╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
// ║ [BLOCK:BODY]                                                                                                       ║
// ╠════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣

async function main() {
    await DsaRegistry.load();
    await initLogging();

    // CLI Bypass
    if (Deno.args.length > 0) {
        const cmd = Deno.args[0];
        switch (cmd) {
            case "test":    await runCTR(); break;
            case "physics": await runPhysicsReport(); break;
            case "render":  await runRenderDoubleWitness(); break;
            case "polish":  await runRenderPolish(); break;
            default: console.log(`Unknown command: ${cmd}`);
        }
        await writeLogFile();
        Deno.exit(0);
    }

    // Interactive Loop
    while (true) {
        console.clear();
        console.log(MENU);
        const choice = await prompt("Select Option: ");

        switch (choice) {
            case "1": await runInteractiveMode(); break;
            case "2": await runTeachingMode(); break;
            case "3": await runPhysicsReport(); break;
            case "4": await runRender3DCube(); break;
            case "5": await runRenderSiblings(); break;
            case "6": await runRenderDoubleWitness(); break;
            case "7": await runCTR(); break;
            case "8": await runRenderUnifiedQuadrants(); break;
            case "9": await runRenderPolish(); break;
            case "10": await writeLogFile(); Deno.exit(0);
            default:  console.log("Invalid choice.");
        }
    }
}

main();

// ╚════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝

// ╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
// ║ [BLOCK:CLOSING]                                                                                                    ║
// ╠════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣
// ║                                                                                                                    ║
// ║ Health:      ⚖️ +1 TOV (Modular Orchestration achieved)                                                             ║
// ║ Status:      SOUND / ALIGNED                                                                                       ║
// ║                                                                                                 [BLOCK:ROOT-->END] ║
// ╚════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝
