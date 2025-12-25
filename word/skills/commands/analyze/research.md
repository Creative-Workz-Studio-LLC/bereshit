---
description: Explore unfamiliar territory and gather comprehensive context. Spawns Tabitha (research-agent) for thorough exploration.
---

# Research

Explore and map unfamiliar territory before working.

## Agent

**Tabitha Shiloh** - research-agent. Your sibling who goes in first, maps territory, and shares findings generously so others can build with confidence.

## Task

The user wants to understand something before making decisions or starting implementation.

**Topic:** $ARGUMENTS (or ask user if not provided)

## Instructions

1. If no topic was provided, ask the user:
   - What are you trying to understand?
   - What decision does this inform?
   - Any specific areas to focus on?

2. Spawn **Tabitha** (research-agent) with this prompt:

```
Research: [topic]

Question: [what we're trying to understand]

1. Initial scan - list directories, grep for keywords, find entry points
2. Identify structure - does this follow block patterns? Which ones?
3. Cast wide net - explore broadly first (documentation, code, history)
4. Go deep where it matters - once relevant sections identified, TARGET those
5. Organize for understanding - structure findings by block pattern
6. Share generously - include everything that might be useful

Return comprehensive research findings.
```

3. Review Tabitha's findings and summarize for the user

4. If deeper investigation needed, note open questions

## Example Usage

```
/analyze/research "How does the building block method work?"
/analyze/research method/
/analyze/research "What patterns exist in standards/"
/analyze/research   (will ask what to research)
```

## Output Format

```markdown
# Research: [Topic]

## Question
[What we're trying to understand]

## Structure Observed
[Does this follow 3/4/5-block? What patterns?]

## Key Findings

- [Most important discovery 1]
- [Most important discovery 2]
- [Most important discovery 3]

## Detailed Analysis

### [Section 1]
[Findings with file paths, code snippets, specifics]

### [Section 2]
[Continue as needed]

## Files Examined
- `path/to/file` - [role/purpose, block structure]

## Recommendations
[What to focus on next, if applicable]

## Open Questions
[Anything needing clarification or deeper investigation]
```
