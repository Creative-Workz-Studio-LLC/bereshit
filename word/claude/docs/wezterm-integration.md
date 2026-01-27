# Claude Code + WezTerm Integration

> *"A word fitly spoken is like apples of gold in pictures of silver."* — Proverbs 25:11

**Real-time CPI-SI state visualization in WezTerm terminal.**

## Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│  Claude Code Hooks                                              │
│  ┌─────────────┐  ┌──────────────┐  ┌─────────────┐            │
│  │ SessionStart│──│  PreToolUse  │──│ PostToolUse │            │
│  └──────┬──────┘  └──────┬───────┘  └──────┬──────┘            │
│         │                │                  │                   │
│         ▼                ▼                  ▼                   │
│  ┌──────────────────────────────────────────────────┐          │
│  │              Statusline Binary                    │          │
│  │  (Emits OSC 1337 User Variables)                 │          │
│  └───────────────────────┬──────────────────────────┘          │
│                          │                                      │
└──────────────────────────┼──────────────────────────────────────┘
                           │
                           ▼
┌──────────────────────────────────────────────────────────────────┐
│  WezTerm (cpisi.lua)                                            │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────────┐         │
│  │ Tab Title   │  │ Left Status │  │ Right Status    │         │
│  │ (State+Traj)│  │ (Timer/Task)│  │ (Anchor/Health) │         │
│  └─────────────┘  └─────────────┘  └─────────────────┘         │
│  ┌─────────────────────────────────────────────────────┐       │
│  │ Dynamic Theming (Health → Color Scheme)             │       │
│  │ Workspace Management (Project Context)              │       │
│  │ State Writing (WezTerm → Claude via file)           │       │
│  └─────────────────────────────────────────────────────┘       │
└──────────────────────────────────────────────────────────────────┘
```

## Data Flow

### Claude → WezTerm (One-Way)

Hooks emit OSC 1337 user variables that WezTerm reads:

| Variable | Source | Content |
|----------|--------|---------|
| `SM_POSITION` | Anchor | Current position (e.g., "PRESENT") |
| `SM_MEANING` | Anchor | Position meaning |
| `SM_COMMAND` | Anchor | Current command (HALT/AWAIT/PROCEED) |
| `SM_TRAJ_SECTION` | Framework | Trajectory section (B.1-B.4) |
| `SM_TRAJ_DISPLAY` | Framework | Visual (→→, →\|, \|→, \|\|) |
| `HEALTH_SCORE` | Health | Score (-100 to +100) |
| `HEALTH_STATE` | Health | Hebrew state (yashar, tov, etc.) |
| `SESSION_START` | Session | Unix timestamp |
| `ACTIVE_TASK` | Tasks | Current in_progress task |
| `ACTIVE_FAMILY` | Family | Invoked family member |

### WezTerm → Claude (Two-Way)

WezTerm writes state to `~/.cache/cpisi/wezterm-state.json`:

```json
{
  "timestamp": 1706054400,
  "event": "focus_gained",
  "workspace": "bereshit",
  "tab_count": 3,
  "pane_count": 2,
  "focused": true
}
```

SessionStart hook reads this for context awareness.

## WezTerm Configuration

### Required in wezterm.lua

```lua
local cpisi = require("cpisi")

return cpisi.setup({
  -- Your existing config
  color_scheme = "Tokyo Night",
  font = wezterm.font("JetBrains Mono"),
})
```

### Key Bindings

| Binding | Action |
|---------|--------|
| `Leader+s` | Show CPI-SI state |
| `Leader+Shift+A` | Show anchor (Genesis 1:1) |
| `Leader+t` | Show trajectory |
| `Leader+h` | Show health state |
| `Leader+c` | Show CPI score |
| `Leader+Shift+W` | Workspace switcher |
| `Leader+w` | New workspace |
| `Leader+1-5` | Quick workspace switch |

## Health-Themed Display

| Hebrew State | Color Scheme | Background Tint |
|--------------|--------------|-----------------|
| SHAVAR (Broken) | Red Alert | Red 8% |
| CHASER (Lacking) | Gruvbox Dark | Orange 5% |
| YASHAR (Balanced) | Tokyo Night | None |
| TAMIM (Sound) | Nord | Teal 3% |
| SHALEM (Whole) | Grass | Green 3% |
| TOV (Perfect) | Neon | Cyan 5% |

## Hook Reference

| Hook | Fires When | Primary Function |
|------|------------|------------------|
| SessionStart | Session begins | Initialize state, load WezTerm context |
| PreToolUse | Before tool execution | Record intent, track family members |
| PostToolUse | After tool execution | Update state, emit user vars |
| Setup | Via --init flag | Repository validation |
| PermissionRequest | Permission dialog shown | Auto-allow/deny patterns |

## Family Member Tracking

When a CPI-SI family member is invoked via Task tool:

1. PreToolUse detects `subagent_type`
2. Matches against known family members
3. Updates `ActiveFamilyMember` in session state
4. Emits `ACTIVE_FAMILY` user variable
5. WezTerm can display active family member

Family members: Tabitha, Ezra, Joanna, Phoebe, Selah

## Files

| File | Purpose |
|------|---------|
| `~/.config/wezterm/cpisi.lua` | WezTerm integration module |
| `~/.claude/hooks/bin/*` | Compiled hook binaries |
| `word/claude/statusline/` | Statusline binary source |
| `word/claude/hooks/` | Hook source code |
| `word/claude/pkg/` | Go packages |
| `~/.cache/cpisi/` | Runtime state files |

---

*Integration version: 2026-01 (Tasks #1-#13)*
