# ═══════════════════════════════════════════════════════════════════════════
# TEMPLATE: Project CMakeLists.txt (4-Block Structure)
# Key: CODE-CMAKE-001
# ═══════════════════════════════════════════════════════════════════════════
#
# This is a TEMPLATE file - copy and modify for new CMake projects.
# Replace all [bracketed] placeholders with actual content.
# Rename to "CMakeLists.txt" when ready to use.
#
# Adapted from: templates/code/make/CODE-MAKE-001-MAKE-project-makefile.mk
# See: standards/code/4-block/ for complete documentation
#
# ═══════════════════════════════════════════════════════════════════════════

# ============================================================================
# METADATA
# ============================================================================
#
# Package:     [organization/project-name]
# File:        CMakeLists.txt
# Key:         [PROJECT-BUILD-###] (CMake build configuration)
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
#   mkdir build && cd build   # Create out-of-source build directory
#   cmake ..                  # Configure project (generate build system)
#   cmake --build .           # Build project
#   cmake --build . --target [name]  # Build specific target
#
# Integration Pattern:
#
#   1. [Initial setup step]
#   2. [Configuration step if needed]
#   3. [Typical usage workflow]
#
# Public API (CMake Targets - in typical usage order):
#
#   [Category] ([purpose]):
#     cmake --build . --target [name]   # [Description]
#     cmake --build . --target [name]   # [Description]
#
# ────────────────────────────────────────────────────────────────
# Dependencies
# ────────────────────────────────────────────────────────────────
#
# What This Needs:
#
#   - System Tools: [cmake (>= 3.x), generator (make/ninja/etc.)]
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
# Section order: Declarations → Constants → Variables → Options → Project Config → Include/Subdirectory
# This flows: what exists → fixed config → dynamic config → user options → project settings → included modules
#
# Universal mapping (see standards for cross-language patterns):
#   Declarations ≈ Imports (cmake_minimum_required, project())
#   Constants ≈ Constants (fixed set() values)
#   Variables ≈ Variables (CACHE variables, overridable)
#   Options ≈ Types (option() boolean flags)
#   Project Config ≈ Entry Point (target_*, set_target_properties)
#   Include/Subdirectory ≈ Package-Level State (include, add_subdirectory)

# ────────────────────────────────────────────────────────────────
# Declarations
# ────────────────────────────────────────────────────────────────
#
# CMake version requirements and project declaration.
# This establishes what CMake features are available and project identity.
#
# See: standards/code/4-block/sections/setup/CWS-SECTION-SETUP-001-imports.md

# ═══ CMake Version Requirement ═══
# Minimum version needed for features used in this file.

cmake_minimum_required(VERSION 3.16)

# ═══ Project Declaration ═══
# Project identity - name, version, languages used.

project([ProjectName]
    VERSION [1.0.0]
    DESCRIPTION "[Project description]"
    LANGUAGES [CXX C]
)

# ────────────────────────────────────────────────────────────────
# Constants
# ────────────────────────────────────────────────────────────────
#
# Fixed values that never change. set() without CACHE - these are immutable.
# Magic numbers given meaningful names, paths documented with reasoning.
#
# See: standards/code/4-block/sections/setup/CWS-SECTION-SETUP-002-constants.md

# ═══ Project Constants ═══
# Core project identity - changing these affects all build outputs.

set(BINARY_NAME "[binary-name]")
set(BUILD_DIR "${CMAKE_BINARY_DIR}/bin")

# ═══ Path Constants ═══
# Directory structure - consistent paths for all operations.

set(SRC_DIR "${CMAKE_SOURCE_DIR}/[src-directory]")
set(TEST_DIR "${CMAKE_SOURCE_DIR}/[test-directory]")

# ────────────────────────────────────────────────────────────────
# Variables
# ────────────────────────────────────────────────────────────────
#
# Overridable configuration. Use CACHE for defaults that can be changed.
# Document what each controls and valid values.
#
# See: standards/code/4-block/sections/setup/CWS-SECTION-SETUP-003-variables.md

# ═══ Tool Configuration ═══
# Can be overridden: cmake -DCMAKE_C_COMPILER=clang ..

# set(CMAKE_C_COMPILER "gcc" CACHE STRING "C compiler")
# set(CMAKE_CXX_COMPILER "g++" CACHE STRING "C++ compiler")

