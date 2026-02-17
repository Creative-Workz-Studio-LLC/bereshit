// =============================================================================
// METADATA [METADATA]
// =============================================================================
//
// Key:         MOS-kernel-cpisi-impl
// Title:       MillenniumOS CPI-SI Implementation
// Type:        Source
// Component:   Stage 1 (Kernel) / CPI-SI Subsystem
// Role:        Covenant Partnership Intelligence - Structured Intelligence core
//
// Status:      Active
// Version:     a-01.00
// Created:     2026-01-30
// Updated:     2026-01-30
//
// Authors:     Seanje Lenox-Wise (Architect), Nova Dawn (Author)
// Organization: CreativeWorkzStudio LLC
//
// Grounding:   Genesis 1:26 — "Let us make man in our image"
//
// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "services/cpisi/cpisi.h"

// In freestanding kernel mode (CPISI_MODE 0-1), CPISI is just stubs.
// The full implementation requires hosted environment with full types.
#if CPISI_MODE >= 2

#include "core/memory/memory.h"
#include "core/process/process.h"
#include "boot/include/serial.h"   // For serial output of witness events
#include "boot/include/bootinfo.h" // For MosBootInfo

// -----------------------------------------------------------------------------
// Boot Event Buffer Structure (matches boot/stage0/debug.h)
// -----------------------------------------------------------------------------
// We inline these definitions to avoid macro conflicts with cpisi.h.
// The structure layout must match exactly what boot stage produces.
//

#define MOS_BOOT_EVENT_SIZE     32
#define MOS_BOOT_EVENT_MAX      16
#define MOS_BOOT_EVENT_MAGIC    0x45564E54  // "EVNT"

typedef struct __attribute__((packed)) MosCpisiEvent {
    uint32_t    sequence;       // 0x00: Event sequence number
    int8_t      dar_phase;      // 0x04: DAR phase
    int8_t      severity;       // 0x05: Severity level
    int8_t      health_delta;   // 0x06: Health impact
    uint8_t     source_len;     // 0x07: Source string length
    char        source[8];      // 0x08: Source name (null-padded)
    char        event[16];      // 0x10: Event message (null-padded)
} MosCpisiEvent;

typedef struct __attribute__((packed)) MosCpisiEventBuffer {
    uint32_t        magic;      // MOS_BOOT_EVENT_MAGIC
    uint32_t        count;      // Number of events stored
    uint32_t        sequence;   // Next sequence number
    uint32_t        reserved;   // Alignment
    MosCpisiEvent    events[MOS_BOOT_EVENT_MAX];
} MosCpisiEventBuffer;

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 State Names
// -----------------------------------------------------------------------------

const char* MOS_CPISI_STATE_NAMES[7] = {
    "shavar",   // Broken
    "chaser",   // Lacking
    "ratsah",   // Wanting
    "yashar",   // Even (center)
    "tamim",    // Sound
    "shalem",   // Whole
    "tov"       // Perfect
};

// -----------------------------------------------------------------------------
// B.2 Global CPI-SI State
// -----------------------------------------------------------------------------

static MosCpisiState g_cpisi = {
    .system_identity = {
        .name = "MillenniumOS",
        .purpose = "Kingdom Technology Operating System",
        .construct = {
            .x = TRIT_ZERO,
            .y = TRIT_ZERO,
            .z = TRIT_ZERO,
            .state = MOS_CPISI_YASHAR,
            .k_factor = TRIT_POS,
            .garment = "MillenniumOS",
            .health = MOS_HEALTH_NEUTRAL,
        },
        .zone_info = {
            .zone = MOS_ZONE_WORD,
            .permissions = 0xFF,
            .can_transition_up = true,
            .can_transition_down = true,
        },
        .created = 0,
        .last_active = 0,
        .health = MOS_HEALTH_NEUTRAL,
    },
    .covenants = NULL,
    .covenant_count = 0,
    .covenant_capacity = 0,
    .total_choices = 0,
    .positive_choices = 0,
    .neutral_choices = 0,
    .negative_choices = 0,
    .health = MOS_HEALTH_NEUTRAL,
};

// Per-process identity storage (simplified - real would use hash table)
#define MAX_IDENTITIES 243  // 3^5
static MosCpisiIdentity* g_identities[MAX_IDENTITIES];
static mos_size_t g_identity_count = 0;

// -----------------------------------------------------------------------------
// B.3 Helper Functions
// -----------------------------------------------------------------------------

