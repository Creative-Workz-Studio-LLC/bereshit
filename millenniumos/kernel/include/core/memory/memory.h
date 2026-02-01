/// @file memory.h
/// @omni code --c --header
///
/// =============================================================================
/// OMNICODE PRAGMA [PRAGMA]
/// =============================================================================
///
/// @legend
///   :req  REQUIRED   — must exist, validation fails without
///   :inh  INHERITED  — from template, override allowed
///   :ins  INSTANCE   — file-specific, unique values
/// @endlegend
///
/// ---------------------------------------------------------------------------
/// P.1 CORE — Identity [CORE]
/// ---------------------------------------------------------------------------
///
/// @omni:req  key        = MOS-kernel-core-memory
/// @omni:req  from       = bereshit/word/seed/code/c/type-header.h
/// @omni:req  at         = a-02.00
///
/// ---------------------------------------------------------------------------
/// P.2 FAMILY — Classification [FAMILY]
/// ---------------------------------------------------------------------------
///
/// @omni:inh  type       = header
/// @omni:inh  subtype    = api
/// @omni:inh  role       = subsystem
/// @omni:inh  structure  = 4-block
///
/// ---------------------------------------------------------------------------
/// P.3 INSTANCE — File Details [INSTANCE]
/// ---------------------------------------------------------------------------
///
/// @omni:ins  component  = kernel/core/memory
/// @omni:ins  layer      = 0 (kernel layer)
/// @omni:ins  cpisi_mode = 0 (freestanding)
/// @omni:ins  provides   = [MosPageFrame, MosMemoryManager, MosBuddyAllocator,
///                          frame_alloc, kmalloc, vmap, page_table_ops]
///
/// ---------------------------------------------------------------------------
/// P.4 ARCHITECTURE — Scaling Context [ARCH]
/// ---------------------------------------------------------------------------
///
/// @omni:ins  layers     = [0:kernel, 1:hal, 2:services, 3:framework, 4:app]
/// @omni:ins  position   = 0 (kernel foundation)
/// @omni:ins  depends    = [kernel.h, bootinfo.h]
///
/// ---------------------------------------------------------------------------
/// P.5 SUMMARY — Human Identity [SUMMARY]
/// ---------------------------------------------------------------------------
///
/// @omni:req  title      = MillenniumOS Memory Management
/// @omni:req  brief      = Physical/virtual memory with ternary addressing
///
/// =============================================================================
/// END PRAGMA
/// =============================================================================

// =============================================================================
// METADATA BLOCK [METADATA]
// =============================================================================
//
// @brief Identity and context for memory subsystem.
//
// STRUCTURE: M.1-M.10 sections for complete documentation.
//
// =============================================================================

/**
 * =============================================================================
 * M.1 IDENTITY — Core Identification [IDENTITY]
 * =============================================================================
 *
 * @file      memory.h
 * @brief     MillenniumOS Memory Management — Physical and virtual memory.
 *
 * Key:       MOS-kernel-core-memory
 * Title:     MillenniumOS Memory Management
 * Type:      Header (API)
 * Component: Kernel / Core / Memory Subsystem
 * Role:      Physical frame allocation, virtual mapping, kernel heap
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * M.2 STATE — Lifecycle State [STATE]
 * =============================================================================
 *
 * @version   a-02.00
 * @date      2026-02-01
 *
 * Status:    Active
 * Created:   2026-01-30
 * Updated:   2026-02-01
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * M.3 ATTRIBUTION — Authorship & Rights [ATTRIBUTION]
 * =============================================================================
 *
 * @author    Seanje Lenox-Wise (Architect)
 * @author    Nova Dawn (Implementation)
 * @copyright © 2026 CreativeWorkzStudio LLC. All rights reserved.
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * M.4 LOCATION — File Position [LOCATION]
 * =============================================================================
 *
 * Path:      millenniumos/kernel/include/core/memory/memory.h
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * M.5 DERIVATION — Template Lineage [DERIVATION]
 * =============================================================================
 *
 * Derives:   bereshit/word/seed/code/c/header.h (4-block C template)
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * M.6 CLASSIFICATION — Categorization [CLASSIFICATION]
 * =============================================================================
 *
 * Tags:      kernel, memory, frames, heap, paging, ternary, buddy, slab
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * M.7 INTENT — Purpose Statement [INTENT]
 * =============================================================================
 *
 * Purpose:   Provide physical frame allocation, virtual address mapping,
 *            and kernel heap services using ternary-inspired addressing.
 *            Includes buddy allocator for contiguous pages, slab allocator
 *            for frequent object sizes, and page table manipulation.
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * M.8 GROUNDING — Biblical Foundation [GROUNDING]
 * =============================================================================
 *
 * Scripture: Genesis 1:9 — "Let the waters under the heaven be gathered
 *            together unto one place, and let the dry land appear."
 *
 * Principle: Memory management gathers and organizes physical resources,
 *            creating solid ground upon which processes can execute.
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * M.9 DEPENDENCIES — Required Components [DEPENDENCIES]
 * =============================================================================
 *
 * Depends:
 *   - kernel.h           — Core types (MUST be first)
 *   - bootinfo.h         — Boot handoff structure
 *
 * Used by:
 *   - kernel.c           — Memory initialization
 *   - process.c          — Process address spaces
 *   - hal/hal.c          — DMA buffers
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * M.10 ROADMAP — Version History [ROADMAP]
 * =============================================================================
 *
 * History:
 *   a-01.00 (2026-01-30) — Initial memory subsystem
 *   a-01.50 (2026-02-01) — Full OmniCode template alignment
 *   a-02.00 (2026-02-01) — M.10 implementation: buddy, slab, page tables
 *
 * Completed:
 *   ✓ Frame allocation with early bitmap allocator
 *   ✓ Ternary zone classification (DMA/normal/high)
 *   ✓ Simple bump allocator for kernel heap
 *   ✓ Memory health tracking
 *   ✓ Boot memory map integration
 *   ✓ Page table manipulation API (vmap/vunmap)
 *   ✓ Buddy allocator for contiguous allocation
 *   ✓ Slab allocator for kernel heap
 *   ✓ Memory pressure callbacks
 *   ✓ Full X.1-X.9 CLOSING block
 *
 * Planned:
 *   ⏳ NUMA-aware allocation (multi-socket support)
 *   ⏳ Huge page (2MB/1GB) support
 *   ⏳ Memory hotplug support
 *
 * =============================================================================
 */

