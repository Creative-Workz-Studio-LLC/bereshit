// #!omni:code -typescript\--> module
import { DsaRegistry } from "../../L0-foundation/registry.ts";
import { CurrentState } from "../../../data/state.ts";
import { prompt } from "../../L0-foundation/utils.ts";

export async function runPhysicsReport() {
    console.log(`\n--- Master Constants (Data-Driven) ---`);
    console.log(`Source:       ${CurrentState.source.toUpperCase()} [${CurrentState.isHealthy ? "HEALTHY" : "TRIPPED"}]`);
    console.log(`U (Unit):     ${DsaRegistry.U}`);
    console.log(`N (Bible):    ${DsaRegistry.N}`);
    console.log(`Breath:       ${DsaRegistry.Breath}`);
    
    console.log(`\n--- Witness Scales ---`);
    console.log(`Witness (1):  ${DsaRegistry.Math.witness.single}`);
    console.log(`Witness (2):  ${DsaRegistry.Math.witness.double}`);
    console.log(`Witness (4):  ${DsaRegistry.Math.witness.full}`);

    if (Deno.args.length === 0) {
        await prompt("\nPress Enter to return...");
    }
}
