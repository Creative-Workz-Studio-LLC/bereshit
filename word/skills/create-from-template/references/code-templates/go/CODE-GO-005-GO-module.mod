// ═══════════════════════════════════════════════════════════════════════════
// TEMPLATE: Go Module (4-Block Structure for Manifest Files)
// Key: CODE-GO-005
// ═══════════════════════════════════════════════════════════════════════════
//
// This is a TEMPLATE file - copy and modify for new Go modules.
// Replace all [bracketed] placeholders with actual content.
// Rename from .mod.template to go.mod when ready to use.
//
// See: standards/code/4-block/ for complete documentation
//
// ═══════════════════════════════════════════════════════════════════════════

// ============================================================================
// METADATA
// ============================================================================
//
// # Biblical Foundation
//
// Scripture: [Relevant verse grounding this module's purpose]
//
// Principle: [Kingdom principle this module demonstrates]
//
// Anchor: [Supporting verse reinforcing the principle]
//
// # CPI-SI Identity
//
// Component Type: [Ladder/Baton/Rails - choose based on module role]
//
//   - Ladder: Foundation module others depend on (libraries, utilities)
//   - Baton: Workflow module passing data through pipeline (processors)
//   - Rails: Infrastructure module providing cross-cutting concerns (logging)
//
// Role: [Specific responsibility in system architecture]
//
// Paradigm: CPI-SI framework component - module definition
//
// # Authorship & Lineage
//
//   - Architect: [Who designed the module structure]
//   - Implementation: [Who created and maintains this module]
//   - Created: [YYYY-MM-DD]
//   - Version: [MAJOR.MINOR.PATCH - match go.mod semantic versioning]
//   - Modified: [YYYY-MM-DD - what changed]
//
// # Purpose & Function
//
// Purpose: [What problem does this module solve?]
//
// Core Design: [Architectural pattern - library, command, hybrid]
//
// Key Features:
//
//   - [Major capability 1]
//   - [Major capability 2]
//   - [Major capability 3]
//
// # Dependencies Philosophy
//
// Design Principle: [Minimal dependencies / Standard library first / etc.]
//
// Why These Dependencies:
//   - Each dependency is justified by [reasoning]
//   - [Standard library preferred because...]
//   - [External dependencies only when...]
//
// # Health Scoring
//
// Module Stability: [How changes to this module affect dependents]
//
// Breaking Change Impact:
//   - API changes: [High/Medium/Low - affects X dependents]
//   - Internal changes: [Usually safe / affects behavior]
//
// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================
//
// Module identity and Go version requirement.
// The module path defines the import path for all packages in this module.
//
// Naming Conventions:
//   - Public modules: github.com/org/repo/path
//   - Internal modules: [project]/internal/path
//   - Local development: Relative paths in go.work

// Module path - the import path for this module.
// This is how other Go code will import packages from this module.
module [module/path]

// Go version requirement.
// This module requires at least this version of Go.
// Sets language features available and dependency resolution behavior.
go 1.21

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================
//
// Dependencies this module requires.
// Organized by category for clarity and maintenance.
//
// Dependency Categories:
//   - Direct: Packages explicitly imported in code
//   - Indirect: Transitive dependencies (// indirect comment)
//
// Extension Point:
// To add a new dependency:
//   1. Import the package in your code
//   2. Run 'go mod tidy' to update go.mod and go.sum
//   3. Document the justification in METADATA if significant
//   4. Update this section's comments if adding a new category

// ────────────────────────────────────────────────────────────────
// Required Dependencies
// ────────────────────────────────────────────────────────────────
// Direct and indirect dependencies for this module.
//
// Format: require [module] [version]
// Version formats:
//   - vX.Y.Z: Semantic version tag
//   - vX.Y.Z-date-commit: Pseudo-version for untagged commits
//   - latest: Resolved to actual version by go mod tidy

require (
	// --- Direct Dependencies ---
	// Packages explicitly imported in this module's code.
	//
	// [github.com/org/package] [vX.Y.Z] // [Justification - why needed]
	// [github.com/org/another] [vX.Y.Z] // [Justification]

	// --- Indirect Dependencies ---
	// Transitive dependencies (dependencies of dependencies).
	// These are managed automatically by 'go mod tidy'.
	//
	// [github.com/org/transitive] [vX.Y.Z] // indirect
)

