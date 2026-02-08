/**
 * CWS Manual Builder — Witness Report Serializer
 *
 * JSON serialization for the WitnessReport. Handles edge cases
 * like Map→Object conversion and deterministic key ordering.
 *
 * "Write the vision, and make it plain upon tables,
 *  that he may run that readeth it." — Habakkuk 2:2
 */

import type { WitnessReport } from './types.js';

// =============================================================================
// Serialization
// =============================================================================

/**
 * Serialize a WitnessReport to JSON string.
 *
 * Handles:
 *   - Map → Object conversion (for any Map instances in domain results)
 *   - 2-space indentation for human readability
 *   - Deterministic output for diff-friendly reports
 */
export function serializeWitnessReport(report: WitnessReport): string {
  return JSON.stringify(report, mapReplacer, 2);
}

/**
 * JSON.stringify replacer that converts Map instances to plain objects.
 *
 * The TOC domain's PageParseResult may contain Maps internally.
 * This ensures clean serialization without [object Map] artifacts.
 */
function mapReplacer(_key: string, value: unknown): unknown {
  if (value instanceof Map) {
    const obj: Record<string, unknown> = {};
    for (const [k, v] of value) {
      obj[String(k)] = v;
    }
    return obj;
  }
  return value;
}

// =============================================================================
// File Output
// =============================================================================

/**
 * Write a WitnessReport to a file.
 *
 * @param report The report to serialize
 * @param filePath Absolute path to write to
 */
export async function writeWitnessReport(
  report: WitnessReport,
  filePath: string,
): Promise<void> {
  const { writeFile } = await import('node:fs/promises');
  const json = serializeWitnessReport(report);
  await writeFile(filePath, json + '\n', 'utf-8');
}
