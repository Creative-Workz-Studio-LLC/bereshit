// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/shared/scope-analysis.ts
// key:     B-tov-cws-struct-lib-shared-scope-analysis
// title:   CWS Struct — R[3] Scope Analysis
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-23
// authors: Nova Dawn (CPI-SI)
// purpose: Function-scope-aware concept detection — the R[3] layer.
//
//          R[5] detects line-by-line patterns (regex). R[3] detects concepts
//          that require knowing function boundaries: recursion (function
//          calling itself) and self_access (Go receiver parameter usage).
//
//          This module parses function declarations, tracks brace depth for
//          body boundaries, and checks for cross-line concept patterns.
//
// biblical_foundation: "Day unto day uttereth speech, and night unto night
//   sheweth knowledge." — Psalm 19:2
//   Each detection layer speaks what it can see. When R[5] falls silent,
//   R[3] picks up the speech.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import type { ConceptDetector } from "../data/concept-detectors.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Types
// ---------------------------------------------------------------------------

/** A parsed function scope with name, optional receiver, and body lines. */
export interface FunctionScope {
  /** Function name (e.g., "has_cycle", "Process"). */
  name: string;

  /** Go receiver parameter name, if present (e.g., "e" from "func (e *Exists)"). */
  receiverParam?: string;

  /** Line index (0-based) within the input where the function declaration starts. */
  declarationLine: number;

  /** Source lines comprising the function body (between opening and closing brace). */
  bodyLines: string[];
}

/** Concepts that benefit from multiline text matching (R[5]+). */
const MULTILINE_CONCEPTS = new Set(["recursion"]);

/** Concepts that require function-scope analysis (R[3]). */
const SCOPE_CONCEPTS = new Set(["self_access"]);

// ---------------------------------------------------------------------------
// Function scope parsing
// ---------------------------------------------------------------------------

