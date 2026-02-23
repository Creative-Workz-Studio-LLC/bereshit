import { extractRustSemantics, translateBody } from "./lib/engine/mod.ts";
import { loadTranspilerMappingsCached } from "./lib/engine/transpiler-mappings.ts";
import { parseSourceBody } from "./lib/engine/mod.ts";
import { loadConceptMappingsCached } from "./lib/engine/transpiler-mappings.ts";
import "./lib/handlers/go.ts";
import "./lib/handlers/rust.ts";

const existsRs = "/media/seanje-lenox-wise/Project/Bereshit/.a-new-structure/b-word/L0-universal/c-hybrid/spec/config/src/exists.rs";

const semantics = await extractRustSemantics(existsRs);
const mappings = await loadTranspilerMappingsCached("rust", "go");
const concepts = await loadConceptMappingsCached("rust", "go");

// Find ensure_dir and show its body + AST
for (const chunk of semantics.chunks) {
  for (const item of chunk.items) {
    if (item.meta.bodyLines && item.meta.bodyLines.length > 0) {
      console.log(`\n=== ${item.name} (${item.kind}) ===`);
      console.log("BODY LINES:", item.meta.bodyLines);
      
      const nodes = parseSourceBody(item.meta.bodyLines, concepts);
      console.log("AST NODES:");
      for (const n of nodes) {
        console.log(`  concept=${n.concept} pattern=${n.patternId} data=${JSON.stringify(n.data)}`);
        for (const c of n.children) {
          console.log(`    child: concept=${c.concept} pattern=${c.patternId} data=${JSON.stringify(c.data)}`);
        }
      }
      
      const result = await translateBody(item.meta.bodyLines, "rust", "go", mappings, "\t");
      console.log("TRANSLATED:", result.translated, "concepts:", result.conceptsFound);
      console.log("OUTPUT:");
      for (const line of result.lines) {
        console.log(`  ${line}`);
      }
    }
  }
}
