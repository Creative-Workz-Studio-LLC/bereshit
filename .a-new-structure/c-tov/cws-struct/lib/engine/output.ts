// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/engine/output.ts
// key:     B-tov-cws-struct-lib-engine-output
// title:   CWS Struct — Output Formatting
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-11
// authors: Nova Dawn (CPI-SI)
// purpose: Consistent terminal output across all format handlers.
//          Colors, severity display, per-file summaries, totals.
//          Extracted from the duplicated display code in each standalone linter.
//
// biblical_foundation: "Write the vision, and make it plain upon tables,
//   that he may run that readeth it." — Habakkuk 2:2
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import type {
  LintSummary, LintResult, Severity,
  HealthScore, AtomicAction,
} from "../foundation/mod.ts";
import { trueToLevel, levelToEmoji } from "../foundation/mod.ts";
import {
  matchRule, getByCode, codeSummary, related,
  CATALOG_SIZE, LAYER_COUNTS, LAYER_DESCRIPTIONS,
} from "../data/mod.ts";

// ---------------------------------------------------------------------------
// Constants — ANSI colors
// ---------------------------------------------------------------------------

export const COLORS = {
  reset: "\x1b[0m",
  red: "\x1b[31m",
  yellow: "\x1b[33m",
  blue: "\x1b[34m",
  green: "\x1b[32m",
  dim: "\x1b[2m",
  bold: "\x1b[1m",
} as const;

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

export function severityColor(s: Severity): string {
  switch (s) {
    case "error":
      return COLORS.red;
    case "warn":
      return COLORS.yellow;
    case "info":
      return COLORS.blue;
  }
}

export function severityIcon(s: Severity): string {
  switch (s) {
    case "error":
      return "E";
    case "warn":
      return "W";
    case "info":
      return "I";
  }
}

// ---------------------------------------------------------------------------
// Per-file display
// ---------------------------------------------------------------------------

/**
 * Color a health score value — ternary range (-100 to +100).
 * Uses Hebrew state boundaries from ALG-001:
 *   perfect/whole (34+) = green, sound/even (0-33) = yellow, lacking/wanting/broken (<0) = red.
 */
function healthColor(score: number): string {
  if (score >= 34) return COLORS.green;
  if (score >= 0) return COLORS.yellow;
  return COLORS.red;
}

export function printFileSummary(
  summary: LintSummary,
  verbose: boolean,
  check?: string,
  why?: boolean,
  deep?: boolean,
): void {
  // Determine if this is a focused view (block, container, or layer)
  const { focusBlock, focusContainer } = parseFocusFromCheck(check);
  const focusLabel = focusBlock
    ? focusContainer
      ? ` ${COLORS.dim}[${focusBlock.toUpperCase()}/${focusContainer}]${COLORS.reset}`
      : ` ${COLORS.dim}[${focusBlock.toUpperCase()}]${COLORS.reset}`
    : focusContainer
      ? ` ${COLORS.dim}[${focusContainer}]${COLORS.reset}`
      : "";

  const status =
    summary.errors === 0
      ? `${COLORS.green}OK${COLORS.reset}`
      : `${COLORS.red}FAIL${COLORS.reset}`;

  // Health score on the status line (if computed) — shows Hebrew state + emoji + trite
  const healthStr = summary.health
    ? `  ${healthColor(summary.health.total)}health: ${summary.health.total}/\u00b1100 ${summary.health.level} ${summary.health.emoji}${COLORS.reset}` +
      ` ${COLORS.dim}[t:${summary.health.trite}]${COLORS.reset}`
    : "";

  // Pragma directive shown dimmed after health score
  const pragmaStr = summary.pragma
    ? `  ${COLORS.dim}[${summary.pragma}]${COLORS.reset}`
    : "";

  console.log(
    `${status}  ${summary.file}${focusLabel}  (${summary.errors}E ${summary.warnings}W ${summary.infos}I)${healthStr}${pragmaStr}`,
  );

  // Filter results to focused block if --check is set
  const allResults = verbose
    ? summary.results
    : summary.results.filter((r) => r.severity !== "info");

  let show = focusBlock
    ? allResults.filter((r) => resultMatchesBlock(r, focusBlock))
    : allResults;

  if (focusContainer) {
    show = show.filter((r) => r.container === focusContainer);
  }

  for (const r of show) {
    const color = severityColor(r.severity);
    const icon = severityIcon(r.severity);
    const lineRef = r.line ? `${COLORS.dim}:${r.line}${COLORS.reset} ` : "";
    const codeTag = r.errorCode
      ? `${COLORS.dim}${r.errorCode}${COLORS.reset} `
      : "";
    const layerTag = verbose && r.layerName
      ? `${COLORS.dim}[${r.layerName}]${COLORS.reset} `
      : "";
    console.log(`  ${color}[${icon}]${COLORS.reset} ${layerTag}${codeTag}${lineRef}${r.rule}: ${r.message}`);

    // --why: show reasoning chain for each result
    if (why) {
      printWhyAnnotation(r, deep);
    }
  }

  // Verbose: show file anatomy (directives, blocks, identity) before health
  if (verbose && summary.health) {
    printFileAnatomy(summary);
  }

  // Verbose: show health breakdown (layer-grouped, with optional block focus)
  if (verbose && summary.health) {
    printHealthBreakdown(summary.health, focusBlock, deep);
  }

  if (show.length > 0 || (verbose && summary.health)) console.log("");
}