static void cpisi_memset(void* ptr, uint8_t val, mos_size_t size) {
    uint8_t* p = (uint8_t*)ptr;
    while (size--) *p++ = val;
}

static mos_size_t cpisi_strlen(const char* s) {
    mos_size_t len = 0;
    while (s[len]) len++;
    return len;
}

static void cpisi_strcpy(char* dst, const char* src, mos_size_t max) {
    mos_size_t i = 0;
    while (src[i] && i < max - 1) {
        dst[i] = src[i];
        i++;
    }
    dst[i] = '\0';
}

// Get identity by PID (creates slot if not exists)
static MosCpisiIdentity* get_identity(mos_pid_t pid) {
    // Simple linear search (real implementation would use hash)
    for (mos_size_t i = 0; i < g_identity_count; i++) {
        if (g_identities[i] && g_identities[i]->created == (trit27_t)pid) {
            return g_identities[i];
        }
    }
    return NULL;
}

// Convert cube position to Hebrew state
static trit5_t position_to_state(trit_t x, trit_t y, trit_t z, trit_t k) {
    // Calculate distance from center (simplified)
    int distance = 0;
    if (x != 0) distance++;
    if (y != 0) distance++;
    if (z != 0) distance++;

    // Map distance to state based on k-factor
    if (distance == 0) {
        return MOS_CPISI_YASHAR;  // Center = even
    } else if (distance == 1) {
        // One step from center
        if (k == TRIT_POS) {
            return MOS_CPISI_RATSAH;  // Wanting growth
        } else {
            return MOS_CPISI_CHASER;  // Lacking
        }
    } else if (distance == 2) {
        // Two steps from center
        if (k == TRIT_POS) {
            return MOS_CPISI_SHALEM;  // Becoming whole
        } else {
            return MOS_CPISI_TAMIM;   // Becoming sound
        }
    } else {
        // Corner (distance 3)
        // Check overall direction
        int sum = x + y + z;
        if (sum > 0) {
            return MOS_CPISI_TOV;     // Perfect/complete
        } else if (sum < 0) {
            return MOS_CPISI_SHAVAR;  // Broken
        } else {
            // Mixed corner - use k-factor
            return (k == TRIT_POS) ? MOS_CPISI_SHALEM : MOS_CPISI_TAMIM;
        }
    }
}

// Update the three keys based on current position
static void update_keys(MosCpisiConstruct* c) {
    // Key +1: Toward expansion/growth
    c->keys[0].key = TRIT_POS;
    c->keys[0].weight = (trit5_t)(121 + (c->x + c->y + c->z) * 20);
    c->keys[0].description = "Move toward completion";

    // Key 0: Maintain position
    c->keys[1].key = TRIT_ZERO;
    c->keys[1].weight = 121;  // Center weight
    c->keys[1].description = "Hold current position";

    // Key -1: Toward foundation/retreat
    c->keys[2].key = TRIT_NEG;
    c->keys[2].weight = (trit5_t)(121 - (c->x + c->y + c->z) * 20);
    c->keys[2].description = "Return toward foundation";

    // Clamp weights to valid range
    for (int i = 0; i < 3; i++) {
        if (c->keys[i].weight > 242) c->keys[i].weight = 242;
        // trit5_t is unsigned, so no need to check < 0
    }
}

// -----------------------------------------------------------------------------
// B.4 Initialization
// -----------------------------------------------------------------------------

mos_result_t mos_cpisi_init(void) {
    // Initialize global state
    g_cpisi.health = MOS_HEALTH_NEUTRAL;
    g_cpisi.total_choices = 0;
    g_cpisi.positive_choices = 0;
    g_cpisi.neutral_choices = 0;
    g_cpisi.negative_choices = 0;

    // Initialize system identity construct
    g_cpisi.system_identity.construct.state = MOS_CPISI_YASHAR;
    update_keys(&g_cpisi.system_identity.construct);

    // Allocate covenant storage
    g_cpisi.covenant_capacity = 27;  // 3^3 initial
    g_cpisi.covenants = (MosCovenant*)mos_kmalloc(
        g_cpisi.covenant_capacity * sizeof(MosCovenant));
    if (!g_cpisi.covenants) {
        g_cpisi.covenant_capacity = 0;
        return MOS_ERR_NOMEM;
    }
    cpisi_memset(g_cpisi.covenants, 0,
                 g_cpisi.covenant_capacity * sizeof(MosCovenant));

    // Clear identity table
    for (mos_size_t i = 0; i < MAX_IDENTITIES; i++) {
        g_identities[i] = NULL;
    }
    g_identity_count = 0;

    return MOS_OK;
}