// =============================================================================
// END METADATA
// =============================================================================

#ifndef MOS_KERNEL_CORE_MEMORY_H
#define MOS_KERNEL_CORE_MEMORY_H

// =============================================================================
// SETUP BLOCK [SETUP]
// =============================================================================
//
// @brief Configuration and dependencies before body.
//
// CONTAINS:
//   - S.1 INCLUDES   — Header dependencies (kernel.h, bootinfo.h)
//   - S.2 DEFINES    — Domain constants (page, pool, zone, frame, buddy, slab, PTE)
//   - S.3 TYPES      — Type declarations (enums, structs)
//   - S.4 PROTOTYPES — Function declarations (lifecycle, accessors, operations)
//   - S.5 FILE-LEVEL — [Reserved: no file-level state]
//   - S.6 OMISSION   — Reserved section guide
//
// TYPE HEADER STRUCTURE:
//   S.3 contains full type definitions, S.4 contains all function prototypes.
//   BODY is reserved (all declarations in SETUP for type-header pattern).
//
// =============================================================================

/**
 * =============================================================================
 * S.1 INCLUDES — Header Dependencies [INCLUDES]
 * =============================================================================
 *
 * @defgroup memory_includes S.1 INCLUDES — Header Dependencies
 * @brief    Standard, internal, and external library includes.
 *
 * CONTAINS:
 *   - S.1a STANDARD  — [Reserved: freestanding kernel]
 *   - S.1b INTERNAL  — Project headers (kernel.h, bootinfo.h)
 *   - S.1c EXTERNAL  — [Reserved: none]
 *   - S.1d CPP       — [Reserved: kernel C code]
 *
 * =============================================================================
 * @{ */

/** S.1a STANDARD — [Reserved: freestanding kernel, no libc] @{ @} */

/**
 * ---------------------------------------------------------------------------
 * S.1b INTERNAL — Project Headers [INTERNAL]
 * ---------------------------------------------------------------------------
 *
 * @defgroup memory_internal S.1b INTERNAL — Project Headers
 * @ingroup memory_includes
 * @brief    Internal kernel headers required by this module.
 * @{ */

#include "kernel.h"                   /**< LAYER 0: Foundation (MUST be first) */
#include "boot/include/bootinfo.h"    /**< Boot handoff structure */

/** @} */ /* end S.1b INTERNAL */

/** S.1c EXTERNAL — [Reserved: no external dependencies] @{ @} */

/** S.1d CPP — [Reserved: kernel C code] @{ @} */

/** @} */ /* end S.1 INCLUDES */

/**
 * =============================================================================
 * S.2 DEFINES — Memory Domain Definitions [DEFINES]
 * =============================================================================
 *
 * @defgroup memory_domain S.2 DEFINES — Memory Domain
 * @brief    Memory management constants and configuration.
 *
 * CONTAINS:
 *   - S.2.0 PRELUDE — [Reserved: uses kernel.h primitives]
 *   - S.2a  BOUNDS  — Page, pool, zone, frame, buddy, slab, paging constants
 *   - S.2b  MACROS  — [Reserved: operations in kernel.h]
 *   - S.2c  VERIFY  — Compile-time assertions (in CLOSING X.1)
 *   - S.2d  LOOKUP  — [Reserved: no runtime tables in header]
 *   - S.2e  TYPES   — [Reserved: main types in BODY]
 *
 * =============================================================================
 * @{ */

/** S.2.0 PRELUDE — [Reserved: uses kernel.h primitives] @{ @} */

/**
 * ---------------------------------------------------------------------------
 * S.2a BOUNDS — Memory Domain Bounds [BOUNDS]
 * ---------------------------------------------------------------------------
 *
 * @defgroup memory_bounds S.2a BOUNDS — Memory Domain Bounds
 * @ingroup memory_domain
 * @brief    Counts, ranges, limits for memory subsystem.
 *
 * STRUCTURE:
 *   - Page constants (size, shift, mask)
 *   - Pool sizes (ternary-inspired)
 *   - Zone classification (ternary)
 *   - Frame state encoding (trit5)
 *   - Buddy allocator configuration
 *   - Slab allocator configuration
 *   - Page table entry flags
 *
 * ---------------------------------------------------------------------------
 * @{ */

/** @name Page Constants — Fundamental memory unit
 *  @brief MOS_PAGE_SIZE (4KB), shift, mask.
 *  @{ */
#define MOS_PAGE_SIZE       4096      /**< 4KB: standard x86-64 page size */
#define MOS_PAGE_SHIFT      12        /**< log2(4096): address → page conversion */
#define MOS_PAGE_MASK       0xFFF     /**< Lower 12 bits: offset within page */
/** @} */

/** @name Pool Sizes — Ternary-Inspired Frame Pools
 *  @brief 3^5, 3^7, 3^9 frame counts for kernel allocations.
 *  @{ */
#define MOS_FRAME_POOL_SMALL    243   /**< 3^5: ~1MB for initial heap */
#define MOS_FRAME_POOL_MEDIUM   2187  /**< 3^7: ~9MB for larger allocations */
#define MOS_FRAME_POOL_LARGE    19683 /**< 3^9: ~80MB for substantial regions */
/** @} */

/** @name Zone Classification — Ternary Memory Zones
 *  @brief DMA(-1), Normal(0), High(+1) using trit semantics.
 *  @{ */
#define MOS_MEMZONE_DMA     TRIT_NEG  /**< -1: <16MB, ISA DMA accessible */
#define MOS_MEMZONE_NORMAL  TRIT_ZERO /**< 0: 16MB-4GB, standard memory */
#define MOS_MEMZONE_HIGH    TRIT_POS  /**< +1: >4GB, 64-bit addressing */
/** @} */