# ═══ Build Configuration ═══
# Passed at configuration: cmake -DCMAKE_BUILD_TYPE=Release ..

set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type (Debug/Release/RelWithDebInfo/MinSizeRel)")

# ────────────────────────────────────────────────────────────────
# Options
# ────────────────────────────────────────────────────────────────
#
# Boolean configuration flags. Use option() for ON/OFF toggles.
# These control conditional compilation and feature enablement.
#
# See: standards/code/4-block/sections/setup/CWS-SECTION-SETUP-004-types.md

# ═══ Feature Flags ═══
# Toggle features: cmake -D[OPTION]=ON/OFF ..

option([PROJECT]_BUILD_TESTS "Build test executables" ON)
option([PROJECT]_BUILD_DOCS "Build documentation" OFF)
# option([PROJECT]_ENABLE_FEATURE "Enable specific feature" OFF)

# ────────────────────────────────────────────────────────────────
# Project Config
# ────────────────────────────────────────────────────────────────
#
# Global project settings - output directories, C++ standard, etc.
# These affect all targets defined in this CMakeLists.txt.
#
# See: standards/code/4-block/sections/setup/CWS-SECTION-SETUP-005-type-methods.md

# ═══ Output Directories ═══
# Where built artifacts go.

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${BUILD_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${BUILD_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${BUILD_DIR}/lib")

# ═══ Language Standards ═══
# C/C++ standard requirements.

# set(CMAKE_C_STANDARD 11)
# set(CMAKE_C_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ────────────────────────────────────────────────────────────────
# Include/Subdirectory
# ────────────────────────────────────────────────────────────────
#
# Module includes and subdirectory additions.
# This is the "Rails attachment" - infrastructure all targets use.
#
# See: standards/code/4-block/sections/setup/CWS-SECTION-SETUP-006-package-level-state.md

# ═══ CMake Modules ═══
# Include CMake module files for additional functionality.
# [Reserved: Uncomment if custom modules needed]

# include(cmake/helpers.cmake)      # Project-specific helper functions
# include(FetchContent)             # For downloading dependencies

# ═══ Subdirectories ═══
# Add child CMakeLists.txt from subdirectories.
# [Reserved: Add subdirectories as project grows]

# add_subdirectory(src)             # Main source directory
# add_subdirectory(tests)           # Test directory (if separate)

# ============================================================================
# END SETUP
# ============================================================================

# ============================================================================
# BODY
# ============================================================================
#
# For BODY structure explanation, see: standards/code/4-block/CWS-STD-007-CODE-body-block.md
#
# Section order: Target Dependency Graph → Helper Functions → Library/Executable Targets → Custom Targets → Install Rules
# This flows: map structure → foundations → build artifacts → custom commands → distribution
#
# Universal mapping (see standards for cross-language patterns):
#   Target Dependency Graph ≈ Organizational Chart (internal structure map)
#   Helper Functions ≈ Helpers/Utilities (CMake functions/macros)
#   Library/Executable Targets ≈ Core Operations (add_library, add_executable)
#   Custom Targets ≈ Error Handling/Recovery (add_custom_target)
#   Install Rules ≈ Public APIs (install() commands)

# ────────────────────────────────────────────────────────────────
# Target Dependency Graph - Build Order
# ────────────────────────────────────────────────────────────────
#
# Maps target dependencies and execution flow within this CMakeLists.txt.
# Provides navigation for both development (what targets exist) and
# maintenance (what depends on this target).
#
# See: standards/code/4-block/sections/body/CWS-SECTION-BODY-001-organizational-chart.md
#
# Ladder Structure (Dependencies):
#
#   Install Rules (Top Rungs - Distribution Interface)
#   └── install() → executables, libraries, headers
#
#   Custom Targets (Middle-Upper Rungs - Automation)
#   ├── run → ${BINARY_NAME}
#   └── clean-all → (standalone)
#
#   Library/Executable Targets (Middle Rungs - Core Artifacts)
#   ├── ${BINARY_NAME} (executable) → [lib_name]
#   └── [lib_name] (library) → (standalone)
#
#   Helper Functions (Bottom Rungs - Foundations)
#   └── CMake functions/macros
#
# Baton Flow (Execution Paths):
#
#   Entry → cmake --build .
#     ↓
#   ALL_BUILD (default meta-target)
#     ↓
#   ${BINARY_NAME} → links [lib_name]
#     ↓
#   Exit → success/failure
#
# Target Summary:
# - [X] targets total
# - [X] helper functions (foundations)
# - [X] library/executable targets (core artifacts)
# - [X] custom targets (automation)
# - [X] install rules (distribution)

# ────────────────────────────────────────────────────────────────
# Helper Functions - Reusable CMake Logic
# ────────────────────────────────────────────────────────────────
#
# CMake functions and macros used by other parts of this file.
# Bottom rungs of the ladder - simple, focused, reusable utilities.
#
# See: standards/code/4-block/sections/body/CWS-SECTION-BODY-002-helpers.md
#
# Key characteristics:
# - Defined before use (CMake processes top-to-bottom)
# - function() for local scope, macro() for caller scope
# - Simple, single-purpose operations
# - Reusable across multiple targets

# ensure_directory creates required directories.
#
# What It Does:
# Creates build output directories if they don't exist.
# CMake handles this automatically for most cases.
#
# Used By:
#   Custom targets that need directories before commands run.
#
# function(ensure_directory DIR_PATH)
#     file(MAKE_DIRECTORY "${DIR_PATH}")
# endfunction()

# check_tool verifies a required tool is installed.
#
# What It Does:
# Checks that a required build tool is available in PATH.
# Fails configuration with helpful message if tool missing.
#
# Used By:
#   Configuration phase (ensures environment is ready)
#
# function(check_tool TOOL_NAME)
#     find_program(${TOOL_NAME}_FOUND ${TOOL_NAME})
#     if(NOT ${TOOL_NAME}_FOUND)
#         message(FATAL_ERROR "${TOOL_NAME} not found in PATH")
#     endif()
# endfunction()

# ────────────────────────────────────────────────────────────────
# Library/Executable Targets - Core Artifacts
# ────────────────────────────────────────────────────────────────
#
# Core build targets implementing the CMakeLists.txt's primary purpose.
# Organized by artifact type (libraries, executables) below.
#
# See: standards/code/4-block/sections/body/CWS-SECTION-BODY-003-core-operations.md

# ────────────────────────────────────────────────────────────────
# Libraries - Shared/Static Libraries
# ────────────────────────────────────────────────────────────────
#
# What These Do:
# Create reusable library targets (static or shared).
#
# Why Separated:
# Libraries are dependencies - must be defined before executables that use them.
#
# Extension Point:
# To add new libraries, create add_library() with appropriate sources.

# [lib_name] library - [description of what it provides]
#
# add_library([lib_name] STATIC
#     ${SRC_DIR}/[file1].c
#     ${SRC_DIR}/[file2].c
# )
# target_include_directories([lib_name] PUBLIC ${CMAKE_SOURCE_DIR}/include)

# ────────────────────────────────────────────────────────────────
# Executables - Binary Building
# ────────────────────────────────────────────────────────────────
#
# What These Do:
# Create executable binary targets.
#
# Why Separated:
# Executables are the primary output - link against libraries defined above.
#
# Extension Point:
# To add new executables, create add_executable() with appropriate sources.

# ${BINARY_NAME} executable - main program
add_executable(${BINARY_NAME}
    ${SRC_DIR}/main.[c|cpp]
    # ${SRC_DIR}/[other_sources].[c|cpp]
)

# Link against libraries (uncomment if using library from above)
# target_link_libraries(${BINARY_NAME} PRIVATE [lib_name])

# Set compile options for this target
# target_compile_options(${BINARY_NAME} PRIVATE -Wall -Wextra)

# ────────────────────────────────────────────────────────────────
# Testing - Verification
# ────────────────────────────────────────────────────────────────
#
# What These Do:
# Register and configure test targets.
#
# Why Separated:
# Testing is distinct from building - verifies what was built.
#
# Extension Point:
# To add new tests, create test executables and register with add_test().
# Run tests with: cmake --build . --target test (or ctest)

# Enable CTest for this project
enable_testing()

# Test executable (if tests are separate from main)
# add_executable(test_${BINARY_NAME}
#     ${TEST_DIR}/test_main.[c|cpp]
# )
# target_link_libraries(test_${BINARY_NAME} PRIVATE [lib_name])

# Register tests with CTest
# add_test(NAME unit_tests COMMAND test_${BINARY_NAME})
# add_test(NAME integration COMMAND ${BINARY_NAME} --test-mode)

# ────────────────────────────────────────────────────────────────
# Custom Targets - Quality & Automation
# ────────────────────────────────────────────────────────────────
#
# What These Do:
# Custom targets for quality checks, formatting, and automation.
# Run with: cmake --build . --target [name]
#
# Why Separated:
# Quality checks are pre-commit gates - distinct from build artifacts.
#
# Extension Point:
# To add new quality checks, create add_custom_target() with appropriate COMMAND.

# format: Format code (run clang-format, etc.)
# add_custom_target(format
#     COMMAND clang-format -i ${SRC_DIR}/*.cpp ${SRC_DIR}/*.h
#     COMMENT "Formatting code..."
#     VERBATIM
# )

# lint: Run linter/static analysis
# add_custom_target(lint
#     COMMAND cppcheck --enable=all ${SRC_DIR}
#     COMMENT "Running static analysis..."
#     VERBATIM
# )

# check: Aggregate quality target
# add_custom_target(check
#     DEPENDS format lint
#     COMMENT "Running all quality checks..."
# )

# ────────────────────────────────────────────────────────────────
# Execution - Running
# ────────────────────────────────────────────────────────────────
#
# What These Do:
# Custom target to execute the built binary.
# Run with: cmake --build . --target run
#
# Why Separated:
# Running is distinct from building - uses what was built.
#
# Extension Point:
# To add new run modes, create additional custom targets.

# run: Execute the binary (depends on it being built first)
add_custom_target(run
    COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${BINARY_NAME}
    DEPENDS ${BINARY_NAME}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running ${BINARY_NAME}..."
    VERBATIM
)

# ────────────────────────────────────────────────────────────────
# Maintenance - Cleanup
# ────────────────────────────────────────────────────────────────
#
# What These Do:
# Custom target to clean beyond what 'cmake --build . --target clean' does.
# The built-in 'clean' target handles most cleanup automatically.
#
# Why Separated:
# Cleanup is destructive - intentionally separated from build operations.
#
# Extension Point:
# Add cleanup commands for files CMake doesn't track.
# Note: cmake --build . --target clean handles standard build artifacts.

# clean-all: Remove all generated files including CMake cache
add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/CMakeFiles
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_BINARY_DIR}/CMakeCache.txt
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_BINARY_DIR}/cmake_install.cmake
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${BUILD_DIR}
    COMMENT "Cleaning all generated files..."
    VERBATIM
)

