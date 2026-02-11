# .PHONY: __TEMPLATE__  # TEMPLATE: Remove this line when ready to use
# #!omni template --makefile

# ═══════════════════════════════════════════════════════════════════════════════
# OMNICODE PRAGMA [PRAGMA]
# ═══════════════════════════════════════════════════════════════════════════════
#
# --- P1: Core ---
# key:       B-seed-code-make-game-project
# from:      B-seed-code-make-template
# at:        b-01.00
#
# --- P2: Family ---
# type:      template
# subtype:   makefile
# role:      game-project
# structure: 4-block
#
# --- P3: Instance ---
# component: build-system
# layer:     seed/code/make
# provides:  MAKEFILE_GAME_PROJECT
#
# --- P5: Summary ---
# title:     Game Project Makefile Template
# brief:     4-block build template for Cornerstone/game projects
#
# TEMPLATE: Project Makefile (4-Block Structure)
# STRUCTURE: METADATA → SETUP → BODY → CLOSING
# USAGE: cp makefile.mk Makefile → update pragma → fill sections
#
# ═══════════════════════════════════════════════════════════════════════════════
# END PRAGMA [END]
# ═══════════════════════════════════════════════════════════════════════════════

# ═══════════════════════════════════════════════════════════════════════════════
# METADATA BLOCK [METADATA]
# ═══════════════════════════════════════════════════════════════════════════════
#
# 4-Block Code Structure: Identity and context for this component
#
# Section order: Identity → State → Attribution → Location → Derivation → Classification → Intent → Grounding → Dependencies → Roadmap
# Flow: who → when → by whom → where → from what → tagged as → why → grounded in → needs → going where
#
# ═══════════════════════════════════════════════════════════════════════════════

# # M.1 Core Identity [IDENTITY]
#
#   Key:       [PROJECT-BUILD-###] — Unique identifier
#   Title:     [Title] — What this file is
#   Type:      Build
#   Component: [Ladder/Baton/Rails] — Architectural role
#   Role:      [Specific responsibility in system]
#
# Key: B-word-seed-code-make-makefile
# Title: Project Makefile
# Type: Build
# Component: Rails
# Role: Orchestrates compilation and build automation

# # M.2 Lifecycle State [STATE]
#
#   Status:   Draft | Active | Deprecated | Archived
#   Version:  a-XX.XX — Living versioning
#   Created:  YYYY-MM-DD
#   Updated:  YYYY-MM-DD
#
# Status: Active
# Version: a-01.00
# Created: 2026-01-02
# Updated: 2026-01-03

# # M.3 Attribution [ATTRIBUTION]
#
#   Authors:      [Name (Role)]
#   Organization: [Company]
#   Copyright:    [© YYYY Company]
#
# Authors: Seanje Lenox-Wise (Architect), Nova Dawn (Author)
# Organization: CreativeWorkzStudio LLC
# Copyright: © 2026 CreativeWorkzStudio LLC. All rights reserved.

# # M.4 Location [LOCATION]
#
#   Path:      [/path/to/file]
#
# Path: cornerstone/templates/code/build/makefile.mk

# # M.5 Derivation [DERIVATION]
#
#   Derives_from:  [path/to/parent/template] — file template copied from
#
#   (Note: pragma meta.from = OmniCode spec; Derives_from = file lineage)
#
# Derives_from: Bereshit/word/seed/code/make/makefile.mk

# # M.6 Classification [CLASSIFICATION]
#
#   Tags:      [build, makefile, automation] — discovery
#
# Tags: build, makefile, automation

# # M.7 Intent [INTENT]
#
#   Purpose:     [Enables X for Y] — one-line purpose statement
#   Core Design: [Architectural pattern or paradigm]
#   Philosophy:  [Guiding principle for how this works]
#
#   Key Features:
#     - [What it provides — major capabilities]
#
# Purpose: Orchestrate build process — compile, test, clean, deploy
# Core Design: Target-based dependency resolution
# Philosophy: Explicit is better than implicit — declare dependencies, let make resolve order

