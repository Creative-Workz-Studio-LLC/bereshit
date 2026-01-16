#ifndef BERESHIT_TRIT_CONFIG_H
#define BERESHIT_TRIT_CONFIG_H

// =============================================================================
// METADATA [METADATA]
// =============================================================================
//
// 4-Block Code Structure: Identity and context for this component
//
// =============================================================================

// # M.1 Core Identity [IDENTITY]
//
// Key: B-word-work-pkg-trit-config
// Title: Typed Configuration Structures
// Type: Header
// Component: Ladder
// Role: Declares typed C structs that map to TOML specifications

// # M.2 Lifecycle State [STATE]
//
// Status: Active
// Version: a-03.00
// Created: 2026-01-13
// Updated: 2026-01-13

// # M.3 Attribution [ATTRIBUTION]
//
// Authors: Seanje Lenox-Wise (Architect), Nova Dawn (Author)
// Organization: CreativeWorkzStudio LLC
// Copyright: (c) 2026 CreativeWorkzStudio LLC. All rights reserved.

// # M.4 Location [LOCATION]
//
// Path: word/work/pkg/trit/include/config.h

// # M.5 Derivation [DERIVATION]
//
// Derives_from: word/seed/code/c/header.h

// # M.6 Classification [CLASSIFICATION]
//
// Tags: config, types, toml, c

// # M.7 Intent [INTENT]
//
// Purpose: Typed access to TOML configuration specifications
// Core Design: Structs mirror TOML structure for direct mapping
// Philosophy: Specifications drive code through typed configuration
//
// Key Features:
//   - TritConfig for ternary.toml values
//   - PrimitiveConfig for primitives.toml values
//   - Config aggregates all specs into single loadable structure

// # M.8 Grounding [GROUNDING]
//
// Scripture: Proverbs 24:27 - Prepare thy work without
// Principle: Configuration defines before implementation executes
// Anchor: Exodus 25:40 - Make after the pattern shown

// # M.9 Dependencies [DEPENDENCIES]
//
// What This Needs:
//   Standard Library: stddef.h, stdint.h, stdbool.h
//   Internal: toml.h (TOML parser)
//
// What Uses This:
//   Libraries: libtrit components
//   Commands: demo-config-reader

// # M.10 Roadmap [ROADMAP]
//
// Planned:
//   - Add remaining spec sections as needed
//   - Bible addressing structs
//   - Health scoring structs

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

// # S.1 Includes [INCLUDES]

// # S.1a Standard Library [STDLIB]

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// # S.1b Project Headers [PROJECT]

// [Reserved: toml.h included in config.c, not exposed in header]

// # S.1c External Libraries [EXTERNAL]

// [Reserved: Currently none - uses standard library only]

// # S.2 Defines [DEFINES]

// # S.2a Sizes [SIZES]

#define CONFIG_PATH_MAX      256   // Max path length for config files
#define CONFIG_LABEL_MAX     32    // Max label string length
#define CONFIG_DESC_MAX      256   // Max description string length

// # S.2b Limits [LIMITS]

#define TRIT_VALUES_COUNT    3     // -1, 0, +1
#define TRIT5_POWERS_COUNT   5     // Powers for 5-trit packing
#define TRIT9_POWERS_COUNT   9     // Powers for 9-trit packing
#define TRIT27_POWERS_COUNT  27    // Powers for 27-trit (Word size)
#define TEMPORAL_BASE_COUNT  3     // past, present, future
#define TEMPORAL_COMPOUND_COUNT 6  // compound temporal states
#define TEMPORAL_TOTAL_COUNT 9     // base + compound

// OmniCode limits
#define OMNI_BLOCK_COUNT     5     // Max blocks in any structure (5-block)
#define OMNI_MARKER_COUNT    8     // Max markers per block type
#define OMNI_SECTION_COUNT   8     // Max sections per block
#define OMNI_ERROR_COUNT     32    // Max error definitions per category
#define OMNI_TEMPLATE_COUNT  16    // Max templates per category

// Bible limits
#define BIBLE_BOOK_COUNT     66    // Books in the Bible
#define BIBLE_KJV_VERSES     31102 // KJV canonical verses
#define BIBLE_WEB_VARIANTS   13    // WEB-only verses (trite 243-255)
#define BIBLE_HEBREW_ROOTS   16    // Hebrew roots in encoding
#define BIBLE_GREEK_TERMS    8     // Greek terms in encoding
#define BIBLE_PATTERNS       16    // Biblical patterns per category

// Display limits
#define DISPLAY_COLOR_COUNT       17   // DisplayColor enum values (0-16)
#define DISPLAY_THEME_COUNT       8    // Max themes in config
#define DISPLAY_STYLE_COUNT       32   // Max named styles
#define DISPLAY_ICON_SET_COUNT    8    // Max icon sets
#define DISPLAY_KEY_BINDING_COUNT 64   // Max keybindings per context
#define DISPLAY_MENU_ITEM_COUNT   16   // Max items per submenu

// # S.2c Flags [FLAGS]

// [Reserved: No flags currently needed]

// # S.2d Macros [MACROS]

// [Reserved: No macros currently needed]

// # S.3 Types [TYPES]

// # S.3a Enumerations [ENUMS]

// ConfigError codes for config loading operations.
typedef enum {
    CONFIG_OK = 0,
    CONFIG_ERR_NULL,
    CONFIG_ERR_FILE,
    CONFIG_ERR_PARSE,
    CONFIG_ERR_NOT_FOUND,
    CONFIG_ERR_MEMORY,
} ConfigError;

// # S.3b Structs [STRUCTS]

// ---------------------------------------------------------------------------
// Ternary Math Configuration (from ternary.toml)
// ---------------------------------------------------------------------------

// TritCore holds the fundamental trit values and labels.
//
// Fields:
//   - values: The three trit values (-1, 0, +1)
//   - labels: Human-readable names (negative, neutral, positive)
//   - short_labels: Single-char labels (N, Z, P)
//   - states: Number of states (3)
//   - base: Numeric base (3)
//   - identity_sum: Sum of values (0)
typedef struct {
    int8_t  values[TRIT_VALUES_COUNT];
    char    labels[TRIT_VALUES_COUNT][CONFIG_LABEL_MAX];
    char    short_labels[TRIT_VALUES_COUNT][4];
    int32_t states;
    int32_t base;
    int32_t identity_sum;
} TritCore;

