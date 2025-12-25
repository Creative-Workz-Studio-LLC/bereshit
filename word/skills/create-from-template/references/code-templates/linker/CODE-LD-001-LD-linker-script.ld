/* ═══════════════════════════════════════════════════════════════════════════
 * TEMPLATE: Linker Script (4-Block Structure)
 * Key: CODE-LD-001
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * DEPENDENCY CLASSIFICATION: PURE (self-contained - no external dependencies)
 *   - Linker scripts are self-contained memory layout definitions
 *
 * This is a TEMPLATE file - copy and modify for new linker scripts.
 * Replace all [bracketed] placeholders with actual content.
 * Rename to "linker.ld" when ready to use.
 *
 * See: standards/code/4-block/ for complete documentation
 *
 * ═══════════════════════════════════════════════════════════════════════════ */

/* ============================================================================
 * METADATA
 * ============================================================================
 *
 * Package:     [organization/project-name]
 * File:        linker.ld
 * Key:         [PROJECT-LINK-###] (Linker script for [target])
 *
 * For METADATA structure explanation, see: standards/code/4-block/CWS-STD-004-CODE-metadata-block.md
 *
 * ────────────────────────────────────────────────────────────────
 * Biblical Foundation
 * ────────────────────────────────────────────────────────────────
 *
 * Scripture: [Relevant verse grounding this linker script's purpose]
 *
 * Principle: [Kingdom principle this memory layout demonstrates]
 *
 * Anchor: [Supporting verse reinforcing the principle]
 *
 * ────────────────────────────────────────────────────────────────
 * CPI-SI Identity
 * ────────────────────────────────────────────────────────────────
 *
 * Component Type: Rails (foundational memory layout)
 *
 * Role: [Specific responsibility - defines memory organization for target]
 *
 * Paradigm: CPI-SI framework component
 *
 * ────────────────────────────────────────────────────────────────
 * Authorship & Lineage
 * ────────────────────────────────────────────────────────────────
 *
 *   Architect: [Who designed the memory layout]
 *   Implementation: [Who wrote the linker script]
 *   Created: [YYYY-MM-DD]
 *   Version: [MAJOR.MINOR.PATCH]
 *   Modified: [YYYY-MM-DD - what changed]
 *
 * Version History:
 *
 *   [X.Y.Z] ([YYYY-MM-DD]) - [Brief description of changes]
 *
 * ────────────────────────────────────────────────────────────────
 * Purpose & Function
 * ────────────────────────────────────────────────────────────────
 *
 * Purpose: [What memory layout does this script define?]
 *
 * Core Design: [Target architecture - bare metal, OS, embedded, etc.]
 *
 * Key Features:
 *
 *   - [Section organization - .text, .data, .bss, etc.]
 *   - [Memory regions - ROM, RAM, stack, heap]
 *   - [Special requirements - alignment, placement]
 *
 * Philosophy: [Guiding principle for this memory layout]
 *
 * ────────────────────────────────────────────────────────────────
 * Blocking Status
 * ────────────────────────────────────────────────────────────────
 *
 * Non-blocking: Linker scripts are compile-time only.
 *
 * Mitigation: Link errors caught at build time.
 *
 * ────────────────────────────────────────────────────────────────
 * Usage & Integration
 * ────────────────────────────────────────────────────────────────
 *
 * Link Command:
 *
 *   ld -T linker.ld [object files] -o [output]
 *   gcc -Wl,-T,linker.ld [object files] -o [output]
 *
 * Integration Pattern:
 *
 *   1. Compile source files to object files
 *   2. Link with this script to produce final binary
 *   3. Binary layout matches this script's definitions
 *
 * ────────────────────────────────────────────────────────────────
 * Dependencies
 * ────────────────────────────────────────────────────────────────
 *
 * What This Needs:
 *
 *   - Linker: GNU ld or compatible
 *   - Target: [Architecture - x86, ARM, etc.]
 *
 * What Uses This:
 *
 *   - Makefile: Links final binary
 *   - Build system: Produces executable/firmware
 *
 * Integration Points:
 *
 *   - Object files: Input to linker
 *   - Output binary: Result of linking
 *   - Bootloader: Loads binary at expected address
 *
 * ────────────────────────────────────────────────────────────────
 * Health Scoring
 * ────────────────────────────────────────────────────────────────
 *
 * Health Impact: None - linker scripts are compile-time only.
 *
 * Note: Link errors mean build failure, not health degradation.
 *
 * ============================================================================
 * END METADATA
 * ============================================================================ */

/* ============================================================================
 * SETUP
 * ============================================================================
 *
 * For SETUP structure explanation, see: standards/code/4-block/CWS-STD-006-CODE-setup-block.md
 *
 * Section order: Entry Point → Output Format → Memory Regions
 * This flows: where to start → what format → where things go
 *
 * ────────────────────────────────────────────────────────────────
 * Entry Point
 * ────────────────────────────────────────────────────────────────
 *
 * Defines the program entry point symbol.
 * ============================================================================ */

ENTRY([_start])

/* ────────────────────────────────────────────────────────────────
 * Output Format (Optional)
 * ────────────────────────────────────────────────────────────────
 *
 * Specify output format if not using default.
 * Common formats: elf32-i386, elf64-x86-64, binary
 * ============================================================================ */

/* OUTPUT_FORMAT([elf32-i386]) */