/** @name Frame State — Trit5 Encoding
 *  @brief [alloc, zone, dirty, accessed, locked] state encoding.
 *  @{ */
#define MOS_FRAME_FREE          ((trit5_t)0)   /**< All -1: free */
#define MOS_FRAME_RESERVED      ((trit5_t)121) /**< All 0: reserved */
#define MOS_FRAME_ALLOCATED     ((trit5_t)242) /**< All +1: allocated */
#define MOS_FRAME_INVALID       MOS_SPARE_INVALID /**< Spare 243: invalid */
#define MOS_FRAME_ERROR         MOS_SPARE_ERROR   /**< Spare 244: error */
#define MOS_FRAME_PENDING       MOS_SPARE_PENDING /**< Spare 245: I/O pending */
#define MOS_FRAME_LOCKED        MOS_SPARE_LOCKED  /**< Spare 246: hard-locked */
/** @} */

/** @name Buddy Configuration — Power-of-Two Allocation
 *  @brief Order range for contiguous page allocation.
 *  @{ */
#define MOS_BUDDY_MAX_ORDER     10    /**< 2^10 = 1024 pages = 4MB max block */
#define MOS_BUDDY_MIN_ORDER     0     /**< 2^0 = 1 page = 4KB min block */
/** @} */

/** @name Slab Configuration — Object Cache Sizes
 *  @brief Cache count and size limits for kernel heap.
 *  @{ */
#define MOS_SLAB_CACHE_COUNT    9     /**< Caches: 8, 16, 32, 64, 128, 256, 512, 1024, 2048 */
#define MOS_SLAB_MIN_SIZE       8     /**< 8B: minimum object alignment */
#define MOS_SLAB_MAX_SIZE       2048  /**< 2KB: larger uses buddy directly */
/** @} */

/** @name Page Table Entry Flags — x86-64 PTE Bits
 *  @brief Hardware-defined flags for virtual memory mapping.
 *  @{ */
#define MOS_PTE_PRESENT         (1ULL << 0)  /**< Bit 0: page is present */
#define MOS_PTE_WRITABLE        (1ULL << 1)  /**< Bit 1: page is writable */
#define MOS_PTE_USER            (1ULL << 2)  /**< Bit 2: user-mode accessible */
#define MOS_PTE_WRITETHROUGH    (1ULL << 3)  /**< Bit 3: write-through caching */
#define MOS_PTE_NOCACHE         (1ULL << 4)  /**< Bit 4: disable caching */
#define MOS_PTE_ACCESSED        (1ULL << 5)  /**< Bit 5: page accessed (CPU) */
#define MOS_PTE_DIRTY           (1ULL << 6)  /**< Bit 6: page dirty (CPU) */
#define MOS_PTE_NX              (1ULL << 63) /**< Bit 63: no-execute */
#define MOS_PTE_ADDR_MASK       0x000FFFFFFFFFF000ULL /**< Bits 12-51: phys addr */
/** @} */

/** @} */ /* end S.2a BOUNDS */

/** S.2b MACROS — [Reserved: operations in kernel.h] @{ @} */

/** S.2c VERIFY — [In CLOSING X.1 VALIDATION] @{ @} */

/** S.2d LOOKUP — [Reserved: no runtime tables in header] @{ @} */

/** S.2e TYPES — [Reserved: main types in BODY B.1] @{ @} */

/** @} */ /* end S.2 DEFINES */

/**
 * =============================================================================
 * S.3 TYPES — Type Declarations [TYPES]
 * =============================================================================
 *
 * @defgroup memory_types S.3 TYPES — Type Declarations
 * @brief    Data structures and type definitions.
 *
 * CONTAINS:
 *   - S.3a ENUMS   — Enumeration types (MosPressureLevel)
 *   - S.3b STRUCTS — Structure definitions (all memory structs)
 *   - S.3c OPAQUE  — [Reserved: no opaque types]
 *   - S.3d ERRORS  — [Reserved: errors in kernel.h]
 *
 * =============================================================================
 * @{ */

/**
 * ---------------------------------------------------------------------------
 * S.3a ENUMS — Enumeration Types [ENUMS]
 * ---------------------------------------------------------------------------
 *
 * @defgroup memory_enums S.3a ENUMS — Enumeration Types
 * @ingroup memory_types
 * @brief    Enumeration definitions for memory subsystem.
 * @{ */

/**
 * @brief Memory pressure levels.
 */
typedef enum MosPressureLevel {
    MOS_PRESSURE_NONE     = 0,  /**< No pressure, plenty of memory */
    MOS_PRESSURE_LOW      = 1,  /**< Low pressure, start reclaiming */
    MOS_PRESSURE_MEDIUM   = 2,  /**< Medium pressure, aggressive reclaim */
    MOS_PRESSURE_CRITICAL = 3   /**< Critical, may fail allocations */
} MosPressureLevel;

/** @} */ /* end S.3a ENUMS */

/**
 * ---------------------------------------------------------------------------
 * S.3b STRUCTS — Structure Definitions [STRUCTS]
 * ---------------------------------------------------------------------------
 *
 * @defgroup memory_structs S.3b STRUCTS — Structure Definitions
 * @ingroup memory_types
 * @brief    Structure definitions for memory subsystem.
 * @{ */

/**
 * @brief Physical page frame descriptor.
 *
 * Tracks state and metadata for each physical page frame.
 * Size optimized to 24 bytes for cache efficiency.
 */
typedef struct MosPageFrame {
    mos_paddr_t     address;        /**< Physical address of this frame */
    trit5_t         state;          /**< State (0-242) or spare (243-255) */
    mos_pid_t       owner;          /**< Owning process ID (0 = kernel) */
    uint16_t        ref_count;      /**< Reference count for sharing */
} MosPageFrame;

/**
 * @brief Memory region descriptor.
 *
 * Describes a contiguous region of physical memory from the boot map.
 */
typedef struct MosMemRegion {
    mos_paddr_t     base;           /**< Region base address */
    mos_size_t      size;           /**< Region size in bytes */
    trit_t          type;           /**< -1=unusable, 0=reserved, +1=usable */
    trit_t          zone;           /**< Memory zone classification */
} MosMemRegion;

