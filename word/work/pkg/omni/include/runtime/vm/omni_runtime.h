// #!omni code --c -library
// #!omni meta.key = B-word-work-pkg-omni-runtime
// #!omni meta.from = word/seed/code/c/header.h
// #!omni meta.at = a-01.00
//
// omni_runtime.h — OmniCode Runtime Data Structures
// Dynamic strings, lists, and maps for VM intrinsics.

#ifndef BERESHIT_OMNI_RUNTIME_H
#define BERESHIT_OMNI_RUNTIME_H

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// # M.1 Core Identity [IDENTITY]
//
// Key: B-word-work-pkg-omni-runtime
// Title: OmniCode Runtime Data Structures
// Type: Header
// Role: Provide dynamic data structures for VM intrinsics

// # M.8 Grounding [GROUNDING]
//
// Scripture: Genesis 1:11 - "Let the earth bring forth... yielding seed"
// Principle: Dynamic growth from established patterns
// Anchor: Genesis 2:9 - "And out of the ground made the LORD God to grow"

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include "omni_ir.h"

// # S.1 Runtime Constants [CONSTANTS]

#define OMNI_RT_STRING_POOL_SIZE    256     // Initial string pool capacity
#define OMNI_RT_LIST_INIT_SIZE      16      // Initial list capacity
#define OMNI_RT_MAP_INIT_SIZE       16      // Initial map capacity

// # S.2 Runtime String [STRING]
//
// Dynamically allocated strings managed by the runtime.
// Different from constant pool strings (which are compile-time).

typedef struct {
    char*       data;           // String data (null-terminated)
    uint32_t    length;         // String length (excluding null)
    uint32_t    capacity;       // Allocated capacity
    uint32_t    ref_count;      // Reference count for GC
} OmniString;

// # S.3 Runtime List [LIST]
//
// Dynamic array (generic - holds OmniValue*)

typedef struct {
    OmniValue*          items;      // Array of values
    uint32_t            length;     // Current element count
    uint32_t            capacity;   // Allocated capacity
    uint32_t            ref_count;  // Reference count
} OmniList;

// # S.4 Runtime Map Entry [MAPENTRY]

typedef struct {
    OmniString*         key;        // String key
    OmniValue           value;      // Value (copy, not pointer)
    bool                occupied;   // Is this slot occupied?
} OmniMapEntry;

// # S.5 Runtime Map [MAP]
//
// Hash map with string keys

typedef struct {
    OmniMapEntry*       entries;    // Hash table entries
    uint32_t            count;      // Number of entries
    uint32_t            capacity;   // Table capacity
    uint32_t            ref_count;  // Reference count
} OmniMap;

// # S.6 Runtime Context [CONTEXT]
//
// Manages all runtime allocations for cleanup

typedef struct {
    // String pool
    OmniString**        strings;
    uint32_t            string_count;
    uint32_t            string_capacity;

    // Lists
    OmniList**          lists;
    uint32_t            list_count;
    uint32_t            list_capacity;

    // Maps
    OmniMap**           maps;
    uint32_t            map_count;
    uint32_t            map_capacity;

    // Open files
    FILE**              files;
    uint32_t            file_count;
    uint32_t            file_capacity;
} OmniRuntime;

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// # B.1 Runtime Lifecycle [LIFECYCLE]

// Create runtime context
OmniRuntime* omni_runtime_create(void);

// Destroy runtime context (frees all allocations)
void omni_runtime_destroy(OmniRuntime* rt);

// # B.2 String Operations [STRING_OPS]

// Create new string
OmniString* omni_string_create(OmniRuntime* rt, const char* data);

// Create string from substring
OmniString* omni_string_substr(OmniRuntime* rt, OmniString* str, uint32_t start, uint32_t len);

// Concatenate two strings
OmniString* omni_string_concat(OmniRuntime* rt, OmniString* a, OmniString* b);

// String length
uint32_t omni_string_length(OmniString* str);

// Character at index (-1 if out of bounds)
int32_t omni_string_char_at(OmniString* str, uint32_t index);

// Find substring (-1 if not found)
int32_t omni_string_find(OmniString* haystack, OmniString* needle);

// String comparison (-1, 0, +1)
int8_t omni_string_compare(OmniString* a, OmniString* b);