# # M.8 Grounding [GROUNDING]
#
#   Scripture:  [Book Chapter:Verse] — grounding verse for this component
#   Principle:  [Kingdom principle demonstrated]
#   Anchor:     [Supporting verse] — reinforcing principle (optional)
#
# Scripture: Nehemiah 4:6 — The people had a mind to work
# Principle: Organized labor builds the wall — structured build process
# Anchor: Ecclesiastes 3:3 — A time to build
#
# Example:
#   Scripture: Colossians 3:23 — Do it heartily, as to the Lord
#   Principle: Excellence in build orchestration as worship
#   Anchor: Proverbs 22:29 — Skilled work stands before kings

# # M.9 Dependencies [DEPENDENCIES]
#
#   Purpose:  Relationship context — what this needs, what uses it
#   Contains: What This Needs, What Uses This, Integration Points, Usage
#
#   What This Needs:
#     System Tools: [make, gcc, etc.]
#     External: [None | tool with version]
#     Internal: [project files this depends on]
#
#   What Uses This:
#     Commands: [executables built by this]
#     Libraries: [libraries built by this]
#     Scripts: [scripts that invoke this]
#
#   Integration Points:
#     - [How other systems connect — Rails/Ladder/Baton mechanism]
#     - [Cross-component interactions]
#     - [Build pipeline integration]
#
#   Usage:
#     Command Line:
#       make [target]         Build specified target
#       make                  Build default target
#       make help             Show available targets
#
#     Exit Codes:
#       0  - Success
#       1  - Build error
#       2  - Missing dependency
#
# What This Needs:
#   System Tools: make, shell
#   External: None
#   Internal: None — self-contained template
#
# What Uses This:
#   Commands: None
#   Libraries: None
#   Scripts: cp (template instantiation)
#
# Integration Points:
#   - Template system in bereshit/word/seed/
#   - 4-block structure for Makefiles
#
# Cross-Format References:
#   toml:         None
#   json:         None
#   editorconfig: compiler-project.editorconfig — game project formatting
#   makefile:     template.mk (parent template)
#   relationship: Makefile defines build rules; EditorConfig defines formatting rules
#
# Usage: N/A — this is a template, not an executable

# # M.10 Roadmap [ROADMAP]
#
#   Purpose:  Vision and trajectory — where this is going
#   Contains: Planned, Limitations, Research, Operational
#
#   Planned:
#     ⏳ [Planned feature 1]
#     ⏳ [Planned feature 2]
#
#   Limitations:
#     - [Known limitation 1]
#
#   Research:
#     - [Research direction 1]
#
#   Operational (for components with health tracking):
#     Blocking: [Blocking/Non-blocking] — [Brief explanation]
#     Mitigation: [How blocking/failures handled]
#
#     Health Scoring:
#       System: Base100 with 1-point granular scale (-100 to +100)
#       States: Granted (>+50), Deferred (±50), Denied (<-50)
#
#       [Operation Category]:
#         - [Specific operation]: ±X points
#         - [Another operation]: ±Y points
#
# Planned:
#   ⏳ Align with bereshit/word/seed/ canonical templates
#
# Limitations:
#   - Currently project-local, not yet in bereshit
#
# Research:
#   - Cross-platform make patterns (Windows nmake compatibility)
#
# Operational: N/A — template, not runtime component

# ═══════════════════════════════════════════════════════════════════════════════
# END METADATA [END]
# ═══════════════════════════════════════════════════════════════════════════════

# ═══════════════════════════════════════════════════════════════════════════════
# SETUP BLOCK [SETUP]
# ═══════════════════════════════════════════════════════════════════════════════
#
# 4-Block Code Structure: Configuration and declarations before body
#
# Section order: Declarations → Constants → Variables → Patterns → Default → Shell → Omission
# Flow: what exists → fixed config → dynamic config → transformations → entry → environment → guidance
#
# ═══════════════════════════════════════════════════════════════════════════════

# ───────────────────────────────────────────────────────────────────────────────
# CODE SECTIONS
# ───────────────────────────────────────────────────────────────────────────────

# # S.1 Declarations [DECLARATIONS]
#
#   Purpose:  Declare targets and includes
#   Contains: PHONY Targets, Include Directives