/**
 * @brief Buddy allocator free list entry.
 *
 * Each entry represents a free block of 2^order pages.
 */
typedef struct MosBuddyBlock {
    struct MosBuddyBlock* next;     /**< Next block in free list */
    uint8_t         order;          /**< Block order (0-10) */
    uint8_t         reserved[7];    /**< Alignment padding */
} MosBuddyBlock;

/**
 * @brief Buddy allocator state.
 *
 * Manages power-of-two page allocation for contiguous memory.
 */
typedef struct MosBuddyAllocator {
    MosBuddyBlock*  free_list[MOS_BUDDY_MAX_ORDER + 1];  /**< Free lists by order */
    mos_size_t      free_count[MOS_BUDDY_MAX_ORDER + 1]; /**< Free blocks per order */
    mos_paddr_t     base;           /**< Managed region base */
    mos_size_t      size;           /**< Managed region size */
    mos_health_t    health;         /**< Allocator health */
} MosBuddyAllocator;

/**
 * @brief Slab object header (embedded in free objects).
 */
typedef struct MosSlabObject {
    struct MosSlabObject* next;     /**< Next free object */
} MosSlabObject;

/**
 * @brief Slab descriptor (one per page of objects).
 */
typedef struct MosSlab {
    struct MosSlab* next;           /**< Next slab in cache */
    MosSlabObject*  free_list;      /**< Free object list */
    uint16_t        in_use;         /**< Objects currently allocated */
    uint16_t        total;          /**< Total objects in slab */
    mos_paddr_t     page;           /**< Physical page address */
} MosSlab;

/**
 * @brief Slab cache for a specific object size.
 */
typedef struct MosSlabCache {
    MosSlab*        slabs_partial;  /**< Slabs with free objects */
    MosSlab*        slabs_full;     /**< Completely full slabs */
    MosSlab*        slabs_empty;    /**< Empty slabs (for reclaim) */
    mos_size_t      object_size;    /**< Size of objects in this cache */
    mos_size_t      objects_per_slab; /**< Objects per slab */
    mos_size_t      alloc_count;    /**< Total allocations */
    mos_size_t      free_count;     /**< Total frees */
} MosSlabCache;

/**
 * @brief Memory pressure callback function.
 *
 * Called when memory is low to request reclamation.
 *
 * @param[in] pressure  Pressure level (0=low, 1=medium, 2=critical).
 * @return              Number of pages reclaimed.
 */
typedef mos_size_t (*MosPressureCallback)(int pressure);

/**
 * @brief Memory pressure handler registration.
 */
typedef struct MosPressureHandler {
    MosPressureCallback callback;   /**< Callback function */
    const char*         name;       /**< Handler name for debugging */
    int                 priority;   /**< Priority (higher = called first) */
} MosPressureHandler;

/**
 * @brief Memory manager global state.
 *
 * Central structure tracking all memory subsystem state.
 */
typedef struct MosMemoryManager {
    /* Frame tracking */
    MosPageFrame*       frames;         /**< Frame descriptor array */
    mos_size_t          frame_count;    /**< Total frames tracked */
    mos_size_t          free_count;     /**< Free frames available */
    mos_size_t          used_count;     /**< Frames in use */

    /* Memory totals */
    mos_size_t          total_memory;   /**< Total physical memory */
    mos_size_t          usable_memory;  /**< Usable (non-reserved) memory */
    mos_size_t          kernel_memory;  /**< Kernel reserved memory */

    /* Allocators */
    MosBuddyAllocator   buddy;          /**< Buddy allocator state */
    MosSlabCache        slab_caches[MOS_SLAB_CACHE_COUNT]; /**< Slab caches */

    /* Pressure handling */
    MosPressureHandler  pressure_handlers[16]; /**< Registered handlers */
    int                 pressure_count; /**< Number of handlers */

    /* Health tracking */
    mos_health_t        health;         /**< Subsystem health (-243 to +243) */
    MosHebrewState      state;          /**< Cognitive state */

    /* Statistics */
    trit27_t            alloc_count;    /**< Total frame allocations */
    trit27_t            free_ops;       /**< Total frame frees */
    trit27_t            page_faults;    /**< Page fault count */
    trit27_t            slab_allocs;    /**< Total slab allocations */
    trit27_t            buddy_allocs;   /**< Total buddy allocations */
} MosMemoryManager;

/** @} */ /* end S.3b STRUCTS */

/** S.3c OPAQUE — [Reserved: no opaque types] @{ @} */

/** S.3d ERRORS — [Reserved: errors in kernel.h] @{ @} */

/** @} */ /* end S.3 TYPES */

/**
 * =============================================================================
 * S.4 PROTOTYPES — Function Declarations [PROTOTYPES]
 * =============================================================================
 *
 * @defgroup memory_prototypes S.4 PROTOTYPES — Function Declarations
 * @brief    Function signatures for memory subsystem.
 *
 * CONTAINS:
 *   - S.4a LIFECYCLE  — Init/destroy functions
 *   - S.4b ACCESSORS  — Get/query functions
 *   - S.4c OPERATIONS — Alloc/free/map operations
 *
 * =============================================================================
 * @{ */

/**
 * ---------------------------------------------------------------------------
 * S.4a LIFECYCLE — Initialization Functions [LIFECYCLE]
 * ---------------------------------------------------------------------------
 *
 * @defgroup memory_lifecycle S.4a LIFECYCLE — Initialization Functions
 * @ingroup memory_prototypes
 * @brief    Memory subsystem initialization.
 * @{ */

/**
 * @brief Initialize memory manager from boot info.
 *
 * Called during kernel boot Stage 3 (Firmament). Initializes:
 *   - Frame tracking from boot memory map
 *   - Early bitmap allocator
 *   - Memory health baseline
 *
 * @param[in] boot  Valid boot info with memory map.
 *
 * @return  MOS_OK on success.
 * @retval  MOS_ERR_INVALID if boot is NULL or invalid.
 * @retval  MOS_ERR_NOMEM if no usable memory found.
 *
 * @pre  boot != NULL && mos_boot_valid(boot)
 * @post Memory manager ready for frame allocations.
 */