# ────────────────────────────────────────────────────────────────
# Error Handling - CMake Patterns
# ────────────────────────────────────────────────────────────────
#
# Common error handling patterns for CMake.
# These patterns ensure graceful failure and helpful error messages.
#
# See: standards/code/4-block/sections/body/CWS-SECTION-BODY-004-error-handling.md
#
# Pattern: Fatal error (stops configuration)
#   message(FATAL_ERROR "Error message here")
#
# Pattern: Warning (continues but alerts)
#   message(WARNING "Warning message here")
#
# Pattern: Check prerequisites at configure time
#   find_program(TOOL_FOUND tool_name)
#   if(NOT TOOL_FOUND)
#       message(FATAL_ERROR "tool_name not found")
#   endif()
#
# Pattern: Conditional compilation
#   if(EXISTS "${CMAKE_SOURCE_DIR}/optional_file.cpp")
#       target_sources(${BINARY_NAME} PRIVATE optional_file.cpp)
#   endif()
#
# Pattern: Safe file operations
#   file(MAKE_DIRECTORY dir)     # Create if not exists
#   file(REMOVE_RECURSE dir)     # Remove recursively
#
# Pattern: Check required packages
#   find_package(Package REQUIRED)  # Fails if not found
#   find_package(Package QUIET)     # Continues if not found
#
# Note: message(STATUS "...") for informational output.
# Note: CMake errors at configure time, not build time.

