-- seed-build.sql
-- Seed data for build.db from word/work/pkg/trit/ (libtrit Phase 1)
-- Biblical anchor: "Except the LORD build the house, they labour in vain" (Psalm 127:1)
-- Build state tracks the work of implementation

-- ═══════════════════════════════════════════════════════════════════════════
-- SOURCES - Phase 1 libtrit source files
-- ═══════════════════════════════════════════════════════════════════════════

-- MATTER layer - Types and storage (foundational)
INSERT INTO sources (path, name, extension, source_type, layer, size_bytes, last_modified, created_at)
VALUES ('pkg/trit/include/trit.h', 'trit.h', '.h', 'header', 'MATTER', 28967, '2024-12-12', datetime('now'));

INSERT INTO sources (path, name, extension, source_type, layer, size_bytes, last_modified, created_at)
VALUES ('pkg/trit/src/trit.c', 'trit.c', '.c', 'implementation', 'MATTER', 24799, '2024-12-12', datetime('now'));

INSERT INTO sources (path, name, extension, source_type, layer, size_bytes, last_modified, created_at)
VALUES ('pkg/trit/src/pack.c', 'pack.c', '.c', 'implementation', 'MATTER', 26442, '2024-12-12', datetime('now'));

-- SPACE layer - Navigation and relationships
INSERT INTO sources (path, name, extension, source_type, layer, size_bytes, last_modified, created_at)
VALUES ('pkg/trit/include/dimension.h', 'dimension.h', '.h', 'header', 'SPACE', 28095, '2024-12-12', datetime('now'));

INSERT INTO sources (path, name, extension, source_type, layer, size_bytes, last_modified, created_at)
VALUES ('pkg/trit/src/dimension.c', 'dimension.c', '.c', 'implementation', 'SPACE', 28831, '2024-12-12', datetime('now'));

-- TIME layer - Cognitive modes
INSERT INTO sources (path, name, extension, source_type, layer, size_bytes, last_modified, created_at)
VALUES ('pkg/trit/include/temporal.h', 'temporal.h', '.h', 'header', 'TIME', 28269, '2024-12-12', datetime('now'));

INSERT INTO sources (path, name, extension, source_type, layer, size_bytes, last_modified, created_at)
VALUES ('pkg/trit/src/temporal.c', 'temporal.c', '.c', 'implementation', 'TIME', 29750, '2024-12-12', datetime('now'));

-- Math operations (Phase 2)
INSERT INTO sources (path, name, extension, source_type, layer, size_bytes, last_modified, created_at)
VALUES ('pkg/trit/include/math.h', 'math.h', '.h', 'header', 'MATTER', 40603, '2024-12-14', datetime('now'));

INSERT INTO sources (path, name, extension, source_type, layer, size_bytes, last_modified, created_at)
VALUES ('pkg/trit/src/math.c', 'math.c', '.c', 'implementation', 'MATTER', 40638, '2024-12-14', datetime('now'));

-- Bible rail (Scripture addressing)
INSERT INTO sources (path, name, extension, source_type, layer, size_bytes, last_modified, created_at)
VALUES ('pkg/trit/include/bible_rail.h', 'bible_rail.h', '.h', 'header', 'SPACE', 28304, '2024-12-14', datetime('now'));

INSERT INTO sources (path, name, extension, source_type, layer, size_bytes, last_modified, created_at)
VALUES ('pkg/trit/src/bible_rail.c', 'bible_rail.c', '.c', 'implementation', 'SPACE', 20890, '2024-12-14', datetime('now'));

-- Scripture text encoding
INSERT INTO sources (path, name, extension, source_type, layer, size_bytes, last_modified, created_at)
VALUES ('pkg/trit/include/scripture_text.h', 'scripture_text.h', '.h', 'header', 'SPACE', 23664, '2024-12-14', datetime('now'));

INSERT INTO sources (path, name, extension, source_type, layer, size_bytes, last_modified, created_at)
VALUES ('pkg/trit/src/scripture_text.c', 'scripture_text.c', '.c', 'implementation', 'SPACE', 20155, '2024-12-14', datetime('now'));

-- Scripture path addressing
INSERT INTO sources (path, name, extension, source_type, layer, size_bytes, last_modified, created_at)
VALUES ('pkg/trit/include/scripture_path.h', 'scripture_path.h', '.h', 'header', 'SPACE', 28505, '2024-12-14', datetime('now'));

INSERT INTO sources (path, name, extension, source_type, layer, size_bytes, last_modified, created_at)
VALUES ('pkg/trit/src/scripture_path.c', 'scripture_path.c', '.c', 'implementation', 'SPACE', 25191, '2024-12-14', datetime('now'));