mos_result_t mos_memory_init(const MosBootInfo* boot);

/**
 * @brief Late memory initialization.
 *
 * Called after basic allocator ready. Upgrades:
 *   - Early allocator → buddy allocator
 *   - Initializes slab caches
 *   - Registers default pressure handlers
 *
 * @return  MOS_OK on success.
 *
 * @pre  mos_memory_init() completed successfully.
 * @post Full memory allocation subsystem ready.
 */
mos_result_t mos_memory_late_init(void);

/**
 * @brief Initialize buddy allocator.
 *
 * Sets up power-of-two block allocation for contiguous memory.
 *
 * @param[in] base  Physical base address of managed region.
 * @param[in] size  Size of managed region in bytes.
 *
 * @return  MOS_OK on success.
 */
mos_result_t mos_buddy_init(mos_paddr_t base, mos_size_t size);

/**
 * @brief Initialize slab allocator.
 *
 * Sets up slab caches for common allocation sizes.
 *
 * @return  MOS_OK on success.
 */
mos_result_t mos_slab_init(void);

/** @} */ /* end S.4a LIFECYCLE */

/**
 * ---------------------------------------------------------------------------
 * S.4b ACCESSORS — Query Functions [ACCESSORS]
 * ---------------------------------------------------------------------------
 *
 * @defgroup memory_accessors S.4b ACCESSORS — Query Functions
 * @ingroup memory_prototypes
 * @brief    Memory state queries and information.
 * @{ */

/**
 * @brief Get memory manager state.
 *
 * @return  Pointer to memory manager (read-only).
 */
const MosMemoryManager* mos_memory_info(void);

/**
 * @brief Get memory subsystem health.
 *
 * @return  Health value (-243 to +243).
 *
 * Health interpretation:
 *   - +100 to +243: Excellent (plenty of memory)
 *   -    0 to +99:  Good (normal operation)
 *   - -100 to   -1: Warning (getting low)
 *   - -243 to -101: Critical (may fail allocations)
 */
mos_health_t mos_memory_health(void);

/**
 * @brief Get free memory in bytes.
 *
 * @return  Free memory available for allocation.
 */
mos_size_t mos_memory_free(void);

/**
 * @brief Get used memory in bytes.
 *
 * @return  Currently allocated memory.
 */
mos_size_t mos_memory_used(void);

/**
 * @brief Get total physical memory in bytes.
 *
 * @return  Total physical RAM.
 */
mos_size_t mos_memory_total(void);

/**
 * @brief Get current memory pressure level.
 *
 * @return  Current pressure level.
 */
MosPressureLevel mos_pressure_level(void);

/**
 * @brief Get page table entry for virtual address.
 *
 * @param[in] virt   Virtual address.
 *
 * @return  Page table entry value, or 0 if not mapped.
 */
uint64_t mos_get_pte(mos_vaddr_t virt);

/**
 * @brief Translate virtual address to physical.
 *
 * Walks page tables to find physical address.
 *
 * @param[in] virt   Virtual address.
 *
 * @return  Physical address.
 * @retval  MOS_ADDR_NULL if not mapped.
 */
mos_paddr_t mos_vtophys(mos_vaddr_t virt);

/**
 * @brief Get buddy allocator statistics.
 *
 * @param[out] free_pages  Total free pages across all orders.
 * @param[out] largest     Largest contiguous block available (in pages).
 */
void mos_buddy_stats(mos_size_t* free_pages, mos_size_t* largest);

/** @} */ /* end S.4b ACCESSORS */

/**
 * ---------------------------------------------------------------------------
 * S.4c OPERATIONS — Allocation and Mapping [OPERATIONS]
 * ---------------------------------------------------------------------------
 *
 * @defgroup memory_operations S.4c OPERATIONS — Allocation and Mapping
 * @ingroup memory_prototypes
 * @brief    Memory allocation, deallocation, and mapping operations.
 * @{ */

/* --- Frame Allocation --- */

/**
 * @brief Allocate a single physical frame.
 *
 * Returns a 4KB aligned physical address from the free pool.
 * Uses early bitmap allocator before buddy init.
 *
 * @return  Physical address of allocated frame.
 * @retval  MOS_ADDR_NULL if allocation fails.
 *
 * @post  Frame marked as allocated in tracking.
 */
mos_paddr_t mos_frame_alloc(void);

/**
 * @brief Allocate contiguous physical frames.
 *
 * @param[in] count  Number of frames to allocate (1-512).
 *
 * @return  Base physical address of allocated region.
 * @retval  MOS_ADDR_NULL if allocation fails.
 */
mos_paddr_t mos_frames_alloc(trit9_t count);

/**
 * @brief Allocate frame from specific memory zone.
 *
 * @param[in] zone  MOS_MEMZONE_DMA, _NORMAL, or _HIGH.
 *
 * @return  Physical address from requested zone.
 * @retval  MOS_ADDR_NULL if no frames available in zone.
 */
mos_paddr_t mos_frame_alloc_zone(trit_t zone);

/**
 * @brief Free a physical frame.
 *
 * Returns frame to free pool and updates tracking.
 *
 * @param[in] addr  Physical address to free (must be page-aligned).
 *
 * @return  MOS_OK on success.
 * @retval  MOS_ERR_INVALID if address invalid or not allocated.
 */
mos_result_t mos_frame_free(mos_paddr_t addr);

/**
 * @brief Free contiguous physical frames.
 *
 * @param[in] addr   Base physical address.
 * @param[in] count  Number of frames to free.
 *
 * @return  MOS_OK on success.
 */
mos_result_t mos_frames_free(mos_paddr_t addr, trit9_t count);

/* --- Buddy Allocator --- */

/**
 * @brief Allocate pages using buddy allocator.
 *
 * Allocates 2^order contiguous pages.
 *
 * @param[in] order  Order of allocation (0-10, pages = 2^order).
 *
 * @return  Physical address of allocated block.
 * @retval  MOS_ADDR_NULL if allocation fails.
 */
mos_paddr_t mos_buddy_alloc(uint8_t order);

