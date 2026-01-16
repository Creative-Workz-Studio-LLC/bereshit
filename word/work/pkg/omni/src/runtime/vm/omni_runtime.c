// #!omni code --c -library
// #!omni meta.key = B-word-work-pkg-omni-runtime-impl
// #!omni meta.from = word/seed/code/c/source.c
// #!omni meta.at = a-01.00
//
// omni_runtime.c — OmniCode Runtime Implementation
// Dynamic strings, lists, and maps for VM intrinsics.

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// # M.1 Core Identity [IDENTITY]
//
// Key: B-word-work-pkg-omni-runtime-impl
// Title: OmniCode Runtime Implementation
// Type: Source
// Role: Implement dynamic data structures for VM intrinsics

// # M.8 Grounding [GROUNDING]
//
// Scripture: Genesis 1:11 - "Let the earth bring forth"
// Principle: Dynamic growth from established patterns
// Anchor: Genesis 2:9 - "And out of the ground made the LORD God to grow"

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <dirent.h>
#include "omni_ir.h"
#include "omni_runtime.h"

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// # B.1 Runtime Lifecycle [LIFECYCLE]

OmniRuntime* omni_runtime_create(void) {
    OmniRuntime* rt = calloc(1, sizeof(OmniRuntime));
    if (!rt) return NULL;

    // Initialize string pool
    rt->string_capacity = 64;
    rt->strings = calloc(rt->string_capacity, sizeof(OmniString*));

    // Initialize list pool
    rt->list_capacity = 16;
    rt->lists = calloc(rt->list_capacity, sizeof(OmniList*));

    // Initialize map pool
    rt->map_capacity = 16;
    rt->maps = calloc(rt->map_capacity, sizeof(OmniMap*));

    // Initialize file pool
    rt->file_capacity = 8;
    rt->files = calloc(rt->file_capacity, sizeof(FILE*));

    return rt;
}

void omni_runtime_destroy(OmniRuntime* rt) {
    if (!rt) return;

    // Free strings
    for (uint32_t i = 0; i < rt->string_count; i++) {
        if (rt->strings[i]) {
            free(rt->strings[i]->data);
            free(rt->strings[i]);
        }
    }
    free(rt->strings);

    // Free lists
    for (uint32_t i = 0; i < rt->list_count; i++) {
        if (rt->lists[i]) {
            free(rt->lists[i]->items);
            free(rt->lists[i]);
        }
    }
    free(rt->lists);

    // Free maps
    for (uint32_t i = 0; i < rt->map_count; i++) {
        if (rt->maps[i]) {
            free(rt->maps[i]->entries);
            free(rt->maps[i]);
        }
    }
    free(rt->maps);

    // Close files
    for (uint32_t i = 0; i < rt->file_count; i++) {
        if (rt->files[i]) {
            fclose(rt->files[i]);
        }
    }
    free(rt->files);

    free(rt);
}

// # B.2 String Operations [STRING_OPS]

static OmniString* register_string(OmniRuntime* rt, OmniString* str) {
    if (!rt || !str) return str;

    // Grow pool if needed
    if (rt->string_count >= rt->string_capacity) {
        rt->string_capacity *= 2;
        rt->strings = realloc(rt->strings, rt->string_capacity * sizeof(OmniString*));
    }

    rt->strings[rt->string_count++] = str;
    return str;
}

OmniString* omni_string_create(OmniRuntime* rt, const char* data) {
    OmniString* str = calloc(1, sizeof(OmniString));
    if (!str) return NULL;

    if (data) {
        str->length = strlen(data);
        str->capacity = str->length + 1;
        str->data = malloc(str->capacity);
        if (str->data) {
            memcpy(str->data, data, str->capacity);
        }
    } else {
        str->length = 0;
        str->capacity = 1;
        str->data = malloc(1);
        if (str->data) str->data[0] = '\0';
    }

    str->ref_count = 1;
    return register_string(rt, str);
}

OmniString* omni_string_substr(OmniRuntime* rt, OmniString* str, uint32_t start, uint32_t len) {
    if (!str || !str->data) return omni_string_create(rt, "");

    // Clamp bounds
    if (start >= str->length) return omni_string_create(rt, "");
    if (start + len > str->length) len = str->length - start;

    OmniString* result = calloc(1, sizeof(OmniString));
    if (!result) return NULL;

    result->length = len;
    result->capacity = len + 1;
    result->data = malloc(result->capacity);
    if (result->data) {
        memcpy(result->data, str->data + start, len);
        result->data[len] = '\0';
    }

    result->ref_count = 1;
    return register_string(rt, result);
}

