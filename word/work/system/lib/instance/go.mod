// ============================================================================
// METADATA
// ============================================================================
// Instance Library Module - Instance identity provider
//
// Version: 3.0.0 (Orchestrator extraction - 4 primitives, direct access pattern)
// Purpose: Loads instance configuration using two-step dynamic loading
// (root pointer config → full identity config) with graceful degradation.
//
// Dependencies: system/lib/jsonc (JSONC comment stripping)

module system/lib/instance

// ============================================================================
// SETUP
// ============================================================================

go 1.24

require system/lib/jsonc v0.0.0

replace system/lib/jsonc => ../jsonc

// ============================================================================
// BODY
// ============================================================================
// Foundational rung - minimal dependencies (stdlib + jsonc)
// Everything depends ON this, this depends on almost nothing

// ============================================================================
// CLOSING
// ============================================================================
// Module Path: system/lib/instance
// Consumers: hooks/session/*, statusline, all components needing identity
