# ═══════════════════════════════════════════════════════════════════════════
# TEMPLATE: Project Makefile (4-Block Structure)
# Key: CODE-MAKE-001
# ═══════════════════════════════════════════════════════════════════════════
#
# This is a TEMPLATE file - copy and modify for new project Makefiles.
# Replace all [bracketed] placeholders with actual content.
# Rename to "Makefile" (no extension) when ready to use.
#
# Derived from: divisions/tech/language/compiler/Makefile (reference implementation)
# See: standards/code/4-block/ for complete documentation
#
# ═══════════════════════════════════════════════════════════════════════════

# ============================================================================
# METADATA
# ============================================================================
#
# Package:     [organization/project-name]
# File:        Makefile
# Key:         [PROJECT-BUILD-###] (Build automation system)
#
# For METADATA structure explanation, see: standards/code/4-block/CWS-STD-004-CODE-metadata-block.md
#
# ────────────────────────────────────────────────────────────────
# Biblical Foundation
# ────────────────────────────────────────────────────────────────
#
# Scripture: [Relevant verse grounding this build system's purpose]
#
# Principle: [Kingdom principle this build system demonstrates]
#
# Anchor: [Supporting verse reinforcing the principle]
#
# ────────────────────────────────────────────────────────────────
# CPI-SI Identity
# ────────────────────────────────────────────────────────────────
#
# Component Type: [Ladder/Baton/Rails - see CWS-STD-004 for explanations]
#
# Role: [Specific responsibility in system architecture]
#
# Paradigm: CPI-SI framework component
#
# ────────────────────────────────────────────────────────────────
# Authorship & Lineage
# ────────────────────────────────────────────────────────────────
#
#   Architect: [Who designed the approach and requirements]
#   Implementation: [Who wrote the Makefile and verified it works]
#   Created: [YYYY-MM-DD]
#   Version: [MAJOR.MINOR.PATCH]
#   Modified: [YYYY-MM-DD - what changed]
#
# Version History:
#
#   [X.Y.Z] ([YYYY-MM-DD]) - [Brief description of changes]
#
# ────────────────────────────────────────────────────────────────
# Purpose & Function
# ────────────────────────────────────────────────────────────────
#
# Purpose: [What problem does this build system solve?]
#
# Core Design: [Architectural pattern or paradigm]
#
# Key Features:
#
#   - [What it provides - major build capabilities]
#   - [What it enables - what developers can do with this]
#   - [What problems it solves - specific build use cases]
#
# Philosophy: [Guiding principle for how this build system works]
#
# ────────────────────────────────────────────────────────────────
# Blocking Status
# ────────────────────────────────────────────────────────────────
#
# [Blocking/Non-blocking]: [Brief explanation]
#
# Mitigation: [How blocking/failures handled]
#
# ────────────────────────────────────────────────────────────────
# Usage & Integration
# ────────────────────────────────────────────────────────────────
#
# Basic Usage:
#
#   make [target]         # Run specific target
#   make                  # Run default target (all)
#   make help             # Show available targets
#
# Integration Pattern:
#
#   1. [Initial setup step]
#   2. [Configuration step if needed]
#   3. [Typical usage workflow]
#
# Public API (Make Targets - in typical usage order):
#
#   [Category] ([purpose]):
#     make [target]       # [Description]
#     make [target]       # [Description]
#
# ────────────────────────────────────────────────────────────────
# Dependencies
# ────────────────────────────────────────────────────────────────
#
# What This Needs:
#
#   - System Tools: [make, shell, etc.]
#   - Language Toolchain: [go, gcc, rustc, etc.]
#   - External Tools: [None | list external tools]
#
# What Uses This:
#
#   - Developers: [build, test, run workflows]
#   - CI/CD: [automated build pipelines]
#
# Integration Points:
#
#   - [How other systems connect - scripts, CI, etc.]
#
# ────────────────────────────────────────────────────────────────
# Health Scoring
# ────────────────────────────────────────────────────────────────
#
# [Brief description of how health is tracked for this build system]
#
# [Operation Category]:
#
#   - [Specific operation]: ±X
#   - [Another operation]: ±Y
#
# Note: Scores reflect TRUE impact. Health scorer normalizes to -100 to +100 scale.
#
# ============================================================================
# END METADATA
# ============================================================================

