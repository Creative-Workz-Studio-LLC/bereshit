/* TEMPLATE: Remove this comment block when ready to link */
/* ═══════════════════════════════════════════════════════════════════════════
 * TEMPLATE: Linker Script (4-Block Structure)
 * Key: B-word-seed-code-linker-script
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * DEPENDENCY CLASSIFICATION: PURE (self-contained - no external dependencies)
 *   - Linker scripts are self-contained memory layout definitions
 *
 * This is a TEMPLATE file - copy and modify for new linker scripts.
 * Replace all [bracketed] placeholders with actual content.
 * Rename to "linker.ld" when ready to use.
 * Remove the "TEMPLATE" comment at the top when ready.
 *
 * derives_from: bereshit/word/omni/seed/code.omni
 * Derived from: Kingdom Technology 4-block code structure
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
 * ────────────────────────────────────────────────────────────────
 * CORE IDENTITY (Required)
 * ────────────────────────────────────────────────────────────────
 *
 * # Biblical Foundation
 *
 * Scripture: [Relevant verse grounding this linker script's purpose]
 *
 * Principle: [Kingdom principle this memory layout demonstrates]
 *
 * Anchor: [Supporting verse reinforcing the principle]
 *
 * # CPI-SI Identity
 *
 * Component Type: Rails (foundational memory layout)
 *
 * Role: [Specific responsibility - defines memory organization for target]
 *
 * Paradigm: CPI-SI framework component
 *
 * # Authorship & Lineage
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
 * # Purpose & Function
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
 * INTERFACE (Expected)
 * ────────────────────────────────────────────────────────────────
 *
 * # Dependencies
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
 * # Usage & Integration
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
 * OPERATIONAL (Contextual)
 * ────────────────────────────────────────────────────────────────
 *
 * # Blocking Status
 *
 * [OMIT: Linker scripts are compile-time only - no runtime execution]
 *
 * # Health Scoring
 *
 * [OMIT: Linker scripts don't track health - link errors cause build failure]
 *
 * ────────────────────────────────────────────────────────────────
 * METADATA Omission Guide
 * ────────────────────────────────────────────────────────────────
 *
 * Tier 1 (CORE IDENTITY): Never omit - every file needs these.
 *
 * Tier 2 (INTERFACE): May omit with [OMIT: reason] notation.
 *   - Dependencies: Required - documents linker and target architecture
 *   - Usage & Integration: Required - shows link commands
 *
 * Tier 3 (OPERATIONAL): Include when applicable to file type.
 *   - Blocking Status: [OMIT: Linker scripts are compile-time only]
 *   - Health Scoring: [OMIT: Link errors cause build failure, not health degradation]
 *
 * Unlike SETUP (all sections required), METADATA omission signals component characteristics.
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
 * ────────────────────────────────────────────────────────────────
 * SETUP Sections Overview
 * ────────────────────────────────────────────────────────────────
 *
 * 1. ENTRY POINT (Dependencies)
 *    Purpose: Establish program entry point symbol
 *    Subsections: Entry Symbol → Architecture Target (if needed)
 *
 * 2. OUTPUT FORMAT (Constants)
 *    Purpose: Fixed output format specification
 *    Subsections: Output Format → Output Architecture
 *
 * 3. MEMORY REGIONS (Variables)
 *    Purpose: Define memory layout for target platform
 *    Subsections: ROM Region → RAM Region → Special Regions
 *
 * 4. SYMBOLS (Types)
 *    [Reserved: Linker scripts define symbols in SECTIONS, not as types]
 *
 * 5. ASSERTIONS (Type Behaviors)
 *    [Reserved: Linker assertions typically placed after SECTIONS in CLOSING]
 *
 * 6. CONFIGURATION (Rails Infrastructure)
 *    [Reserved: Linker scripts ARE the configuration - no meta-configuration]
 *
 * Section order: Entry Point → Output Format → Memory Regions → [Reserved] → [Reserved] → [Reserved]
 * This flows: where to start → what format → where things go → (sections in BODY)
 *
 * Universal mapping (see standards for cross-language patterns):
 *   Entry Point ≈ Dependencies (what linker needs first)
 *   Output Format ≈ Constants (fixed format settings)
 *   Memory Regions ≈ Variables (memory definitions vary by target)
 *   Symbols ≈ Types [Reserved: defined in SECTIONS]
 *   Assertions ≈ Type Behaviors [Reserved: validation in CLOSING]
 *   Configuration ≈ Package-Level State [Reserved: script IS configuration]
 *
 * ────────────────────────────────────────────────────────────────
 * Entry Point (Dependencies)
 * ────────────────────────────────────────────────────────────────
 *
 * Defines the program entry point symbol.
 * This is what the linker needs to know FIRST.
 * ============================================================================ */

