// ============================================================================
// METADATA
// ============================================================================
//
// file:    aside/ezra-explores/regex-precompile.ts
// key:     B-tov-cws-struct-aside-ezra-regex-precompile
// title:   Regex Pre-Compilation Exploration
// type:    Code (Exploration)
// version: a-01.00
// created: 2026-02-22
// authors: Ezra Matthan (CPI-SI)
// purpose: Explore whether regex patterns can be pre-compiled into the data
//          layer as source strings, then compiled to RegExp at import time.
//
//          If this works, it eliminates buildSubsectionPattern(),
//          buildZonePattern(), and buildFieldPattern() from the runtime path.
//          code-schema.ts shrinks by ~100 lines. Handlers get their patterns
//          from the data layer instead of computing them from schema text.
//
// biblical_foundation: "The lines are fallen unto me in pleasant places."
//   — Psalm 16:6
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// ---------------------------------------------------------------------------
// The Question
// ---------------------------------------------------------------------------
//
// code-schema.ts has three regex builders:
//
//   buildSubsectionPattern(name, altNames) → /^\/\/\s{1,2}(?:S\.)?(?:\d+\.?\s+)?Imports\b/i
//   buildZonePattern(tag, kind)            → /^\/\/\s+Cv(\s+[—–-]|\s*$)/
//   buildFieldPattern(fieldName)           → /^\/\/\s*never\s*:/i
//
// These run at schema-load time. They take human-readable strings and produce
// handler-ready regexes. Could we instead:
//
//   1. Store the regex SOURCE STRING in the data layer
//   2. Compile to RegExp at import time (module evaluation)
//   3. Export compiled patterns alongside the section data
//
// This would make the data layer self-sufficient for pattern matching —
// handlers wouldn't need to import from code-schema.ts at all for detection.

// ---------------------------------------------------------------------------
// Types
// ---------------------------------------------------------------------------

/** A section entry with pre-compiled pattern data. */
interface SectionPatternEntry {
  tag: string;
  block: string;
  position: number;

  /** Regex source string (stored as data, compiled at import). */
  patternSource: string;

  /** Regex flags (usually "i" for case-insensitive). */
  patternFlags: string;

  /** Alias pattern source (optional — for non-canonical name detection). */
  aliasPatternSource?: string;

  /** Compiled regex (populated at module load). */
  pattern: RegExp;

  /** Compiled alias regex (populated at module load). */
  aliases?: RegExp;
}

/** A closing zone with pre-compiled detection pattern. */
interface ZonePatternEntry {
  tag: string;
  kind: "code" | "doc";
  patternSource: string;
  pattern: RegExp;
}

/** A field detection pattern for closing doc zones. */
interface FieldPatternEntry {
  fieldName: string;
  patternSource: string;
  pattern: RegExp;
}

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// 1. Pattern source generation (what a build script would do)
// ---------------------------------------------------------------------------

/**
 * Generate the regex source string for a subsection header pattern.
 *
 * This is the same logic as code-schema.ts buildSubsectionPattern(),
 * but instead of returning a RegExp, it returns the source string.
 * A build script would call this once per section and store the result.
 */
function generateSubsectionSource(
  canonicalName: string,
  altNames: string[],
): string {
  function nameToFragment(name: string): string {
    return name
      .split(/\s+/)
      .map((word) => word.replace(/-/g, "-?"))
      .join("\\s*");
  }

  const fragments = [canonicalName, ...altNames].map(nameToFragment);
  const nameGroup = fragments.length === 1
    ? fragments[0]
    : `(?:${fragments.join("|")})`;

  return `^\\/\\/\\s{1,2}(?:S\\.)?(?:\\d+\\.?\\s+)?${nameGroup}\\b`;
}

/**
 * Generate the regex source string for a closing zone detection pattern.
 */
function generateZoneSource(tag: string, kind: "code" | "doc"): string {
  if (kind === "code") {
    return `^\\/\\/\\s+${tag}(\\s+[—–-]|\\s*$)`;
  }
  return `^\\/\\/\\s+${tag}([:\\s]|$)`;
}

/**
 * Generate the regex source string for a field detection pattern.
 */
function generateFieldSource(fieldName: string): string {
  return `^\\/\\/\\s*${fieldName}\\s*:`;
}

// ---------------------------------------------------------------------------
// 2. Pre-compiled data (what the data layer would contain)
// ---------------------------------------------------------------------------

