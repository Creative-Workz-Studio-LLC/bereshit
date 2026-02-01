// =============================================================================
// METADATA [METADATA]
// =============================================================================
//
// Key:         MOS-kernel-hal-impl
// Title:       MillenniumOS Hardware Abstraction Layer Implementation
// Type:        Source
// Component:   Stage 2 (HAL)
// Role:        Abstract hardware access - display, input, storage, audio
//
// Status:      Active
// Version:     a-01.00
// Created:     2026-01-30
// Updated:     2026-01-30
//
// Authors:     Seanje Lenox-Wise (Architect), Nova Dawn (Author)
// Organization: CreativeWorkzStudio LLC
//
// Grounding:   Genesis 1:6 — "Let there be a firmament"
//
// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "hal.h"
#include "memory/memory.h"
#include "interrupt/interrupt.h"
#include "boot/include/bootinfo.h"

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 Global HAL State
// -----------------------------------------------------------------------------

static MosHal g_hal;

// -----------------------------------------------------------------------------
// B.2 Helper Functions
// -----------------------------------------------------------------------------

static void hal_memset(void* ptr, uint8_t val, mos_size_t size) {
    uint8_t* p = (uint8_t*)ptr;
    while (size--) *p++ = val;
}

// -----------------------------------------------------------------------------
// B.3 Keyboard Driver
// -----------------------------------------------------------------------------

// PS/2 keyboard ports
#define PS2_DATA_PORT       0x60
#define PS2_STATUS_PORT     0x64
#define PS2_COMMAND_PORT    0x64

// Extended scancode tracking
static bool g_e0_prefix = false;   // E0 extended key prefix received
static bool g_e1_prefix = false;   // E1 prefix (Pause key) received

/**
 * PS/2 Set 1 to evdev scancode mapping.
 *
 * PS/2 Set 1 scancodes are what the hardware sends.
 * evdev scancodes are the Linux standard that keyboard.c expects.
 * This table maps base scancodes (0x00-0x7F).
 */
static const uint16_t ps2_to_evdev[128] = {
    /*      0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F */
    /* 0 */ 0,    1,    2,    3,    4,    5,    6,    7,    8,    9,   10,   11,   12,   13,   14,   15,
    /* 1 */16,   17,   18,   19,   20,   21,   22,   23,   24,   25,   26,   27,   28,   29,   30,   31,
    /* 2 */32,   33,   34,   35,   36,   37,   38,   39,   40,   41,   42,   43,   44,   45,   46,   47,
    /* 3 */48,   49,   50,   51,   52,   53,   54,   55,   56,   57,   58,   59,   60,   61,   62,   63,
    /* 4 */64,   65,   66,   67,   68,   69,   70,   71,   72,   73,   74,   75,   76,   77,   78,   79,
    /* 5 */80,   81,   82,   83,   84,   85,   86,   87,   88,    0,    0,    0,    0,    0,    0,    0,
    /* 6 */ 0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    /* 7 */ 0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
};

/**
 * PS/2 Set 1 E0-extended to evdev scancode mapping.
 *
 * When E0 prefix is received, the next scancode maps here.
 * These are navigation keys, right-side modifiers, etc.
 */
static const uint16_t ps2_e0_to_evdev[128] = {
    /*      0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F */
    /* 0 */ 0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    /* 1 */ 0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,   96,   97,    0,    0, /* 1C=KP_Enter, 1D=RCtrl */
    /* 2 */ 0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    /* 3 */ 0,    0,    0,    0,    0,   98,    0,    0,  100,    0,    0,    0,    0,    0,    0,    0, /* 35=KP_Div, 38=RAlt */
    /* 4 */ 0,    0,    0,    0,    0,    0,    0,  102,  103,  104,    0,  105,    0,  106,    0,  107, /* 47=Home, 48=Up, 49=PgUp, 4B=Left, 4D=Right, 4F=End */
    /* 5 */108,  109,  110,  111,    0,    0,    0,    0,    0,    0,    0,  125,  126,  127,    0,    0, /* 50=Down, 51=PgDn, 52=Insert, 53=Delete, 5B=LSuper, 5C=RSuper, 5D=Menu */
    /* 6 */ 0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    /* 7 */ 0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
};

/**
 * evdev scancode to ASCII character mapping.
 *
 * Maps evdev scancodes to printable ASCII characters (unshifted).
 * Non-printable keys map to 0.
 */
