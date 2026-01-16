// #!omni code --c -test
// #!omni meta.key = B-tov-demo-phase3-test-intrinsics
// #!omni meta.from = word/seed/code/c/source.c
// #!omni meta.at = a-01.00
//
// test_intrinsics.c — VM Intrinsics Test
// Validates string, collection, and file I/O intrinsics.

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// # M.1 Core Identity [IDENTITY]
//
// Key: B-tov-demo-phase3-test-intrinsics
// Title: VM Intrinsics Test
// Type: Test
// Role: Validate runtime intrinsics for strings, collections, and files

// # M.8 Grounding [GROUNDING]
//
// Scripture: Genesis 1:11 - "Let the earth bring forth"
// Principle: Dynamic growth from established patterns
// Anchor: Runtime intrinsics enable the Word to become executable

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "omni_runtime.h"

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// # B.1 String Tests [STRING]

static int test_string_basic(void) {
    printf("  Testing string basics...\n");

    OmniRuntime* rt = omni_runtime_create();
    if (!rt) {
        printf("    FAIL: Could not create runtime\n");
        return 1;
    }

    // Create string
    OmniString* s1 = omni_string_create(rt, "Hello");
    if (!s1) {
        printf("    FAIL: Could not create string\n");
        omni_runtime_destroy(rt);
        return 1;
    }

    // Check length
    if (omni_string_length(s1) != 5) {
        printf("    FAIL: Expected length 5, got %u\n", omni_string_length(s1));
        omni_runtime_destroy(rt);
        return 1;
    }

    // Check cstr
    if (strcmp(omni_string_cstr(s1), "Hello") != 0) {
        printf("    FAIL: String content mismatch\n");
        omni_runtime_destroy(rt);
        return 1;
    }

    printf("    PASS: String basics work\n");
    omni_runtime_destroy(rt);
    return 0;
}

static int test_string_concat(void) {
    printf("  Testing string concatenation...\n");

    OmniRuntime* rt = omni_runtime_create();
    if (!rt) return 1;

    OmniString* s1 = omni_string_create(rt, "Hello");
    OmniString* s2 = omni_string_create(rt, " World");
    OmniString* s3 = omni_string_concat(rt, s1, s2);

    if (!s3) {
        printf("    FAIL: Concat returned NULL\n");
        omni_runtime_destroy(rt);
        return 1;
    }

    if (strcmp(omni_string_cstr(s3), "Hello World") != 0) {
        printf("    FAIL: Expected 'Hello World', got '%s'\n", omni_string_cstr(s3));
        omni_runtime_destroy(rt);
        return 1;
    }

    printf("    PASS: String concatenation works\n");
    omni_runtime_destroy(rt);
    return 0;
}

static int test_string_substr(void) {
    printf("  Testing substring...\n");

    OmniRuntime* rt = omni_runtime_create();
    if (!rt) return 1;

    OmniString* s1 = omni_string_create(rt, "Hello World");
    OmniString* s2 = omni_string_substr(rt, s1, 6, 5);

    if (!s2) {
        printf("    FAIL: Substr returned NULL\n");
        omni_runtime_destroy(rt);
        return 1;
    }

    if (strcmp(omni_string_cstr(s2), "World") != 0) {
        printf("    FAIL: Expected 'World', got '%s'\n", omni_string_cstr(s2));
        omni_runtime_destroy(rt);
        return 1;
    }

    printf("    PASS: Substring works\n");
    omni_runtime_destroy(rt);
    return 0;
}

static int test_string_find(void) {
    printf("  Testing string find...\n");

    OmniRuntime* rt = omni_runtime_create();
    if (!rt) return 1;

    OmniString* haystack = omni_string_create(rt, "Hello World");
    OmniString* needle = omni_string_create(rt, "World");
    OmniString* notfound = omni_string_create(rt, "xyz");

    int32_t pos = omni_string_find(haystack, needle);
    if (pos != 6) {
        printf("    FAIL: Expected pos 6, got %d\n", pos);
        omni_runtime_destroy(rt);
        return 1;
    }

    pos = omni_string_find(haystack, notfound);
    if (pos != -1) {
        printf("    FAIL: Expected -1 for not found, got %d\n", pos);
        omni_runtime_destroy(rt);
        return 1;
    }

    printf("    PASS: String find works\n");
    omni_runtime_destroy(rt);
    return 0;
}