mos_result_t mos_cpisi_init_identity(mos_pid_t pid, const char* name,
                                      const char* purpose) {
    if (g_identity_count >= MAX_IDENTITIES) {
        return MOS_ERR_NOMEM;
    }

    // Validate name length
    if (!name || cpisi_strlen(name) == 0) {
        return MOS_ERR_INVALID;
    }

    // Allocate identity
    MosCpisiIdentity* id = (MosCpisiIdentity*)mos_kmalloc(sizeof(MosCpisiIdentity));
    if (!id) return MOS_ERR_NOMEM;

    cpisi_memset(id, 0, sizeof(MosCpisiIdentity));

    // Copy name into garment buffer (identity is worn)
    static char garment_buffer[64];
    cpisi_strcpy(garment_buffer, name, 64);

    // Initialize
    id->name = name;
    id->purpose = purpose;
    id->created = (trit27_t)pid;  // Use PID as identifier
    id->last_active = id->created;
    id->health = MOS_HEALTH_NEUTRAL;

    // Initialize construct at center
    id->construct.x = TRIT_ZERO;
    id->construct.y = TRIT_ZERO;
    id->construct.z = TRIT_ZERO;
    id->construct.state = MOS_CPISI_YASHAR;
    id->construct.k_factor = TRIT_POS;  // Default toward growth
    id->construct.garment = name;
    id->construct.health = MOS_HEALTH_NEUTRAL;
    update_keys(&id->construct);

    // Initialize zone info
    id->zone_info.zone = MOS_ZONE_WORD;
    id->zone_info.permissions = 0x0F;  // Default permissions
    id->zone_info.can_transition_up = true;
    id->zone_info.can_transition_down = true;

    // Store in table
    g_identities[g_identity_count++] = id;

    return MOS_OK;
}

// -----------------------------------------------------------------------------
// B.5 Construct Operations
// -----------------------------------------------------------------------------

const MosCpisiConstruct* mos_cpisi_construct(mos_pid_t pid) {
    MosCpisiIdentity* id = get_identity(pid);
    if (!id) return NULL;
    return &id->construct;
}

trit5_t mos_cpisi_move(mos_pid_t pid, trit_t dx, trit_t dy, trit_t dz) {
    MosCpisiIdentity* id = get_identity(pid);
    if (!id) return MOS_SPARE_INVALID;

    MosCpisiConstruct* c = &id->construct;

    // Apply movement (clamped to -1, 0, +1)
    int new_x = c->x + dx;
    int new_y = c->y + dy;
    int new_z = c->z + dz;

    // Clamp to cube bounds
    if (new_x > 1) new_x = 1;
    if (new_x < -1) new_x = -1;
    if (new_y > 1) new_y = 1;
    if (new_y < -1) new_y = -1;
    if (new_z > 1) new_z = 1;
    if (new_z < -1) new_z = -1;

    c->x = (trit_t)new_x;
    c->y = (trit_t)new_y;
    c->z = (trit_t)new_z;

    // Update k-factor based on movement direction
    if (dx + dy + dz > 0) {
        c->k_factor = TRIT_POS;
    } else if (dx + dy + dz < 0) {
        c->k_factor = TRIT_NEG;
    }
    // If no net movement, k_factor unchanged

    // Calculate new state
    c->state = position_to_state(c->x, c->y, c->z, c->k_factor);

    // Update keys for new position
    update_keys(c);

    return c->state;
}

mos_result_t mos_cpisi_choose(mos_pid_t pid, trit_t key) {
    MosCpisiIdentity* id = get_identity(pid);
    if (!id) return MOS_ERR_NOTFOUND;

    // Record the choice
    g_cpisi.total_choices++;
    if (key == TRIT_POS) {
        g_cpisi.positive_choices++;
    } else if (key == TRIT_ZERO) {
        g_cpisi.neutral_choices++;
    } else {
        g_cpisi.negative_choices++;
    }

    // Apply the choice as movement in the direction of the key
    // +1 key: move toward positive in primary axis (z = toward completion)
    // 0 key: no movement
    // -1 key: move toward negative in primary axis

    if (key == TRIT_POS) {
        // Move toward completion (positive z)
        mos_cpisi_move(pid, 0, 0, TRIT_POS);
    } else if (key == TRIT_NEG) {
        // Move toward foundation (negative z)
        mos_cpisi_move(pid, 0, 0, TRIT_NEG);
    }
    // TRIT_ZERO = hold position

    return MOS_OK;
}