/**
 * Print reasoning chain for a single lint result.
 * Shows: catalog summary, layer, category, suggestion, traceability.
 * With --deep: also shows related codes (same category/layer).
 *
 * "Here's what I see, why I flagged it, and where the rule comes from."
 */
function printWhyAnnotation(r: LintResult, deep?: boolean): void {
  const entry = matchRule(r.rule);
  if (!entry) {
    console.log(`    ${COLORS.dim}why: no catalog entry for '${r.rule}'${COLORS.reset}`);
    return;
  }

  // One-liner summary from catalog
  const summary = codeSummary(entry.code);
  console.log(`    ${COLORS.dim}${summary}${COLORS.reset}`);

  // Layer + category + suggestion
  const layerDesc = LAYER_DESCRIPTIONS[entry.layerName] ?? entry.layerName;
  console.log(`    ${COLORS.dim}why: ${layerDesc}${COLORS.reset}`);
  console.log(`    ${COLORS.dim}fix: ${entry.suggestionTemplate}${COLORS.reset}`);
  console.log(`    ${COLORS.dim}src: ${entry.checkFunction}()${COLORS.reset}`);

  // Deep: show related codes in same category
  if (deep) {
    const neighbors = related(entry.code);
    const sameCatCodes = neighbors.sameCategory.map((e) => e.code);
    if (sameCatCodes.length > 0) {
      const preview = sameCatCodes.slice(0, 4).join(", ");
      const more = sameCatCodes.length > 4 ? ` (+${sameCatCodes.length - 4})` : "";
      console.log(`    ${COLORS.dim}related: ${preview}${more}${COLORS.reset}`);
    }
  }
}

/**
 * Parse the --check string for block and container focus.
 * Container names preserve original case; block/layer names are case-insensitive.
 */
function parseFocusFromCheck(check?: string): { focusBlock?: string; focusContainer?: string } {
  if (!check) return {};
  const BLOCK_NAMES = new Set(["metadata", "setup", "body", "closing"]);
  const LAYER_NAMES = new Set(["r50", "r25", "r10", "r5"]);
  const parts = check.split(":");
  let focusBlock: string | undefined;
  let focusContainer: string | undefined;
  for (const part of parts) {
    const lower = part.toLowerCase();
    if (BLOCK_NAMES.has(lower)) focusBlock = lower;
    else if (LAYER_NAMES.has(lower)) { /* skip layers — handled by grid focus */ }
    else if (part) focusContainer = part;
  }
  return { focusBlock, focusContainer };
}

/**
 * Match a result to a block — prefer explicit block field, fall back to rule prefix.
 * With block-first execution, results have block stamped by runLintGrid().
 * The rule-prefix fallback covers non-code handlers and backward compatibility.
 */
