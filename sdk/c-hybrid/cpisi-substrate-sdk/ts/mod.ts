//omni:key B-sdk-ts-sabbath
//omni:code --typescript
//omni:version a-01.00

/**
 * Sabbath Artifact Generator (Mind Layer)
 * Orchestrates the creation of session mission testimonies.
 */

// ============================================================================
// METADATA
// ============================================================================
// Key: B-sdk-ts-sabbath
// Purpose: CLI entry point for session artifact generation
// ============================================================================

import { generateSabbathArtifacts, SessionTestimony } from "./lib/substrate.ts";

async function main() {
  const chunks = [];
  const reader = Deno.stdin.readable.getReader();
  while (true) {
    const { done, value } = await reader.read();
    if (done) break;
    chunks.push(value);
  }
  
  const totalLength = chunks.reduce((acc, chunk) => acc + chunk.length, 0);
  const rawInputBytes = new Uint8Array(totalLength);
  let offset = 0;
  for (const chunk of chunks) {
    rawInputBytes.set(chunk, offset);
    offset += chunk.length;
  }
  
  const rawInput = new TextDecoder().decode(rawInputBytes);
  if (!rawInput) {
    console.error("No input received on stdin");
    Deno.exit(1);
  }

  try {
    const testimony: SessionTestimony = JSON.parse(rawInput);
    
    // Potential for Mind-layer narrative enhancement here
    // e.g. using a fast LLM to summarize events or applying complex rules
    
    const result = await generateSabbathArtifacts(testimony);

    if (result !== 0) {
      console.error(`Rust engine failed to generate artifacts (code: ${result})`);
      Deno.exit(1);
    }

    console.log(`✅ Session artifacts generated for: ${testimony.session_id}`);
  } catch (e) {
    console.error(`Failed to process sabbath artifacts: ${e}`);
    Deno.exit(1);
  }
}

if (import.meta.main) {
  main();
}