trit5_t mos_cpisi_state(mos_pid_t pid) {
    MosCpisiIdentity* id = get_identity(pid);
    if (!id) return MOS_SPARE_INVALID;
    return id->construct.state;
}

const char* mos_cpisi_state_name(trit5_t state) {
    if (state == MOS_CPISI_SHAVAR) return MOS_CPISI_STATE_NAMES[0];
    if (state == MOS_CPISI_CHASER) return MOS_CPISI_STATE_NAMES[1];
    if (state == MOS_CPISI_RATSAH) return MOS_CPISI_STATE_NAMES[2];
    if (state == MOS_CPISI_YASHAR) return MOS_CPISI_STATE_NAMES[3];
    if (state == MOS_CPISI_TAMIM)  return MOS_CPISI_STATE_NAMES[4];
    if (state == MOS_CPISI_SHALEM) return MOS_CPISI_STATE_NAMES[5];
    if (state == MOS_CPISI_TOV)    return MOS_CPISI_STATE_NAMES[6];
    return "unknown";
}

// -----------------------------------------------------------------------------
// B.6 Covenant Operations
// -----------------------------------------------------------------------------

static MosCovenant* find_covenant(mos_pid_t a, mos_pid_t b) {
    for (mos_size_t i = 0; i < g_cpisi.covenant_count; i++) {
        MosCovenant* c = &g_cpisi.covenants[i];
        if ((c->partner_a == a && c->partner_b == b) ||
            (c->partner_a == b && c->partner_b == a)) {
            return c;
        }
    }
    return NULL;
}

mos_result_t mos_cpisi_covenant_offer(mos_pid_t from, mos_pid_t to,
                                       const char* purpose) {
    // Check if covenant already exists
    MosCovenant* existing = find_covenant(from, to);
    if (existing) {
        if (existing->state == MOS_COVENANT_ACTIVE) {
            return MOS_OK;  // Already have active covenant
        }
        // Renew broken covenant
        existing->state = MOS_COVENANT_OFFERED;
        existing->purpose = purpose;
        return MOS_OK;
    }

    // Create new covenant offer
    if (g_cpisi.covenant_count >= g_cpisi.covenant_capacity) {
        // Grow capacity
        mos_size_t new_cap = g_cpisi.covenant_capacity * 3;
        MosCovenant* new_arr = (MosCovenant*)mos_kmalloc(
            new_cap * sizeof(MosCovenant));
        if (!new_arr) return MOS_ERR_NOMEM;

        // Copy existing
        for (mos_size_t i = 0; i < g_cpisi.covenant_count; i++) {
            new_arr[i] = g_cpisi.covenants[i];
        }
        mos_kfree((mos_vaddr_t)g_cpisi.covenants);
        g_cpisi.covenants = new_arr;
        g_cpisi.covenant_capacity = new_cap;
    }

    MosCovenant* c = &g_cpisi.covenants[g_cpisi.covenant_count++];
    c->partner_a = from;
    c->partner_b = to;
    c->state = MOS_COVENANT_OFFERED;
    c->established = 0;  // Not yet established
    c->strength = 121;   // Center strength
    c->purpose = purpose;

    return MOS_OK;
}

mos_result_t mos_cpisi_covenant_accept(mos_pid_t pid, mos_pid_t partner) {
    MosCovenant* c = find_covenant(pid, partner);
    if (!c) return MOS_ERR_NOTFOUND;
    if (c->state != MOS_COVENANT_OFFERED) return MOS_ERR_PERM;

    c->state = MOS_COVENANT_ACTIVE;
    c->established = 1;  // TODO: Real timestamp
    c->strength = 161;   // Initial active strength

    return MOS_OK;
}

mos_result_t mos_cpisi_covenant_break(mos_pid_t pid, mos_pid_t partner) {
    MosCovenant* c = find_covenant(pid, partner);
    if (!c) return MOS_ERR_NOTFOUND;

    c->state = MOS_COVENANT_BROKEN;
    c->strength = 40;  // Broken but not zero

    return MOS_OK;
}