// PackingConfig holds trit-byte packing constants.
//
// Fields:
//   - trits_per_byte: Trits that fit in one byte (5)
//   - max_trit_value: 3^5 = 243
//   - byte_capacity: 2^8 = 256
//   - gap: 256 - 243 = 13 (used for WEB variants)
//   - trit5_powers: Powers of 3 for 5-trit packing
//   - trit9_powers: Powers of 3 for 9-trit packing
typedef struct {
    int32_t trits_per_byte;
    int32_t max_trit_value;
    int32_t byte_capacity;
    int32_t gap;
    int64_t trit5_powers[TRIT5_POWERS_COUNT];
    int64_t trit9_powers[TRIT9_POWERS_COUNT];
} PackingConfig;

// ArithmeticConfig holds lookup tables for trit operations.
//
// Fields:
//   - negation: Trit negation lookup
//   - addition_no_carry: Simple addition without carry
//   - multiplication: Trit multiplication lookup
typedef struct {
    int8_t negation[TRIT_VALUES_COUNT];
    int8_t addition_no_carry[TRIT_VALUES_COUNT][TRIT_VALUES_COUNT];
    int8_t multiplication[TRIT_VALUES_COUNT][TRIT_VALUES_COUNT];
} ArithmeticConfig;

// HalfAdderConfig holds half-adder lookup tables.
//
// Fields:
//   - sum: Result of (a + b) mod 3
//   - carry: Carry from (a + b)
typedef struct {
    int8_t sum[TRIT_VALUES_COUNT][TRIT_VALUES_COUNT];
    int8_t carry[TRIT_VALUES_COUNT][TRIT_VALUES_COUNT];
} HalfAdderConfig;

// FullAdderConfig holds full-adder lookup tables.
//
// Fields:
//   - sum: Result of (a + b + c_in) mod 3
//   - carry_out: Carry from (a + b + c_in)
typedef struct {
    int8_t sum[TRIT_VALUES_COUNT][TRIT_VALUES_COUNT][TRIT_VALUES_COUNT];
    int8_t carry_out[TRIT_VALUES_COUNT][TRIT_VALUES_COUNT][TRIT_VALUES_COUNT];
} FullAdderConfig;

// BuildingBlockConfig holds CPI-SI reasoning method mapping.
//
// Fields:
//   - operations: break_down, anchor, build_up
//   - equation: The balanced equation (-1 + 0 + 1 = 0)
typedef struct {
    char operations[TRIT_VALUES_COUNT][CONFIG_LABEL_MAX];
    char equation[CONFIG_LABEL_MAX];
} BuildingBlockConfig;

// DimensionConfig holds a single dimension's properties.
//
// Fields:
//   - trit: Dimension's trit value
//   - name: Human-readable name
//   - question: The question this dimension answers
//   - building_block: Associated operation
typedef struct {
    int8_t trit;
    char   name[CONFIG_LABEL_MAX];
    char   question[CONFIG_LABEL_MAX];
    char   building_block[CONFIG_LABEL_MAX];
} DimensionConfig;

// DimensionsConfig holds all three dimension configurations.
//
// Fields:
//   - temporal: TIME dimension (0)
//   - spatial: SPACE dimension (+1)
//   - material: MATTER dimension (-1)
typedef struct {
    DimensionConfig temporal;
    DimensionConfig spatial;
    DimensionConfig material;
} DimensionsConfig;

// TemporalBaseState holds a base temporal state.
//
// Fields:
//   - trit: State's trit value
//   - meaning: What this state represents
typedef struct {
    int8_t trit;
    char   meaning[CONFIG_LABEL_MAX];
} TemporalBaseState;

// TemporalCompoundState holds a compound temporal state.
//
// Fields:
//   - lens: Viewing FROM position
//   - target: Viewing AT position
//   - description: State description
typedef struct {
    char lens[CONFIG_LABEL_MAX];
    char target[CONFIG_LABEL_MAX];
    char description[CONFIG_DESC_MAX];
} TemporalCompoundState;

// TemporalStatesConfig holds the 9-state temporal model.
//
// Fields:
//   - past, present, future: Base states
//   - memory, prophecy, reflection, planning, hindsight, preparation: Compound states
//   - base_count, compound_count, total_modes: Counts
typedef struct {
    TemporalBaseState     past;
    TemporalBaseState     present;
    TemporalBaseState     future;

    TemporalCompoundState memory;
    TemporalCompoundState prophecy;
    TemporalCompoundState reflection;
    TemporalCompoundState planning;
    TemporalCompoundState hindsight;
    TemporalCompoundState preparation;

    int32_t base_count;
    int32_t compound_count;
    int32_t total_modes;
} TemporalStatesConfig;

// TernaryMathConfig aggregates all ternary.toml sections.
//
// Fields:
//   - trit: Core trit values and properties
//   - packing: Trit-byte packing constants
//   - arithmetic: Basic arithmetic tables
//   - half_adder: Half-adder lookup
//   - full_adder: Full-adder lookup
//   - building_block: CPI-SI method mapping
//   - dimensions: Cognitive navigation framework
//   - temporal_states: 9-state temporal model
typedef struct {
    TritCore            trit;
    PackingConfig       packing;
    ArithmeticConfig    arithmetic;
    HalfAdderConfig     half_adder;
    FullAdderConfig     full_adder;
    BuildingBlockConfig building_block;
    DimensionsConfig    dimensions;
    TemporalStatesConfig temporal_states;
} TernaryMathConfig;

// ---------------------------------------------------------------------------
// Primitive Types Configuration (from primitives.toml)
// ---------------------------------------------------------------------------

// PrimitiveTypeConfig holds a single primitive type definition.
//
// Fields:
//   - name: Type name (trit, int9, etc.)
//   - size_trits: Size in trits (or -1 for variable)
//   - min_value: Minimum value
//   - max_value: Maximum value
//   - total_states: Total possible states
//   - is_signed: Whether type is signed
//   - is_nullable: Whether type can be null
typedef struct {
    char    name[CONFIG_LABEL_MAX];
    int32_t size_trits;
    int64_t min_value;
    int64_t max_value;
    int64_t total_states;
    bool    is_signed;
    bool    is_nullable;
} PrimitiveTypeConfig;

// PrimitivesConfig holds all primitive type definitions.
//
// Fields:
//   - trit: Single trit type
//   - trit5: 5-trit packed type
//   - int9: 9-trit signed integer
//   - uint9: 9-trit unsigned integer
//   - int27: 27-trit signed integer (Word size)
//   - uint27: 27-trit unsigned integer
//   - bool3: Ternary boolean
typedef struct {
    PrimitiveTypeConfig trit;
    PrimitiveTypeConfig trit5;
    PrimitiveTypeConfig int9;
    PrimitiveTypeConfig uint9;
    PrimitiveTypeConfig int27;
    PrimitiveTypeConfig uint27;
    PrimitiveTypeConfig bool3;
} PrimitivesConfig;