static const char evdev_to_ascii[128] = {
    /*      0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F */
    /* 0 */ 0,   27,  '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=','\b','\t',
    /* 1 */'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']','\n',   0, 'a', 's',
    /* 2 */'d', 'f', 'g', 'h', 'j', 'k', 'l', ';','\'', '`',   0,'\\', 'z', 'x', 'c', 'v',
    /* 3 */'b', 'n', 'm', ',', '.', '/',   0, '*',   0, ' ',   0,   0,   0,   0,   0,   0,
    /* 4 */  0,   0,   0,   0,   0,   0,   0, '7', '8', '9', '-', '4', '5', '6', '+', '1',
    /* 5 */'2', '3', '0', '.',   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    /* 6 */'\n',  0, '/',   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    /* 7 */  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
};

/**
 * evdev scancode to shifted ASCII character mapping.
 */
static const char evdev_to_ascii_shifted[128] = {
    /*      0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F */
    /* 0 */ 0,   27,  '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+','\b','\t',
    /* 1 */'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}','\n',   0, 'A', 'S',
    /* 2 */'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',   0, '|', 'Z', 'X', 'C', 'V',
    /* 3 */'B', 'N', 'M', '<', '>', '?',   0, '*',   0, ' ',   0,   0,   0,   0,   0,   0,
    /* 4 */  0,   0,   0,   0,   0,   0,   0, '7', '8', '9', '-', '4', '5', '6', '+', '1',
    /* 5 */'2', '3', '0', '.',   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    /* 6 */'\n',  0, '/',   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    /* 7 */  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
};

// Keyboard interrupt handler
static void keyboard_handler(MosInterruptFrame* frame) {
    (void)frame;

    uint8_t raw = mos_inb(PS2_DATA_PORT);

    // Handle E0 prefix (extended keys)
    if (raw == 0xE0) {
        g_e0_prefix = true;
        return;  // Wait for actual scancode
    }

    // Handle E1 prefix (Pause key - 2 byte sequence)
    if (raw == 0xE1) {
        g_e1_prefix = true;
        return;  // Wait for rest of sequence
    }

    // Skip E1 sequence bytes (Pause key generates E1 1D 45 / E1 9D C5)
    if (g_e1_prefix) {
        // Simple handling: ignore E1 sequences for now
        // Full implementation would track and emit KEY_PAUSE
        if (raw == 0xC5 || raw == 0x45) {
            g_e1_prefix = false;
        }
        return;
    }

    // Extract release bit and base scancode
    bool released = (raw & 0x80) != 0;
    uint8_t scancode = raw & 0x7F;

    // Map PS/2 scancode to evdev scancode
    uint16_t evdev_code;
    if (g_e0_prefix) {
        evdev_code = ps2_e0_to_evdev[scancode];
        g_e0_prefix = false;
    } else {
        evdev_code = ps2_to_evdev[scancode];
    }

    // Skip unmapped scancodes
    if (evdev_code == 0 && scancode != 0) {
        return;
    }

    // Build key event
    MosKeyEvent key_event;
    hal_memset(&key_event, 0, sizeof(key_event));

    key_event.scancode = evdev_code;
    key_event.state = released ? MOS_KEY_UP : MOS_KEY_DOWN;

    // Update modifier state based on evdev scancodes
    switch (evdev_code) {
        case 42: case 54:  // Left Shift, Right Shift
            if (!released) {
                g_hal.modifiers |= MOS_MOD_SHIFT;
            } else {
                g_hal.modifiers &= ~MOS_MOD_SHIFT;
            }
            break;
        case 29: case 97:  // Left Ctrl, Right Ctrl
            if (!released) {
                g_hal.modifiers |= MOS_MOD_CTRL;
            } else {
                g_hal.modifiers &= ~MOS_MOD_CTRL;
            }
            break;
        case 56: case 100:  // Left Alt, Right Alt
            if (!released) {
                g_hal.modifiers |= MOS_MOD_ALT;
            } else {
                g_hal.modifiers &= ~MOS_MOD_ALT;
            }
            break;
        case 125: case 126:  // Left Super, Right Super
            if (!released) {
                g_hal.modifiers |= MOS_MOD_SUPER;
            } else {
                g_hal.modifiers &= ~MOS_MOD_SUPER;
            }
            break;
    }

    key_event.modifiers = g_hal.modifiers;

    // Translate evdev scancode to ASCII character
    if (evdev_code < 128) {
        if (g_hal.modifiers & MOS_MOD_SHIFT) {
            key_event.character = evdev_to_ascii_shifted[evdev_code];
        } else {
            key_event.character = evdev_to_ascii[evdev_code];
        }
        // Store keycode as the ASCII value for compatibility
        key_event.keycode = (uint8_t)key_event.character;
    }

    // Update key state table (use evdev scancode as index, clamped)
    if (evdev_code < 256) {
        g_hal.key_states[evdev_code] = released ? 0 : 1;
    }

    // Push event to queue
    MosEvent event;
    event.type = MOS_EVENT_KEY;
    event.timestamp = mos_hal_ticks_ms();
    event.key = key_event;
    mos_hal_push_event(&event);
}