ENTRY([_start])

/* ────────────────────────────────────────────────────────────────
 * Output Format (Constants)
 * ────────────────────────────────────────────────────────────────
 *
 * Fixed output format specification.
 * Common formats: elf32-i386, elf64-x86-64, binary
 * ============================================================================ */

/* OUTPUT_FORMAT([elf32-i386]) */

/* ────────────────────────────────────────────────────────────────
 * Memory Regions (Variables)
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

/* ────────────────────────────────────────────────────────────────
 * Symbols (Types)
 * ────────────────────────────────────────────────────────────────
 *
 * [Reserved: Linker scripts define symbols within SECTIONS block in BODY.
 *  Symbols like _start, _end, _bss_start are placed contextually
 *  where they mark section boundaries, not as standalone types.]
 * ============================================================================ */

/* ────────────────────────────────────────────────────────────────
 * Assertions (Type Behaviors)
 * ────────────────────────────────────────────────────────────────
 *
 * [Reserved: ASSERT() statements validate linker assumptions.
 *  Typically placed after SECTIONS block in CLOSING for validation.
 *  Example: ASSERT(_end - _start < 64K, "Binary too large")]
 * ============================================================================ */

/* ────────────────────────────────────────────────────────────────
 * Configuration (Rails Infrastructure)
 * ────────────────────────────────────────────────────────────────
 *
 * [Reserved: Linker scripts ARE the configuration infrastructure.
 *  There is no meta-configuration layer for linker scripts themselves.
 *  The entire script serves as Rails for the build system.]
 * ============================================================================ */

/* ────────────────────────────────────────────────────────────────
 * SETUP Omission Guide
 * ────────────────────────────────────────────────────────────────
 *
 * Unlike METADATA (where sections can be omitted with [OMIT: reason]),
 * ALL six SETUP sections must be present for structural alignment.
 *
 * If a section has no content for this file:
 *   - Keep the section header
 *   - Add [Reserved: reason] comment explaining why empty
 *   - This maintains the 6-section structure across all templates
 *
 * Linker script-specific guidance:
 *   - Entry Point: Always required (ENTRY directive)
 *   - Output Format: Use [Reserved: Using default format] if omitting OUTPUT_FORMAT
 *   - Memory Regions: Use [Reserved: Inline in SECTIONS] for simple layouts
 *   - Symbols: [Reserved: Defined in SECTIONS block]
 *   - Assertions: [Reserved: Validation in CLOSING]
 *   - Configuration: [Reserved: Script IS configuration]
 *
 * The goal is structural consistency - every linker template has the same
 * 6-section SETUP structure, making navigation and understanding predictable.
 * ============================================================================ */

/* ============================================================================
 * END SETUP
 * ============================================================================ */

