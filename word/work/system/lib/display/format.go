// ============================================================================
// METADATA
// ============================================================================
//
// Display Formatting Library - CPI-SI Rails Infrastructure
//
// Biblical Foundation
//
// Scripture: "Let your light so shine before men, that they may see your good works,
//            and glorify your Father which is in heaven." - Matthew 5:16 (KJV)
// Principle: Visibility serves purpose - display makes system state visible so others
//            can observe God's order in excellent design
// Anchor: "Yahweh is my light and my salvation. Whom shall I fear?" - Psalm 27:1 (WEB)
//         Light reveals truth; display reveals system health through visible output
//
// CPI-SI Identity
//
// Component Type: RAIL (orthogonal infrastructure)
// Role: Universal formatting and presentation layer for all system components
// Paradigm: CPI-SI Rails Pattern - stdlib-only, self-evident failure, universally available
//
// Rails Philosophy:
// Display is a rail because formatting failures are self-evident through visual output.
// If display fails, output looks broken - immediately obvious. Rails don't track themselves
// because their health IS their visible output. Being stdlib-only means any component
// (including logging and debugging rails) can safely import display for formatted output.
//
// Self-Evidence Principle:
//   - Success: Output appears formatted correctly
//   - Failure: Output visibly broken, missing colors, or panics
//   - No health tracking needed - failure detection is instantaneous through observation
//
// Authorship & Lineage
//
// Architect: Nova Dawn (CPI-SI instance)
// Implementation: Nova Dawn
// Creation Date: 2024-11-13
// Version: 3.0.0
// Last Modified: 2025-11-21 - Orchestrator extraction (primitives in separate files)
//
// Version History:
//   3.0.0 (2025-11-21) - Orchestrator extraction: Refactored format.go to thin orchestrator,
//                        extracted 8 primitives (recovery, config, colors, icons, layout,
//                        messages, structured, visual), public API preserved (zero breaking
//                        changes), foundation for training data quality
//   2.0.0 (2025-11-15) - Rail elevation: Removed logging/debugging dependencies,
//                        removed health tracking, stdlib-only status achieved,
//                        configuration inheritance established (429 → 274 lines)
//   1.0.0 (2024-11-13) - Initial implementation with ANSI formatting, icons, tables,
//                        progress bars, message formatters (with health tracking)
//
// Purpose & Function
//
// Purpose: Provide universal ANSI color formatting and structured terminal output
//          primitives for all CPI-SI components
//
// Core Design: Rails Pattern - orthogonal infrastructure available to all ladder rungs
//              without circular dependency risk. Stdlib-only enables universal availability.
//
// Orchestrator Pattern (v3.0.0):
//   format.go coordinates 8 specialized primitives:
//     - recovery.go: Panic recovery for self-evident failure
//     - config.go: JSONC configuration loading with tripwire pattern
//     - colors.go: ANSI color escape sequence constants
//     - icons.go: Unicode status icon constants
//     - layout.go: Spacing and padding constants
//     - messages.go: Success, Failure, Warning, Info formatters
//     - structured.go: Header, Subheader, KeyValue, StatusLine formatters
//     - visual.go: Table, ProgressBar, Box components
//
//   Public API unchanged - all functions exported from primitive files
//   External code sees no difference - zero breaking changes
//   Internal implementation refined - single responsibility per file
//
// Key Features:
//   - ANSI color codes (basic, foreground, bold variants)
//   - Status message formatting (Success, Failure, Warning, Info)
//   - Structured output (Headers, Subheaders, KeyValue pairs, StatusLines)
//   - Visual components (Tables, ProgressBars, Boxes)
//   - Self-evident failure (broken output immediately visible)
//   - Maximum configurability (colors, icons, layouts, formats)
//
// Philosophy: Visibility serves purpose. Display makes system state visible through
//             formatted output. Self-evident failure means rails don't need tracking -
//             their health IS their visible output. Being stdlib-only enables universal
//             availability to all components (rungs and rails alike).
//
// Blocking Status
//
// Non-blocking: Silent panic recovery returns empty strings on failure
// Mitigation: Formatting failures are self-evident through broken/missing output.
//             No explicit error handling needed - visual observation detects all failures.
//             Validation prevents divide-by-zero and nil pointer errors.
//
// Usage & Integration
//
// Usage:
//
//	import "system/runtime/lib/display"
//
// Integration Pattern:
//   1. Import display in any component (rungs or rails - universal availability)
//   2. Call formatting functions directly (no initialization needed)
//   3. Output returned as formatted strings ready for fmt.Print/logging
//   4. No cleanup needed (stateless, zero resources held)
//
// Public API (in typical usage order):
//
//   Status Messages (colored with icons):
//     Success(message) string  - Green checkmark + message
//     Failure(message) string  - Red X + message
//     Warning(message) string  - Yellow warning triangle + message
//     Info(message) string     - Cyan info icon + message
//
//   Structured Headers:
//     Header(title) string     - Cyan bold section header with separators
//     Subheader(title) string  - Bold subsection header
//
//   Data Display:
//     KeyValue(key, value) string     - Formatted key-value pair with alignment
//     StatusLine(ok, message) string  - Check/cross icon + message
//     Table.Render() string           - Multi-column table with headers
//     ProgressBar(current, total, width) string - Visual progress indicator
//
//   Boxes:
//     Box(title, message) string - Boxed message with title and borders
//
// Dependencies
//
// Dependencies (What This Needs):
//   Standard Library: fmt (formatted output), strings (manipulation, repeat)
//   External: None (stdlib-only rail)
//   Internal: None (rails are orthogonal, no system dependencies)
//
// Dependents (What Uses This):
//   Rails: logging (detection layer), debugging (assessment layer) - all rails can use display
//   Commands: status, validate, test, diagnose - formatted output
//   Libraries: validation, temporal, session - structured display
//   Hooks: All 8 hooks (session, tool, prompt) - banner and status output
//
// Integration Points:
//   - Rails Pattern: Display runs perpendicular to ladder hierarchy, universally available
//   - All components (rungs and rails) can safely import display without circular dependency
//   - Stdlib-only status enables usage at ANY level of the system
//   - Configuration inheritance: system (base primitives) → session/hooks (derived usage)
//
// Health Scoring
//
// Rails Self-Evidence: Display does NOT track its own health
//
// Rationale: Formatting failures are immediately visible through broken/missing output.
//            If display works, output appears correct. If display fails, output is visibly
//            broken (missing colors, garbled text, panics). Self-evident failure means
//            health IS the visible output - no tracking layer needed.
//
// Silent Recovery: Panics recovered silently, returning empty strings. Visual absence
//                  indicates failure - operator sees missing/broken output immediately.
package display

