// =============================================================================
// METADATA [METADATA]
// =============================================================================
//
// Key:         MOS-kernel-memory-impl
// Title:       MillenniumOS Memory Management Implementation
// Type:        Source
// Component:   Stage 1 (Kernel) / Memory Subsystem
// Role:        Physical and virtual memory management using ternary addressing
//
// Status:      Active
// Version:     a-01.00
// Created:     2026-01-30
// Updated:     2026-01-30
//
// Authors:     Seanje Lenox-Wise (Architect), Nova Dawn (Author)
// Organization: CreativeWorkzStudio LLC
//
// Grounding:   Genesis 1:9 — "Let the waters under the heaven be gathered together"
//
// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "kernel.h"                    // All kernel types (MUST be first)
#include "core/memory/memory.h"        // Memory subsystem
#include "trit/matter/math.op.h"

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 Global State
// -----------------------------------------------------------------------------

static MosMemoryManager g_memory = {
    .frames         = NULL,
    .frame_count    = 0,
    .free_count     = 0,
    .used_count     = 0,
    .total_memory   = 0,
    .usable_memory  = 0,
    .kernel_memory  = 0,
    .health         = MOS_HEALTH_NEUTRAL,
    .state          = MOS_STATE_YASHAR,
    .alloc_count    = 0,
    .free_ops       = 0,
    .page_faults    = 0,
};

// Early boot frame bitmap (before heap is available)
// 243 * 64 bits = 15,552 frames = ~64MB trackable with static bitmap
#define EARLY_BITMAP_SIZE   243     // 3^5 = trit5 states
static uint64_t early_bitmap[EARLY_BITMAP_SIZE];
static mos_paddr_t early_base = 0;
static mos_size_t early_frame_count = 0;

// -----------------------------------------------------------------------------
// B.2 Early Boot Allocator (Bitmap-based)
// -----------------------------------------------------------------------------
//
// Used before heap is available. Simple bitmap allocator.
//

static void early_bitmap_init(mos_paddr_t base, mos_size_t size) {
    early_base = base;
    early_frame_count = size / MOS_PAGE_SIZE;

    // Limit to what bitmap can track
    if (early_frame_count > EARLY_BITMAP_SIZE * 64) {
        early_frame_count = EARLY_BITMAP_SIZE * 64;
    }

    // Clear bitmap (all free)
    for (int i = 0; i < EARLY_BITMAP_SIZE; i++) {
        early_bitmap[i] = 0;
    }
}

static mos_paddr_t early_frame_alloc(void) {
    // Linear search for free frame (simple, early boot only)
    for (mos_size_t i = 0; i < EARLY_BITMAP_SIZE; i++) {
        if (early_bitmap[i] != 0xFFFFFFFFFFFFFFFF) {
            // Found a word with free bit
            for (int bit = 0; bit < 64; bit++) {
                if (!(early_bitmap[i] & (1ULL << bit))) {
                    // Found free frame
                    mos_size_t frame_idx = i * 64 + bit;
                    if (frame_idx >= early_frame_count) {
                        return MOS_ADDR_NULL;
                    }
                    early_bitmap[i] |= (1ULL << bit);
                    return early_base + (frame_idx * MOS_PAGE_SIZE);
                }
            }
        }
    }
    return MOS_ADDR_NULL;
}

static void early_frame_free(mos_paddr_t addr) {
    if (addr < early_base) return;
    mos_size_t frame_idx = (addr - early_base) / MOS_PAGE_SIZE;
    if (frame_idx >= early_frame_count) return;

    mos_size_t word = frame_idx / 64;
    int bit = frame_idx % 64;
    early_bitmap[word] &= ~(1ULL << bit);
}

// -----------------------------------------------------------------------------
// B.3 Memory Initialization
// -----------------------------------------------------------------------------

