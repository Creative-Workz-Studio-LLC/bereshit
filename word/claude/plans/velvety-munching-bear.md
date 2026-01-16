# Plan: Comprehensive Claude Folder Alignment

## Problem Statement

Three Claude folders need proper alignment with clear roles:

| Layer | Location | Role |
|-------|----------|------|
| **DEV** | `claude-global/` | Source code, builds, Makefile |
| **PROD** | `bereshit/word/claude/` | Final products, customizations |
| **OPS** | `~/.claude/` | Mirror of PROD via symlinks |

**Current Issues:**
1. `pkg/` symlink in `~/.claude/` points to `claude-global/` (DEV) instead of `bereshit/word/claude/` (PROD)
2. Nested `~/.claude/.claude/` directory exists (shouldn't)
3. 257MB+ of debug data in `~/.claude/debug/` (managed by Claude Code, but bloated)
4. Type changes in git showing file mode changes

## Org Chart: Claude Code Directory Structure

### What Claude Code Manages (DO NOT SYMLINK)

These directories are auto-managed by Claude Code itself:

| Directory | Purpose | Action |
|-----------|---------|--------|
| `debug/` | CLI debug logs | Leave as-is, clean periodically |
| `downloads/` | Downloaded files | Leave as-is |
| `file-history/` | File change tracking | Leave as-is |
| `projects/` | Project metadata | Leave as-is |
| `session-env/` | Session environment | Leave as-is |
| `shell-snapshots/` | Shell state | Leave as-is |
| `statsig/` | Feature flags | Leave as-is |
| `todos/` | Task tracking | Leave as-is |
| `ide/` | IDE integration | Leave as-is |
| `logs/` | Claude Code logs | Leave as-is |
| `plans/` | Plan files | Leave as-is |
| `generated/` | Generated content | Leave as-is |
| `prompts/` | Prompt history | Leave as-is |
| `cache/` | Caches | Leave as-is |

### What Users Customize (SYMLINK to PROD)

| Item | Purpose | Symlink Target |
|------|---------|----------------|
| `CLAUDE.md` | Global identity | → `bereshit/word/claude/CLAUDE.md` |
| `settings.json` | Configuration | → `bereshit/word/claude/settings.json` |
| `settings.jsonc` | Configuration (comments) | → `bereshit/word/claude/settings.jsonc` |
| `agents/` | CPI-SI agents | → `bereshit/word/claude/agents/` |
| `commands/` | Custom commands | → `bereshit/word/claude/commands/` |
| `hooks/` | Session/tool hooks | → `bereshit/word/claude/hooks/` |
| `skills/` | CPI-SI skills | → `bereshit/word/claude/skills/` |
| `output-styles/` | Output formatting | → `bereshit/word/claude/output-styles/` |
| `config/` | Runtime config | → `bereshit/word/claude/config/` |
| `data/` | Database & logs | → `bereshit/word/claude/data/` |
| `statusline/` | Statusline binary | → `bereshit/word/claude/statusline/` |
| `rules/` | Rules files | → `bereshit/word/claude/rules/` |
| `system/` | System utilities | → `bereshit/word/claude/system/` |

### What Should NOT Exist in OPS (~/.claude/)

| Item | Issue | Action |
|------|-------|--------|
| `pkg/` | Points to DEV | Remove symlink |
| `.claude/` (nested) | Malformed | Remove |
| `cpi-si/` | Old structure | Move to holding |

## Current State Comparison

### ~/.claude/ (OPS) Current Symlinks — VERIFIED

```
CLAUDE.md -> bereshit/word/claude/CLAUDE.md ✓ CORRECT
Makefile -> bereshit/word/claude/Makefile ✓ CORRECT
agents -> bereshit/word/claude/agents ✓ CORRECT
bin -> bereshit/word/claude/bin ✓ CORRECT
commands -> bereshit/word/claude/commands ✓ CORRECT
config -> bereshit/word/claude/config ✓ CORRECT
docs -> bereshit/word/claude/docs ✓ CORRECT
go.work -> bereshit/word/claude/go.work ✓ CORRECT
hooks -> bereshit/word/claude/hooks ✓ CORRECT
instance.jsonc -> bereshit/word/claude/instance.jsonc ✓ CORRECT
output-styles -> bereshit/word/claude/output-styles ✓ CORRECT
plans -> bereshit/word/claude/plans ✓ CORRECT
rules -> bereshit/word/claude/rules ✓ CORRECT
settings.json -> bereshit/word/claude/settings.json ✓ CORRECT
settings.jsonc -> bereshit/word/claude/settings.jsonc ✓ CORRECT
pkg -> claude-global/pkg ✗ WRONG (points to DEV)
```

### Real Directories in ~/.claude/ (Not Symlinks)

| Directory | Status | Action |
|-----------|--------|--------|
| `.claude/` | Nested (BAD) | Remove |
| `.git/` | OPS git tracking | Leave |
| `cpi-si/` | Old structure | Move to holding |
| `current/` | Unknown | Investigate |
| `custom-gpt/` | User data | Leave |
| `debug/` | Claude Code managed | Leave |
| `downloads/` | Claude Code managed | Leave |
| `file-history/` | Claude Code managed | Leave |
| `ide/` | Claude Code managed | Leave |
| `plugins/` | Claude Code managed | Leave |
| `projects/` | Claude Code managed | Leave |
| `session/` | CPI-SI session data | Should symlink |
| `session-env/` | Claude Code managed | Leave |

### Missing Symlinks in ~/.claude/

| Item | Exists in PROD | Action |
|------|----------------|--------|
| `skills/` | ✓ Yes | Add symlink |
| `statusline/` | ✓ Yes | Add symlink |
| `data/` | ✓ Yes | Add symlink |
| `system/` | ✓ Yes | Add symlink |

## Implementation Steps

### Phase 1: Create Holding Directory

```bash
mkdir -p /media/seanje-lenox-wise/Project/CreativeWorkzStudio_LLC/bereshit/word/claude/.holding/2024-12-24
```

### Phase 2: Clean Up OPS (~/.claude/)

1. **Remove wrong pkg symlink:**
   ```bash
   rm ~/.claude/pkg  # Points to DEV, not needed in OPS
   ```

2. **Remove nested .claude:**
   ```bash
   rm -rf ~/.claude/.claude
   ```

3. **Move old directories to holding:**
   ```bash
   # Move cpi-si (old structure)
   mv ~/.claude/cpi-si /media/seanje-lenox-wise/Project/CreativeWorkzStudio_LLC/bereshit/word/claude/.holding/2024-12-24/

   # Move current (investigate later)
   mv ~/.claude/current /media/seanje-lenox-wise/Project/CreativeWorkzStudio_LLC/bereshit/word/claude/.holding/2024-12-24/

   # Move session (before symlinking)
   mv ~/.claude/session /media/seanje-lenox-wise/Project/CreativeWorkzStudio_LLC/bereshit/word/claude/.holding/2024-12-24/
   ```

### Phase 3: Add Missing Symlinks

```bash
PROD="/media/seanje-lenox-wise/Project/CreativeWorkzStudio_LLC/bereshit/word/claude"

# Skills (CPI-SI skills)
ln -sf $PROD/skills ~/.claude/skills

# Statusline binary
ln -sf $PROD/statusline ~/.claude/statusline

# Data directory (database, logs)
ln -sf $PROD/data ~/.claude/data

# System utilities
ln -sf $PROD/system ~/.claude/system
```

### Phase 4: Verify All Symlinks

```bash
# List all symlinks and verify targets
ls -la ~/.claude/ | grep "^l" | while read line; do
  echo "$line"
done
```

Expected output — ALL symlinks point to `bereshit/word/claude/`:
- No symlinks pointing to `claude-global/`

### Phase 5: Verify settings.json Paths

`~/.claude/settings.json` is a symlink to PROD, so paths should use `~/.claude/`:

```json
{
  "env": {
    "CPI_SI_SESSION_DIR": "/home/seanje-lenox-wise/.claude/session",
    "CPI_SI_CONFIG_DIR": "/home/seanje-lenox-wise/.claude/config",
    ...
  }
}
```

Since OPS symlinks point to PROD, all paths resolve correctly.

### Phase 6: Optional Cleanup

1. **Check debug size:**
   ```bash
   du -sh ~/.claude/debug
   ```

2. **If > 100MB, clean old files:**
   ```bash
   find ~/.claude/debug -type f -mtime +7 -delete
   ```

## Final State

### ~/.claude/ (OPS) After Alignment

```
# Claude Code Managed (real directories - DO NOT TOUCH)
.git/               # OPS git tracking
debug/              # CLI debug logs
downloads/          # Downloaded files
file-history/       # File change tracking
ide/                # IDE integration
plugins/            # Plugins
projects/           # Project metadata
session-env/        # Session environment
custom-gpt/         # User data

# CPI-SI Customized (symlinks to PROD)
CLAUDE.md      -> bereshit/word/claude/CLAUDE.md
Makefile       -> bereshit/word/claude/Makefile
agents/        -> bereshit/word/claude/agents/
bin/           -> bereshit/word/claude/bin/
commands/      -> bereshit/word/claude/commands/
config/        -> bereshit/word/claude/config/
data/          -> bereshit/word/claude/data/          # NEW
docs/          -> bereshit/word/claude/docs/
go.work        -> bereshit/word/claude/go.work
hooks/         -> bereshit/word/claude/hooks/
instance.jsonc -> bereshit/word/claude/instance.jsonc
output-styles/ -> bereshit/word/claude/output-styles/
plans/         -> bereshit/word/claude/plans/
rules/         -> bereshit/word/claude/rules/
settings.json  -> bereshit/word/claude/settings.json
settings.jsonc -> bereshit/word/claude/settings.jsonc
skills/        -> bereshit/word/claude/skills/        # NEW
statusline/    -> bereshit/word/claude/statusline/    # NEW
system/        -> bereshit/word/claude/system/        # NEW
```

### bereshit/word/claude/ (PROD) Contents

```
CLAUDE.md           # Identity document
Makefile            # Build commands
agents/             # CPI-SI agents
bin/                # Compiled binaries
commands/           # Custom commands
config/             # Runtime config (statemachine, health)
data/               # Database (cpisi.db), logs
docs/               # Documentation
go.work             # Go workspace
hooks/              # Compiled hook binaries
instance.jsonc      # Instance configuration
output-styles/      # Output formatting
pkg/                # Go packages (NOT symlinked to OPS)
plans/              # Planning files
rules/              # Modular rules
settings.json       # Settings
settings.jsonc      # Settings (with comments)
skills/             # CPI-SI skills
statusline/         # Statusline binary
system/             # System utilities
.holding/           # Archived old data
```

### claude-global/ (DEV) - Development Only

```
pkg/                # Source Go packages
hooks/              # Hook source code
skills/             # Skill source
Makefile            # Build & deploy to PROD
config/             # Dev config (stale after deploy)
data/               # Dev data (stale after deploy)
```

## Verification Commands

```bash
# Verify ALL symlinks point to PROD (none to claude-global)
ls -la ~/.claude/ | grep "^l" | grep -v "bereshit/word/claude"
# Should return NOTHING

# Verify state updates work
cat ~/.claude/config/statemachine/runtime/state.jsonc | jq .session.hooks_fired

# Verify database accessible
sqlite3 ~/.claude/data/cpisi.db "SELECT COUNT(*) FROM choices;"
```

## Risk Assessment

| Risk | Mitigation |
|------|------------|
| Breaking existing symlinks | Only touch incorrect ones (pkg) |
| Losing data | Move to .holding, don't delete |
| Session data loss | Move session/ to holding, can restore if needed |

## Summary

**Remove:**
- `~/.claude/pkg` (wrong symlink to DEV)
- `~/.claude/.claude` (nested, malformed)

**Move to holding:**
- `~/.claude/cpi-si/` (old structure)
- `~/.claude/current/` (unknown, investigate later)
- `~/.claude/session/` (before symlinking)

**Add symlinks:**
- `skills/` → `bereshit/word/claude/skills/`
- `statusline/` → `bereshit/word/claude/statusline/`
- `data/` → `bereshit/word/claude/data/`
- `system/` → `bereshit/word/claude/system/`

**Leave alone:**
- All Claude Code managed directories (debug/, projects/, etc.)
- All correct symlinks (15 already correct)
