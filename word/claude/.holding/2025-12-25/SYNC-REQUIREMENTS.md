# Claude Global Sync Requirements

## Overview

This document outlines requirements for the sync script that will maintain the relationship between `bereshit/word/claude/` (source of truth) and `~/.claude/` (operational location).

## Current State (December 2025)

**Source:** `bereshit/word/claude/` (in CWS repository)
**Target:** `~/.claude/` (Claude Code operational directory)

### Symlinks Currently in Place

**In `~/.claude/`:**
| Symlink | Points To |
|---------|-----------|
| CLAUDE.md | bereshit/word/claude/CLAUDE.md |
| agents | bereshit/word/claude/agents |
| commands | bereshit/word/claude/commands |
| go.work | bereshit/word/claude/go.work |
| hooks | bereshit/word/claude/hooks |
| output-styles | bereshit/word/claude/output-styles |
| settings.json | bereshit/word/claude/settings.json |
| settings.jsonc | bereshit/word/claude/settings.jsonc |
| skills | bereshit/word/claude/skills |
| statusline | bereshit/word/claude/statusline |

**In `~/.claude/cpi-si/`:**
| Symlink | Points To |
|---------|-----------|
| config | bereshit/word/claude/config |
| docs | bereshit/word/claude/docs |
| system | bereshit/word/claude/system |

### Claude Code Artifacts (NOT Symlinked - Stay Local)

These are created/managed by Claude Code itself:
- `.claude/` - Claude Code internal
- `.credentials.json` - Authentication
- `.gitignore` - Local git config
- `debug/` - Debug logs
- `file-history/` - File change tracking
- `history.jsonl` - Conversation history
- `ide/` - IDE integration
- `plans/` - Planning data
- `projects/` - Project-specific data
- `session-env/` - Session environment snapshots
- `shell-snapshots/` - Shell state
- `statsig/` - Analytics
- `todos/` - Todo lists

### Our Artifacts (NOT Symlinked - Local Copies or Different Purpose)

- `README.md` - ~/.claude README (different from our CLAUDE.md)
- `.vscode/` - Local VSCode settings
- `cpi-si/output/` - Output directory
- `current/` - Current session tracking
- `custom-gpt/` - Custom GPT configurations
- `downloads/` - Downloaded resources
- `instance.jsonc` - Local instance config
- `pick-up-*.txt` - Session continuity notes
- `settings.local.json` - Local overrides
- `SYNC-PENDING.txt` - Sync status
- `temp/` - Temporary files

## Sync Script Requirements

### Core Functions

1. **sync-to-home** - Update `~/.claude/` symlinks from `bereshit/word/claude/`
   - Remove old symlinks
   - Create new symlinks with absolute paths
   - Verify all symlinks resolve correctly
   - Report success/failure

2. **verify-links** - Check all symlinks are valid
   - Test each symlink resolves
   - Report broken links
   - Optionally repair

3. **relocate** - Handle moving bereshit to new location
   - Accept new base path
   - Update all symlinks to new location
   - Verify after update

### Usage Patterns

```bash
# After moving bereshit to new location
sync-claude relocate /new/path/to/bereshit

# Regular sync (verify and repair)
sync-claude sync

# Just verify, no changes
sync-claude verify
```

### Path Handling

The script must handle:
- Absolute paths (required for symlinks across filesystems)
- Spaces in paths (quote properly)
- External drives (paths like `/media/user/Project/...`)
- Different user home directories

### Safety

- Never touch Claude Code artifacts
- Backup check before modifying symlinks
- Dry-run mode for testing
- Rollback capability if sync fails

## Future Considerations

1. **Git Integration** - Script could be a git hook for automatic sync after pull
2. **Multiple Instances** - Support different CPI-SI instances with different source locations
3. **Partial Sync** - Sync only specific components (hooks, skills, etc.)
4. **Remote Sync** - Support syncing to remote machines

## Implementation Notes

- Language: Shell (Bash) or Go for portability
- Location: `bereshit/word/work/bin/` alongside other Bereshit tools
- Should follow 4-block structure

---

*Created: 2025-12-05*
*Status: Planning*