-- Verse addressing
INSERT INTO sources (path, name, extension, source_type, layer, size_bytes, last_modified, created_at)
VALUES ('pkg/trit/include/verse_addr.h', 'verse_addr.h', '.h', 'header', 'SPACE', 28899, '2024-12-14', datetime('now'));

INSERT INTO sources (path, name, extension, source_type, layer, size_bytes, last_modified, created_at)
VALUES ('pkg/trit/src/verse_addr.c', 'verse_addr.c', '.c', 'implementation', 'SPACE', 19674, '2024-12-14', datetime('now'));

-- Test files
INSERT INTO sources (path, name, extension, source_type, layer, size_bytes, last_modified, created_at)
VALUES ('pkg/trit/test/trit_test.c', 'trit_test.c', '.c', 'test', 'MATTER', 49756, '2024-12-12', datetime('now'));

INSERT INTO sources (path, name, extension, source_type, layer, size_bytes, last_modified, created_at)
VALUES ('pkg/trit/test/pack_test.c', 'pack_test.c', '.c', 'test', 'MATTER', 53655, '2024-12-12', datetime('now'));

INSERT INTO sources (path, name, extension, source_type, layer, size_bytes, last_modified, created_at)
VALUES ('pkg/trit/test/dimension_test.c', 'dimension_test.c', '.c', 'test', 'SPACE', 51261, '2024-12-12', datetime('now'));

INSERT INTO sources (path, name, extension, source_type, layer, size_bytes, last_modified, created_at)
VALUES ('pkg/trit/test/temporal_test.c', 'temporal_test.c', '.c', 'test', 'TIME', 52764, '2024-12-12', datetime('now'));

INSERT INTO sources (path, name, extension, source_type, layer, size_bytes, last_modified, created_at)
VALUES ('pkg/trit/test/math_test.c', 'math_test.c', '.c', 'test', 'MATTER', 41039, '2024-12-14', datetime('now'));

INSERT INTO sources (path, name, extension, source_type, layer, size_bytes, last_modified, created_at)
VALUES ('pkg/trit/test/bible_rail_test.c', 'bible_rail_test.c', '.c', 'test', 'SPACE', 46171, '2024-12-14', datetime('now'));

INSERT INTO sources (path, name, extension, source_type, layer, size_bytes, last_modified, created_at)
VALUES ('pkg/trit/test/scripture_text_test.c', 'scripture_text_test.c', '.c', 'test', 'SPACE', 40714, '2024-12-14', datetime('now'));

INSERT INTO sources (path, name, extension, source_type, layer, size_bytes, last_modified, created_at)
VALUES ('pkg/trit/test/scripture_path_test.c', 'scripture_path_test.c', '.c', 'test', 'SPACE', 38619, '2024-12-14', datetime('now'));

INSERT INTO sources (path, name, extension, source_type, layer, size_bytes, last_modified, created_at)
VALUES ('pkg/trit/test/verse_addr_test.c', 'verse_addr_test.c', '.c', 'test', 'SPACE', 47011, '2024-12-14', datetime('now'));

INSERT INTO sources (path, name, extension, source_type, layer, size_bytes, last_modified, created_at)
VALUES ('pkg/trit/test/integration_test.c', 'integration_test.c', '.c', 'test', NULL, 46794, '2024-12-12', datetime('now'));

-- ═══════════════════════════════════════════════════════════════════════════
-- OBJECTS - Compiled .o files
-- ═══════════════════════════════════════════════════════════════════════════

INSERT INTO objects (source_id, path, size_bytes, last_built, build_status, compiler_version, created_at)
SELECT id, 'pkg/trit/build/trit.o', 2072, '2024-12-14', 'success', 'gcc 14.2.0', datetime('now')
FROM sources WHERE name = 'trit.c';

INSERT INTO objects (source_id, path, size_bytes, last_built, build_status, compiler_version, created_at)
SELECT id, 'pkg/trit/build/pack.o', 2072, '2024-12-14', 'success', 'gcc 14.2.0', datetime('now')
FROM sources WHERE name = 'pack.c';

INSERT INTO objects (source_id, path, size_bytes, last_built, build_status, compiler_version, created_at)
SELECT id, 'pkg/trit/build/dimension.o', 2368, '2024-12-14', 'success', 'gcc 14.2.0', datetime('now')
FROM sources WHERE name = 'dimension.c';