# ────────────────────────────────────────────────────────────────
# Install Rules - Distribution Interface
# ────────────────────────────────────────────────────────────────
#
# Install rules for distributing built artifacts.
# Run with: cmake --install . (or cmake --build . --target install)
#
# See: standards/code/4-block/sections/body/CWS-SECTION-BODY-005-public-apis.md
#
# Key characteristics:
# - Executed by cmake --install
# - Define where artifacts go in the system
# - Support different install prefixes (CMAKE_INSTALL_PREFIX)
# - Handle executables, libraries, headers, data files

# Install executable to bin/
install(TARGETS ${BINARY_NAME}
    RUNTIME DESTINATION bin
)

# Install library to lib/ (if applicable)
# install(TARGETS [lib_name]
#     LIBRARY DESTINATION lib
#     ARCHIVE DESTINATION lib
# )

# Install headers to include/ (if applicable)
# install(DIRECTORY include/
#     DESTINATION include
#     FILES_MATCHING PATTERN "*.h"
# )

# Install configuration files (if applicable)
# install(FILES config/default.conf
#     DESTINATION etc/${BINARY_NAME}
# )

# Print configuration summary at end of configure
message(STATUS "")
message(STATUS "[ProjectName] Configuration Summary:")
message(STATUS "  Binary Name:     ${BINARY_NAME}")
message(STATUS "  Build Type:      ${CMAKE_BUILD_TYPE}")
message(STATUS "  Source Dir:      ${SRC_DIR}")
message(STATUS "  Build Dir:       ${BUILD_DIR}")
message(STATUS "  Install Prefix:  ${CMAKE_INSTALL_PREFIX}")
message(STATUS "")

