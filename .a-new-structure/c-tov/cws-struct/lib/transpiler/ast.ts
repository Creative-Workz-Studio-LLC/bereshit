// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/engine/transpiler-ast.ts
// key:     B-tov-cws-struct-lib-engine-transpiler-ast
// title:   CWS Struct — Universal Concept AST
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-21
// authors: Nova Dawn (CPI-SI)
// purpose: Schema-driven AST for function body translation. The AST knows
//          CONCEPTS, not languages. Source language patterns detect concepts
//          in source code; target language patterns emit concepts in target
//          code. Add a language = add a folder. Add a concept = add a file.
//
//          Architecture: Parse source → concept AST → emit target.
//          The concept IS the map. Languages are just different expressions.
//
// biblical_foundation: "For as the body is one, and hath many members, and
//   all the members of that one body, being many, are one body." — 1 Cor 12:12
//   One concept. Many language members. One body of understanding.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// ---------------------------------------------------------------------------
// 1. Imports
// ---------------------------------------------------------------------------

import type {
  ConceptMappingPair,
  LanguageConcept,
  ResolvedMappings,
} from "./mappings.ts";
import {
  loadConceptMappingsCached,
  convertType,
  convertName,
} from "./mappings.ts";

// ---------------------------------------------------------------------------
// 2. Types — Concept AST
// ---------------------------------------------------------------------------

/**
 * A concept-typed AST node.
 *
 * The node doesn't know what language it came from or where it's going.
 * It knows what CONCEPT it represents and carries the captured data
 * needed to express that concept in any target language.
 */
export interface AstNode {
  /** Universal concept name (e.g., "variable_binding", "method_call"). */
  concept: string;
  /** Pattern ID that matched (e.g., "immutable", "method_call"). */
  patternId: string;
  /** Captured data from source pattern (name → value). */
  data: Record<string, string>;
  /** Child nodes (for nested constructs like match arms, if bodies). */
  children: AstNode[];
  /** Original source line(s) for diagnostics. */
  sourceLine: string;
  /** Indentation level in source. */
  indent: number;
}

/**
 * Result of translating a function body.
 */
export interface BodyTranslation {
  /** Translated lines of target code. */
  lines: string[];
  /** Whether any lines were actually translated (vs all falling through). */
  translated: boolean;
  /** Concepts that were successfully recognized. */
  conceptsFound: string[];
  /** Lines that couldn't be translated (fell through to literal copy). */
  untranslatedLines: number;
  /** Go packages detected in emitted code (e.g., "os", "fmt", "errors"). */
  packagesUsed: string[];
}

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// 1. Source parsing — detect concepts in source lines
// ---------------------------------------------------------------------------

/**
 * Parse source body lines into concept AST nodes.
 *
 * For each line, tries all source language concept patterns. First match
 * by priority wins. Unrecognized lines become "unknown" concept nodes
 * (passed through literally).
 *
 * @param lines - Source code lines (function body, without braces)
 * @param sourceConcepts - All loaded source language concepts
 * @returns Array of concept AST nodes
 */
export function parseSourceBody(
  lines: string[],
  sourceConcepts: Map<string, ConceptMappingPair>,
): AstNode[] {
  const nodes: AstNode[] = [];
  let i = 0;

  while (i < lines.length) {
    const line = lines[i]!;
    const trimmed = line.trim();

    // Skip empty lines and pure comment lines
    if (trimmed === "" || trimmed.startsWith("//")) {
      i++;
      continue;
    }

    // Skip closing braces (handled by parent context)
    if (trimmed === "}" || trimmed === "},") {
      i++;
      continue;
    }

    const indent = line.length - line.trimStart().length;

    // Try to detect a match statement (multi-line construct)
    const matchResult = tryParseMatch(lines, i, sourceConcepts);
    if (matchResult) {
      nodes.push(matchResult.node);
      i = matchResult.nextIndex;
      continue;
    }

    // Try to detect an if statement (multi-line construct)
    const ifResult = tryParseIf(lines, i, sourceConcepts);
    if (ifResult) {
      nodes.push(ifResult.node);
      i = ifResult.nextIndex;
      continue;
    }

    // Try single-line concept detection
    const node = detectSingleLineConcept(trimmed, indent, line, sourceConcepts);
    nodes.push(node);
    i++;
  }

  return nodes;
}

/**
 * Try to detect a single-line concept from all loaded patterns.
 *
 * Iterates concepts ordered by priority. First match wins.
 */