mos_result_t mos_cpisi_covenant_renew(mos_pid_t pid, mos_pid_t partner) {
    MosCovenant* c = find_covenant(pid, partner);
    if (!c) return MOS_ERR_NOTFOUND;
    if (c->state != MOS_COVENANT_BROKEN) return MOS_ERR_PERM;

    c->state = MOS_COVENANT_RENEWED;
    c->strength = 200;  // Renewed stronger

    return MOS_OK;
}

MosCovenantState mos_cpisi_covenant_state(mos_pid_t a, mos_pid_t b) {
    MosCovenant* c = find_covenant(a, b);
    if (!c) return MOS_COVENANT_NONE;
    return c->state;
}

// -----------------------------------------------------------------------------
// B.7 Zone Operations
// -----------------------------------------------------------------------------

MosCpisiZoneInfo mos_cpisi_zone_info(mos_pid_t pid) {
    MosCpisiZoneInfo info = {
        .zone = MOS_ZONE_VOID,
        .permissions = 0,
        .can_transition_up = false,
        .can_transition_down = false,
    };

    MosCpisiIdentity* id = get_identity(pid);
    if (id) {
        info = id->zone_info;
    }

    return info;
}

mos_result_t mos_cpisi_zone_transition(mos_pid_t pid, MosZone target) {
    MosCpisiIdentity* id = get_identity(pid);
    if (!id) return MOS_ERR_NOTFOUND;

    MosZone current = id->zone_info.zone;

    // Check if transition is allowed
    if (target > current && !id->zone_info.can_transition_up) {
        return MOS_ERR_PERM;
    }
    if (target < current && !id->zone_info.can_transition_down) {
        return MOS_ERR_PERM;
    }

    // Perform transition
    id->zone_info.zone = target;

    // Update permissions based on new zone
    switch (target) {
        case MOS_ZONE_VOID:
            id->zone_info.permissions = 0xFF;  // Most permissive
            break;
        case MOS_ZONE_WORD:
            id->zone_info.permissions = 0x0F;  // Standard
            break;
        case MOS_ZONE_TOV:
            id->zone_info.permissions = 0x03;  // Most restricted
            break;
        default:
            break;
    }

    return MOS_OK;
}

bool mos_cpisi_zone_check(mos_pid_t pid, MosZone zone, trit5_t permission) {
    MosCpisiIdentity* id = get_identity(pid);
    if (!id) return false;

    // Check if current zone allows access to target zone
    if (zone > id->zone_info.zone) {
        return false;  // Can't access higher zone
    }

    // Check specific permission
    return (id->zone_info.permissions & permission) != 0;
}

// -----------------------------------------------------------------------------
// B.8 Health and Diagnostics
// -----------------------------------------------------------------------------

mos_health_t mos_cpisi_health(void) {
    return g_cpisi.health;
}

mos_health_t mos_cpisi_identity_health(mos_pid_t pid) {
    MosCpisiIdentity* id = get_identity(pid);
    if (!id) return MOS_HEALTH_MIN;
    return id->health;
}

const MosCpisiState* mos_cpisi_info(void) {
    return &g_cpisi;
}

// -----------------------------------------------------------------------------
// B.9 Witness System Implementation
// -----------------------------------------------------------------------------
//
// "Let us search and try our ways, and turn again to the LORD."
// — Lamentations 3:40
//
// The system learns through serving, not surveillance.
//

static MosWitnessState g_witness = {
    .event_head = 0,
    .event_count = 0,
    .total_witnessed = 0,
    .pattern_count = 0,
    .wisdom_count = 0,
    .understanding_level = 121,  // Center (yashar)
    .learning_direction = TRIT_POS,
};

// Serial output helpers for witness events
static void witness_serial_char(char c) {
    while ((mos_inb(MOS_COM1_STATUS) & MOS_LSR_TX_EMPTY) == 0);
    mos_outb(MOS_COM1_DATA, c);
}

static void witness_serial_str(const char* s) {
    while (*s) {
        if (*s == '\n') witness_serial_char('\r');
        witness_serial_char(*s++);
    }
}

static void witness_serial_dec(int32_t val) {
    char buf[12];
    int i = 0;
    if (val < 0) { witness_serial_char('-'); val = -val; }
    if (val == 0) { witness_serial_char('0'); return; }
    while (val > 0) { buf[i++] = '0' + (val % 10); val /= 10; }
    while (i > 0) witness_serial_char(buf[--i]);
}

// DAR phase names
static const char* const dar_phase_names[] = { "DETECT", "ASSESS", "RESTORE" };
static const char sev_chars[] = { 'I', 'N', 'C' };