/**
 * @brief Allocate pages by count using buddy allocator.
 *
 * Rounds up to nearest power of two.
 *
 * @param[in] pages  Number of pages needed.
 *
 * @return  Physical address of allocated block.
 * @retval  MOS_ADDR_NULL if allocation fails.
 */
mos_paddr_t mos_buddy_alloc_pages(mos_size_t pages);

/**
 * @brief Free pages to buddy allocator.
 *
 * @param[in] addr   Physical address (must be block-aligned).
 * @param[in] order  Order of block to free.
 *
 * @return  MOS_OK on success.
 */
mos_result_t mos_buddy_free(mos_paddr_t addr, uint8_t order);

/* --- Slab Allocator --- */

/**
 * @brief Allocate object from slab cache.
 *
 * Fast allocation for common object sizes.
 * Sizes: 8, 16, 32, 64, 128, 256, 512, 1024, 2048 bytes.
 *
 * @param[in] size  Object size (rounded up to cache size).
 *
 * @return  Pointer to allocated object.
 * @retval  NULL if allocation fails.
 */
void* mos_slab_alloc(mos_size_t size);

/**
 * @brief Allocate zeroed object from slab cache.
 *
 * @param[in] size  Object size.
 *
 * @return  Pointer to zeroed object.
 * @retval  NULL if allocation fails.
 */
void* mos_slab_zalloc(mos_size_t size);

/**
 * @brief Free object to slab cache.
 *
 * @param[in] ptr   Pointer to object.
 * @param[in] size  Object size (determines cache).
 */
void mos_slab_free(void* ptr, mos_size_t size);

/**
 * @brief Reclaim empty slabs.
 *
 * Called under memory pressure to free unused pages.
 *
 * @return  Number of pages reclaimed.
 */
mos_size_t mos_slab_reclaim(void);

/* --- Virtual Memory --- */

/**
 * @brief Map virtual address to physical address.
 *
 * Creates or updates page table entry to map virt → phys.
 * Allocates intermediate page tables as needed.
 *
 * @param[in] virt   Virtual address (must be page-aligned).
 * @param[in] phys   Physical address (must be page-aligned).
 * @param[in] flags  Mapping flags (MOS_PTE_* combinations).
 *
 * @return  MOS_OK on success.
 * @retval  MOS_ERR_NOMEM if page table allocation fails.
 */
mos_result_t mos_vmap(mos_vaddr_t virt, mos_paddr_t phys, mos_flags_t flags);

/**
 * @brief Map range of virtual addresses.
 *
 * Maps size bytes from phys to virt.
 *
 * @param[in] virt   Virtual base address.
 * @param[in] phys   Physical base address.
 * @param[in] size   Size in bytes (rounded up to pages).
 * @param[in] flags  Mapping flags.
 *
 * @return  MOS_OK on success.
 */
mos_result_t mos_vmap_range(mos_vaddr_t virt, mos_paddr_t phys,
                            mos_size_t size, mos_flags_t flags);

/**
 * @brief Unmap virtual address.
 *
 * Clears page table entry and invalidates TLB.
 *
 * @param[in] virt   Virtual address to unmap.
 *
 * @return  MOS_OK on success.
 * @retval  MOS_ERR_INVALID if address not mapped.
 */
mos_result_t mos_vunmap(mos_vaddr_t virt);

/**
 * @brief Unmap range of virtual addresses.
 *
 * @param[in] virt   Virtual base address.
 * @param[in] size   Size in bytes.
 *
 * @return  MOS_OK on success.
 */
mos_result_t mos_vunmap_range(mos_vaddr_t virt, mos_size_t size);

/**
 * @brief Map kernel memory region (identity or higher-half).
 *
 * @param[in] phys   Physical base address.
 * @param[in] size   Size in bytes.
 *
 * @return  MOS_OK on success.
 */
mos_result_t mos_kmap(mos_paddr_t phys, mos_size_t size);

/**
 * @brief Invalidate TLB entry for address.
 *
 * @param[in] virt   Virtual address to invalidate.
 */
void mos_tlb_invalidate(mos_vaddr_t virt);

/**
 * @brief Flush entire TLB.
 *
 * Used after bulk page table changes.
 */
void mos_tlb_flush_all(void);

/* --- Kernel Heap --- */

/**
 * @brief Allocate kernel memory.
 *
 * Uses slab allocator for small sizes, buddy for larger.
 *
 * @param[in] size   Size in bytes.
 *
 * @return  Virtual address of allocated memory.
 * @retval  MOS_ADDR_NULL on failure.
 */
mos_vaddr_t mos_kmalloc(mos_size_t size);

/**
 * @brief Allocate zeroed kernel memory.
 *
 * @param[in] size   Size in bytes.
 *
 * @return  Virtual address of zeroed memory.
 * @retval  MOS_ADDR_NULL on failure.
 */
mos_vaddr_t mos_kzalloc(mos_size_t size);

/**
 * @brief Free kernel memory.
 *
 * @param[in] addr   Virtual address to free.
 */
void mos_kfree(mos_vaddr_t addr);

/**
 * @brief Reallocate kernel memory.
 *
 * @param[in] addr       Existing allocation.
 * @param[in] new_size   New size in bytes.
 *
 * @return  New virtual address (may differ from addr).
 * @retval  MOS_ADDR_NULL on failure.
 */
mos_vaddr_t mos_krealloc(mos_vaddr_t addr, mos_size_t new_size);

/**
 * @brief Allocate aligned kernel memory.
 *
 * @param[in] size       Size in bytes.
 * @param[in] alignment  Required alignment (must be power of 2).
 *
 * @return  Aligned virtual address.
 * @retval  MOS_ADDR_NULL on failure.
 */
mos_vaddr_t mos_kmalloc_aligned(mos_size_t size, mos_size_t alignment);

/* --- Pressure Handling --- */

/**
 * @brief Register memory pressure handler.
 *
 * @param[in] callback  Function to call under pressure.
 * @param[in] name      Handler name for debugging.
 * @param[in] priority  Higher priority = called first.
 *
 * @return  MOS_OK on success.
 */