function detectSingleLineConcept(
  trimmed: string,
  indent: number,
  originalLine: string,
  sourceConcepts: Map<string, ConceptMappingPair>,
): AstNode {
  // Try each concept's source patterns
  for (const [_conceptName, pair] of sourceConcepts) {
    const patterns = [...pair.source.patterns].sort(
      (a, b) => (a.priority ?? 99) - (b.priority ?? 99),
    );

    for (const pattern of patterns) {
      if (!pattern.detect) continue;

      try {
        const regex = new RegExp(pattern.detect);
        const match = regex.exec(trimmed);
        if (match) {
          const data: Record<string, string> = {};
          if (pattern.captures) {
            for (const [name, groupIdx] of Object.entries(pattern.captures)) {
              data[name] = (match[groupIdx] ?? "").trim();
            }
          }
          // Carry forward metadata
          if (pattern.mutable !== undefined) data._mutable = String(pattern.mutable);
          if (pattern.explicit !== undefined) data._explicit = String(pattern.explicit);
          if (pattern.void !== undefined) data._void = String(pattern.void);

          return {
            concept: pair.source.concept,
            patternId: pattern.id,
            data,
            children: [],
            sourceLine: originalLine,
            indent,
          };
        }
      } catch {
        // Invalid regex — skip this pattern
      }
    }
  }

  // No concept matched — literal passthrough
  return {
    concept: "unknown",
    patternId: "literal",
    data: { raw: trimmed },
    children: [],
    sourceLine: originalLine,
    indent,
  };
}

/**
 * Try to parse a Rust match statement starting at index i.
 *
 * match expr {
 *     Ok(meta) if meta.is_dir() => Ok(()),
 *     Err(e) if ... => ...,
 *     Err(e) => Err(e),
 * }
 */
function tryParseMatch(
  lines: string[],
  startIdx: number,
  _sourceConcepts: Map<string, ConceptMappingPair>,
): { node: AstNode; nextIndex: number } | null {
  const line = lines[startIdx]!;
  const trimmed = line.trim();

  // Check for "match expr {"
  const matchHeader = /^\s*match\s+(.+)\s*\{/.exec(trimmed);
  if (!matchHeader) return null;

  const scrutinee = matchHeader[1]!.trim();
  const indent = line.length - line.trimStart().length;

  // Collect all lines inside the match braces, joining multi-line arms.
  // An arm is complete only when nesting depth returns to 1 (match-brace level)
  // AND the line ends with a comma. This prevents splitting on commas inside
  // nested function calls like Err(io::Error::new(kind, format!(...), )).
  const rawArms: string[] = [];
  let i = startIdx + 1;
  let braceDepth = 1;
  let currentArm = "";

  while (i < lines.length && braceDepth > 0) {
    const armLine = lines[i]!;
    const armTrimmed = armLine.trim();

    // Track brace/paren depth (string-aware — skips { } inside quotes)
    braceDepth = countNestingDepth(armTrimmed, braceDepth);

    if (braceDepth === 0) break; // closing brace of match

    if (armTrimmed === "" || armTrimmed.startsWith("//")) {
      i++;
      continue;
    }

    // Accumulate multi-line arms
    currentArm += (currentArm ? " " : "") + armTrimmed;

    // Arm complete: back at match-brace level (depth 1) AND trailing comma
    if (braceDepth === 1 && (armTrimmed.endsWith(",") || armTrimmed.endsWith("),"))) {
      rawArms.push(currentArm.replace(/,\s*$/, ""));
      currentArm = "";
    }
    i++;
  }
  // Push any remaining arm
  if (currentArm.trim()) {
    rawArms.push(currentArm.replace(/,\s*$/, ""));
  }

  // Parse each arm into a structured AstNode
  const arms: AstNode[] = [];
  for (const raw of rawArms) {
    const arm = parseMatchArm(raw, indent + 4);
    if (arm) arms.push(arm);
  }

  return {
    node: {
      concept: "conditional_match",
      patternId: "match_expr",
      data: { scrutinee },
      children: arms,
      sourceLine: line,
      indent,
    },
    nextIndex: i + 1,
  };
}

/**
 * Parse a single match arm string into a structured AstNode.
 *
 * Patterns:
 *   Ok(meta) if meta.is_dir() => Ok(())
 *   Ok(_) => Err(io::Error::new(...))
 *   Err(e) if e.kind() == ErrorKind::NotFound => fs::create_dir_all(path)
 *   Err(e) => Err(e)
 *   _ => default_expr
 */
function parseMatchArm(raw: string, indent: number): AstNode | null {
  // Pattern: Result(binding) [if guard] => body
  const armMatch = raw.match(
    /^(Ok|Err|_|[A-Z]\w*)\s*(?:\(([^)]*)\))?\s*(?:if\s+(.+?))?\s*=>\s*(.+)$/,
  );

  if (!armMatch) {
    return {
      concept: "conditional_match",
      patternId: "arm_unknown",
      data: { raw },
      children: [],
      sourceLine: raw,
      indent,
    };
  }

  const [, pattern, binding, guard, body] = armMatch;

  const patternId = guard
    ? "arm_with_guard_and_binding"
    : binding && binding !== "_"
      ? "arm_with_binding"
      : pattern === "_"
        ? "arm_wildcard"
        : "arm_simple";

  return {
    concept: "conditional_match",
    patternId,
    data: {
      pattern: pattern ?? "_",
      binding: binding ?? "",
      guard: guard ?? "",
      body: body ?? "",
    },
    children: [],
    sourceLine: raw,
    indent,
  };
}