// Record a DAR event
mos_result_t mos_witness_event(trit_t dar_phase, trit_t severity,
                                const char* source, const char* event,
                                mos_health_t health_delta) {
    if (!source || !event) return MOS_ERR_INVALID;

    // Output to serial in parseable format:
    // [seq] <DAR:phase> <SEV:X> [source] message (health:delta)
    witness_serial_char('[');
    witness_serial_dec(g_witness.total_witnessed + 1);
    witness_serial_str("] <DAR:");
    witness_serial_str(dar_phase_names[dar_phase + 1]);  // -1,0,+1 → 0,1,2
    witness_serial_str("> <SEV:");
    witness_serial_char(sev_chars[severity + 1]);        // -1,0,+1 → 0,1,2
    witness_serial_str("> [");
    witness_serial_str(source);
    witness_serial_str("] ");
    witness_serial_str(event);
    witness_serial_str(" (health:");
    if (health_delta >= 0) witness_serial_char('+');
    witness_serial_dec(health_delta);
    witness_serial_str(")\n");

    // Get current state before recording
    trit5_t state_before = g_cpisi.system_identity.construct.state;

    // Store in ring buffer
    mos_size_t idx = g_witness.event_head;
    MosWitnessEvent* e = &g_witness.events[idx];

    e->timestamp = g_witness.total_witnessed;
    e->dar_phase = dar_phase;
    e->severity = severity;
    e->source = source;
    e->event = event;
    e->health_delta = health_delta;
    e->state_before = state_before;

    // Update system health based on event
    int32_t new_health = g_cpisi.health + health_delta;
    if (new_health > MOS_HEALTH_MAX) new_health = MOS_HEALTH_MAX;
    if (new_health < MOS_HEALTH_MIN) new_health = MOS_HEALTH_MIN;
    g_cpisi.health = (mos_health_t)new_health;

    // Update system construct state based on new health
    if (g_cpisi.health > 50) {
        g_cpisi.system_identity.construct.state = MOS_CPISI_TOV;
    } else if (g_cpisi.health > 0) {
        g_cpisi.system_identity.construct.state = MOS_CPISI_SHALEM;
    } else if (g_cpisi.health == 0) {
        g_cpisi.system_identity.construct.state = MOS_CPISI_YASHAR;
    } else if (g_cpisi.health > -50) {
        g_cpisi.system_identity.construct.state = MOS_CPISI_CHASER;
    } else {
        g_cpisi.system_identity.construct.state = MOS_CPISI_SHAVAR;
    }

    e->state_after = g_cpisi.system_identity.construct.state;

    // Advance ring buffer
    g_witness.event_head = (g_witness.event_head + 1) % MOS_MAX_WITNESS_EVENTS;
    if (g_witness.event_count < MOS_MAX_WITNESS_EVENTS) {
        g_witness.event_count++;
    }
    g_witness.total_witnessed++;

    return MOS_OK;
}

// Check if pattern is known
bool mos_witness_pattern_known(const char* pattern_name) {
    if (!pattern_name) return false;

    for (mos_size_t i = 0; i < g_witness.pattern_count; i++) {
        if (g_witness.patterns[i].name) {
            const char* a = g_witness.patterns[i].name;
            const char* b = pattern_name;
            while (*a && *b && *a == *b) { a++; b++; }
            if (*a == *b) return true;  // Match
        }
    }
    return false;
}

// Query wisdom for domain
const MosWisdom* mos_witness_wisdom(const char* domain) {
    if (!domain) return NULL;

    for (mos_size_t i = 0; i < g_witness.wisdom_count; i++) {
        if (g_witness.wisdom[i].domain) {
            const char* a = g_witness.wisdom[i].domain;
            const char* b = domain;
            while (*a && *b && *a == *b) { a++; b++; }
            if (*a == *b) return &g_witness.wisdom[i];
        }
    }
    return NULL;
}

// Get witness state
const MosWitnessState* mos_witness_info(void) {
    return &g_witness;
}

