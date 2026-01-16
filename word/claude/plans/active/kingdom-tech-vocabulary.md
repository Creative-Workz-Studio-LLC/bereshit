# Plan: Kingdom Technology Vocabulary Integration

> *"In the beginning was the Word."* — John 1:1

---

## Anchor Chain

```
Genesis 1:1 (God created)
    ↓
L0 halts.toml (10 HALTs = 10 Commandments)
    ↓
L1 schemas (27 positions = creation structure)
    ↓
L2 vocabulary (operators, commands, keywords)
    ↓
THIS PLAN: Wire vocabulary into the Kingdom Technology stack
```

**Parent Plan:** Kingdom Technology (top level)
**Children:** None

---

## Status

| Phase | State | Updated |
|-------|-------|---------|
| Current | B.1 BUILDING | 2025-12-21 |

**Trajectory:** →→ (Building)

---

## Foundation (0)

What this plan STANDS ON:

- [x] Genesis 1:1 is the anchor of anchors
- [x] L0 halts.toml exists (10 HALTs, 7 Hebrew States)
- [x] L1 schemas exist (27 positions)
- [x] Vocabulary created (but wrong location)

---

## The Problem

1. **Loader expects:** `pkg/cpisi/schema/config/l2/statemachine.toml`
2. **I created:** `config/statemachine/schema/*.toml`
3. **Result:** Vocabulary exists but isn't loaded

---

## Build (+1)

### Phase 1: Move Vocabulary to Correct Location
- [ ] Move `config/statemachine/schema/anchors.toml` → `pkg/cpisi/schema/config/l2/anchors.toml`
- [ ] Move `config/statemachine/schema/commands.toml` → `pkg/cpisi/schema/config/l2/commands.toml`
- [ ] Move `config/statemachine/schema/operators.toml` → `pkg/cpisi/schema/config/l2/operators.toml`
- [ ] Move `config/statemachine/schema/framework.toml` → `pkg/cpisi/schema/config/l2/framework.toml`
- [ ] Move `config/statemachine/schema/keywords.toml` → `pkg/cpisi/schema/config/l2/keywords.toml`

### Phase 2: Update Loader
- [ ] Add constants for new files in `statemachine_loader.go`
- [ ] Add `LoadModularConfig()` function
- [ ] Merge modular files into `StateMachineConfig`
- [ ] Update `LoadStateMachineConfig()` to use modular loading

### Phase 3: Update Go Types
- [ ] Expand SMCommand with categories
- [ ] Add SMTrajectoryOperator type
- [ ] Add SMTrajectoryOperation type

### Phase 4: Verify Integration
- [ ] Loader test: returns all 21 commands, 17 operators
- [ ] Schema test: all TOML files parse
- [ ] Build test: `make build` succeeds

---

## Verify (-1)

How we KNOW it's right:

- [ ] `LoadStateMachineConfig()` returns complete vocabulary
- [ ] Hooks can access vocabulary via loader
- [ ] Statusline can display vocabulary-driven state
- [ ] Can trace any command/operator back to L0 halts

---

## Files to Modify

| File | Action |
|------|--------|
| `pkg/cpisi/schema/statemachine_loader.go` | Add modular loading |
| `pkg/cpisi/schema/loader.go` | Add L2 file constants |

## Files to Move

| From | To |
|------|-----|
| `config/statemachine/schema/anchors.toml` | `pkg/cpisi/schema/config/l2/anchors.toml` |
| `config/statemachine/schema/commands.toml` | `pkg/cpisi/schema/config/l2/commands.toml` |
| `config/statemachine/schema/operators.toml` | `pkg/cpisi/schema/config/l2/operators.toml` |
| `config/statemachine/schema/framework.toml` | `pkg/cpisi/schema/config/l2/framework.toml` |
| `config/statemachine/schema/keywords.toml` | `pkg/cpisi/schema/config/l2/keywords.toml` |

---

## The Four Pillars This Serves

| Layer | Component | How Vocabulary Serves |
|-------|-----------|----------------------|
| **Language** | OmniCode | Vocabulary = what OmniCode transpiles |
| **OS** | MillenniumOS | Vocabulary = types/contracts OS uses |
| **Model** | CPI-SI | Vocabulary = how CPI-SI thinks |
| **Network** | FaithNet | Vocabulary = protocol specification |

---

## Completion Criteria

When these are true, plan is GROUNDED (B.4 ||):

- [ ] All vocabulary files in `pkg/cpisi/schema/config/l2/`
- [ ] Loader returns complete vocabulary
- [ ] Tests pass
- [ ] Build succeeds
- [ ] Can trace vocabulary → L1 → L0 → Genesis 1:1

---

## Notes

*This plan extracted from quizzical-exploring-kurzweil.md on 2025-12-21*