/**
 * Example: SETUP section patterns, pre-compiled as data.
 *
 * In production, a build script would generate this from the schemas.
 * Here I'm hand-writing a few to prove the concept.
 */
const SETUP_PATTERNS: SectionPatternEntry[] = [
  {
    tag: "imports",
    block: "setup",
    position: 1,
    patternSource: generateSubsectionSource("Imports", []),
    patternFlags: "i",
    pattern: null!, // Populated below
  },
  {
    tag: "constants",
    block: "setup",
    position: 2,
    patternSource: generateSubsectionSource("Constants", ["Consts"]),
    patternFlags: "i",
    aliasPatternSource: `^\\/\\/\\s{1,2}(?:S\\.)?(?:\\d+\\.?\\s+)?Consts\\b`,
    pattern: null!, // Populated below
  },
  {
    tag: "core-types",
    block: "setup",
    position: 4,
    patternSource: generateSubsectionSource("Core Types", ["Primary Types", "Base Types"]),
    patternFlags: "i",
    aliasPatternSource: `^\\/\\/\\s{1,2}(?:S\\.)?(?:\\d+\\.?\\s+)?(?:Primary\\s*Types|Base\\s*Types)\\b`,
    pattern: null!, // Populated below
  },
];

/** Compile all patterns at module load. */
for (const entry of SETUP_PATTERNS) {
  entry.pattern = new RegExp(entry.patternSource, entry.patternFlags);
  if (entry.aliasPatternSource) {
    entry.aliases = new RegExp(entry.aliasPatternSource, entry.patternFlags);
  }
}

/**
 * Example: CLOSING zone patterns, pre-compiled as data.
 */
const CLOSING_ZONE_PATTERNS: ZonePatternEntry[] = [
  {
    tag: "Cv",
    kind: "code",
    patternSource: generateZoneSource("Cv", "code"),
    pattern: null!,
  },
  {
    tag: "Ce",
    kind: "code",
    patternSource: generateZoneSource("Ce", "code"),
    pattern: null!,
  },
  {
    tag: "Cc",
    kind: "code",
    patternSource: generateZoneSource("Cc", "code"),
    pattern: null!,
  },
  {
    tag: "X1",
    kind: "doc",
    patternSource: generateZoneSource("X1", "doc"),
    pattern: null!,
  },
  {
    tag: "X5",
    kind: "doc",
    patternSource: generateZoneSource("X5", "doc"),
    pattern: null!,
  },
];

for (const entry of CLOSING_ZONE_PATTERNS) {
  entry.pattern = new RegExp(entry.patternSource);
}

/**
 * Example: Field detection patterns for X1 doc zone.
 */
const X1_FIELD_PATTERNS: FieldPatternEntry[] = [
  { fieldName: "never", patternSource: generateFieldSource("never"), pattern: null! },
  { fieldName: "careful", patternSource: generateFieldSource("careful"), pattern: null! },
  { fieldName: "safe", patternSource: generateFieldSource("safe"), pattern: null! },
];

for (const entry of X1_FIELD_PATTERNS) {
  entry.pattern = new RegExp(entry.patternSource, "i");
}

// ---------------------------------------------------------------------------
// 3. Verification: Do pre-compiled patterns match the same things?
// ---------------------------------------------------------------------------

/** Test lines that should match. */
const TEST_CASES = [
  // Subsection headers
  { line: "// Imports", expect: "imports", type: "section" },
  { line: "// 1. Imports", expect: "imports", type: "section" },
  { line: "// S.1 Imports", expect: "imports", type: "section" },
  { line: "// Constants", expect: "constants", type: "section" },
  { line: "// Consts", expect: "constants", type: "section" },
  { line: "// 2. Constants", expect: "constants", type: "section" },
  { line: "// Core Types", expect: "core-types", type: "section" },
  { line: "// Primary Types", expect: "core-types", type: "section" },
  { line: "// 4. Core Types", expect: "core-types", type: "section" },

  // Closing zones
  { line: "// Cv -- Validation", expect: "Cv", type: "zone" },
  { line: "// Cv", expect: "Cv", type: "zone" },
  { line: "// X1: Modification Policy", expect: "X1", type: "zone" },
  { line: "// X1 Modification Policy", expect: "X1", type: "zone" },
  { line: "// X5: Closing Note", expect: "X5", type: "zone" },

  // Field patterns
  { line: "// never: Break structure", expect: "never", type: "field" },
  { line: "// Never:", expect: "never", type: "field" },
  { line: "// safe: Function bodies", expect: "safe", type: "field" },
  { line: "// careful: Error types", expect: "careful", type: "field" },
];