OmniString* omni_string_concat(OmniRuntime* rt, OmniString* a, OmniString* b) {
    if (!a && !b) return omni_string_create(rt, "");
    if (!a) return omni_string_create(rt, b->data);
    if (!b) return omni_string_create(rt, a->data);

    OmniString* result = calloc(1, sizeof(OmniString));
    if (!result) return NULL;

    result->length = a->length + b->length;
    result->capacity = result->length + 1;
    result->data = malloc(result->capacity);
    if (result->data) {
        memcpy(result->data, a->data, a->length);
        memcpy(result->data + a->length, b->data, b->length + 1);
    }

    result->ref_count = 1;
    return register_string(rt, result);
}

uint32_t omni_string_length(OmniString* str) {
    return str ? str->length : 0;
}

int32_t omni_string_char_at(OmniString* str, uint32_t index) {
    if (!str || index >= str->length) return -1;
    return (unsigned char)str->data[index];
}

int32_t omni_string_find(OmniString* haystack, OmniString* needle) {
    if (!haystack || !needle || !haystack->data || !needle->data) return -1;
    if (needle->length == 0) return 0;
    if (needle->length > haystack->length) return -1;

    char* found = strstr(haystack->data, needle->data);
    if (!found) return -1;
    return (int32_t)(found - haystack->data);
}

int8_t omni_string_compare(OmniString* a, OmniString* b) {
    if (!a || !a->data) return b && b->data ? -1 : 0;
    if (!b || !b->data) return 1;

    int cmp = strcmp(a->data, b->data);
    if (cmp < 0) return -1;
    if (cmp > 0) return +1;
    return 0;
}

bool omni_string_equals(OmniString* a, OmniString* b) {
    if (!a || !b) return a == b;
    if (a->length != b->length) return false;
    if (!a->data || !b->data) return a->data == b->data;
    return strcmp(a->data, b->data) == 0;
}

OmniString* omni_string_trim(OmniRuntime* rt, OmniString* str) {
    if (!str || !str->data || str->length == 0) {
        return omni_string_create(rt, "");
    }

    // Find start (skip whitespace)
    uint32_t start = 0;
    while (start < str->length && isspace((unsigned char)str->data[start])) {
        start++;
    }

    // Find end (skip trailing whitespace)
    uint32_t end = str->length;
    while (end > start && isspace((unsigned char)str->data[end - 1])) {
        end--;
    }

    return omni_string_substr(rt, str, start, end - start);
}

OmniString* omni_string_upper(OmniRuntime* rt, OmniString* str) {
    if (!str || !str->data) return omni_string_create(rt, "");

    OmniString* result = calloc(1, sizeof(OmniString));
    if (!result) return NULL;

    result->length = str->length;
    result->capacity = str->capacity;
    result->data = malloc(result->capacity);
    result->ref_count = 1;

    if (result->data) {
        for (uint32_t i = 0; i < str->length; i++) {
            result->data[i] = toupper((unsigned char)str->data[i]);
        }
        result->data[result->length] = '\0';
    }

    return register_string(rt, result);
}

OmniString* omni_string_lower(OmniRuntime* rt, OmniString* str) {
    if (!str || !str->data) return omni_string_create(rt, "");

    OmniString* result = calloc(1, sizeof(OmniString));
    if (!result) return NULL;

    result->length = str->length;
    result->capacity = str->capacity;
    result->data = malloc(result->capacity);
    result->ref_count = 1;

    if (result->data) {
        for (uint32_t i = 0; i < str->length; i++) {
            result->data[i] = tolower((unsigned char)str->data[i]);
        }
        result->data[result->length] = '\0';
    }

    return register_string(rt, result);
}

bool omni_string_starts_with(OmniString* str, OmniString* prefix) {
    if (!str || !prefix || !str->data || !prefix->data) return false;
    if (prefix->length > str->length) return false;
    return strncmp(str->data, prefix->data, prefix->length) == 0;
}

bool omni_string_ends_with(OmniString* str, OmniString* suffix) {
    if (!str || !suffix || !str->data || !suffix->data) return false;
    if (suffix->length > str->length) return false;
    return strcmp(str->data + str->length - suffix->length, suffix->data) == 0;
}