# # S.1a PHONY Targets [PHONY]
#
#   .PHONY: all build test clean run help
#   .PHONY: fmt lint check                   # quality targets
#   .PHONY: install uninstall                # installation targets
#   .PHONY: debug release                    # build variants

.PHONY: all build test clean run help

# # S.1b Include Directives [INCLUDE]
#
#   include config.mk                        # shared configuration
#   include $(BUILD_DIR)/deps.mk             # generated dependencies
#   -include local.mk                        # optional local overrides (- = silent fail)

# [Reserved: Currently none - self-contained Makefile]
# include [path/to/shared.mk]

# # S.2 Constants [CONSTANTS]
#
#   Purpose:  Fixed values that never change (not overridable)
#   Contains: Project Constants, Path Constants
#
#   Project Constants:
#     BINARY_NAME  — output binary name
#     VERSION      — project version
#     PREFIX       — installation prefix
#
#   Path Constants:
#     SRC_DIR      — source files
#     TEST_DIR     — test files
#     INCLUDE_DIR  — header files
#     BUILD_DIR    — build output

# # S.2a Project Constants [PROJECT]
#
#   BINARY_NAME = [name]             # output binary name
#   VERSION = 1.0.0                  # project version
#   PREFIX = /usr/local              # installation prefix

BINARY_NAME = [binary-name]
BUILD_DIR = bin

# # S.2b Path Constants [PATHS]
#
#   SRC_DIR = src                    # source files
#   TEST_DIR = tests                 # test files
#   INCLUDE_DIR = include            # header files
#   LIB_DIR = lib                    # library files

SRC_DIR = [src-directory]
TEST_DIR = [test-directory]

# # S.3 Variables [VARIABLES]
#
#   Purpose:  Configurable values (can be overridden: make build CC=clang)
#   Contains: Tool Config, Runtime Config
#
#   Tool Config:
#     CC       — C compiler (gcc, clang)
#     CXX      — C++ compiler (g++, clang++)
#     CFLAGS   — C compiler flags
#     LDFLAGS  — linker flags
#
#   Runtime Config:
#     ARGS     — arguments passed to run target
#     VERBOSE  — verbose output (0/1)
#     DEBUG    — debug mode (0/1)

# # S.3a Tool Config [TOOLS]
#
#   CC ?= gcc                        # C compiler
#   CXX ?= g++                       # C++ compiler
#   CFLAGS ?= -Wall -Wextra          # C flags
#   CXXFLAGS ?= -Wall -Wextra        # C++ flags
#   LDFLAGS ?= -lm                   # linker flags

# [Reserved: Uncomment if C/C++ compilation needed]
# CC ?= gcc
# CFLAGS ?= -Wall

# # S.3b Runtime Config [RUNTIME]
#
#   ARGS ?=                          # arguments passed to run target
#   VERBOSE ?= 0                     # verbose output
#   DEBUG ?= 0                       # debug mode

ARGS ?=

# # S.4 Pattern Rules [PATTERNS]
#
#   Purpose:  File type transformations (how to build file types)
#   Contains: Compilation, Linking
#
#   Compilation:
#     %.o: %.c     — compile C source to object
#     %.o: %.cpp   — compile C++ source to object
#
#   Linking:
#     $(TARGET): $(OBJS)  — link objects to binary

# # S.4a Compilation [COMPILATION]
#
#   %.o: %.c
#   	$(CC) $(CFLAGS) -c $< -o $@
#
#   %.o: %.cpp
#   	$(CXX) $(CXXFLAGS) -c $< -o $@

# [Reserved: Uncomment if object file compilation needed]
# %.o: %.c
# 	$(CC) $(CFLAGS) -c $< -o $@

# # S.4b Linking [LINKING]
#
#   $(BUILD_DIR)/$(BINARY_NAME): $(OBJS)
#   	$(CC) $(LDFLAGS) -o $@ $^

# [Reserved: Uncomment if explicit linking rule needed]

# # S.5 Default Target [DEFAULT]
#
#   Purpose:  Entry point when running 'make' with no arguments
#   Contains: Default target declaration
#
#   all: build test                  # build + test
#   all: build                       # build only
#   all: test                        # test only

all: build test