/**
 * Try to parse an if/else block starting at index i.
 */
function tryParseIf(
  lines: string[],
  startIdx: number,
  sourceConcepts: Map<string, ConceptMappingPair>,
): { node: AstNode; nextIndex: number } | null {
  const line = lines[startIdx]!;
  const trimmed = line.trim();

  const ifHeader = /^\s*if\s+(.+)\s*\{/.exec(trimmed);
  if (!ifHeader) return null;

  const condition = ifHeader[1]!.trim();
  const indent = line.length - line.trimStart().length;

  // Collect body lines until matching brace
  const thenBody: AstNode[] = [];
  const elseBody: AstNode[] = [];
  let i = startIdx + 1;
  let braceDepth = 1;
  let inElse = false;

  while (i < lines.length && braceDepth > 0) {
    const bodyLine = lines[i]!;
    const bodyTrimmed = bodyLine.trim();

    // Track brace depth
    for (const ch of bodyTrimmed) {
      if (ch === "{") braceDepth++;
      if (ch === "}") braceDepth--;
    }

    if (braceDepth === 0) {
      // Check if this line is "} else {"
      if (/\}\s*else\s*\{/.test(bodyTrimmed)) {
        inElse = true;
        braceDepth = 1;
        i++;
        continue;
      }
      break;
    }

    if (bodyTrimmed !== "" && !bodyTrimmed.startsWith("//")) {
      const bodyNode = detectSingleLineConcept(
        bodyTrimmed,
        bodyLine.length - bodyLine.trimStart().length,
        bodyLine,
        sourceConcepts,
      );
      if (inElse) {
        elseBody.push(bodyNode);
      } else {
        thenBody.push(bodyNode);
      }
    }

    i++;
  }

  const node: AstNode = {
    concept: "conditional_if",
    patternId: elseBody.length > 0 ? "if_else" : "if_only",
    data: { condition },
    children: [...thenBody, ...elseBody],
    sourceLine: line,
    indent,
  };
  // Store boundary between then and else
  if (elseBody.length > 0) {
    node.data._thenCount = String(thenBody.length);
  }

  return { node, nextIndex: i + 1 };
}

// ---------------------------------------------------------------------------
// 2. Target emission — render concept AST as target language code
// ---------------------------------------------------------------------------

/**
 * Emit target language code from concept AST nodes.
 *
 * For each node, finds the matching target concept and applies its emit
 * template. Falls back to literal passthrough for unrecognized concepts.
 *
 * @param nodes - Concept AST nodes from parseSourceBody
 * @param targetConcepts - All loaded target language concepts
 * @param mappings - Structural mappings (for type/name conversion)
 * @param baseIndent - Base indentation string (e.g., "\t")
 * @returns Translated lines of target code
 */
export function emitTargetBody(
  nodes: AstNode[],
  targetConcepts: Map<string, ConceptMappingPair>,
  mappings: ResolvedMappings | null,
  baseIndent: string = "\t",
): string[] {
  const lines: string[] = [];

  for (const node of nodes) {
    const emitted = emitNode(node, targetConcepts, mappings, baseIndent);
    lines.push(...emitted);
  }

  return lines;
}

/**
 * Emit a single AST node as target language code.
 */
function emitNode(
  node: AstNode,
  targetConcepts: Map<string, ConceptMappingPair>,
  mappings: ResolvedMappings | null,
  indent: string,
): string[] {
  // Unknown concept — pass through literally
  if (node.concept === "unknown") {
    return [`${indent}${node.data.raw ?? node.sourceLine.trim()}`];
  }

  // Find the matching concept in target language
  const conceptKey = conceptToKebab(node.concept);
  const pair = targetConcepts.get(conceptKey);
  if (!pair) {
    // No target concept — pass through
    return [`${indent}// TODO: ${node.concept} — ${node.sourceLine.trim()}`];
  }

  // Dispatch by concept type
  switch (node.concept) {
    case "conditional_match":
      return emitMatch(node, targetConcepts, mappings, indent);

    case "conditional_if":
      return emitIf(node, targetConcepts, mappings, indent);

    case "ok_wrap":
      return emitOkWrap(node, pair.target, indent);

    case "err_wrap":
      return emitErrWrap(node, pair.target, mappings, indent);

    case "return_value":
      return emitReturn(node, pair.target, mappings, indent);

    case "method_call":
      return emitMethodCall(node, pair.target, mappings, indent);

    case "function_call":
      return emitFunctionCall(node, pair.target, mappings, indent);

    case "string_format":
      return emitStringFormat(node, pair.target, indent);

    case "variable_binding":
      return emitVariableBinding(node, pair.target, mappings, indent);

    case "error_propagation":
      return emitErrorPropagation(node, pair.target, mappings, indent);

    case "self_access":
      return emitSelfAccess(node, pair.target, indent);

    default:
      return [`${indent}// TODO: ${node.concept} — ${node.sourceLine.trim()}`];
  }
}

// ---------------------------------------------------------------------------
// 3. Concept-specific emitters
// ---------------------------------------------------------------------------

/** Emit a match/switch construct. */
function emitMatch(
  node: AstNode,
  targetConcepts: Map<string, ConceptMappingPair>,
  mappings: ResolvedMappings | null,
  indent: string,
): string[] {
  const scrutinee = node.data.scrutinee ?? "";

  // Check if this is a Result-returning expression (common Rust pattern)
  if (scrutinee.includes("fs::") || scrutinee.includes("::")) {
    return emitResultMatch(node, mappings, indent);
  }

  // Generic switch
  const lines: string[] = [];
  const goScrutinee = translateExpression(scrutinee, mappings);
  lines.push(`${indent}switch ${goScrutinee} {`);
  for (const child of node.children) {
    const childLines = emitNode(child, targetConcepts, mappings, indent + "\t");
    lines.push(...childLines);
  }
  lines.push(`${indent}}`);
  return lines;
}

/**
 * Emit a Rust `match` on `Result<T, E>` as Go error-check pattern.
 *
 * Rust:  match fs::metadata(path) { Ok(meta) if ... => ..., Err(e) => ... }
 * Go:    info, err := os.Stat(path); if err != nil { ... } if info.IsDir() { ... }
 */
function emitResultMatch(
  node: AstNode,
  mappings: ResolvedMappings | null,
  indent: string,
): string[] {
  const lines: string[] = [];
  const scrutinee = node.data.scrutinee ?? "";
  const callExpr = translateExpression(scrutinee, mappings);

  lines.push(`${indent}info, err := ${callExpr}`);
  lines.push(`${indent}if err != nil {`);

  // Partition arms by Ok/Err pattern (filter by data.pattern, not patternId)
  const errArms = node.children.filter((c) => c.data.pattern === "Err");
  const okArms = node.children.filter((c) => c.data.pattern === "Ok");

  // Emit error handling arms
  for (const arm of errArms) {
    const guard = arm.data.guard ?? "";
    const body = arm.data.body ?? "";
    const binding = arm.data.binding ?? "e";
    const goBody = translateMatchArmBody(body, mappings);

    if (guard) {
      // Rust: Err(e) if e.kind() == ErrorKind::NotFound => fs::create_dir_all(path)
      const goGuard = translateGuard(guard, binding, "err", mappings);
      lines.push(`${indent}\tif ${goGuard} {`);
      lines.push(`${indent}\t\treturn ${goBody}`);
      lines.push(`${indent}\t}`);
    } else {
      // Catch-all: Err(e) => Err(e) → return err
      lines.push(`${indent}\treturn ${goBody}`);
    }
  }

  // If no Err arms at all, add default
  if (errArms.length === 0) {
    lines.push(`${indent}\treturn err`);
  }

  lines.push(`${indent}}`);

  // Emit success (Ok) arms
  for (const arm of okArms) {
    const guard = arm.data.guard ?? "";
    const body = arm.data.body ?? "";
    const binding = arm.data.binding ?? "_";
    const goBody = translateMatchArmBody(body, mappings);

    if (guard) {
      // Rust: Ok(meta) if meta.is_dir() => Ok(()) → if info.IsDir() { return nil }
      const goGuard = translateGuard(guard, binding, "info", mappings);
      lines.push(`${indent}if ${goGuard} {`);
      lines.push(`${indent}\treturn ${goBody}`);
      lines.push(`${indent}}`);
    } else {
      // Catch-all Ok arm
      lines.push(`${indent}return ${goBody}`);
    }
  }

  return lines;
}

/** Emit an if/else construct. */
function emitIf(
  node: AstNode,
  targetConcepts: Map<string, ConceptMappingPair>,
  mappings: ResolvedMappings | null,
  indent: string,
): string[] {
  const lines: string[] = [];
  const condition = translateExpression(node.data.condition ?? "", mappings);

  const thenCount = node.data._thenCount ? parseInt(node.data._thenCount) : node.children.length;
  const thenChildren = node.children.slice(0, thenCount);
  const elseChildren = node.children.slice(thenCount);

  lines.push(`${indent}if ${condition} {`);
  for (const child of thenChildren) {
    const childLines = emitNode(child, targetConcepts, mappings, indent + "\t");
    lines.push(...childLines);
  }

  if (elseChildren.length > 0) {
    lines.push(`${indent}} else {`);
    for (const child of elseChildren) {
      const childLines = emitNode(child, targetConcepts, mappings, indent + "\t");
      lines.push(...childLines);
    }
  }

  lines.push(`${indent}}`);
  return lines;
}

/** Emit an Ok wrap → Go success return. */
function emitOkWrap(
  node: AstNode,
  _target: LanguageConcept,
  indent: string,
): string[] {
  const isVoid = node.data._void === "true" || node.patternId === "ok_unit";
  if (isVoid) {
    return [`${indent}return nil`];
  }
  const expr = node.data.expr ?? "";
  return [`${indent}return ${expr}, nil`];
}

/** Emit an Err wrap → Go error return. */
function emitErrWrap(
  node: AstNode,
  _target: LanguageConcept,
  mappings: ResolvedMappings | null,
  indent: string,
): string[] {
  if (node.patternId === "err_new") {
    const kind = node.data.kind ?? "";
    const message = node.data.message ?? "";

    // Translate the error kind
    const goMessage = translateExpression(message, mappings);

    // Rust io::Error::new(ErrorKind::AlreadyExists, msg) → Go fmt.Errorf(msg)
    if (kind.includes("AlreadyExists")) {
      return [`${indent}return fmt.Errorf("already exists: %s", ${goMessage})`];
    }
    return [`${indent}return fmt.Errorf("%s", ${goMessage})`];
  }

  // Err(e) — propagate
  const expr = node.data.expr ?? "err";
  return [`${indent}return ${expr}`];
}

/** Emit a return value. */
function emitReturn(
  node: AstNode,
  _target: LanguageConcept,
  mappings: ResolvedMappings | null,
  indent: string,
): string[] {
  const expr = node.data.expr ?? "";

  // --- Path method expansion ---
  // Rust: path.exists() → Go: _, err := os.Stat(path); return err == nil
  const existsMatch = expr.match(/^(\w+)\.exists\(\)$/);
  if (existsMatch) {
    return [
      `${indent}_, err := os.Stat(${existsMatch[1]})`,
      `${indent}return err == nil`,
    ];
  }

  // Rust: path.is_dir() → Go: stat + check IsDir()
  const isDirMatch = expr.match(/^(\w+)\.is_dir\(\)$/);
  if (isDirMatch) {
    return [
      `${indent}info, err := os.Stat(${isDirMatch[1]})`,
      `${indent}return err == nil && info.IsDir()`,
    ];
  }

  // Rust: path.is_file() → Go: stat + check Mode().IsRegular()
  const isFileMatch = expr.match(/^(\w+)\.is_file\(\)$/);
  if (isFileMatch) {
    return [
      `${indent}info, err := os.Stat(${isFileMatch[1]})`,
      `${indent}return err == nil && info.Mode().IsRegular()`,
    ];
  }

  // --- Iterator chain: .iter().find(|...|).map(|...|) → loop-based lookup ---
  const iterFindMap = expr.match(
    /^(\w+)\.iter\(\)\.find\(\|\((\w+),\s*_\)\|\s*\*\2\s*==\s*(\w+)\)\.map\(\|\(_,\s*(\w+)\)\|\s*\*\4\)$/,
  );
  if (iterFindMap) {
    const [, collection, , keyParam, ] = iterFindMap;
    return [
      `${indent}for _, pair := range ${collection} {`,
      `${indent}\tif pair[0] == ${keyParam} {`,
      `${indent}\t\tv := pair[1]`,
      `${indent}\t\treturn &v`,
      `${indent}\t}`,
      `${indent}}`,
      `${indent}return nil`,
    ];
  }

  const goExpr = translateExpression(expr, mappings);

  // If translateExpression left a TODO comment, return it as-is
  if (goExpr.startsWith("// TODO:")) {
    return [`${indent}${goExpr}`];
  }

  return [`${indent}return ${goExpr}`];
}

/** Emit a method call. */
function emitMethodCall(
  node: AstNode,
  _target: LanguageConcept,
  mappings: ResolvedMappings | null,
  indent: string,
): string[] {
  const receiver = node.data.receiver ?? "";
  const method = node.data.method ?? "";
  const args = node.data.args ?? "";

  // Convert method name
  const goMethod = mappings ? convertName(mappings, method, "public", "function") : method;
  const goReceiver = receiver === "self" ? "r" : receiver;

  return [`${indent}${goReceiver}.${goMethod}(${args})`];
}

/** Emit a function call. */
function emitFunctionCall(
  node: AstNode,
  _target: LanguageConcept,
  mappings: ResolvedMappings | null,
  indent: string,
): string[] {
  const module = node.data.module ?? "";
  const func = node.data.function ?? "";
  const args = node.data.args ?? "";

  const goExpr = translateFunctionCall(module, func, args, mappings);
  return [`${indent}${goExpr}`];
}

/** Emit a string format expression. */
function emitStringFormat(
  node: AstNode,
  _target: LanguageConcept,
  indent: string,
): string[] {
  const template = node.data.template ?? "";
  const args = node.data.args ?? "";

  // Convert {} placeholders to %v / %s
  const goTemplate = template.replace(/\{\}/g, "%v").replace(/\{:[^}]+\}/g, "%v");
  // Convert args: remove .display() calls (Go doesn't need them)
  const goArgs = args.replace(/\.display\(\)/g, "");

  if (args) {
    return [`${indent}fmt.Sprintf("${goTemplate}", ${goArgs})`];
  }
  return [`${indent}"${goTemplate}"`];
}