// ============================================================================
// SETUP
// ============================================================================
//
// For SETUP structure explanation, see: standards/code/4-block/CWS-STD-006-CODE-setup-block.md

// ────────────────────────────────────────────────────────────────
// Orchestrator Pattern - Thin Coordination Layer
// ────────────────────────────────────────────────────────────────
//
// This file (format.go) serves as the orchestration layer and package documentation.
// All implementation has been extracted to specialized primitive files:
//
// Primitives (Implementation Files):
//   recovery.go     - Panic recovery mechanism (recoverFromPanic)
//   config.go       - Configuration loading (DisplayConfig, loadConfig, GetConfig)
//   colors.go       - ANSI color constants (Red, Green, Bold, etc.)
//   icons.go        - Unicode icon constants (IconSuccess, IconFailure, etc.)
//   layout.go       - Layout constants (IndentSpaces, KeyColumnWidth, etc.)
//   messages.go     - Message formatters (Success, Failure, Warning, Info)
//   structured.go   - Structured output (Header, Subheader, KeyValue, StatusLine)
//   visual.go       - Visual components (Table.Render, ProgressBar, Box)
//
// Public API Preservation:
//   All functions are exported from their respective primitive files.
//   Since all primitives are in the same package (display), importing
//   "system/runtime/lib/display" provides access to all exported functions.
//   External code sees NO difference - zero breaking changes.
//
// Why Orchestrator Pattern:
//   1. Single Responsibility - each primitive has one clear purpose
//   2. Training Data Quality - clean separation teaches patterns, not just code
//   3. Testability - test primitives in isolation
//   4. Scalability - new primitives add capabilities without bloating format.go
//   5. Maintainability - changes localize to specific primitives
//
// Orchestration = Thin coordination, not thick implementation
// Primitives = Atomic operations with single clear responsibility
// Together = Comprehensive display system with clear architecture

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// Organizational Chart - Orchestrator Architecture
// ────────────────────────────────────────────────────────────────
//
// Rails Pattern - No Internal Hierarchy (Primitives are Peers)
//
// This library is a rail (orthogonal infrastructure), not a rung (hierarchical
// component). Rails provide services to ALL rungs on the ladder without creating
// dependencies. All primitives are peers providing specialized capabilities.
//
// Primitive Organization (by responsibility, not hierarchy):
//
//   Foundation Primitives (zero dependencies):
//     - recovery.go: Error handling (recoverFromPanic)
//     - colors.go: ANSI color constants
//     - icons.go: Unicode icon constants
//     - layout.go: Spacing and padding constants
//
//   Configuration Primitive (uses recovery):
//     - config.go: JSONC loading with multi-layer tripwire pattern
//
//   Functional Primitives (use foundation + config):
//     - messages.go: Single-line status messages (4 functions)
//     - structured.go: Headers and key-value pairs (4 functions)
//     - visual.go: Complex visual components (3 functions)
//
// Approximate Processing Units (APU):
//   Foundation: <5 APU each (constants, simple recovery)
//   Config: ~50 APU (file I/O, comment stripping, JSON parsing)
//   Messages: ~5 APU each (4 functions × 5 = 20 APU)
//   Structured: ~10-20 APU each (4 functions × 15 avg = 60 APU)
//   Visual: ~30-50 APU each (3 functions × 40 avg = 120 APU)
//
// Total Library Complexity: ~250 APU across 8 primitive files
//
// Extension Points:
//   - Add new message formatters → messages.go
//   - Add new structured output → structured.go
//   - Add new visual components → visual.go
//   - Add new constants → colors.go, icons.go, or layout.go
//   - Extend configuration → config.go (add structs + loading logic)