// -----------------------------------------------------------------------------
// B.4 Mouse Driver
// -----------------------------------------------------------------------------

// PS/2 mouse state
static uint8_t mouse_cycle = 0;
static int8_t mouse_bytes[3];

// Mouse interrupt handler
static void mouse_handler(MosInterruptFrame* frame) {
    (void)frame;

    uint8_t status = mos_inb(PS2_STATUS_PORT);
    if (!(status & 0x20)) return;  // Not mouse data

    mouse_bytes[mouse_cycle++] = mos_inb(PS2_DATA_PORT);

    if (mouse_cycle >= 3) {
        mouse_cycle = 0;

        // Parse mouse packet
        MosMouseEvent mouse_event;
        hal_memset(&mouse_event, 0, sizeof(mouse_event));

        // Byte 0: buttons and signs
        uint8_t flags = mouse_bytes[0];
        mouse_event.buttons = 0;
        if (flags & 0x01) mouse_event.buttons |= MOS_MOUSE_LEFT;
        if (flags & 0x02) mouse_event.buttons |= MOS_MOUSE_RIGHT;
        if (flags & 0x04) mouse_event.buttons |= MOS_MOUSE_MIDDLE;

        // Byte 1: X movement
        mouse_event.dx = mouse_bytes[1];
        if (flags & 0x10) mouse_event.dx |= 0xFFFFFF00;  // Sign extend

        // Byte 2: Y movement (inverted)
        mouse_event.dy = -mouse_bytes[2];
        if (flags & 0x20) mouse_event.dy |= 0xFFFFFF00;

        // Update position
        g_hal.mouse_x += mouse_event.dx;
        g_hal.mouse_y += mouse_event.dy;

        // Clamp to display bounds
        MosDisplay* disp = &g_hal.displays[g_hal.primary_display];
        if (disp->active) {
            if (g_hal.mouse_x < 0) g_hal.mouse_x = 0;
            if (g_hal.mouse_y < 0) g_hal.mouse_y = 0;
            if (g_hal.mouse_x >= (int32_t)disp->mode.width)
                g_hal.mouse_x = disp->mode.width - 1;
            if (g_hal.mouse_y >= (int32_t)disp->mode.height)
                g_hal.mouse_y = disp->mode.height - 1;
        }

        mouse_event.x = g_hal.mouse_x;
        mouse_event.y = g_hal.mouse_y;

        // Check for button changes
        if (mouse_event.buttons != g_hal.mouse_buttons) {
            MosEvent event;
            event.type = MOS_EVENT_MOUSE_BUTTON;
            event.timestamp = 0;
            event.mouse = mouse_event;
            mos_hal_push_event(&event);
            g_hal.mouse_buttons = mouse_event.buttons;
        }

        // Always send move event if there was movement
        if (mouse_event.dx != 0 || mouse_event.dy != 0) {
            MosEvent event;
            event.type = MOS_EVENT_MOUSE_MOVE;
            event.timestamp = 0;
            event.mouse = mouse_event;
            mos_hal_push_event(&event);
        }
    }
}

// Initialize PS/2 mouse
static void init_ps2_mouse(void) {
    // Enable auxiliary device (mouse)
    mos_outb(PS2_COMMAND_PORT, 0xA8);
    mos_io_wait();

    // Enable interrupts
    mos_outb(PS2_COMMAND_PORT, 0x20);
    mos_io_wait();
    uint8_t status = mos_inb(PS2_DATA_PORT);
    status |= 0x02;  // Enable IRQ12
    mos_outb(PS2_COMMAND_PORT, 0x60);
    mos_io_wait();
    mos_outb(PS2_DATA_PORT, status);
    mos_io_wait();

    // Tell mouse to use default settings
    mos_outb(PS2_COMMAND_PORT, 0xD4);
    mos_io_wait();
    mos_outb(PS2_DATA_PORT, 0xF6);
    mos_io_wait();
    mos_inb(PS2_DATA_PORT);  // ACK

    // Enable mouse
    mos_outb(PS2_COMMAND_PORT, 0xD4);
    mos_io_wait();
    mos_outb(PS2_DATA_PORT, 0xF4);
    mos_io_wait();
    mos_inb(PS2_DATA_PORT);  // ACK
}