// ---------------------------------------------------------------------------
// OmniCode Block Configuration (from blocks.toml)
// ---------------------------------------------------------------------------

// OmniBlockSpec defines a single block's requirements.
typedef struct {
    char name[CONFIG_LABEL_MAX];              // Block name (METADATA, SETUP, etc.)
    int  position;                            // Position in structure (1-5)
    bool required;                            // Is this block required?
    char purpose[CONFIG_DESC_MAX];            // What this block is for
    char required_sections[OMNI_SECTION_COUNT][CONFIG_LABEL_MAX];
    int  required_section_count;
    char optional_sections[OMNI_SECTION_COUNT][CONFIG_LABEL_MAX];
    int  optional_section_count;
} OmniBlockSpec;

// OmniBlockStructure defines a complete block structure (3/4/5-block).
typedef struct {
    char name[CONFIG_LABEL_MAX];              // "3-Block", "4-Block", "5-Block"
    char use_case[CONFIG_DESC_MAX];           // What this structure is for
    int  block_count;                         // 3, 4, or 5
    char order[OMNI_BLOCK_COUNT][CONFIG_LABEL_MAX];  // Block order
    OmniBlockSpec blocks[OMNI_BLOCK_COUNT];   // Block specifications
} OmniBlockStructure;

// OmniBlocksConfig holds all block structure definitions.
typedef struct {
    OmniBlockStructure three;                 // 3-block (data files)
    OmniBlockStructure four;                  // 4-block (code files)
    OmniBlockStructure five;                  // 5-block (documentation)
    bool               loaded;
} OmniBlocksConfig;

// ---------------------------------------------------------------------------
// OmniCode Error Configuration (from errors.toml)
// ---------------------------------------------------------------------------

// OmniErrorDef defines a single error message.
typedef struct {
    char code[16];                            // Error code (E001, S001, L001)
    int  severity;                            // -1=error, 0=warning, +1=info
    char message[CONFIG_DESC_MAX];            // Short message
    char detail[CONFIG_DESC_MAX];             // Detailed explanation
    char recovery[CONFIG_LABEL_MAX];          // Recovery action name
} OmniErrorDef;

// OmniErrorCategory holds errors for one category.
typedef struct {
    OmniErrorDef errors[OMNI_ERROR_COUNT];
    int          count;
} OmniErrorCategory;

// OmniErrorsConfig holds all error definitions.
typedef struct {
    OmniErrorCategory structure;              // Block-level errors
    OmniErrorCategory semantic;               // Chunk-level errors
    OmniErrorCategory lexical;                // Token-level errors
    bool              loaded;
} OmniErrorsConfig;

// ---------------------------------------------------------------------------
// OmniCode Recovery Configuration (from recovery.toml)
// ---------------------------------------------------------------------------

// OmniRecoveryTemplate defines a single recovery template.
typedef struct {
    char name[CONFIG_LABEL_MAX];              // Template name
    char template_text[1024];                 // Template content
} OmniRecoveryTemplate;

// OmniRecoveryConfig holds recovery templates and actions.
typedef struct {
    OmniRecoveryTemplate pragma[OMNI_TEMPLATE_COUNT];
    int                  pragma_count;
    OmniRecoveryTemplate blocks[OMNI_TEMPLATE_COUNT];
    int                  blocks_count;
    OmniRecoveryTemplate sections[OMNI_TEMPLATE_COUNT];
    int                  sections_count;
    bool                 continue_after_error;
    int                  max_errors;
    bool                 loaded;
} OmniRecoveryConfig;

// ---------------------------------------------------------------------------
// OmniCode Unified Configuration
// ---------------------------------------------------------------------------

// OmniConfig aggregates all OmniCode configuration.
typedef struct {
    OmniBlocksConfig   blocks;                // Block structure specs
    OmniErrorsConfig   errors;                // Error messages
    OmniRecoveryConfig recovery;              // Recovery templates
    bool               loaded;
} OmniConfig;

// ---------------------------------------------------------------------------
// Bible Addressing Configuration (from addressing.toml)
// ---------------------------------------------------------------------------

// BibleVersionConfig holds version-specific addressing info.
typedef struct {
    char    name[CONFIG_LABEL_MAX];           // "King James Version"
    char    role[CONFIG_LABEL_MAX];           // "encoding" or "decoding"
    int32_t total_verses;                     // 31102 (KJV) or 31115 (WEB)
    int32_t ordinal_min;                      // 1
    int32_t ordinal_max;                      // 31102
    int32_t variant_min;                      // 243 (WEB only)
    int32_t variant_max;                      // 255 (WEB only)
    int32_t books;                            // 66
} BibleVersionConfig;

// BibleBookConfig holds a single book's addressing.
typedef struct {
    char    name[CONFIG_LABEL_MAX];           // "Genesis"
    char    abbreviation[16];                 // "Gen"
    int32_t ordinal;                          // Book number (1-66)
    int32_t chapters;                         // Number of chapters
    int32_t verses;                           // Total verses in book
    int32_t range_start;                      // First ordinal
    int32_t range_end;                        // Last ordinal
} BibleBookConfig;

// BibleTernaryConfig holds ternary encoding rules.
typedef struct {
    int32_t trits_needed;                     // 10
    int32_t max_ordinal;                      // 31102
    int32_t max_ternary_value;                // 59049 (3^10)
    int32_t trit5_states;                     // 243
    int32_t trit5_bits;                       // 8
    int32_t waste_states;                     // 13 (used for WEB variants)
} BibleTernaryConfig;

// BibleVariantConfig holds a WEB-only verse.
typedef struct {
    int32_t trite;                            // 243-255
    char    book[CONFIG_LABEL_MAX];           // Book name
    int32_t chapter;                          // Chapter number
    int32_t verse;                            // Verse number
    char    summary[CONFIG_DESC_MAX];         // Brief description
} BibleVariantConfig;

// BibleAddressingConfig aggregates addressing.toml sections.
typedef struct {
    BibleVersionConfig  kjv;                  // KJV address space
    BibleVersionConfig  web;                  // WEB address space
    BibleBookConfig     books[BIBLE_BOOK_COUNT];
    int32_t             book_count;
    BibleTernaryConfig  ternary;              // Encoding rules
    BibleVariantConfig  variants[BIBLE_WEB_VARIANTS];
    int32_t             variant_count;
    bool                loaded;
} BibleAddressingConfig;

// ---------------------------------------------------------------------------
// Bible Encoding Configuration (from encoding.toml)
// ---------------------------------------------------------------------------

