// #!omni code --c -library
// #!omni meta.key = B-word-work-pkg-omni-menu-transpiler
// #!omni meta.from = word/seed/code/c/header.h
// #!omni meta.at = a-01.00
//
// omni_menu_transpiler.h — OmniCode Menu DSL to JSONC Transpiler
// Parses sentence-based menu definitions and outputs JSONC.

#ifndef BERESHIT_OMNI_MENU_TRANSPILER_H
#define BERESHIT_OMNI_MENU_TRANSPILER_H

// =============================================================================
// METADATA [METADATA]
// =============================================================================
//
// Key: B-word-work-pkg-omni-menu-transpiler
// Title: OmniCode Menu DSL to JSONC Transpiler
// Type: Header
// Role: Parse OmniCode menu files, output JSONC for cornerstone menu system
//
// Scripture: Proverbs 4:26 — Ponder the path of thy feet
// Principle: Clear transformations lead to correct destinations
//
// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include <stdbool.h>
#include <stddef.h>

// # S.1 Defines [DEFINES]

// # S.1a String Size Limits (by semantic category)
#define MENU_KEY_MAX      16    // Keys/symbols: "w", "up", "escape", "> "
#define MENU_NAME_MAX     32    // Short names: type, colorscheme, box_style
#define MENU_ID_MAX       64    // Identifiers: menu_id, item_id, targets
#define MENU_LABEL_MAX    128   // User-facing: labels, conditions
#define MENU_DESC_MAX     256   // Descriptions

// # S.1b Array Count Limits
#define MENU_BINDINGS_MAX 8     // Key bindings per direction
#define MENU_OPTIONS_MAX  8     // Options per adjust action
#define MENU_ITEMS_MAX    32    // Items per menu
#define MENU_RULES_MAX    16    // Hebrew rules per menu
#define MENU_MENUS_MAX    32    // Menus per file

// # S.2 Types [TYPES]

// # S.2a Error Codes [ERRORS]

typedef enum MenuTranspileError {
    MENU_TRANSPILE_OK = 0,
    MENU_TRANSPILE_ERR_NULL,
    MENU_TRANSPILE_ERR_FILE,
    MENU_TRANSPILE_ERR_PARSE,
    MENU_TRANSPILE_ERR_SYNTAX,
    MENU_TRANSPILE_ERR_MEMORY,
    MENU_TRANSPILE_ERR_WRITE,
} MenuTranspileError;

// # S.2b Action Types [ACTIONS]

typedef enum MenuActionType {
    MENU_ACT_NONE = 0,
    MENU_ACT_NAVIGATE,
    MENU_ACT_EXECUTE,
    MENU_ACT_TOGGLE,
    MENU_ACT_ADJUST,
    MENU_ACT_BACK,
    MENU_ACT_QUIT,
} MenuActionType;

// # S.2c Parsed Structures [PARSED]

typedef struct {
    MenuActionType type;
    char           target[MENU_ID_MAX];
    char           options[MENU_OPTIONS_MAX][MENU_LABEL_MAX];
    int            option_count;
} ParsedMenuAction;

typedef struct {
    char             item_id[MENU_ID_MAX];
    char             label[MENU_LABEL_MAX];
    char             description[MENU_DESC_MAX];
    ParsedMenuAction action;
    char             hotkey[8];
    char             visible_when[MENU_LABEL_MAX];
    char             enabled_when[MENU_LABEL_MAX];
    char             selected_when[MENU_LABEL_MAX];
} ParsedMenuItem;

typedef struct {
    char default_item[MENU_ID_MAX];
    bool wrap;
    bool remember_position;
    char parent_menu[MENU_ID_MAX];
} ParsedMenuNav;

typedef struct {
    char up[MENU_BINDINGS_MAX][MENU_KEY_MAX];
    int  up_count;
    char down[MENU_BINDINGS_MAX][MENU_KEY_MAX];
    int  down_count;
    char left[MENU_BINDINGS_MAX][MENU_KEY_MAX];
    int  left_count;
    char right[MENU_BINDINGS_MAX][MENU_KEY_MAX];
    int  right_count;
    char select[MENU_BINDINGS_MAX][MENU_KEY_MAX];
    int  select_count;
    char back[MENU_BINDINGS_MAX][MENU_KEY_MAX];
    int  back_count;
    char cancel[MENU_BINDINGS_MAX][MENU_KEY_MAX];
    int  cancel_count;
} ParsedMenuBindings;