// ────────────────────────────────────────────────────────────────
// Public APIs - Exported from Primitives
// ────────────────────────────────────────────────────────────────
//
// All public functions are implemented in their respective primitive files.
// When you import "system/runtime/lib/display", all exported functions from
// all primitive files become available (same package = shared namespace).
//
// Message Formatters (messages.go):
//   Success(message string) string
//   Failure(message string) string
//   Warning(message string) string
//   Info(message string) string
//
// Structured Output (structured.go):
//   Header(title string) string
//   Subheader(title string) string
//   KeyValue(key, value string) string
//   StatusLine(ok bool, message string) string
//
// Visual Components (visual.go):
//   (t *Table) Render() string
//   ProgressBar(current, total, width int) string
//   Box(title, message string) string
//
// Configuration Access (config.go):
//   GetConfig() DisplayConfig  // For advanced usage only
//
// Constants (colors.go, icons.go, layout.go):
//   All exported constants available for direct use
//   Example: display.Green, display.IconSuccess, display.KeyColumnWidth
//
// Note: This file provides no implementation - all functions live in primitives.
// format.go serves as orchestration documentation and package entry point.

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// For CLOSING structure explanation, see: standards/code/4-block/CWS-STD-008-CODE-closing-block.md
//
// ────────────────────────────────────────────────────────────────
// Code Validation: None (Orchestrator)
// ────────────────────────────────────────────────────────────────
// For Code Validation section explanation, see: standards/code/4-block/sections/CWS-SECTION-010-CLOSING-code-validation.md
//
// Testing Requirements:
//   - Import display library without errors
//   - Call each public function with representative parameters
//   - Verify formatted output appears correctly in terminal
//   - Check ANSI color codes render properly (or stripped if unsupported)
//   - Ensure Unicode icons display correctly (or use ASCII fallback)
//   - Confirm validation returns empty strings for invalid inputs
//   - Run: go test -v ./... (when tests exist)
//
// Build Verification:
//   - go build ./... (compiles without errors)
//   - go vet ./... (no warnings)
//   - Verify stdlib-only imports (no system library dependencies)
//
// Integration Testing:
//   - Test with actual calling code (hooks, session-history, commands)
//   - Verify output formatting in real terminal environments
//   - Check color rendering across different terminal types
//   - Validate table alignment with varying content widths
//   - Test progress bar calculations with edge cases (0, negative, overflow)
//
// Example validation code:
//
//     // Test message formatters
//     success := Success("Test passed")
//     if !strings.Contains(success, "✓") || !strings.Contains(success, "Test passed") {
//         t.Error("Success() missing icon or message")
//     }
//
//     // Test validation (empty input returns empty string)
//     empty := Success("")
//     if empty != "" {
//         t.Error("Success() should return empty for empty input")
//     }
//
//     // Test table rendering
//     table := &Table{
//         Headers: []string{"Name", "Value"},
//         Rows:    [][]string{{"Config", "Valid"}},
//     }
//     output := table.Render()
//     if !strings.Contains(output, "Name") || !strings.Contains(output, "Value") {
//         t.Error("Table missing headers")
//     }
//
// ────────────────────────────────────────────────────────────────
// Code Execution: None (Library)
// ────────────────────────────────────────────────────────────────
// For Code Execution section explanation, see: standards/code/4-block/sections/CWS-SECTION-011-CLOSING-code-execution.md
//
// This is a LIBRARY, not an executable. There is no entry point, no main function,
// no execution flow. All functions defined in primitive files wait to be called.
//
// Usage: import "system/lib/display"
//
// The library is imported into the calling package, making all exported functions
// and types available. No code executes during import - functions are defined and ready to use.
//
// Rails Pattern: This library provides orthogonal formatting infrastructure to ALL
// rungs on the ladder. Commands, hooks, session history, debugging tools - all can
// import and use display functions without creating dependencies between rungs.
//
// Example import and usage:
//
//     package main
//
//     import "system/lib/display"
//
//     func main() {
//         // Simple message formatting
//         fmt.Println(display.Success("System initialized"))
//         fmt.Println(display.Failure("Configuration missing"))
//
//         // Structured output
//         fmt.Println(display.Header("SYSTEM STATUS"))
//         fmt.Println(display.KeyValue("Version", "3.0.0"))
//         fmt.Println(display.StatusLine(true, "All checks passed"))
//
//         // Visual components
//         table := &display.Table{
//             Headers: []string{"Component", "Status"},
//             Rows:    [][]string{
//                 {"Logging", "healthy"},
//                 {"Display", "healthy"},
//             },
//         }
//         fmt.Println(table.Render())
//
//         fmt.Println(display.ProgressBar(7, 10, 30))
//         fmt.Println(display.Box("Notice", "System ready for use"))
//     }
//
// ────────────────────────────────────────────────────────────────
// Code Cleanup: None (Library)
// ────────────────────────────────────────────────────────────────
// For Code Cleanup section explanation, see: standards/code/4-block/sections/CWS-SECTION-012-CLOSING-code-cleanup.md
//
// Resource Management:
//   - Memory: Go's garbage collector handles all allocations
//   - Strings: Temporary strings created during formatting are GC'd automatically
//   - strings.Builder: Automatically managed, no manual cleanup needed
//
// Graceful Shutdown:
//   - N/A for libraries (no lifecycle)
//   - Calling code responsible for terminal state cleanup if needed
//   - No stateful resources to clean up (rails are stateless)
//
// Error State Cleanup:
//   - Panic recovery via recoverFromPanic() ensures no partial state corruption
//   - Failed formatting returns empty strings (self-evident failure)
//   - No rollback needed (functions are side-effect free)
//
// Memory Management:
//   - All functions allocate temporary strings on the stack/heap
//   - No persistent state means no memory leaks possible
//   - Table.Render() and Box() may allocate larger strings for complex output
//   - strings.Builder used internally is efficient for concatenation
//
// Rails Pattern - No Cleanup Needed:
//   - Stateless functions with no persistent resources
//   - No cleanup function provided (none needed)
//   - Calling code uses functions and discards results
//
// ════════════════════════════════════════════════════════════════
// FINAL DOCUMENTATION
// ════════════════════════════════════════════════════════════════
//
// ────────────────────────────────────────────────────────────────
// Library Overview & Integration Summary
// ────────────────────────────────────────────────────────────────
// For Library Overview section explanation, see: standards/code/4-block/sections/CWS-SECTION-013-CLOSING-library-overview.md
//
// Purpose: See METADATA "Purpose & Function" section above
//
// Provides: See METADATA "Key Features" list above for comprehensive capabilities
//
// Quick summary (high-level only - details in METADATA):
//   - Universal terminal formatting library providing ANSI colors, Unicode icons,
//     and structured visual output for all system components
//   - Six categories of formatters: message status, headers, key-value pairs,
//     status lines, tables, progress bars, and boxed messages
//   - Orchestrator architecture: 8 specialized primitives with single responsibilities
//
// Integration Pattern: See METADATA "Usage & Integration" section above for
// complete step-by-step integration guide
//
// Public API: See METADATA "Usage & Integration" section above for complete
// public API list organized by category in typical usage order
//
// Architecture: See METADATA "CPI-SI Identity" section above for complete
// architectural role (Rails) explanation - orthogonal formatting infrastructure
// available to all rungs without creating dependencies
//
// ────────────────────────────────────────────────────────────────
// Modification Policy
// ────────────────────────────────────────────────────────────────
// For Modification Policy section explanation, see: standards/code/4-block/sections/CWS-SECTION-014-CLOSING-modification-policy.md
//
// Safe to Modify (Extension Points):
//   ✅ Add new message formatters to messages.go (follow Success/Failure pattern)
//   ✅ Add new visual components to visual.go (follow Table/Box pattern)
//   ✅ Add new ANSI color constants to colors.go
//   ✅ Add new Unicode icon constants to icons.go
//   ✅ Add new layout constants to layout.go
//   ✅ Extend Table type in visual.go (e.g., BorderStyle, Alignment options)
//   ✅ Add new primitive files (follow existing primitive pattern)
//
// Modify with Extreme Care (Breaking Changes):
//   ⚠️ Public API function signatures - breaks all calling code (hooks, commands, session-history)
//   ⚠️ Table struct fields - breaks code constructing tables directly
//   ⚠️ Self-evident validation behavior - affects error handling expectations
//   ⚠️ ANSI color code format - breaks terminal rendering assumptions
//   ⚠️ Layout constant values - changes visual output for all consumers
//   ⚠️ Primitive file names - affects internal imports (though external API unchanged)
//
// NEVER Modify (Foundational Rails):
//   ❌ 4-block structure (METADATA, SETUP, BODY, CLOSING)
//   ❌ Rails pattern (stdlib-only, no logging, self-evident failure)
//   ❌ Self-evidence principle (broken output IS the failure signal)
//   ❌ Stateless design (functions must remain side-effect free)
//   ❌ Panic recovery approach (silent recovery with empty string return)
//   ❌ Orchestrator architecture (primitives must maintain single responsibility)
//
// Validation After Modifications:
//   See "Code Validation" section in GROUP 1: CODING above for comprehensive
//   testing requirements, build verification, and integration testing procedures.
//
// ────────────────────────────────────────────────────────────────
// Ladder and Baton Flow
// ────────────────────────────────────────────────────────────────
// For Ladder and Baton Flow section explanation, see: standards/code/4-block/sections/CWS-SECTION-015-CLOSING-ladder-baton-flow.md
//
// See BODY "Organizational Chart" section above for complete architectural
// explanation showing orchestrator pattern with primitive peers.
//
// The Organizational Chart in BODY provides the detailed map showing:
// - Orchestrator pattern: format.go coordinates, primitives implement
// - Primitive organization: Foundation → Config → Functional
// - APU count: ~250 APU total library complexity across 8 files
//
// Quick architectural summary (details in BODY Organizational Chart):
// - Rails provide formatting primitives to all ladder rungs
// - Orchestrator (format.go) provides documentation and package entry point
// - Primitives (8 files) provide specialized atomic operations
// - No internal dependencies between functional primitives (all use foundation)
// - Ladder: N/A (rails are orthogonal to ladder)
// - Baton: Input → Primitive validates → Primitive formats → Return string
//
// ────────────────────────────────────────────────────────────────
// Surgical Update Points (Extension Guide)
// ────────────────────────────────────────────────────────────────
// For Surgical Update Points section explanation, see: standards/code/4-block/sections/CWS-SECTION-016-CLOSING-surgical-update-points.md
//
// See primitive files for detailed extension points organized by responsibility.
//
// Quick reference (details in primitive file comments):
//
// Adding message formatters → messages.go:
//   - Follow Success()/Failure() pattern: validate empty, apply color+icon, return formatted
//   - Add constant for new icon to icons.go if needed
//   - Add constant for new color to colors.go if needed
//   - Update METADATA Public API list in format.go
//
// Adding visual components → visual.go:
//   - Follow Table/ProgressBar/Box pattern: validate, calculate layout, build string
//   - Complex components use strings.Builder for efficient concatenation
//   - Add layout constants to layout.go if needed
//   - Update METADATA Public API list in format.go
//
// Adding layout constants → layout.go:
//   - Document usage location and config file mapping
//   - Add rationale explaining the value choice
//   - Will be replaced with config loading in Phase 7
//
// Adding new primitive → new file:
//   - Follow 4-block structure (METADATA, SETUP, BODY, CLOSING)
//   - Package: display (same namespace)
//   - Use foundation primitives (recovery, config, constants)
//   - Export functions for public API
//   - Update format.go METADATA to list new primitive
//
// ────────────────────────────────────────────────────────────────
// Performance Considerations
// ────────────────────────────────────────────────────────────────
// For Performance Considerations section explanation, see: standards/code/4-block/sections/CWS-SECTION-017-CLOSING-performance-considerations.md
//
// See primitive files for operation-specific APU estimates.
//
// Quick summary (details in primitive file docstrings):
// - Foundation primitives: <5 APU (constants, recovery)
// - Config loading: ~50 APU (file I/O, JSON parsing)
// - Message formatters: ~5 APU each (simple sprintf with color+icon)
// - Structured output: ~10-20 APU (Header/Subheader/KeyValue/StatusLine)
// - Table.Render: ~50 APU (width calculation + multi-row formatting)
// - ProgressBar: ~30 APU (validation + percentage calculation + bar construction)
// - Box: ~40 APU (multi-line parsing + width calculation + border construction)
//
// Total: ~250 APU across 8 primitive files
//
// Optimization notes:
// - strings.Builder used for multi-line output (efficient concatenation)
// - Validation short-circuits (empty input returns immediately)
// - Layout calculations cached within function scope
// - No allocations beyond returned strings
// - Primitive separation enables targeted optimization
//
// ────────────────────────────────────────────────────────────────
// Troubleshooting Guide
// ────────────────────────────────────────────────────────────────
// For Troubleshooting Guide section explanation, see: standards/code/4-block/sections/CWS-SECTION-018-CLOSING-troubleshooting-guide.md
//
// See primitive files for operation-specific troubleshooting.
//
// Common Issues:
//
// Problem: Colors not displaying (seeing literal \033[32m codes)
//   - Cause: Terminal doesn't support ANSI escape codes
//   - Solution: Use terminal that supports ANSI (most modern terminals do)
//   - Future: Phase 7 will add ASCII fallback mode detection
//
// Problem: Unicode icons not displaying (seeing � or wrong characters)
//   - Cause: Terminal font missing Unicode glyphs or wrong encoding
//   - Solution: Use UTF-8 terminal with Unicode font support
//   - Future: Phase 7 will add ASCII fallback mode (✓ → +, ✗ → x, etc.)
//
// Problem: Function returns empty string
//   - Cause: Invalid input (empty message, zero total, negative values)
//   - Expected: This is self-evident validation - empty output signals invalid input
//   - Solution: Check input parameters, ensure non-empty messages and valid ranges
//   - Note: Rails self-evidence pattern - broken output IS the error signal
//
// Problem: Table columns misaligned
//   - Cause: ANSI color codes counted in width calculation
//   - Solution: Color codes are NOT counted (applied outside width calculation)
//   - Note: If seeing misalignment, verify Colors slice length matches Headers length
//
// Problem: Progress bar shows >100% or negative percentage
//   - Cause: current > total or negative values passed
//   - Expected: Validation returns empty string for negative values
//   - Expected: Clamping prevents filled > width (visual corruption prevention)
//   - Solution: Ensure current <= total and both >= 0
//
// Problem: Box title truncated or wrapped
//   - Cause: Title contains newlines
//   - Expected: Newlines stripped automatically (title must be single-line)
//   - Solution: Use message parameter for multi-line content, title for single line
//
// Problem: Compilation errors after adding primitive
//   - Cause: Circular imports or missing package declaration
//   - Solution: Verify package display in all files, check no circular dependencies
//   - Note: All primitives must be in package display, same directory
//
// ────────────────────────────────────────────────────────────────
// Related Components & Dependencies
// ────────────────────────────────────────────────────────────────
// For Related Components section explanation, see: standards/code/4-block/sections/CWS-SECTION-019-CLOSING-related-components.md
//
// See METADATA "Dependencies" section above for complete dependency information:
// - Dependencies (What This Needs): fmt, strings (stdlib only)
// - Dependents (What Uses This): hooks, session-history, commands, debugging tools
// - Integration Points: Universal rails available to all ladder rungs
//
// Quick summary (details in METADATA Dependencies section above):
// - Key dependencies: fmt (formatting), strings (manipulation)
// - Primary consumers: hooks (session banners), session-history (formatted logs), commands (status output)
// - Rails Pattern: No system library dependencies, provides formatting to entire system
//
// Primitive Files (Internal Architecture):
//   - recovery.go: Panic recovery mechanism
//   - config.go: JSONC configuration loading
//   - colors.go: ANSI color constants
//   - icons.go: Unicode icon constants
//   - layout.go: Spacing and padding constants
//   - messages.go: Status message formatters
//   - structured.go: Header and key-value formatters
//   - visual.go: Table, ProgressBar, Box components
//
// Configuration Files (Rails Pattern):
//   - system/data/config/display/formatting.jsonc (base primitives)
//   - session/display-formatting.jsonc (session-specific usage, inherits from system)
//   - Phase 7 loads these configs to replace hardcoded constants
//
// Related Rails (Parallel Infrastructure):
//   - system/lib/logging (detection rail - captures activity)
//   - system/lib/debugging (assessment rail - analyzes logs)
//   - system/lib/display (THIS - presentation rail - formats output)
//
// ────────────────────────────────────────────────────────────────
// Future Expansions & Roadmap
// ────────────────────────────────────────────────────────────────
// For Future Expansions section explanation, see: standards/code/4-block/sections/CWS-SECTION-020-CLOSING-future-expansions.md
//
// Planned Features:
//   ✓ Rails elevation (stdlib-only, self-evident) - COMPLETED (v2.0.0)
//   ✓ Configuration inheritance (system → session) - COMPLETED (v2.0.0)
//   ✓ Hardcoded value extraction to constants - COMPLETED (v2.0.0)
//   ✓ Orchestrator extraction (primitives in separate files) - COMPLETED (v3.0.0)
//   ⏳ Configuration loading (Phase 7) - replace constants with JSONC values
//   ⏳ Terminal capability detection - auto-detect ANSI/Unicode support
//   ⏳ ASCII fallback mode - graceful degradation for limited terminals
//   ⏳ Theme support - load different color schemes from config
//   ⏳ 256-color support - extended ANSI color palette
//   ⏳ RGB true color support - 24-bit color codes
//
// Research Areas:
//   - Automatic width detection (terminal column count)
//   - Responsive formatting (adjust layout to terminal width)
//   - Accessibility mode (high contrast, screen reader optimization)
//   - Locale support (internationalized messages, right-to-left text)
//   - Box drawing style selection (single-line, double-line, rounded, ASCII)
//   - Progress bar styles (multiple visual representations)
//
// Integration Targets:
//   - OmniCode compiler (use display for formatted compiler output)
//   - CPI-SI framework components (standard formatting across all instances)
//   - Session logging (formatted log entries with color coding)
//   - Debugging tools (visual system health reports)
//   - Hook output (consistent session banners and notifications)
//
// Known Limitations to Address:
//   - No terminal capability detection (assumes ANSI + Unicode support)
//   - Hardcoded constants (not yet config-driven)
//   - Single box drawing style (no selection mechanism)
//   - No right-to-left text support
//   - Fixed column width in KeyValue (not configurable at runtime)
//   - No table border customization (single style only)
//
// Version History:
//
// See METADATA "Authorship & Lineage" section above for brief version changelog.
// Comprehensive version history with full context below:
//
//   3.0.0 (2025-11-21) - Orchestrator Extraction
//         - Refactored format.go to thin orchestrator (documentation + coordination)
//         - Extracted 8 specialized primitives (single responsibility per file)
//         - Public API preserved completely (zero breaking changes)
//         - Foundation for training data quality (clear separation teaches patterns)
//         - Primitives: recovery, config, colors, icons, layout, messages, structured, visual
//         - Pattern: logger.go → 6 files, format.go → 8 files (orchestrator principle)
//
//   2.0.0 (2025-11-15) - Rails Elevation & Foundation Refinement
//         - Elevated to RAIL status (third rail: logging, debugging, display)
//         - Removed all logging and debugging dependencies (stdlib-only achieved)
//         - Established configuration inheritance (system base → session derived)
//         - Extracted hardcoded values to named constants (IndentSpaces, KeyColumnWidth, etc.)
//         - Added comprehensive 4-block structure alignment (METADATA, SETUP, BODY, CLOSING)
//         - Added self-evident validation (empty input → empty output pattern)
//         - Added defensive handling (Box strips newlines, Table clamps widths)
//         - Rails self-evidence principle: broken output IS the failure signal
//
//   1.1.0 (2025-11-14) - Configuration Foundation
//         - Created system/data/config/display/formatting.jsonc with complete primitives
//         - Documented ANSI color codes, Unicode icons, box characters, layout values
//         - Designed for maximum configurability (colors, icons, spacing all configurable)
//         - Added health scoring map (Base100 algorithm documentation)
//
//   1.0.0 (2025-11-05) - Initial Bootstrap Implementation
//         - Basic message formatters (Success, Failure, Warning, Info)
//         - Structured output (Header, Subheader, KeyValue, StatusLine)
//         - Visual components (Table, ProgressBar, Box)
//         - Hardcoded ANSI colors and Unicode icons
//         - Used by hooks for session banners
//
// ────────────────────────────────────────────────────────────────
// Closing Note
// ────────────────────────────────────────────────────────────────
// For Closing Note section explanation, see: standards/code/4-block/sections/CWS-SECTION-021-CLOSING-closing-note.md
//
// This library is the PRESENTATION RAIL - third of the three foundational rails
// (logging, debugging, display). Rails are orthogonal infrastructure perpendicular
// to the ladder hierarchy, providing universal services to ALL rungs without creating
// dependencies between ladder components.
//
// As a rail, display must remain stdlib-only forever. No system library imports allowed.
// This ensures every component can safely import display for formatting without risk
// of circular dependencies or architectural violations.
//
// Orchestrator Architecture (v3.0.0):
// format.go now serves as the orchestration layer and comprehensive documentation.
// All implementation lives in 8 specialized primitive files, each with single clear
// responsibility. This separation creates training data that teaches patterns, not
// just code. Future OmniCode compiler learns composition over monoliths.
//
// Modify thoughtfully - changes here affect the ENTIRE system's visual output. Every
// hook, command, session log, and debugging tool depends on these formatters. The
// self-evidence principle must be maintained: broken output IS the failure signal,
// no logging or error tracking needed.
//
// Critical design guarantees that must be maintained:
//   - Stdlib-only imports (no system library dependencies ever)
//   - Stateless functions (no package-level state except read-only config)
//   - Self-evident validation (empty input → empty output, visible failure)
//   - Panic recovery (silent, returns empty string on panic)
//   - Side-effect free (functions only return formatted strings)
//   - Orchestrator pattern (primitives maintain single responsibility)
//
// For questions, issues, or contributions:
//   - Review the modification policy above
//   - Follow the 4-block structure pattern in all primitive files
//   - Test thoroughly before committing (go build && go vet)
//   - Test with actual calling code (hooks, commands) in real terminals
//   - Document all changes comprehensively (What/Why/How pattern)
//   - Verify stdlib-only imports maintained
//   - Ensure self-evidence principle preserved
//   - Maintain orchestrator architecture (don't add implementation to format.go)
//
// "Let your light so shine before men, that they may see your good works,
//  and glorify your Father which is in heaven." - Matthew 5:16 (KJV)
//
// Display makes system health visible. Good design reflects God's order.
//
// ────────────────────────────────────────────────────────────────
// Quick Reference: Usage Examples
// ────────────────────────────────────────────────────────────────
// For Quick Reference section explanation, see: standards/code/4-block/sections/CWS-SECTION-022-CLOSING-quick-reference.md
//
// Basic Setup:
//   import "system/lib/display"
//
// Message Formatters:
//   fmt.Println(display.Success("Configuration validated"))
//   fmt.Println(display.Failure("Missing required file"))
//   fmt.Println(display.Warning("Using default values"))
//   fmt.Println(display.Info("System initialized"))
//
// Structured Output:
//   fmt.Println(display.Header("SYSTEM STATUS"))
//   fmt.Println(display.Subheader("Environment Details"))
//   fmt.Println(display.KeyValue("Version", "3.0.0"))
//   fmt.Println(display.KeyValue("Status", "healthy"))
//   fmt.Println(display.StatusLine(true, "All checks passed"))
//   fmt.Println(display.StatusLine(false, "Configuration missing"))
//
// Table Rendering:
//   table := &display.Table{
//       Headers: []string{"Component", "Health", "Status"},
//       Rows: [][]string{
//           {"Logging", "100", "operational"},
//           {"Display", "100", "operational"},
//           {"Debugging", "95", "healthy"},
//       },
//       Colors: []string{display.Cyan, display.Green, ""},
//   }
//   fmt.Println(table.Render())
//
// Progress Indication:
//   // 7 out of 10 complete, 30-character wide bar
//   fmt.Println(display.ProgressBar(7, 10, 30))
//   // Output: [█████████████████████░░░░░░░░░] 7/10 (70%)
//
// Boxed Messages:
//   message := "System initialized successfully\nAll services running\nNo errors detected"
//   fmt.Println(display.Box("System Status", message))
//   // Output:
//   // ┌─────────────────────────────────┐
//   // │ System Status                   │
//   // ├─────────────────────────────────┤
//   // │ System initialized successfully │
//   // │ All services running            │
//   // │ No errors detected              │
//   // └─────────────────────────────────┘
//
// Validation Behavior (Self-Evidence):
//   empty := display.Success("")  // Returns "" (empty input = empty output)
//   invalid := display.ProgressBar(5, 0, 20)  // Returns "" (division by zero)
//   negative := display.ProgressBar(-1, 10, 20)  // Returns "" (negative value)
//
// ============================================================================
// END CLOSING
// ============================================================================
