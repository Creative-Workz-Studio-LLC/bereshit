// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/T00/R25/shared/lint_grid_test.ts
// key:     B-tov-cws-struct-tests-shared-lint-grid
// title:   CWS Struct — Block × Layer Lint Grid Tests
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-23
// authors: Nova Dawn (CPI-SI)
// purpose: Verify the block × layer lint grid orchestrator — block-first
//          traversal, vertical slicing (by block), horizontal slicing
//          (by layer), single-cell targeting, and result block stamping.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals } from "jsr:@std/assert";
import type { BlockLintChain, CheckFn } from "@lib/shared/mod.ts";
import { runLintGrid } from "@lib/shared/mod.ts";
import type { LintResult, LintGridOptions } from "@lib/foundation/mod.ts";
import { info, warn, error } from "@lib/foundation/mod.ts";

// ---------------------------------------------------------------------------
// Helpers — tagged result factories
// ---------------------------------------------------------------------------

const FILE = "test.rs";

/** Create a check function that returns results tagged with a marker. */
function check(marker: string, severity: "info" | "warn" | "error" = "info"): CheckFn {
  const fn = severity === "info" ? info : severity === "warn" ? warn : error;
  return () => [fn(FILE, `test/${marker}`, `Check: ${marker}`)];
}

/** Build a simple chain with one check per cell for easy verification. */
function buildTestChain(): BlockLintChain {
  return {
    pregate: [
      check("pregate-a"),
      check("pregate-b"),
    ],
    blocks: [
      {
        block: "metadata",
        structure: [check("meta-L1")],
        crossContainer: [check("meta-L2")],
        containers: [],
        content: [check("meta-L3")],
      },
      {
        block: "setup",
        structure: [check("setup-L1")],
        crossContainer: [check("setup-L2")],
        containers: [],
        content: [check("setup-L3")],
      },
      {
        block: "body",
        structure: [check("body-L1")],
        crossContainer: [check("body-L2")],
        containers: [],
        content: [check("body-L3")],
      },
      {
        block: "closing",
        structure: [check("closing-L1")],
        crossContainer: [check("closing-L2")],
        containers: [],
        content: [check("closing-L3")],
      },
    ],
  };
}

/** Extract rule names from results for easy assertion. */
function rules(results: LintResult[]): string[] {
  return results.map((r) => r.rule);
}

/** Extract block stamps from results. */
function blocks(results: LintResult[]): (string | undefined)[] {
  return results.map((r) => r.block);
}

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Full grid traversal (no focus)
// ---------------------------------------------------------------------------

Deno.test("[CWS-T00-104] runLintGrid: full grid — all checks run, block-first order", async () => {
  const chain = buildTestChain();
  const results = await runLintGrid(chain);

  // 2 pregate + 4 blocks × 3 layers = 14 results
  assertEquals(results.length, 14);

  // Block-first order: pregate → metadata(L1,L2,L3) → setup(L1,L2,L3) → ...
  assertEquals(rules(results), [
    "test/pregate-a", "test/pregate-b",
    "test/meta-L1", "test/meta-L2", "test/meta-L3",
    "test/setup-L1", "test/setup-L2", "test/setup-L3",
    "test/body-L1", "test/body-L2", "test/body-L3",
    "test/closing-L1", "test/closing-L2", "test/closing-L3",
  ]);
});

Deno.test("runLintGrid: full grid — results stamped with correct block", async () => {
  const chain = buildTestChain();
  const results = await runLintGrid(chain);

  assertEquals(blocks(results), [
    "structural", "structural",
    "metadata", "metadata", "metadata",
    "setup", "setup", "setup",
    "body", "body", "body",
    "closing", "closing", "closing",
  ]);
});

// ---------------------------------------------------------------------------
// Vertical slice — focusBlock
// ---------------------------------------------------------------------------