// -----------------------------------------------------------------------------
// B.5 Initialization
// -----------------------------------------------------------------------------

// Forward declaration for timer init
static void init_pit_timer(void);

mos_result_t mos_hal_init(void) {
    // Clear state
    hal_memset(&g_hal, 0, sizeof(g_hal));

    g_hal.health = MOS_HEALTH_NEUTRAL;

    // Initialize PIT timer for timing functions
    init_pit_timer();

    return MOS_OK;
}

mos_result_t mos_hal_init_display(const MosBootInfo* boot) {
    if (!boot) return MOS_ERR_INVALID;

    // Initialize primary display from boot info
    MosDisplay* disp = &g_hal.displays[0];
    disp->id = 0;
    disp->active = true;

    // Copy framebuffer info
    disp->framebuffer = (mos_paddr_t)boot->framebuffer.address;
    disp->mapped_fb = (mos_vaddr_t)boot->framebuffer.address;  // Identity mapped for now
    disp->fb_size = boot->framebuffer.pitch * boot->framebuffer.height;

    // Set video mode
    disp->mode.width = boot->framebuffer.width;
    disp->mode.height = boot->framebuffer.height;
    disp->mode.pitch = boot->framebuffer.pitch;
    disp->mode.bpp = boot->framebuffer.bpp;
    disp->mode.refresh = 60;  // Assume 60Hz

    // Copy color shift values from boot info
    disp->red_shift = boot->framebuffer.red_shift;
    disp->green_shift = boot->framebuffer.green_shift;
    disp->blue_shift = boot->framebuffer.blue_shift;

    // Determine if we need to swap R and B channels
    // KT_RGB produces 0xAARRGGBB (red at bit 16, blue at bit 0)
    // If framebuffer expects blue at bit 16 (RGB format), we need to swap
    disp->swap_rb = (boot->framebuffer.red_shift == 0 &&
                     boot->framebuffer.blue_shift == 16);

    // Determine pixel format
    if (disp->mode.bpp == 32) {
        if (disp->swap_rb) {
            disp->mode.format = MOS_PIXEL_BGRA8888;  // RGB framebuffer needs swap
        } else {
            disp->mode.format = MOS_PIXEL_RGBA8888;  // BGR framebuffer (common)
        }
    } else if (disp->mode.bpp == 24) {
        disp->mode.format = MOS_PIXEL_RGB888;
    } else if (disp->mode.bpp == 16) {
        disp->mode.format = MOS_PIXEL_RGB565;
    } else {
        disp->mode.format = MOS_PIXEL_INDEXED8;
    }

    disp->capabilities = MOS_DISP_CAP_GRAPHICS;
    disp->health = MOS_HEALTH_NEUTRAL;

    g_hal.display_count = 1;
    g_hal.primary_display = 0;

    // Center mouse
    g_hal.mouse_x = disp->mode.width / 2;
    g_hal.mouse_y = disp->mode.height / 2;

    return MOS_OK;
}

mos_result_t mos_hal_init_input(void) {
    // Register keyboard interrupt handler
    mos_result_t res = mos_interrupt_register(MOS_IRQ_KEYBOARD, keyboard_handler,
                                               TRIT_POS, "keyboard");
    if (res != MOS_OK) return res;

    // Initialize PS/2 mouse
    init_ps2_mouse();

    // Register mouse interrupt handler
    res = mos_interrupt_register(MOS_IRQ_MOUSE, mouse_handler,
                                  TRIT_POS, "mouse");
    if (res != MOS_OK) return res;

    // Enable keyboard and mouse IRQs
    mos_irq_enable(1);   // IRQ1 = keyboard
    mos_irq_enable(12);  // IRQ12 = mouse

    // Record devices
    g_hal.keyboards[0].id = 0;
    g_hal.keyboards[0].connected = true;
    g_hal.keyboards[0].name = "PS/2 Keyboard";
    g_hal.keyboards[0].health = MOS_HEALTH_NEUTRAL;
    g_hal.keyboard_count = 1;

    g_hal.mice[0].id = 0;
    g_hal.mice[0].connected = true;
    g_hal.mice[0].name = "PS/2 Mouse";
    g_hal.mice[0].health = MOS_HEALTH_NEUTRAL;
    g_hal.mouse_count = 1;

    return MOS_OK;
}

