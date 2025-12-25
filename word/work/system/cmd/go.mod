// ============================================================================
// METADATA - System Commands Module
// ============================================================================
// Purpose: Define Go module for CPI-SI system command-line tools
// Contains: All cmd/* binaries

module cmd

go 1.24.4

require system/lib v0.0.0

require github.com/BurntSushi/toml v1.5.0 // indirect

replace system/lib => ../lib