# ============================================================================
# SETUP
# ============================================================================
#
# For SETUP structure explanation, see: standards/code/4-block/CWS-STD-006-CODE-setup-block.md
#
# Section order: Declarations → Constants → Variables → Pattern Rules → Default Target → Shell Configuration
# This flows: what exists → fixed config → dynamic config → file patterns → entry point → environment
#
# Universal mapping (see standards for cross-language patterns):
#   Declarations ≈ Imports (what's available)
#   Constants ≈ Constants (fixed values)
#   Variables ≈ Variables (overridable values)
#   Pattern Rules ≈ Types (file type transformations)
#   Default Target ≈ Entry Point (what runs by default)
#   Shell Configuration ≈ Package-Level State (environment setup)

# ────────────────────────────────────────────────────────────────
# Declarations
# ────────────────────────────────────────────────────────────────
#
# Declare what targets exist. .PHONY prevents conflicts with files of same name.
# Include directives bring in shared Makefile fragments if needed.
#
# See: standards/code/4-block/sections/setup/CWS-SECTION-SETUP-001-imports.md

# ═══ Target Declarations ═══
# All targets that don't produce files of the same name.

.PHONY: all build test clean run help

# ═══ Include Directives ═══
# Shared Makefile fragments (use sparingly - each adds complexity).
# [Reserved: Currently none - self-contained Makefile]

# include [path/to/shared.mk]  # [Purpose of included fragment]

# ────────────────────────────────────────────────────────────────
# Constants
# ────────────────────────────────────────────────────────────────
#
# Fixed values that never change. No ?= syntax - these are immutable.
# Magic numbers given meaningful names, paths documented with reasoning.
#
# See: standards/code/4-block/sections/setup/CWS-SECTION-SETUP-002-constants.md

# ═══ Project Constants ═══
# Core project identity - changing these affects all build outputs.

BINARY_NAME = [binary-name]
BUILD_DIR = bin

# ═══ Path Constants ═══
# Directory structure - consistent paths for all operations.

SRC_DIR = [src-directory]
TEST_DIR = [test-directory]

# ────────────────────────────────────────────────────────────────
# Variables
# ────────────────────────────────────────────────────────────────
#
# Overridable configuration. Use ?= for defaults that can be changed.
# Document what each controls and valid values.
#
# See: standards/code/4-block/sections/setup/CWS-SECTION-SETUP-003-variables.md

# ═══ Tool Configuration ═══
# Can be overridden: make build CC=clang

# CC ?= gcc           # C compiler (default: gcc)
# CFLAGS ?= -Wall     # Compiler flags (default: warnings enabled)

# ═══ Runtime Configuration ═══
# Passed at invocation: make run ARGS="--verbose"

ARGS ?=              # Arguments passed to run target

# ────────────────────────────────────────────────────────────────
# Pattern Rules
# ────────────────────────────────────────────────────────────────
#
# File type transformations. Define how one file type becomes another.
# These are like "type definitions" - they define transformation shapes.
#
# See: standards/code/4-block/sections/setup/CWS-SECTION-SETUP-004-types.md

# ═══ Compilation Patterns ═══
# [Reserved: Add pattern rules like %.o: %.c if needed]

# %.o: %.c
# 	$(CC) $(CFLAGS) -c $< -o $@

# ────────────────────────────────────────────────────────────────
# Default Target
# ────────────────────────────────────────────────────────────────
#
# Entry point - what happens when you run `make` with no arguments.
# Should be the most common workflow (typically build + test).
#
# See: standards/code/4-block/sections/setup/CWS-SECTION-SETUP-005-type-methods.md

# Default: build and test
all: build test

# ────────────────────────────────────────────────────────────────
# Shell Configuration
# ────────────────────────────────────────────────────────────────
#
# Environment setup. Shell selection, exports, environment variables.
# This is the "Rails attachment" - infrastructure all targets use.
#
# See: standards/code/4-block/sections/setup/CWS-SECTION-SETUP-006-package-level-state.md

# ═══ Shell Selection ═══
# Use bash for consistent behavior across platforms.
# [Reserved: Uncomment if bash-specific features needed]

# SHELL := /bin/bash
# .SHELLFLAGS := -eu -o pipefail -c