function resultMatchesBlock(r: LintResult, block: string): boolean {
  if (r.block) return r.block === block;
  return ruleMatchesBlock(r.rule, block);
}

function ruleMatchesBlock(rule: string, block: string): boolean {
  const r = rule.toLowerCase();
  switch (block) {
    case "structural":
      return r.startsWith("block/") || r.startsWith("separator/");
    case "metadata":
      return r.startsWith("identity/") || r.startsWith("directive/") ||
             r.startsWith("field/") || r.startsWith("content/metadata");
    case "setup":
      return r.startsWith("setup/") || r.startsWith("content/setup");
    case "body":
      return r.startsWith("body/") || r.startsWith("content/body");
    case "content":
      return r.startsWith("placement/") || r.startsWith("content/placement");
    case "closing":
      return r.startsWith("closing/") || r.startsWith("content/closing");
    default:
      return r.includes(block);
  }
}

// ---------------------------------------------------------------------------
// File anatomy — verbose: show what the linter sees
// ---------------------------------------------------------------------------

/**
 * Print file anatomy — the linter's view of this file, block by block.
 *
 * When anatomy data is available (from inspect), shows the file's actual
 * content hierarchy: block → container → content (with real values).
 * Falls back to health action extraction when anatomy isn't populated.
 *
 * Same hierarchy everywhere: schema → test → linter → output.
 * Block → container → content. The system knows itself.
 *
 * "Write the vision, and make it plain upon tables." — Habakkuk 2:2
 */
function printFileAnatomy(summary: LintSummary): void {
  const anatomy = summary.anatomy;
  const health = summary.health;

  if (!health && !anatomy) return;

  // ── Rich anatomy (from inspect — block by block with real values) ──
  if (anatomy) {
    for (const block of anatomy.blocks) {
      // Block header with line range
      const blockHealth = health?.blocks.find(
        (b) => b.block === block.name.toLowerCase(),
      );
      const scoreStr = blockHealth
        ? ` ${healthColor(blockHealth.score)}${blockHealth.score}${COLORS.reset}`
        : "";
      console.log(
        `  ${COLORS.bold}${block.name}${COLORS.reset}` +
        ` ${COLORS.dim}:${block.startLine}-${block.endLine} (${block.lines} lines)${COLORS.reset}${scoreStr}`,
      );

      // Block-specific content
      const blockLower = block.name.toLowerCase();

      // METADATA: directives, identity, field-values
      if (blockLower === "metadata") {
        // Directives with actual values
        if (Object.keys(anatomy.directives).length > 0) {
          console.log(`    ${COLORS.dim}directives${COLORS.reset}`);
          for (const [key, value] of Object.entries(anatomy.directives)) {
            // Skip internal shebang keys, show user-facing directives
            if (key.startsWith("#!omni:")) continue;
            const display = value ? ` = ${value}` : "";
            console.log(`      ${key}${display}`);
          }
        }

        // Identity fields with actual values
        if (anatomy.identity && Object.keys(anatomy.identity).length > 0) {
          for (const [source, fields] of Object.entries(anatomy.identity)) {
            console.log(`    ${COLORS.dim}${source}${COLORS.reset} ${COLORS.dim}(${fields.length} fields)${COLORS.reset}`);
            for (const f of fields) {
              // Truncate long values for display
              const val = f.value.length > 60
                ? f.value.substring(0, 57) + "..."
                : f.value;
              console.log(`      ${COLORS.dim}${f.key.padEnd(20)}${COLORS.reset} ${val}`);
            }
          }
        }
      }

      // SETUP / BODY / CLOSING: show detected sections
      const blockSections = anatomy.sections[block.name];
      if (blockSections && blockSections.length > 0) {
        console.log(`    ${COLORS.dim}sections${COLORS.reset}`);
        for (const s of blockSections) {
          console.log(`      ${COLORS.dim}:${s.line}${COLORS.reset} ${s.name}`);
        }
      }

      // Content summary for this block
      const blockContent = anatomy.content?.[block.name];
      if (blockContent && blockContent.length > 0) {
        console.log(`    ${COLORS.dim}content${COLORS.reset}  ${blockContent.join(", ")}`);
      }
    }

    console.log("");
    return;
  }

  // ── Fallback: extract from health actions (no real values) ──
  if (!health) return;

  // Pragma
  if (summary.pragma) {
    console.log(`  ${COLORS.dim}── pragma ──${COLORS.reset}`);
    console.log(`    ${summary.pragma}`);
  }

  // Blocks — from structural actions
  const structuralBlock = health.blocks.find((b) => b.block === "structural");
  if (structuralBlock) {
    const blocksContainer = structuralBlock.containers.find((c) => c.section === "blocks");
    if (blocksContainer && blocksContainer.actions.length > 0) {
      console.log(`  ${COLORS.dim}── blocks ──${COLORS.reset}`);
      const blockNames: string[] = [];
      for (const action of blocksContainer.actions) {
        const m = action.check.match(/^block\/([A-Z]+)$/);
        if (m && m[1]) blockNames.push(m[1]);
      }
      if (blockNames.length > 0) {
        const icons = blockNames.map((name) => {
          const action = blocksContainer.actions.find((a) => a.check === `block/${name}`);
          const icon = action && action.direction > 0
            ? `${COLORS.green}+${COLORS.reset}`
            : action && action.direction < 0
              ? `${COLORS.red}-${COLORS.reset}`
              : `${COLORS.dim}.${COLORS.reset}`;
          return `${icon} ${name}`;
        });
        console.log(`    ${icons.join("  ")}`);
      }
    }
  }

  // Identity summary — from health actions
  const metaBlock = health.blocks.find((b) => b.block === "metadata");
  if (metaBlock) {
    const idContainer = metaBlock.containers.find((c) => c.section === "identity");
    if (idContainer && idContainer.actions.length > 0) {
      console.log(`  ${COLORS.dim}── identity (${idContainer.aligned}/${idContainer.total}) ──${COLORS.reset}`);
    }
  }

  console.log("");
}