# # S.6 Shell Configuration [SHELL]
#
#   Purpose:  Environment setup for recipe execution
#   Contains: Shell Selection, Environment Exports
#
#   Shell Selection:
#     SHELL := /bin/bash               — use bash for recipes
#     .SHELLFLAGS := -eu -o pipefail -c  — strict bash mode
#
#   Environment Exports:
#     export PATH      — extend PATH for recipe execution
#     export CC        — share compiler with subprocesses

# # S.6a Shell Selection [SHELL_SELECT]
#
#   SHELL := /bin/bash               # use bash for recipes
#   .SHELLFLAGS := -eu -o pipefail -c  # strict bash mode

# [Reserved: Uncomment if bash-specific features needed]
# SHELL := /bin/bash
# .SHELLFLAGS := -eu -o pipefail -c

# # S.6b Environment Exports [EXPORTS]
#
#   export PATH := $(BUILD_DIR):$(PATH)
#   export CC
#   export CFLAGS

# [Reserved: Add exports as needed]
# export PATH := $(BUILD_DIR):$(PATH)

# # S.7 Omission Guide [OMISSION]
#
#   Purpose:  What can be reserved and why
#
#   All sections must be present. Empty sections use: [Reserved: reason]
#   Omit subsection content, never the subsection header itself.

# ═══════════════════════════════════════════════════════════════════════════════
# END SETUP [END]
# ═══════════════════════════════════════════════════════════════════════════════

# ═══════════════════════════════════════════════════════════════════════════════
# BODY BLOCK [BODY]
# ═══════════════════════════════════════════════════════════════════════════════
#
# 4-Block Code Structure: The actual work — targets and recipes
#
# Section order: Graph → Helpers → Operations → Errors → API → Omission
# Flow: map structure → foundations → categorized work → safety → public interface → guidance
#
# ═══════════════════════════════════════════════════════════════════════════════

# # B.1 Dependency Graph [GRAPH]
#
#   Purpose:  Map target dependencies and execution flow
#   Contains: Ladder Structure, Baton Flow, Target Summary
#
#   Ladder Structure:
#     User-Facing (top) / Build Ops (middle) / Helpers (bottom)
#
#   Baton Flow:
#     Execution path from entry to exit
#
#   Target Summary:
#     Count of targets by category

# # B.1a Ladder Structure [LADDER]

# User-Facing (top):    all → build, test | help → standalone
# Build Ops (middle):   build → _ensure-dirs | test, clean → standalone
# Helpers (bottom):     _ensure-dirs, _check-tools → standalone

# # B.1b Baton Flow [BATON]

# make → all → build → _ensure-dirs → test → exit

# # B.1c Target Summary [SUMMARY]

# User-Facing:  [X] targets
# Build Ops:    [X] targets
# Helpers:      [X] targets

# # B.2 Internal Helpers [HELPERS]
#
#   Purpose:  Foundation targets used by others (prefixed with _)
#   Contains: Directory Setup, Tool Checks
#
#   Directory Setup:
#     Create build directories, clean artifacts
#
#   Tool Checks:
#     Verify required tools are available

# # B.2a Directory Setup [DIRS]
#
#   _ensure-dirs:
#   	@mkdir -p $(BUILD_DIR)
#   	@mkdir -p $(BUILD_DIR)/obj
#
#   _clean-dirs:
#   	@rm -rf $(BUILD_DIR)

_ensure-dirs:
	@mkdir -p $(BUILD_DIR)

# # B.2b Tool Checks [TOOLS]
#
#   _check-tools:
#   	@which gcc > /dev/null || (echo "Error: gcc not found" && exit 1)
#   	@which make > /dev/null || (echo "Error: make not found" && exit 1)

# [Reserved: Uncomment if tool verification needed]
# _check-tools:
# 	@which [tool] > /dev/null || (echo "Error: [tool] not found" && exit 1)

# # B.3 Core Operations [OPERATIONS]
#
#   Purpose:  The actual build work — compilation, testing, execution
#   Contains: Compilation, Testing, Quality, Execution, Maintenance
#
#   Compilation:
#     Build targets — release, debug variants
#
#   Testing:
#     Test targets — unit, integration, coverage
#
#   Quality:
#     Code quality — format, lint, static analysis
#
#   Execution:
#     Run targets — execute built binary
#
#   Maintenance:
#     Cleanup targets — remove artifacts