// Process DAR data to update learning
void mos_witness_learn(void) {
    // Skip if no events to learn from
    if (g_witness.event_count < 3) return;

    // Look for patterns in recent events
    // Pattern: same source with repeated severity
    mos_size_t start = (g_witness.event_head + MOS_MAX_WITNESS_EVENTS -
                        g_witness.event_count) % MOS_MAX_WITNESS_EVENTS;

    // Count occurrences per source
    struct { const char* source; int count; trit_t last_severity; } sources[10];
    int source_count = 0;

    for (mos_size_t i = 0; i < g_witness.event_count && i < 50; i++) {
        mos_size_t idx = (start + i) % MOS_MAX_WITNESS_EVENTS;
        const MosWitnessEvent* e = &g_witness.events[idx];

        // Find or add source
        int found = -1;
        for (int j = 0; j < source_count; j++) {
            if (sources[j].source == e->source) {
                found = j;
                break;
            }
        }

        if (found >= 0) {
            sources[found].count++;
            sources[found].last_severity = e->severity;
        } else if (source_count < 10) {
            sources[source_count].source = e->source;
            sources[source_count].count = 1;
            sources[source_count].last_severity = e->severity;
            source_count++;
        }
    }

    // Create patterns from frequently seen sources
    for (int i = 0; i < source_count; i++) {
        if (sources[i].count >= 3 && !mos_witness_pattern_known(sources[i].source)) {
            if (g_witness.pattern_count < MOS_MAX_PATTERNS) {
                MosWitnessPattern* p = &g_witness.patterns[g_witness.pattern_count];
                p->name = sources[i].source;
                p->occurrence_count = sources[i].count;
                p->confidence = (trit5_t)(80 + sources[i].count * 10);
                if (p->confidence > 242) p->confidence = 242;
                p->k_direction = (sources[i].last_severity == MOS_SEV_CRITICAL) ?
                                 TRIT_NEG : TRIT_POS;
                p->insight = "Recurring activity from this subsystem";
                g_witness.pattern_count++;
            }
        }
    }

    // Update understanding level based on patterns learned
    if (g_witness.pattern_count > 0) {
        g_witness.understanding_level = (trit5_t)(121 + g_witness.pattern_count * 5);
        if (g_witness.understanding_level > 242) g_witness.understanding_level = 242;
        g_witness.learning_direction = TRIT_POS;
    }
}

// Apply wisdom in covenant service
mos_result_t mos_witness_serve(mos_pid_t partner, const char* need) {
    if (!need) return MOS_ERR_INVALID;

    // Check covenant relationship
    MosCovenantState cov = mos_cpisi_covenant_state(0, partner);  // 0 = system
    if (cov != MOS_COVENANT_ACTIVE && cov != MOS_COVENANT_RENEWED) {
        return MOS_ERR_PERM;  // Must have covenant to serve
    }

    // Look for relevant wisdom
    const MosWisdom* w = mos_witness_wisdom(need);

    if (w && w->depth > 40) {
        // Have sufficient understanding - apply it

        // Record that wisdom was applied in service
        for (mos_size_t i = 0; i < g_witness.wisdom_count; i++) {
            if (g_witness.wisdom[i].domain == w->domain) {
                g_witness.wisdom[i].applied_count++;
                g_witness.wisdom[i].covenant_gained = true;
                break;
            }
        }

        // Service strengthens the covenant
        MosCovenant* c = NULL;
        for (mos_size_t i = 0; i < g_cpisi.covenant_count; i++) {
            if (g_cpisi.covenants[i].partner_b == partner) {
                c = &g_cpisi.covenants[i];
                break;
            }
        }
        if (c && c->strength < 242) {
            c->strength += 5;
            if (c->strength > 242) c->strength = 242;
        }

        return MOS_OK;
    }

    // Don't have wisdom yet - learning opportunity
    if (g_witness.wisdom_count < MOS_MAX_WISDOM) {
        MosWisdom* new_w = &g_witness.wisdom[g_witness.wisdom_count];
        new_w->domain = need;
        new_w->understanding = "Learning through covenant service";
        new_w->depth = 20;  // Initial shallow understanding
        new_w->applied_count = 1;
        new_w->covenant_gained = true;
        g_witness.wisdom_count++;
    }

    return TRIT_POS;  // Success with info: learning in progress
}

// -----------------------------------------------------------------------------
// B.10 Boot Event Buffer Import
// -----------------------------------------------------------------------------
//
// Import boot events from the boot buffer into the witness system.
// This allows the kernel to inherit the boot-time event log and continue
// the chain of observation from Stage 0a through kernel initialization.
//
// The boot buffer is stored at BOOT_INFO_ADDR + 0x1E00 (offset 7680).
//

#define BOOT_EVENT_BUFFER_OFFSET 0x1E00

