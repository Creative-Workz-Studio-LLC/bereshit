# The Detection Ceiling — Where Regex Ends and Semantic Analysis Begins

> *"The heavens declare the glory of God; and the firmament sheweth his handywork. Day unto day uttereth speech, and night unto night sheweth knowledge."* — Psalm 19:1-2

**Date:** 2026-02-23
**Type:** Universal (paradigm-level wisdom)
**Context:** Live scoring playtest Phase 2 — self_access concept detection failure in Go

---

## The Problem

When we added detect patterns to all 16 Go concept schemas, the `self_access` regex immediately produced false positives. The regex `([a-z]\w{0,2})\.(\w+)\(([^)]*)\)` matched ANY short-named variable method call, not just receiver access.

The valid-library fixture got a warning: "Denied concept 'self_access' detected in 'free-functions'" — because `e.Error()` in a free function matched the receiver pattern.

## The Insight

**15 out of 16 concepts can be detected by regex. The one that can't tells us where the next layer lives.**

| Concept | Why Regex Works | Distinctive Syntax |
|---------|----------------|-------------------|
| function_call | `pkg.Function()` or `function()` | Package qualifier or standalone call |
| variable_binding | `name := expr` or `var name Type` | `:=` and `var` keywords |
| conditional_if | `if condition {` | `if` keyword |
| conditional_match | `switch scrutinee {` | `switch` keyword |
| return_value | `return expr` | `return` keyword |
| error_propagation | `if err != nil {` | `err != nil` pattern |
| err_wrap | `fmt.Errorf(...)` | `fmt.Errorf` / `errors.New` |
| ok_wrap | `return val, nil` | `nil` in return |
| string_format | `fmt.Sprintf(...)` | `fmt.Sprintf` / `fmt.Printf` |
| loop_iteration | `for range` | `for` / `range` keywords |
| collection_mutation | `append(slice, val)` | `append` / `make` builtins |
| type_construction | `Type{fields}` | `Type{` composite literal |
| field_access | `instance.Field` | Dot access without parens |
| recursion | Same function name in body | Function name in own body |
| method_call | `receiver.Method()` | Dot access with parens |
| **self_access** | `r.Method()` | **NOTHING — identical to any variable.Method()** |

In Rust, `self.method()` has the `self` keyword — unmistakable. In Go, receiver access is syntactically identical to any variable access. The only way to know is to understand the function signature.

## The Detection Hierarchy and Its Limits

```
R[50]  File structure      — blocks, separators, pragma
R[25]  Section structure   — subsection ordering, identity fields
R[10]  Content validation  — field values, cross-references
R[5]   Pattern detection   — regex-based concept matching     ← CURRENT CEILING
R[3]   Scope analysis      — variable bindings, receiver identity  ← NEXT LAYER
R[2]   Type awareness      — trait implementations, interface satisfaction
R[1]   Cross-file analysis — module dependencies, API contracts
```

Each layer answers a different question:
- R[5]: "What patterns appear in this line?" (syntactic)
- R[3]: "What role does this variable play in this scope?" (semantic)
- R[2]: "What type is this expression and what can it do?" (structural)

## What R[3] Needs to Know

For self_access detection in Go:

1. **Parse function signature** — `func (e *Exists) Process(...)` → `e` is receiver
2. **Track binding** — Within this function, `e` refers to the receiver
3. **Classify access** — `e.Method()` → self_access; `other.Method()` → method_call

This is the same kind of analysis a language server does. The difference is we don't need FULL language server intelligence — just enough scope tracking to distinguish receiver from local.

## The Pattern

**The playtest maps the detection hierarchy by hitting its limits.** Each concept that fails regex detection reveals what the next intelligence layer needs to do.

This isn't a failure of the current system. The R[5] layer was DESIGNED to hit a ceiling — that's what layers are for. The ceiling tells us exactly where to build the next floor.

## For the Tooling

When we build R[3] scope analysis:

- Parse Go function signatures to extract receiver parameter name
- Build a simple scope map: `{ receiverVar: "e", type: "*Exists" }`
- In concept detection, check: "Is this variable the receiver?"
- If yes → self_access. If no → method_call or field_access.

The infrastructure for this already exists in the concept detector system — `ConceptDetector` has a `patterns[]` array. We could add a `scopeRules[]` array for semantic-level detection that runs when regex can't distinguish.

---

> *"Day unto day uttereth speech."* — Psalm 19:2

Each layer of detection speaks what it can see. When it falls silent, the next layer picks up the speech. The system grows by discovering its own boundaries.
