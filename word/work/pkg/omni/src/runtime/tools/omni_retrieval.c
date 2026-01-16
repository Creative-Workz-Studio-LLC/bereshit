// #!omni code --c -library
// #!omni meta.key = B-word-work-pkg-omni-retrieval-impl
// #!omni meta.from = word/seed/code/c/source.c
// #!omni meta.at = a-01.00
//
// omni_retrieval.c — Knowledge Retrieval Layer Implementation
// Connect mind to filesystem - Scripture, glossary, knowledge

#define _GNU_SOURCE

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// Key: B-word-work-pkg-omni-retrieval-impl
// Grounding: Psalm 119:11 - "Thy word have I hid in mine heart"

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "omni_retrieval.h"
#include "jsonc.h"  // Cornerstone util
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/stat.h>

// Scripture index cache (loaded once, used many times)
static ScriptureEntry* g_kjv_index = NULL;
static int32_t g_kjv_count = 0;
static ScriptureEntry* g_web_index = NULL;
static int32_t g_web_count = 0;
static bool g_indexes_loaded = false;

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 Helper Functions
// -----------------------------------------------------------------------------

// Trim whitespace from string
static char* trim(char* str) {
    if (!str) return str;
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return str;
    char* end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    return str;
}

// Safe copy with explicit null termination (truncation is intentional)
static void safe_copy(char* dest, size_t dest_size, const char* src) {
    if (!dest || !src || dest_size == 0) return;
    size_t src_len = strlen(src);
    size_t copy_len = (src_len < dest_size - 1) ? src_len : dest_size - 1;
    memcpy(dest, src, copy_len);
    dest[copy_len] = '\0';
}

// Safe append with explicit bounds checking (truncation is intentional)
static void safe_append(char* dest, size_t dest_size, const char* src) {
    if (!dest || !src || dest_size == 0) return;
    size_t dest_len = strlen(dest);
    if (dest_len >= dest_size - 1) return;  // Already full
    size_t space = dest_size - dest_len - 1;
    size_t src_len = strlen(src);
    size_t copy_len = (src_len < space) ? src_len : space;
    memcpy(dest + dest_len, src, copy_len);
    dest[dest_len + copy_len] = '\0';
}

// Check if file exists
bool retrieval_path_exists(const char* path) {
    struct stat st;
    return stat(path, &st) == 0;
}

// -----------------------------------------------------------------------------
// B.1b JSON Helpers (new DOM API)
// -----------------------------------------------------------------------------

static void copy_json_string(const JsonValue* val, char* buf, size_t size) {
    if (!val || !buf || size == 0) return;
    const char* str = jsonc_get_string(val);
    if (str) {
        size_t len = strlen(str);
        if (len >= size) len = size - 1;
        memcpy(buf, str, len);
        buf[len] = '\0';
    }
}

static int32_t get_json_int(const JsonValue* val) {
    if (!val) return 0;
    return (int32_t)jsonc_get_int(val, 0);
}

// -----------------------------------------------------------------------------
// B.1c Scripture Index Loading
// -----------------------------------------------------------------------------

// Load scripture index from JSON file
// Returns allocated array of ScriptureEntry, sets count
ScriptureEntry* scripture_index_load(const char* bereshit_root, const char* version, int32_t* count) {
    if (!bereshit_root || !version || !count) return NULL;
    *count = 0;

    // Build path to scripture-key.json
    char path[MAX_PATH_LENGTH];
    snprintf(path, sizeof(path), "%s/word/scripture/%s-scripture-key.json",
             bereshit_root, version[0] == 'K' ? "kjv" : "web");

    // Load JSON file (new DOM API)
    JsonValue* root = jsonc_load(path);
    if (!root) {
        fprintf(stderr, "[RETRIEVAL] Failed to load scripture index: %s\n", path);
        return NULL;
    }

    // Find entries array
    JsonValue* entries = jsonc_object_get(root, "entries");
    if (!entries) {
        fprintf(stderr, "[RETRIEVAL] No 'entries' array in scripture index\n");
        jsonc_free(root);
        return NULL;
    }

    // Count entries
    int32_t entry_count = jsonc_array_size(entries);
    if (entry_count <= 0) {
        jsonc_free(root);
        return NULL;
    }

    // Allocate array
    ScriptureEntry* index = calloc(entry_count, sizeof(ScriptureEntry));
    if (!index) {
        jsonc_free(root);
        return NULL;
    }

    // Parse each entry
    for (int32_t i = 0; i < entry_count; i++) {
        JsonValue* entry = jsonc_array_get(entries, i);
        if (!entry) continue;

        index[i].ordinal = get_json_int(jsonc_object_get(entry, "ordinal"));
        copy_json_string(jsonc_object_get(entry, "book"), index[i].book, sizeof(index[i].book));
        index[i].chapter = get_json_int(jsonc_object_get(entry, "chapter"));
        index[i].verse = get_json_int(jsonc_object_get(entry, "verse"));
        copy_json_string(jsonc_object_get(entry, "text"), index[i].text, sizeof(index[i].text));
        copy_json_string(jsonc_object_get(entry, "testament"), index[i].testament, sizeof(index[i].testament));
        index[i].cube_pos = get_json_int(jsonc_object_get(entry, "cube"));
    }

    jsonc_free(root);
    *count = entry_count;
    fprintf(stderr, "[RETRIEVAL] Loaded %d entries from %s\n", entry_count, path);
    return index;
}

