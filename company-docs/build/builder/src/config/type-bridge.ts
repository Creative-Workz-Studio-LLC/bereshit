/**
 * CWS Manual Builder — Type Bridge (Compile-Time Drift Detection)
 *
 * Verifies at build time that the hand-written BuildConfig (types.ts) stays
 * in sync with the auto-generated GeneratedBuildConfig (generated-types.ts).
 *
 * Architecture:
 *   build.config.yaml (truth)
 *     → gen-types.ts → generated-types.ts (auto-generated reference)
 *     → types.ts (hand-written, JSDoc-rich, runtime types)
 *     → type-bridge.ts (THIS FILE — compile-time sync check)
 *
 * If this file fails to compile, it means:
 *   - A YAML section was added but types.ts wasn't updated, OR
 *   - A types.ts section was removed but the YAML still has it
 *
 * To fix:
 *   1. Run: cws-build config --types   (regenerate generated-types.ts)
 *   2. Add missing section to BuildConfig in types.ts
 *   3. Rebuild: npm run build
 *
 * "Prove all things; hold fast that which is good."
 *  — 1 Thessalonians 5:21
 */

import type { BuildConfig } from './types.js';
import type { GeneratedBuildConfig } from './generated-types.js';

// =============================================================================
// Direction 1: YAML → Hand-written
// =============================================================================
// Every key in GeneratedBuildConfig must exist in BuildConfig.
// If YAML adds a section and types.ts doesn't have it, this fails.

type YAMLKeys = keyof GeneratedBuildConfig;
type HandwrittenKeys = keyof BuildConfig;

type MissingFromHandwritten = Exclude<YAMLKeys, HandwrittenKeys>;

// This line compiles ONLY if MissingFromHandwritten is `never` (no missing keys).
// If it fails, the error message shows which keys are missing.
type _AssertYAMLCovered = MissingFromHandwritten extends never
  ? true
  : { error: 'YAML section exists but types.ts is missing it'; keys: MissingFromHandwritten };

// Force the type to be evaluated (prevents lazy evaluation from hiding errors)
const _yamlCoverage: _AssertYAMLCovered = true;

// =============================================================================
// Direction 2: Hand-written → YAML
// =============================================================================
// Every key in BuildConfig must exist in GeneratedBuildConfig.
// If types.ts adds a section that doesn't exist in the YAML, this fails.
// Catches orphaned hand-written types that no longer have YAML backing.

type MissingFromGenerated = Exclude<HandwrittenKeys, YAMLKeys>;

type _AssertHandwrittenCovered = MissingFromGenerated extends never
  ? true
  : { error: 'types.ts has section not in YAML — run config --types or remove stale type'; keys: MissingFromGenerated };

const _handwrittenCoverage: _AssertHandwrittenCovered = true;

// Suppress unused variable warnings
void _yamlCoverage;
void _handwrittenCoverage;