OmniString* omni_string_replace(OmniRuntime* rt, OmniString* str,
                                 OmniString* find, OmniString* replace) {
    if (!str || !str->data) return omni_string_create(rt, "");
    if (!find || !find->data || find->length == 0) {
        return omni_string_create(rt, str->data);
    }
    if (!replace) replace = omni_string_create(rt, "");

    // Count occurrences
    uint32_t count = 0;
    const char* pos = str->data;
    while ((pos = strstr(pos, find->data)) != NULL) {
        count++;
        pos += find->length;
    }

    if (count == 0) return omni_string_create(rt, str->data);

    // Calculate new length
    uint32_t new_len = str->length + count * ((int32_t)replace->length - (int32_t)find->length);

    OmniString* result = calloc(1, sizeof(OmniString));
    if (!result) return NULL;

    result->length = new_len;
    result->capacity = new_len + 1;
    result->data = malloc(result->capacity);
    result->ref_count = 1;

    if (result->data) {
        char* dst = result->data;
        const char* src = str->data;
        while ((pos = strstr(src, find->data)) != NULL) {
            // Copy before match
            size_t before = pos - src;
            memcpy(dst, src, before);
            dst += before;

            // Copy replacement
            memcpy(dst, replace->data, replace->length);
            dst += replace->length;

            src = pos + find->length;
        }
        // Copy remainder
        strcpy(dst, src);
    }

    return register_string(rt, result);
}

OmniList* omni_string_split(OmniRuntime* rt, OmniString* str, OmniString* delim) {
    OmniList* list = omni_list_create(rt);
    if (!list || !str || !str->data) return list;

    if (!delim || !delim->data || delim->length == 0) {
        // No delimiter - return single element
        OmniValue v = {.type = VAL_LIST, .as_list = omni_string_create(rt, str->data)};
        omni_list_push(list, v);
        return list;
    }

    const char* start = str->data;
    const char* pos;

    while ((pos = strstr(start, delim->data)) != NULL) {
        OmniString* part = omni_string_substr(rt, str, start - str->data, pos - start);
        OmniValue v = {.type = VAL_STRING};
        v.as_list = part;  // Store string pointer
        omni_list_push(list, v);
        start = pos + delim->length;
    }

    // Add final part
    OmniString* part = omni_string_create(rt, start);
    OmniValue v = {.type = VAL_STRING};
    v.as_list = part;
    omni_list_push(list, v);

    return list;
}

OmniString* omni_string_join(OmniRuntime* rt, OmniList* list, OmniString* delim) {
    if (!list || list->length == 0) return omni_string_create(rt, "");

    // Calculate total length
    uint32_t total = 0;
    for (uint32_t i = 0; i < list->length; i++) {
        OmniString* s = (OmniString*)list->items[i].as_list;
        if (s) total += s->length;
    }
    if (delim && delim->length > 0 && list->length > 1) {
        total += delim->length * (list->length - 1);
    }

    OmniString* result = calloc(1, sizeof(OmniString));
    if (!result) return NULL;

    result->length = total;
    result->capacity = total + 1;
    result->data = malloc(result->capacity);
    result->ref_count = 1;

    if (result->data) {
        char* dst = result->data;
        for (uint32_t i = 0; i < list->length; i++) {
            if (i > 0 && delim && delim->length > 0) {
                memcpy(dst, delim->data, delim->length);
                dst += delim->length;
            }
            OmniString* s = (OmniString*)list->items[i].as_list;
            if (s && s->length > 0) {
                memcpy(dst, s->data, s->length);
                dst += s->length;
            }
        }
        *dst = '\0';
    }

    return register_string(rt, result);
}

const char* omni_string_cstr(OmniString* str) {
    return str ? str->data : "";
}

// # B.3 List Operations [LIST_OPS]

static OmniList* register_list(OmniRuntime* rt, OmniList* list) {
    if (!rt || !list) return list;

    if (rt->list_count >= rt->list_capacity) {
        rt->list_capacity *= 2;
        rt->lists = realloc(rt->lists, rt->list_capacity * sizeof(OmniList*));
    }

    rt->lists[rt->list_count++] = list;
    return list;
}

OmniList* omni_list_create(OmniRuntime* rt) {
    OmniList* list = calloc(1, sizeof(OmniList));
    if (!list) return NULL;

    list->capacity = OMNI_RT_LIST_INIT_SIZE;
    list->items = calloc(list->capacity, sizeof(OmniValue));
    list->length = 0;
    list->ref_count = 1;

    return register_list(rt, list);
}

bool omni_list_push(OmniList* list, OmniValue value) {
    if (!list) return false;

    if (list->length >= list->capacity) {
        list->capacity *= 2;
        list->items = realloc(list->items, list->capacity * sizeof(OmniValue));
        if (!list->items) return false;
    }

    list->items[list->length++] = value;
    return true;
}