// Free scripture index
void scripture_index_free(ScriptureEntry* index) {
    if (index) free(index);
}

// Ensure indexes are loaded (lazy loading)
static void ensure_indexes_loaded(const char* bereshit_root) {
    if (g_indexes_loaded) return;

    // Load KJV index
    if (!g_kjv_index) {
        g_kjv_index = scripture_index_load(bereshit_root, "KJV", &g_kjv_count);
    }

    // Load WEB index
    if (!g_web_index) {
        g_web_index = scripture_index_load(bereshit_root, "WEB", &g_web_count);
    }

    g_indexes_loaded = true;
}

// Lookup by reference in loaded index
const ScriptureEntry* scripture_index_lookup(
    const ScriptureEntry* index, int32_t count,
    const char* book, int32_t chapter, int32_t verse
) {
    if (!index || count <= 0 || !book) return NULL;

    // Linear search (could be binary search if sorted by ordinal)
    for (int32_t i = 0; i < count; i++) {
        if (index[i].chapter == chapter &&
            index[i].verse == verse &&
            strcasecmp(index[i].book, book) == 0) {
            return &index[i];
        }
    }
    return NULL;
}

// Lookup by cube position
const ScriptureEntry** scripture_index_by_cube(
    const ScriptureEntry* index, int32_t count,
    int32_t cube_pos, int32_t* result_count
) {
    if (!index || count <= 0 || !result_count) return NULL;
    *result_count = 0;

    // Count matching entries
    int32_t matches = 0;
    for (int32_t i = 0; i < count; i++) {
        if (index[i].cube_pos == cube_pos) matches++;
    }

    if (matches == 0) return NULL;

    // Allocate result array
    const ScriptureEntry** results = malloc(matches * sizeof(ScriptureEntry*));
    if (!results) return NULL;

    // Fill result array
    int32_t j = 0;
    for (int32_t i = 0; i < count && j < matches; i++) {
        if (index[i].cube_pos == cube_pos) {
            results[j++] = &index[i];
        }
    }

    *result_count = j;
    return results;
}

// Get ordinal from reference (simplified - returns 0 if not found)
int32_t scripture_ordinal(const char* book, int32_t chapter, int32_t verse) {
    if (!g_kjv_index) return 0;
    const ScriptureEntry* entry = scripture_index_lookup(g_kjv_index, g_kjv_count, book, chapter, verse);
    return entry ? entry->ordinal : 0;
}

// -----------------------------------------------------------------------------
// B.2 Scripture Parsing
// -----------------------------------------------------------------------------

