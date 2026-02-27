// #!omni:code -typescript\--> module
import { CTR } from "../../L1-core/runner.ts";
import { DsaEngine } from "../../L1-core/engine.ts";
import { prompt } from "../../L0-foundation/utils.ts";

export async function setupTests() {
    const engine = new DsaEngine();

    CTR.test("121 Baseline (Single Witness)", () => {
        const scope = { origin: { line: 1, col: 1 }, terminal: { line: 10, col: 121 } };
        const layout = engine.deriveLayoutFromAnchors(scope, 1, 0, 0, 1);
        CTR.expectShalem(layout.health);
        CTR.expectRatio(layout);
    });

    CTR.test("242 Double Witness (Width Scale)", () => {
        const scope = { origin: { line: 1, col: 1 }, terminal: { line: 10, col: 242 } };
        const layout = engine.deriveLayoutFromAnchors(scope, 1, 0, 0, 2);
        CTR.expectRatio(layout);
    });
}

export async function runCTR() {
    console.clear();
    await setupTests();
    await CTR.run();
    if (Deno.args.length === 0) {
        await prompt("\nValidation Complete. Press Enter to return...");
    }
}