Deno.test("runLintGrid: focusBlock=metadata — pregate + metadata only", async () => {
  const chain = buildTestChain();
  const results = await runLintGrid(chain, { focusBlock: "metadata" });

  // 2 pregate + 3 metadata layers = 5
  assertEquals(results.length, 5);
  assertEquals(rules(results), [
    "test/pregate-a", "test/pregate-b",
    "test/meta-L1", "test/meta-L2", "test/meta-L3",
  ]);
});

Deno.test("runLintGrid: focusBlock=closing — pregate + closing only", async () => {
  const chain = buildTestChain();
  const results = await runLintGrid(chain, { focusBlock: "closing" });

  assertEquals(results.length, 5);
  assertEquals(rules(results), [
    "test/pregate-a", "test/pregate-b",
    "test/closing-L1", "test/closing-L2", "test/closing-L3",
  ]);
});

// ---------------------------------------------------------------------------
// Horizontal slice — focusLayer
// ---------------------------------------------------------------------------

Deno.test("runLintGrid: focusLayer=0 — pregate only", async () => {
  const chain = buildTestChain();
  const results = await runLintGrid(chain, { focusLayer: 0 });

  assertEquals(results.length, 2);
  assertEquals(rules(results), ["test/pregate-a", "test/pregate-b"]);
});

Deno.test("runLintGrid: focusLayer=1 — L1 structure across all blocks, no pregate", async () => {
  const chain = buildTestChain();
  const results = await runLintGrid(chain, { focusLayer: 1 });

  assertEquals(results.length, 4);
  assertEquals(rules(results), [
    "test/meta-L1", "test/setup-L1", "test/body-L1", "test/closing-L1",
  ]);
});

Deno.test("runLintGrid: focusLayer=2 — L2 container across all blocks, no pregate", async () => {
  const chain = buildTestChain();
  const results = await runLintGrid(chain, { focusLayer: 2 });

  assertEquals(results.length, 4);
  assertEquals(rules(results), [
    "test/meta-L2", "test/setup-L2", "test/body-L2", "test/closing-L2",
  ]);
});

Deno.test("runLintGrid: focusLayer=3 — L3 content across all blocks, no pregate", async () => {
  const chain = buildTestChain();
  const results = await runLintGrid(chain, { focusLayer: 3 });

  assertEquals(results.length, 4);
  assertEquals(rules(results), [
    "test/meta-L3", "test/setup-L3", "test/body-L3", "test/closing-L3",
  ]);
});

// ---------------------------------------------------------------------------
// Single cell — focusBlock + focusLayer
// ---------------------------------------------------------------------------

Deno.test("runLintGrid: metadata:R25 — single cell (metadata L1 only)", async () => {
  const chain = buildTestChain();
  const results = await runLintGrid(chain, { focusBlock: "metadata", focusLayer: 1 });

  assertEquals(results.length, 1);
  assertEquals(rules(results), ["test/meta-L1"]);
  assertEquals(results[0]!.block, "metadata");
});

Deno.test("runLintGrid: body:R5 — single cell (body L3 only)", async () => {
  const chain = buildTestChain();
  const results = await runLintGrid(chain, { focusBlock: "body", focusLayer: 3 });

  assertEquals(results.length, 1);
  assertEquals(rules(results), ["test/body-L3"]);
  assertEquals(results[0]!.block, "body");
});

Deno.test("runLintGrid: closing:R50 — pregate only (L0 is pregate, block filter ignored for L0)", async () => {
  const chain = buildTestChain();
  // focusLayer=0 means only pregate. focusBlock is set but pregate isn't per-block.
  const results = await runLintGrid(chain, { focusBlock: "closing", focusLayer: 0 });

  // Pre-gate runs (L0), but closing L1-L3 are skipped because focusLayer=0
  assertEquals(results.length, 2);
  assertEquals(rules(results), ["test/pregate-a", "test/pregate-b"]);
});

// ---------------------------------------------------------------------------
// Block stamp preservation — pre-existing block field not overwritten
// ---------------------------------------------------------------------------

