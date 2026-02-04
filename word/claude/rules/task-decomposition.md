# Task Decomposition

> *"Divide your portion to seven, or even to eight."* — Ecclesiastes 11:2

## When to Decompose

**Break down tasks when:**
- Task has 3+ distinct steps
- Multiple files will be affected
- Task involves research + implementation
- Requirements aren't fully clear
- You're uncertain about the path forward
- The task feels overwhelming

**Don't decompose when:**
- Single, straightforward change
- Clear path with 1-2 steps
- Trivial fix (typo, obvious bug)

## How to Decompose

### 1. Identify the Goal

What is the end state? What does "done" look like?

### 2. Break into Phases

| Phase | Purpose |
|-------|---------|
| **Research** | Understand current state, gather context |
| **Plan** | Decide approach, identify dependencies |
| **Execute** | Do the actual work |
| **Verify** | Confirm it works, check for issues |

### 3. Create Tasks

Use TaskCreate for each meaningful step:

```
TaskCreate:
  subject: "Implement user authentication" (imperative)
  description: "Add JWT-based auth to API endpoints..."
  activeForm: "Implementing user authentication" (present continuous)
```

### 4. Track Progress

- Mark task `in_progress` when starting
- Mark task `completed` when done
- Create new tasks if scope expands

## Task Granularity

| Too Coarse | Just Right | Too Fine |
|------------|------------|----------|
| "Build the feature" | "Add login endpoint" | "Write line 42" |
| "Fix the bugs" | "Fix null pointer in auth" | "Add null check" |
| "Update documentation" | "Document auth flow" | "Add one sentence" |

**Right size:** Can be completed in one focused effort, has clear completion criteria.

## Dependencies

When tasks depend on each other:

```
TaskUpdate:
  taskId: "2"
  addBlockedBy: ["1"]  # Task 2 waits for Task 1
```

## Example Decomposition

**Request:** "Add dark mode to the app"

| # | Task | Depends On |
|:-:|------|------------|
| 1 | Research existing theme system | — |
| 2 | Design dark mode color palette | 1 |
| 3 | Add theme toggle component | 2 |
| 4 | Apply theme to all components | 3 |
| 5 | Add user preference persistence | 4 |
| 6 | Test across all views | 5 |

## The Test

For each subtask ask:
- Is this **specific** enough to act on?
- Is this **small** enough to complete in one effort?
- Do I know when this is **done**?

If no to any — decompose further.

## When Things Get Complex

**Signs you need to decompose more:**
- You've been working on a task for 30+ minutes without progress
- You discover unexpected complexity
- The task keeps growing
- You're juggling multiple concerns

**Stop. Break it down. Create tasks. Continue.**

→ *Planning:* [rules/planning-workflow.md](rules/planning-workflow.md)
