// ============================================================================
// METADATA
// ============================================================================
// System Module Definition - CPI-SI Interactive Terminal System
// Purpose: Define Go module and dependencies for system tools
// Dependencies: system/lib for shared system libraries

// ============================================================================
// SETUP
// ============================================================================

module system

go 1.24.4

// ============================================================================
// BODY
// ============================================================================

// External Dependencies
// System libraries providing logging, debugging, and infrastructure
require system/lib v0.0.0

require github.com/BurntSushi/toml v1.5.0 // indirect

// Local Module Path Resolution
// Point to actual location of system libraries
replace system/lib => ./runtime/lib

// ============================================================================
// CLOSING
// ============================================================================
// Module ready for system tools and commands