# # B.3a Compilation [COMPILATION]
#
#   ## build: Build the binary
#   build: _ensure-dirs
#   	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$(BINARY_NAME) $(SRC_DIR)/*.c $(LDFLAGS)
#
#   ## debug: Build with debug symbols
#   debug: _ensure-dirs
#   	$(CC) $(CFLAGS) -g -O0 -o $(BUILD_DIR)/$(BINARY_NAME) $(SRC_DIR)/*.c $(LDFLAGS)

## build: Build the [binary] binary
build: _ensure-dirs
	@echo "Building $(BINARY_NAME)..."
	@# [Build command - language specific]
	@# go build -o $(BUILD_DIR)/$(BINARY_NAME) ./cmd/[name]
	@# gcc $(CFLAGS) -o $(BUILD_DIR)/$(BINARY_NAME) $(SRC_DIR)/*.c
	@echo "✓ Build complete: $(BUILD_DIR)/$(BINARY_NAME)"

# # B.3b Testing [TESTING]
#
#   ## test: Run all tests
#   test:
#   	go test -v ./...
#   	pytest -v
#
#   ## test-coverage: Run tests with coverage
#   test-coverage:
#   	go test -coverprofile=coverage.out ./...

## test: Run all tests
test:
	@echo "Running tests..."
	@# [Test command - language specific]
	@# go test -v ./...
	@# pytest -v
	@echo "✓ Tests complete"

# # B.3c Quality [QUALITY]
#
#   ## fmt: Format code
#   fmt:
#   	go fmt ./...
#   	clang-format -i $(SRC_DIR)/*.c
#
#   ## lint: Run linter
#   lint:
#   	golangci-lint run
#   	cppcheck $(SRC_DIR)

# [Reserved: Uncomment quality targets as needed]
## fmt: Format code
# fmt:
# 	@echo "Formatting..."
# 	@echo "✓ Format complete"

## lint: Run linter
# lint:
# 	@echo "Linting..."
# 	@echo "✓ Lint complete"

# # B.3d Execution [EXECUTION]
#
#   ## run: Run the binary
#   run: build
#   	./$(BUILD_DIR)/$(BINARY_NAME) $(ARGS)
#
#   ## run-dev: Run in development mode
#   run-dev: debug
#   	./$(BUILD_DIR)/$(BINARY_NAME) --dev $(ARGS)

## run: Run the binary (use ARGS for arguments)
run: build
	@if [ -z "$(ARGS)" ]; then \
		echo "Running $(BINARY_NAME)..."; \
		./$(BUILD_DIR)/$(BINARY_NAME); \
	else \
		echo "Running $(BINARY_NAME) $(ARGS)..."; \
		./$(BUILD_DIR)/$(BINARY_NAME) $(ARGS); \
	fi

# # B.3e Maintenance [MAINTENANCE]
#
#   ## clean: Remove build artifacts
#   clean:
#   	@rm -rf $(BUILD_DIR)
#
#   ## distclean: Remove all generated files
#   distclean: clean
#   	@rm -rf vendor/ node_modules/

## clean: Remove build artifacts
clean:
	@echo "Cleaning..."
	@rm -rf $(BUILD_DIR)
	@echo "✓ Clean complete"

# # B.4 Error Handling [ERRORS]
#
#   Purpose:  Common recipe patterns for graceful failure
#   Contains: Failure Patterns, Recovery Patterns
#
#   Failure Patterns:
#     How to handle command failures in recipes
#
#   Recovery Patterns:
#     How to detect and recover from missing resources

# # B.4a Failure Patterns [FAILURE]
#
#   @command || echo "Warning: failed"           # continue on failure
#   @command1 && command2                        # stop on failure
#   @command || exit 1                           # fail recipe on error
#   -@command                                    # ignore exit status (- prefix)

# # B.4b Recovery Patterns [RECOVERY]
#
#   @which tool > /dev/null || (echo "Error: tool not found" && exit 1)
#   @test -d $(DIR) || mkdir -p $(DIR)           # create if missing
#   @test -f $(FILE) || touch $(FILE)            # create file if missing
#   @mkdir -p dir                                # safe create (no error if exists)
#   @rm -rf dir                                  # safe remove (no error if missing)