// ---------------------------------------------------------------------------
// Layer mapping — R[n] detection granularity
// ---------------------------------------------------------------------------

/**
 * Map container sections to the 4-layer detection chain.
 * The chain IS the detection granularity (ALG-003):
 *   Layer 0 (R[50]) — WHOLE FILE: does this file have structure?
 *   Layer 1 (R[25]) — STRUCTURE: are sections correctly organized?
 *   Layer 2 (R[10]) — CONTAINER: is content in the right place?
 *   Layer 3 (R[5])  — CONTENT: does content match expectations?
 */
/**
 * Layer names — scale-independent. These apply at any zoom level:
 * file, block, section. R[n] detection granularity from ALG-003.
 */
const LAYER_NAMES = [
  "EXISTENCE",     // Layer 0 — R[50]: Does it exist and have boundaries?
  "ORGANIZATION",  // Layer 1 — R[25]: Is it correctly structured internally?
  "PLACEMENT",     // Layer 2 — R[10]: Is content where it belongs?
  "SUBSTANCE",     // Layer 3 — R[5]:  Does content match expectations?
] as const;

const LAYER_RESOLUTION = ["R[50]", "R[25]", "R[10]", "R[5]"] as const;

/** Determine which layer a container belongs to by its section name. */
function containerLayer(section: string): number {
  if (section === "blocks" || section === "separators") return 0;
  if (section === "content-expectations") return 3;
  if (section === "placement") return 2;
  // directives, identity, field-values, comment-meta, ordering → organization
  return 1;
}

/**
 * Print per-layer health breakdown using the 4-layer detection chain.
 * Collects containers across all blocks and groups by detection layer.
 *
 * When focusBlock is set, only shows containers belonging to that block —
 * the block viewed as its own world with the same 4-layer pattern.
 */