// Parse scripture reference like "John 3:16" or "1 John 2:3-4"
ScriptureRef retrieval_parse_scripture_ref(const char* ref_str) {
    ScriptureRef ref = {0};
    ref.valid = false;

    if (!ref_str || !*ref_str) return ref;

    // Copy to work buffer
    char buf[256];
    strncpy(buf, ref_str, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';
    char* p = trim(buf);

    // Handle books starting with numbers (1 John, 2 Kings, etc.)
    char* book_end = p;
    if (isdigit((unsigned char)*p)) {
        // Skip the number and space
        while (*book_end && !isspace((unsigned char)*book_end)) book_end++;
        while (*book_end && isspace((unsigned char)*book_end)) book_end++;
    }
    // Find where book name ends (at the chapter number)
    while (*book_end && !isdigit((unsigned char)*book_end)) book_end++;

    if (book_end == p) return ref;  // No book name found

    // Extract book name
    size_t book_len = book_end - p;
    if (book_len >= sizeof(ref.book)) book_len = sizeof(ref.book) - 1;
    strncpy(ref.book, p, book_len);
    ref.book[book_len] = '\0';
    // Trim trailing spaces from book name
    char* be = ref.book + strlen(ref.book) - 1;
    while (be > ref.book && isspace((unsigned char)*be)) *be-- = '\0';

    // Parse chapter:verse
    p = book_end;
    if (*p) {
        ref.chapter = atoi(p);
        if (ref.chapter > 0) {
            // Find colon for verse
            char* colon = strchr(p, ':');
            if (colon) {
                ref.verse_start = atoi(colon + 1);
                // Check for range (e.g., 16-17)
                char* dash = strchr(colon, '-');
                if (dash) {
                    ref.verse_end = atoi(dash + 1);
                } else {
                    ref.verse_end = ref.verse_start;
                }
                ref.valid = (ref.verse_start > 0);
            } else {
                // Chapter only, no verse
                ref.verse_start = 0;
                ref.verse_end = 0;
                ref.valid = true;
            }
        }
    }

    return ref;
}

// -----------------------------------------------------------------------------
// B.3 Scripture Retrieval
// -----------------------------------------------------------------------------

// Normalize book name to match filesystem directory names
// Handles common variations: Psalm→Psalms, etc.
static void normalize_book_name(char* book, size_t size) {
    if (!book || size == 0) return;

    // Psalm → Psalms
    if (strcmp(book, "Psalm") == 0) {
        strncpy(book, "Psalms", size - 1);
        book[size - 1] = '\0';
    }
    // Add more normalizations as needed
}

// Get scripture content - uses cached index if available, falls back to files
// Format: word/scripture/{VERSION}/{Book}/Chapter_{N}/Verse_{M}.txt
RetrievalResult retrieval_get_scripture_version(const char* bereshit_root, ScriptureRef* ref, const char* version) {
    RetrievalResult result = {0};
    result.found = false;

    if (!bereshit_root || !ref || !ref->valid) return result;
    if (!version) version = "KJV";  // Default to KJV

    // Ensure indexes are loaded (lazy loading, happens once)
    ensure_indexes_loaded(bereshit_root);

    // Try cached index first (fast path)
    ScriptureEntry* index = (version[0] == 'K') ? g_kjv_index : g_web_index;
    int32_t count = (version[0] == 'K') ? g_kjv_count : g_web_count;

    if (index && count > 0) {
        // Normalize book name for index lookup
        char book_normalized[64];
        strncpy(book_normalized, ref->book, sizeof(book_normalized) - 1);
        book_normalized[sizeof(book_normalized) - 1] = '\0';
        normalize_book_name(book_normalized, sizeof(book_normalized));

        // Single verse or range?
        int verse_start = ref->verse_start;
        int verse_end = (ref->verse_end > 0) ? ref->verse_end : ref->verse_start;

        result.content[0] = '\0';
        for (int v = verse_start; v <= verse_end; v++) {
            const ScriptureEntry* entry = scripture_index_lookup(index, count, book_normalized, ref->chapter, v);
            if (entry) {
                if (result.content[0] != '\0') {
                    safe_append(result.content, sizeof(result.content), " ");
                }
                safe_append(result.content, sizeof(result.content), entry->text);
                result.found = true;
            }
        }

        if (result.found) {
            if (verse_end > verse_start) {
                snprintf(result.reference, sizeof(result.reference),
                         "%s %d:%d-%d (%s)", ref->book, ref->chapter, verse_start, verse_end, version);
            } else {
                snprintf(result.reference, sizeof(result.reference),
                         "%s %d:%d (%s)", ref->book, ref->chapter, verse_start, version);
            }
            snprintf(result.path, sizeof(result.path), "[from cached index]");
            return result;
        }
    }

    // Fall back to file-based retrieval
    // Format book name: spaces become underscores
    char book_name[64];
    strncpy(book_name, ref->book, sizeof(book_name) - 1);
    book_name[sizeof(book_name) - 1] = '\0';

    // Normalize book name (Psalm → Psalms, etc.)
    normalize_book_name(book_name, sizeof(book_name));

    // Replace spaces with underscores
    for (char* c = book_name; *c; c++) {
        if (*c == ' ') *c = '_';
    }

    // Build chapter directory path
    char chapter_dir[MAX_PATH_LENGTH];
    snprintf(chapter_dir, sizeof(chapter_dir),
             "%s/word/scripture/%s/%s/Chapter_%d",
             bereshit_root, version, book_name, ref->chapter);

    // Check if directory exists
    if (!retrieval_path_exists(chapter_dir)) {
        return result;  // Chapter not found
    }

    // Determine verse range
    int verse_start = ref->verse_start;
    int verse_end = (ref->verse_end > 0) ? ref->verse_end : ref->verse_start;

    // Read each verse file in range
    result.content[0] = '\0';
    char verse_path[MAX_PATH_LENGTH + 32];  // Extra room for /Verse_N.txt
    char line[1024];

    for (int v = verse_start; v <= verse_end; v++) {
        snprintf(verse_path, sizeof(verse_path),
                 "%s/Verse_%d.txt", chapter_dir, v);

        FILE* f = fopen(verse_path, "r");
        if (f) {
            // Read verse content (usually single line)
            if (fgets(line, sizeof(line), f)) {
                char* content = trim(line);
                // Skip BOM if present
                if ((unsigned char)content[0] == 0xEF &&
                    (unsigned char)content[1] == 0xBB &&
                    (unsigned char)content[2] == 0xBF) {
                    content += 3;
                }

                // Add space between verses if not first
                if (result.content[0] != '\0') {
                    safe_append(result.content, sizeof(result.content), " ");
                }
                safe_append(result.content, sizeof(result.content), content);
            }
            fclose(f);

            // Mark path of first verse found
            if (result.path[0] == '\0') {
                safe_copy(result.path, sizeof(result.path), verse_path);
            }
        }
    }

    // Set result if we found content
    if (result.content[0] != '\0') {
        result.found = true;
        if (verse_end > verse_start) {
            snprintf(result.reference, sizeof(result.reference),
                     "%s %d:%d-%d (%s)", ref->book, ref->chapter, verse_start, verse_end, version);
        } else {
            snprintf(result.reference, sizeof(result.reference),
                     "%s %d:%d (%s)", ref->book, ref->chapter, verse_start, version);
        }
    }

    return result;
}

// Get scripture content (default KJV)
RetrievalResult retrieval_get_scripture(const char* bereshit_root, ScriptureRef* ref) {
    return retrieval_get_scripture_version(bereshit_root, ref, "KJV");
}

// Convenience function
RetrievalResult retrieval_scripture(const char* bereshit_root, const char* ref_str) {
    ScriptureRef ref = retrieval_parse_scripture_ref(ref_str);
    return retrieval_get_scripture(bereshit_root, &ref);
}

// -----------------------------------------------------------------------------
// B.4 Glossary Retrieval
// -----------------------------------------------------------------------------

RetrievalResult retrieval_get_definition(const char* bereshit_root, const char* term) {
    RetrievalResult result = {0};
    result.found = false;

    if (!bereshit_root || !term) return result;

    // Try various paths in glossary
    const char* categories[] = {"biblical", "technical", "paradigm", NULL};

    for (int i = 0; categories[i]; i++) {
        char path[MAX_PATH_LENGTH];
        snprintf(path, sizeof(path), "%s/word/glossary/%s/%s.adoc",
                 bereshit_root, categories[i], term);

        FILE* f = fopen(path, "r");
        if (f) {
            // Read first meaningful lines for definition
            char line[512];
            bool found_def = false;

            while (fgets(line, sizeof(line), f)) {
                char* p = trim(line);
                // Skip headers and empty lines
                if (*p == '=' || *p == '[' || *p == '\0' || *p == '/') continue;

                // First substantial line is the definition
                if (strlen(p) > 10) {
                    safe_copy(result.content, sizeof(result.content), p);
                    result.found = true;
                    found_def = true;
                    break;
                }
            }
            fclose(f);

            if (found_def) {
                snprintf(result.reference, sizeof(result.reference), "%s", term);
                safe_copy(result.path, sizeof(result.path), path);
                return result;
            }
        }
    }

    return result;
}

RetrievalResult retrieval_search_glossary(const char* bereshit_root, const char* query) {
    // For now, just try exact match
    return retrieval_get_definition(bereshit_root, query);
}

// -----------------------------------------------------------------------------
// B.5 General File Retrieval
// -----------------------------------------------------------------------------

RetrievalResult retrieval_read_file(const char* path) {
    RetrievalResult result = {0};
    result.found = false;

    if (!path) return result;

    FILE* f = fopen(path, "r");
    if (!f) return result;

    size_t total = 0;
    char line[512];
    result.content[0] = '\0';

    while (fgets(line, sizeof(line), f) && total < sizeof(result.content) - 1) {
        size_t len = strlen(line);
        if (total + len >= sizeof(result.content) - 1) break;
        strcat(result.content, line);
        total += len;
    }
    fclose(f);

    result.found = true;
    safe_copy(result.path, sizeof(result.path), path);
    return result;
}

// -----------------------------------------------------------------------------
// B.6 Query Parsing
// -----------------------------------------------------------------------------

ParsedQuery retrieval_parse_query(const char* query) {
    ParsedQuery pq = {0};
    pq.type = QUERY_UNKNOWN;

    if (!query) return pq;

    // Check for scripture request patterns
    // "Quote John 3:16", "Read John 3:16", "What does Genesis 1:1 say?"
    const char* scripture_triggers[] = {
        "quote ", "read ", "scripture ", "verse ", "bible ",
        "what does ", "what did ", NULL
    };

    char lower[256];
    safe_copy(lower, sizeof(lower), query);
    for (char* c = lower; *c; c++) *c = tolower((unsigned char)*c);

    // Find and skip past scripture trigger words
    const char* ref_start = query;
    for (int i = 0; scripture_triggers[i]; i++) {
        char* found = strstr(lower, scripture_triggers[i]);
        if (found) {
            // Move past the trigger in the original query
            ref_start = query + (found - lower) + strlen(scripture_triggers[i]);
            break;
        }
    }

    // Skip any remaining whitespace
    while (*ref_start && isspace((unsigned char)*ref_start)) ref_start++;

    // Remove trailing " say?" or " say" if present
    char ref_buf[256];
    safe_copy(ref_buf, sizeof(ref_buf), ref_start);
    char* say_pos = strstr(ref_buf, " say");
    if (say_pos) *say_pos = '\0';

    // Try to parse as scripture reference
    ScriptureRef ref = retrieval_parse_scripture_ref(ref_buf);
    if (ref.valid) {
        pq.type = QUERY_SCRIPTURE;
        pq.scripture = ref;
        snprintf(pq.subject, sizeof(pq.subject), "%s %d:%d",
                 ref.book, ref.chapter, ref.verse_start);
        return pq;
    }

    // Check for definition request patterns
    // "What is X?", "Define X", "What does X mean?"
    const char* def_triggers[] = {"what is", "what's", "define", "meaning of", "what does", NULL};

    for (int i = 0; def_triggers[i]; i++) {
        char* found = strstr(lower, def_triggers[i]);
        if (found) {
            pq.type = QUERY_DEFINITION;
            // Extract subject after trigger
            const char* start = query + (found - lower) + strlen(def_triggers[i]);
            while (*start && isspace((unsigned char)*start)) start++;
            strncpy(pq.subject, start, sizeof(pq.subject) - 1);
            // Remove trailing punctuation
            char* end = pq.subject + strlen(pq.subject) - 1;
            while (end > pq.subject && (*end == '?' || *end == '.' || isspace((unsigned char)*end))) {
                *end-- = '\0';
            }
            return pq;
        }
    }

    // Default: search query
    pq.type = QUERY_SEARCH;
    strncpy(pq.subject, query, sizeof(pq.subject) - 1);
    return pq;
}

// -----------------------------------------------------------------------------
// B.7 High-Level Retrieval
// -----------------------------------------------------------------------------

RetrievalResult retrieval_query(const char* bereshit_root, const char* query) {
    ParsedQuery pq = retrieval_parse_query(query);

    switch (pq.type) {
        case QUERY_SCRIPTURE:
            return retrieval_get_scripture(bereshit_root, &pq.scripture);

        case QUERY_DEFINITION:
            return retrieval_get_definition(bereshit_root, pq.subject);

        default:
            // Try scripture first, then glossary
            {
                ScriptureRef ref = retrieval_parse_scripture_ref(query);
                if (ref.valid) {
                    return retrieval_get_scripture(bereshit_root, &ref);
                }
            }
            return retrieval_get_definition(bereshit_root, pq.subject);
    }
}

// -----------------------------------------------------------------------------
// B.7 Cube Index Implementation
// -----------------------------------------------------------------------------

// Load cube index from JSON file
CubeIndex* cube_index_load(const char* bereshit_root, const char* version) {
    if (!bereshit_root || !version) return NULL;

    CubeIndex* idx = calloc(1, sizeof(CubeIndex));
    if (!idx) return NULL;

    safe_copy(idx->version, sizeof(idx->version), version);

    // Build path to cube-index.json
    char path[MAX_PATH_LENGTH];
    snprintf(path, sizeof(path), "%s/word/scripture/%s-cube-index.json",
             bereshit_root, version[0] == 'K' ? "kjv" : "web");

    // Load JSON file (new DOM API)
    JsonValue* root = jsonc_load(path);
    if (!root) {
        fprintf(stderr, "[RETRIEVAL] Failed to load cube index: %s\n", path);
        free(idx);
        return NULL;
    }

    // Find cube array (3D array structure)
    JsonValue* cube_arr = jsonc_object_get(root, "cube");
    if (!cube_arr) {
        fprintf(stderr, "[RETRIEVAL] No 'cube' array in cube index\n");
        jsonc_free(root);
        free(idx);
        return NULL;
    }

    // Parse the 3D structure: cube[x][y][z] where each leaf is an array of verses
    int32_t pos = 0;
    int x_count = jsonc_array_size(cube_arr);
    for (int xi = 0; xi < x_count && pos < 27; xi++) {
        JsonValue* x_arr = jsonc_array_get(cube_arr, xi);
        if (!x_arr) continue;

        int y_count = jsonc_array_size(x_arr);
        for (int yi = 0; yi < y_count && pos < 27; yi++) {
            JsonValue* y_arr = jsonc_array_get(x_arr, yi);
            if (!y_arr) continue;

            int z_count = jsonc_array_size(y_arr);
            for (int zi = 0; zi < z_count && pos < 27; zi++) {
                JsonValue* z_arr = jsonc_array_get(y_arr, zi);
                if (!z_arr) { pos++; continue; }

                // z_arr is an array of verse objects at this position
                int32_t verse_count = jsonc_array_size(z_arr);
                if (verse_count > 0) {
                    idx->positions[pos].verses = calloc(verse_count, sizeof(ScriptureEntry*));
                    idx->positions[pos].count = 0;

                    // Parse each verse in this position
                    for (int32_t v = 0; v < verse_count; v++) {
                        JsonValue* verse = jsonc_array_get(z_arr, v);
                        if (!verse) continue;

                        ScriptureEntry* entry = calloc(1, sizeof(ScriptureEntry));
                        if (entry) {
                            entry->ordinal = get_json_int(jsonc_object_get(verse, "ordinal"));
                            copy_json_string(jsonc_object_get(verse, "book"), entry->book, sizeof(entry->book));
                            entry->chapter = get_json_int(jsonc_object_get(verse, "chapter"));
                            entry->verse = get_json_int(jsonc_object_get(verse, "verse"));
                            copy_json_string(jsonc_object_get(verse, "text"), entry->text, sizeof(entry->text));
                            entry->cube_pos = get_json_int(jsonc_object_get(verse, "cube"));

                            idx->positions[pos].verses[idx->positions[pos].count++] = entry;
                        }
                    }
                }
                pos++;
            }
        }
    }

    jsonc_free(root);
    idx->loaded = true;
    fprintf(stderr, "[RETRIEVAL] Loaded cube index: %s (%d positions)\n", version, pos);
    return idx;
}

// Free cube index
void cube_index_free(CubeIndex* idx) {
    if (!idx) return;
    for (int i = 0; i < 27; i++) {
        if (idx->positions[i].verses) {
            for (int32_t v = 0; v < idx->positions[i].count; v++) {
                free((void*)idx->positions[i].verses[v]);
            }
            free(idx->positions[i].verses);
        }
    }
    free(idx);
}

// Get verses at cube position
const ScriptureEntry** cube_index_at_position(
    const CubeIndex* idx, int32_t cube_pos, int32_t* count
) {
    if (!idx || !idx->loaded || cube_pos < 0 || cube_pos >= 27) {
        if (count) *count = 0;
        return NULL;
    }
    if (count) *count = idx->positions[cube_pos].count;
    return (const ScriptureEntry**)idx->positions[cube_pos].verses;
}

// Get verses at xyz coordinates
const ScriptureEntry** cube_index_at_xyz(
    const CubeIndex* idx, int x, int y, int z, int32_t* count
) {
    int32_t pos = cube_pos_from_xyz(x, y, z);
    return cube_index_at_position(idx, pos, count);
}

// -----------------------------------------------------------------------------
// B.8 ASCII Index Implementation
// -----------------------------------------------------------------------------

// Load ASCII index from JSON
AsciiIndex* ascii_index_load(const char* bereshit_root) {
    if (!bereshit_root) return NULL;

    AsciiIndex* idx = calloc(1, sizeof(AsciiIndex));
    if (!idx) return NULL;

    // Build path to ascii-key.json (same for all versions)
    char path[MAX_PATH_LENGTH];
    snprintf(path, sizeof(path), "%s/word/scripture/kjv-ascii-key.json", bereshit_root);

    // Load JSON file (new DOM API)
    JsonValue* root = jsonc_load(path);
    if (!root) {
        fprintf(stderr, "[RETRIEVAL] Failed to load ASCII index: %s\n", path);
        free(idx);
        return NULL;
    }

    // Find entries array
    JsonValue* entries = jsonc_object_get(root, "entries");
    if (!entries) {
        fprintf(stderr, "[RETRIEVAL] No 'entries' array in ASCII index\n");
        jsonc_free(root);
        free(idx);
        return NULL;
    }

    // Parse each entry
    int32_t entry_count = jsonc_array_size(entries);
    if (entry_count > 256) entry_count = 256;

    for (int32_t i = 0; i < entry_count; i++) {
        JsonValue* entry = jsonc_array_get(entries, i);
        if (!entry) continue;

        AsciiEntry* ae = &idx->entries[i];

        ae->ascii = get_json_int(jsonc_object_get(entry, "ascii"));

        JsonValue* char_val = jsonc_object_get(entry, "char");
        const char* char_str = jsonc_get_string(char_val);
        if (char_str && char_str[0]) ae->character = char_str[0];

        ae->printable = jsonc_get_bool(jsonc_object_get(entry, "printable"), false);
        ae->trit_value = get_json_int(jsonc_object_get(entry, "trite_value"));
        ae->cube_x = get_json_int(jsonc_object_get(entry, "cube_x"));
        ae->cube_y = get_json_int(jsonc_object_get(entry, "cube_y"));
        ae->cube_z = get_json_int(jsonc_object_get(entry, "cube_z"));
        ae->cube_pos = get_json_int(jsonc_object_get(entry, "cube_pos"));
    }

    jsonc_free(root);
    idx->loaded = true;
    fprintf(stderr, "[RETRIEVAL] Loaded ASCII index: %d entries\n", entry_count);
    return idx;
}

// Free ASCII index
void ascii_index_free(AsciiIndex* idx) {
    if (idx) free(idx);
}

// Get cube position for character
int32_t ascii_to_cube_pos(const AsciiIndex* idx, char c) {
    if (!idx || !idx->loaded) return -1;
    unsigned char uc = (unsigned char)c;
    return idx->entries[uc].cube_pos;
}

// Get character at cube position (reverse lookup - first match)
char cube_pos_to_ascii(const AsciiIndex* idx, int32_t pos) {
    if (!idx || !idx->loaded || pos < 0 || pos >= 27) return '\0';
    for (int i = 0; i < 256; i++) {
        if (idx->entries[i].cube_pos == pos && idx->entries[i].printable) {
            return idx->entries[i].character;
        }
    }
    return '\0';
}

// -----------------------------------------------------------------------------
// B.9 Glossary Index Implementation
// -----------------------------------------------------------------------------

// Load a single glossary entry from JSONC file
static bool load_glossary_entry(const char* path, GlossaryEntry* entry) {
    JsonValue* root = jsonc_load(path);
    if (!root) return false;

    memset(entry, 0, sizeof(*entry));

    // Parse metadata
    JsonValue* meta = jsonc_object_get(root, "metadata");
    if (meta) {
        copy_json_string(jsonc_object_get(meta, "key"), entry->key, sizeof(entry->key));
        copy_json_string(jsonc_object_get(meta, "term"), entry->term, sizeof(entry->term));
        copy_json_string(jsonc_object_get(meta, "hebrew"), entry->hebrew, sizeof(entry->hebrew));
        copy_json_string(jsonc_object_get(meta, "transliteration"), entry->transliteration, sizeof(entry->transliteration));
        copy_json_string(jsonc_object_get(meta, "category"), entry->category, sizeof(entry->category));
        copy_json_string(jsonc_object_get(meta, "scripture_reference"), entry->scripture_ref, sizeof(entry->scripture_ref));
    }

    // Parse definition
    copy_json_string(jsonc_object_get(root, "definition"), entry->definition, sizeof(entry->definition));
    copy_json_string(jsonc_object_get(root, "literal_meaning"), entry->literal_meaning, sizeof(entry->literal_meaning));

    // Parse scripture if present
    JsonValue* scripture = jsonc_object_get(root, "scripture");
    if (scripture) {
        copy_json_string(jsonc_object_get(scripture, "kjv"), entry->scripture_kjv, sizeof(entry->scripture_kjv));
        copy_json_string(jsonc_object_get(scripture, "web"), entry->scripture_web, sizeof(entry->scripture_web));
    }

    // Parse see_also array as comma-separated string
    JsonValue* see_also = jsonc_object_get(root, "see_also");
    if (see_also) {
        entry->see_also[0] = '\0';
        int count = jsonc_array_size(see_also);
        for (int i = 0; i < count; i++) {
            const char* term = jsonc_get_string(jsonc_array_get(see_also, i));
            if (term) {
                if (i > 0) {
                    safe_append(entry->see_also, sizeof(entry->see_also), ", ");
                }
                safe_append(entry->see_also, sizeof(entry->see_also), term);
            }
        }
    }

    jsonc_free(root);
    return entry->term[0] != '\0';
}

// Load entire glossary from JSONC files
GlossaryIndex* glossary_index_load(const char* bereshit_root) {
    if (!bereshit_root) return NULL;

    GlossaryIndex* idx = calloc(1, sizeof(GlossaryIndex));
    if (!idx) return NULL;

    // Initial capacity
    idx->capacity = 128;
    idx->entries = calloc(idx->capacity, sizeof(GlossaryEntry));
    if (!idx->entries) {
        free(idx);
        return NULL;
    }

    fprintf(stderr, "[RETRIEVAL] Loading glossary index...\n");

    // Load index.jsonc to get categories and terms
    char index_path[MAX_PATH_LENGTH];
    snprintf(index_path, sizeof(index_path), "%s/word/glossary/index.jsonc", bereshit_root);

    JsonValue* root = jsonc_load(index_path);
    if (!root) {
        fprintf(stderr, "[RETRIEVAL] Failed to load glossary index\n");
        free(idx->entries);
        free(idx);
        return NULL;
    }

    // Parse categories
    JsonValue* categories = jsonc_object_get(root, "categories");
    if (!categories) {
        jsonc_free(root);
        free(idx->entries);
        free(idx);
        return NULL;
    }

    // Iterate through categories: biblical, paradigm, technical, operations
    const char* cat_names[] = {"biblical", "paradigm", "technical", "operations", NULL};

    for (int c = 0; cat_names[c]; c++) {
        JsonValue* cat = jsonc_object_get(categories, cat_names[c]);
        if (!cat) continue;

        // Get terms array
        JsonValue* terms = jsonc_object_get(cat, "terms");
        if (!terms) continue;

        // Load each term's JSONC file
        int term_count = jsonc_array_size(terms);
        for (int t = 0; t < term_count; t++) {
            const char* term_name = jsonc_get_string(jsonc_array_get(terms, t));
            if (!term_name) continue;

            // Build path to term's JSONC file
            char term_path[MAX_PATH_LENGTH];
            snprintf(term_path, sizeof(term_path), "%s/word/glossary/%s/%s.jsonc",
                     bereshit_root, cat_names[c], term_name);

            // Load entry if file exists
            if (retrieval_path_exists(term_path)) {
                if (idx->count >= idx->capacity) {
                    idx->capacity *= 2;
                    idx->entries = realloc(idx->entries, idx->capacity * sizeof(GlossaryEntry));
                }
                if (load_glossary_entry(term_path, &idx->entries[idx->count])) {
                    idx->count++;
                }
            }
        }
    }

    jsonc_free(root);
    idx->loaded = true;
    fprintf(stderr, "[RETRIEVAL] Loaded glossary: %d terms\n", idx->count);
    return idx;
}

// Free glossary index
void glossary_index_free(GlossaryIndex* idx) {
    if (!idx) return;
    if (idx->entries) free(idx->entries);
    free(idx);
}

// Lookup term by name (exact match, case insensitive)
const GlossaryEntry* glossary_lookup(const GlossaryIndex* idx, const char* term) {
    if (!idx || !idx->loaded || !term) return NULL;

    for (int32_t i = 0; i < idx->count; i++) {
        if (strcasecmp(idx->entries[i].term, term) == 0) {
            return &idx->entries[i];
        }
    }
    return NULL;
}

// Search glossary (fuzzy/partial match)
const GlossaryEntry* glossary_search(const GlossaryIndex* idx, const char* query) {
    if (!idx || !idx->loaded || !query) return NULL;

    // First try exact match
    const GlossaryEntry* exact = glossary_lookup(idx, query);
    if (exact) return exact;

    // Then try partial match in term or definition
    char lower_query[256];
    safe_copy(lower_query, sizeof(lower_query), query);
    for (char* c = lower_query; *c; c++) *c = tolower((unsigned char)*c);

    for (int32_t i = 0; i < idx->count; i++) {
        char lower_term[64];
        safe_copy(lower_term, sizeof(lower_term), idx->entries[i].term);
        for (char* c = lower_term; *c; c++) *c = tolower((unsigned char)*c);

        if (strstr(lower_term, lower_query)) {
            return &idx->entries[i];
        }

        char lower_def[512];
        safe_copy(lower_def, sizeof(lower_def), idx->entries[i].definition);
        for (char* c = lower_def; *c; c++) *c = tolower((unsigned char)*c);

        if (strstr(lower_def, lower_query)) {
            return &idx->entries[i];
        }
    }
    return NULL;
}

// -----------------------------------------------------------------------------
// B.10 Unified Knowledge System Implementation
// -----------------------------------------------------------------------------

// Initialize the full knowledge system (Scripture + Glossary = Mind's Memory)
KnowledgeSystem* knowledge_system_init(const char* bereshit_root) {
    if (!bereshit_root) return NULL;

    KnowledgeSystem* sys = calloc(1, sizeof(KnowledgeSystem));
    if (!sys) return NULL;

    fprintf(stderr, "[RETRIEVAL] Initializing Knowledge System...\n");

    // Load KJV entries
    sys->kjv_entries = scripture_index_load(bereshit_root, "KJV", &sys->kjv_count);

    // Load WEB entries
    sys->web_entries = scripture_index_load(bereshit_root, "WEB", &sys->web_count);

    // Load cube indexes
    sys->kjv_cube = cube_index_load(bereshit_root, "KJV");
    sys->web_cube = cube_index_load(bereshit_root, "WEB");

    // Load ASCII index
    sys->ascii = ascii_index_load(bereshit_root);

    // Load glossary
    sys->glossary = glossary_index_load(bereshit_root);

    sys->initialized = true;
    fprintf(stderr, "[RETRIEVAL] Knowledge System initialized: KJV=%d, WEB=%d verses, %d glossary terms\n",
            sys->kjv_count, sys->web_count, sys->glossary ? sys->glossary->count : 0);
    return sys;
}

// Shutdown and free all resources
void knowledge_system_shutdown(KnowledgeSystem* sys) {
    if (!sys) return;

    scripture_index_free(sys->kjv_entries);
    scripture_index_free(sys->web_entries);
    cube_index_free(sys->kjv_cube);
    cube_index_free(sys->web_cube);
    ascii_index_free(sys->ascii);
    glossary_index_free(sys->glossary);

    free(sys);
    fprintf(stderr, "[RETRIEVAL] Knowledge System shutdown complete\n");
}

// Get scripture at current mind position
// EMERGENCE: cognitive state directly retrieves associated Scripture
const ScriptureEntry** scripture_at_mind_position(
    const KnowledgeSystem* sys, int32_t cube_pos, const char* version, int32_t* count
) {
    if (!sys || !sys->initialized) {
        if (count) *count = 0;
        return NULL;
    }

    // Select version
    const CubeIndex* idx = (version && version[0] == 'W') ? sys->web_cube : sys->kjv_cube;
    return cube_index_at_position(idx, cube_pos, count);
}

// Get glossary term
const GlossaryEntry* knowledge_lookup_term(const KnowledgeSystem* sys, const char* term) {
    if (!sys || !sys->initialized || !sys->glossary) return NULL;
    return glossary_lookup(sys->glossary, term);
}

// Search for related terms
const GlossaryEntry* knowledge_search_term(const KnowledgeSystem* sys, const char* query) {
    if (!sys || !sys->initialized || !sys->glossary) return NULL;
    return glossary_search(sys->glossary, query);
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// "Thy word have I hid in mine heart" — Psalm 119:11
// The cube holds Scripture. The mind navigates. Emergence happens.

// =============================================================================
// END CLOSING
// =============================================================================