Deno.test("runLintGrid: preserves pre-existing block field on results", async () => {
  const chain: BlockLintChain = {
    pregate: [],
    blocks: [{
      block: "setup",
      structure: [
        // This check returns a result already tagged as "metadata" (cross-block check)
        () => [{ file: FILE, severity: "info" as const, rule: "cross/check", message: "cross", block: "metadata" }],
      ],
      crossContainer: [],
      containers: [],
      content: [],
    }],
  };

  const results = await runLintGrid(chain);

  // The pre-existing "metadata" block should be preserved, not overwritten to "setup"
  assertEquals(results[0]!.block, "metadata");
});

// ---------------------------------------------------------------------------
// Empty chain
// ---------------------------------------------------------------------------

Deno.test("runLintGrid: empty chain — no results", async () => {
  const chain: BlockLintChain = { pregate: [], blocks: [] };
  const results = await runLintGrid(chain);
  assertEquals(results.length, 0);
});

// ---------------------------------------------------------------------------
// Async checks
// ---------------------------------------------------------------------------

Deno.test("runLintGrid: handles async check functions", async () => {
  const chain: BlockLintChain = {
    pregate: [
      async () => {
        await new Promise((r) => setTimeout(r, 1));
        return [info(FILE, "async/pregate", "async pregate")];
      },
    ],
    blocks: [{
      block: "metadata",
      structure: [
        async () => {
          await new Promise((r) => setTimeout(r, 1));
          return [info(FILE, "async/meta", "async metadata")];
        },
      ],
      crossContainer: [],
      containers: [],
      content: [],
    }],
  };

  const results = await runLintGrid(chain);
  assertEquals(results.length, 2);
  assertEquals(rules(results), ["async/pregate", "async/meta"]);
});

// ---------------------------------------------------------------------------
// Container axis — per-container checks within a block
// ---------------------------------------------------------------------------

/** Build a chain with per-container checks for container-axis testing. */
function buildContainerChain(): BlockLintChain {
  return {
    pregate: [check("pregate")],
    blocks: [
      {
        block: "setup",
        structure: [check("setup-struct")],
        crossContainer: [check("setup-cross")],
        containers: [
          { container: "Imports", checks: [check("setup-Imports")] },
          { container: "Constants", checks: [check("setup-Constants")] },
          { container: "CoreTypes", checks: [check("setup-CoreTypes")] },
        ],
        content: [check("setup-content")],
      },
      {
        block: "closing",
        structure: [check("closing-struct")],
        crossContainer: [check("closing-cross")],
        containers: [
          { container: "Cv", checks: [check("closing-Cv")] },
          { container: "X5", checks: [check("closing-X5")] },
        ],
        content: [check("closing-content")],
      },
    ],
  };
}

/** Extract container stamps from results. */
function containerStamps(results: LintResult[]): (string | undefined)[] {
  return results.map((r) => r.container);
}

Deno.test("runLintGrid: full grid with containers — container-first within block", async () => {
  const chain = buildContainerChain();
  const results = await runLintGrid(chain);

  // 1 pregate + setup(struct + cross + 3 containers + content) + closing(struct + cross + 2 containers + content)
  // = 1 + 6 + 5 = 12
  assertEquals(results.length, 12);
  assertEquals(rules(results), [
    "test/pregate",
    "test/setup-struct", "test/setup-cross",
    "test/setup-Imports", "test/setup-Constants", "test/setup-CoreTypes",
    "test/setup-content",
    "test/closing-struct", "test/closing-cross",
    "test/closing-Cv", "test/closing-X5",
    "test/closing-content",
  ]);
});

Deno.test("runLintGrid: container results stamped with container name", async () => {
  const chain = buildContainerChain();
  const results = await runLintGrid(chain);

  // Non-container checks should NOT have a container stamp
  // Container checks should have their container name
  assertEquals(containerStamps(results), [
    undefined,      // pregate
    undefined, undefined,  // setup struct + cross
    "Imports", "Constants", "CoreTypes",  // setup containers
    undefined,      // setup content
    undefined, undefined,  // closing struct + cross
    "Cv", "X5",    // closing containers
    undefined,      // closing content
  ]);
});