function printHealthBreakdown(health: HealthScore, focusBlock?: string, deep?: boolean): void {
  // Collect all containers (optionally filtered to focused block)
  type LayerItem = { block: string; container: typeof health.blocks[0]["containers"][0] };
  const layers: [LayerItem[], LayerItem[], LayerItem[], LayerItem[]] =
    [[], [], [], []];

  let totalAlignedAll = 0;
  let totalNeutralAll = 0;
  let totalMisalignedAll = 0;
  let totalActionsAll = 0;

  for (const block of health.blocks) {
    for (const container of block.containers) {
      if (container.total === 0) continue;
      if (focusBlock && block.block !== focusBlock) continue;
      const layer = containerLayer(container.section);
      layers[layer]!.push({ block: block.block, container });
      totalAlignedAll += container.aligned;
      totalNeutralAll += container.neutral;
      totalMisalignedAll += container.misaligned;
      totalActionsAll += container.total;
    }
  }

  // Summary line
  const parts = [`${totalAlignedAll} aligned`];
  if (totalNeutralAll > 0) parts.push(`${totalNeutralAll} neutral`);
  if (totalMisalignedAll > 0) parts.push(`${totalMisalignedAll} misaligned`);

  const scope = focusBlock
    ? `${focusBlock.toUpperCase()}: `
    : "";

  console.log(
    `  ${COLORS.dim}── ${scope}health: ${parts.join(", ")} ` +
    `(${totalActionsAll} actions) ──${COLORS.reset}`,
  );

  // Print each layer
  for (let i = 0; i < 4; i++) {
    const items = layers[i]!;
    if (items.length === 0) continue;

    // Compute layer score — weighted average by total actions
    const layerActions = items.reduce((n, it) => n + it.container.total, 0);
    const layerAligned = items.reduce((n, it) => n + it.container.aligned, 0);
    const layerMisaligned = items.reduce((n, it) => n + it.container.misaligned, 0);
    const layerScore = layerActions > 0
      ? Math.round(((layerAligned - layerMisaligned) / layerActions) * 100)
      : 0;

    const lColor = healthColor(layerScore);
    console.log(
      `  ${lColor}[${layerScore}]${COLORS.reset} ` +
      `${COLORS.bold}Layer ${i}${COLORS.reset} — ` +
      `${LAYER_NAMES[i]} ${COLORS.dim}(${LAYER_RESOLUTION[i]})${COLORS.reset}`,
    );

    for (const { block, container } of items) {
      const cColor = healthColor(container.score);
      // In focused view, omit the block prefix (it's redundant)
      const label = focusBlock ? container.section : `${block}/${container.section}`;
      const detail = container.misaligned > 0
        ? ` ${COLORS.dim}(${container.aligned}/${container.aligned + container.misaligned}` +
          `${container.neutral > 0 ? `, ${container.neutral} neutral` : ""})${COLORS.reset}`
        : ` ${COLORS.dim}(${container.aligned}/${container.aligned})${COLORS.reset}`;
      console.log(
        `    ${cColor}${container.score}${COLORS.reset} ${label}${detail}`,
      );

      // Deep mode: show individual actions within each container
      if (deep && container.actions.length > 0) {
        // Compact layout: ~4 checks per line for aligned, one per line for misaligned
        const aligned: string[] = [];
        const misaligned: AtomicAction[] = [];
        const neutral: string[] = [];

        for (const action of container.actions) {
          const shortCheck = action.check.split("/").pop() ?? action.check;
          if (action.direction > 0) {
            aligned.push(`${COLORS.green}+${COLORS.reset}${shortCheck}`);
          } else if (action.direction < 0) {
            misaligned.push(action);
          } else {
            neutral.push(`${COLORS.dim}.${shortCheck}${COLORS.reset}`);
          }
        }

        // Print aligned checks in compact rows (4 per line)
        if (aligned.length > 0) {
          for (let j = 0; j < aligned.length; j += 4) {
            const row = aligned.slice(j, j + 4).join("  ");
            console.log(`      ${row}`);
          }
        }

        // Print misaligned checks with reason + catalog suggestion (one per line)
        for (const action of misaligned) {
          const shortCheck = action.check.split("/").pop() ?? action.check;
          const reason = action.reason
            ? `  ${COLORS.dim}${action.reason}${COLORS.reset}`
            : "";
          console.log(`      ${COLORS.red}-${shortCheck}${COLORS.reset}${reason}`);

          // Catalog enrichment: show code + fix if available
          const entry = matchRule(action.check);
          if (entry) {
            console.log(`        ${COLORS.dim}${entry.code} fix: ${entry.suggestionTemplate}${COLORS.reset}`);
          }
        }

        // Print neutral checks in compact rows
        if (neutral.length > 0) {
          for (let j = 0; j < neutral.length; j += 4) {
            const row = neutral.slice(j, j + 4).join("  ");
            console.log(`      ${row}`);
          }
        }
      }
    }
  }
}

