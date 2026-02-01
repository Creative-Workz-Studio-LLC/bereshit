// =============================================================================
// METADATA [METADATA]
// =============================================================================
//
// Key:         MOS-kernel-hal
// Title:       MillenniumOS Hardware Abstraction Layer
// Type:        Header
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
// Grounding:   Genesis 1:6 — "Let there be a firmament in the midst of the waters"
//              HAL is the firmament - separating hardware from software.
//
// =============================================================================
// END METADATA
// =============================================================================

#ifndef MILLENNIUMOS_HAL_H
#define MILLENNIUMOS_HAL_H

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "kernel.h"
#include "boot/include/bootinfo.h"

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 Display Constants
// -----------------------------------------------------------------------------

#define MOS_MAX_DISPLAYS        3       // 3^1 displays
#define MOS_MAX_MODES           27      // 3^3 video modes

// Display capabilities (trit5 flags)
#define MOS_DISP_CAP_TEXT       ((trit5_t)1)     // Text mode
#define MOS_DISP_CAP_GRAPHICS   ((trit5_t)3)     // Graphics mode
#define MOS_DISP_CAP_ACCEL      ((trit5_t)9)     // Hardware acceleration
#define MOS_DISP_CAP_DOUBLE     ((trit5_t)27)    // Double buffering
#define MOS_DISP_CAP_VSYNC      ((trit5_t)81)    // VSync support

// Pixel formats
typedef enum MosPixelFormat {
    MOS_PIXEL_RGB565    = 0,    // 16-bit RGB
    MOS_PIXEL_RGB888    = 1,    // 24-bit RGB
    MOS_PIXEL_RGBA8888  = 2,    // 32-bit RGBA (0xAARRGGBB - matches KT_RGB)
    MOS_PIXEL_BGRA8888  = 3,    // 32-bit BGRA (0xAABBGGRR - swap R/B)
    MOS_PIXEL_INDEXED8  = 4,    // 8-bit indexed
} MosPixelFormat;

// -----------------------------------------------------------------------------
// B.2 Display Structures
// -----------------------------------------------------------------------------

typedef struct MosVideoMode {
    uint32_t        width;          // Width in pixels
    uint32_t        height;         // Height in pixels
    uint32_t        pitch;          // Bytes per row
    uint8_t         bpp;            // Bits per pixel
    MosPixelFormat  format;         // Pixel format
    uint32_t        refresh;        // Refresh rate (Hz)
} MosVideoMode;

typedef struct {
    uint8_t         id;             // Display ID (0-2)
    bool            active;         // Is display active?
    MosVideoMode    mode;           // Current video mode
    mos_paddr_t     framebuffer;    // Framebuffer physical address
    mos_vaddr_t     mapped_fb;      // Mapped framebuffer address
    mos_size_t      fb_size;        // Framebuffer size
    trit5_t         capabilities;   // Display capabilities
    mos_health_t    health;         // Display health

    // Color format info from boot
    uint8_t         red_shift;      // Red channel bit position
    uint8_t         green_shift;    // Green channel bit position
    uint8_t         blue_shift;     // Blue channel bit position
    bool            swap_rb;        // Need to swap R/B? (true for RGB framebuffers)
} MosDisplay;

// -----------------------------------------------------------------------------
// B.3 Input Constants
// -----------------------------------------------------------------------------

#define MOS_MAX_KEYBOARDS       3
#define MOS_MAX_MICE            3
#define MOS_MAX_CONTROLLERS     9       // 3^2 game controllers

// Key states (ternary)
#define MOS_KEY_UP              TRIT_NEG    // Key released
#define MOS_KEY_IDLE            TRIT_ZERO   // Key idle
#define MOS_KEY_DOWN            TRIT_POS    // Key pressed

// Modifier keys (trit5 flags)
#define MOS_MOD_SHIFT           ((trit5_t)1)
#define MOS_MOD_CTRL            ((trit5_t)3)
#define MOS_MOD_ALT             ((trit5_t)9)
#define MOS_MOD_SUPER           ((trit5_t)27)   // Windows/Command key
#define MOS_MOD_CAPSLOCK        ((trit5_t)81)
#define MOS_MOD_NUMLOCK         ((trit5_t)162)