// -----------------------------------------------------------------------------
// B.6 Display Operations
// -----------------------------------------------------------------------------

const MosDisplay* mos_hal_display(uint8_t id) {
    if (id >= MOS_MAX_DISPLAYS) return NULL;
    if (!g_hal.displays[id].active) return NULL;
    return &g_hal.displays[id];
}

const MosDisplay* mos_hal_primary_display(void) {
    return mos_hal_display(g_hal.primary_display);
}

mos_result_t mos_hal_set_mode(uint8_t display, const MosVideoMode* mode) {
    if (display >= MOS_MAX_DISPLAYS) return MOS_ERR_INVALID;
    if (!mode) return MOS_ERR_INVALID;

    // For now, we only support the boot-time mode
    // Real implementation would program VGA/VESA registers
    (void)mode;

    return MOS_ERR_PERM;  // Mode change not supported yet
}

mos_vaddr_t mos_hal_framebuffer(uint8_t display) {
    if (display >= MOS_MAX_DISPLAYS) return 0;
    return g_hal.displays[display].mapped_fb;
}

mos_result_t mos_hal_swap_buffers(uint8_t display) {
    // No double buffering yet
    (void)display;
    return MOS_OK;
}

mos_result_t mos_hal_vsync(uint8_t display) {
    // Simple vsync wait (VGA port 0x3DA)
    (void)display;
    while (mos_inb(0x3DA) & 0x08);  // Wait for vsync to end
    while (!(mos_inb(0x3DA) & 0x08));  // Wait for vsync to start
    return MOS_OK;
}

// -----------------------------------------------------------------------------
// B.7 Event Queue Operations
// -----------------------------------------------------------------------------

mos_result_t mos_hal_push_event(const MosEvent* event) {
    if (!event) return MOS_ERR_INVALID;

    if (g_hal.event_count >= MOS_EVENT_QUEUE_SIZE) {
        // Queue full, drop oldest event
        g_hal.event_tail = (g_hal.event_tail + 1) % MOS_EVENT_QUEUE_SIZE;
        g_hal.event_count--;
    }

    g_hal.events[g_hal.event_head] = *event;
    g_hal.event_head = (g_hal.event_head + 1) % MOS_EVENT_QUEUE_SIZE;
    g_hal.event_count++;
    g_hal.total_events++;

    return MOS_OK;
}

bool mos_hal_poll_event(MosEvent* event) {
    if (g_hal.event_count == 0) {
        return false;
    }

    if (event) {
        *event = g_hal.events[g_hal.event_tail];
    }
    g_hal.event_tail = (g_hal.event_tail + 1) % MOS_EVENT_QUEUE_SIZE;
    g_hal.event_count--;

    return true;
}

mos_result_t mos_hal_wait_event(MosEvent* event) {
    // Busy wait for event (real implementation would use HLT)
    while (g_hal.event_count == 0) {
        __asm__ volatile("hlt");
    }

    if (event) {
        *event = g_hal.events[g_hal.event_tail];
    }
    g_hal.event_tail = (g_hal.event_tail + 1) % MOS_EVENT_QUEUE_SIZE;
    g_hal.event_count--;

    return MOS_OK;
}

// -----------------------------------------------------------------------------
// B.8 Input State Queries
// -----------------------------------------------------------------------------

trit_t mos_hal_key_state(uint8_t keycode) {
    return g_hal.key_states[keycode] ? MOS_KEY_DOWN : MOS_KEY_UP;
}

void mos_hal_mouse_position(int32_t* x, int32_t* y) {
    if (x) *x = g_hal.mouse_x;
    if (y) *y = g_hal.mouse_y;
}

trit5_t mos_hal_mouse_buttons(void) {
    return g_hal.mouse_buttons;
}

mos_result_t mos_hal_controller_state(uint8_t id, MosControllerState* state) {
    if (id >= MOS_MAX_CONTROLLERS) return MOS_ERR_INVALID;
    if (!g_hal.controllers[id].connected) return MOS_ERR_NOTFOUND;
    if (!state) return MOS_ERR_INVALID;

    // No controllers implemented yet
    return MOS_ERR_NOTFOUND;
}