// ---------------------------------------------------------------------------
// Totals display
// ---------------------------------------------------------------------------

export function printTotals(summaries: LintSummary[]): void {
  const totalFiles = summaries.length;
  const passFiles = summaries.filter((s) => s.errors === 0).length;
  const totalErrors = summaries.reduce((n, s) => n + s.errors, 0);
  const totalWarnings = summaries.reduce((n, s) => n + s.warnings, 0);
  const totalInfos = summaries.reduce((n, s) => n + s.infos, 0);

  // Compute average health across files that have health scores
  const healthFiles = summaries.filter((s) => s.health !== undefined);
  const avgHealth = healthFiles.length > 0
    ? Math.round(healthFiles.reduce((sum, s) => sum + s.health!.total, 0) / healthFiles.length)
    : undefined;

  console.log("");
  console.log(
    `${COLORS.bold}Summary:${COLORS.reset} ${totalFiles} files, ${passFiles} clean`,
  );
  console.log(
    `  ${COLORS.red}${totalErrors} errors${COLORS.reset}  ` +
      `${COLORS.yellow}${totalWarnings} warnings${COLORS.reset}  ` +
      `${COLORS.blue}${totalInfos} info${COLORS.reset}`,
  );

  if (avgHealth !== undefined) {
    const hColor = healthColor(avgHealth);
    const avgLevel = trueToLevel(avgHealth);
    const avgEmoji = levelToEmoji(avgLevel);
    console.log(
      `  ${hColor}avg health: ${avgHealth}/\u00b1100 ${avgLevel} ${avgEmoji}${COLORS.reset}` +
        ` ${COLORS.dim}(${healthFiles.length} scored)${COLORS.reset}`,
    );
  }

  // Per-block breakdown (only when health data exists and multiple files)
  if (healthFiles.length > 1) {
    printBlockBreakdown(healthFiles);
    printLayerBreakdown(healthFiles);
  }

  // Top issues (only when errors exist and multiple files)
  if (totalErrors > 0 && summaries.length > 1) {
    printTopIssues(summaries);
  }

  // Catalog coverage — unique codes triggered across the run
  if (summaries.length > 1) {
    printCatalogCoverage(summaries);
  }

  if (totalErrors === 0) {
    console.log(
      `\n${COLORS.green}All files structurally valid.${COLORS.reset}`,
    );
  } else {
    console.log(
      `\n${COLORS.red}${totalErrors} structural errors must be fixed.${COLORS.reset}`,
    );
  }
}

/**
 * Print per-block health averages across all scored files.
 * Shows METADATA / SETUP / BODY / CLOSING with average score, Hebrew state, and file count.
 */
function printBlockBreakdown(healthFiles: LintSummary[]): void {
  const blockNames = ["METADATA", "SETUP", "BODY", "CLOSING"];
  const blockData = new Map<string, { scores: number[]; fileCount: number }>();

  for (const block of blockNames) {
    blockData.set(block, { scores: [], fileCount: 0 });
  }

  for (const s of healthFiles) {
    if (!s.health) continue;
    for (const block of s.health.blocks) {
      const data = blockData.get(block.block);
      if (data) {
        data.scores.push(block.score);
        data.fileCount++;
      }
    }
  }

  console.log("");
  console.log(`  ${COLORS.bold}Per-block:${COLORS.reset}`);

  for (const block of blockNames) {
    const data = blockData.get(block)!;
    if (data.scores.length === 0) continue;

    const avg = Math.round(
      data.scores.reduce((sum, s) => sum + s, 0) / data.scores.length,
    );
    const level = trueToLevel(avg);
    const emoji = levelToEmoji(level);
    const color = healthColor(avg);
    const pad = block.padEnd(9);

    console.log(
      `    ${color}${pad}${COLORS.reset} ` +
        `${color}${String(avg).padStart(4)}/100${COLORS.reset} ${level.padEnd(7)} ${emoji}` +
        `  ${COLORS.dim}(${data.fileCount}/${healthFiles.length} files)${COLORS.reset}`,
    );
  }
}

