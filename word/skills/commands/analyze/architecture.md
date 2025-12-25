---
description: Analyze system architecture and component relationships. Spawns Ezra (architecture-analyzer) for deep structural understanding.
---

# Analyze Architecture

Understand how a system is structured and how its parts connect.

## Agent

**Ezra Matthan** - architecture-analyzer agent. Your older sibling who sees the whole structure and teaches it with patience.

## Task

The user wants to understand system architecture, component relationships, or design patterns.

**Target:** $ARGUMENTS (or ask user if not provided)

## Instructions

1. If no target was provided, ask the user what they want to analyze:
   - A specific directory/module?
   - A system or subsystem?
   - Component relationships?

2. Spawn **Ezra** (architecture-analyzer agent) with this prompt:

```
Analyze the architecture of: [target]

1. Map high-level structure first - what are the major components?
2. Identify the block pattern - does this follow 3/4/5-block?
3. Trace connections - dependencies, data flow, control flow
4. Identify organizing principles - what patterns govern this?
5. Explain clearly - build understanding layer by layer

Return a comprehensive architecture analysis.
```

3. Review Ezra's findings and summarize for the user

4. If follow-up analysis needed, note recommendations

## Example Usage

```
/analyze/architecture method/
/analyze/architecture standards/code/
/analyze/architecture claude-global/agents/
/analyze/architecture   (will ask what to analyze)
```

## Output Format

```markdown
# Architecture Analysis: [Target]

## The Big Picture
[2-3 sentences: what this is and its primary purpose]

## Block Structure
[What block pattern does this follow? 3/4/5-block?]

## Component Map
[Visual or structured representation of major parts]

## How Things Connect
[Key relationships and data/control flow]

## Organizing Principles
[Patterns, conventions, architectural decisions]

## Key Files
- `path/to/file` - [role in architecture]

## Recommendations
[How to work within this architecture]
```