# # B.5 Public API [API]
#
#   Purpose:  User-facing targets — what `make help` shows
#   Contains: Aggregators, Information
#
#   Aggregators:
#     Composite targets combining multiple operations
#
#   Information:
#     Targets that display help and configuration

# # B.5a Aggregators [AGGREGATORS]
#
#   ## check: Run all quality checks
#   check: fmt lint test
#   	@echo "✓ All checks passed"
#
#   ## all: Build and test
#   all: build test

## check: Run all quality checks
# check: fmt lint test
# 	@echo "✓ All checks passed"

# # B.5b Information [INFO]
#
#   ## help: Show available targets
#   help:
#   	@sed -n 's/^## //p' Makefile | column -t -s ':'
#
#   ## info: Show build configuration
#   info:
#   	@echo "BINARY_NAME: $(BINARY_NAME)"

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

# # B.6 Omission Guide [OMISSION]
#
#   Purpose:  What can be reserved and why
#
#   All sections must be present. Empty sections use: [Reserved: reason]
#   Omit subsection content, never the subsection header itself.

# ═══════════════════════════════════════════════════════════════════════════════
# END BODY [END]
# ═══════════════════════════════════════════════════════════════════════════════

# ═══════════════════════════════════════════════════════════════════════════════
# CLOSING BLOCK [CLOSING]
# ═══════════════════════════════════════════════════════════════════════════════
#
# 4-Block Code Structure: Operational guidance and closing notes
#
# Section order: Validation → Execution → Cleanup | Policy → Extension → Troubleshooting → Reference → Note → Template
# Flow: verify → run → release | modify → extend → debug → lookup → ground → instantiate
#
# ═══════════════════════════════════════════════════════════════════════════════

# # X.1 Validation [VALIDATION]
#
#   Purpose:  Verify Makefile correctness
#   Contains: Checks, Build Commands, Common Issues
#
#   Checks:
#     What to verify before running
#
#   Build Commands:
#     Commands to test the Makefile
#
#   Common Issues:
#     Frequent problems and solutions

# # X.1a Checks [CHECKS]
#
#   - Targets exist and are spelled correctly
#   - Recipe lines use TAB, not spaces
#   - Variables are defined before use
#   - Dependencies are correct

# # X.1b Build Commands [BUILD]
#
#   make -n all                       # Dry-run (show commands)
#   make --warn-undefined-variables   # Catch undefined vars
#   make clean && make all            # Clean build test
#   make help                         # Check documentation

# # X.1c Common Issues [ISSUES]
#
#   "No rule to make target" → Check spelling, target exists
#   "Missing separator" → Recipe lines need TAB, not spaces
#   "Up to date" → Use make clean first
#   "Undefined variable" → Define before use or use ?=

# # X.2 Execution [EXECUTION]
#
#   Purpose:  Entry points and execution flow
#   Contains: Entry Points, Flow
#
#   Entry Points:
#     Common make commands and what they do
#
#   Flow:
#     How targets depend on each other

# # X.2a Entry Points [ENTRY]
#
#   make              # Default: all (build + test)
#   make build        # Build only
#   make test         # Test only
#   make run          # Build and run
#   make clean        # Remove artifacts
#   make help         # Show targets

# # X.2b Flow [FLOW]
#
#   make → all → build → _ensure-dirs → test → exit
#   make run → build → _ensure-dirs → run binary → exit
#   make clean → remove $(BUILD_DIR) → exit

# # X.3 Cleanup [CLEANUP]
#
#   Purpose:  Clean target patterns and artifact management
#   Contains: Safe Patterns, Dangerous Patterns
#
#   Safe Patterns:
#     rm commands that only remove build artifacts
#
#   Dangerous Patterns:
#     Commands that can destroy source or system

# # X.3a Safe Patterns [SAFE]
#
#   rm -rf $(BUILD_DIR)   # Safe - only build output
#   rm -f *.o             # Safe - only object files
#   rm -f $(BUILD_DIR)/*  # Safe - contents only

