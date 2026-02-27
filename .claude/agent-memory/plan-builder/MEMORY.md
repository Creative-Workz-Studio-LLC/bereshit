# Caleb Navon - Plan Builder Memory

## Go SDK Architecture (pkg/)
- **Module:** `cws.studio/pkg` at `word/claude/pkg/`
- **Layers:** foundation/ (L0) -> core/ (L1) -> orchestration/ (L2) -> util/ (grouped by concern)
- **Hooks module:** `cws.studio/claude/hooks` at `word/claude/hooks/`, requires pkg via replace directive
- **Workspace:** `word/claude/go.work` ties everything together
- **hookoutput consumers:** 8 files in hooks/ (session/start, stop, setup, subagent_start, subagent_stop; tool/pre-use, post-use; prompt/submit)
- **hookoutput has ZERO consumers inside pkg/** — only hooks module imports it

## cws-struct Organizational Principles
- **Location:** `.a-new-structure/c-tov/cws-struct/lib/`
- **Key layers:** foundation/ (contracts), data/ (compiled truth), engine/ (orchestration), handlers/ (format-specific), shared/ (cross-cutting), commands/ (CLI), transpiler/, api/, types/
- **Pure parse + I/O wrapper pattern** everywhere
- **Foundation README:** Comprehensive API reference written by Deborah Saphar (2026-02-22)
- **Self-registering handlers:** tool grows by addition

## Planning Patterns Learned
- Always count affected files with grep, not estimation
- Verify go.work workspace resolves cross-module deps before planning import moves
- Phase ordering: fix in-place -> split -> move -> update consumers -> verify
- Never move files and update imports in the same phase — verify between each