// HebrewRootConfig holds a Hebrew trilateral root mapping.
typedef struct {
    char root[16];                            // Hebrew letters "ברא"
    char transliteration[CONFIG_LABEL_MAX];   // "bara"
    char meaning[CONFIG_DESC_MAX];            // "to create (ex nihilo)"
    char semantic_field[CONFIG_LABEL_MAX];    // "creation"
    char scripture[CONFIG_LABEL_MAX];         // "Genesis 1:1"
    char omnicode_mapping[CONFIG_LABEL_MAX];  // "declaration"
    char maps_to[4][CONFIG_LABEL_MAX];        // Keywords it maps to
    int  maps_to_count;
} HebrewRootConfig;

// GreekTermConfig holds a Greek term mapping.
typedef struct {
    char term[32];                            // Greek letters "λόγος"
    char transliteration[CONFIG_LABEL_MAX];   // "logos"
    char meaning[CONFIG_DESC_MAX];            // "word, reason, logic"
    char scripture[CONFIG_LABEL_MAX];         // "John 1:1"
    char omnicode_mapping[CONFIG_LABEL_MAX];  // "language itself"
} GreekTermConfig;

// BiblicalPatternConfig holds a pattern from Scripture.
typedef struct {
    char biblical_pattern[CONFIG_DESC_MAX];   // "Let there be [thing]"
    char scripture[CONFIG_LABEL_MAX];         // "Genesis 1:3"
    char syntactic_role[CONFIG_LABEL_MAX];    // "existence declaration"
    char omnicode_form[CONFIG_DESC_MAX];      // "there shall be [type]"
    char example[CONFIG_DESC_MAX];            // "there shall be trit"
} BiblicalPatternConfig;

// NumberSymbolConfig holds biblical number symbolism.
typedef struct {
    int32_t number;                           // 3, 7, 27, etc.
    char    biblical_meaning[CONFIG_DESC_MAX];// "divine completeness"
    char    scripture[CONFIG_LABEL_MAX];      // Reference
    char    omnicode_mapping[CONFIG_LABEL_MAX];// "ternary system base"
} NumberSymbolConfig;

// BibleEncodingConfig aggregates encoding.toml sections.
typedef struct {
    HebrewRootConfig     hebrew[BIBLE_HEBREW_ROOTS];
    int32_t              hebrew_count;
    GreekTermConfig      greek[BIBLE_GREEK_TERMS];
    int32_t              greek_count;
    BiblicalPatternConfig creation[BIBLE_PATTERNS];
    int32_t              creation_count;
    BiblicalPatternConfig covenant[BIBLE_PATTERNS];
    int32_t              covenant_count;
    BiblicalPatternConfig genealogy[BIBLE_PATTERNS];
    int32_t              genealogy_count;
    NumberSymbolConfig   numbers[16];
    int32_t              numbers_count;
    bool                 loaded;
} BibleEncodingConfig;

// ---------------------------------------------------------------------------
// Bible Unified Configuration
// ---------------------------------------------------------------------------

// BibleConfig aggregates all Bible configuration.
typedef struct {
    BibleAddressingConfig addressing;         // Verse addressing
    BibleEncodingConfig   encoding;           // Scripture ↔ OmniCode mapping
    bool                  loaded;
} BibleConfig;

// ---------------------------------------------------------------------------
// Menu/UI Configuration (from menus/*.jsonc)
// ---------------------------------------------------------------------------

// Menu limits
#define MENU_MAX_ITEMS        32    // Max items per menu
#define MENU_MAX_MENUS        32    // Max menus in collection
#define MENU_MAX_BINDINGS     8     // Max keys per binding
#define MENU_MAX_OPTIONS      16    // Max options for value items

// MenuItemCondition defines when an item is active.
typedef struct {
    char    type[CONFIG_LABEL_MAX];           // "save_exists", "display_mode", "hebrew_state"
    char    value[CONFIG_LABEL_MAX];          // Condition value to check
} MenuItemCondition;

// MenuItemConfig holds a single menu item definition.
//
// Fields:
//   - item_id: Unique identifier for this item
//   - label: Display text
//   - action: navigate, execute, toggle, value, back, quit
//   - target: Target of action (menu_id, command_id, setting_key)
//   - enabled, visible: State flags
//   - hotkey: Keyboard shortcut
//   - condition: Optional enable condition
//   - value_type: For value items (enum, int, float)
//   - options: For enum value items
//   - current: Current value index/state
typedef struct {
    char    item_id[CONFIG_LABEL_MAX];
    char    label[CONFIG_LABEL_MAX];
    char    action[CONFIG_LABEL_MAX];
    char    target[CONFIG_LABEL_MAX];
    bool    enabled;
    bool    visible;
    char    hotkey[8];
    MenuItemCondition condition;
    char    value_type[CONFIG_LABEL_MAX];
    char    options[MENU_MAX_OPTIONS][CONFIG_LABEL_MAX];
    int     option_count;
    int     current;
} MenuItemConfig;

// MenuNavConfig holds navigation rules for a menu.
typedef struct {
    char    default_item[CONFIG_LABEL_MAX];   // Initial selection
    bool    wrap;                              // Wrap at boundaries
    int     columns;                           // Layout columns (1 = vertical list)
    char    cancel_action[CONFIG_LABEL_MAX];  // Action on cancel (back, quit, execute)
    char    cancel_target[CONFIG_LABEL_MAX];  // Target for cancel action
} MenuNavConfig;

// MenuBindingsConfig holds input bindings.
typedef struct {
    char    up[MENU_MAX_BINDINGS][CONFIG_LABEL_MAX];
    int     up_count;
    char    down[MENU_MAX_BINDINGS][CONFIG_LABEL_MAX];
    int     down_count;
    char    left[MENU_MAX_BINDINGS][CONFIG_LABEL_MAX];
    int     left_count;
    char    right[MENU_MAX_BINDINGS][CONFIG_LABEL_MAX];
    int     right_count;
    char    select[MENU_MAX_BINDINGS][CONFIG_LABEL_MAX];
    int     select_count;
    char    cancel[MENU_MAX_BINDINGS][CONFIG_LABEL_MAX];
    int     cancel_count;
} MenuBindingsConfig;

// MenuDisplayHintsCLI holds CLI-specific display hints.
typedef struct {
    int     width;
    char    align[CONFIG_LABEL_MAX];          // left, center, right
    bool    border;
    int     value_column;                      // For settings menus
} MenuDisplayHintsCLI;

// MenuDisplayHintsTUI holds TUI-specific display hints.
typedef struct {
    int     width;
    int     height;
    char    position[CONFIG_LABEL_MAX];        // center, top, bottom
    char    border_style[CONFIG_LABEL_MAX];    // single, double
} MenuDisplayHintsTUI;