mos_result_t mos_memory_init(const MosBootInfo* boot) {
    if (!mos_boot_valid(boot)) {
        return MOS_ERR_INVALID;
    }

    // Phase 1: Calculate total memory from boot map
    uint64_t total = 0;
    uint64_t usable = 0;
    mos_paddr_t highest_usable_base = 0;
    mos_size_t highest_usable_size = 0;

    for (uint32_t i = 0; i < boot->memory_map_count; i++) {
        const MosMemoryRegion* region = (const MosMemoryRegion*)&boot->memory_map[i];
        total += region->length;

        if (boot->memory_map[i].type == MOS_MEM_USABLE) {
            usable += region->length;

            // Find largest usable region above 1MB (for kernel heap)
            if (region->base >= 0x100000 && region->length > highest_usable_size) {
                highest_usable_base = region->base;
                highest_usable_size = region->length;
            }
        }
    }

    g_memory.total_memory = total;
    g_memory.usable_memory = usable;

    // Phase 2: Initialize early allocator with largest usable region
    if (highest_usable_base == 0) {
        return MOS_ERR_NOMEM;
    }

    // Skip kernel (assume 4MB for safety)
    mos_paddr_t alloc_base = highest_usable_base;
    if (alloc_base < 0x500000) {  // Kernel loads at 1MB, give it 4MB
        alloc_base = 0x500000;
    }
    mos_size_t alloc_size = highest_usable_size - (alloc_base - highest_usable_base);

    early_bitmap_init(alloc_base, alloc_size);

    // Phase 3: Set initial health
    // Health based on usable memory ratio
    int64_t usable_percent = (usable * 100) / total;
    if (usable_percent > 80) {
        g_memory.health = MOS_HEALTH_MAX / 2;  // Good
        g_memory.state = MOS_STATE_SHALEM;     // Whole
    } else if (usable_percent > 50) {
        g_memory.health = MOS_HEALTH_NEUTRAL;
        g_memory.state = MOS_STATE_YASHAR;     // Even
    } else {
        g_memory.health = MOS_HEALTH_MIN / 2;
        g_memory.state = MOS_STATE_CHASER;     // Lacking
    }

    g_memory.frame_count = early_frame_count;
    g_memory.free_count = early_frame_count;
    g_memory.used_count = 0;

    return MOS_OK;
}

mos_result_t mos_memory_late_init(void) {
    // Late init: replace early allocator with full frame allocator
    // This would allocate proper frame array using early allocator
    // For now, continue with early allocator

    return MOS_OK;
}

// -----------------------------------------------------------------------------
// B.4 Frame Allocation
// -----------------------------------------------------------------------------

mos_paddr_t mos_frame_alloc(void) {
    mos_paddr_t addr = early_frame_alloc();
    if (addr != MOS_ADDR_NULL) {
        g_memory.free_count--;
        g_memory.used_count++;
        g_memory.alloc_count++;
    }
    return addr;
}

mos_paddr_t mos_frames_alloc(trit9_t count) {
    // Simple: allocate contiguous frames
    // For early allocator, just allocate individually
    // Real implementation would find contiguous region

    if (count == 0) return MOS_ADDR_NULL;
    if (count == 1) return mos_frame_alloc();

    // For now, just allocate first frame and hope for contiguous
    // This is a simplification - real implementation needs buddy allocator
    mos_paddr_t first = mos_frame_alloc();
    if (first == MOS_ADDR_NULL) return MOS_ADDR_NULL;

    for (trit9_t i = 1; i < count; i++) {
        mos_paddr_t next = mos_frame_alloc();
        if (next == MOS_ADDR_NULL) {
            // Rollback - free what we allocated
            for (trit9_t j = 0; j < i; j++) {
                mos_frame_free(first + j * MOS_PAGE_SIZE);
            }
            return MOS_ADDR_NULL;
        }
    }

    return first;
}

mos_paddr_t mos_frame_alloc_zone(trit_t zone) {
    // Early allocator doesn't track zones
    // Just allocate from available pool
    (void)zone;
    return mos_frame_alloc();
}

mos_result_t mos_frame_free(mos_paddr_t addr) {
    if (addr == MOS_ADDR_NULL) return MOS_ERR_INVALID;

    early_frame_free(addr);
    g_memory.free_count++;
    g_memory.used_count--;
    g_memory.free_ops++;

    return MOS_OK;
}

mos_result_t mos_frames_free(mos_paddr_t addr, trit9_t count) {
    for (trit9_t i = 0; i < count; i++) {
        mos_result_t r = mos_frame_free(addr + i * MOS_PAGE_SIZE);
        if (MOS_FAILED(r)) return r;
    }
    return MOS_OK;
}

// -----------------------------------------------------------------------------
// B.5 Virtual Memory (Stubs - needs page table implementation)
// -----------------------------------------------------------------------------