static int test_string_case(void) {
    printf("  Testing string case conversion...\n");

    OmniRuntime* rt = omni_runtime_create();
    if (!rt) return 1;

    OmniString* s1 = omni_string_create(rt, "Hello World");
    OmniString* upper = omni_string_upper(rt, s1);
    OmniString* lower = omni_string_lower(rt, s1);

    if (strcmp(omni_string_cstr(upper), "HELLO WORLD") != 0) {
        printf("    FAIL: Upper expected 'HELLO WORLD', got '%s'\n", omni_string_cstr(upper));
        omni_runtime_destroy(rt);
        return 1;
    }

    if (strcmp(omni_string_cstr(lower), "hello world") != 0) {
        printf("    FAIL: Lower expected 'hello world', got '%s'\n", omni_string_cstr(lower));
        omni_runtime_destroy(rt);
        return 1;
    }

    printf("    PASS: String case conversion works\n");
    omni_runtime_destroy(rt);
    return 0;
}

static int test_string_trim(void) {
    printf("  Testing string trim...\n");

    OmniRuntime* rt = omni_runtime_create();
    if (!rt) return 1;

    OmniString* s1 = omni_string_create(rt, "  Hello World  ");
    OmniString* trimmed = omni_string_trim(rt, s1);

    if (strcmp(omni_string_cstr(trimmed), "Hello World") != 0) {
        printf("    FAIL: Trim expected 'Hello World', got '%s'\n", omni_string_cstr(trimmed));
        omni_runtime_destroy(rt);
        return 1;
    }

    printf("    PASS: String trim works\n");
    omni_runtime_destroy(rt);
    return 0;
}

static int test_string_prefix_suffix(void) {
    printf("  Testing starts_with/ends_with...\n");

    OmniRuntime* rt = omni_runtime_create();
    if (!rt) return 1;

    OmniString* s1 = omni_string_create(rt, "Hello World");
    OmniString* prefix = omni_string_create(rt, "Hello");
    OmniString* suffix = omni_string_create(rt, "World");
    OmniString* wrong = omni_string_create(rt, "xyz");

    if (!omni_string_starts_with(s1, prefix)) {
        printf("    FAIL: Should start with 'Hello'\n");
        omni_runtime_destroy(rt);
        return 1;
    }

    if (!omni_string_ends_with(s1, suffix)) {
        printf("    FAIL: Should end with 'World'\n");
        omni_runtime_destroy(rt);
        return 1;
    }

    if (omni_string_starts_with(s1, wrong)) {
        printf("    FAIL: Should not start with 'xyz'\n");
        omni_runtime_destroy(rt);
        return 1;
    }

    printf("    PASS: Prefix/suffix works\n");
    omni_runtime_destroy(rt);
    return 0;
}

static int test_string_split_join(void) {
    printf("  Testing split/join...\n");

    OmniRuntime* rt = omni_runtime_create();
    if (!rt) return 1;

    OmniString* s1 = omni_string_create(rt, "a,b,c");
    OmniString* delim = omni_string_create(rt, ",");

    OmniList* parts = omni_string_split(rt, s1, delim);
    if (!parts) {
        printf("    FAIL: Split returned NULL\n");
        omni_runtime_destroy(rt);
        return 1;
    }

    if (omni_list_length(parts) != 3) {
        printf("    FAIL: Expected 3 parts, got %u\n", omni_list_length(parts));
        omni_runtime_destroy(rt);
        return 1;
    }

    OmniString* joined = omni_string_join(rt, parts, delim);
    if (strcmp(omni_string_cstr(joined), "a,b,c") != 0) {
        printf("    FAIL: Join expected 'a,b,c', got '%s'\n", omni_string_cstr(joined));
        omni_runtime_destroy(rt);
        return 1;
    }

    printf("    PASS: Split/join works\n");
    omni_runtime_destroy(rt);
    return 0;
}