/** Emit a variable binding. */
function emitVariableBinding(
  node: AstNode,
  _target: LanguageConcept,
  mappings: ResolvedMappings | null,
  indent: string,
): string[] {
  const name = node.data.name ?? "";
  const expr = node.data.expr ?? "";
  const type = node.data.type;

  // Convert name to Go convention
  const goName = mappings ? convertName(mappings, name, "private", "parameter") : name;
  const goExpr = translateExpression(expr, mappings);

  if (type) {
    const goType = mappings ? convertType(mappings, type) : type;
    return [`${indent}var ${goName} ${goType} = ${goExpr}`];
  }
  return [`${indent}${goName} := ${goExpr}`];
}

/** Emit error propagation (Rust ? → Go if err != nil). */
function emitErrorPropagation(
  node: AstNode,
  _target: LanguageConcept,
  mappings: ResolvedMappings | null,
  indent: string,
): string[] {
  const lines: string[] = [];
  const name = node.data.name ?? "result";
  const expr = node.data.expr ?? "";

  const goExpr = translateExpression(expr, mappings);
  const goName = mappings ? convertName(mappings, name, "private", "parameter") : name;

  lines.push(`${indent}${goName}, err := ${goExpr}`);
  lines.push(`${indent}if err != nil {`);
  lines.push(`${indent}\treturn err`);
  lines.push(`${indent}}`);

  return lines;
}

