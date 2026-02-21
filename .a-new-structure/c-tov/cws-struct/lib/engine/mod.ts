// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/engine/mod.ts
// key:     B-tov-cws-struct-lib-engine-mod
// title:   CWS Struct — Engine Barrel
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-17
// authors: Nova Dawn (CPI-SI)
// purpose: Re-export all engine modules — discovery, output, registry.
//          The runtime machinery that drives format handlers.
//
// ============================================================================

// Discovery
export { discoverFiles, discoverAllFiles, relativePaths } from "./discovery.ts";

// Output
export {
  COLORS,
  severityColor,
  severityIcon,
  printFileSummary,
  printTotals,
  printHeader,
} from "./output.ts";

// Registry
export {
  registerFormat,
  getFormat,
  listFormats,
  listFormatDetails,
  detectFormat,
} from "./registry.ts";

// Fill engine (schema-driven file generation)
export type { FillContext } from "./fill.ts";
export { generateFile, generateFileText } from "./fill.ts";

// ============================================================================
// CLOSING
// ============================================================================
//
// One import path for all engine machinery.
// "Seek, and ye shall find." — Matthew 7:7
// ============================================================================