// MenuDisplayHintsGUI holds GUI-specific display hints.
typedef struct {
    int     width;
    int     height;
    char    background[CONFIG_LABEL_MAX];      // Background asset ID
    char    animation[CONFIG_LABEL_MAX];       // Entry animation
} MenuDisplayHintsGUI;

// MenuDisplayHintsConfig holds all display hints.
typedef struct {
    MenuDisplayHintsCLI cli;
    MenuDisplayHintsTUI tui;
    MenuDisplayHintsGUI gui;
} MenuDisplayHintsConfig;

// MenuIdentityConfig holds menu identity fields.
typedef struct {
    char    menu_id[CONFIG_LABEL_MAX];
    char    title[CONFIG_LABEL_MAX];
    char    type[CONFIG_LABEL_MAX];            // main, pause, settings, dialog, context
    char    description[CONFIG_DESC_MAX];
} MenuIdentityConfig;

// MenuConfig holds a complete menu definition.
typedef struct {
    MenuIdentityConfig      identity;
    MenuItemConfig          items[MENU_MAX_ITEMS];
    int                     item_count;
    MenuNavConfig           navigation;
    MenuBindingsConfig      bindings;
    MenuDisplayHintsConfig  display_hints;
} MenuConfig;

// HebrewStateRule defines menu behavior based on cognitive state.
typedef struct {
    char    rule_id[CONFIG_LABEL_MAX];
    char    state[CONFIG_LABEL_MAX];           // shavar, chaser, yashar, shalem, tov
    char    effect[CONFIG_LABEL_MAX];          // show_warning, enable_items, disable_items
    char    message[CONFIG_DESC_MAX];          // Optional warning message
    char    items[MENU_MAX_ITEMS][CONFIG_LABEL_MAX]; // Affected item IDs
    int     items_count;
} HebrewStateRule;

// MenuTripwireConfig holds fallback behavior.
typedef struct {
    char    on_menu_not_found[CONFIG_LABEL_MAX];
    char    on_item_not_found[CONFIG_LABEL_MAX];
    char    on_parse_error[CONFIG_LABEL_MAX];
    char    default_menu[CONFIG_LABEL_MAX];
    char    error_menu[CONFIG_LABEL_MAX];
} MenuTripwireConfig;

// MenusConfig holds all menu configurations.
typedef struct {
    char                name[CONFIG_LABEL_MAX];
    char                version[CONFIG_LABEL_MAX];
    MenuConfig          menus[MENU_MAX_MENUS];
    int                 menu_count;
    HebrewStateRule     hebrew_rules[16];
    int                 hebrew_rule_count;
    MenuTripwireConfig  tripwire;
    bool                loaded;
} MenusConfig;

// ---------------------------------------------------------------------------
// Session State Configuration (from session/*.jsonc)
// ---------------------------------------------------------------------------

// SessionCubeState holds the 27-position cube state.
typedef struct {
    int8_t     x;                              // -1, 0, +1
    int8_t     y;                              // -1, 0, +1
    int8_t     z;                              // -1, 0, +1
    char       position_name[CONFIG_LABEL_MAX];
    char       position_description[CONFIG_DESC_MAX];
} SessionCubeState;

// SessionHebrewState holds the Hebrew cognitive state.
typedef struct {
    char       state[CONFIG_LABEL_MAX];        // shavar, chaser, yashar, shalem, tov
    int8_t     state_value;                    // -2 to +2
    char       state_description[CONFIG_DESC_MAX];
    char       k_factor[CONFIG_LABEL_MAX];     // retreat, maintain, advance
    int8_t     k_value;                        // -1, 0, +1
    char       k_description[CONFIG_DESC_MAX];
} SessionHebrewState;

// SessionHealth holds the base100 health state.
typedef struct {
    int8_t     score;                          // -99 to +99
    char       derived_state[CONFIG_LABEL_MAX];
    int64_t    last_change;                    // Timestamp
    char       change_reason[CONFIG_LABEL_MAX];
} SessionHealth;

// SessionTripwire holds fallback tracking.
typedef struct {
    int32_t    count;
    char       active_backend[CONFIG_LABEL_MAX];
    int64_t    last_tripwire;
} SessionTripwire;

// SessionTimestamps holds session timing.
typedef struct {
    int64_t    session_start;
    int64_t    session_end;
    int64_t    last_activity;
    int64_t    total_active_ms;
    int64_t    idle_threshold_ms;
} SessionTimestamps;

// SessionDisplay holds display state.
typedef struct {
    char       mode[CONFIG_LABEL_MAX];         // cli, tui, gui
    char       current_menu[CONFIG_LABEL_MAX];
    char       menu_stack[16][CONFIG_LABEL_MAX];
    int        menu_stack_depth;
} SessionDisplay;

// SessionLog holds logging configuration.
typedef struct {
    char       min_level[CONFIG_LABEL_MAX];
    bool       include_state;
    bool       include_cube;
    bool       timestamps;
    bool       colors;
    int32_t    log_count;
} SessionLog;

// SessionRestoration holds restoration rules.
typedef struct {
    char       on_no_save[CONFIG_LABEL_MAX];
    char       on_corrupt_save[CONFIG_LABEL_MAX];
    char       on_stale_save[CONFIG_LABEL_MAX];
    int32_t    stale_threshold_hours;
    int32_t    max_tripwire_before_reset;
} SessionRestoration;

// SessionConfig holds complete session state.
typedef struct {
    char                  name[CONFIG_LABEL_MAX];
    char                  version[CONFIG_LABEL_MAX];
    char                  session_id[CONFIG_LABEL_MAX];
    char                  instance_id[CONFIG_LABEL_MAX];

    SessionCubeState      cube;
    SessionHebrewState    hebrew;
    SessionHealth         health;
    SessionTripwire       tripwire;
    SessionTimestamps     timestamps;
    SessionDisplay        display;
    SessionLog            log;
    SessionRestoration    restoration;

    bool                  loaded;
    bool                  modified;            // Dirty flag for save
} SessionConfig;

// ---------------------------------------------------------------------------
// Display Configuration (from display/formatting.jsonc)
// ---------------------------------------------------------------------------

// DisplayStyleDef defines a single style (fg, bg, bold).
// Maps to DisplayStyle in cornerstone/engine/display/include/display.h
typedef struct {
    char    name[CONFIG_LABEL_MAX];           // Style name
    int8_t  fg;                               // Foreground color (0-16)
    int8_t  bg;                               // Background color (0-16)
    bool    bold;                             // Bold text
} DisplayStyleDef;

