# Scene Stack & StartupService Implementation Plan

## Summary

Add StartupService for engine splash, fix scene stack flow, and improve Game Selector display.

---

## Tasks

| # | Task | Files |
|---|------|-------|
| 1 | Create StartupService | `startup_service.h`, `startup_service.c` (new) |
| 2 | Add SCENE_STARTUP | `scene.h`, `scene.c` |
| 3 | Wire startup flow | `cornerstone.c` |
| 4 | Fix Game Selector | `title_service.c` |
| 5 | Verify flow | Build & test |

---

## Task 1: Create StartupService

### 1.1 Header: `cornerstone/engine/include/framework/service/startup_service.h`

```c
// Follow launcher_service.h pattern
#define STARTUP_EVENT_COMPLETE "STARTUP_COMPLETE"

typedef enum {
    STARTUP_PHASE_LOGO = 0,
    STARTUP_PHASE_FADE
} StartupPhase;

Service* startup_service_create(ServiceOrchestrator* orchestrator);
void startup_service_destroy(Service* svc);
StartupPhase startup_service_get_phase(Service* svc);
void startup_service_skip(Service* svc);

// Callbacks
void startup_svc_on_activate(Service* svc);
void startup_svc_on_suspend(Service* svc);
void startup_svc_on_resume(Service* svc);
void startup_svc_on_terminate(Service* svc);
void startup_svc_update(Service* svc, float dt);
void startup_svc_render(Service* svc);
```

### 1.2 Implementation: `cornerstone/engine/src/framework/service/startup_service.c`

**State Structure:**
```c
typedef struct StartupServiceState {
    ServiceOrchestrator* orchestrator;
    StartupPhase phase;
    float timer;
    bool active;
    BlockFont font;
    bool font_loaded;
    Texture logo;
    bool logo_loaded;
} StartupServiceState;

#define STARTUP_SPLASH_DURATION 2.5f
```

**Key Logic:**
- `on_activate`: Load assets (logo, font), reset timer
- `update`: Increment timer, skip on key press, transition to SCENE_LAUNCHER after duration
- `render`: Draw logo centered, "CreativeWorkzStudio LLC" below
- `on_terminate`: Cleanup

---

## Task 2: Add SCENE_STARTUP to Scene System

### 2.1 Modify `scene.h` (line 259-260)

```c
// Layer 1: Entry (exclusive)
SCENE_STARTUP,        // <<< ADD - Startup splash (CreativeWorkzStudio LLC)
SCENE_LAUNCHER,       // Launcher (mode selector: Engine/Editor/IDE)
```

### 2.2 Update `scene_set_ui_services()` signature

```c
void scene_set_ui_services(Service* startup, Service* launcher, Service* title,
                           Service* loading, Service* game_menu, Service* modal,
                           Service* choice, Service* pause, Service* error,
                           Service* gameplay);
```

### 2.3 Modify `scene.c`

**Add static variable** (~line 757):
```c
static Service* s_startup_service = NULL;
```

**Add callback prototypes** (~line 547):
```c
static void startup_init(void);
static void startup_update(float dt);
static void startup_render(void);
static void startup_destroy(void);
```

**Add NO-OP implementations** (delegate to service):
```c
static void startup_init(void) {
    LOG_INFO("scene", "startup_init: Delegating to StartupService");
}
static void startup_update(float dt) { UNUSED(dt); }
static void startup_render(void) { }
static void startup_destroy(void) { }
```

**Register in `scene_init()`** (~line 2225):
```c
scenes[SCENE_STARTUP].init    = startup_init;
scenes[SCENE_STARTUP].update  = startup_update;
scenes[SCENE_STARTUP].render  = startup_render;
scenes[SCENE_STARTUP].destroy = startup_destroy;
```

**Update `scene_set_ui_services()`** (~line 2339):
```c
void scene_set_ui_services(Service* startup, Service* launcher, ...) {
    s_startup_service = startup;
    s_launcher_service = launcher;
    // ... rest
}
```

**Add activation in `scene_change()`** (~line 2611):
```c
else if (type == SCENE_STARTUP && s_startup_service != NULL) {
    orchestrator_push(s_orchestrator, s_startup_service);
    LOG_INFO("scene", "Activated StartupService for scene_change");
}
```

---

## Task 3: Wire in cornerstone.c