// -----------------------------------------------------------------------------
// B.9 Information
// -----------------------------------------------------------------------------

const MosHal* mos_hal_info(void) {
    return &g_hal;
}

mos_health_t mos_hal_health(void) {
    return g_hal.health;
}

// -----------------------------------------------------------------------------
// B.10 Timing Functions
// -----------------------------------------------------------------------------

// PIT (Programmable Interval Timer) constants
#define PIT_FREQUENCY       1193182     // 1.193182 MHz
#define PIT_CHANNEL0        0x40        // Channel 0 data port
#define PIT_COMMAND         0x43        // Mode/Command register
#define PIT_DIVIDER         11932       // ~100 Hz tick rate (PIT_FREQUENCY / 100)

// Tick count (incremented by timer interrupt)
static volatile uint64_t g_tick_count = 0;
static volatile uint64_t g_boot_ticks = 0;

// Timer interrupt handler
static void timer_handler(MosInterruptFrame* frame) {
    (void)frame;
    g_tick_count++;
}

// Initialize PIT timer (called during HAL init)
static void init_pit_timer(void) {
    // Configure PIT channel 0 in rate generator mode (mode 2)
    // Command byte: channel 0, access mode lobyte/hibyte, mode 2, binary
    mos_outb(PIT_COMMAND, 0x34);  // 00110100

    // Set divisor for ~100 Hz
    mos_outb(PIT_CHANNEL0, PIT_DIVIDER & 0xFF);         // Low byte
    mos_outb(PIT_CHANNEL0, (PIT_DIVIDER >> 8) & 0xFF);  // High byte

    // Register timer interrupt handler (IRQ0)
    mos_interrupt_register(MOS_IRQ_TIMER, timer_handler, TRIT_ZERO, "pit_timer");
    mos_irq_enable(0);  // IRQ0 = PIT
}

uint64_t mos_hal_ticks_ms(void) {
    // Each tick is approximately 10ms (at 100Hz)
    return g_tick_count * 10;
}

uint64_t mos_hal_ticks_us(void) {
    // Approximate: each tick is 10000 microseconds at 100Hz
    // For more precision, read PIT counter directly
    uint64_t base_us = g_tick_count * 10000;

    // Read current PIT counter for sub-tick precision
    // Latch channel 0 counter
    mos_outb(PIT_COMMAND, 0x00);  // Latch command for channel 0

    uint8_t lo = mos_inb(PIT_CHANNEL0);
    uint8_t hi = mos_inb(PIT_CHANNEL0);
    uint16_t counter = (uint16_t)lo | ((uint16_t)hi << 8);

    // Counter counts down from PIT_DIVIDER
    // Convert remaining counts to microseconds
    uint32_t elapsed_counts = PIT_DIVIDER - counter;
    uint32_t sub_tick_us = (elapsed_counts * 10000) / PIT_DIVIDER;

    return base_us + sub_tick_us;
}

void mos_hal_sleep(uint32_t milliseconds) {
    if (milliseconds == 0) return;

    uint64_t target = mos_hal_ticks_ms() + milliseconds;

    // Yield to scheduler while waiting
    while (mos_hal_ticks_ms() < target) {
        // HLT waits for next interrupt, saves power
        __asm__ volatile("hlt");
    }
}

void mos_hal_sleep_us(uint32_t microseconds) {
    if (microseconds == 0) return;

    // For very short delays (<1ms), use busy-wait with PIT counter
    if (microseconds < 1000) {
        uint64_t target = mos_hal_ticks_us() + microseconds;
        while (mos_hal_ticks_us() < target) {
            // Busy wait - no HLT for precision
            __asm__ volatile("pause");  // Hint for spin-wait
        }
    } else {
        // For longer delays, use millisecond sleep
        mos_hal_sleep((microseconds + 999) / 1000);
    }
}

uint64_t mos_hal_timer_frequency(void) {
    return 100;  // 100 Hz tick rate
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================
//
// HAL implementation provides:
//   - Display initialization from boot info
//   - PS/2 keyboard driver with scancode translation
//   - PS/2 mouse driver with movement/button tracking
//   - Event queue for input handling
//   - Low-level port I/O functions
//
// =============================================================================
// END CLOSING
// =============================================================================