/** Emit self access. */
function emitSelfAccess(
  node: AstNode,
  _target: LanguageConcept,
  indent: string,
): string[] {
  const field = node.data.field ?? node.data.method ?? "";
  const args = node.data.args;

  if (args !== undefined) {
    // Method call on self
    const goMethod = field.charAt(0).toUpperCase() + field.slice(1);
    return [`${indent}r.${goMethod}(${args})`];
  }
  // Field access
  const goField = field.charAt(0).toUpperCase() + field.slice(1);
  return [`${indent}r.${goField}`];
}

// ---------------------------------------------------------------------------
// 4. Expression translation helpers
// ---------------------------------------------------------------------------

/**
 * Translate a Rust expression to Go.
 *
 * This is the workhorse — handles nested concepts within expressions.
 * Uses pattern-based rewriting.
 */
function translateExpression(
  expr: string,
  mappings: ResolvedMappings | null,
): string {
  if (!expr) return expr;

  let result = expr.trim();

  // Ok(()) → nil
  result = result.replace(/Ok\(\(\)\)/g, "nil");

  // Ok(expr) → expr, nil
  result = result.replace(/Ok\(([^)]+)\)/g, "$1, nil");

  // Err(e) → e (when used as expression)
  // Note: full Err wrapping handled by emitErrWrap

  // format!("...", args) → fmt.Sprintf("...", args)
  result = result.replace(
    /format!\("([^"]+)"(?:,\s*(.+))?\)/g,
    (_, template: string, args: string) => {
      const goTemplate = template.replace(/\{\}/g, "%v");
      if (args) {
        const goArgs = args.replace(/\.display\(\)/g, "");
        return `fmt.Sprintf("${goTemplate}", ${goArgs})`;
      }
      return `"${goTemplate}"`;
    },
  );

  // fs::metadata(path) → os.Stat(path)
  result = result.replace(/fs::metadata\(([^)]+)\)/g, "os.Stat($1)");

  // fs::create_dir_all(path) → os.MkdirAll(path, 0755)
  result = result.replace(/fs::create_dir_all\(([^)]+)\)/g, "os.MkdirAll($1, 0755)");

  // io::Error::new(kind, msg) → fmt.Errorf(msg)
  result = result.replace(
    /io::Error::new\(\s*io::ErrorKind::(\w+),\s*(.+)\)/g,
    (_, _kind: string, msg: string) => {
      const goMsg = translateExpression(msg, mappings);
      return `fmt.Errorf("%s", ${goMsg})`;
    },
  );

  // e.kind() == io::ErrorKind::NotFound → errors.Is(err, os.ErrNotExist)
  result = result.replace(
    /(\w+)\.kind\(\)\s*==\s*io::ErrorKind::NotFound/g,
    "errors.Is($1, os.ErrNotExist)",
  );

  result = result.replace(
    /(\w+)\.kind\(\)\s*==\s*io::ErrorKind::AlreadyExists/g,
    "errors.Is($1, os.ErrExist)",
  );

  // .display() → remove (Go prints paths directly)
  result = result.replace(/\.display\(\)/g, "");

  // path.exists() → exists(path) or use helper
  result = result.replace(/(\w+)\.exists\(\)/g, "exists($1)");

  // path.is_dir() → isDir(path) or after stat: info.IsDir()
  result = result.replace(/(\w+)\.is_dir\(\)/g, "$1.IsDir()");

  // path.is_file() → isFile(path) or after stat: info.Mode().IsRegular()
  result = result.replace(/(\w+)\.is_file\(\)/g, "$1.Mode().IsRegular()");

  // &PRAGMA → PRAGMA (Go doesn't have reference syntax for slice access)
  result = result.replace(/^&(\w+)$/g, "$1");

  // PRAGMA.iter().find(|...| ...).map(|...| ...) → helper lookup
  if (result.includes(".iter().find(")) {
    result = `// TODO: translate iterator chain — ${result}`;
  }

  return result;
}