mos_result_t mos_pressure_register(MosPressureCallback callback,
                                   const char* name, int priority);

/**
 * @brief Unregister memory pressure handler.
 *
 * @param[in] callback  Callback to unregister.
 *
 * @return  MOS_OK on success.
 */
mos_result_t mos_pressure_unregister(MosPressureCallback callback);

/**
 * @brief Invoke pressure handlers to reclaim memory.
 *
 * @param[in] level  Pressure level to signal.
 *
 * @return  Total pages reclaimed.
 */
mos_size_t mos_pressure_reclaim(MosPressureLevel level);

/* --- Debug --- */

/**
 * @brief Print memory statistics to serial.
 *
 * Useful for debugging memory state.
 */
void mos_memory_dump_stats(void);

/** @} */ /* end S.4c OPERATIONS */

/** @} */ /* end S.4 PROTOTYPES */

/** S.5 FILE-LEVEL — [Reserved: no mutable file-level state] @{ @} */

/**
 * =============================================================================
 * S.6 OMISSION — Reserved Section Guide [OMISSION]
 * =============================================================================
 *
 * @defgroup memory_omission S.6 OMISSION — Reserved Sections
 * @brief    Intentional omissions documented.
 *
 * SETUP RESERVED:
 *   - S.1a STANDARD: Freestanding kernel, no libc
 *   - S.1c EXTERNAL: No external dependencies
 *   - S.1d CPP: Kernel C code only
 *   - S.2.0 PRELUDE: Uses kernel.h primitives
 *   - S.2b MACROS: Operations in kernel.h
 *   - S.2c VERIFY: In CLOSING X.1
 *   - S.2d LOOKUP: No runtime tables
 *   - S.2e TYPES: In S.3
 *   - S.3c OPAQUE: No opaque types
 *   - S.3d ERRORS: In kernel.h
 *   - S.5 FILE-LEVEL: No mutable state
 *
 * BODY RESERVED:
 *   - All types in S.3, all prototypes in S.4
 *
 * =============================================================================
 * @{ @} */

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY BLOCK [BODY]
// =============================================================================
//
// @brief [Reserved: all types and prototypes in SETUP S.3-S.4]
//
// =============================================================================

/** B.1 ORG CHART — [Reserved: types in S.3] @{ @} */
/** B.2 HELPERS — [Reserved: prototypes in S.4] @{ @} */
/** B.3 RESERVED — [Reserved] @{ @} */
/** B.4 RESERVED — [Reserved] @{ @} */
/** B.5 RESERVED — [Reserved] @{ @} */
/** B.6 RESERVED — [Reserved] @{ @} */

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING BLOCK [CLOSING]
// =============================================================================
//
// @brief Final assembly — X.1-X.9 sections (template-aligned).
//
// CONTAINS:
//   - X.1 VALIDATION    — Build verification
//   - X.2 EXECUTION     — Usage patterns
//   - X.3 CLEANUP       — Ownership notes
//   - X.4 POLICY        — Modification guidance
//   - X.5 EXTENSION     — How to extend
//   - X.6 TROUBLESHOOT  — Common problems
//   - X.7 REFERENCE     — Quick lookup
//   - X.8 NOTE          — Closing guidance
//   - X.9 OMISSION      — Reserved sections
//
// =============================================================================

/**
 * =============================================================================
 * X.1 VALIDATION — Build Verification [VALIDATION]
 * =============================================================================
 *
 * @defgroup memory_validation X.1 VALIDATION — Build Verification
 * @brief    Compile-time assertions and build verification.
 * @{
 *
 * @par X.1a BUILD — Compilation Commands
 * @code{.sh}
 * # Syntax check (from millenniumos root):
 * gcc -fsyntax-only -Wall -Wextra -I kernel/include \
 *     kernel/include/core/memory/memory.h
 * @endcode
 *
 * @par X.1b CHECKS — Verification Checklist
 *   - ✓ Compiles without warnings (-Wall -Wextra)
 *   - ✓ All structures properly aligned
 *   - ✓ No implicit dependencies
 */

_Static_assert(sizeof(MosPageFrame) <= 24,
               "MosPageFrame exceeds expected 24-byte size");

_Static_assert(sizeof(MosMemRegion) <= 32,
               "MosMemRegion exceeds expected 32-byte size");

_Static_assert(sizeof(MosBuddyBlock) == 16,
               "MosBuddyBlock must be 16 bytes for alignment");

_Static_assert(MOS_BUDDY_MAX_ORDER <= 12,
               "Buddy order too large (max 4096 pages = 16MB)");

_Static_assert(MOS_SLAB_CACHE_COUNT == 9,
               "Slab cache count must match size array");

/** @} */ /* end memory_validation */

/**
 * =============================================================================
 * X.2 EXECUTION — Usage Patterns [EXECUTION]
 * =============================================================================
 *
 * @defgroup memory_execution X.2 EXECUTION — Usage Patterns
 * @brief    Common usage patterns for memory API.
 * @{
 *
 * @par X.2a INCLUDE — Include Directive
 * @code{.c}
 * #include "core/memory/memory.h"
 * @endcode
 *
 * @par X.2b FRAME — Frame Allocation Pattern
 * @code{.c}
 * mos_paddr_t frame = mos_frame_alloc();
 * if (frame == MOS_ADDR_NULL) {
 *     return MOS_ERR_NOMEM;
 * }
 * // Use frame for page table, DMA buffer, etc.
 * mos_frame_free(frame);
 * @endcode
 *
 * @par X.2c HEAP — Kernel Heap Pattern
 * @code{.c}
 * MyStruct* obj = (MyStruct*)mos_kzalloc(sizeof(MyStruct));
 * if (!obj) { return MOS_ERR_NOMEM; }
 * // Use object...
 * mos_kfree((mos_vaddr_t)obj);
 * @endcode
 *
 * @par X.2d MAPPING — Virtual Memory Pattern
 * @code{.c}
 * mos_paddr_t phys = mos_frame_alloc();
 * mos_vaddr_t virt = 0xFFFF800000100000;
 * mos_vmap(virt, phys, MOS_PTE_PRESENT | MOS_PTE_WRITABLE);
 * // Access via virt...
 * mos_vunmap(virt);
 * mos_frame_free(phys);
 * @endcode
 *
 * @} */