# ═══ Environment Exports ═══
# Variables exported to sub-processes.
# [Reserved: Add exports as needed]

# export PATH := $(BUILD_DIR):$(PATH)

# ============================================================================
# END SETUP
# ============================================================================

# ============================================================================
# BODY
# ============================================================================
#
# For BODY structure explanation, see: standards/code/4-block/CWS-STD-007-CODE-body-block.md
#
# Section order: Target Dependency Graph → Internal Helpers → Build Operations → Error Handling → User-Facing Targets
# This flows: map structure → foundations → categorized operations → safety patterns → public interface
#
# Universal mapping (see standards for cross-language patterns):
#   Target Dependency Graph ≈ Organizational Chart (internal structure map)
#   Internal Helper Targets ≈ Helpers/Utilities (foundations)
#   Build Operations ≈ Core Operations (categorized business logic)
#   Error Handling Recipes ≈ Error Handling/Recovery Patterns
#   User-Facing Targets ≈ Public APIs (exported interface)

# ────────────────────────────────────────────────────────────────
# Target Dependency Graph - Build Order
# ────────────────────────────────────────────────────────────────
#
# Maps target dependencies and execution flow within this Makefile.
# Provides navigation for both development (what targets exist) and
# maintenance (what depends on this target).
#
# See: standards/code/4-block/sections/body/CWS-SECTION-BODY-001-organizational-chart.md
#
# Ladder Structure (Dependencies):
#
#   User-Facing Targets (Top Rungs - Primary Interface)
#   ├── all → build, test
#   ├── check → fmt, vet, test
#   └── help → (standalone)
#
#   Build Operations (Middle Rungs - Core Functionality)
#   ├── build → _ensure-dirs
#   ├── test → (standalone)
#   ├── run → build
#   └── clean → (standalone)
#
#   Internal Helpers (Bottom Rungs - Foundations)
#   ├── _ensure-dirs → (standalone)
#   └── _check-tools → (standalone)
#
# Baton Flow (Execution Paths):
#
#   Entry → make (no args)
#     ↓
#   all (default target)
#     ↓
#   build → _ensure-dirs
#     ↓
#   test
#     ↓
#   Exit → success/failure
#
# Target Summary:
# - [X] targets total
# - [X] internal helpers (foundation targets)
# - [X] build operations (core functionality)
# - [X] user-facing targets (primary interface)

# ────────────────────────────────────────────────────────────────
# Internal Helper Targets - Support Operations
# ────────────────────────────────────────────────────────────────
#
# Foundation targets used by other targets. Bottom rungs of the ladder -
# simple, focused, reusable utilities. Usually prefixed with underscore
# to indicate internal use, not listed in help.
#
# See: standards/code/4-block/sections/body/CWS-SECTION-BODY-002-helpers.md
#
# Key characteristics:
# - NOT in .PHONY declarations at top (unless needed)
# - NOT documented in help target
# - Prefixed with underscore (_) by convention
# - Simple, single-purpose operations
# - Reusable by multiple targets

# _ensure-dirs creates required directories.
#
# What It Does:
# Creates build output directories if they don't exist.
# Silent operation - only shows output on error.
#
# Used By:
#   build (ensures output directory exists before compilation)
#
_ensure-dirs:
	@mkdir -p $(BUILD_DIR)

# _check-tools verifies required tools are installed.
#
# What It Does:
# Checks that required build tools are available in PATH.
# Fails with helpful message if tools missing.
#
# Used By:
#   build, test (ensures environment is ready)
#
# _check-tools:
# 	@which [tool] > /dev/null || (echo "Error: [tool] not found" && exit 1)

# ────────────────────────────────────────────────────────────────
# Build Operations - Core Functionality
# ────────────────────────────────────────────────────────────────
#
# Core build operations implementing the Makefile's primary purpose.
# Organized by operational categories (descriptive subsections) below.
#
# See: standards/code/4-block/sections/body/CWS-SECTION-BODY-003-core-operations.md

# ────────────────────────────────────────────────────────────────
# Compilation - Binary Building
# ────────────────────────────────────────────────────────────────
#
# What These Do:
# Transform source code into executable binaries.
#
# Why Separated:
# Compilation is the core purpose - central to all build workflows.
#
# Extension Point:
# To add new build variants, create target following [name]-build pattern.
# Each build target should use _ensure-dirs prerequisite.

