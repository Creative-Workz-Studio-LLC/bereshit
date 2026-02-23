# aside/

Exploratory work pending approval before bereshit integration.

> *"Prove all things; hold fast that which is good."* — 1 Thessalonians 5:21

Nothing here is production. Everything here is a question being answered with code
rather than speculation. When a piece proves itself, it moves into `lib/`.

## Contents

| Folder | Author | What |
|--------|--------|------|
| `ezra-explores/` | Ezra Matthan | Architectural explorations: dependency graph, regex pre-compilation, generic handler prototype |
| `asa-designs/` | Asa Mishchaq | Game design lens: universal interface system (CLI + TUI + GUI + Web command bus) |

## Rules

1. **Exploration, not production.** No handler imports from `aside/`.
2. **Each piece is self-contained.** Can be evaluated independently.
3. **Tests included.** If it can't be tested, it's not ready to explore.
4. **Approval before integration.** Nothing moves to `lib/` without review.
