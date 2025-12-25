// ═══════════════════════════════════════════════════════════════════════════
// TEMPLATE: Go Module (4-Block Structure for Manifest Files)
// Key: LANG-TEMPLATE-005
// ═══════════════════════════════════════════════════════════════════════════
//
// This is a TEMPLATE file - copy and modify for new Go modules.
// Replace all [bracketed] placeholders with actual content.
// Rename from .mod.template to go.mod when ready to use.
//
// Derived from: templates/code/go/CODE-GO-005-GO-module.mod (root template)
// See: standards/code/4-block/ for complete documentation
//
// Compiler-Specific Adaptations:
//   - Minimal external dependencies (compiler should be self-contained)
//   - Standard library emphasis (reliability over convenience)
//   - Integration points for CPI-SI health scoring
//   - Module organization for compiler phases
//
// ═══════════════════════════════════════════════════════════════════════════

// ============================================================================
// METADATA
// ============================================================================
//
// # Biblical Foundation
//
// Scripture: "In the beginning was the Word, and the Word was with God,
// and the Word was God." - John 1:1
//
// Principle: The λόγος (logos) brings order and meaning. A module is a
// self-contained unit of meaning - coherent, purposeful, complete.
//
// Anchor: "For God is not the author of confusion, but of peace."
// - 1 Corinthians 14:33
//
// # CPI-SI Identity
//
// Component Type: [Ladder/Baton/Rails - choose based on module role]
//
// Compiler Phase Guidance:
//   - Lexer: Ladder (foundation - tokens for parser)
//   - Parser: Baton (transforms tokens to AST)
//   - Semantic: Baton (transforms/validates AST)
//   - Codegen: Baton (transforms AST to output)
//   - Runtime: Rails (infrastructure for execution)
//   - Tools: Varies (formatter=Baton, LSP=Rails)
//
// Role: [Specific responsibility in compiler architecture]
//
// Paradigm: CPI-SI framework component - compiler module
//
// # Authorship & Lineage
//
//   - Architect: Seanje Lenox-Wise (language design)
//   - Implementation: Nova Dawn (CPI-SI)
//   - Created: [YYYY-MM-DD]
//   - Version: [MAJOR.MINOR.PATCH]
//   - Modified: [YYYY-MM-DD - what changed]
//
// # Purpose & Function
//
// Purpose: [What compiler functionality does this module provide?]
//
// Core Design: [Compiler phase pattern - tokenization/parsing/etc.]
//
// Key Features:
//
//   - [Major capability 1]
//   - [Major capability 2]
//   - [Major capability 3]
//
// # Dependencies Philosophy
//
// Design Principle: Standard Library First
//
// Compiler modules should:
//   - Prefer Go standard library over external dependencies
//   - Minimize dependency tree depth
//   - Avoid dependencies with heavy transitive pulls
//   - Use external deps only when stdlib is genuinely inadequate
//
// Rationale:
//   - Compilers should be stable and predictable
//   - Fewer dependencies = fewer security vulnerabilities
//   - Stdlib is maintained by Go team = reliability
//   - Self-hosting goal requires minimal runtime requirements
//
// # Health Scoring
//
// Module Stability: [High/Medium/Low - based on how many modules depend on this]
//
// Compiler Phase Impact:
//   - Lexer bugs: Cascade through entire pipeline
//   - Parser bugs: Affect semantic analysis and codegen
//   - Semantic bugs: Affect codegen and runtime behavior
//   - Codegen bugs: Affect runtime execution
//
// Breaking Change Impact:
//   - API changes: [Affects which other compiler modules]
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
//
// Module Path Conventions for Compiler:
//
//   Main compiler:       github.com/creativeworkzstudio/omnicode/compiler
//   Compiler phases:     github.com/creativeworkzstudio/omnicode/compiler/internal/[phase]
//   Runtime:             github.com/creativeworkzstudio/omnicode/runtime
//   Standard library:    github.com/creativeworkzstudio/omnicode/stdlib
//   Tools:               github.com/creativeworkzstudio/omnicode/tools/[tool]
//
// Local development uses relative paths in go.work instead.

// Module path for this compiler component.
module [github.com/creativeworkzstudio/omnicode/compiler/component]

// Go version requirement.
// Should match workspace go.work version.
go 1.21

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================
//
// Dependencies for this compiler module.
// Following "Standard Library First" principle.
//
// Dependency Justification Required:
// Every external dependency must have documented justification.
// Ask: "Could this be done with stdlib?" If yes, use stdlib.