## build: Build the [binary] binary
build: _ensure-dirs
	@echo "Building $(BINARY_NAME)..."
	@# [Build command - language specific]
	@# go build -o $(BUILD_DIR)/$(BINARY_NAME) ./cmd/[name]
	@# gcc $(CFLAGS) -o $(BUILD_DIR)/$(BINARY_NAME) $(SRC_DIR)/*.c
	@# cargo build --release
	@echo "✓ Build complete: $(BUILD_DIR)/$(BINARY_NAME)"

# ────────────────────────────────────────────────────────────────
# Testing - Verification
# ────────────────────────────────────────────────────────────────
#
# What These Do:
# Run tests to verify correctness and quality.
#
# Why Separated:
# Testing is distinct from building - verifies what was built.
#
# Extension Point:
# To add new test types, create target following test-[type] pattern.
# Consider: test-unit, test-integration, test-e2e, test-cover.

## test: Run all tests
test:
	@echo "Running tests..."
	@# [Test command - language specific]
	@# go test -v ./...
	@# pytest -v
	@# cargo test
	@echo "✓ Tests complete"

## test-cover: Run tests with coverage
# test-cover:
# 	@echo "Running tests with coverage..."
# 	@# [Coverage command]
# 	@echo "✓ Coverage complete"

# ────────────────────────────────────────────────────────────────
# Quality - Code Standards
# ────────────────────────────────────────────────────────────────
#
# What These Do:
# Enforce code quality standards (formatting, linting, static analysis).
#
# Why Separated:
# Quality checks are pre-commit gates - run before other operations.
#
# Extension Point:
# To add new quality checks, create target and add to 'check' prerequisites.
# Pattern: [tool]: runs the tool, check: aggregates all quality targets.

## fmt: Format code
# fmt:
# 	@echo "Formatting code..."
# 	@# [Format command]
# 	@echo "✓ Format complete"

## vet: Run static analysis
# vet:
# 	@echo "Running static analysis..."
# 	@# [Analysis command]
# 	@echo "✓ Analysis complete"

## lint: Run linter
# lint:
# 	@echo "Running linter..."
# 	@# [Lint command]
# 	@echo "✓ Lint complete"

# ────────────────────────────────────────────────────────────────
# Execution - Running
# ────────────────────────────────────────────────────────────────
#
# What These Do:
# Execute the built binary with various configurations.
#
# Why Separated:
# Running is distinct from building - uses what was built.
#
# Extension Point:
# To add new run modes, create target following run-[mode] pattern.
# Pass arguments via ARGS variable: make run ARGS="--flag value"

## run: Run the binary (use ARGS for arguments)
run: build
	@if [ -z "$(ARGS)" ]; then \
		echo "Running $(BINARY_NAME)..."; \
		./$(BUILD_DIR)/$(BINARY_NAME); \
	else \
		echo "Running $(BINARY_NAME) $(ARGS)..."; \
		./$(BUILD_DIR)/$(BINARY_NAME) $(ARGS); \
	fi

# ────────────────────────────────────────────────────────────────
# Maintenance - Cleanup
# ────────────────────────────────────────────────────────────────
#
# What These Do:
# Remove generated files and reset to clean state.
#
# Why Separated:
# Cleanup is destructive - intentionally separated from build operations.
#
# Extension Point:
# Add cleanup commands to 'clean' target. Be thorough but careful -
# never remove source files or version control.

## clean: Remove build artifacts
clean:
	@echo "Cleaning..."
	@rm -rf $(BUILD_DIR)
	@# [Additional cleanup - language specific]
	@# go clean
	@# cargo clean
	@# rm -f *.o
	@echo "✓ Clean complete"

