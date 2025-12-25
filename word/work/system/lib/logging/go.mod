// ============================================================================
// METADATA
// ============================================================================
// Logging Library Module - Health tracking and logging infrastructure
//
// Version: 1.0.0 (Orchestrator extraction - 7 files, switchboard pattern)
// Purpose: Provides Rails pattern logging - orthogonal infrastructure that
// all components attach to for health tracking and activity logging.
//
// Dependencies: TOML parser (config.go only - intentional Rails exception)

module system/lib/logging

// ============================================================================
// SETUP
// ============================================================================

go 1.24.4

// ============================================================================
// BODY
// ============================================================================
// No external dependencies - pure stdlib implementation
// Rails pattern: components create own loggers, never pass as parameters

// ============================================================================
// CLOSING
// ============================================================================
// Module Path: system/lib/logging
// Consumers: All components requiring health tracking infrastructure

require github.com/BurntSushi/toml v1.5.0