// Mouse buttons (trit5 flags)
#define MOS_MOUSE_LEFT          ((trit5_t)1)
#define MOS_MOUSE_RIGHT         ((trit5_t)3)
#define MOS_MOUSE_MIDDLE        ((trit5_t)9)
#define MOS_MOUSE_EXTRA1        ((trit5_t)27)
#define MOS_MOUSE_EXTRA2        ((trit5_t)81)

// -----------------------------------------------------------------------------
// B.4 Input Structures
// -----------------------------------------------------------------------------

typedef struct MosKeyEvent {
    uint8_t         scancode;       // Hardware scancode
    uint8_t         keycode;        // Translated keycode
    trit_t          state;          // Key state (-1=up, 0=idle, +1=down)
    trit5_t         modifiers;      // Active modifiers
    char            character;      // Translated character (if printable)
} MosKeyEvent;

typedef struct MosMouseEvent {
    int32_t         x;              // X position
    int32_t         y;              // Y position
    int32_t         dx;             // X delta (movement)
    int32_t         dy;             // Y delta
    int32_t         wheel;          // Scroll wheel delta
    trit5_t         buttons;        // Button state flags
} MosMouseEvent;

typedef struct MosControllerState {
    uint8_t         id;             // Controller ID
    int16_t         axes[8];        // Analog axes (-32768 to 32767)
    trit5_t         buttons;        // Digital buttons
    uint8_t         triggers[2];    // L/R triggers (0-255)
} MosControllerState;

// Input device info
typedef struct MosInputDevice {
    uint8_t         id;
    bool            connected;
    const char*     name;
    mos_health_t    health;
} MosInputDevice;

// -----------------------------------------------------------------------------
// B.5 Event Queue
// -----------------------------------------------------------------------------

typedef enum MosEventType {
    MOS_EVENT_NONE          = 0,
    MOS_EVENT_KEY           = 1,
    MOS_EVENT_MOUSE_MOVE    = 2,
    MOS_EVENT_MOUSE_BUTTON  = 3,
    MOS_EVENT_MOUSE_WHEEL   = 4,
    MOS_EVENT_CONTROLLER    = 5,
    MOS_EVENT_DISPLAY       = 6,    // Display connect/disconnect
    MOS_EVENT_QUIT          = 243,  // Spare value: quit request
} MosEventType;

typedef struct MosEvent {
    MosEventType    type;
    trit27_t        timestamp;      // Event timestamp
    union {
        MosKeyEvent         key;
        MosMouseEvent       mouse;
        MosControllerState  controller;
        uint8_t             display_id;
    };  // Anonymous union (C11) - members accessed directly
} MosEvent;

#define MOS_EVENT_QUEUE_SIZE    243     // 3^5 events

// -----------------------------------------------------------------------------
// B.6 HAL State
// -----------------------------------------------------------------------------

typedef struct MosHal {
    // Displays
    MosDisplay      displays[MOS_MAX_DISPLAYS];
    uint8_t         display_count;
    uint8_t         primary_display;

    // Input devices
    MosInputDevice  keyboards[MOS_MAX_KEYBOARDS];
    MosInputDevice  mice[MOS_MAX_MICE];
    MosInputDevice  controllers[MOS_MAX_CONTROLLERS];
    uint8_t         keyboard_count;
    uint8_t         mouse_count;
    uint8_t         controller_count;

    // Current input state
    uint8_t         key_states[256];    // All key states
    trit5_t         modifiers;          // Active modifiers
    int32_t         mouse_x, mouse_y;   // Mouse position
    trit5_t         mouse_buttons;      // Mouse button state

    // Event queue
    MosEvent        events[MOS_EVENT_QUEUE_SIZE];
    mos_size_t      event_head;
    mos_size_t      event_tail;
    mos_size_t      event_count;

    // Statistics
    trit27_t        total_events;
    mos_health_t    health;
} MosHal;

// -----------------------------------------------------------------------------
// B.7 Initialization
// -----------------------------------------------------------------------------

// Initialize HAL subsystem
mos_result_t mos_hal_init(void);

// Initialize display from boot info
mos_result_t mos_hal_init_display(const MosBootInfo* boot);

