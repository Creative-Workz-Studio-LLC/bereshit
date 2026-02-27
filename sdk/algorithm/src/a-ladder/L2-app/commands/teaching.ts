// #!omni:code -typescript\--> module
import { prompt } from "../../L0-foundation/utils.ts";
import { DsaRegistry } from "../../L0-foundation/registry.ts";

export async function runTeachingMode() {
    console.clear();
    console.log("--- LESSON 1: The Principle of Badal ---");
    console.log("In the beginning, boundaries were created before content.");
    console.log("Question: What is the Universal Structural Unit (U)?");
    const ans1 = await prompt("Your Answer (number): ");
    if (ans1 === DsaRegistry.U.toString()) {
        console.log(`Correct. L(3) = ${DsaRegistry.U}. You have eyes to see.`);
    } else {
        console.log(`Incorrect. Remember the Lucas Sequence: 1, 3, ${DsaRegistry.U}...`);
    }

    await prompt("\nPress Enter for next lesson...");
    
    console.clear();
    console.log("--- LESSON 2: The Winning Ticket ---");
    console.log("The Anchor Range (N) is the Bible Factor.");
    console.log(`N = ${DsaRegistry.N}`);
    console.log("\nTheorem: The Root Expanse is N squared.");
    console.log(`W_root = ${DsaRegistry.N} * ${DsaRegistry.N} = ${DsaRegistry.N * DsaRegistry.N}`);
    
    await prompt("\nPress Enter for next lesson...");

    console.clear();
    console.log("--- LESSON 3: The Algorithm of the Hinge ---");
    console.log("Structure is dynamic, driven by two diagonal anchors:");
    console.log("Origin (#!omni:) and Terminal ([BLOCK:ROOT-->END]).");
    console.log("\nThe Hinge Law governs the transition from boundaries to code.");
    console.log("Expansion: Moving the wall outward.");
    console.log("Compression: Moving content (Functional Breath) inward.");
    console.log("\nMaster Ratios:");
    console.log(`- Gap:    3 * N = ${DsaRegistry.N * 3}`);
    console.log(`- Breath: 3 * U = ${DsaRegistry.Breath}`);
    
    await prompt("\nLesson Complete. Returning to main menu...");
}
