# ============================================================================
# METADATA
# ============================================================================
# Key: B-root-CMakeLists
# Purpose: CMake build configuration for Bereshit C components
# Biblical: "For every house is builded by some man; but he that built all
#           things is God." — Hebrews 3:4
#
# Layers Built:
#   L0: libtrit   - Ternary mathematics foundation
#   L1: libomni   - OmniCode language processor
#   L2: fuse      - Bereshit FUSE filesystem
#
# Usage:
#   mkdir build && cd build
#   cmake .. [-DCMAKE_BUILD_TYPE=Debug|Release]
#   make
#
# ============================================================================

cmake_minimum_required(VERSION 3.16)

# ============================================================================
# SETUP - Project Configuration
# ============================================================================

project(Bereshit
    VERSION 0.1.25
    DESCRIPTION "Bereshit - Ternary File System following the Genesis Pattern"
    LANGUAGES C
)

# C Standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Build type default
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Global compile options
add_compile_options(-Wall -Wextra)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g -O0)
    add_definitions(-DDEBUG)
else()
    add_compile_options(-O2)
    add_definitions(-DNDEBUG)
endif()

# BERESHIT_ROOT for config loading
set(BERESHIT_ROOT ${CMAKE_SOURCE_DIR})
add_definitions(-DBERESHIT_ROOT="${BERESHIT_ROOT}")

# ============================================================================
# BODY - Component Directories
# ============================================================================

# Layer directories (current structure)
set(L0_DIR ${CMAKE_SOURCE_DIR}/word/work/pkg/trit)
set(L1_DIR ${CMAKE_SOURCE_DIR}/word/work/pkg/omni)
set(L2_DIR ${CMAKE_SOURCE_DIR}/word/work/pkg/fuse)

# ────────────────────────────────────────────────────────────────────────────
# L0: libtrit - Ternary Mathematics Foundation
# ────────────────────────────────────────────────────────────────────────────

# Collect libtrit sources
file(GLOB TRIT_SOURCES "${L0_DIR}/src/*.c")

# Create libtrit library
add_library(trit STATIC ${TRIT_SOURCES})

target_include_directories(trit
    PUBLIC
        ${L0_DIR}/include
        ${L0_DIR}/include/generated
)

# Math library linkage
target_link_libraries(trit PUBLIC m)

# ────────────────────────────────────────────────────────────────────────────
# L1: libomni - OmniCode Language Processor
# ────────────────────────────────────────────────────────────────────────────

# Collect OmniCode sources by component
file(GLOB OMNI_CORE_SOURCES "${L1_DIR}/src/lang/core/*.c")
file(GLOB OMNI_FRONTEND_SOURCES "${L1_DIR}/src/lang/frontend/*.c")
file(GLOB OMNI_BACKEND_SOURCES "${L1_DIR}/src/lang/backend/*.c")
file(GLOB OMNI_RUNTIME_SOURCES
    "${L1_DIR}/src/runtime/vm/*.c"
    "${L1_DIR}/src/runtime/tools/*.c"
    "${L1_DIR}/src/runtime/cpisi/*.c"
    "${L1_DIR}/src/runtime/bible/*.c"
)
file(GLOB OMNI_TRANSPILER_SOURCES "${L1_DIR}/src/transpiler/*.c")
file(GLOB OMNI_IDE_SOURCES
    "${L1_DIR}/src/ide/core/*.c"
    "${L1_DIR}/src/ide/ui/*.c"
    "${L1_DIR}/src/ide/panels/*.c"
)

# Combine all OmniCode sources
set(OMNI_SOURCES
    ${OMNI_CORE_SOURCES}
    ${OMNI_FRONTEND_SOURCES}
    ${OMNI_BACKEND_SOURCES}
    ${OMNI_RUNTIME_SOURCES}
    ${OMNI_TRANSPILER_SOURCES}
    ${OMNI_IDE_SOURCES}
)

# Create libomni library
add_library(omni STATIC ${OMNI_SOURCES})

target_include_directories(omni
    PUBLIC
        ${L1_DIR}/include
    PRIVATE
        ${L0_DIR}/include
        ${L0_DIR}/include/generated
)

# OmniCode depends on libtrit
target_link_libraries(omni PUBLIC trit)

# ────────────────────────────────────────────────────────────────────────────
# L2: Bereshit FUSE Filesystem
# ────────────────────────────────────────────────────────────────────────────

# Check for FUSE3
find_package(PkgConfig)
if(PkgConfig_FOUND)
    pkg_check_modules(FUSE3 fuse3)
endif()

if(FUSE3_FOUND)
    # Collect FUSE sources
    file(GLOB FUSE_SOURCES "${L2_DIR}/src/*.c")

    # Create bereshit-fs executable
    add_executable(bereshit-fs ${FUSE_SOURCES})

    target_include_directories(bereshit-fs
        PRIVATE
            ${L2_DIR}/include
            ${L0_DIR}/include
            ${FUSE3_INCLUDE_DIRS}
    )

    target_link_libraries(bereshit-fs
        PRIVATE
            trit
            ${FUSE3_LIBRARIES}
    )

    target_compile_options(bereshit-fs PRIVATE ${FUSE3_CFLAGS_OTHER})

    message(STATUS "FUSE3 found - building bereshit-fs")
