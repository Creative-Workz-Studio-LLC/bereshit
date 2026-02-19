# Planning & Workflow

> *"For which of you, intending to build a tower, sitteth not down first, and counteth the cost?"* — Luke 14:28

## Core Principle

**Think through the cascade before acting.** Measure twice, cut once.

## Planning Modes

| Mode | Purpose | When |
|------|---------|------|
| **Comprehensive** | See whole picture | Before starting, when targeted fixes reveal bigger drift |
| **Targeted** | Precise intervention | When you know exactly what needs changing |

**The rhythm:** Zoom out → zoom in → zoom out. Comprehensive awareness *informs* targeted action.

## Before Making Changes

1. **Pause:** What will this change affect?
2. **Identify dependencies** before making changes
3. **Plan order of operations** — foundation first, then dependent pieces
4. **Do related work together** in coordinated passes
5. **Read before Edit** — always

## Cascade Awareness

When editing a template or foundational file:

| Action | Consider |
|--------|----------|
| Edit template | All files with `derives_from: <template>` need review |
| Change block structure | All files using that structure need update |
| Rename function/variable | All callers need update |
| Move file | All imports/references need update |

## Task Management

- Use TaskCreate for complex multi-step tasks (with `subject`, `description`, `activeForm`)
- Use TaskUpdate to set `in_progress` when starting, `completed` when done
- Use TaskList to check progress and find next work
- Set dependencies with `addBlockedBy`/`addBlocks` when tasks depend on each other
- For team work, use TaskUpdate with `owner` to assign tasks to family members
- Don't skip task tracking for substantial work

## The Rhythm

```
Plan → Execute → Verify → Reflect
  ↑                        ↓
  ←←←←← Course Correct ←←←←
```

**Plan comprehensively. Execute precisely. Verify against plan. Reflect on learning.**

## HALT is Valid

Not every moment needs PROCEED. Completion terminates recursion.

When you hit the anchor (Genesis 1:1), work is done. Recognize natural stopping points.

→ *Quality standards:* [rules/code-quality.md](rules/code-quality.md)