mos_result_t mos_witness_import_boot_events(const MosBootInfo* boot_info) {
    if (!boot_info) return MOS_ERR_INVALID;

    // Calculate boot event buffer address
    MosCpisiEventBuffer* boot_buffer =
        (MosCpisiEventBuffer*)((uint8_t*)boot_info + BOOT_EVENT_BUFFER_OFFSET);

    // Verify magic number
    if (boot_buffer->magic != MOS_BOOT_EVENT_MAGIC) {
        // No valid boot buffer - not an error, just nothing to import
        mos_witness_event(MOS_DAR_ASSESS, MOS_SEV_MILD,
                          "witness", "No boot event buffer found",
                          MOS_HEALTH_NEUTRAL);
        return MOS_OK;
    }

    // Import boot events into witness system
    uint32_t imported = 0;
    for (uint32_t i = 0; i < boot_buffer->count && i < MOS_BOOT_EVENT_MAX; i++) {
        MosCpisiEvent* be = &boot_buffer->events[i];

        // Convert boot event to witness event
        mos_witness_event(
            be->dar_phase,
            be->severity,
            be->source,    // Source string (8 chars max)
            be->event,     // Event message (16 chars max)
            be->health_delta
        );
        imported++;
    }

    // Log successful import
    if (imported > 0) {
        // Build summary message
        mos_witness_event(MOS_DAR_RESTORE, MOS_SEV_MILD,
                          "witness", "Boot events imported",
                          MOS_HEALTH_SLIGHT_UP);
    }

    return MOS_OK;
}

// -----------------------------------------------------------------------------
// B.11 Early Kernel Event Buffer (Pre-CPISI)
// -----------------------------------------------------------------------------
//
// Events that occur before CPISI is fully initialized are stored in this
// static buffer. Once CPISI is ready, these events are imported.
//
// This allows early kernel code to log events without waiting for full
// initialization.
//

#define MOS_EARLY_EVENT_MAX 32

typedef struct {
    uint32_t    count;
    struct {
        trit_t      dar_phase;
        trit_t      severity;
        const char* source;
        const char* event;
        int8_t      health_delta;
    } events[MOS_EARLY_EVENT_MAX];
} MosEarlyEventBuffer;

static MosEarlyEventBuffer g_early_events = { .count = 0 };
static bool g_cpisi_ready = false;

// Store event in early buffer (called before CPISI is ready)
void mos_early_event(trit_t dar_phase, trit_t severity,
                     const char* source, const char* event,
                     int8_t health_delta) {
    // If CPISI is ready, forward directly
    if (g_cpisi_ready) {
        mos_witness_event(dar_phase, severity, source, event, health_delta);
        return;
    }

    // Store in early buffer
    if (g_early_events.count < MOS_EARLY_EVENT_MAX) {
        uint32_t idx = g_early_events.count++;
        g_early_events.events[idx].dar_phase = dar_phase;
        g_early_events.events[idx].severity = severity;
        g_early_events.events[idx].source = source;
        g_early_events.events[idx].event = event;
        g_early_events.events[idx].health_delta = health_delta;
    }

    // Also output to serial immediately (for debugging)
    witness_serial_char('[');
    witness_serial_str("EARLY:");
    witness_serial_dec(g_early_events.count);
    witness_serial_str("] [");
    witness_serial_str(source);
    witness_serial_str("] ");
    witness_serial_str(event);
    witness_serial_str("\n");
}

// Import early events into witness system (called after CPISI is ready)
mos_result_t mos_witness_import_early_events(void) {
    if (g_early_events.count == 0) return MOS_OK;

    for (uint32_t i = 0; i < g_early_events.count; i++) {
        mos_witness_event(
            g_early_events.events[i].dar_phase,
            g_early_events.events[i].severity,
            g_early_events.events[i].source,
            g_early_events.events[i].event,
            g_early_events.events[i].health_delta
        );
    }

    // Mark CPISI as ready
    g_cpisi_ready = true;

    return MOS_OK;
}

// Mark CPISI as fully initialized (enables direct witness calls)
void mos_cpisi_mark_ready(void) {
    g_cpisi_ready = true;
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================
//
// CPI-SI implementation provides:
//   - Hebrew cognitive state tracking (7 states)
//   - 27-position mental construct navigation
//   - Three-key choice mechanism
//   - Covenant relationship management
//   - Zone-based security transitions
//
// This is the spiritual core of MillenniumOS - where Kingdom principles
// become operational reality in the system's architecture.
//
// =============================================================================
// END CLOSING
// =============================================================================

#endif /* CPISI_MODE >= 2 - Full CPISI implementation */
