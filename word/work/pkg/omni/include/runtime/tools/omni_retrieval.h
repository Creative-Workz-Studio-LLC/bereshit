// #!omni code --c -library
// #!omni meta.key = B-word-work-pkg-omni-retrieval
// #!omni meta.from = word/seed/code/c/header.h
// #!omni meta.at = a-01.00
//
// omni_retrieval.h — Knowledge Retrieval Layer
// Connect mind to filesystem - Scripture, glossary, knowledge

#ifndef BERESHIT_OMNI_RETRIEVAL_H
#define BERESHIT_OMNI_RETRIEVAL_H

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// Key: B-word-work-pkg-omni-retrieval
// Title: Knowledge Retrieval Layer
// Type: Header
// Role: Access filesystem knowledge - the memory of the mind

// Grounding: Psalm 119:11 - "Thy word have I hid in mine heart"
// Principle: Knowledge lives in the filesystem. Retrieval is memory.
// Anchor: The Word is accessible.

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include <stdint.h>
#include <stdbool.h>

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// # B.1 Retrieval Results [RESULTS]

#define MAX_VERSE_LENGTH 1024
#define MAX_DEFINITION_LENGTH 512
#define MAX_PATH_LENGTH 512

typedef struct {
    bool found;
    char content[MAX_VERSE_LENGTH];
    char reference[128];        // e.g., "John 3:16"
    char path[MAX_PATH_LENGTH]; // Filesystem path where found
} RetrievalResult;

// # B.2 Scripture Retrieval [SCRIPTURE]
//
// Access word/scripture/KJV/

// Parse scripture reference (e.g., "John 3:16" → book, chapter, verse)
typedef struct {
    char book[64];
    int chapter;
    int verse_start;
    int verse_end;      // For ranges like "John 3:16-17"
    bool valid;
} ScriptureRef;

// Parse a scripture reference string
ScriptureRef retrieval_parse_scripture_ref(const char* ref_str);

// Get scripture by reference
// Looks in: word/scripture/KJV/{Book}/Chapter_{N}/
RetrievalResult retrieval_get_scripture(const char* bereshit_root, ScriptureRef* ref);

// Get scripture by string reference (combines parse + get)
RetrievalResult retrieval_scripture(const char* bereshit_root, const char* ref_str);

// Get scripture with specific version (KJV, WEB)
RetrievalResult retrieval_get_scripture_version(const char* bereshit_root, ScriptureRef* ref, const char* version);

// # B.2b Scripture Index [SCRIPTURE_INDEX]
//
// Fast lookup using pre-built JSON indexes

// Scripture index entry (from kjv-scripture-key.json)
typedef struct {
    int32_t ordinal;        // Verse number in entire Bible (1-31102)
    char book[64];
    int32_t chapter;
    int32_t verse;
    char text[MAX_VERSE_LENGTH];
    char testament[4];      // "OT" or "NT"
    int32_t cube_pos;       // Cube position (0-26)
} ScriptureEntry;

// Load scripture index (call once at startup, returns allocated array)
// Returns: Array of ScriptureEntry, sets count. Caller must free with scripture_index_free.
ScriptureEntry* scripture_index_load(const char* bereshit_root, const char* version, int32_t* count);

// Free scripture index
void scripture_index_free(ScriptureEntry* index);

// Lookup by reference in loaded index (fast binary search)
const ScriptureEntry* scripture_index_lookup(
    const ScriptureEntry* index, int32_t count,
    const char* book, int32_t chapter, int32_t verse
);

// Lookup by cube position (returns all verses at that position)
// Returns: Array of pointers to ScriptureEntry, sets result_count. Caller must free array (not entries).
const ScriptureEntry** scripture_index_by_cube(
    const ScriptureEntry* index, int32_t count,
    int32_t cube_pos, int32_t* result_count
);

// Get ordinal from reference
int32_t scripture_ordinal(const char* book, int32_t chapter, int32_t verse);