typedef struct {
    int  width;
    bool show_border;
    bool show_hotkeys;
    char cursor[MENU_KEY_MAX];
} ParsedCLIHints;

typedef struct {
    char colorscheme[MENU_NAME_MAX];
    char box_style[MENU_KEY_MAX];
} ParsedTUIHints;

typedef struct {
    char layout[MENU_NAME_MAX];
    char animation[MENU_NAME_MAX];
} ParsedGUIHints;

typedef struct {
    ParsedCLIHints cli;
    ParsedTUIHints tui;
    ParsedGUIHints gui;
} ParsedDisplayHints;

typedef struct {
    char  state[MENU_KEY_MAX];  // shavar, chaser, yashar, shalem, tov
    char  hide_items[MENU_ITEMS_MAX][MENU_ID_MAX];
    int   hide_count;
    char  enable_items[MENU_ITEMS_MAX][MENU_ID_MAX];
    int   enable_count;
    bool  simplify_navigation;
    bool  reduce_options;
    bool  show_all_options;
} ParsedHebrewRule;

typedef struct {
    char               menu_id[MENU_ID_MAX];
    char               title[MENU_LABEL_MAX];
    char               type[32];          // main, pause, settings, dialog, submenu
    char               description[MENU_DESC_MAX];

    ParsedMenuItem     items[MENU_ITEMS_MAX];
    int                item_count;

    ParsedMenuNav      nav;
    ParsedMenuBindings bindings;
    ParsedDisplayHints display;

    ParsedHebrewRule   hebrew_rules[MENU_RULES_MAX];
    int                rule_count;
} ParsedMenu;

typedef struct {
    ParsedMenu menus[MENU_MENUS_MAX];
    int        menu_count;

    // Tripwire behavior
    char       on_menu_not_found[MENU_NAME_MAX];
    char       on_render_failure[MENU_NAME_MAX];
    int        health_penalty;
} ParsedMenuFile;

// # S.3 Function Prototypes [PROTOTYPES]

// # S.3a Lifecycle [LIFECYCLE]

// Create/destroy parsed menu file structure
ParsedMenuFile* menu_parsed_create(void);
void            menu_parsed_destroy(ParsedMenuFile* pmf);

// # S.3b Parse [PARSE]

// Parse OmniCode menu file into structure
MenuTranspileError menu_parse_file(const char* input_path, ParsedMenuFile* out);

// Parse OmniCode menu from string
MenuTranspileError menu_parse_string(const char* source, ParsedMenuFile* out);

// # S.3c Transpile [TRANSPILE]

// Transpile parsed menu to JSONC file
MenuTranspileError menu_transpile_to_file(const ParsedMenuFile* pmf, const char* output_path);

// Transpile parsed menu to JSONC string (caller must free)
MenuTranspileError menu_transpile_to_string(const ParsedMenuFile* pmf, char** out_jsonc);

// # S.3d Convenience [CONVENIENCE]

// One-shot: parse and transpile file to file
MenuTranspileError menu_omni_to_jsonc(const char* input_path, const char* output_path);

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================
//
// Organization:
//   Types: 8 structs (parsed structures), 2 enums (error, action)
//   Functions: 2 lifecycle, 2 parse, 2 transpile, 1 convenience
//
// Flow:
//   menu_parse_file() → ParsedMenuFile → menu_transpile_to_file() → JSONC
//
// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================
//
// "Ponder the path of thy feet, and let all thy ways be established."
//   — Proverbs 4:26
//
// The transpiler transforms sentence-based OmniCode menus into
// structured JSONC that the Cornerstone engine consumes.
//
// =============================================================================
// END CLOSING
// =============================================================================

#endif // BERESHIT_OMNI_MENU_TRANSPILER_H