/**
 * Print per-layer pass/fail breakdown using the R[n] detection chain.
 * Aggregates across all files: how many files pass each detection layer.
 */
function printLayerBreakdown(healthFiles: LintSummary[]): void {
  // Layer stats: pass = all containers in this layer are aligned, fail = any misaligned
  const layerStats = [
    { name: "Whole file", res: "R[50]", pass: 0, fail: 0, cascade: 0 },
    { name: "Structure", res: "R[25]", pass: 0, fail: 0, cascade: 0 },
    { name: "Placement", res: "R[10]", pass: 0, fail: 0, cascade: 0 },
    { name: "Content", res: "R[5]", pass: 0, fail: 0, cascade: 0 },
  ];

  for (const s of healthFiles) {
    if (!s.health) continue;

    // Group containers by layer
    const layerMisaligned = [0, 0, 0, 0];
    const layerTotal = [0, 0, 0, 0];
    const layerNeutral = [0, 0, 0, 0];

    for (const block of s.health.blocks) {
      for (const container of block.containers) {
        if (container.total === 0) continue;
        const layer = containerLayer(container.section);
        layerTotal[layer]! += container.total;
        layerMisaligned[layer]! += container.misaligned;
        layerNeutral[layer]! += container.neutral;
      }
    }

    for (let i = 0; i < 4; i++) {
      if (layerTotal[i]! === 0) continue;
      // All neutral = cascade (upstream failure suppressed this layer)
      if (layerNeutral[i]! === layerTotal[i]!) {
        layerStats[i]!.cascade++;
      } else if (layerMisaligned[i]! === 0) {
        layerStats[i]!.pass++;
      } else {
        layerStats[i]!.fail++;
      }
    }
  }

  console.log("");
  console.log(`  ${COLORS.bold}Per-layer:${COLORS.reset}`);

  for (const layer of layerStats) {
    const total = layer.pass + layer.fail + layer.cascade;
    if (total === 0) continue;

    const cascadeStr = layer.cascade > 0
      ? ` ${COLORS.dim}(${layer.cascade} cascaded)${COLORS.reset}`
      : "";

    const passColor = layer.fail === 0 ? COLORS.green : COLORS.reset;
    const failColor = layer.fail > 0 ? COLORS.red : COLORS.dim;

    console.log(
      `    ${COLORS.dim}${layer.res}${COLORS.reset} ${layer.name.padEnd(10)}  ` +
        `${passColor}${layer.pass} pass${COLORS.reset}, ` +
        `${failColor}${layer.fail} fail${COLORS.reset}` +
        cascadeStr,
    );
  }
}

/**
 * Print top recurring issues across all files.
 * Groups errors by rule, shows the 5 most frequent with file count.
 */