### 3.1 Add global variable (~line 951)
```c
static Service* g_startup_service = NULL;
```

### 3.2 Create service (~line 3642)
```c
g_startup_service = startup_service_create(&g_orchestrator);
```

### 3.3 Register with orchestrator (~line 3684)
```c
if (g_startup_service) orchestrator_register(&g_orchestrator, g_startup_service);
```

### 3.4 Update scene_set_ui_services() call (~line 3696)
```c
scene_set_ui_services(g_startup_service, g_launcher_service, g_title_service,
                      g_loading_service, g_game_menu_service, g_modal_service,
                      g_choice_service, g_pause_service, g_error_service,
                      g_gameplay_service);
```

### 3.5 Change startup flow (~line 3729)
```c
if (g_config.display_mode == DISPLAY_GUI && g_startup_service != NULL) {
    scene_change(SCENE_STARTUP);  // Start with splash
} else if (g_config.show_launcher) {
    scene_change(SCENE_LAUNCHER);
} else {
    scene_change(SCENE_TITLE);
}
```

### 3.6 Add destroy (~line 3816)
```c
if (g_startup_service != NULL) {
    startup_service_destroy(g_startup_service);
    g_startup_service = NULL;
}
```

---

## Task 4: Fix Game Selector Display

### Issue from Screenshot
The title screen shows "ECHO'S BECOMING a-01.60" - games ARE being found. But we need:
1. Better spacing in "NO GAMES FOUND" fallback
2. Possibly a "Play Demo" option even when games exist

### Changes to `title_service.c` (lines 469-481)

**Increase spacing for "NO GAMES FOUND" state:**
```c
} else {
    /* No games found - show Play Demo option */
    tw = blockfont_measure_text(&state->font, "NO GAMES FOUND", studio_cell);
    blockfont_draw_text(&state->font, "NO GAMES FOUND", (vp_width - tw) / 2,
                        center_y, studio_cell, dim);

    // Extra blank line spacing
    tw = blockfont_measure_text(&state->font, "", studio_cell);

    tw = blockfont_measure_text(&state->font, "> PLAY DEMO <", studio_cell);
    blockfont_draw_text(&state->font, "> PLAY DEMO <", (vp_width - tw) / 2,
                        center_y + line_spacing * 3, studio_cell, green);  // Was * 2

    tw = blockfont_measure_text(&state->font, "[Press ENTER to start demo]", desc_cell);
    blockfont_draw_text(&state->font, "[Press ENTER to start demo]", (vp_width - tw) / 2,
                        center_y + line_spacing * 5, desc_cell, dim);  // Was * 3
}
```

---

## Task 5: Verification

### Expected Flow After Changes
```
SCENE_STARTUP (splash: logo + "CreativeWorkzStudio LLC", 2.5s)
  ↓ (auto or key press)
SCENE_LAUNCHER (Engine / Editor / IDE)
  ↓ (Engine selected)
SCENE_TITLE (Game Selector: games list or "NO GAMES FOUND" + Play Demo)
  ↓ (game/demo selected)
SCENE_LOADING → SCENE_GAME_MENU → Gameplay → SCENE_DEMO_COMPLETE
```

### Build & Test
```bash
cd /media/seanje-lenox-wise/Project/Bereshit/cornerstone
make clean && make
./bin/cornerstone --display-mode=gui
```

### Verify:
1. ✓ Startup splash appears with logo
2. ✓ Auto-transitions to Launcher after 2.5s (or key skip)
3. ✓ Launcher shows Engine/Editor/IDE selection
4. ✓ Title screen shows game list or "NO GAMES FOUND" with proper spacing
5. ✓ Play Demo option works
6. ✓ Full game flow completes to DEMO_COMPLETE

---

## Files Modified

| File | Changes |
|------|---------|
| `engine/include/framework/service/startup_service.h` | **NEW** - Service header |
| `engine/src/framework/service/startup_service.c` | **NEW** - Service implementation |
| `engine/include/framework/scene.h` | Add SCENE_STARTUP enum, update signature |
| `engine/src/framework/scene.c` | Register callbacks, wire service activation |
| `engine/cornerstone.c` | Create/register/inject service, change startup |
| `engine/src/framework/service/title_service.c` | Fix NO GAMES FOUND spacing |