# ============================================================================
# END BODY
# ============================================================================

# ============================================================================
# CLOSING
# ============================================================================
#
# For CLOSING structure explanation, see: standards/code/4-block/CWS-STD-008-CODE-closing-block.md
#
# Section order: CMake Validation → CMake Execution → CMake Cleanup → Final Documentation
# This flows: verify correctness → entry points → resource management → synthesis
#
# Universal mapping (see standards for cross-language patterns):
#   CMake Validation ≈ Code Validation (testing/verification)
#   CMake Execution ≈ Code Execution (entry points and flow)
#   CMake Cleanup ≈ Code Cleanup (resource management)
#   Final Documentation = Same across all languages

# ────────────────────────────────────────────────────────────────
# CMake Validation: [project-name] (Build System)
# ────────────────────────────────────────────────────────────────
#
# For Code Validation section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-001-code-validation.md
#
# Testing Requirements:
#   - Verify CMake configuration completes without errors
#   - Confirm all targets build successfully
#   - Test with clean build directory (rm -rf build && cmake -B build)
#   - Verify CTest runs all registered tests
#   - Check install target works correctly
#
# Configuration Verification:
#   - cmake -B build (configuration succeeds)
#   - cmake -B build -DCMAKE_BUILD_TYPE=Release (release config works)
#   - cmake -L -B build (list cached variables)
#   - cmake --graphviz=deps.dot -B build (dependency graph)
#
# Build Verification:
#   - cmake --build build (complete build succeeds)
#   - cmake --build build --clean-first (clean build works)
#   - ctest --test-dir build (all tests pass)
#
# Integration Testing:
#   - Test from project root directory
#   - Verify out-of-source build works
#   - Test with different generators (make, ninja)
#   - Confirm CI/CD integration works
#
# Example validation sequence:
#
#   # Fresh configuration
#   rm -rf build && cmake -B build
#
#   # Build
#   cmake --build build
#
#   # Run tests
#   ctest --test-dir build
#
#   # Test install
#   cmake --install build --prefix /tmp/test-install

# ────────────────────────────────────────────────────────────────
# CMake Execution: [project-name] (Build System)
# ────────────────────────────────────────────────────────────────
#
# For Code Execution section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-002-code-execution.md
#
# This is a BUILD SYSTEM. CMake generates build files for native build tools.
# Two-phase process: configure (cmake) then build (cmake --build).
#
# Entry Point: `cmake -B build && cmake --build build`
#
# Execution Flow:
#   1. User invokes `cmake -B build` (configure phase)
#   2. CMake processes CMakeLists.txt, generates native build files
#   3. User invokes `cmake --build build` (build phase)
#   4. Native build tool (make/ninja) compiles and links
#   5. Status messages shown during configuration and build
#   6. Exit code propagates (0=success, non-zero=failure)
#
# Available Entry Points:
#   cmake -B build                     # Configure with defaults
#   cmake -B build -DCMAKE_BUILD_TYPE=Release  # Configure release
#   cmake --build build                # Build all targets
#   cmake --build build --target run   # Build and run
#   cmake --build build --target test  # Run tests (or ctest)
#   cmake --build build --target clean # Clean build artifacts
#   cmake --build build --target clean-all  # Deep clean
#   cmake --install build              # Install to prefix
#
# Dependency Resolution:
#   cmake --build . → ${BINARY_NAME} → [lib_name]
#                   → test_${BINARY_NAME}
#
# Example invocation:
#
#   # Default workflow
#   cmake -B build && cmake --build build
#
#   # Release build with custom install
#   cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/opt/app
#   cmake --build build
#   cmake --install build