/**
 * Translate a match arm guard expression to Go.
 * Substitutes the Rust binding variable (e.g., `meta`, `e`) with the Go
 * variable name (e.g., `info`, `err`), then translates the expression.
 */
function translateGuard(
  guard: string,
  binding: string,
  goVar: string,
  mappings: ResolvedMappings | null,
): string {
  let result = guard;
  // Replace the Rust binding variable with the Go variable name
  if (binding && binding !== goVar) {
    result = result.replace(new RegExp(`\\b${binding}\\b`, "g"), goVar);
  }
  return translateExpression(result, mappings);
}

/**
 * Translate a match arm body to Go, handling common Rust Result patterns.
 * More reliable than running the full expression through translateExpression
 * because it decomposes Ok/Err/format! patterns structurally.
 */
function translateMatchArmBody(
  body: string,
  mappings: ResolvedMappings | null,
): string {
  const trimmed = body.trim();

  // Ok(()) → nil
  if (trimmed === "Ok(())") return "nil";

  // Err(simple_identifier) → err
  if (/^Err\(\w+\)$/.test(trimmed)) return "err";

  // Err(io::Error::new(kind, format!(...))) → fmt.Errorf(msg_translated)
  // Uses paren-aware extraction because format! is nested inside Err(Error::new(...)).
  if (trimmed.startsWith("Err(io::Error::new(")) {
    const fmtIdx = trimmed.indexOf('format!("');
    if (fmtIdx >= 0) {
      const tplStart = fmtIdx + 'format!("'.length;
      const tplEnd = trimmed.indexOf('"', tplStart);
      if (tplEnd >= 0) {
        const template = trimmed.slice(tplStart, tplEnd).replace(/\{\}/g, "%v");

        // Check for args after the template string
        const afterTemplate = trimmed.slice(tplEnd + 1);
        const commaMatch = afterTemplate.match(/^,\s*/);
        if (commaMatch) {
          // Extract args up to the first unmatched ")" (closing format!)
          const argsStr = afterTemplate.slice(commaMatch[0].length);
          let depth = 0;
          let end = 0;
          for (let j = 0; j < argsStr.length; j++) {
            if (argsStr[j] === "(") depth++;
            else if (argsStr[j] === ")") {
              if (depth === 0) { end = j; break; }
              depth--;
            }
          }
          const args = argsStr.slice(0, end).replace(/\.display\(\)/g, "").trim();
          if (args) return `fmt.Errorf("${template}", ${args})`;
        }
        return `fmt.Errorf("${template}")`;
      }
    }
    return `fmt.Errorf("%v", ${translateExpression(trimmed, mappings)})`;
  }

  // fs::create_dir_all(path) → os.MkdirAll(path, 0755)
  const createDir = trimmed.match(/^fs::create_dir_all\((.+)\)$/);
  if (createDir) return `os.MkdirAll(${createDir[1]!}, 0755)`;

  // Ok(expr) → expr, nil
  if (trimmed.startsWith("Ok(") && trimmed.endsWith(")")) {
    const inner = trimmed.slice(3, -1);
    const goExpr = translateExpression(inner, mappings);
    return `${goExpr}, nil`;
  }

  // Generic fallback
  return translateExpression(trimmed, mappings);
}