/* ────────────────────────────────────────────────────────────────
 * Memory Regions (Optional)
 * ────────────────────────────────────────────────────────────────
 *
 * Define memory regions for embedded/bare-metal targets.
 * Not needed for hosted (OS) environments.
 *
 * Example:
 *   MEMORY
 *   {
 *       ROM (rx)  : ORIGIN = 0x00000000, LENGTH = 256K
 *       RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 64K
 *   }
 * ============================================================================ */

/* [Reserved: Memory regions defined inline in SECTIONS for simple layouts] */

/* ============================================================================
 * END SETUP
 * ============================================================================ */

/* ============================================================================
 * BODY
 * ============================================================================
 *
 * For BODY structure explanation, see: standards/code/4-block/CWS-STD-007-CODE-body-block.md
 *
 * ────────────────────────────────────────────────────────────────
 * Organizational Chart - Section Structure
 * ────────────────────────────────────────────────────────────────
 *
 * Section Layout:
 *
 *   Load Address: [0x????????]
 *   │
 *   ├── .text (code)
 *   │   ├── *(.text)
 *   │   └── *(.text.*)
 *   │
 *   ├── .rodata (read-only data)
 *   │   └── *(.rodata)
 *   │
 *   ├── .data (initialized data)
 *   │   └── *(.data)
 *   │
 *   └── .bss (uninitialized data)
 *       └── *(.bss)
 *
 * ============================================================================ */

SECTIONS
{
    /* ────────────────────────────────────────────────────────────────
     * Base Address
     * ────────────────────────────────────────────────────────────────
     *
     * Set the location counter to the load address.
     * For kernels: typically where bootloader loads the binary.
     * For firmware: typically start of ROM.
     * ================================================================ */

    . = [0x????????];

    /* ────────────────────────────────────────────────────────────────
     * .text Section - Executable Code
     * ────────────────────────────────────────────────────────────────
     *
     * Contains executable instructions.
     * Typically read-only, execute-enabled.
     * ================================================================ */

    .text : {
        *(.text)
        *(.text.*)
    }

    /* ────────────────────────────────────────────────────────────────
     * .rodata Section - Read-Only Data
     * ────────────────────────────────────────────────────────────────
     *
     * Contains constant data (strings, lookup tables).
     * Read-only, no execute.
     * ================================================================ */

    .rodata : {
        *(.rodata)
        *(.rodata.*)
    }

    /* ────────────────────────────────────────────────────────────────
     * .data Section - Initialized Data
     * ────────────────────────────────────────────────────────────────
     *
     * Contains initialized variables.
     * Read-write.
     * ================================================================ */

    .data : {
        *(.data)
        *(.data.*)
    }

    /* ────────────────────────────────────────────────────────────────
     * .bss Section - Uninitialized Data
     * ────────────────────────────────────────────────────────────────
     *
     * Contains uninitialized variables.
     * Zero-initialized at startup.
     * Does not take space in binary file.
     * ================================================================ */

    .bss : {
        *(.bss)
        *(.bss.*)
        *(COMMON)
    }

    /* ────────────────────────────────────────────────────────────────
     * Discard Sections (Optional)
     * ────────────────────────────────────────────────────────────────
     *
     * Sections to discard (debug info, comments, etc.)
     * ================================================================ */

    /DISCARD/ : {
        *(.comment)
        *(.note*)
    }
}

/* ============================================================================
 * END BODY
 * ============================================================================ */

/* ============================================================================
 * CLOSING
 * ============================================================================
 *
 * For CLOSING structure explanation, see: standards/code/4-block/CWS-STD-008-CODE-closing-block.md
 *
 * ────────────────────────────────────────────────────────────────
 * Code Validation
 * ────────────────────────────────────────────────────────────────
 *
 * Build Verification:
 *   ld -T linker.ld --verbose  # Verify script parses correctly
 *
 * Testing Requirements:
 *   - Link test objects to verify section placement
 *   - Use objdump -h to verify section addresses
 *   - For bare metal: verify entry point is correct
 *
 * ────────────────────────────────────────────────────────────────
 * Modification Policy
 * ────────────────────────────────────────────────────────────────
 *
 * Safe to Modify:
 *   + Add new sections for specific needs
 *   + Adjust alignment requirements
 *   + Add symbols for runtime use (_end, _bss_start, etc.)
 *
 * Modify with Extreme Care:
 *   ! Base address - must match bootloader expectations
 *   ! Section order - .text before .data typically required
 *   ! Entry point - must match actual entry symbol
 *
 * NEVER Modify:
 *   X 4-block structure
 *   X Remove required sections (.text, .data, .bss)
 *
 * ────────────────────────────────────────────────────────────────
 * Related Components
 * ────────────────────────────────────────────────────────────────
 *
 * Dependencies:
 *   - Makefile: Invokes linker with this script
 *   - Bootloader: Loads binary at expected address
 *   - Entry code: Must define ENTRY symbol
 *
 * ────────────────────────────────────────────────────────────────
 * Closing Note
 * ────────────────────────────────────────────────────────────────
 *
 * This linker script is FOUNDATIONAL RAILS - defines where code
 * and data live in memory. Changes here affect the entire binary.
 *
 * "[Scripture verse]" - [Reference]
 *
 * ============================================================================
 * END CLOSING
 * ============================================================================ */