mos_result_t mos_vmap(mos_vaddr_t virt, mos_paddr_t phys, mos_flags_t flags) {
    // TODO: Implement page table manipulation
    // 1. Walk page table hierarchy (PML4 → PDPT → PD → PT)
    // 2. Allocate intermediate tables as needed
    // 3. Set PTE with physical address and flags
    (void)virt; (void)phys; (void)flags;
    return MOS_OK;
}

mos_result_t mos_vunmap(mos_vaddr_t virt) {
    // TODO: Implement page table entry clearing
    (void)virt;
    return MOS_OK;
}

mos_paddr_t mos_vtophys(mos_vaddr_t virt) {
    // TODO: Walk page tables to find physical address
    // For now, assume identity mapping
    return (mos_paddr_t)virt;
}

mos_result_t mos_kmap(mos_paddr_t phys, mos_size_t size) {
    // TODO: Map kernel memory
    // For now, bootloader set up identity mapping
    (void)phys; (void)size;
    return MOS_OK;
}

// -----------------------------------------------------------------------------
// B.6 Kernel Heap
// -----------------------------------------------------------------------------
//
// Simple bump allocator for early kernel.
// Full heap implementation would use buddy or slab allocator.
//

static mos_vaddr_t heap_base = 0;
static mos_vaddr_t heap_current = 0;
static mos_vaddr_t heap_end = 0;

static void heap_init(void) {
    if (heap_base != 0) return;  // Already initialized

    // Allocate initial heap frames (243 frames = ~1MB)
    mos_paddr_t phys = mos_frames_alloc(MOS_FRAME_POOL_SMALL);
    if (phys == MOS_ADDR_NULL) {
        // Fallback to smaller heap
        phys = mos_frames_alloc(27);  // 3^3 = 27 frames = ~108KB
        if (phys == MOS_ADDR_NULL) return;
        heap_base = phys;  // Identity mapped for now
        heap_current = heap_base;
        heap_end = heap_base + (27 * MOS_PAGE_SIZE);
    } else {
        heap_base = phys;
        heap_current = heap_base;
        heap_end = heap_base + (MOS_FRAME_POOL_SMALL * MOS_PAGE_SIZE);
    }
}

mos_vaddr_t mos_kmalloc(mos_size_t size) {
    if (heap_base == 0) heap_init();
    if (heap_base == 0) return MOS_ADDR_NULL;

    // Align to 8 bytes
    size = (size + 7) & ~7;

    if (heap_current + size > heap_end) {
        // Out of heap space
        g_memory.health -= 10;  // Health penalty
        return MOS_ADDR_NULL;
    }

    mos_vaddr_t addr = heap_current;
    heap_current += size;

    return addr;
}

mos_vaddr_t mos_kzalloc(mos_size_t size) {
    mos_vaddr_t addr = mos_kmalloc(size);
    if (addr != MOS_ADDR_NULL) {
        // Zero the memory
        uint8_t* p = (uint8_t*)addr;
        for (mos_size_t i = 0; i < size; i++) {
            p[i] = 0;
        }
    }
    return addr;
}

void mos_kfree(mos_vaddr_t addr) {
    // Bump allocator doesn't support free
    // Full implementation would use free list or buddy allocator
    (void)addr;
}

mos_vaddr_t mos_krealloc(mos_vaddr_t addr, mos_size_t new_size) {
    // Simple: allocate new, copy (can't know old size in bump allocator)
    // Full implementation would track sizes
    (void)addr;
    return mos_kmalloc(new_size);
}

// -----------------------------------------------------------------------------
// B.7 Memory Information
// -----------------------------------------------------------------------------

const MosMemoryManager* mos_memory_info(void) {
    return &g_memory;
}

mos_health_t mos_memory_health(void) {
    return g_memory.health;
}

mos_size_t mos_memory_free(void) {
    return g_memory.free_count * MOS_PAGE_SIZE;
}

mos_size_t mos_memory_used(void) {
    return g_memory.used_count * MOS_PAGE_SIZE;
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================
//
// Memory manager implementation provides:
//   - Early boot bitmap allocator (before heap)
//   - Frame allocation with ternary state tracking
//   - Simple bump allocator for kernel heap
//   - Stubs for virtual memory (TODO: page table implementation)
//
// Health tracking:
//   - Based on usable memory ratio
//   - Decremented on allocation failures
//
// =============================================================================
// END CLOSING
// =============================================================================