// DisplayThemeSyntax holds syntax highlighting colors for a theme.
// Each element has _fg (foreground), _bg (background), and _bold fields.
typedef struct {
    // Normal text
    char    normal[CONFIG_LABEL_MAX];
    char    normal_bg[CONFIG_LABEL_MAX];
    bool    normal_bold;

    // Pragma directives (#!omni)
    char    pragma[CONFIG_LABEL_MAX];
    char    pragma_bg[CONFIG_LABEL_MAX];
    bool    pragma_bold;

    // Language keywords
    char    keyword[CONFIG_LABEL_MAX];
    char    keyword_bg[CONFIG_LABEL_MAX];
    bool    keyword_bold;

    // Comments
    char    comment[CONFIG_LABEL_MAX];
    char    comment_bg[CONFIG_LABEL_MAX];
    bool    comment_bold;

    // String literals
    char    string[CONFIG_LABEL_MAX];
    char    string_bg[CONFIG_LABEL_MAX];
    bool    string_bold;

    // Numeric literals
    char    number[CONFIG_LABEL_MAX];
    char    number_bg[CONFIG_LABEL_MAX];
    bool    number_bold;

    // Block markers
    char    block_marker[CONFIG_LABEL_MAX];
    char    block_marker_bg[CONFIG_LABEL_MAX];
    bool    block_marker_bold;

    // Section markers
    char    section_marker[CONFIG_LABEL_MAX];
    char    section_marker_bg[CONFIG_LABEL_MAX];
    bool    section_marker_bold;

    // Identifiers
    char    identifier[CONFIG_LABEL_MAX];
    char    identifier_bg[CONFIG_LABEL_MAX];
    bool    identifier_bold;

    // Operators
    char    operator_c[CONFIG_LABEL_MAX];
    char    operator_bg[CONFIG_LABEL_MAX];
    bool    operator_bold;

    // Type names
    char    type[CONFIG_LABEL_MAX];
    char    type_bg[CONFIG_LABEL_MAX];
    bool    type_bold;

    // Function names
    char    function[CONFIG_LABEL_MAX];
    char    function_bg[CONFIG_LABEL_MAX];
    bool    function_bold;

    // Error highlighting
    char    error[CONFIG_LABEL_MAX];
    char    error_bg[CONFIG_LABEL_MAX];
    bool    error_bold;
} DisplayThemeSyntax;

// DisplayThemeBlocks holds block colors for a theme.
// Each block has _fg, _bg, and _bold fields.
typedef struct {
    char    metadata[CONFIG_LABEL_MAX];
    char    metadata_bg[CONFIG_LABEL_MAX];
    bool    metadata_bold;

    char    setup[CONFIG_LABEL_MAX];
    char    setup_bg[CONFIG_LABEL_MAX];
    bool    setup_bold;

    char    header[CONFIG_LABEL_MAX];
    char    header_bg[CONFIG_LABEL_MAX];
    bool    header_bold;

    char    context[CONFIG_LABEL_MAX];
    char    context_bg[CONFIG_LABEL_MAX];
    bool    context_bold;

    char    body[CONFIG_LABEL_MAX];
    char    body_bg[CONFIG_LABEL_MAX];
    bool    body_bold;

    char    content[CONFIG_LABEL_MAX];
    char    content_bg[CONFIG_LABEL_MAX];
    bool    content_bold;

    char    closing[CONFIG_LABEL_MAX];
    char    closing_bg[CONFIG_LABEL_MAX];
    bool    closing_bold;

    char    footer[CONFIG_LABEL_MAX];
    char    footer_bg[CONFIG_LABEL_MAX];
    bool    footer_bold;
} DisplayThemeBlocks;

// DisplayThemeMenu holds menu colors for a theme.
// Each element has _fg, _bg, and _bold fields.
typedef struct {
    char    bar_fg[CONFIG_LABEL_MAX];
    char    bar_bg[CONFIG_LABEL_MAX];
    bool    bar_bold;

    char    item_fg[CONFIG_LABEL_MAX];
    char    item_bg[CONFIG_LABEL_MAX];
    bool    item_bold;

    char    selected_fg[CONFIG_LABEL_MAX];
    char    selected_bg[CONFIG_LABEL_MAX];
    bool    selected_bold;

    char    hotkey_fg[CONFIG_LABEL_MAX];
    char    hotkey_bg[CONFIG_LABEL_MAX];
    bool    hotkey_bold;

    char    disabled_fg[CONFIG_LABEL_MAX];
    char    disabled_bg[CONFIG_LABEL_MAX];
    bool    disabled_bold;
} DisplayThemeMenu;

// DisplayThemeStatus holds status bar colors for a theme.
// Complete theme support for status bar elements.
typedef struct {
    char    bar_fg[CONFIG_LABEL_MAX];
    char    bar_bg[CONFIG_LABEL_MAX];
    bool    bar_bold;

    char    mode_fg[CONFIG_LABEL_MAX];
    char    mode_bg[CONFIG_LABEL_MAX];
    bool    mode_bold;

    char    file_fg[CONFIG_LABEL_MAX];
    char    file_bg[CONFIG_LABEL_MAX];
    bool    file_bold;

    char    position_fg[CONFIG_LABEL_MAX];
    char    position_bg[CONFIG_LABEL_MAX];
    bool    position_bold;

    // Modified indicator
    char    modified_fg[CONFIG_LABEL_MAX];
    char    modified_bg[CONFIG_LABEL_MAX];
    bool    modified_bold;

    // Cursor position display
    char    cursor_fg[CONFIG_LABEL_MAX];
    char    cursor_bg[CONFIG_LABEL_MAX];
    bool    cursor_bold;
} DisplayThemeStatus;

// DisplayThemeEditor holds editor colors for a theme.
// Complete theme support for editor elements.
typedef struct {
    // Main text area
    char    text_fg[CONFIG_LABEL_MAX];
    char    text_bg[CONFIG_LABEL_MAX];
    bool    text_bold;

    // Line numbers
    char    line_number_fg[CONFIG_LABEL_MAX];
    char    line_number_bg[CONFIG_LABEL_MAX];
    bool    line_number_bold;

    // Current line highlight
    char    current_line_fg[CONFIG_LABEL_MAX];
    char    current_line_bg[CONFIG_LABEL_MAX];
    bool    current_line_bold;

    // Cursor
    char    cursor_fg[CONFIG_LABEL_MAX];
    char    cursor_bg[CONFIG_LABEL_MAX];
    bool    cursor_bold;

    // Selection
    char    selection_fg[CONFIG_LABEL_MAX];
    char    selection_bg[CONFIG_LABEL_MAX];
    bool    selection_bold;

    // Matching bracket
    char    matching_bracket_fg[CONFIG_LABEL_MAX];
    char    matching_bracket_bg[CONFIG_LABEL_MAX];
    bool    matching_bracket_bold;

    // Gutter (line number area)
    char    gutter_fg[CONFIG_LABEL_MAX];
    char    gutter_bg[CONFIG_LABEL_MAX];
    bool    gutter_bold;
} DisplayThemeEditor;