// String equality
bool omni_string_equals(OmniString* a, OmniString* b);

// Trim whitespace
OmniString* omni_string_trim(OmniRuntime* rt, OmniString* str);

// Convert to uppercase
OmniString* omni_string_upper(OmniRuntime* rt, OmniString* str);

// Convert to lowercase
OmniString* omni_string_lower(OmniRuntime* rt, OmniString* str);

// Starts with prefix
bool omni_string_starts_with(OmniString* str, OmniString* prefix);

// Ends with suffix
bool omni_string_ends_with(OmniString* str, OmniString* suffix);

// Replace all occurrences
OmniString* omni_string_replace(OmniRuntime* rt, OmniString* str,
                                 OmniString* find, OmniString* replace);

// Split by delimiter (returns list)
OmniList* omni_string_split(OmniRuntime* rt, OmniString* str, OmniString* delim);

// Join list with delimiter
OmniString* omni_string_join(OmniRuntime* rt, OmniList* list, OmniString* delim);

// Get raw C string (for printing/comparison)
const char* omni_string_cstr(OmniString* str);

// # B.3 List Operations [LIST_OPS]

// Create new list
OmniList* omni_list_create(OmniRuntime* rt);

// Push value to end
bool omni_list_push(OmniList* list, OmniValue value);

// Pop value from end
bool omni_list_pop(OmniList* list, OmniValue* value);

// Get element at index
bool omni_list_get(OmniList* list, uint32_t index, OmniValue* value);

// Set element at index
bool omni_list_set(OmniList* list, uint32_t index, OmniValue value);

// List length
uint32_t omni_list_length(OmniList* list);

// Find element (-1 if not found)
int32_t omni_list_find(OmniList* list, OmniValue value);

// Clear list
void omni_list_clear(OmniList* list);

// # B.4 Map Operations [MAP_OPS]

// Create new map
OmniMap* omni_map_create(OmniRuntime* rt);

// Get value by key
bool omni_map_get(OmniMap* map, OmniString* key, OmniValue* value);

// Set value by key
bool omni_map_set(OmniRuntime* rt, OmniMap* map, OmniString* key, OmniValue value);

// Check if key exists
bool omni_map_has(OmniMap* map, OmniString* key);

// Delete key
bool omni_map_delete(OmniMap* map, OmniString* key);

// Get all keys as list
OmniList* omni_map_keys(OmniRuntime* rt, OmniMap* map);

// Map size
uint32_t omni_map_length(OmniMap* map);

// # B.5 File Operations [FILE_OPS]

// Open file
FILE* omni_file_open(OmniRuntime* rt, const char* path, const char* mode);

// Close file
void omni_file_close(OmniRuntime* rt, FILE* f);

// Read entire file
OmniString* omni_file_read(OmniRuntime* rt, FILE* f);

// Write string to file
bool omni_file_write(FILE* f, OmniString* str);

// Read line from file
OmniString* omni_file_readline(OmniRuntime* rt, FILE* f);

// Write line to file
bool omni_file_writeline(FILE* f, OmniString* str);

// File exists
bool omni_file_exists(const char* path);

// Directory exists
bool omni_dir_exists(const char* path);

// File size
int64_t omni_file_size(const char* path);

// Delete file
bool omni_file_delete(const char* path);

// Create directory
bool omni_dir_create(const char* path);

// List directory
OmniList* omni_dir_list(OmniRuntime* rt, const char* path);

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// # X.1 Usage Example
//
//   OmniRuntime* rt = omni_runtime_create();
//
//   // Strings
//   OmniString* s1 = omni_string_create(rt, "Hello");
//   OmniString* s2 = omni_string_create(rt, " World");
//   OmniString* s3 = omni_string_concat(rt, s1, s2);
//   printf("%s\n", omni_string_cstr(s3));
//
//   // Lists
//   OmniList* list = omni_list_create(rt);
//   omni_list_push(list, (OmniValue){.type = VAL_INT, .as_int = 42});
//
//   // Cleanup
//   omni_runtime_destroy(rt);

// # X.2 Closing Note
//
// "Let the earth bring forth... whose seed is in itself."
//   — Genesis 1:11
//
// Dynamic growth follows established patterns.

// =============================================================================
// END CLOSING
// =============================================================================

#endif // BERESHIT_OMNI_RUNTIME_H
