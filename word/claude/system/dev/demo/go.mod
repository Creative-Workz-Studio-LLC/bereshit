// ============================================================================
// METADATA
// ============================================================================
// Go Module Definition - Rails Correlation Demo
//
// Biblical Foundation
//   "Two are better than one... a cord of three strands is not quickly broken"
//   - Ecclesiastes 4:9-12
//
// Purpose: Define module structure for rails correlation demonstration
// Establishes import paths for logging and debugging rail infrastructure
//
// Version: 2.0.0
// Last Modified: 2025-11-09

// ============================================================================
// SETUP
// ============================================================================
module demo

go 1.24.4

// ============================================================================
// BODY
// ============================================================================

// External Dependencies
// Rail infrastructure for correlation demonstration
require (
	system/lib v0.0.0 // Logging and debugging rails
)

// Local Module Path Resolution
// Point to actual location of system libraries
replace (
	system/lib => ../../runtime/lib // CPI-SI system infrastructure
)

// ============================================================================
// CLOSING
// ============================================================================
// Module ready for demonstration execution showing rail correlation