# ────────────────────────────────────────────────────────────────
# Error Handling - Recipe Patterns
# ────────────────────────────────────────────────────────────────
#
# Common error handling patterns for Makefile recipes.
# These patterns ensure graceful failure and helpful error messages.
#
# See: standards/code/4-block/sections/body/CWS-SECTION-BODY-004-error-handling.md
#
# Pattern: Continue on failure (||)
#   @command || echo "Warning: command failed"
#
# Pattern: Stop on failure (&&)
#   @command1 && command2 && command3
#
# Pattern: Check prerequisites
#   @which tool > /dev/null || (echo "Error: tool not found" && exit 1)
#
# Pattern: Conditional execution
#   @if [ -f file ]; then command; fi
#
# Pattern: Safe directory operations
#   @mkdir -p dir          # Create if not exists (no error if exists)
#   @rm -rf dir            # Remove recursively (no error if not exists)
#
# Pattern: Verbose error messages
#   @command 2>&1 || (echo "Error: operation failed"; exit 1)
#
# Note: Recipes prefixed with @ suppress command echo.
# Note: Recipes prefixed with - continue on error (not recommended).

# ────────────────────────────────────────────────────────────────
# User-Facing Targets - Public Interface
# ────────────────────────────────────────────────────────────────
#
# Primary targets for user interaction. Top rungs of the ladder -
# orchestrate build operations into complete workflows.
# These ARE in .PHONY, ARE documented in help.
#
# See: standards/code/4-block/sections/body/CWS-SECTION-BODY-005-public-apis.md
#
# Key characteristics:
# - Listed in .PHONY declarations
# - Documented with ## comments for help extraction
# - Orchestrate lower targets (don't duplicate logic)
# - Represent complete workflows

## check: Run all quality checks (fmt, vet, test)
# check: fmt vet test
# 	@echo "✓ All checks passed"

## help: Show available targets
help:
	@echo "[Project Name] - Makefile Commands"
	@echo ""
	@echo "Usage: make [target]"
	@echo ""
	@echo "Targets:"
	@sed -n 's/^## //p' Makefile | column -t -s ':' | sed -e 's/^/  /'
	@echo ""
	@echo "Examples:"
	@echo "  make build              # Build binary"
	@echo "  make test               # Run tests"
	@echo "  make run                # Run binary"
	@echo "  make run ARGS=\"--flag\"  # Run with arguments"
	@echo "  make clean              # Remove artifacts"

## info: Show build configuration
info:
	@echo "Build Configuration:"
	@echo "  BINARY_NAME: $(BINARY_NAME)"
	@echo "  BUILD_DIR:   $(BUILD_DIR)"
	@echo "  SRC_DIR:     $(SRC_DIR)"
	@# @echo "  CC:          $(CC)"
	@# @echo "  CFLAGS:      $(CFLAGS)"

# ============================================================================
# END BODY
# ============================================================================

# ============================================================================
# CLOSING
# ============================================================================
#
# For CLOSING structure explanation, see: standards/code/4-block/CWS-STD-008-CODE-closing-block.md
#
# Section order: Makefile Validation → Makefile Execution → Makefile Cleanup → Final Documentation
# This flows: verify correctness → entry points → resource management → synthesis
#
# Universal mapping (see standards for cross-language patterns):
#   Makefile Validation ≈ Code Validation (testing/verification)
#   Makefile Execution ≈ Code Execution (entry points and flow)
#   Makefile Cleanup ≈ Code Cleanup (resource management)
#   Final Documentation = Same across all languages

# ────────────────────────────────────────────────────────────────
# Makefile Validation: [project-name] (Build System)
# ────────────────────────────────────────────────────────────────
#
# For Code Validation section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-001-code-validation.md
#
# Testing Requirements:
#   - Verify all targets execute without errors
#   - Confirm dependencies resolve correctly
#   - Test with clean environment (make clean && make all)
#   - Verify PHONY targets don't conflict with file names
#   - Check help target produces expected output
#   - Run: make help (verify all documented targets appear)
#
# Syntax Verification:
#   - make -n [target] (dry-run shows commands without executing)
#   - make --warn-undefined-variables (catch undefined variable usage)
#   - make -p (print database to verify target/variable definitions)
#
# Build Verification:
#   - make all (complete build succeeds)
#   - make clean && make build (fresh build works)
#   - make test (all tests pass if applicable)
#
# Integration Testing:
#   - Test from project root directory
#   - Verify paths work with different working directories
#   - Test with different shell environments if applicable
#   - Confirm CI/CD integration works
#
# Example validation sequence:
#
#   # Syntax check (dry-run)
#   make -n all
#
#   # Clean build test
#   make clean && make all
#
#   # Help documentation check
#   make help
#
#   # Individual target verification
#   make build
#   make test
#   make run ARGS="--test"