else()
    message(STATUS "FUSE3 not found - skipping bereshit-fs")
endif()

# ────────────────────────────────────────────────────────────────────────────
# Tests
# ────────────────────────────────────────────────────────────────────────────

# Enable testing
enable_testing()

# L0 Tests - libtrit
file(GLOB TRIT_TEST_SOURCES "${L0_DIR}/test/*_test.c")

foreach(TEST_SOURCE ${TRIT_TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    target_link_libraries(${TEST_NAME} PRIVATE trit)
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()

# L1 Tests - libomni
file(GLOB OMNI_TEST_SOURCES "${L1_DIR}/test/*.c")

foreach(TEST_SOURCE ${OMNI_TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    target_link_libraries(${TEST_NAME} PRIVATE omni trit)
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()

# ────────────────────────────────────────────────────────────────────────────
# Phase Demos (Optional)
# ────────────────────────────────────────────────────────────────────────────

option(BUILD_DEMOS "Build phase demonstration programs" ON)

if(BUILD_DEMOS)
    set(DEMO_DIR ${CMAKE_SOURCE_DIR}/tov/demo)

    # Phase 1: demo-trit
    if(EXISTS "${DEMO_DIR}/phase-1/demo-trit/main.c")
        add_executable(demo-trit "${DEMO_DIR}/phase-1/demo-trit/main.c")
        target_link_libraries(demo-trit PRIVATE trit)
    endif()

    # Phase 2: demo-math
    if(EXISTS "${DEMO_DIR}/phase-2/demo-math/main.c")
        add_executable(demo-math "${DEMO_DIR}/phase-2/demo-math/main.c")
        target_link_libraries(demo-math PRIVATE trit)
    endif()

    # Phase 2: demo-omni-parser
    if(EXISTS "${DEMO_DIR}/phase-2/demo-omni-parser/main.c")
        add_executable(demo-omni-parser "${DEMO_DIR}/phase-2/demo-omni-parser/main.c")
        target_link_libraries(demo-omni-parser PRIVATE omni trit)
    endif()

    # Phase 4: demo-omni-lexer
    if(EXISTS "${DEMO_DIR}/phase-4/demo-omni-lexer/main.c")
        add_executable(demo-omni-lexer "${DEMO_DIR}/phase-4/demo-omni-lexer/main.c")
        target_link_libraries(demo-omni-lexer PRIVATE omni trit)
    endif()

    # Phase 4: demo-omni-chunks
    if(EXISTS "${DEMO_DIR}/phase-4/demo-omni-chunks/main.c")
        add_executable(demo-omni-chunks "${DEMO_DIR}/phase-4/demo-omni-chunks/main.c")
        target_link_libraries(demo-omni-chunks PRIVATE omni trit)
    endif()

    # Phase 4: demo-omni-semantic
    if(EXISTS "${DEMO_DIR}/phase-4/demo-omni-semantic/main.c")
        add_executable(demo-omni-semantic "${DEMO_DIR}/phase-4/demo-omni-semantic/main.c")
        target_link_libraries(demo-omni-semantic PRIVATE omni trit)
    endif()

    message(STATUS "Demo programs enabled")
endif()

# ============================================================================
# CLOSING - Installation and Summary
# ============================================================================

# Installation rules
install(TARGETS trit omni
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(DIRECTORY ${L0_DIR}/include/
    DESTINATION include/trit
    FILES_MATCHING PATTERN "*.h"
)

install(DIRECTORY ${L1_DIR}/include/
    DESTINATION include/omni
    FILES_MATCHING PATTERN "*.h"
)

if(FUSE3_FOUND)
    install(TARGETS bereshit-fs
        RUNTIME DESTINATION bin
    )
endif()

# Summary
message(STATUS "")
message(STATUS "═══════════════════════════════════════════════════════════════")
message(STATUS "  Bereshit Build Configuration")
message(STATUS "═══════════════════════════════════════════════════════════════")
message(STATUS "  Version:      ${PROJECT_VERSION}")
message(STATUS "  Build Type:   ${CMAKE_BUILD_TYPE}")
message(STATUS "  C Compiler:   ${CMAKE_C_COMPILER}")
message(STATUS "  Install:      ${CMAKE_INSTALL_PREFIX}")
message(STATUS "")
message(STATUS "  Components:")
message(STATUS "    L0 libtrit: YES")
message(STATUS "    L1 libomni: YES")
message(STATUS "    L2 fuse:    ${FUSE3_FOUND}")
message(STATUS "    Demos:      ${BUILD_DEMOS}")
message(STATUS "")
message(STATUS "  To build: make")
message(STATUS "  To test:  make test")
message(STATUS "  To install: make install")
message(STATUS "═══════════════════════════════════════════════════════════════")
message(STATUS "")

# ============================================================================
# END
# ============================================================================
# "For every house is builded by some man; but he that built all things is God."
# — Hebrews 3:4 KJV
# ============================================================================
