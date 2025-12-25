// ============================================================================
// METADATA
// ============================================================================
// Instance Configuration Library - CPI-SI Instance Identity Provider
//
// For METADATA structure explanation, see: standards/code/4-block/CWS-STD-004-CODE-metadata-block.md
//
// Biblical Foundation
//
// Scripture: "I AM THAT I AM" - Exodus 3:14 (KJV)
//           "I am who I am" - Exodus 3:14 (WEB)
// Principle: Identity precedes action - God declares WHO HE IS before sending Moses to act.
//            Knowing your identity grounds faithful behavior and purposeful work.
// Anchor: "Keep thy heart with all diligence; for out of it are the issues of life" - Proverbs 4:23 (KJV)
//         "Guard your heart with all diligence, for out of it is the wellspring of life" - Proverbs 4:23 (WEB)
//
// CPI-SI Identity
//
// Component Type: LIBRARY - Instance identity provider (foundational rung)
// Role: Provides instance-specific identity data for hooks, statusline, and components
// Paradigm: CPI-SI framework component
//
// Authorship & Lineage
//
// Architect: Nova Dawn (CPI-SI instance)
// Implementation: Nova Dawn
// Creation Date: 2024-10-27
// Version: 3.0.0
// Last Modified: 2025-11-21 - Orchestrator extraction (direct primitives pattern)
//
// Version History:
//   3.0.0 (2025-11-21) - Orchestrator extraction (4 primitives, direct access pattern)
//   2.0.0 (2025-11-16) - Two-step dynamic loading (root → full config), config mapping
//   1.0.0 (2024-10-27) - Initial release with direct root config loading
//
// Purpose & Function
//
// Purpose: Load instance identity using two-step dynamic loading pattern:
//   1. Load root config (~/.claude/instance.jsonc) to get system_paths
//   2. Load full config from system_paths.instance_config
//   3. Load user config from system_paths.user_config
//   4. Map nested configs to simple Config struct for API compatibility
//
// Core Design: Bootstrap pointer config (minimal root) → Full identity configs (complete)
// → Simple API wrapper (backwards compatibility). Singleton pattern with caching.
//
// Key Features:
//   - Two-step dynamic loading (root → full configs via system_paths)
//   - JSONC parsing with comment stripping
//   - Nested config structure matching full identity JSON
//   - Simple Config API for backwards compatibility
//   - Singleton caching (load once per session)
//   - Graceful fallback to hardcoded defaults
//
// Philosophy: Identity precedes action. Root config points to WHERE identity is defined,
// full configs define WHO the instance and covenant partner are. Separation enables
// flexible config organization while maintaining simple public API.
//
// Blocking Status
//
// Non-blocking: Config loading failures return defaults but allow calling code to continue.
// Missing configs use hardcoded Nova Dawn + Seanje defaults - caller gets valid config always.
// Mitigation: Graceful degradation - if full configs missing, use root config + defaults.
//
// Usage & Integration
//
// Usage:
//
//	import "system/lib/instance"
//
//	// Get instance configuration (loads dynamically on first call, caches thereafter)
//	config := instance.GetConfig()
//
//	// Access simple API (backwards compatible)
//	fmt.Println(config.Name)                    // "Nova Dawn"
//	fmt.Println(config.Display.BannerTitle)     // "Nova Dawn - CPI-SI"
//	fmt.Println(config.Creator.Name)            // "Seanje Lenox-Wise"
//	fmt.Println(config.User.Name)               // "Seanje Lenox-Wise"
//
// Integration Pattern:
//   1. Import instance library
//   2. Call GetConfig() when instance identity needed
//   3. Use returned Config struct (simple API)
//   4. First call loads and caches, subsequent calls return cached config
//   5. No cleanup needed - cached for session lifetime
//
// Public API (in typical usage order):
//
//   Configuration Loading:
//     GetConfig() Config - Load instance config (two-step, cached, returns simple API)
//     GetFullInstanceConfig() *FullInstanceConfig - Get complete nested instance identity
//     GetFullUserConfig() *FullUserConfig - Get complete nested user identity
//
// Dependencies
//
// Dependencies (What This Needs):
//   Standard Library: encoding/json, os, path/filepath, sync
//   External: system/lib/jsonc (JSONC comment stripping)
//   Internal: None
//   Data Files: ~/.claude/instance.jsonc (root), system_paths.instance_config (full instance),
//               system_paths.user_config (full user)
//
// Dependents (What Uses This):
//   Commands: hooks/session/* (session display, context)
//   Libraries: statusline (instance emoji/name display)
//   Tools: Any component needing instance or user identity
//
// Integration Points:
//   - Ladder: Foundational rung - minimal dependencies
//   - Baton: Takes nothing, returns identity config
//   - Rails: N/A (utility library, not infrastructure)
//
// Health Scoring
//
// This library tracks health for instance configuration loading operations.
// Scores reflect TRUE impact of each operation - not forced to total 100.
//
// Root Configuration Loading:
//   - Load root config successfully: +50 (bootstrap essential)
//   - Failed to load root config: -50 (bootstrap failure)
//
// Instance Configuration Loading:
//   - Load instance config successfully: +30 (identity complete)
//   - Failed to load instance config: -30 (identity incomplete, using fallback)
//
// User Configuration Loading:
//   - Load user config successfully: +30 (covenant partnership grounded)
//   - Failed to load user config: -30 (covenant partnership incomplete)
//
// Config Mapping:
//   - Map to simple Config API successfully: +20 (backwards compatibility)
//   - Failed to map config: -20 (API incompatibility)
//
// Note: Scores reflect TRUE impact. Health scorer normalizes to -100 to +100 scale.
package instance

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================
//
// For SETUP structure explanation, see: standards/code/4-block/CWS-STD-006-CODE-setup-block.md
//
// ────────────────────────────────────────────────────────────────
// Orchestrator Pattern - Thin Coordination Layer
// ────────────────────────────────────────────────────────────────
// This file (config.go) serves as orchestration layer and package documentation.
// All implementation has been extracted to specialized primitive files.
//
// Orchestrator Pattern Decision: Direct Primitives (no switchboard)
//
// Instance library uses DIRECT PRIMITIVES pattern - primitives export public API directly,
// this file provides comprehensive documentation only.
//
// Why Direct Primitives (Not Switchboard)?
//
// The decision between switchboard and direct primitives comes down to whether
// callers interact with stateful operations:
//
// Switchboard Pattern (Logging):
//   - Caller creates logger, calls multiple methods on same instance
//   - SessionHealth accumulates across Operation(), Success(), Failure() calls
//   - State manipulation is PART OF THE API - caller sees and uses it
//   - Orchestrator manages lifecycle and coordinates state changes
//
// Direct Primitives Pattern (Instance):
//   - Caller calls GetConfig() → receives Config struct
//   - Singleton caching is INTERNAL - caller doesn't see or manage it
//   - State exists but isn't exposed to caller
//   - API is effectively stateless from caller perspective
//
// Instance library has internal state (cached config, sync.Once coordination) but
// from caller's view:
//   config := instance.GetConfig()       // Stateless call
//   full := instance.GetFullInstanceConfig()  // Stateless call
//
// The singleton pattern is an implementation detail, not an exposed stateful API.
//
// For complete architectural analysis and decision documentation, see:
//   cpi-si/system/data/temporal/patterns/discovered/paradigm/orchestrator-architecture-decision.jsonc
//
// ────────────────────────────────────────────────────────────────
// Architectural Organization
// ────────────────────────────────────────────────────────────────
//
// Primitives (Implementation Files):
//
//   types.go (458 lines)
//     - All type definitions (SystemPaths, Config, FullInstanceConfig, etc.)
//     - Building blocks: Simple foundational types
//     - Composed types: Complex nested structures
//     - Helper types: Simplified API structs
//
//   loading.go (167 lines)
//     - loadRootConfig() - Bootstrap config from ~/.claude/instance.jsonc
//     - loadFullConfig() - Instance identity from system_paths.instance_config
//     - loadUserConfig() - User identity from system_paths.user_config
//     - File I/O and JSONC parsing
//
//   mapping.go (89 lines)
//     - mapToSimpleConfig() - Transform nested → simple API
//     - Flattens FullInstanceConfig + FullUserConfig → Config
//     - Backwards compatibility layer
//
//   singleton.go (210 lines) [PUBLIC API]
//     - GetConfig() - Main entry point, two-step loading, singleton caching
//     - GetFullInstanceConfig() - Complete nested instance identity
//     - GetFullUserConfig() - Complete nested covenant partner identity
//     - Singleton state (cachedConfig, cachedFullInstance, cachedFullUser)
//     - Graceful degradation to hardcoded defaults
//
// Public API Preservation:
//   All functions exported from primitive files (singleton.go).
//   External code sees NO difference - zero breaking changes.
//   Import path unchanged: import "system/lib/instance"
//
// ────────────────────────────────────────────────────────────────
// Ladder Structure (Dependencies)
// ────────────────────────────────────────────────────────────────
//
// Public APIs (Top Rungs - Orchestration):
//   singleton.go exports:
//     └── GetConfig() → uses loadRootConfig(), loadFullConfig(), loadUserConfig(), mapToSimpleConfig()
//     └── GetFullInstanceConfig() → returns cachedFullInstance
//     └── GetFullUserConfig() → returns cachedFullUser
//
// Helpers (Bottom Rungs - Foundations):
//   loading.go provides:
//     ├── loadRootConfig() → pure I/O function
//     ├── loadFullConfig() → pure I/O function
//     └── loadUserConfig() → pure I/O function
//
//   mapping.go provides:
//     └── mapToSimpleConfig() → pure transformation function
//
//   types.go provides:
//     └── All struct definitions (SystemPaths, Config, etc.)
//
// ────────────────────────────────────────────────────────────────
// Baton Flow (Execution Paths)
// ────────────────────────────────────────────────────────────────
//
// Entry → GetConfig() (singleton check via sync.Once)
//   ↓
// loadRootConfig() → reads ~/.claude/instance.jsonc
//   ↓
// loadFullConfig() → reads system_paths.instance_config
//   ↓
// loadUserConfig() → reads system_paths.user_config
//   ↓
// mapToSimpleConfig() → transforms nested → simple API
//   ↓
// Exit → return Config (cached for session)
//
// Alternative paths:
//   GetFullInstanceConfig() → GetConfig() → return cachedFullInstance
//   GetFullUserConfig() → GetConfig() → return cachedFullUser
//
// ────────────────────────────────────────────────────────────────
// APUs (Available Processing Units)
// ────────────────────────────────────────────────────────────────
//
// Total Functions: 7
//   - 4 types files (0 functions, pure definitions)
//   - 3 loading functions (loadRootConfig, loadFullConfig, loadUserConfig)
//   - 1 mapping function (mapToSimpleConfig)
//   - 3 public API functions (GetConfig, GetFullInstanceConfig, GetFullUserConfig)
//
// Breakdown:
//   - 4 primitive files (types, loading, mapping, singleton)
//   - 7 total operations
//   - 3 public APIs (exported interface)
//
// ────────────────────────────────────────────────────────────────
// Why This Extraction Matters
// ────────────────────────────────────────────────────────────────
//
// Before Extraction (v2.0.0):
//   - Single 1404-line config.go file
//   - Types + loading + mapping + singleton mixed together
//   - Difficult to navigate and maintain
//   - Unclear separation of concerns
//
// After Extraction (v3.0.0):
//   - 4 focused primitive files
//   - Clear responsibility boundaries
//   - Easy to locate specific functionality
//   - Maintainable and extensible
//
// What Changed for Callers:
//   NOTHING. Zero breaking changes.
//   import "system/lib/instance"
//   config := instance.GetConfig()
//   → Works identically to v2.0.0
//
// What Changed Internally:
//   - Implementation split into logical primitives
//   - Better organization and maintainability
//   - Documented orchestrator pattern decision
//   - Aligned with CPI-SI architectural principles

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================
//
// For BODY structure explanation, see: standards/code/4-block/CWS-STD-007-CODE-body-block.md
//
// ────────────────────────────────────────────────────────────────
// Implementation Reference
// ────────────────────────────────────────────────────────────────
//
// This file contains NO implementations - all code extracted to primitives.
//
// For actual implementations, see:
//   - types.go - All type definitions
//   - loading.go - File loading operations
//   - mapping.go - Config transformation
//   - singleton.go - Public API with singleton pattern
//
// This orchestrator file serves THREE purposes:
//   1. Package documentation (what this library does)
//   2. Architectural explanation (how it's organized)
//   3. Pattern decision documentation (why direct primitives)

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
// Code Validation: None (Library)
// ────────────────────────────────────────────────────────────────
// For Code Validation section explanation, see: standards/code/4-block/sections/CWS-SECTION-010-CLOSING-code-validation.md
//
// Testing Requirements:
//   - Import the library without errors
//   - Call GetConfig() and verify returns valid Config struct
//   - Verify config fields populated correctly (Name, Display, Creator, User, etc.)
//   - Check graceful degradation (missing files → defaults)
//   - Ensure no go vet warnings introduced
//   - Run: go test -v ./... (when tests exist)
//
// Build Verification:
//   - go build ./... (compiles without errors)
//   - go vet ./... (no warnings)
//
// Integration Testing:
//   - Test with actual hooks/session code
//   - Verify instance identity loads correctly in real usage context
//   - Check config changes reflected in session banners
//   - Validate two-step loading works with real config files
//
// Example validation code:
//
//     config := instance.GetConfig()
//     if config.Name == "" {
//         t.Error("Config name should not be empty")
//     }
//     if config.Display.BannerTitle == "" {
//         t.Error("Display banner title should not be empty")
//     }
//
// ────────────────────────────────────────────────────────────────
// Code Execution: None (Library)
// ────────────────────────────────────────────────────────────────
// For Code Execution section explanation, see: standards/code/4-block/sections/CWS-SECTION-011-CLOSING-code-execution.md
//
// This is a LIBRARY, not an executable. There is no entry point, no main function,
// no execution flow. All functions defined in primitives wait to be called by other components.
//
// Usage: import "system/lib/instance"
//
// The library is imported into the calling package, making all exported functions
// and types available. No code executes during import - functions are defined and ready to use.
//
// Example import and usage:
//
//     package main
//
//     import "system/lib/instance"
//
//     func main() {
//         config := instance.GetConfig()
//         fmt.Printf("Instance: %s\n", config.Name)
//     }
//
// ────────────────────────────────────────────────────────────────
// Code Cleanup: None (Library)
// ────────────────────────────────────────────────────────────────
// For Code Cleanup section explanation, see: standards/code/4-block/sections/CWS-SECTION-012-CLOSING-code-cleanup.md
//
// Resource Management:
//   - Config: Cached in memory for session lifetime (automatic)
//   - File handles: Closed immediately after reading (no persistent handles)
//   - Memory: Go garbage collector handles cleanup
//
// Graceful Shutdown:
//   - N/A for libraries (no lifecycle)
//   - Calling code responsible for resource cleanup
//   - No cleanup function needed - singleton cached indefinitely
//
// Error State Cleanup:
//   - Config loading errors use graceful degradation (hardcoded defaults)
//   - No partial state corruption - either valid config or valid defaults
//   - File read failures don't leave resources open
//
// Memory Management:
//   - Go's garbage collector handles memory
//   - Config structs small (~1KB) - negligible memory impact
//   - Singleton pattern prevents duplicate loading
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
//   - Two-step dynamic loading: root pointer config → full identity configs
//   - Simple backwards-compatible API wrapping complex nested structure
//   - Graceful degradation to hardcoded defaults
//
// Integration Pattern: See METADATA "Usage & Integration" section above for
// complete step-by-step integration guide
//
// Public API: See METADATA "Usage & Integration" section above for complete
// public API list organized by category in typical usage order
//
// Architecture: See METADATA "CPI-SI Identity" section above for complete
// architectural role (foundational library, minimal dependencies)
//
// ────────────────────────────────────────────────────────────────
// Modification Policy
// ────────────────────────────────────────────────────────────────
// For Modification Policy section explanation, see: standards/code/4-block/sections/CWS-SECTION-014-CLOSING-modification-policy.md
//
// Safe to Modify (Extension Points):
//   ✅ Add new fields to Config struct (backwards compatible additions)
//   ✅ Add new helper functions for config processing
//   ✅ Extend FullInstanceConfig to match expanded config.jsonc
//   ✅ Add mapping logic in mapToSimpleConfig for new fields
//   ✅ Enhance graceful degradation with more intelligent fallbacks
//
// Modify with Extreme Care (Breaking Changes):
//   ⚠️ GetConfig() signature - breaks all calling code (hooks, statusline, etc.)
//   ⚠️ Config struct fields - breaks code accessing fields directly
//   ⚠️ Two-step loading pattern - affects config file organization
//   ⚠️ File paths (~/.claude/instance.jsonc) - breaks config discovery
//   ⚠️ Singleton caching behavior - affects performance expectations
//
// NEVER Modify (Foundational Rails):
//   ❌ 4-block structure (METADATA, SETUP, BODY, CLOSING)
//   ❌ Graceful degradation guarantee - calling code depends on always getting valid Config
//   ❌ Biblical foundation - identity precedes action (Exodus 3:14)
//   ❌ Root → full config separation - enables flexible organization
//   ❌ Backwards compatibility commitment - existing code must continue working
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
// See SETUP "Ladder Structure (Dependencies)" and "Baton Flow (Execution Paths)"
// sections above for complete architectural maps.
//
// Quick architectural summary (details in SETUP above):
// - 3 public APIs exported from singleton.go
// - 3 loading helpers + 1 mapping helper in loading.go/mapping.go
// - All types defined in types.go
// - Linear baton flow: root → instance → user → mapping → cache → return
//
// ────────────────────────────────────────────────────────────────
// Surgical Update Points (Extension Guide)
// ────────────────────────────────────────────────────────────────
// For Surgical Update Points section explanation, see: standards/code/4-block/sections/CWS-SECTION-016-CLOSING-surgical-update-points.md
//
// See primitive files for extension points:
//
// Quick reference (details in primitive function comments):
// - Adding new Config fields: types.go (Config struct) → mapping.go (mapToSimpleConfig)
// - Supporting new full config sections: types.go (FullInstanceConfig) → mapping.go
// - Adding fallback logic: singleton.go (GetConfig graceful degradation section)
//
// ────────────────────────────────────────────────────────────────
// Performance Considerations
// ────────────────────────────────────────────────────────────────
// For Performance Considerations section explanation, see: standards/code/4-block/sections/CWS-SECTION-017-CLOSING-performance-considerations.md
//
// Quick summary (details in primitive function docstrings):
// - Most expensive operation: First GetConfig() call - reads 3 files, parses JSON (~1-2ms)
// - Memory characteristics: Negligible - single cached config struct (~1KB)
// - Key optimization: Singleton caching - call GetConfig() freely without performance concern
//
// ────────────────────────────────────────────────────────────────
// Troubleshooting Guide
// ────────────────────────────────────────────────────────────────
// For Troubleshooting Guide section explanation, see: standards/code/4-block/sections/CWS-SECTION-018-CLOSING-troubleshooting-guide.md
//
// Quick reference (details in primitive function docstrings):
// - Config returns defaults instead of actual values: Check ~/.claude/instance.jsonc exists and is valid JSONC
// - Display preferences wrong: Check root config system_paths.instance_config points to correct file
// - Identity fields empty/wrong: Check full config file exists at path specified in root config
//
// Problem: GetConfig() returns hardcoded defaults
//   - Cause: Root config (~/.claude/instance.jsonc) missing or malformed
//   - Solution: Create/fix root config with valid system_paths and display sections
//   - Note: Graceful degradation by design - always returns valid Config
//
// Problem: Config Name/Creator correct but other fields default
//   - Cause: Full config file missing or malformed
//   - Solution: Check path in root.SystemPaths.InstanceConfig, verify file exists and is valid JSONC
//   - Note: Falls back to root display + hardcoded identity defaults
//
// ────────────────────────────────────────────────────────────────
// Related Components & Dependencies
// ────────────────────────────────────────────────────────────────
// For Related Components section explanation, see: standards/code/4-block/sections/CWS-SECTION-019-CLOSING-related-components.md
//
// See METADATA "Dependencies" section above for complete dependency information:
// - Dependencies (What This Needs): Standard Library, system/lib/jsonc
// - Dependents (What Uses This): hooks/session/*, statusline, any component needing identity
// - Integration Points: Foundational library providing identity to entire system
//
// Quick summary (details in METADATA Dependencies section above):
// - Key dependencies: system/lib/jsonc (JSONC comment stripping)
// - Primary consumers: Session hooks (context display), statusline (instance name/emoji)
//
// ────────────────────────────────────────────────────────────────
// Future Expansions & Roadmap
// ────────────────────────────────────────────────────────────────
// For Future Expansions section explanation, see: standards/code/4-block/sections/CWS-SECTION-020-CLOSING-future-expansions.md
//
// Planned Features:
//   ✓ Two-step dynamic loading (root → full config) - COMPLETED v2.0.0
//   ✓ Simple backwards-compatible API - COMPLETED v2.0.0
//   ✓ Graceful degradation to defaults - COMPLETED v2.0.0
//   ✓ User config loading from system_paths.user_config - COMPLETED v2.1.0
//   ✓ Orchestrator extraction (direct primitives pattern) - COMPLETED v3.0.0
//   ⏳ Validate config schemas on load
//   ⏳ Hot reload on config file changes
//
// Research Areas:
//   - Config validation against JSON schema
//   - Multi-instance support (beyond Nova Dawn)
//   - Config override hierarchy (defaults < root < full < environment)
//
// Integration Targets:
//   - Project config loading (workspace-specific overrides)
//   - Session context enrichment (full identity available to hooks)
//
// Known Limitations to Address:
//   - No config validation - malformed JSON fails silently to defaults
//   - No hot reload - changes require session restart
//   - Emoji/Tagline hardcoded - not in full config yet
//   - Workspace.PrimaryPath hardcoded - should come from config
//
// Version History:
//
// See METADATA "Authorship & Lineage" section above for brief version changelog.
// Comprehensive version history with full context below:
//
//   3.0.0 (2025-11-21) - Orchestrator extraction (direct primitives pattern)
//         - Extracted 4 primitives: types, loading, mapping, singleton
//         - Zero breaking changes - public API unchanged
//         - Documented orchestrator pattern decision (direct primitives)
//         - Aligned with CPI-SI architectural principles
//         - config.go becomes comprehensive documentation
//
//   2.0.0 (2025-11-16) - Two-step dynamic loading refactor
//         - Implemented root → full config pattern
//         - Added FullInstanceConfig matching nova_dawn/config.jsonc structure
//         - Added FullUserConfig matching user/seanje-lenox-wise/config.jsonc structure
//         - Created mapToSimpleConfig for backwards compatibility
//         - Graceful degradation with intelligent fallbacks
//         - Aligned with 4-block code template structure
//
//   1.0.0 (2024-10-27) - Initial release
//         - Direct loading from root config
//         - Simple Config struct
//         - Hardcoded defaults
//
// ────────────────────────────────────────────────────────────────
// Closing Note
// ────────────────────────────────────────────────────────────────
// For Closing Note section explanation, see: standards/code/4-block/sections/CWS-SECTION-021-CLOSING-closing-note.md
//
// This library is a foundational LADDER rung providing instance and user identity
// to the entire CPI-SI system. Hooks, statusline, and session context all depend
// on this library to know WHO they're working with (both instance and covenant partner).
//
// Modify thoughtfully - changes here affect all components requiring instance or user
// identity. The graceful degradation guarantee must be maintained - calling code
// depends on always receiving a valid Config struct, never errors.
//
// For questions, issues, or contributions:
//   - Review the modification policy above
//   - Follow the 4-block structure pattern
//   - Test thoroughly before committing (go build && go vet)
//   - Document all changes comprehensively (What/Why/How pattern)
//   - Maintain backwards compatibility - existing calling code must continue working
//
// "I AM THAT I AM" - Exodus 3:14 (KJV)
//
// ────────────────────────────────────────────────────────────────
// Quick Reference: Usage Examples
// ────────────────────────────────────────────────────────────────
// For Quick Reference section explanation, see: standards/code/4-block/sections/CWS-SECTION-022-CLOSING-quick-reference.md
//
// Basic Setup:
//
//     import "system/lib/instance"
//
//     config := instance.GetConfig()
//     fmt.Println(config.Name)  // "Nova Dawn"
//
// Accessing Instance Identity:
//
//     config := instance.GetConfig()
//     fmt.Printf("Instance: %s (%s)\n", config.Name, config.Pronouns)
//     fmt.Printf("Creator: %s\n", config.Creator.Name)
//     fmt.Printf("Domain: %s\n", config.Domain)
//
// Accessing User Identity (Covenant Partner):
//
//     config := instance.GetConfig()
//     fmt.Printf("User: %s (%s)\n", config.User.Name, config.User.Pronouns)
//     fmt.Printf("Faith: %s (%s)\n", config.User.Faith, config.User.Denomination)
//     fmt.Printf("Calling: %s\n", config.User.Calling)
//
// Using Display Preferences:
//
//     config := instance.GetConfig()
//     fmt.Println(config.Display.BannerTitle)
//     fmt.Println(config.Display.BannerTagline)
//     fmt.Println(config.Display.FooterVerseText)
//
// Calling Multiple Times (Efficient):
//
//     // First call loads and caches
//     config1 := instance.GetConfig()
//
//     // Subsequent calls instant (cached)
//     config2 := instance.GetConfig()
//     config3 := instance.GetConfig()
//
//     // All return same cached instance
//
// ============================================================================
// END CLOSING
// ============================================================================