# ────────────────────────────────────────────────────────────────
# Makefile Execution: [project-name] (Build System)
# ────────────────────────────────────────────────────────────────
#
# For Code Execution section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-002-code-execution.md
#
# This is a BUILD SYSTEM. It orchestrates build operations via make targets.
# Entry points are make targets, not functions. Default target runs when
# make is invoked without arguments.
#
# Entry Point: `make` (runs default target: all)
#
# Execution Flow:
#   1. User invokes `make [target]` or just `make`
#   2. Make parses Makefile, resolves target dependencies
#   3. Prerequisites execute first (bottom-up dependency resolution)
#   4. Target recipe executes
#   5. Health status echoed (✓/✗ messages)
#   6. Exit code propagates (0=success, non-zero=failure)
#
# Available Entry Points:
#   make                  # Default: runs 'all' target
#   make all              # Build and test
#   make build            # Build only
#   make test             # Test only
#   make run              # Build and run
#   make run ARGS="..."   # Run with arguments
#   make clean            # Remove build artifacts
#   make help             # Show available targets
#   make info             # Show build configuration
#
# Dependency Resolution:
#   make all → build → _ensure-dirs
#           → test
#
# Example invocation:
#
#   # Default workflow
#   make
#
#   # Specific target with variable override
#   make build BINARY_NAME=custom-name
#
#   # Run with arguments
#   make run ARGS="--verbose --config=test.yaml"

# ────────────────────────────────────────────────────────────────
# Makefile Cleanup: [project-name] (Build System)
# ────────────────────────────────────────────────────────────────
#
# For Code Cleanup section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-003-code-cleanup.md
#
# Resource Management:
#   - Build artifacts: Removed by `make clean`
#   - Temporary files: Removed by `make clean`
#   - Object files: Removed by `make clean` (if applicable)
#   - Cache directories: [Preserved/Removed - specify behavior]
#
# Clean Target Scope:
#   - $(BUILD_DIR): Removed entirely (bin/ directory)
#   - [Generated files]: [How handled]
#   - Source files: NEVER removed (safety boundary)
#   - Version control: NEVER touched (.git/)
#
# Error State Cleanup:
#   - Partial builds: `make clean` resets to known state
#   - Failed targets: Re-run after fixing issues
#   - Interrupted builds: `make clean && make all` for fresh start
#
# Safe Cleanup Patterns:
#   - rm -rf $(BUILD_DIR) - Safe (only removes build output)
#   - rm -f *.o - Safe (only removes object files)
#   - NEVER: rm -rf * or rm -rf / (catastrophic)
#   - NEVER: rm -rf $(SRC_DIR) (removes source code)
#
# Example cleanup sequence:
#
#   # Standard cleanup
#   make clean
#
#   # Full reset (clean + rebuild)
#   make clean && make all
#
#   # Verify clean state
#   ls -la $(BUILD_DIR)  # Should not exist or be empty

# ════════════════════════════════════════════════════════════════
# FINAL DOCUMENTATION
# ════════════════════════════════════════════════════════════════

# ────────────────────────────────────────────────────────────────
# Build System Overview & Integration Summary
# ────────────────────────────────────────────────────────────────
#
# For Library Overview section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-004-library-overview.md
#
# Purpose: See METADATA "Purpose & Function" section above
#
# Provides: See METADATA "Key Features" list above for comprehensive capabilities
#
# Quick summary (high-level only - details in METADATA):
#   - [1-2 sentence overview of what this build system does]
#   - Orchestrates [build/test/run] operations for [project type]
#
# Integration Pattern: See METADATA "Usage & Integration" section above for
# complete step-by-step integration guide
#
# Public API (Targets): See METADATA "Usage & Integration" section above for
# complete target list organized by category in typical usage order
#
# Architecture: See METADATA "CPI-SI Identity" section above for complete
# architectural role (Rails/Ladder/Baton) explanation