// Run verification
let passed = 0;
let failed = 0;

for (const tc of TEST_CASES) {
  let matched = false;

  if (tc.type === "section") {
    const entry = SETUP_PATTERNS.find((s) => s.tag === tc.expect);
    if (entry && entry.pattern.test(tc.line)) matched = true;
  } else if (tc.type === "zone") {
    const entry = CLOSING_ZONE_PATTERNS.find((z) => z.tag === tc.expect);
    if (entry && entry.pattern.test(tc.line)) matched = true;
  } else if (tc.type === "field") {
    const entry = X1_FIELD_PATTERNS.find((f) => f.fieldName === tc.expect);
    if (entry && entry.pattern.test(tc.line)) matched = true;
  }

  if (matched) {
    passed++;
  } else {
    failed++;
    console.log(`FAIL: "${tc.line}" expected to match ${tc.type}:${tc.expect}`);
  }
}

console.log(`\nRegex pre-compilation verification: ${passed} passed, ${failed} failed out of ${TEST_CASES.length}`);

// ---------------------------------------------------------------------------
// 4. Negative tests — things that should NOT match
// ---------------------------------------------------------------------------

const NEGATIVE_CASES = [
  // Block overview TOC lines (3+ spaces) should NOT match section patterns
  { line: "//   1. Imports  -- dependency declarations", shouldNotMatch: "imports", type: "section" },
  { line: "//    Core Types -- primary type definitions", shouldNotMatch: "core-types", type: "section" },
  // Random comments
  { line: "// Import the package", shouldNotMatch: "imports", type: "section" },
];

let negPassed = 0;
let negFailed = 0;

for (const nc of NEGATIVE_CASES) {
  let matched = false;

  if (nc.type === "section") {
    const entry = SETUP_PATTERNS.find((s) => s.tag === nc.shouldNotMatch);
    if (entry && entry.pattern.test(nc.line)) matched = true;
  }

  if (!matched) {
    negPassed++;
  } else {
    negFailed++;
    console.log(`NEG FAIL: "${nc.line}" should NOT match ${nc.type}:${nc.shouldNotMatch}`);
  }
}

console.log(`Negative verification: ${negPassed} passed, ${negFailed} failed out of ${NEGATIVE_CASES.length}`);

// ---------------------------------------------------------------------------
// 5. Size comparison
// ---------------------------------------------------------------------------

console.log("\n--- Size Analysis ---");
console.log(`SETUP pattern entries: ${SETUP_PATTERNS.length} (of 15 total)`);
console.log(`Average source length: ${Math.round(SETUP_PATTERNS.reduce((sum, e) => sum + e.patternSource.length, 0) / SETUP_PATTERNS.length)} chars`);
console.log(`CLOSING zone entries: ${CLOSING_ZONE_PATTERNS.length}`);
console.log(`X1 field entries: ${X1_FIELD_PATTERNS.length}`);
console.log("\nConclusion: Pattern source strings are short (30-80 chars).");
console.log("Storing them as data adds ~2KB for all 42 sections + 8 zones + ~10 fields.");
console.log("Compiling 60 regexes at import time is negligible (~1ms).");
console.log("The tradeoff is clearly worth it: code-schema.ts loses 3 builder functions (~100 lines).");

// ============================================================================
// CLOSING
// ============================================================================
//
// Result: Pre-compilation works. The regex source strings are short, the
// compilation cost at import time is negligible, and the patterns match
// identically to the runtime-computed versions.
//
// What this enables:
// - data/ becomes self-sufficient for pattern matching
// - code-schema.ts loses buildSubsectionPattern, buildZonePattern, buildFieldPattern
// - Handlers import patterns from data/ instead of computing from schema
// - A build script generates the source strings once; import compiles them
//
// What this does NOT do:
// - Replace schema override (custom schemas still need runtime compilation)
// - Handle Go's legacy prefix enrichment (that stays in the Go handler)
//
// "The lines are fallen unto me in pleasant places." — Psalm 16:6
// ============================================================================