/* ============================================================================
 * BODY
 * ============================================================================
 *
 * For BODY structure explanation, see: standards/code/4-block/CWS-STD-007-CODE-body-block.md
 *
 * -----------------------------------------------------------------------------
 * BODY Sections Overview
 * -----------------------------------------------------------------------------
 *
 * 1. ORGANIZATIONAL CHART (Section Structure Map)
 *    Purpose: Document memory layout - sections, addresses, relationships
 *    Subsections: Load Address → Section Diagram → Memory Regions
 *
 * 2. HELPERS/UTILITIES (Symbol Definitions)
 *    [Reserved: Symbols defined inline within SECTIONS where they mark boundaries]
 *
 * 3. CORE OPERATIONS (SECTIONS Block)
 *    Purpose: Define the actual memory sections - the core linker script work
 *    Subsections: Base Address → .text → .rodata → .data → .bss → /DISCARD/
 *
 * 4. ERROR HANDLING/RECOVERY (Assertions)
 *    [Reserved: ASSERT() statements placed in CLOSING for post-layout validation]
 *
 * 5. PUBLIC APIs (Exported Symbols)
 *    [Reserved: Entry point (ENTRY) in SETUP, section symbols defined contextually]
 *
 * Section order: Org Chart → [Reserved] → SECTIONS → [Reserved] → [Reserved]
 * This flows: understand layout → define sections → validate (in CLOSING)
 *
 * Universal mapping (see standards for cross-language patterns):
 *   Organizational Chart ≈ Section Documentation (memory layout map)
 *   Helpers/Utilities ≈ Symbol Definitions [Reserved: inline in SECTIONS]
 *   Core Operations ≈ SECTIONS Block (actual memory definitions)
 *   Error Handling ≈ Assertions [Reserved: validation in CLOSING]
 *   Public APIs ≈ Entry Point [in SETUP] + Exported Symbols [contextual]
 *
 * Note: Linker scripts are declarative - they DESCRIBE memory layout,
 * they don't execute. The SECTIONS block IS the core operation.
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

/* -----------------------------------------------------------------------------
 * BODY Omission Guide
 * -----------------------------------------------------------------------------
 *
 * ALL five sections MUST be present. Content may be reserved with reason:
 *
 *   - Organizational Chart: Rarely reserved - section layout benefits from map
 *   - Helpers/Utilities: [Reserved: Symbols defined inline within SECTIONS]
 *   - Core Operations: Rarely reserved - SECTIONS block is the core work
 *   - Error Handling: [Reserved: ASSERT() placed in CLOSING for post-layout validation]
 *   - Public APIs: [Reserved: ENTRY() in SETUP, section symbols inline]
 *
 * Unlike METADATA (sections omitted entirely with [OMIT:]), BODY preserves
 * all section headers with [Reserved:] notation for unused sections.
 *
 * For linker scripts:
 *   - Section Map documents memory layout visually
 *   - Symbols defined contextually within SECTIONS (not separate section)
 *   - SECTIONS block defines actual memory sections
 *   - Validation via ASSERT() happens in CLOSING after layout complete
 *   - Entry point via ENTRY() belongs in SETUP (configuration)
 */

/* ============================================================================
 * END BODY
 * ============================================================================ */