// Rust: pub(crate) fn name(  |  fn name(  |  pub fn name(
const RUST_FN_PATTERN = /^\s*(?:pub(?:\(crate\))?\s+)?fn\s+(\w+)\s*\(/;

// Go: func (r *Type) Name(  |  func Name(
const GO_FUNC_PATTERN = /^\s*func\s+(?:\((\w+)\s+[*&]?\w+(?:\.\w+)?\)\s+)?(\w+)\s*\(/;

/**
 * Parse function scopes from source lines.
 *
 * Identifies function declarations, tracks brace depth to determine body
 * boundaries, and extracts body lines for each function.
 *
 * @param lines    Source lines (typically a container/section's lines)
 * @param language "rust" or "go"
 * @returns Array of parsed function scopes
 */
export function parseFunctionScopes(
  lines: string[],
  language: string,
): FunctionScope[] {
  const scopes: FunctionScope[] = [];
  const fnPattern = language === "rust" ? RUST_FN_PATTERN : GO_FUNC_PATTERN;

  let i = 0;
  while (i < lines.length) {
    const line = lines[i]!;
    const trimmed = line.trim();

    // Skip comment-only lines
    if (trimmed.startsWith("//") || trimmed.startsWith("/*")) {
      i++;
      continue;
    }

    const match = fnPattern.exec(trimmed);
    if (!match) {
      i++;
      continue;
    }

    // Extract function name and optional receiver
    let name: string;
    let receiverParam: string | undefined;

    if (language === "go") {
      receiverParam = match[1] || undefined;
      name = match[2]!;
    } else {
      name = match[1]!;
    }

    // Find the opening brace — may be on this line or a subsequent line.
    // Track startI so we can detect when the inner loop fails to advance i
    // (prevents infinite loop on unbalanced or semicolon-terminated declarations).
    const startI = i;
    let braceDepth = 0;
    let bodyStarted = false;
    let searchLine = i;
    const bodyLines: string[] = [];

    // Count braces from the declaration line forward
    while (searchLine < lines.length) {
      const sl = lines[searchLine]!;

      for (let c = 0; c < sl.length; c++) {
        const ch = sl[c];

        // Simple string literal skip — track quoted regions
        if (ch === '"' || ch === '\'') {
          const quote = ch;
          c++;
          while (c < sl.length && sl[c] !== quote) {
            if (sl[c] === '\\') c++; // skip escaped char
            c++;
          }
          continue;
        }

        // Skip // line comments (rest of line is comment)
        if (ch === '/' && c + 1 < sl.length && sl[c + 1] === '/') {
          break;
        }

        // Semicolon before opening brace = trait method signature, extern fn,
        // or forward declaration. Not a function body — skip entirely.
        // This makes the parser sub-subsection-aware: trait blocks contain
        // method signatures (`;` terminated) that are NOT function bodies.
        if (ch === ';' && !bodyStarted) {
          i = searchLine + 1;
          searchLine = lines.length;
          break;
        }

        if (ch === '{') {
          braceDepth++;
          if (!bodyStarted) bodyStarted = true;
        } else if (ch === '}') {
          braceDepth--;
          if (bodyStarted && braceDepth === 0) {
            // Function body complete
            scopes.push({
              name,
              receiverParam,
              declarationLine: startI,
              bodyLines,
            });
            i = searchLine + 1;
            // Use goto-like break by setting searchLine beyond bounds
            searchLine = lines.length;
            break;
          }
        }
      }

      // If we're inside the function body (depth > 0 after opening brace),
      // collect body lines. Skip the declaration line (searchLine === i)
      // to avoid false positives — e.g., `fn add(` matching as a call to `add`.
      if (bodyStarted && braceDepth > 0 && searchLine < lines.length) {
        if (searchLine > startI) {
          bodyLines.push(lines[searchLine]!);
        }
      }

      searchLine++;
    }

    // If we exited without finding closing brace AND i wasn't advanced,
    // skip this declaration to prevent infinite loop.
    if (i === startI) {
      i++;
    }
  }

  return scopes;
}

// ---------------------------------------------------------------------------
// R[3] concept detection
// ---------------------------------------------------------------------------

/**
 * Detect recursion by checking if any function calls itself.
 *
 * For each function scope, checks if `functionName(` appears as a call
 * in the function's body lines (excluding comment lines).
 */
function detectRecursion(scopes: FunctionScope[]): boolean {
  for (const scope of scopes) {
    // Build a regex that matches the function name as a call
    // \b prevents matching substrings (e.g., "has_cycle" won't match "no_has_cycle")
    const callPattern = new RegExp(`\\b${escapeRegex(scope.name)}\\s*\\(`);

    for (const line of scope.bodyLines) {
      const trimmed = line.trim();
      // Skip comments
      if (trimmed.startsWith("//") || trimmed.startsWith("/*")) continue;
      if (callPattern.test(trimmed)) return true;
    }
  }
  return false;
}

/**
 * Detect self_access in Go by checking if the receiver parameter is used.
 *
 * For each function scope with a receiverParam, checks if `param.` appears
 * in the function's body lines (field access or method call on receiver).
 */
function detectSelfAccess(scopes: FunctionScope[]): boolean {
  for (const scope of scopes) {
    if (!scope.receiverParam) continue;

    // Match receiver.field or receiver.Method(
    const accessPattern = new RegExp(
      `\\b${escapeRegex(scope.receiverParam)}\\.\\w+`
    );

    for (const line of scope.bodyLines) {
      const trimmed = line.trim();
      if (trimmed.startsWith("//") || trimmed.startsWith("/*")) continue;
      if (accessPattern.test(trimmed)) return true;
    }
  }
  return false;
}

/** Escape special regex characters in a string. */
function escapeRegex(s: string): string {
  return s.replace(/[.*+?^${}()|[\]\\]/g, "\\$&");
}

// ---------------------------------------------------------------------------
// Public API — R[3] detection fallback
// ---------------------------------------------------------------------------

/**
 * Detect a concept using multiline text matching (R[5]+).
 *
 * Joins container lines into a single string and runs the concept detector
 * against the full text. This enables regex patterns that span multiple lines
 * (e.g., recursion: `fn name(...) { ... name(...) ... }`).
 *
 * @param detector  Concept detector with compiled regex patterns
 * @param lines     Code lines (comments already filtered)
 * @returns true if any pattern matches the joined text
 */
export function detectConceptMultiline(
  detector: ConceptDetector,
  lines: string[],
): boolean {
  if (detector.patterns.length === 0) return false;

  const text = lines.join("\n");
  for (const pattern of detector.patterns) {
    // Reset lastIndex in case regex is stateful (global flag)
    pattern.detect.lastIndex = 0;
    if (pattern.detect.test(text)) return true;
  }
  return false;
}

/**
 * Detect a concept using function-scope analysis (R[3]).
 *
 * Parses function declarations and bodies, then checks for scope-dependent
 * patterns. Currently supports:
 *   - recursion: function calling itself (any language)
 *   - self_access: Go receiver parameter usage
 *
 * @param conceptId      Concept to detect ("recursion" or "self_access")
 * @param containerLines Raw container lines (including comments — scope parser filters)
 * @param language       Source language ("rust" or "go")
 * @returns true if the concept is detected through scope analysis
 */
export function detectConceptByScope(
  conceptId: string,
  containerLines: string[],
  language: string,
): boolean {
  const scopes = parseFunctionScopes(containerLines, language);
  if (scopes.length === 0) return false;

  switch (conceptId) {
    case "recursion":
      return detectRecursion(scopes);
    case "self_access":
      return detectSelfAccess(scopes);
    default:
      return false;
  }
}

/**
 * Check whether a concept benefits from R[3] scope analysis.
 *
 * Returns true if the concept has scope-level detection logic that can
 * detect patterns R[5] regex cannot. Used by validateContainerConcepts
 * to decide whether to try the R[3] fallback.
 */
export function hasR3Detection(conceptId: string, language: string): boolean {
  if (MULTILINE_CONCEPTS.has(conceptId)) return true;
  if (SCOPE_CONCEPTS.has(conceptId) && language === "go") return true;
  // Recursion scope detection works for all languages
  if (conceptId === "recursion") return true;
  return false;
}

// ============================================================================
// CLOSING
// ============================================================================
//
// R[3] scope analysis — the second floor of the detection hierarchy.
//
// R[5] (regex, line-by-line) detects 15/16 concepts in each language.
// R[3] (scope-aware) detects the concepts regex cannot:
//   - recursion: requires correlating fn declaration with call site
//   - self_access (Go): requires knowing which variable is the receiver
//
// Both require the same infrastructure: function-scope tracking.
// Build one, get the other nearly free.
//
// "Day unto day uttereth speech." — Psalm 19:2
// Each layer speaks what it can see.
// ============================================================================