// # B.2 List Tests [LIST]

static int test_list_basic(void) {
    printf("  Testing list basics...\n");

    OmniRuntime* rt = omni_runtime_create();
    if (!rt) return 1;

    OmniList* list = omni_list_create(rt);
    if (!list) {
        printf("    FAIL: Could not create list\n");
        omni_runtime_destroy(rt);
        return 1;
    }

    // Push values
    OmniValue v1 = { .type = VAL_INT, .as_int = 42 };
    OmniValue v2 = { .type = VAL_INT, .as_int = 100 };
    OmniValue v3 = { .type = VAL_BOOL, .as_bool = true };

    omni_list_push(list, v1);
    omni_list_push(list, v2);
    omni_list_push(list, v3);

    if (omni_list_length(list) != 3) {
        printf("    FAIL: Expected length 3, got %u\n", omni_list_length(list));
        omni_runtime_destroy(rt);
        return 1;
    }

    // Get value
    OmniValue out;
    if (!omni_list_get(list, 0, &out) || out.as_int != 42) {
        printf("    FAIL: Get index 0 failed\n");
        omni_runtime_destroy(rt);
        return 1;
    }

    printf("    PASS: List basics work\n");
    omni_runtime_destroy(rt);
    return 0;
}

static int test_list_pop(void) {
    printf("  Testing list pop...\n");

    OmniRuntime* rt = omni_runtime_create();
    if (!rt) return 1;

    OmniList* list = omni_list_create(rt);

    OmniValue v1 = { .type = VAL_INT, .as_int = 1 };
    OmniValue v2 = { .type = VAL_INT, .as_int = 2 };
    OmniValue v3 = { .type = VAL_INT, .as_int = 3 };

    omni_list_push(list, v1);
    omni_list_push(list, v2);
    omni_list_push(list, v3);

    OmniValue popped;
    if (!omni_list_pop(list, &popped) || popped.as_int != 3) {
        printf("    FAIL: Pop should return 3\n");
        omni_runtime_destroy(rt);
        return 1;
    }

    if (omni_list_length(list) != 2) {
        printf("    FAIL: Length should be 2 after pop\n");
        omni_runtime_destroy(rt);
        return 1;
    }

    printf("    PASS: List pop works\n");
    omni_runtime_destroy(rt);
    return 0;
}

static int test_list_find(void) {
    printf("  Testing list find...\n");

    OmniRuntime* rt = omni_runtime_create();
    if (!rt) return 1;

    OmniList* list = omni_list_create(rt);

    OmniValue v1 = { .type = VAL_INT, .as_int = 10 };
    OmniValue v2 = { .type = VAL_INT, .as_int = 20 };
    OmniValue v3 = { .type = VAL_INT, .as_int = 30 };

    omni_list_push(list, v1);
    omni_list_push(list, v2);
    omni_list_push(list, v3);

    OmniValue search = { .type = VAL_INT, .as_int = 20 };
    int32_t pos = omni_list_find(list, search);

    if (pos != 1) {
        printf("    FAIL: Expected pos 1, got %d\n", pos);
        omni_runtime_destroy(rt);
        return 1;
    }

    OmniValue notfound = { .type = VAL_INT, .as_int = 999 };
    pos = omni_list_find(list, notfound);

    if (pos != -1) {
        printf("    FAIL: Expected -1 for not found\n");
        omni_runtime_destroy(rt);
        return 1;
    }

    printf("    PASS: List find works\n");
    omni_runtime_destroy(rt);
    return 0;
}

// # B.3 Map Tests [MAP]

