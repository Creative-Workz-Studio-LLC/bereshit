// ============================================================================
// METADATA
// ============================================================================
// Go Module Definition - Shared Library Configuration
// Provides common functionality for hooks, statusline, and future components
// Lower rung in the ladder - consumed by orchestrators above

// ============================================================================
// SETUP
// ============================================================================
module system/lib

go 1.24.4

// ============================================================================
// BODY
// ============================================================================
// Dependencies would be listed here if we had external dependencies
// Currently using only Go standard library

// ============================================================================
// CLOSING
// ============================================================================
// Module is ready for use by all Claude Code components

require github.com/BurntSushi/toml v1.5.0