// DisplayTheme holds a complete theme definition.
typedef struct {
    char                name[CONFIG_LABEL_MAX];
    char                description[CONFIG_DESC_MAX];
    DisplayThemeSyntax  syntax;
    DisplayThemeBlocks  blocks;
    DisplayThemeMenu    menu;
    DisplayThemeStatus  status;
    DisplayThemeEditor  editor;
} DisplayTheme;

// DisplayKeyBinding holds a single keybinding.
typedef struct {
    char    key[CONFIG_LABEL_MAX];            // "Ctrl+S", "F5", etc.
    char    action[CONFIG_LABEL_MAX];         // Action name
} DisplayKeyBinding;

// DisplayKeybindings holds keybindings for all contexts.
typedef struct {
    DisplayKeyBinding global[DISPLAY_KEY_BINDING_COUNT];
    int               global_count;
    DisplayKeyBinding editor[DISPLAY_KEY_BINDING_COUNT];
    int               editor_count;
    DisplayKeyBinding menu[DISPLAY_KEY_BINDING_COUNT];
    int               menu_count;
    DisplayKeyBinding prompt[DISPLAY_KEY_BINDING_COUNT];
    int               prompt_count;
} DisplayKeybindings;

// DisplayLayout holds layout dimensions.
typedef struct {
    int     line_number_width;
    int     left_margin;
    int     right_margin;
    int     tab_width;
    int     scroll_margin;
    int     file_tree_width;
    int     output_height;
    int     min_editor_width;
    int     min_editor_height;
    int     menu_item_padding;
} DisplayLayout;

// DisplayIDESettings holds IDE-specific settings.
typedef struct {
    bool    show_line_numbers;
    bool    show_block_indicators;
    bool    highlight_current_line;
    bool    show_matching_brackets;
    bool    auto_indent;
    bool    wrap_lines;
    bool    tab_to_spaces;
    bool    trim_trailing_whitespace;
    bool    ensure_newline_at_eof;
    bool    syntax_enabled;
    bool    validate_on_save;
} DisplayIDESettings;

// DisplayBehavior holds behavioral settings.
typedef struct {
    bool    panic_recovery_enabled;
    bool    reduce_motion;
    bool    high_contrast_mode;
    bool    unicode_fallback;
    bool    color_fallback;
    int     min_terminal_width;
    int     min_terminal_height;
} DisplayBehavior;

// IDEDisplayConfig aggregates all display configuration.
// NOTE: Named struct for forward declaration compatibility with display.h.
// Distinct from cornerstone's DisplayConfig which is for init parameters only.
typedef struct IDEDisplayConfig {
    char                name[CONFIG_LABEL_MAX];
    char                version[CONFIG_LABEL_MAX];
    char                current_theme[CONFIG_LABEL_MAX];

    DisplayTheme        themes[DISPLAY_THEME_COUNT];
    int                 theme_count;

    DisplayStyleDef     styles[DISPLAY_STYLE_COUNT];
    int                 style_count;

    DisplayKeybindings  keybindings;
    DisplayLayout       layout;
    DisplayIDESettings  ide;
    DisplayBehavior     behavior;

    bool                loaded;
} IDEDisplayConfig;

// ---------------------------------------------------------------------------
// Unified Configuration
// ---------------------------------------------------------------------------

// Config aggregates all configuration sections.
//
// Fields:
//   - math: Ternary math configuration (ternary.toml)
//   - primitives: Primitive types (primitives.toml)
//   - omni: OmniCode configuration (blocks.toml, errors.toml, recovery.toml)
//   - bible: Bible configuration (addressing.toml, encoding.toml)
//   - menus: Menu/UI configuration (menus/*.jsonc)
//   - display: Display configuration (display/formatting.jsonc)
//   - session: Session state (session/*.jsonc) - persists across restarts
//   - loaded: Whether config has been loaded
typedef struct {
    TernaryMathConfig math;
    PrimitivesConfig  primitives;
    OmniConfig        omni;
    BibleConfig       bible;
    MenusConfig       menus;
    IDEDisplayConfig  display;
    SessionConfig     session;
    bool              loaded;
} Config;

// # S.3c Opaque Types [OPAQUE]

// [Reserved: No opaque types needed]

// # S.3d Error Codes [ERRORS]

// [Defined above as ConfigError enum]

// # S.4 Function Prototypes [PROTOTYPES]

// # S.4a Lifecycle [LIFECYCLE]

Config* config_create(void);
int config_init(Config* cfg);
void config_destroy(Config* cfg);

// # S.4b Accessors [ACCESSORS]

bool config_is_loaded(const Config* cfg);

// # S.4c Operations [OPERATIONS]

int config_load_from_dir(Config* cfg, const char* core_dir);
int config_load_ternary(Config* cfg, const char* path);
int config_load_primitives(Config* cfg, const char* path);

// OmniCode config loaders
int config_load_omni(Config* cfg, const char* omni_dir);
int config_load_omni_blocks(OmniBlocksConfig* blocks, const char* path);
int config_load_omni_errors(OmniErrorsConfig* errors, const char* path);
int config_load_omni_recovery(OmniRecoveryConfig* recovery, const char* path);

// Bible config loaders
int config_load_bible(Config* cfg, const char* bible_dir);
int config_load_bible_addressing(BibleAddressingConfig* addr, const char* path);
int config_load_bible_encoding(BibleEncodingConfig* enc, const char* path);

// Utility: Look up verse ordinal from book/chapter/verse
int32_t bible_verse_to_ordinal(const BibleAddressingConfig* addr,
                                const char* book, int chapter, int verse);

// Utility: Look up book/chapter/verse from ordinal
int bible_ordinal_to_verse(const BibleAddressingConfig* addr, int32_t ordinal,
                           char* book, size_t book_size, int* chapter, int* verse);

// Utility: Find Hebrew root by keyword
const HebrewRootConfig* bible_find_hebrew_by_keyword(
    const BibleEncodingConfig* enc, const char* keyword);

// Utility: Find Scripture source for OmniCode keyword
int bible_find_scripture_source(const BibleEncodingConfig* enc,
                                const char* keyword,
                                char* scripture, size_t scripture_size);