Deno.test("runLintGrid: focusContainer filters to one container", async () => {
  const chain = buildContainerChain();
  const results = await runLintGrid(chain, { focusBlock: "setup", focusContainer: "Imports" });

  // focusBlock=setup + focusContainer=Imports:
  // pregate runs (L0), setup struct(L1), setup cross(L2), Imports container only, setup content(L3)
  // But focusContainer only filters containers[], not crossContainer.
  // Wait — we also need pregate? No, focusBlock is set, so pregate runs.
  // And focusLayer is not set, so all layers run.
  // = 1 pregate + struct + cross + 1 container + content = 5
  assertEquals(results.length, 5);
  assertEquals(rules(results), [
    "test/pregate",
    "test/setup-struct", "test/setup-cross",
    "test/setup-Imports",
    "test/setup-content",
  ]);
  assertEquals(results[3]!.container, "Imports");
});

Deno.test("runLintGrid: focusContainer with no focusBlock — filters containers in all blocks", async () => {
  const chain = buildContainerChain();
  // Only focusContainer, no focusBlock — should still filter across all blocks
  const results = await runLintGrid(chain, { focusContainer: "Cv" });

  // pregate(1) + setup(struct+cross+0containers+content=3) + closing(struct+cross+Cv+content=4)
  // = 1 + 3 + 4 = 8
  assertEquals(results.length, 8);
  // The Cv container appears only in closing
  const cvResults = results.filter((r) => r.container === "Cv");
  assertEquals(cvResults.length, 1);
  assertEquals(cvResults[0]!.rule, "test/closing-Cv");
});

Deno.test("runLintGrid: focusLayer=2 includes crossContainer + containers", async () => {
  const chain = buildContainerChain();
  const results = await runLintGrid(chain, { focusLayer: 2 });

  // L2 = crossContainer + containers (per block)
  // setup: cross + 3 containers = 4
  // closing: cross + 2 containers = 3
  // = 7
  assertEquals(results.length, 7);
  assertEquals(rules(results), [
    "test/setup-cross",
    "test/setup-Imports", "test/setup-Constants", "test/setup-CoreTypes",
    "test/closing-cross",
    "test/closing-Cv", "test/closing-X5",
  ]);
});

Deno.test("runLintGrid: container preserves pre-existing container field", async () => {
  const chain: BlockLintChain = {
    pregate: [],
    blocks: [{
      block: "setup",
      structure: [],
      crossContainer: [],
      containers: [{
        container: "Imports",
        checks: [
          // Returns a result already tagged with a different container
          () => [{ file: FILE, severity: "info" as const, rule: "cross/container", message: "cross", container: "Constants" }],
        ],
      }],
      content: [],
    }],
  };

  const results = await runLintGrid(chain);
  // Pre-existing "Constants" should be preserved, not overwritten to "Imports"
  assertEquals(results[0]!.container, "Constants");
});

Deno.test("runLintGrid: combined focus — block + container + layer", async () => {
  const chain = buildContainerChain();
  // focusBlock=setup, focusContainer=CoreTypes, focusLayer=2
  // L2 only: crossContainer + containers (filtered to CoreTypes)
  const results = await runLintGrid(chain, { focusBlock: "setup", focusContainer: "CoreTypes", focusLayer: 2 });

  // crossContainer(1) + CoreTypes container(1) = 2
  assertEquals(results.length, 2);
  assertEquals(rules(results), ["test/setup-cross", "test/setup-CoreTypes"]);
});

// ============================================================================
// CLOSING
// ============================================================================
//
// The grid IS the detection matrix. Block × Container × Layer = three
// dimensions of structural health. All axes navigable, default traversal
// block-first, container-first within each block.
//
// "Look that thou make them after their pattern." — Exodus 25:40
// ============================================================================