bool omni_list_pop(OmniList* list, OmniValue* value) {
    if (!list || list->length == 0) return false;

    list->length--;
    if (value) *value = list->items[list->length];
    return true;
}

bool omni_list_get(OmniList* list, uint32_t index, OmniValue* value) {
    if (!list || index >= list->length) return false;
    if (value) *value = list->items[index];
    return true;
}

bool omni_list_set(OmniList* list, uint32_t index, OmniValue value) {
    if (!list || index >= list->length) return false;
    list->items[index] = value;
    return true;
}

uint32_t omni_list_length(OmniList* list) {
    return list ? list->length : 0;
}

int32_t omni_list_find(OmniList* list, OmniValue value) {
    if (!list) return -1;

    for (uint32_t i = 0; i < list->length; i++) {
        OmniValue* item = &list->items[i];
        if (item->type != value.type) continue;

        bool match = false;
        switch (value.type) {
            case VAL_INT:    match = item->as_int == value.as_int; break;
            case VAL_FLOAT:  match = item->as_float == value.as_float; break;
            case VAL_BOOL:   match = item->as_bool == value.as_bool; break;
            case VAL_TRIT:   match = item->as_trit == value.as_trit; break;
            case VAL_STRING: match = item->as_string == value.as_string; break;
            default: break;
        }
        if (match) return (int32_t)i;
    }

    return -1;
}

void omni_list_clear(OmniList* list) {
    if (list) list->length = 0;
}

// # B.4 Map Operations [MAP_OPS]

static uint32_t hash_string(OmniString* str) {
    if (!str || !str->data) return 0;

    // FNV-1a hash
    uint32_t hash = 2166136261u;
    for (uint32_t i = 0; i < str->length; i++) {
        hash ^= (unsigned char)str->data[i];
        hash *= 16777619u;
    }
    return hash;
}

static OmniMap* register_map(OmniRuntime* rt, OmniMap* map) {
    if (!rt || !map) return map;

    if (rt->map_count >= rt->map_capacity) {
        rt->map_capacity *= 2;
        rt->maps = realloc(rt->maps, rt->map_capacity * sizeof(OmniMap*));
    }

    rt->maps[rt->map_count++] = map;
    return map;
}

OmniMap* omni_map_create(OmniRuntime* rt) {
    OmniMap* map = calloc(1, sizeof(OmniMap));
    if (!map) return NULL;

    map->capacity = OMNI_RT_MAP_INIT_SIZE;
    map->entries = calloc(map->capacity, sizeof(OmniMapEntry));
    map->count = 0;
    map->ref_count = 1;

    return register_map(rt, map);
}

static OmniMapEntry* map_find_entry(OmniMap* map, OmniString* key) {
    if (!map || !key) return NULL;

    uint32_t index = hash_string(key) % map->capacity;
    uint32_t start = index;

    do {
        OmniMapEntry* entry = &map->entries[index];
        if (!entry->occupied) return NULL;
        if (omni_string_equals(entry->key, key)) return entry;
        index = (index + 1) % map->capacity;
    } while (index != start);

    return NULL;
}

bool omni_map_get(OmniMap* map, OmniString* key, OmniValue* value) {
    OmniMapEntry* entry = map_find_entry(map, key);
    if (!entry) return false;
    if (value) *value = entry->value;
    return true;
}

bool omni_map_set(OmniRuntime* rt, OmniMap* map, OmniString* key, OmniValue value) {
    if (!map || !key) return false;

    // Check if key exists
    OmniMapEntry* existing = map_find_entry(map, key);
    if (existing) {
        existing->value = value;
        return true;
    }

    // Grow if needed (keep load factor < 0.75)
    if (map->count * 4 >= map->capacity * 3) {
        uint32_t old_cap = map->capacity;
        OmniMapEntry* old_entries = map->entries;

        map->capacity *= 2;
        map->entries = calloc(map->capacity, sizeof(OmniMapEntry));
        map->count = 0;

        // Rehash all entries
        for (uint32_t i = 0; i < old_cap; i++) {
            if (old_entries[i].occupied) {
                omni_map_set(rt, map, old_entries[i].key, old_entries[i].value);
            }
        }
        free(old_entries);
    }

    // Find empty slot
    uint32_t index = hash_string(key) % map->capacity;
    while (map->entries[index].occupied) {
        index = (index + 1) % map->capacity;
    }

    map->entries[index].key = key;
    map->entries[index].value = value;
    map->entries[index].occupied = true;
    map->count++;

    return true;
}