/** Translate a Rust module::function(args) call to Go. */
function translateFunctionCall(
  module: string,
  func: string,
  args: string,
  mappings: ResolvedMappings | null,
): string {
  // Known Rust→Go function mappings
  const fullPath = `${module}::${func}`;

  if (fullPath === "fs::metadata") return `os.Stat(${args})`;
  if (fullPath === "fs::create_dir_all") return `os.MkdirAll(${args}, 0755)`;
  if (fullPath === "io::Error::new") return `fmt.Errorf("%s", ${args})`;

  // Generic: convert module path → Go package
  const goFunc = mappings ? convertName(mappings, func, "public", "function") : func;
  if (module) {
    // Use last segment as package name
    const parts = module.split("::");
    const goPkg = parts[parts.length - 1] ?? module;
    return `${goPkg}.${goFunc}(${args})`;
  }
  return `${goFunc}(${args})`;
}

/** Convert concept name from snake_case to kebab-case for schema lookup. */
function conceptToKebab(concept: string): string {
  return concept.replace(/_/g, "-");
}

/**
 * Extract Go package names from emitted code lines.
 *
 * Scans for the `package.Function` pattern that Go uses for standard
 * library calls. Returns deduplicated package names.
 *
 * Known Go standard library packages — extend as more concepts are added.
 */