// ────────────────────────────────────────────────────────────────
// Replace Directives
// ────────────────────────────────────────────────────────────────
// Override dependency resolution for specific modules.
//
// Common Use Cases:
//   - Local development: replace github.com/org/pkg => ../local/pkg
//   - Fork usage: replace github.com/org/pkg => github.com/fork/pkg
//   - Version pinning: replace github.com/org/pkg => github.com/org/pkg vX.Y.Z
//   - Bug workarounds: Temporary fixes before upstream merges
//
// WARNING: Replace directives in go.mod only affect this module.
// For workspace-wide replacements, use go.work instead.
//
// Format: replace [old-module] => [new-module-or-path] [version]

// replace (
// 	// --- Local Development ---
// 	// [github.com/org/shared-lib] => [../shared-lib]
//
// 	// --- Forked Dependencies ---
// 	// [github.com/org/buggy-dep] => [github.com/yourfork/buggy-dep] [vX.Y.Z]
//
// 	// --- Version Pins ---
// 	// [github.com/org/unstable] => [github.com/org/unstable] [vX.Y.Z-specific]
// )

// ────────────────────────────────────────────────────────────────
// Exclude Directives (Rare)
// ────────────────────────────────────────────────────────────────
// Prevent specific versions from being selected.
// Used to avoid known-bad versions or security vulnerabilities.
//
// Format: exclude [module] [version]

// exclude (
// 	// [github.com/org/pkg] [vX.Y.Z] // [Why excluded - e.g., CVE-XXXX]
// )

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// ────────────────────────────────────────────────────────────────
// Module Validation
// ────────────────────────────────────────────────────────────────
//
// Verify module is correctly configured:
//
//   go mod tidy           # Clean up and verify dependencies
//   go mod verify         # Verify dependencies haven't been modified
//   go mod graph          # View dependency graph
//   go build ./...        # Build all packages in module
//   go test ./...         # Test all packages in module
//
// Dependency Analysis:
//
//   go mod why [module]   # Why is this dependency needed?
//   go list -m all        # List all dependencies with versions
//   go list -m -u all     # Check for available updates
//
// Common Issues:
//
//   "missing go.sum entry" - Run 'go mod tidy'
//   "ambiguous import" - Check for conflicting module paths
//   "module declares its path as X" - Module path mismatch
//
// ────────────────────────────────────────────────────────────────
// Modification Policy
// ────────────────────────────────────────────────────────────────
//
// Safe to Modify:
//   ✅ Update Go version (ensure code compatibility)
//   ✅ Add dependencies via 'go get' or 'go mod tidy'
//   ✅ Add replace directives for local development
//   ✅ Add exclude directives for security
//
// Modify with Care:
//   ⚠️ Changing module path - breaks all import statements
//   ⚠️ Downgrading dependencies - may break functionality
//   ⚠️ Removing dependencies - ensure not used anywhere
//
// NEVER Modify:
//   ❌ 4-block documentation structure
//   ❌ go.sum directly (auto-generated, security-critical)
//   ❌ Module path after publishing (breaking change)
//
// ────────────────────────────────────────────────────────────────
// Versioning Guidelines
// ────────────────────────────────────────────────────────────────
//
// Semantic Versioning (SemVer):
//   - MAJOR: Breaking API changes (v2.0.0)
//   - MINOR: New features, backward compatible (v1.1.0)
//   - PATCH: Bug fixes, backward compatible (v1.0.1)
//
// Go Module Versioning:
//   - v0.x.x and v1.x.x: Module path stays as-is
//   - v2.0.0+: Module path must end in /v2, /v3, etc.
//
// Releasing:
//   git tag v1.0.0
//   git push origin v1.0.0
//
// ────────────────────────────────────────────────────────────────
// Integration Notes
// ────────────────────────────────────────────────────────────────
//
// This go.mod file should:
//   - Live at the root of your module (where main.go or package dir is)
//   - Be committed to version control
//   - Be updated via 'go mod tidy' not manual editing
//
// Related files:
//   - go.sum: Checksums for dependencies (auto-generated, commit to VCS)
//   - go.work: Workspace configuration (if in multi-module workspace)
//   - vendor/: Vendored dependencies (if using 'go mod vendor')
//
// ────────────────────────────────────────────────────────────────
// Closing Note
// ────────────────────────────────────────────────────────────────
//
// This module is [describe role - e.g., "the core library for health scoring"].
// Changes to dependencies affect all code importing this module.
//
// Prefer standard library when possible. Each external dependency:
//   - Adds potential security vulnerabilities
//   - Increases build complexity
//   - Creates version management burden
//
// "[Relevant Scripture verse]" - [Reference]
//
// ============================================================================
// END CLOSING
// ============================================================================