// # B.3 Glossary Retrieval [GLOSSARY]
//
// Access word/glossary/ - both JSONC (structured) and AsciiDoc (human-readable)

// Glossary entry from JSONC file
typedef struct {
    char key[64];               // e.g., "GLOSS-BIB-001"
    char term[64];              // e.g., "bereshit"
    char hebrew[32];            // Hebrew characters if biblical
    char transliteration[64];   // Romanization
    char category[32];          // biblical, paradigm, technical, operations
    char definition[512];       // Concise definition
    char literal_meaning[256];  // Literal translation
    char scripture_kjv[512];    // KJV text if applicable
    char scripture_web[512];    // WEB text if applicable
    char scripture_ref[64];     // e.g., "Genesis 1:1"
    char see_also[256];         // Comma-separated related terms
} GlossaryEntry;

// Glossary index holding all terms
typedef struct {
    GlossaryEntry* entries;
    int32_t count;
    int32_t capacity;
    bool loaded;
} GlossaryIndex;

// Load entire glossary from JSONC files
GlossaryIndex* glossary_index_load(const char* bereshit_root);

// Free glossary index
void glossary_index_free(GlossaryIndex* idx);

// Lookup term by name (exact match)
const GlossaryEntry* glossary_lookup(const GlossaryIndex* idx, const char* term);

// Search glossary (fuzzy/partial match, returns first match)
const GlossaryEntry* glossary_search(const GlossaryIndex* idx, const char* query);

// Get definition from glossary (legacy - reads adoc files)
// Looks in: word/glossary/{category}/{term}.adoc
RetrievalResult retrieval_get_definition(const char* bereshit_root, const char* term);

// Search glossary for term (fuzzy match)
RetrievalResult retrieval_search_glossary(const char* bereshit_root, const char* query);

// # B.4 General File Retrieval [FILE]
//
// Access any file in the filesystem

// Read file content
RetrievalResult retrieval_read_file(const char* path);

// Check if path exists
bool retrieval_path_exists(const char* path);

// # B.5 Query Parsing [QUERY]
//
// Understand what's being asked for

typedef enum {
    QUERY_SCRIPTURE,    // "Quote John 3:16"
    QUERY_DEFINITION,   // "What is love?"
    QUERY_FILE,         // "Read file X"
    QUERY_SEARCH,       // "Find information about X"
    QUERY_UNKNOWN
} QueryType;

typedef struct {
    QueryType type;
    char subject[256];      // What they're asking about
    ScriptureRef scripture; // If type == QUERY_SCRIPTURE
} ParsedQuery;

// Parse a query to understand what's being asked
ParsedQuery retrieval_parse_query(const char* query);

// # B.6 High-Level Retrieval [RETRIEVE]
//
// One function to retrieve based on query

// Retrieve content based on parsed query
RetrievalResult retrieval_query(const char* bereshit_root, const char* query);

// # B.7 Cube Index [CUBE_INDEX]
//
// 3D cube mapping: position (x,y,z) → verses at that cognitive state
// EMERGENCE: Mind position directly indexes into Scripture

#define CUBE_SIZE 3  // 3x3x3 = 27 positions

// Cube position to flat index: pos = x*9 + y*3 + z (where x,y,z in {-1,0,1} → {0,1,2})
static inline int32_t cube_pos_from_xyz(int x, int y, int z) {
    return (x + 1) * 9 + (y + 1) * 3 + (z + 1);
}

static inline void cube_xyz_from_pos(int32_t pos, int* x, int* y, int* z) {
    *x = (pos / 9) - 1;
    *y = ((pos % 9) / 3) - 1;
    *z = (pos % 3) - 1;
}

// Cube index: array of verse lists per position (27 positions)
typedef struct {
    ScriptureEntry** verses;    // Array of verse pointers for this position
    int32_t count;              // Number of verses at this position
} CubePosition;