# ────────────────────────────────────────────────────────────────
# Modification Policy
# ────────────────────────────────────────────────────────────────
#
# For Modification Policy section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-005-modification-policy.md
#
# Safe to Modify (Extension Points):
#   ✅ Add new targets (follow ## comment convention for help)
#   ✅ Add new internal helpers (use _ prefix)
#   ✅ Extend clean target (add more cleanup commands)
#   ✅ Add build variants (build-debug, build-release, etc.)
#   ✅ Add test variants (test-unit, test-integration, etc.)
#
# Modify with Extreme Care (Breaking Changes):
#   ⚠️ Default target (all) - affects `make` with no args
#   ⚠️ Target names - breaks scripts/CI depending on them
#   ⚠️ Variable names - breaks command-line overrides
#   ⚠️ Build output paths - breaks deployment/packaging
#   ⚠️ clean target scope - may remove more/less than expected
#
# NEVER Modify (Foundational Structure):
#   ❌ 4-block structure (METADATA, SETUP, BODY, CLOSING)
#   ❌ Remove clean target (safety mechanism)
#   ❌ Remove help target (documentation mechanism)
#   ❌ Add recursive rm without constraints
#   ❌ Hardcode absolute paths (portability)
#
# Validation After Modifications:
#   See "Makefile Validation" section above for comprehensive testing
#   requirements, syntax verification, and integration testing procedures.

# ────────────────────────────────────────────────────────────────
# Target Dependency Flow (Ladder and Baton)
# ────────────────────────────────────────────────────────────────
#
# For Ladder and Baton Flow section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-006-ladder-baton-flow.md
#
# See BODY "Target Dependency Graph - Build Order" section above for
# complete ladder structure (dependencies) and baton flow (execution paths).
#
# The Target Dependency Graph in BODY provides the detailed map showing:
# - All targets and their prerequisites (ladder)
# - Complete execution flow paths (baton)
# - Target count and categorization
#
# Quick architectural summary (details in BODY Target Dependency Graph):
# - [X] user-facing targets orchestrate [Y] build operations using [Z] helpers
# - Ladder: User targets → Build operations → Internal helpers
# - Baton: make → all → build → _ensure-dirs → test → exit

# ────────────────────────────────────────────────────────────────
# Surgical Update Points (Extension Guide)
# ────────────────────────────────────────────────────────────────
#
# For Surgical Update Points section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-007-surgical-update-points.md
#
# See BODY "Build Operations" subsection header comments above for detailed
# extension points. Each subsection includes "Extension Point" guidance showing:
# - Where to add new targets
# - What naming pattern to follow
# - How to integrate with existing targets
# - What documentation to update (## comments for help)
#
# Quick reference (details in BODY subsection comments):
# - Adding build variants: See BODY "Compilation" extension point
# - Adding test types: See BODY "Testing" extension point
# - Adding quality checks: See BODY "Quality" extension point
# - Adding run modes: See BODY "Execution" extension point
# - Adding cleanup commands: See BODY "Maintenance" extension point
# - Adding internal helpers: See BODY "Internal Helper Targets" section

# ────────────────────────────────────────────────────────────────
# Performance Considerations
# ────────────────────────────────────────────────────────────────
#
# For Performance Considerations section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-008-performance-considerations.md
#
# See SETUP section above for build performance characteristics:
# - Constants: Paths and names affecting build output
# - Variables: Configurable options affecting build behavior
#
# Quick summary:
# - Parallel builds: Use `make -j$(nproc)` for parallel execution
# - Incremental builds: Make only rebuilds changed dependencies
# - Clean builds: `make clean && make` forces full rebuild
# - Dry runs: `make -n` shows commands without executing (fast)
#
# Key optimization tips:
# - Order prerequisites by execution time (longest first for parallelism)
# - Use order-only prerequisites (|) when dependency doesn't require rebuild
# - Minimize recipe shell invocations (combine commands with &&)
# - Use @ prefix to suppress command echo (reduces output overhead)

# ────────────────────────────────────────────────────────────────
# Troubleshooting Guide
# ────────────────────────────────────────────────────────────────
#
# For Troubleshooting Guide section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-009-troubleshooting-guide.md
#
# Problem: "make: *** No rule to make target 'X'"
#   Check: Target exists in Makefile
#   Check: Target spelling matches exactly (case-sensitive)
#   Check: No typo in prerequisite names
#   Solution: Add missing target or fix spelling
#
# Problem: "make: 'X' is up to date"
#   Expected: Target has no changed prerequisites
#   Solution: `make clean && make X` for forced rebuild
#   Solution: Touch source files to trigger rebuild
#
# Problem: Target runs but command fails
#   Check: Required tools are installed (which tool)
#   Check: Paths are correct for current directory
#   Check: Variables are set correctly (make info)
#   Solution: Install missing tools, fix paths, set variables
#
# Problem: "*** missing separator"
#   Cause: Recipe lines must start with TAB, not spaces
#   Solution: Convert leading spaces to TAB character
#
# Problem: Variable not expanding
#   Check: Use $(VAR) not $VAR for multi-char names
#   Check: Variable is defined before use
#   Solution: Fix syntax or define variable
#
# Problem: Help shows wrong/missing targets
#   Check: ## comments have correct format
#   Check: sed command in help target is correct
#   Solution: Add/fix ## comments on target lines