const GO_STD_PACKAGES = new Set([
  "os", "fmt", "errors", "strings", "strconv", "sort", "sync",
  "io", "bufio", "filepath", "time", "net", "math", "bytes",
  "context", "log", "regexp", "unicode", "encoding",
]);

function extractPackagesFromLines(lines: string[]): string[] {
  const found = new Set<string>();
  // Match package.Identifier patterns (e.g., os.Stat, fmt.Errorf)
  const pkgPattern = /\b([a-z][a-z0-9]*)\.[A-Z]\w*/g;

  for (const line of lines) {
    let match: RegExpExecArray | null;
    while ((match = pkgPattern.exec(line)) !== null) {
      const pkg = match[1]!;
      if (GO_STD_PACKAGES.has(pkg)) {
        found.add(pkg);
      }
    }
  }

  return [...found].sort();
}

/**
 * Count brace/paren nesting depth, skipping characters inside string literals.
 * Handles both `"double"` and `'single'` quoted strings with escape support.
 * Critical for multi-line match arm joining where format strings contain { }.
 */
function countNestingDepth(line: string, initialDepth: number): number {
  let depth = initialDepth;
  let inString = false;
  let stringChar = "";
  for (let j = 0; j < line.length; j++) {
    const ch = line[j]!;
    if (inString) {
      if (ch === "\\" && j + 1 < line.length) { j++; continue; }
      if (ch === stringChar) inString = false;
      continue;
    }
    if (ch === '"' || ch === "'") {
      inString = true;
      stringChar = ch;
      continue;
    }
    if (ch === "(" || ch === "{") depth++;
    else if (ch === ")" || ch === "}") depth--;
  }
  return depth;
}

// ---------------------------------------------------------------------------
// 5. Top-level translation function
// ---------------------------------------------------------------------------

/**
 * Translate a function body from source language to target language.
 *
 * This is the primary entry point. Loads concept mappings, parses source
 * body into concept AST, emits target body from AST.
 *
 * @param bodyLines - Source function body lines (inside braces)
 * @param sourceLanguage - Source language (e.g., "rust")
 * @param targetLanguage - Target language (e.g., "go")
 * @param mappings - Optional structural mappings for type/name conversion
 * @param indent - Base indentation string
 * @returns Translation result
 */
export async function translateBody(
  bodyLines: string[],
  sourceLanguage: string,
  targetLanguage: string,
  mappings: ResolvedMappings | null = null,
  indent: string = "\t",
): Promise<BodyTranslation> {
  // Load concept mappings
  let conceptMappings: Map<string, ConceptMappingPair>;
  try {
    conceptMappings = await loadConceptMappingsCached(sourceLanguage, targetLanguage);
  } catch {
    // Can't load concepts — fall back to untranslated
    return {
      lines: bodyLines.map((l) => `${indent}${l.trim()}`),
      translated: false,
      conceptsFound: [],
      untranslatedLines: bodyLines.length,
      packagesUsed: [],
    };
  }

  if (conceptMappings.size === 0) {
    return {
      lines: bodyLines.map((l) => `${indent}${l.trim()}`),
      translated: false,
      conceptsFound: [],
      untranslatedLines: bodyLines.length,
      packagesUsed: [],
    };
  }

  // Parse source → concept AST
  const nodes = parseSourceBody(bodyLines, conceptMappings);

  // Emit concept AST → target language
  const translatedLines = emitTargetBody(nodes, conceptMappings, mappings, indent);

  // Gather stats
  const conceptsFound = [...new Set(nodes.filter((n) => n.concept !== "unknown").map((n) => n.concept))];
  const untranslatedLines = nodes.filter((n) => n.concept === "unknown").length;

  // Extract packages from emitted Go code
  const packagesUsed = extractPackagesFromLines(translatedLines);

  return {
    lines: translatedLines,
    translated: conceptsFound.length > 0,
    conceptsFound,
    untranslatedLines,
    packagesUsed,
  };
}

// ============================================================================
// CLOSING
// ============================================================================
//
// The Universal Concept AST. The transpiler doesn't know languages — it knows
// CONCEPTS. Parse source → concept nodes → emit target. Each concept is its
// own schema file. Each language is its own folder.
//
// Add a language: add a folder. Add a concept: add a file. No code changes.
//
// "For as the body is one, and hath many members, and all the members of
// that one body, being many, are one body." — 1 Corinthians 12:12
//
// The pentagon holds: schema = template = linter = transpiler mapping = AST concept.
// ============================================================================