# ────────────────────────────────────────────────────────────────
# CMake Cleanup: [project-name] (Build System)
# ────────────────────────────────────────────────────────────────
#
# For Code Cleanup section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-003-code-cleanup.md
#
# Resource Management:
#   - Build artifacts: Removed by `cmake --build build --target clean`
#   - CMake cache: Removed by deleting build directory or clean-all target
#   - Object files: Removed by clean target (handled by generator)
#   - Build directory: Out-of-source, can be deleted entirely
#
# Clean Target Scope:
#   - build/: Entire directory can be safely deleted
#   - CMakeCache.txt: Regenerated on reconfigure
#   - Source files: NEVER removed (outside build directory)
#   - Version control: NEVER touched (.git/)
#
# Error State Cleanup:
#   - Partial builds: `cmake --build build --target clean` resets
#   - Failed targets: Re-run after fixing issues
#   - CMake cache issues: Delete build/ and reconfigure
#
# Safe Cleanup Patterns:
#   - rm -rf build/ - Safe (out-of-source build directory)
#   - cmake --build build --target clean - Safe (standard cleanup)
#   - cmake --build build --target clean-all - Safe (deep clean)
#   - NEVER: rm -rf * from source directory (catastrophic)
#   - NEVER: rm -rf / (obviously catastrophic)
#
# Example cleanup sequence:
#
#   # Standard cleanup
#   cmake --build build --target clean
#
#   # Full reset (delete build directory and reconfigure)
#   rm -rf build && cmake -B build
#
#   # Verify clean state
#   ls -la build/  # Should show only CMake cache files

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
#   ✅ Add new targets (add_executable, add_library, add_custom_target)
#   ✅ Add new helper functions (function(), macro())
#   ✅ Extend install rules (add more install() commands)
#   ✅ Add build options (option() for feature toggles)
#   ✅ Add test cases (add_test())
#
# Modify with Extreme Care (Breaking Changes):
#   ⚠️ project() name - affects package naming
#   ⚠️ Target names - breaks scripts/CI depending on them
#   ⚠️ CACHE variable names - breaks command-line overrides (-D flags)
#   ⚠️ Install destinations - breaks deployment/packaging
#   ⚠️ cmake_minimum_required version - affects compatibility
#
# NEVER Modify (Foundational Structure):
#   ❌ 4-block structure (METADATA, SETUP, BODY, CLOSING)
#   ❌ Remove install() rules (distribution mechanism)
#   ❌ Remove enable_testing() (test framework hook)
#   ❌ Add file(REMOVE) in source tree
#   ❌ Hardcode absolute paths (portability)
#
# Validation After Modifications:
#   See "CMake Validation" section above for comprehensive testing
#   requirements, configuration verification, and integration testing procedures.

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
# - All targets and their link dependencies (ladder)
# - Complete execution flow paths (baton)
# - Target count and categorization
#
# Quick architectural summary (details in BODY Target Dependency Graph):
# - [X] install rules → [Y] executable/library targets → [Z] helper functions
# - Ladder: Install rules → Executables/Libraries → Helper functions
# - Baton: cmake --build → ${BINARY_NAME} → [lib_name] → complete

# ────────────────────────────────────────────────────────────────
# Surgical Update Points (Extension Guide)
# ────────────────────────────────────────────────────────────────
#
# For Surgical Update Points section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-007-surgical-update-points.md
#
# See BODY section subsection header comments above for detailed
# extension points. Each subsection includes "Extension Point" guidance showing:
# - Where to add new targets
# - What CMake commands to use
# - How to integrate with existing targets
# - What target properties to set
#
# Quick reference (details in BODY subsection comments):
# - Adding libraries: See BODY "Libraries" section - use add_library()
# - Adding executables: See BODY "Executables" section - use add_executable()
# - Adding tests: See BODY "Testing" section - use add_test()
# - Adding custom targets: See BODY "Custom Targets" section - use add_custom_target()
# - Adding install rules: See BODY "Install Rules" section - use install()
# - Adding helper functions: See BODY "Helper Functions" section - use function()