# ────────────────────────────────────────────────────────────────
# Related Components & Dependencies
# ────────────────────────────────────────────────────────────────
#
# For Related Components section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-010-related-components.md
#
# See METADATA "Dependencies" section above for complete dependency information:
# - Dependencies (What This Needs): System tools, language toolchain, external tools
# - Dependents (What Uses This): Developers, CI/CD pipelines
# - Integration Points: How scripts and automation connect
#
# Quick summary (details in METADATA Dependencies section above):
# - Key dependencies: make, [language toolchain]
# - Primary consumers: Developers, CI/CD
#
# Parallel Implementation (if applicable):
#   - Other build systems: [CMake/Meson/Ninja equivalents if any]
#   - CI configuration: [.github/workflows/, .gitlab-ci.yml, etc.]
#   - Shared philosophy: 4-block structure, health tracking, clear targets

# ────────────────────────────────────────────────────────────────
# Future Expansions & Roadmap
# ────────────────────────────────────────────────────────────────
#
# For Future Expansions section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-011-future-expansions.md
#
# Planned Features:
#   ⏳ [Additional build targets]
#   ⏳ [CI/CD integration targets]
#   ⏳ [Documentation generation]
#   ⏳ [Release/packaging automation]
#
# Research Areas:
#   - [Cross-platform compatibility]
#   - [Parallel build optimization]
#   - [Containerized builds]
#
# Integration Targets:
#   - [CI/CD pipeline integration]
#   - [IDE integration]
#   - [Container/Docker builds]
#
# Known Limitations to Address:
#   - [Current limitation 1]
#   - [Current limitation 2]
#
# Version History:
#
# See METADATA "Authorship & Lineage" section above for brief version changelog.
# Comprehensive version history with full context:
#
#   [X.Y.Z] ([Date]) - [Initial version]
#         - [Core targets implemented]
#         - [4-block structure established]

# ────────────────────────────────────────────────────────────────
# Closing Note
# ────────────────────────────────────────────────────────────────
#
# For Closing Note section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-012-closing-note.md
#
# This Makefile is the BUILD ORCHESTRATION LAYER for [project-name].
# It coordinates compilation, testing, and execution through simple commands.
#
# Modify thoughtfully - changes here affect all build workflows.
# Maintain the target naming conventions and help documentation.
#
# For questions, issues, or contributions:
#   - Review the modification policy above
#   - Follow the 4-block structure pattern
#   - Test all targets before committing (make clean && make all)
#   - Document new targets with ## comments
#   - Update Target Dependency Graph when adding targets
#
# "[Relevant Scripture verse]" - [Reference]

# ────────────────────────────────────────────────────────────────
# Quick Reference: Usage Examples
# ────────────────────────────────────────────────────────────────
#
# For Quick Reference section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-013-quick-reference.md
#
# Basic Usage:
#   make              # Build and test (default)
#   make build        # Build only
#   make test         # Run tests
#   make clean        # Remove artifacts
#   make help         # Show all targets
#
# Development Workflow:
#   make build && make run          # Build and run
#   make run ARGS="--verbose"       # Run with arguments
#   make clean && make all          # Fresh build
#
# CI/CD Integration:
#   make all          # Complete build pipeline
#   make test         # Test phase
#   make clean        # Cleanup phase
#
# Debugging:
#   make -n all       # Dry-run (show commands)
#   make info         # Show configuration
#   make -p           # Print make database
#
# Configuration Override:
#   make build BINARY_NAME=custom   # Override variable
#   CC=clang make build             # Override environment

# ============================================================================
# END CLOSING
# ============================================================================

