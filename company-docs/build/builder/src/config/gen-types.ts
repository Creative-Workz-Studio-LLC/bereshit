/**
 * CWS Manual Builder — YAML-to-TypeScript Type Generator
 *
 * Reads build.config.yaml and generates TypeScript interfaces that mirror
 * the YAML structure. When the config evolves, regenerate types to stay
 * in sync — single source of truth maintained.
 *
 * Architecture:
 *   build.config.yaml (truth) → gen-types.ts (this) → generated-types.ts (output)
 *   Parallel to: build.config.yaml → gen-config-mk.sh → config.mk
 *
 * "Write the vision, and make it plain upon tables,
 *  that he may run that readeth it." — Habakkuk 2:2
 */

import { readFileSync } from 'fs';
import { parse as parseYaml } from 'yaml';

// =============================================================================
// Setup — Naming Conventions
// =============================================================================

/**
 * Convert a YAML key (snake_case) to PascalCase for TypeScript interface names.
 * Examples: "output_name" → "OutputName", "pdf" → "Pdf"
 */
function toPascalCase(key: string): string {
  return key
    .split(/[_-]/)
    .map((word) => word.charAt(0).toUpperCase() + word.slice(1))
    .join('');
}

/**
 * Singularize a key for array element interface names.
 * Simple heuristic — handles common patterns.
 */
function singularize(key: string): string {
  if (key.endsWith('ies')) return key.slice(0, -3) + 'y';
  if (key.endsWith('ses')) return key.slice(0, -2);
  if (key.endsWith('s') && !key.endsWith('ss')) return key.slice(0, -1);
  return key;
}

/**
 * Build the interface name from a YAML path.
 * Top-level keys get "Config" suffix. Nested keys inherit parent context.
 */
function buildInterfaceName(key: string, parentName?: string): string {
  const pascal = toPascalCase(key);
  if (!parentName) {
    return `${pascal}Config`;
  }
  return `${parentName}${pascal}`;
}

// =============================================================================
// Type Inference Engine
// =============================================================================

/** Collected interface definition */
interface InterfaceDef {
  name: string;
  fields: Array<{
    key: string;
    type: string;
    comment?: string;
  }>;
}

/**
 * Infer the TypeScript type from a YAML value.
 * Returns the type string and may push new interfaces to the collector.
 */
function inferType(
  value: unknown,
  key: string,
  interfaces: InterfaceDef[],
  parentName?: string
): string {
  // Null/undefined
  if (value === null || value === undefined) {
    return 'unknown';
  }

  // Primitives
  if (typeof value === 'string') return 'string';
  if (typeof value === 'number') return 'number';
  if (typeof value === 'boolean') return 'boolean';

  // Arrays
  if (Array.isArray(value)) {
    if (value.length === 0) return 'unknown[]';

    const first = value[0];

    if (typeof first === 'string') return 'string[]';
    if (typeof first === 'number') return 'number[]';
    if (typeof first === 'boolean') return 'boolean[]';

    // Array of objects — generate interface for element
    if (typeof first === 'object' && first !== null) {
      const elementName = buildInterfaceName(singularize(key), parentName);
      generateInterface(first as Record<string, unknown>, elementName, interfaces);
      return `${elementName}[]`;
    }

    return 'unknown[]';
  }

  // Objects
  if (typeof value === 'object') {
    const obj = value as Record<string, unknown>;
    const entries = Object.entries(obj);

    if (entries.length === 0) {
      return 'Record<string, unknown>';
    }

    // Check if this is a Record<string, T> pattern (all values same shape)
    if (isHomogeneousRecord(obj)) {
      const firstEntry = entries[0];
      if (firstEntry) {
        const sampleValue = firstEntry[1];

        if (typeof sampleValue === 'string') return 'Record<string, string>';
        if (typeof sampleValue === 'number') return 'Record<string, number>';
        if (typeof sampleValue === 'boolean') return 'Record<string, boolean>';

        // Record of objects — generate interface for value type
        if (typeof sampleValue === 'object' && sampleValue !== null && !Array.isArray(sampleValue)) {
          const valueName = buildInterfaceName(singularize(key), parentName);
          generateInterface(sampleValue as Record<string, unknown>, valueName, interfaces);
          return `Record<string, ${valueName}>`;
        }
      }
    }

    // Specific object — generate a named interface
    const ifaceName = buildInterfaceName(key, parentName);
    generateInterface(obj, ifaceName, interfaces);
    return ifaceName;
  }

  return 'unknown';
}