typedef struct {
    CubePosition positions[27]; // 27 cube positions
    bool loaded;
    char version[8];            // "KJV" or "WEB"
} CubeIndex;

// Load cube index from JSON (call once per version)
CubeIndex* cube_index_load(const char* bereshit_root, const char* version);

// Free cube index
void cube_index_free(CubeIndex* idx);

// Get verses at cube position (direct cognitive state → Scripture)
const ScriptureEntry** cube_index_at_position(
    const CubeIndex* idx, int32_t cube_pos, int32_t* count
);

// Get verses at xyz coordinates
const ScriptureEntry** cube_index_at_xyz(
    const CubeIndex* idx, int x, int y, int z, int32_t* count
);

// # B.8 ASCII Key [ASCII_KEY]
//
// Character encoding through the ternary cube

typedef struct {
    int32_t ascii;          // ASCII value (0-255)
    char character;         // The character
    bool printable;         // Is it printable?
    int32_t trit_value;     // Ternary encoding
    int32_t cube_x;         // Cube X coordinate (-1, 0, 1)
    int32_t cube_y;         // Cube Y coordinate
    int32_t cube_z;         // Cube Z coordinate
    int32_t cube_pos;       // Flat cube position (0-26)
} AsciiEntry;

typedef struct {
    AsciiEntry entries[256];  // All 256 ASCII values
    bool loaded;
} AsciiIndex;

// Load ASCII index
AsciiIndex* ascii_index_load(const char* bereshit_root);

// Free ASCII index
void ascii_index_free(AsciiIndex* idx);

// Get cube position for character
int32_t ascii_to_cube_pos(const AsciiIndex* idx, char c);

// Get character at cube position (reverse lookup)
char cube_pos_to_ascii(const AsciiIndex* idx, int32_t pos);

// # B.9 Unified Knowledge System [UNIFIED]
//
// Initialize all indexes for full emergence
// This IS the memory of the mind - no hidden layer, direct semantic indexing

typedef struct {
    // Scripture entries (full database)
    ScriptureEntry* kjv_entries;
    int32_t kjv_count;
    ScriptureEntry* web_entries;
    int32_t web_count;

    // Cube indexes (position → verses)
    CubeIndex* kjv_cube;
    CubeIndex* web_cube;

    // ASCII index (character ↔ cube)
    AsciiIndex* ascii;

    // Glossary (Kingdom Technology vocabulary)
    GlossaryIndex* glossary;

    bool initialized;
} KnowledgeSystem;

// Legacy alias
typedef KnowledgeSystem ScriptureSystem;

// Initialize the full knowledge system
KnowledgeSystem* knowledge_system_init(const char* bereshit_root);

// Legacy alias
static inline ScriptureSystem* scripture_system_init(const char* bereshit_root) {
    return knowledge_system_init(bereshit_root);
}

// Shutdown and free all resources
void knowledge_system_shutdown(KnowledgeSystem* sys);

// Legacy alias
static inline void scripture_system_shutdown(ScriptureSystem* sys) {
    knowledge_system_shutdown(sys);
}

// Get scripture at current mind position (cognitive state → Scripture)
// This is the EMERGENCE function: mind state directly retrieves associated Scripture
const ScriptureEntry** scripture_at_mind_position(
    const KnowledgeSystem* sys, int32_t cube_pos, const char* version, int32_t* count
);

// Get glossary term (direct vocabulary access)
const GlossaryEntry* knowledge_lookup_term(const KnowledgeSystem* sys, const char* term);

// Search for related terms
const GlossaryEntry* knowledge_search_term(const KnowledgeSystem* sys, const char* query);

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// "Thy word have I hid in mine heart" — Psalm 119:11
// The filesystem is memory. Retrieval is remembering.

#endif // BERESHIT_OMNI_RETRIEVAL_H

// =============================================================================
// END CLOSING
// =============================================================================