/**
 * =============================================================================
 * X.3 CLEANUP — Ownership Patterns [CLEANUP]
 * =============================================================================
 *
 * @defgroup memory_cleanup X.3 CLEANUP — Ownership Patterns
 * @brief    Memory ownership and cleanup responsibilities.
 * @{
 *
 * @par X.3a OWNERSHIP — Caller Responsibility
 *   - Caller owns memory returned by mos_kmalloc/mos_frame_alloc
 *   - Caller must free before losing reference
 *   - Reference counting via MosPageFrame.ref_count for sharing
 *
 * @par X.3b MAPPINGS — Virtual Memory Cleanup
 *   - Always unmap before freeing physical frame
 *   - TLB automatically flushed on unmap
 *
 * @} */

/**
 * =============================================================================
 * X.4 POLICY — Modification Guidance [POLICY]
 * =============================================================================
 *
 * @defgroup memory_policy X.4 POLICY — Modification Guidance
 * @brief    Rules for modifying this file.
 * @{
 *
 * @par DO NOT CHANGE
 *   - Page size constants (4KB is x86-64 standard)
 *   - Buddy max order (larger wastes memory)
 *   - PTE flag values (hardware-defined)
 *
 * @par MAY ADD
 *   - New allocator types (e.g., per-CPU caches)
 *   - NUMA zone support
 *   - Huge page allocation functions
 *
 * @} */

/**
 * =============================================================================
 * X.5 EXTENSION — How to Extend [EXTENSION]
 * =============================================================================
 *
 * @defgroup memory_extension X.5 EXTENSION — How to Extend
 * @brief    How to add new memory features.
 * @{
 *
 * @par Adding New Allocator
 *   1. Add configuration in S.2 DEFINES
 *   2. Add state to MosMemoryManager
 *   3. Add init function in B.2 INIT
 *   4. Add alloc/free functions in appropriate B.x section
 *   5. Update mos_memory_dump_stats()
 *
 * @par Adding Pressure Handler
 *   1. Create callback: mos_size_t my_reclaim(int pressure)
 *   2. Register: mos_pressure_register(my_reclaim, "my_cache", 50)
 *   3. Implement reclamation logic in callback
 *
 * @} */

/**
 * =============================================================================
 * X.6 TROUBLESHOOT — Common Problems [TROUBLESHOOT]
 * =============================================================================
 *
 * @defgroup memory_troubleshoot X.6 TROUBLESHOOT — Common Problems
 * @brief    Common problems and solutions.
 * @{
 *
 * @par "mos_frame_alloc returns MOS_ADDR_NULL"
 *   Check: mos_memory_health() — if negative, memory exhausted
 *   Fix: Free unused allocations, register pressure handlers
 *
 * @par "Page fault after mos_vmap"
 *   Check: Verify MOS_PTE_PRESENT flag in mapping
 *   Check: Physical address valid and not in reserved range
 *
 * @par "Slab allocation slow"
 *   Cause: Too many slabs full, constant slab creation
 *   Fix: Call mos_slab_reclaim() periodically
 *
 * @} */

/**
 * =============================================================================
 * X.7 REFERENCE — Quick Lookup [REFERENCE]
 * =============================================================================
 *
 * @defgroup memory_reference X.7 REFERENCE — Quick Lookup
 * @brief    Quick reference for memory constants and functions.
 * @{
 *
 * @par Page Constants
 *   - MOS_PAGE_SIZE = 4096 (4KB)
 *   - MOS_PAGE_SHIFT = 12
 *   - MOS_PAGE_MASK = 0xFFF
 *
 * @par Memory Zones
 *   - MOS_MEMZONE_DMA (-1): < 16MB, ISA DMA
 *   - MOS_MEMZONE_NORMAL (0): 16MB - 4GB
 *   - MOS_MEMZONE_HIGH (+1): > 4GB
 *
 * @par Allocation Functions
 *   - mos_frame_alloc() → single 4KB frame
 *   - mos_buddy_alloc(order) → 2^order contiguous frames
 *   - mos_kmalloc(size) → kernel heap
 *   - mos_slab_alloc(size) → object cache
 *
 * @par Mapping Functions
 *   - mos_vmap(virt, phys, flags) → create mapping
 *   - mos_vunmap(virt) → remove mapping
 *   - mos_vtophys(virt) → translate address
 *
 * @} */

/**
 * =============================================================================
 * X.8 NOTE — Closing Guidance [NOTE]
 * =============================================================================
 *
 * @defgroup memory_note X.8 NOTE — Closing Guidance
 * @brief    Final guidance and biblical grounding.
 *
 * Scripture: Genesis 1:9
 * @{
 *
 * Memory management uses ternary concepts throughout:
 *   - Addresses as trit27 (7.6 trillion addressable bytes)
 *   - Frame states: free(-1), reserved(0), allocated(+1)
 *   - Zones: DMA(-1), normal(0), high(+1)
 *   - Health tracking with ternary thresholds
 *
 * The memory subsystem provides the "dry land" upon which all
 * processes execute. Like Genesis 1:9, it gathers and organizes
 * the formless physical resources into usable structure.
 *
 * "Let the waters under the heaven be gathered together unto one place,
 *  and let the dry land appear: and it was so."
 *   — Genesis 1:9
 *
 * @} */

/**
 * =============================================================================
 * X.9 OMISSION — Reserved Sections [OMISSION]
 * =============================================================================
 *
 * @defgroup memory_omission X.9 OMISSION — Reserved Sections
 * @brief    Reserved section documentation.
 * @{
 *
 * @par Reserved for Future
 *   - NUMA domain support
 *   - Huge page (2MB/1GB) allocation
 *   - Memory hotplug/hotremove
 *   - Persistent memory (PMEM) support
 *
 * @} */

// =============================================================================
// END CLOSING
// =============================================================================

#endif /* MOS_KERNEL_CORE_MEMORY_H */