static int test_map_basic(void) {
    printf("  Testing map basics...\n");

    OmniRuntime* rt = omni_runtime_create();
    if (!rt) return 1;

    OmniMap* map = omni_map_create(rt);
    if (!map) {
        printf("    FAIL: Could not create map\n");
        omni_runtime_destroy(rt);
        return 1;
    }

    // Set values
    OmniString* k1 = omni_string_create(rt, "name");
    OmniString* k2 = omni_string_create(rt, "age");

    OmniValue v1 = { .type = VAL_INT, .as_int = 42 };
    OmniValue v2 = { .type = VAL_INT, .as_int = 25 };

    omni_map_set(rt, map, k1, v1);
    omni_map_set(rt, map, k2, v2);

    if (omni_map_length(map) != 2) {
        printf("    FAIL: Expected length 2, got %u\n", omni_map_length(map));
        omni_runtime_destroy(rt);
        return 1;
    }

    // Get value
    OmniValue out;
    if (!omni_map_get(map, k1, &out) || out.as_int != 42) {
        printf("    FAIL: Get 'name' failed\n");
        omni_runtime_destroy(rt);
        return 1;
    }

    printf("    PASS: Map basics work\n");
    omni_runtime_destroy(rt);
    return 0;
}

static int test_map_has_delete(void) {
    printf("  Testing map has/delete...\n");

    OmniRuntime* rt = omni_runtime_create();
    if (!rt) return 1;

    OmniMap* map = omni_map_create(rt);

    OmniString* k1 = omni_string_create(rt, "key1");
    OmniString* k2 = omni_string_create(rt, "key2");

    OmniValue v = { .type = VAL_INT, .as_int = 1 };
    omni_map_set(rt, map, k1, v);

    if (!omni_map_has(map, k1)) {
        printf("    FAIL: Should have 'key1'\n");
        omni_runtime_destroy(rt);
        return 1;
    }

    if (omni_map_has(map, k2)) {
        printf("    FAIL: Should not have 'key2'\n");
        omni_runtime_destroy(rt);
        return 1;
    }

    omni_map_delete(map, k1);

    if (omni_map_has(map, k1)) {
        printf("    FAIL: Should not have 'key1' after delete\n");
        omni_runtime_destroy(rt);
        return 1;
    }

    printf("    PASS: Map has/delete works\n");
    omni_runtime_destroy(rt);
    return 0;
}

static int test_map_keys(void) {
    printf("  Testing map keys...\n");

    OmniRuntime* rt = omni_runtime_create();
    if (!rt) return 1;

    OmniMap* map = omni_map_create(rt);

    OmniString* k1 = omni_string_create(rt, "a");
    OmniString* k2 = omni_string_create(rt, "b");
    OmniString* k3 = omni_string_create(rt, "c");

    OmniValue v = { .type = VAL_INT, .as_int = 1 };
    omni_map_set(rt, map, k1, v);
    omni_map_set(rt, map, k2, v);
    omni_map_set(rt, map, k3, v);

    OmniList* keys = omni_map_keys(rt, map);
    if (!keys) {
        printf("    FAIL: Could not get keys\n");
        omni_runtime_destroy(rt);
        return 1;
    }

    if (omni_list_length(keys) != 3) {
        printf("    FAIL: Expected 3 keys, got %u\n", omni_list_length(keys));
        omni_runtime_destroy(rt);
        return 1;
    }

    printf("    PASS: Map keys works\n");
    omni_runtime_destroy(rt);
    return 0;
}

// # B.4 File Tests [FILE]

static int test_file_exists(void) {
    printf("  Testing file exists...\n");

    // This test file should exist
    if (!omni_file_exists("test_intrinsics.c")) {
        printf("    FAIL: test_intrinsics.c should exist\n");
        return 1;
    }

    // This should not exist
    if (omni_file_exists("nonexistent_file_xyz.txt")) {
        printf("    FAIL: nonexistent file should not exist\n");
        return 1;
    }

    printf("    PASS: File exists works\n");
    return 0;
}