// Initialize input devices
mos_result_t mos_hal_init_input(void);

// -----------------------------------------------------------------------------
// B.8 Display Operations
// -----------------------------------------------------------------------------

// Get display info
const MosDisplay* mos_hal_display(uint8_t id);

// Get primary display
const MosDisplay* mos_hal_primary_display(void);

// Set video mode
mos_result_t mos_hal_set_mode(uint8_t display, const MosVideoMode* mode);

// Get framebuffer address
mos_vaddr_t mos_hal_framebuffer(uint8_t display);

// Swap buffers (if double buffered)
mos_result_t mos_hal_swap_buffers(uint8_t display);

// Wait for vsync
mos_result_t mos_hal_vsync(uint8_t display);

// -----------------------------------------------------------------------------
// B.9 Input Operations
// -----------------------------------------------------------------------------

// Poll events (non-blocking)
bool mos_hal_poll_event(MosEvent* event);

// Wait for event (blocking)
mos_result_t mos_hal_wait_event(MosEvent* event);

// Push event to queue
mos_result_t mos_hal_push_event(const MosEvent* event);

// Get current key state
trit_t mos_hal_key_state(uint8_t keycode);

// Get current mouse position
void mos_hal_mouse_position(int32_t* x, int32_t* y);

// Get current mouse buttons
trit5_t mos_hal_mouse_buttons(void);

// Get controller state
mos_result_t mos_hal_controller_state(uint8_t id, MosControllerState* state);

// -----------------------------------------------------------------------------
// B.10 Low-Level Hardware Access
// -----------------------------------------------------------------------------
//
// Port I/O primitives for x86. These are also defined in boot/stage0/serial.h
// for early boot use. The guard ensures only one definition is compiled.
//

#ifndef MOS_PORT_IO_DEFINED
#define MOS_PORT_IO_DEFINED

// Write byte to I/O port
static inline void mos_outb(uint16_t port, uint8_t val) {
    __asm__ volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

// Read byte from I/O port
static inline uint8_t mos_inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// I/O delay (write to unused port 0x80)
static inline void mos_io_wait(void) {
    mos_outb(0x80, 0);
}

#endif // MOS_PORT_IO_DEFINED

// 16-bit and 32-bit port I/O (kernel-only, not in serial.h)
static inline void mos_outw(uint16_t port, uint16_t val) {
    __asm__ volatile("outw %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint16_t mos_inw(uint16_t port) {
    uint16_t ret;
    __asm__ volatile("inw %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void mos_outl(uint16_t port, uint32_t val) {
    __asm__ volatile("outl %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint32_t mos_inl(uint16_t port) {
    uint32_t ret;
    __asm__ volatile("inl %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// -----------------------------------------------------------------------------
// B.11 Information
// -----------------------------------------------------------------------------

// Get HAL state
const MosHal* mos_hal_info(void);

// Get HAL health
mos_health_t mos_hal_health(void);

// -----------------------------------------------------------------------------
// B.12 Timing Functions
// -----------------------------------------------------------------------------

// Get current tick count in milliseconds (since boot)
uint64_t mos_hal_ticks_ms(void);

// Get current tick count in microseconds (since boot)
uint64_t mos_hal_ticks_us(void);

// Sleep for specified milliseconds (yields to scheduler)
void mos_hal_sleep(uint32_t milliseconds);

// Sleep for specified microseconds (busy-wait for short delays)
void mos_hal_sleep_us(uint32_t microseconds);

// Get high-resolution timer frequency
uint64_t mos_hal_timer_frequency(void);

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================
//
// HAL provides hardware abstraction:
//   - Display management (framebuffer, video modes)
//   - Input handling (keyboard, mouse, controllers)
//   - Event queue with ternary timestamps
//   - Low-level port I/O for drivers
//
// "And God said, Let there be a firmament in the midst of the waters,
//  and let it divide the waters from the waters."
// — Genesis 1:6
//
// HAL is the firmament dividing hardware from software,
// allowing the OS to work on any compatible platform.
//
// =============================================================================
// END CLOSING
// =============================================================================

#endif // MILLENNIUMOS_HAL_H