# # X.3b Dangerous Patterns [DANGEROUS]
#
#   rm -rf *              # CATASTROPHIC - removes everything
#   rm -rf $(SRC_DIR)     # CATASTROPHIC - removes source code
#   rm -rf /              # CATASTROPHIC - system destruction

# ───────────────────────────────────────────────────────────────────────────────
# END CODE SECTIONS
# ───────────────────────────────────────────────────────────────────────────────

# # X.4 Policy [POLICY]
#
#   Purpose:  Guide future maintainers — what's safe to change
#   Contains: Safe / Careful / Never modification levels
#
# Safe to Modify:
#   ✅ Add new targets (use ## for help)
#   ✅ Add internal helpers (use _ prefix)
#   ✅ Extend clean target
#
# Modify with Care:
#   ⚠️ Default target (all) — affects bare `make`
#   ⚠️ Target names — breaks scripts/CI
#   ⚠️ Variable names — breaks overrides
#
# NEVER Modify:
#   ❌ 4-block structure (METADATA, SETUP, BODY, CLOSING)
#   ❌ Remove clean/help targets
#   ❌ Hardcode absolute paths
#
# Validation: See X.1 for build and correctness checks.

# # X.5 Extension [EXTENSION]
#
#   Purpose:  How to expand this Makefile — mirrors M.10 Roadmap
#   Contains: Extension patterns, planned features
#
# Adding New Targets:
#   1. Add to .PHONY declaration
#   2. Add ## comment for help
#   3. Add target with recipe
#   4. Update B.1 Dependency Graph
#
# Adding New Variables:
#   1. Add to S.2 (constants) or S.3 (variables)
#   2. Use ?= for overridable, = for fixed
#   3. Document in make info target
#
# Architecture: See BODY B.1 for Ladder/Baton structure.
# Roadmap: See M.10 for planned features and research areas.

# # X.6 Troubleshooting [TROUBLESHOOTING]
#
#   Purpose:  Common problems and solutions — makefile issues
#
# "No rule to make target 'X'":
#   → Check target exists, spelling matches
#
# "'X' is up to date":
#   → make clean && make X
#
# "Missing separator":
#   → Recipe lines must use TAB, not spaces
#
# Variable not expanding:
#   → Use $(VAR) not $VAR for multi-char names
#
# Target-specific troubleshooting in BODY B.3 target docstrings.

# # X.7 Reference [REFERENCE]
#
#   Purpose:  Quick lookup — copy-paste ready commands
#
# Basic:
#   make              # Build and test
#   make build        # Build only
#   make test         # Test only
#   make clean        # Remove artifacts
#
# Development:
#   make run ARGS="--verbose"   # Run with args
#   make clean && make all      # Fresh build
#
# Debug:
#   make -n all       # Dry-run
#   make info         # Show config
#   make -p           # Print database

# # X.8 Closing Note [NOTE]
#
#   Purpose:  Final guidance and grounding — echoes M.8
#   Verse:    Nehemiah 4:6
#
# This Makefile orchestrates build operations for [project-name].
# Modify thoughtfully — changes affect all build workflows.
#
# "So built we the wall... for the people had a mind to work."
#   — Nehemiah 4:6
#
# May organized labor build well.

# # X.9 Template Guidance [TEMPLATE]
#
#   Purpose:  How to use this template — remove when instantiating
#   Usage:    cp makefile.mk Makefile → update pragma → fill sections
#
# Update on instantiation:
#   - M.1: Key, Title
#   - M.2: Created/Updated dates
#   - M.4: Path
#   - M.5: Derives_from (point to this template)
#   - M.9: Dependencies
#   - M.10: Roadmap
#   - S.2: Constants (BINARY_NAME, paths)
#
# Remove this section (X.9) when instantiating.

# # X.10 Omission Guide [OMISSION]
#
#   Purpose:  What can be reserved and why — guidance for CLOSING sections
#
#   All sections must be present. Empty sections use: [Reserved: reason]
#   Omit subsection content, never the subsection header itself.

# ═══════════════════════════════════════════════════════════════════════════════
# END CLOSING [END]
# ═══════════════════════════════════════════════════════════════════════════════