/**
 * Check if an object looks like a homogeneous Record<string, T>.
 * Heuristic: all values have the same typeof and structure.
 */
function isHomogeneousRecord(obj: Record<string, unknown>): boolean {
  const entries = Object.entries(obj);
  if (entries.length < 2) return false;

  const firstEntry = entries[0];
  if (!firstEntry) return false;
  const firstType = typeof firstEntry[1];

  // If all primitives of same type → homogeneous
  if (['string', 'number', 'boolean'].includes(firstType)) {
    return entries.every(([, v]) => typeof v === firstType);
  }

  // If all objects → check they have the same keys
  if (firstType === 'object') {
    const firstVal = firstEntry[1];
    if (!firstVal || typeof firstVal !== 'object') return false;
    const firstKeys = Object.keys(firstVal).sort().join(',');
    return entries.every(([, v]) => {
      if (typeof v !== 'object' || v === null || Array.isArray(v)) return false;
      return Object.keys(v).sort().join(',') === firstKeys;
    });
  }

  return false;
}

/**
 * Generate an interface definition from a YAML object.
 */
function generateInterface(
  obj: Record<string, unknown>,
  name: string,
  interfaces: InterfaceDef[]
): void {
  // Avoid duplicates
  if (interfaces.some((i) => i.name === name)) return;

  const fields: InterfaceDef['fields'] = [];

  for (const [key, value] of Object.entries(obj)) {
    const type = inferType(value, key, interfaces, name);
    fields.push({ key, type });
  }

  interfaces.push({ name, fields });
}

// =============================================================================
// Code Generation
// =============================================================================

/**
 * Generate the TypeScript source code from collected interface definitions.
 */
function generateTypeScript(
  interfaces: InterfaceDef[],
  rootName: string,
  rootFields: Array<{ key: string; type: string; optional: boolean }>,
  version: string
): string {
  const lines: string[] = [];

  // File header
  lines.push('/**');
  lines.push(' * AUTO-GENERATED from build.config.yaml — DO NOT EDIT');
  lines.push(' *');
  lines.push(` * Generated from build.config.yaml v${version}`);
  lines.push(` * Generated at: ${new Date().toISOString()}`);
  lines.push(' *');
  lines.push(' * To regenerate: cws-build config --types');
  lines.push(' * To edit types:  Modify build.config.yaml, then regenerate.');
  lines.push(' *');
  lines.push(' * "Write the vision, and make it plain upon tables,');
  lines.push(' *  that he may run that readeth it." — Habakkuk 2:2');
  lines.push(' */');
  lines.push('');

  // Sub-interfaces first (dependencies before dependents)
  for (const iface of interfaces) {
    lines.push(`export interface ${iface.name} {`);
    for (const field of iface.fields) {
      // Use bracket notation for keys that aren't valid identifiers
      const needsBrackets = /[^a-zA-Z0-9_]/.test(field.key) || /^\d/.test(field.key);
      const keyStr = needsBrackets ? `'${field.key}'` : field.key;
      lines.push(`  ${keyStr}: ${field.type};`);
    }
    lines.push('}');
    lines.push('');
  }

  // Root BuildConfig interface
  lines.push(`export interface ${rootName} {`);
  for (const field of rootFields) {
    const optional = field.optional ? '?' : '';
    lines.push(`  ${field.key}${optional}: ${field.type};`);
  }
  lines.push('}');
  lines.push('');

  return lines.join('\n');
}

// =============================================================================
// Public API
// =============================================================================

/**
 * Generate TypeScript type definitions from a YAML config file.
 *
 * @param configPath - Path to build.config.yaml
 * @returns Generated TypeScript source code
 */
export function generateTypesFromYaml(configPath: string): string {
  const yamlContent = readFileSync(configPath, 'utf-8');
  const config = parseYaml(yamlContent) as Record<string, unknown>;
  const version = (config.version as string) || 'unknown';

  const interfaces: InterfaceDef[] = [];
  const rootFields: Array<{ key: string; type: string; optional: boolean }> = [];

  // Known required sections (core builder needs them)
  const requiredSections = new Set(['document', 'attributes', 'formats', 'watch', 'display', 'version']);

  for (const [key, value] of Object.entries(config)) {
    if (key === 'version') {
      rootFields.push({ key, type: 'string', optional: false });
      continue;
    }

    const type = inferType(value, key, interfaces);
    const optional = !requiredSections.has(key);
    rootFields.push({ key, type, optional });
  }

  return generateTypeScript(interfaces, 'GeneratedBuildConfig', rootFields, version);
}