# ────────────────────────────────────────────────────────────────
# Performance Considerations
# ────────────────────────────────────────────────────────────────
#
# For Performance Considerations section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-008-performance-considerations.md
#
# See SETUP section above for build performance characteristics:
# - Constants: Paths and names affecting build output
# - Options: Feature flags affecting what gets built
#
# Quick summary:
# - Parallel builds: cmake --build build -- -j$(nproc) (or use Ninja generator)
# - Incremental builds: CMake tracks dependencies automatically
# - Clean builds: cmake --build build --clean-first forces full rebuild
# - Configuration cache: CMakeCache.txt stores config for fast reconfigure
#
# Key optimization tips:
# - Use Ninja generator: cmake -G Ninja -B build (faster than Make)
# - Use ccache: set(CMAKE_C_COMPILER_LAUNCHER ccache)
# - Precompiled headers: target_precompile_headers() for large projects
# - Unity builds: set(CMAKE_UNITY_BUILD ON) to combine translation units
# - Out-of-source builds: Always build outside source tree for cleanliness

# ────────────────────────────────────────────────────────────────
# Troubleshooting Guide
# ────────────────────────────────────────────────────────────────
#
# For Troubleshooting Guide section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-009-troubleshooting-guide.md
#
# Problem: "CMake Error: Could not find CMAKE_PROJECT_NAME"
#   Check: project() command exists and is spelled correctly
#   Check: CMakeLists.txt is in correct directory
#   Solution: Add or fix project() declaration
#
# Problem: "Target X does not exist in this directory"
#   Check: add_executable/add_library exists for target
#   Check: Target name spelling matches exactly
#   Solution: Add missing target definition or fix spelling
#
# Problem: "Cannot find source file: X"
#   Check: File path is correct relative to CMAKE_SOURCE_DIR
#   Check: File actually exists
#   Solution: Fix path or create missing file
#
# Problem: Cache variable not updating
#   Cause: CACHE variables persist in CMakeCache.txt
#   Solution: Delete build/CMakeCache.txt and reconfigure
#   Solution: Use cmake -U VAR -B build to unset specific variable
#
# Problem: Variable not expanding as expected
#   Check: Use ${VAR} syntax (not $VAR)
#   Check: Variable is set() before use
#   Check: Quoted strings for paths with spaces: "${VAR}"
#   Solution: Fix syntax or move set() before usage
#
# Problem: "target_link_libraries called with invalid target"
#   Check: Target defined with add_executable/add_library BEFORE link call
#   Check: Target name spelling matches exactly
#   Solution: Reorder CMakeLists.txt or fix target name

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
# - Key dependencies: cmake, generator (make/ninja), [language toolchain]
# - Primary consumers: Developers, CI/CD
#
# Parallel Implementation (if applicable):
#   - Other build systems: [Makefile/Meson equivalents if any]
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
# This CMakeLists.txt is the BUILD CONFIGURATION LAYER for [project-name].
# It defines targets, dependencies, and build configuration for CMake.
#
# Modify thoughtfully - changes here affect all build workflows.
# Maintain the target naming conventions and install rules.
#
# For questions, issues, or contributions:
#   - Review the modification policy above
#   - Follow the 4-block structure pattern
#   - Test configuration before committing (rm -rf build && cmake -B build && cmake --build build)
#   - Comment new targets appropriately
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
#   cmake -B build                         # Configure project
#   cmake --build build                    # Build all targets
#   ctest --test-dir build                 # Run tests
#   cmake --build build --target clean     # Clean artifacts
#   cmake --install build                  # Install to prefix
#
# Development Workflow:
#   cmake -B build && cmake --build build --target run    # Build and run
#   cmake --build build --clean-first                     # Clean + build
#   rm -rf build && cmake -B build                        # Full reconfigure
#
# CI/CD Integration:
#   cmake -B build -DCMAKE_BUILD_TYPE=Release   # Configure release
#   cmake --build build                          # Build phase
#   ctest --test-dir build --output-on-failure   # Test phase
#
# Debugging:
#   cmake -L -B build                      # List cache variables
#   cmake --build build --verbose          # Verbose build output
#   cmake --graphviz=deps.dot -B build     # Generate dependency graph
#
# Configuration Override:
#   cmake -B build -DBINARY_NAME=custom           # Override variable
#   cmake -B build -DCMAKE_C_COMPILER=clang       # Override compiler

# ============================================================================
# END CLOSING
# ============================================================================