INSERT INTO objects (source_id, path, size_bytes, last_built, build_status, compiler_version, created_at)
SELECT id, 'pkg/trit/build/temporal.o', 2856, '2024-12-14', 'success', 'gcc 14.2.0', datetime('now')
FROM sources WHERE name = 'temporal.c';

INSERT INTO objects (source_id, path, size_bytes, last_built, build_status, compiler_version, created_at)
SELECT id, 'pkg/trit/build/math.o', 12664, '2024-12-14', 'success', 'gcc 14.2.0', datetime('now')
FROM sources WHERE name = 'math.c';

INSERT INTO objects (source_id, path, size_bytes, last_built, build_status, compiler_version, created_at)
SELECT id, 'pkg/trit/build/bible_rail.o', 4720, '2024-12-14', 'success', 'gcc 14.2.0', datetime('now')
FROM sources WHERE name = 'bible_rail.c';

INSERT INTO objects (source_id, path, size_bytes, last_built, build_status, compiler_version, created_at)
SELECT id, 'pkg/trit/build/scripture_text.o', 2928, '2024-12-14', 'success', 'gcc 14.2.0', datetime('now')
FROM sources WHERE name = 'scripture_text.c';

INSERT INTO objects (source_id, path, size_bytes, last_built, build_status, compiler_version, created_at)
SELECT id, 'pkg/trit/build/scripture_path.o', 6672, '2024-12-14', 'success', 'gcc 14.2.0', datetime('now')
FROM sources WHERE name = 'scripture_path.c';

-- ═══════════════════════════════════════════════════════════════════════════
-- LIBRARIES - libtrit.a archive
-- ═══════════════════════════════════════════════════════════════════════════

INSERT INTO libraries (name, path, version, size_bytes, last_built, object_count, created_at)
VALUES ('libtrit', 'pkg/trit/build/libtrit.a', 'a-01.00', 41430, '2024-12-14', 8, datetime('now'));

-- ═══════════════════════════════════════════════════════════════════════════
-- LIBRARY_OBJECTS - Link objects to library
-- ═══════════════════════════════════════════════════════════════════════════

INSERT INTO library_objects (library_id, object_id, created_at)
SELECT l.id, o.id, datetime('now')
FROM libraries l, objects o
WHERE l.name = 'libtrit' AND o.path LIKE '%trit.o';

INSERT INTO library_objects (library_id, object_id, created_at)
SELECT l.id, o.id, datetime('now')
FROM libraries l, objects o
WHERE l.name = 'libtrit' AND o.path LIKE '%pack.o';

INSERT INTO library_objects (library_id, object_id, created_at)
SELECT l.id, o.id, datetime('now')
FROM libraries l, objects o
WHERE l.name = 'libtrit' AND o.path LIKE '%dimension.o';

INSERT INTO library_objects (library_id, object_id, created_at)
SELECT l.id, o.id, datetime('now')
FROM libraries l, objects o
WHERE l.name = 'libtrit' AND o.path LIKE '%temporal.o';

INSERT INTO library_objects (library_id, object_id, created_at)
SELECT l.id, o.id, datetime('now')
FROM libraries l, objects o
WHERE l.name = 'libtrit' AND o.path LIKE '%math.o';

INSERT INTO library_objects (library_id, object_id, created_at)
SELECT l.id, o.id, datetime('now')
FROM libraries l, objects o
WHERE l.name = 'libtrit' AND o.path LIKE '%bible_rail.o';

INSERT INTO library_objects (library_id, object_id, created_at)
SELECT l.id, o.id, datetime('now')
FROM libraries l, objects o
WHERE l.name = 'libtrit' AND o.path LIKE '%scripture_text.o';

INSERT INTO library_objects (library_id, object_id, created_at)
SELECT l.id, o.id, datetime('now')
FROM libraries l, objects o
WHERE l.name = 'libtrit' AND o.path LIKE '%scripture_path.o';

-- ═══════════════════════════════════════════════════════════════════════════
-- TESTS - Test suite results
-- ═══════════════════════════════════════════════════════════════════════════

INSERT INTO tests (name, source_id, last_run, status, assertions_total, assertions_passed, assertions_failed, created_at)
SELECT 'trit_test', id, '2024-12-25', 'passed', 15, 15, 0, datetime('now')
FROM sources WHERE name = 'trit_test.c';

INSERT INTO tests (name, source_id, last_run, status, assertions_total, assertions_passed, assertions_failed, created_at)
SELECT 'pack_test', id, '2024-12-25', 'passed', 12, 12, 0, datetime('now')
FROM sources WHERE name = 'pack_test.c';

