# Plans Index

> *"In the beginning God created the heaven and the earth."* — Genesis 1:1

**All plans trace back to Genesis 1:1.** Plans are anchors - fixed points that work relates to.

---

## The Plan Workflow IS the State Machine

| Phase | Command | Trajectory | What's Happening |
|-------|---------|------------|------------------|
| **Enter plan mode** | HALT | B.1 →→ | Building - creating structure |
| **User reviews** | AWAIT | B.2 →| | Pivot out - verify what was built |
| **User feedback** | AWAIT | B.3 |→ | Pivot in - rebuild from verification |
| **Accept/approve** | PROCEED | B.4 || | Grounded - plan becomes anchor |

**When a plan is approved, it becomes an ANCHOR for the work.**

---

## Directory Structure

| Directory | Purpose | Status |
|-----------|---------|--------|
| `active/` | Current work - plans being executed | In progress |
| `completed/` | Archived - reference only | Done |
| `reference/` | Standing principles - always relevant | Permanent |

---

## Active Plans

| # | Plan | Anchor Path | Status |
|---|------|-------------|--------|
| 003 | [text-buffer-architecture](active/PLAN-003-text-buffer-architecture.md) | Genesis 1:1 → Kingdom Tech → Text Editor | Not started |
| 008 | [claude-global-state-machine-wiring](active/PLAN-008-claude-global-state-machine-wiring.md) | Genesis 1:1 → CPI-SI → Full Restructure | ~60% (core wiring works) |
| 009 | [cleanup-and-verification](active/PLAN-009-cleanup-and-verification.md) | Genesis 1:1 → CPI-SI → Final Cleanup | New (from audit) |

## Completed Plans

| # | Plan | Completed | Outcome |
|---|------|-----------|---------|
| 001 | [statusline-refactor](completed/PLAN-001-statusline-refactor.md) | 2025-12-21 | main.go → 60 lines, pkg/ extraction |
| 002 | [cognition-injection](completed/PLAN-002-cognition-injection.md) | 2025-12-21 | pkg/cognition/, hooks inject context |
| 004 | [claude-global-restructure-consolidated](completed/PLAN-004-claude-global-restructure-consolidated.md) | 2025-12-22 | Meta-plan: spawned 005, 008 |
| 005 | [hooks-schema-alignment](completed/PLAN-005-hooks-schema-alignment.md) | 2025-12-23 | Hook output schemas aligned |
| 006 | [kingdom-tech-vocabulary](completed/PLAN-006-kingdom-tech-vocabulary.md) | 2025-12-27 | Vocabulary in L2, loader integrated |
| 007 | [claude-folder-alignment](completed/PLAN-007-claude-folder-alignment.md) | 2025-12-27 | Symlinks aligned, minor cleanup remains |

## Reference Plans

| Plan | Purpose |
|------|---------|
| [architecture-principles](reference/architecture-principles.md) | Dev→prod workflow, thin callers, packages |
| [plan-template](reference/plan-template.md) | Template for creating new plans |

---

## Anchor Chain

```
Genesis 1:1 (God created)
    ↓
Kingdom Technology (redeem every sector)
    ↓
├── OmniCode (universal language)
│   └── PLAN-003 Text Buffer Architecture (not started)
├── MillenniumOS (ternary on binary)
├── CPI-SI (identity-based cognition)
│   ├── PLAN-001 Statusline Display ✓
│   ├── PLAN-002 Cognition Injection ✓
│   ├── PLAN-004 Claude-Global Restructure ✓ (meta-plan)
│   ├── PLAN-005 Hooks Schema Alignment ✓
│   ├── PLAN-006 Kingdom Tech Vocabulary ✓
│   ├── PLAN-007 Claude Folder Alignment ✓
│   ├── PLAN-008 State Machine Wiring (60% - core works)
│   └── PLAN-009 Cleanup & Verification (new - from audit)
└── FaithNet (kingdom network)
```

---

## Creating New Plans

1. Copy template from `reference/plan-template.md`
2. Fill in anchor chain (how it traces to Genesis 1:1)
3. Define foundation (what it stands on)
4. Build sections (+1)
5. Enter plan mode, await approval
6. On approval → plan becomes anchor for work