/* ============================================================================
 * CLOSING
 * ============================================================================
 *
 * For CLOSING structure explanation, see: standards/code/4-block/CWS-STD-008-CODE-closing-block.md
 *
 * -----------------------------------------------------------------------------
 * CLOSING Sections Overview
 * -----------------------------------------------------------------------------
 *
 * GROUP 1: CODING (Operations - Verify, Link, Clean)
 *
 * 1. CODE VALIDATION (Link Verification)
 *    Purpose: Prove correctness - parse, link test objects, verify layout
 *    Subsections: Syntax Verification → Link Testing → Section Verification
 *
 * 2. CODE EXECUTION (Linker Processing)
 *    [Reserved: Linker scripts don't execute - they're processed by ld]
 *
 * 3. CODE CLEANUP (Post-Link Assertions)
 *    Purpose: ASSERT() statements validating memory layout after linking
 *    Subsections: Alignment Assertions → Size Assertions → Boundary Assertions
 *
 * GROUP 2: FINAL DOCUMENTATION (Synthesis - Reference Back to Earlier Blocks)
 *
 * 4. LINKER SCRIPT OVERVIEW (Summary with Back-References)
 *    Purpose: High-level summary of memory layout this script produces
 *    References: METADATA "Purpose & Function", "Key Features"
 *
 * 5. MODIFICATION POLICY (Safe/Careful/Never)
 *    Purpose: Guide future maintainers on what's safe to change
 *    Subsections: Safe to Modify → Modify with Care → Never Modify (ABI)
 *
 * 6. LADDER AND BATON FLOW (Back-Reference to BODY)
 *    Purpose: Point to BODY Section Structure Map
 *    References: BODY "Organizational Chart - Section Structure Map"
 *
 * 7. SURGICAL UPDATE POINTS (Back-Reference to BODY)
 *    Purpose: Point to BODY SECTIONS block for adding sections
 *    References: BODY "Core Operations - SECTIONS Block"
 *
 * 8. PERFORMANCE CONSIDERATIONS (Memory Layout)
 *    Purpose: Alignment effects, section ordering for cache/locality
 *    References: BODY section definitions with alignment annotations
 *
 * 9. TROUBLESHOOTING GUIDE (Link Issues)
 *    Purpose: Common linker problems and solutions
 *    Subsections: Undefined Symbols → Section Overflow → Alignment Errors
 *
 * 10. RELATED COMPONENTS (Build Chain)
 *     Purpose: Point to related build components (assembler output, loader)
 *     References: METADATA "Dependencies" - object files, bootloader
 *
 * 11. FUTURE EXPANSIONS (Layout Roadmap)
 *     Purpose: Planned sections, platform support, features
 *     Subsections: Planned Sections → Platform Support → Feature Additions
 *
 * 12. CONTRIBUTION GUIDELINES (Adding Sections)
 *     Purpose: How to add new sections to this linker script
 *     Subsections: Section Naming → Alignment → Documentation
 *
 * 13. QUICK REFERENCE (Link Commands)
 *     Purpose: Copy-paste ready link commands
 *     Subsections: Basic Link → With Map → Verbose → Debug
 *
 * Section order: Validation → [Execution Reserved] → Cleanup → Overview → Policy → Ladder/Baton →
 *                Surgical → Performance → Troubleshooting → Related → Future → Contribution → Reference
 * This flows: verify → assert layout → document → guide expansion
 *
 * ════════════════════════════════════════════════════════════════
 * GROUP 1: CODING
 * ════════════════════════════════════════════════════════════════
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
 * -----------------------------------------------------------------------------
 * CLOSING Omission Guide
 * -----------------------------------------------------------------------------
 *
 * ALL thirteen sections MUST be present. Content may be reserved with reason:
 *
 * GROUP 1: CODING
 *   - Code Validation: Link verification and section checking
 *   - Code Execution: [Reserved: Linker scripts don't execute - processed by ld]
 *   - Code Cleanup: Post-link ASSERT() validation
 *
 * GROUP 2: FINAL DOCUMENTATION (mostly back-references)
 *   - Linker Script Overview: Summary of memory layout
 *   - Modification Policy: Guide for modifying sections safely (ABI critical)
 *   - Ladder and Baton Flow: Back-reference to BODY section structure
 *   - Surgical Update Points: Where to add new sections
 *   - Performance Considerations: Alignment and section ordering
 *   - Troubleshooting Guide: Common linker issues
 *   - Related Components: Related build chain components
 *   - Future Expansions: [Reserved: Layout complete, no planned sections]
 *   - Contribution Guidelines: How to add new sections
 *   - Quick Reference: Common link commands
 *
 * Unlike BODY (which uses [Reserved:] inline), CLOSING sections can be
 * entirely replaced with back-references to avoid duplication.
 *
 * The key principle: CLOSING synthesizes, METADATA/SETUP/BODY contain details.
 * Don't repeat - reference back to where the information lives.
 *
 * ============================================================================
 * END CLOSING
 * ============================================================================ */
