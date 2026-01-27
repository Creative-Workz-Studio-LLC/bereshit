// ============================================================================
// METADATA - Hooks Module
// ============================================================================
// Purpose: Claude Code hooks for CPI-SI
// Build: make all

module cws.studio/claude/hooks

go 1.24.4

require github.com/creativeworkzstudio/claude-global/pkg v0.0.0

require (
	github.com/BurntSushi/toml v1.5.0 // indirect
	github.com/mattn/go-sqlite3 v1.14.24 // indirect
)

replace github.com/creativeworkzstudio/claude-global/pkg => ../pkg