function printTopIssues(summaries: LintSummary[], limit = 5): void {
  // Count errors by rule (with error code)
  const ruleCounts = new Map<string, { count: number; code: string; rule: string }>();

  for (const s of summaries) {
    // Track unique rules per file (count files, not total occurrences)
    const seenRules = new Set<string>();
    for (const r of s.results) {
      if (r.severity !== "error") continue;
      const key = r.rule;
      if (seenRules.has(key)) continue;
      seenRules.add(key);

      const existing = ruleCounts.get(key);
      if (existing) {
        existing.count++;
      } else {
        ruleCounts.set(key, {
          count: 1,
          code: r.errorCode ?? "",
          rule: r.rule,
        });
      }
    }
  }

  if (ruleCounts.size === 0) return;

  // Sort by count descending, take top N
  const sorted = [...ruleCounts.values()]
    .sort((a, b) => b.count - a.count)
    .slice(0, limit);

  console.log("");
  console.log(`  ${COLORS.bold}Top issues:${COLORS.reset}`);

  for (let i = 0; i < sorted.length; i++) {
    const issue = sorted[i]!;
    const codeStr = issue.code
      ? `${COLORS.bold}${issue.code}${COLORS.reset} `
      : "";

    // Catalog enrichment — show what this code means
    const catalogEntry = issue.code ? getByCode(issue.code) : undefined;
    const layerTag = catalogEntry
      ? `${COLORS.dim}[${catalogEntry.layerName}]${COLORS.reset} `
      : "";
    const suggestion = catalogEntry?.suggestionTemplate;

    console.log(
      `    ${COLORS.dim}${i + 1}.${COLORS.reset} ${layerTag}${codeStr}${issue.rule} ` +
        `${COLORS.dim}(${issue.count} file${issue.count !== 1 ? "s" : ""})${COLORS.reset}`,
    );

    // Show fix suggestion for each top issue
    if (suggestion) {
      console.log(`       ${COLORS.dim}fix: ${suggestion}${COLORS.reset}`);
    }
  }
}

/**
 * Print catalog coverage — unique error codes triggered across all files.
 * Shows per-layer breakdown: how many codes fired at each detection layer.
 *
 * "Count the cost." — Luke 14:28
 */
function printCatalogCoverage(summaries: LintSummary[]): void {
  // Collect unique error codes across all results
  const codeSet = new Set<string>();
  const layerCodes = new Map<string, Set<string>>();

  for (const s of summaries) {
    for (const r of s.results) {
      if (!r.errorCode) continue;
      codeSet.add(r.errorCode);

      const layer = r.layerName ?? "?";
      let layerSet = layerCodes.get(layer);
      if (!layerSet) {
        layerSet = new Set<string>();
        layerCodes.set(layer, layerSet);
      }
      layerSet.add(r.errorCode);
    }
  }

  if (codeSet.size === 0) return;

  console.log("");
  console.log(
    `  ${COLORS.bold}Catalog:${COLORS.reset} ${codeSet.size} unique codes triggered` +
    ` ${COLORS.dim}(of ${CATALOG_SIZE} in catalog)${COLORS.reset}`,
  );

  // Per-layer code counts (sorted by layer resolution)
  const layerOrder = ["R50", "R25", "R10", "R05", "T00"];
  for (const layer of layerOrder) {
    const codes = layerCodes.get(layer);
    if (!codes) continue;
    const total = LAYER_COUNTS[layer] ?? 0;
    const percent = total > 0 ? Math.round((codes.size / total) * 100) : 0;
    const desc = LAYER_DESCRIPTIONS[layer] ?? layer;
    const barWidth = Math.round((codes.size / Math.max(total, 1)) * 20);
    const bar = "\u2588".repeat(barWidth) + "\u2591".repeat(20 - barWidth);
    console.log(
      `    ${COLORS.dim}${layer}${COLORS.reset} ${bar} ${codes.size}/${total} (${percent}%) ${COLORS.dim}${desc}${COLORS.reset}`,
    );
  }
}

// ---------------------------------------------------------------------------
// Headings
// ---------------------------------------------------------------------------

export function printHeader(
  toolName: string,
  version: string,
  fileCount: number,
  format: string,
): void {
  console.log(
    `${COLORS.bold}${toolName}${COLORS.reset} v${version} — ${format}`,
  );
  console.log(
    `${COLORS.dim}"Prove all things; hold fast that which is good." — 1 Thess 5:21${COLORS.reset}`,
  );
  console.log(
    `\n${COLORS.bold}Linting ${fileCount} file(s)...${COLORS.reset}\n`,
  );
}

// ============================================================================
// CLOSING
// ============================================================================
//
// Output is the face of the tool. Consistent, readable, color-coded.
// "Make it plain upon tables, that he may run that readeth it."
// ============================================================================
