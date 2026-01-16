# claude-global Architecture

> **This is the DEVELOPMENT stage.** Code is developed here, then synced to bereshit for production.

## Directory Purpose

```
claude-global/                    ← DEVELOPMENT (you are here)
    ├── hooks/                    ← Hook source code (develop here)
    ├── statusline/               ← Statusline source (develop here)
    ├── pkg/                      ← Shared packages (develop here)
    ├── bin/                      ← Built binaries (local testing)
    └── skills/                   ← Skill source code

bereshit/word/claude/             ← PRODUCTION (symlinks point here)
    ├── hooks/                    ← Synced from claude-global
    ├── statusline/               ← Synced from claude-global
    └── ...                       ← ~/.claude/ symlinks to here

~/.claude/                        ← RUNTIME (symlinks to bereshit)
    ├── hooks -> bereshit/word/claude/hooks
    ├── settings.json -> bereshit/word/claude/settings.json
    └── ...
```

## Workflow

1. **Develop** in `claude-global/`
2. **Test** locally with `make build`
3. **Sync** to `bereshit/word/claude/` when ready
4. **Rebuild** binaries in bereshit location
5. Symlinks automatically pick up changes

## Key Packages

| Package | Purpose |
|---------|---------|
| `pkg/statemachine/` | State machine + runtime state |
| `pkg/health/` | Health scoring system |
| `pkg/display/` | Terminal colors + command display |
| `pkg/wezterm/` | WezTerm OSC integration |
| `hooks/` | Claude Code hooks |
| `statusline/` | Statusline display |

## State Machine Flow

```
SessionStart hook
    ↓ initializes
RuntimeState (config/statemachine/runtime/state.jsonc)
    ↓ read by
Pre/Post hooks (update state)
    ↓ read by
Statusline (displays state)
    ↓ emits to
WezTerm (terminal visualization)
```

## DO NOT CONFUSE

- **claude-global** = where you EDIT code
- **bereshit/word/claude** = where symlinks POINT
- Always develop in claude-global, sync to bereshit