// Menu config loaders
int config_load_menus(MenusConfig* menus, const char* path);

// Utility: Find menu by ID
const MenuConfig* menu_find_by_id(const MenusConfig* menus, const char* menu_id);

// Utility: Find menu item by ID
const MenuItemConfig* menu_find_item(const MenuConfig* menu, const char* item_id);

// Utility: Get default menu
const MenuConfig* menu_get_default(const MenusConfig* menus);

// Utility: Apply Hebrew state rules to menu
int menu_apply_hebrew_rules(const MenusConfig* menus, const char* state,
                            MenuConfig* menu_out);

// Display config loaders
int config_load_display(IDEDisplayConfig* display, const char* path);

// Utility: Find theme by name
const DisplayTheme* display_find_theme(const IDEDisplayConfig* display, const char* name);

// Utility: Get current theme
const DisplayTheme* display_get_current_theme(const IDEDisplayConfig* display);

// Utility: Find style by name
const DisplayStyleDef* display_find_style(const IDEDisplayConfig* display, const char* name);

// Utility: Find keybinding action for key in context
const char* display_find_action(const IDEDisplayConfig* display, const char* context, const char* key);

// Utility: Color name to DisplayColor enum value
int8_t display_color_from_name(const char* name);

// Session config loaders
int config_load_session(SessionConfig* session, const char* path);
int config_load_session_defaults(SessionConfig* session, const char* path);

// Session state operations
int session_save(const SessionConfig* session, const char* path);
int session_restore(SessionConfig* session, const char* path);
int session_mark_modified(SessionConfig* session);
int session_update_timestamps(SessionConfig* session, int64_t now_ms);
int session_apply_tripwire(SessionConfig* session, const char* new_backend);

// Utility: Map cube position to Hebrew state
const char* session_cube_to_hebrew(int8_t x, int8_t y, int8_t z);

// Utility: Derive Hebrew state from health score
const char* session_health_to_hebrew(int8_t health_score);

// Utility: Check if session is stale
bool session_is_stale(const SessionConfig* session, int64_t now_ms);

// Utility: Should reset due to tripwire count
bool session_should_reset(const SessionConfig* session);

// # S.5 File-Level Declarations [FILE_LEVEL]

// # S.5a Variables [VARIABLES]

// [Reserved: No global variables]

// # S.5b Function Types [FUNC_TYPES]

// [Reserved: No function types]

// # S.5c Infrastructure [INFRASTRUCTURE]

// [Reserved: Infrastructure lives in source files]

// # S.6 Omission Guide [OMISSION]

// [Reserved: All sections present]

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// # B.1 Organizational Chart [ORG_CHART]

// # B.1a Structure [STRUCTURE]

// Types:      7 structs (config sections), 1 enum (errors)
// Functions:  3 lifecycle, 1 accessor, 3 operations
// Externs:    0 globals

// # B.1b Flow [FLOW]

// Implementation: config.c

// # B.1c Counts [COUNTS]

// Types:      8 total
// Functions:  7 total
// Externs:    0 total

// # B.2 Helpers [HELPERS]

// [Reserved: Helpers implemented in config.c]

// # B.3 Core Operations [OPERATIONS]

// [Reserved: Operations implemented in config.c]

// # B.4 Error Handling [ERRORS]

// # B.4a Error Queries [QUERIES]

// config_get_error_string returns human-readable description.
//
// Parameters:
//   err: Error code from ConfigError enum
//
// Returns: Static string (do not free)
const char* config_get_error_string(ConfigError err);

// # B.4b Recovery [RECOVERY]

// config_reset clears config to unloaded state.
//
// Parameters:
//   cfg: Config to reset
//
// Returns: 0 on success, error code on failure
int config_reset(Config* cfg);

// # B.5 Public API [API]

// [Reserved: Declared in S.4 Prototypes]

// # B.6 Omission Guide [OMISSION]

// [Reserved: All sections present]

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// # X.1 Validation [VALIDATION]

// # X.1a Build Commands [BUILD]
//
//   gcc -fsyntax-only -Wall -Wextra config.h
//   gcc -c -Wall -Wextra config.c -I../include

// # X.1b Checks [CHECKS]
//
//   - Self-contained (includes without errors)
//   - Structs align with TOML spec structure
//   - All field types match TOML value types

// # X.2 Execution [EXECUTION]

// # X.2a Include [INCLUDE]
//
//   #include "config.h"

// # X.2b Usage Example [USAGE]
//
//   Config cfg;
//   config_init(&cfg);
//   config_load_from_dir(&cfg, "word/core");
//
//   // Access typed values
//   int32_t base = cfg.math.trit.base;  // 3
//   int8_t neg = cfg.math.arithmetic.negation[2];  // negate(+1) = -1
//
//   config_destroy(&cfg);

// # X.3 Cleanup [CLEANUP]

// # X.3a Ownership [OWNERSHIP]
//
//   config_create() returns:   Caller owns, use config_destroy() to free
//   config_init() initializes: Caller-owned memory, use config_destroy()

// # X.4 Policy [POLICY]
//
// Safe to Modify:
//   - Add new config sections as TOML specs grow
//   - Add accessor functions for convenience
//
// NEVER Modify:
//   - Struct field types (must match TOML spec)
//   - Field names (semantic alignment with specs)

// # X.5 Extension [EXTENSION]
//
// Adding Specs:
//   1. Add struct for new spec section
//   2. Add field to Config aggregate
//   3. Add load function (config_load_[name])
//   4. Update config_load_from_dir

// # X.6 Troubleshooting [TROUBLESHOOTING]
//
// "CONFIG_ERR_FILE":
//   -> Check path exists and is readable
//
// "CONFIG_ERR_PARSE":
//   -> TOML syntax error, check spec file
//
// "CONFIG_ERR_NOT_FOUND":
//   -> Section or key missing from TOML file

// # X.7 Reference [REFERENCE]
//
// Quick Load:
//   Config cfg = {0};
//   config_load_from_dir(&cfg, "word/core");
//
// Access Math:
//   cfg.math.trit.values[1]  // 0 (neutral)
//   cfg.math.packing.trits_per_byte  // 5
//
// Access Types:
//   cfg.primitives.int27.max_value  // 3812798742493

// # X.8 Closing Note [NOTE]
//
// This header bridges TOML specifications to C code.
// Specifications define truth; code implements it.
//
// "Prepare thy work without, and make it fit for thyself in the field."
//   - Proverbs 24:27

// =============================================================================
// END CLOSING
// =============================================================================

#endif // BERESHIT_TRIT_CONFIG_H