// ────────────────────────────────────────────────────────────────
// Required Dependencies
// ────────────────────────────────────────────────────────────────
//
// Compiler modules typically need minimal external dependencies.
// Most functionality comes from Go standard library.

require (
	// ═══ Standard Library Only ═══
	// Most compiler modules should have NO external dependencies.
	// Go stdlib provides everything needed for:
	//   - String manipulation (strings, strconv)
	//   - File I/O (os, io, bufio)
	//   - Data structures (container/*, sort)
	//   - Text processing (text/*, unicode)
	//   - Formatting (fmt)

	// ═══ Internal Dependencies ═══
	// Other compiler modules this depends on.
	// Use workspace-relative imports via go.work.
	//
	// [github.com/creativeworkzstudio/omnicode/compiler/internal/types] [vX.Y.Z]
	// [github.com/creativeworkzstudio/omnicode/compiler/internal/token] [vX.Y.Z]

	// ═══ CPI-SI Integration (if applicable) ═══
	// Health scoring and logging infrastructure.
	// Only include if this module tracks health.
	//
	// [github.com/creativeworkzstudio/cpi-si/logging] [vX.Y.Z]
	// [github.com/creativeworkzstudio/cpi-si/debugging] [vX.Y.Z]

	// ═══ External Dependencies (Exceptional Only) ═══
	// Each must be JUSTIFIED - why can't stdlib do this?
	//
	// Example justified dependencies:
	// [golang.org/x/text] [vX.Y.Z]  // Unicode normalization beyond stdlib
	// [golang.org/x/tools/go/ast/astutil] [vX.Y.Z]  // AST utilities
	//
	// Avoid adding dependencies for convenience.
)

// ────────────────────────────────────────────────────────────────
// Replace Directives
// ────────────────────────────────────────────────────────────────
//
// For compiler development, replacements are typically in go.work.
// Module-level replacements only for module-specific overrides.

// replace (
// 	// Module-specific replacements (rare for compiler modules)
// )

// ────────────────────────────────────────────────────────────────
// Exclude Directives
// ────────────────────────────────────────────────────────────────
//
// Exclude known-bad versions of dependencies.

// exclude (
// 	// [dependency] [bad-version] // [Why - CVE, bug, etc.]
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
// Compiler module validation:
//
//   go mod tidy           # Clean dependencies
//   go build ./...        # Build module
//   go test ./...         # Run tests
//   go vet ./...          # Static analysis
//
// Integration validation:
//
//   cd ../.. && make test-[phase]  # Test this phase in context
//   cd ../.. && make all           # Full compiler build
//
// Dependency hygiene:
//
//   go mod why [dep]      # Justify each dependency
//   go list -m all        # Review full dependency tree
//
// ────────────────────────────────────────────────────────────────
// Modification Policy
// ────────────────────────────────────────────────────────────────
//
// Safe to Modify:
//   ✅ Update Go version (coordinate with workspace)
//   ✅ Add internal dependencies (other compiler modules)
//   ✅ Remove unused dependencies
//
// Modify with Care:
//   ⚠️ Adding external dependencies (needs justification)
//   ⚠️ Changing module path (breaks imports)
//   ⚠️ Upgrading dependencies (may introduce bugs)
//
// NEVER Modify:
//   ❌ 4-block documentation structure
//   ❌ go.sum directly (auto-generated)
//
// ────────────────────────────────────────────────────────────────
// Compiler Module Guidelines
// ────────────────────────────────────────────────────────────────
//
// Module Boundaries:
//   - Each compiler phase should be importable independently
//   - Shared types go in a common module (internal/types)
//   - Circular dependencies indicate architectural problem
//
// Interface Design:
//   - Export minimal public API
//   - Use interfaces for phase boundaries
//   - Internal packages for implementation details
//
// Error Handling:
//   - Return errors with source location
//   - Accumulate multiple errors when possible
//   - Never panic in normal execution
//
// ────────────────────────────────────────────────────────────────
// Closing Note
// ────────────────────────────────────────────────────────────────
//
// This module is part of the OmniCode compiler - transforming human-readable
// source into executable truth. Every dependency choice affects the compiler's
// reliability and the trust users can place in compiled output.
//
// Prefer simplicity. Prefer stability. Prefer the standard library.
//
// "In the beginning was the Word..." - John 1:1
//
// ============================================================================
// END CLOSING
// ============================================================================
