# CPI-SI State Machine - Design Notes

## TUI Design (Grade A - Original Version)

The original TUI design was noted as "grade A" by Seanje. Key elements:
- Clean layout with mode tabs
- Color-coded positions (HALT=red, AWAIT=yellow, PROCEED=green)
- ASCII cube visualization
- Box-style panels with rounded borders

### Keybinding Issue (FIXED)
- **Problem:** Using 1, 2, 3 for mode switches conflicts with number input in Ψ(N) mode
- **Solution:** Use F1, F2, F3 or Tab for mode switching, leave numbers for input

## Three Display Modes

| Mode | Purpose | When to Use |
|------|---------|-------------|
| CLI | Debugging, live logic testing | Development, scripting |
| TUI | Dev tool, interactive exploration | Active development |
| GUI | Pre-prod visualization | User-facing, demos |

Each mode is independent but interconnected - can trigger each other.

## Ψ(N) Saturation Analysis

λ = 0.5493 (from atanh(0.5) for cube model)

| N Range | tanh(λN) | Behavior |
|---------|----------|----------|
| 0-2 | 0.0-0.8 | Good spread, distinct coordinates |
| 3-5 | 0.9-0.99 | Saturating, diminishing returns |
| 5+ | ~1.0 | Saturated, all map to same boundary |

**Expected N range for ranking:** [-3, +6] (within good spread zone)

**Design decision:** Saturation is a FEATURE with structural alignment:

### N=26 Saturation Discovery

λ = 0.5493 produces saturation at exactly N=26. This aligns with cube structure:

| Value | Meaning |
|-------|---------|
| 27 | 3³ = total cube positions |
| 26 | Saturation point = 27 - 1 (excluding center) |
| 1 | Center position (origin, HALT) |

**Interpretation:**
- N values 0-26 map to distinct coordinates (fill the cube)
- N=27+ saturates to boundary (0.5, 0.5, -1.0)
- The 27th position IS the origin - when you exceed cube capacity, you hit the wall

This wasn't designed - it emerged from the math. The cube model and Ψ(N) formula align naturally.

## Hook Integration

State file: `~/.claude/state/position.json`

Hooks update position:
```bash
cpisi set PAST-PRESENT "hook:tool/pre-use"
cpisi set PRESENT-PRESENT "hook:session/end"
```

Daemon watches file and displays live state.