bool omni_map_has(OmniMap* map, OmniString* key) {
    return map_find_entry(map, key) != NULL;
}

bool omni_map_delete(OmniMap* map, OmniString* key) {
    OmniMapEntry* entry = map_find_entry(map, key);
    if (!entry) return false;

    // Tombstone marking (simple deletion)
    entry->occupied = false;
    entry->key = NULL;
    map->count--;

    return true;
}

OmniList* omni_map_keys(OmniRuntime* rt, OmniMap* map) {
    OmniList* list = omni_list_create(rt);
    if (!list || !map) return list;

    for (uint32_t i = 0; i < map->capacity; i++) {
        if (map->entries[i].occupied && map->entries[i].key) {
            OmniValue v = {.type = VAL_STRING};
            v.as_list = map->entries[i].key;  // Store string pointer
            omni_list_push(list, v);
        }
    }

    return list;
}

uint32_t omni_map_length(OmniMap* map) {
    return map ? map->count : 0;
}

// # B.5 File Operations [FILE_OPS]

static void register_file(OmniRuntime* rt, FILE* f) {
    if (!rt || !f) return;

    if (rt->file_count >= rt->file_capacity) {
        rt->file_capacity *= 2;
        rt->files = realloc(rt->files, rt->file_capacity * sizeof(FILE*));
    }

    rt->files[rt->file_count++] = f;
}

FILE* omni_file_open(OmniRuntime* rt, const char* path, const char* mode) {
    if (!path || !mode) return NULL;

    FILE* f = fopen(path, mode);
    if (f) register_file(rt, f);
    return f;
}

void omni_file_close(OmniRuntime* rt, FILE* f) {
    if (!rt || !f) return;

    // Remove from tracking
    for (uint32_t i = 0; i < rt->file_count; i++) {
        if (rt->files[i] == f) {
            rt->files[i] = rt->files[rt->file_count - 1];
            rt->file_count--;
            break;
        }
    }

    fclose(f);
}

OmniString* omni_file_read(OmniRuntime* rt, FILE* f) {
    if (!f) return omni_string_create(rt, "");

    // Get file size
    long pos = ftell(f);
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, pos, SEEK_SET);

    if (size <= 0) return omni_string_create(rt, "");

    char* buffer = malloc(size + 1);
    if (!buffer) return omni_string_create(rt, "");

    size_t read = fread(buffer, 1, size, f);
    buffer[read] = '\0';

    OmniString* str = omni_string_create(rt, buffer);
    free(buffer);
    return str;
}

bool omni_file_write(FILE* f, OmniString* str) {
    if (!f || !str || !str->data) return false;
    size_t written = fwrite(str->data, 1, str->length, f);
    return written == str->length;
}

OmniString* omni_file_readline(OmniRuntime* rt, FILE* f) {
    if (!f) return NULL;

    char buffer[4096];
    if (!fgets(buffer, sizeof(buffer), f)) {
        return NULL;  // EOF or error
    }

    // Remove trailing newline
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
        if (len > 1 && buffer[len - 2] == '\r') {
            buffer[len - 2] = '\0';
        }
    }

    return omni_string_create(rt, buffer);
}

bool omni_file_writeline(FILE* f, OmniString* str) {
    if (!f) return false;
    if (str && str->data) {
        if (fwrite(str->data, 1, str->length, f) != str->length) return false;
    }
    return fputc('\n', f) != EOF;
}

bool omni_file_exists(const char* path) {
    if (!path) return false;
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

bool omni_dir_exists(const char* path) {
    if (!path) return false;
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

int64_t omni_file_size(const char* path) {
    if (!path) return -1;
    struct stat st;
    if (stat(path, &st) != 0) return -1;
    return st.st_size;
}

bool omni_file_delete(const char* path) {
    if (!path) return false;
    return remove(path) == 0;
}

bool omni_dir_create(const char* path) {
    if (!path) return false;
    return mkdir(path, 0755) == 0;
}

OmniList* omni_dir_list(OmniRuntime* rt, const char* path) {
    OmniList* list = omni_list_create(rt);
    if (!list || !path) return list;

    DIR* dir = opendir(path);
    if (!dir) return list;

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        // Skip . and ..
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        OmniString* name = omni_string_create(rt, entry->d_name);
        OmniValue v = {.type = VAL_STRING};
        v.as_list = name;
        omni_list_push(list, v);
    }

    closedir(dir);
    return list;
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// "Let the earth bring forth... whose seed is in itself."
//   — Genesis 1:11
//
// Dynamic growth follows established patterns.

// =============================================================================
// END CLOSING
// =============================================================================
