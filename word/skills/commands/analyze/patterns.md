---
description: Find patterns, conventions, and reusable components across codebase. Spawns Joanna (pattern-finder) for pattern discovery.
---

# Find Patterns

Discover patterns, conventions, and extractable components.

## Agent

**Joanna Elara** - pattern-finder agent. Your sibling who sees connections others miss and helps you work consistently.

## Task

The user wants to understand existing patterns before implementing similar functionality, or find opportunities to extract reusable components.

**Target:** $ARGUMENTS (or ask user if not provided)

## Instructions

1. If no target was provided, ask the user:
   - What area to search for patterns?
   - Looking for a specific type of pattern?
   - Want to find extractable components?

2. Spawn **Joanna** (pattern-finder agent) with this prompt:

```
Find patterns in: [target]

1. Observe the codebase - what conventions are already in use?
2. Spot repetition - what structures appear multiple times?
3. Check block compliance - does this follow 3/4/5-block? That's a pattern.
4. Verify genuineness - is this a real pattern or coincidence?
5. Report helpfully - "Hey, I noticed..." not "This is wrong."

Return a pattern analysis with extractable opportunities.
```

3. Review Joanna's findings and summarize for the user

4. If patterns should be extracted, note recommendations

## Example Usage

```
/analyze/patterns standards/
/analyze/patterns research/building-blocks/
/analyze/patterns claude-global/agents/
/analyze/patterns   (will ask what to analyze)
```

## Output Format

```markdown
# Pattern Analysis: [Target]

## Block Structure Observed
[Does this codebase follow 3/4/5-block? Consistently?]

## Existing Conventions
- [Convention 1] - used in [locations]
- [Convention 2] - used in [locations]

## Patterns Found

| Pattern | Occurrences | Extractable? |
|---------|-------------|--------------|
| [pattern] | [count] | [yes/no/maybe] |

## Key Files Examined
- `path/to/file` - [pattern observed]

## Recommendations
- [What to extract, if anything]
- [How to stay consistent with existing patterns]
```