INSERT INTO tests (name, source_id, last_run, status, assertions_total, assertions_passed, assertions_failed, created_at)
SELECT 'dimension_test', id, '2024-12-25', 'passed', 10, 10, 0, datetime('now')
FROM sources WHERE name = 'dimension_test.c';

INSERT INTO tests (name, source_id, last_run, status, assertions_total, assertions_passed, assertions_failed, created_at)
SELECT 'temporal_test', id, '2024-12-25', 'passed', 9, 9, 0, datetime('now')
FROM sources WHERE name = 'temporal_test.c';

INSERT INTO tests (name, source_id, last_run, status, assertions_total, assertions_passed, assertions_failed, created_at)
SELECT 'math_test', id, '2024-12-25', 'passed', 15, 15, 0, datetime('now')
FROM sources WHERE name = 'math_test.c';

INSERT INTO tests (name, source_id, last_run, status, assertions_total, assertions_passed, assertions_failed, created_at)
SELECT 'bible_rail_test', id, '2024-12-25', 'passed', 5, 5, 0, datetime('now')
FROM sources WHERE name = 'bible_rail_test.c';

INSERT INTO tests (name, source_id, last_run, status, assertions_total, assertions_passed, assertions_failed, created_at)
SELECT 'scripture_text_test', id, '2024-12-25', 'passed', 4, 4, 0, datetime('now')
FROM sources WHERE name = 'scripture_text_test.c';

INSERT INTO tests (name, source_id, last_run, status, assertions_total, assertions_passed, assertions_failed, created_at)
SELECT 'scripture_path_test', id, '2024-12-25', 'passed', 3, 3, 0, datetime('now')
FROM sources WHERE name = 'scripture_path_test.c';

INSERT INTO tests (name, source_id, last_run, status, assertions_total, assertions_passed, assertions_failed, created_at)
SELECT 'verse_addr_test', id, '2024-12-25', 'passed', 4, 4, 0, datetime('now')
FROM sources WHERE name = 'verse_addr_test.c';

INSERT INTO tests (name, source_id, last_run, status, assertions_total, assertions_passed, assertions_failed, created_at)
SELECT 'integration_test', id, '2024-12-25', 'passed', 5, 5, 0, datetime('now')
FROM sources WHERE name = 'integration_test.c';

-- ═══════════════════════════════════════════════════════════════════════════
-- DEMOS - Phase validation demos
-- ═══════════════════════════════════════════════════════════════════════════

INSERT INTO demos (name, phase, path, last_run, status, steps_total, steps_passed, created_at)
VALUES ('demo-config', 0, 'tov/demo/phase-0/demo-config', '2024-12-25', 'passed', 5, 5, datetime('now'));

INSERT INTO demos (name, phase, path, last_run, status, steps_total, steps_passed, created_at)
VALUES ('demo-trit', 1, 'tov/demo/phase-1/demo-trit', '2024-12-25', 'passed', 7, 7, datetime('now'));

INSERT INTO demos (name, phase, path, last_run, status, steps_total, steps_passed, created_at)
VALUES ('demo-math', 2, 'tov/demo/phase-2/demo-math', '2024-12-25', 'passed', 9, 9, datetime('now'));

-- ═══════════════════════════════════════════════════════════════════════════
-- BUILD_EVENTS - Recent build history
-- ═══════════════════════════════════════════════════════════════════════════

INSERT INTO build_events (event_type, target, status, message, timestamp)
VALUES ('compile', 'libtrit', 'success', 'All source files compiled', '2024-12-14T18:55:00Z');

INSERT INTO build_events (event_type, target, status, message, timestamp)
VALUES ('link', 'libtrit.a', 'success', 'Static library archived (8 objects)', '2024-12-14T18:55:05Z');

INSERT INTO build_events (event_type, target, status, message, timestamp)
VALUES ('test', 'trit_test', 'success', '15/15 assertions passed', '2024-12-25T15:01:00Z');

INSERT INTO build_events (event_type, target, status, message, timestamp)
VALUES ('test', 'pack_test', 'success', '12/12 assertions passed', '2024-12-25T15:01:02Z');

INSERT INTO build_events (event_type, target, status, message, timestamp)
VALUES ('test', 'math_test', 'success', '15/15 assertions passed', '2024-12-25T15:01:05Z');

INSERT INTO build_events (event_type, target, status, message, timestamp)
VALUES ('demo', 'demo-trit', 'success', 'Phase 1 validation passed', '2024-12-25T15:02:00Z');

INSERT INTO build_events (event_type, target, status, message, timestamp)
VALUES ('demo', 'demo-math', 'success', 'Phase 2 validation passed', '2024-12-25T15:02:30Z');