static int test_file_read_write(void) {
    printf("  Testing file read/write...\n");

    OmniRuntime* rt = omni_runtime_create();
    if (!rt) return 1;

    const char* test_file = "build/test_output.txt";
    const char* test_content = "Hello from OmniCode!\nLine 2\n";

    // Write
    FILE* f = omni_file_open(rt, test_file, "w");
    if (!f) {
        printf("    FAIL: Could not open file for writing\n");
        omni_runtime_destroy(rt);
        return 1;
    }

    OmniString* content = omni_string_create(rt, test_content);
    if (!omni_file_write(f, content)) {
        printf("    FAIL: Could not write to file\n");
        omni_file_close(rt, f);
        omni_runtime_destroy(rt);
        return 1;
    }

    omni_file_close(rt, f);

    // Read back
    f = omni_file_open(rt, test_file, "r");
    if (!f) {
        printf("    FAIL: Could not open file for reading\n");
        omni_runtime_destroy(rt);
        return 1;
    }

    OmniString* read_content = omni_file_read(rt, f);
    omni_file_close(rt, f);

    if (!read_content) {
        printf("    FAIL: Could not read file\n");
        omni_runtime_destroy(rt);
        return 1;
    }

    if (strcmp(omni_string_cstr(read_content), test_content) != 0) {
        printf("    FAIL: Content mismatch\n");
        printf("      Expected: %s\n", test_content);
        printf("      Got: %s\n", omni_string_cstr(read_content));
        omni_runtime_destroy(rt);
        return 1;
    }

    // Cleanup
    omni_file_delete(test_file);

    printf("    PASS: File read/write works\n");
    omni_runtime_destroy(rt);
    return 0;
}

static int test_dir_operations(void) {
    printf("  Testing directory operations...\n");

    OmniRuntime* rt = omni_runtime_create();
    if (!rt) return 1;

    // build/ should exist (created by Makefile)
    if (!omni_dir_exists("build")) {
        printf("    FAIL: build/ directory should exist\n");
        omni_runtime_destroy(rt);
        return 1;
    }

    // List directory
    OmniList* entries = omni_dir_list(rt, "build");
    if (!entries) {
        printf("    FAIL: Could not list directory\n");
        omni_runtime_destroy(rt);
        return 1;
    }

    if (omni_list_length(entries) == 0) {
        printf("    FAIL: Directory should not be empty\n");
        omni_runtime_destroy(rt);
        return 1;
    }

    printf("    Found %u entries in build/\n", omni_list_length(entries));
    printf("    PASS: Directory operations work\n");
    omni_runtime_destroy(rt);
    return 0;
}

// # B.5 Main [MAIN]

int main(void) {
    int failures = 0;

    printf("=== VM Intrinsics Tests ===\n\n");

    // String tests
    printf("String Tests:\n");
    failures += test_string_basic();
    failures += test_string_concat();
    failures += test_string_substr();
    failures += test_string_find();
    failures += test_string_case();
    failures += test_string_trim();
    failures += test_string_prefix_suffix();
    failures += test_string_split_join();
    printf("\n");

    // List tests
    printf("List Tests:\n");
    failures += test_list_basic();
    failures += test_list_pop();
    failures += test_list_find();
    printf("\n");

    // Map tests
    printf("Map Tests:\n");
    failures += test_map_basic();
    failures += test_map_has_delete();
    failures += test_map_keys();
    printf("\n");

    // File tests
    printf("File Tests:\n");
    failures += test_file_exists();
    failures += test_file_read_write();
    failures += test_dir_operations();
    printf("\n");

    if (failures > 0) {
        printf("=== %d TEST(S) FAILED ===\n", failures);
        return 1;
    }

    printf("=== All Intrinsics Tests Passed ===\n");
    return 0;
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// # X.1 Usage
//
//   make build/test_intrinsics
//   ./build/test_intrinsics
//
// # X.2 Closing Note
//
// "Let the earth bring forth... whose seed is in itself."
//   — Genesis 1:11
//
// Dynamic growth follows established patterns.

// =============================================================================
// END CLOSING
// =============================================================================
