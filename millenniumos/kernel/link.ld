/* =============================================================================
 * METADATA [METADATA]
 * =============================================================================
 *
 * Key:         MOS-kernel-link
 * Title:       MillenniumOS Kernel Linker Script
 * Type:        Linker Script
 * Component:   Stage 1 (Kernel)
 * Role:        Memory layout for kernel - places sections at correct addresses
 *
 * Status:      Active
 * Version:     a-01.00
 * Created:     2026-01-30
 * Updated:     2026-01-30
 *
 * Authors:     Seanje Lenox-Wise (Architect), Nova Dawn (Author)
 * Organization: CreativeWorkzStudio LLC
 *
 * Grounding:   Genesis 1:6-7 — "Let there be a firmament"
 *              The linker script separates and organizes - heaven from earth.
 *
 * =============================================================================
 * END METADATA
 * ============================================================================= */

/* Kernel loads at 1MB in physical memory */
ENTRY(_start)

/* =============================================================================
 * TERNARY-INSPIRED MEMORY LAYOUT
 * =============================================================================
 *
 * Memory organization follows ternary principles where practical:
 *   - Sections aligned to powers of 3 * page_size where possible
 *   - Zone-inspired organization: void (entry) → word (work) → tov (complete)
 *
 * Physical Layout:
 *   0x00000000 - 0x000FFFFF : Low memory (void zone - BIOS, boot)
 *   0x00100000 - ...        : Kernel (word zone - processing)
 *   Higher memory           : User space (tov zone - completed work)
 *
 */

KERNEL_PHYS_BASE = 0x100000;     /* 1MB physical (practical boundary) */
KERNEL_VIRT_BASE = 0xFFFFFFFF80000000;  /* Higher half (-2GB, ternary: negative space) */
PAGE_SIZE = 0x1000;              /* 4KB pages (standard) */

/* Ternary-inspired section sizes */
TRIT9_PAGES = 19683;             /* 3^9 pages theoretical max */
TRIT5_PAGES = 243;               /* 3^5 pages = ~1MB (practical kernel size) */

SECTIONS
{
    /* -------------------------------------------------------------------------
     * S.1: Kernel Code (.text)
     * -------------------------------------------------------------------------
     * Executable code, read-only.
     * Entry point first, then C code.
     */

    . = KERNEL_PHYS_BASE;

    .text ALIGN(PAGE_SIZE) : AT(ADDR(.text))
    {
        __text_start = .;

        /* Entry point first */
        *(.text.entry)
        *entry.o(.text)

        /* All other code */
        *(.text)
        *(.text.*)

        __text_end = .;
    }

    /* -------------------------------------------------------------------------
     * S.2: Read-Only Data (.rodata)
     * -------------------------------------------------------------------------
     * Constants, string literals, etc.
     */

    .rodata ALIGN(PAGE_SIZE) : AT(ADDR(.rodata))
    {
        __rodata_start = .;

        *(.rodata)
        *(.rodata.*)

        /* Include GNU hash for symbols */
        *(.gnu.hash)

        __rodata_end = .;
    }

    /* -------------------------------------------------------------------------
     * S.3: Initialized Data (.data)
     * -------------------------------------------------------------------------
     * Global variables with initial values.
     */

    .data ALIGN(PAGE_SIZE) : AT(ADDR(.data))
    {
        __data_start = .;

        *(.data)
        *(.data.*)

        __data_end = .;
    }

    /* -------------------------------------------------------------------------
     * S.4: Uninitialized Data (.bss)
     * -------------------------------------------------------------------------
     * Zero-initialized globals.
     * Cleared by kernel entry before calling C code.
     */

    .bss ALIGN(PAGE_SIZE) : AT(ADDR(.bss))
    {
        __bss_start = .;

        *(.bss)
        *(.bss.*)
        *(COMMON)

        __bss_end = .;
    }

    /* -------------------------------------------------------------------------
     * S.5: Kernel End Marker
     * -------------------------------------------------------------------------
     */

    . = ALIGN(PAGE_SIZE);
    __kernel_end = .;

    /* Calculate kernel size */
    __kernel_size = __kernel_end - KERNEL_PHYS_BASE;

    /* -------------------------------------------------------------------------
     * S.6: Discarded Sections
     * -------------------------------------------------------------------------
     * Debug info and other unnecessary sections.
     */

    /DISCARD/ :
    {
        *(.comment)
        *(.note)
        *(.note.*)
        *(.eh_frame)
        *(.eh_frame_hdr)
    }
}

/* =============================================================================
 * CLOSING [CLOSING]
 * =============================================================================
 *
 * Kernel memory layout (physical, before paging):
 *
 *   0x00000000 - 0x000FFFFF : Low memory (BIOS, video, etc.)
 *   0x00100000 - ...        : Kernel
 *       .text               : Code (RX)
 *       .rodata             : Read-only data (R)
 *       .data               : Initialized data (RW)
 *       .bss                : Zero-initialized (RW)
 *
 * "And God said, Let there be a firmament in the midst of the waters,
 *  and let it divide the waters from the waters."
 *  - Genesis 1:6
 *
 * The linker script divides and organizes:
 *   - Code separated from data
 *   - Read-only separated from writable
 *   - Initialized separated from zeroed
 *
 * Each section serves its purpose in proper boundaries.
 *
 * =============================================================================
 * END CLOSING
 * ============================================================================= */
