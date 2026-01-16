-- seed-registry.sql
-- Seed data for registry.db from word/core/ TOML specifications
-- Biblical anchor: "In the beginning was the Word" (John 1:1)
-- The Word defines all things - specs define implementations

-- ═══════════════════════════════════════════════════════════════════════════
-- SPECS - Specification files from word/core/index.toml
-- ═══════════════════════════════════════════════════════════════════════════

-- System 0: Math (foundation)
INSERT INTO specs (key, path, name, version, created_at, updated_at, description, status)
VALUES ('math-ternary', 'math/ternary.toml', 'Ternary Math Constants', 'a-02.00', '2025-12-11', '2025-12-14', 'Trit values, packing, arithmetic, half/full adder, temporal states', 'active');

-- System 1: Types
INSERT INTO specs (key, path, name, version, created_at, updated_at, description, status)
VALUES ('types-primitives', 'types/primitives.toml', 'OmniCode Primitive Types', 'a-02.00', '2025-12-11', '2025-12-14', 'Atomic types: trit, trit5, int9, int27, text, bool3, collections', 'active');

INSERT INTO specs (key, path, name, version, created_at, updated_at, description, status)
VALUES ('types-composition', 'types/composition.toml', 'Type Composition', 'a-01.00', '2025-12-11', '2025-12-14', 'Type composition — nullability triad, storage alignment', 'active');

INSERT INTO specs (key, path, name, version, created_at, updated_at, description, status)
VALUES ('types-validation', 'types/validation.toml', 'Type Validation', 'a-01.00', '2025-12-11', '2025-12-14', 'Validation constants — error severity, exit codes', 'active');

-- System 2: Language
INSERT INTO specs (key, path, name, version, created_at, updated_at, description, status)
VALUES ('language-keywords', 'language/keywords.toml', 'OmniCode Keywords', 'a-01.00', '2025-12-11', '2025-12-14', 'Reserved words, operators, punctuation', 'active');

INSERT INTO specs (key, path, name, version, created_at, updated_at, description, status)
VALUES ('language-syntax', 'language/syntax.toml', 'OmniCode Syntax', 'a-01.00', '2025-12-11', '2025-12-14', 'Grammar rules and statement structures', 'active');

-- System 3: Bible
INSERT INTO specs (key, path, name, version, created_at, updated_at, description, status)
VALUES ('bible-scripture-text', 'bible/scripture-text.toml', 'Scripture Text Encoding', 'a-01.00', '2025-12-11', '2025-12-14', 'Character encoding for scripture text (ASCII subset + extended)', 'active');

INSERT INTO specs (key, path, name, version, created_at, updated_at, description, status)
VALUES ('bible-addressing', 'bible/addressing.toml', 'Bible Addressing', 'a-01.00', '2025-12-11', '2025-12-14', 'Verse addressing — book/chapter/verse to ternary ordinal', 'active');

INSERT INTO specs (key, path, name, version, created_at, updated_at, description, status)
VALUES ('bible-encoding', 'bible/encoding.toml', 'Bible Encoding', 'a-01.00', '2025-12-11', '2025-12-14', 'How scripture text is encoded in trit-bytes', 'active');

INSERT INTO specs (key, path, name, version, created_at, updated_at, description, status)
VALUES ('bible-decoding', 'bible/decoding.toml', 'Bible Decoding', 'a-01.00', '2025-12-11', '2025-12-14', 'How encoded scripture is decoded back to text', 'active');

INSERT INTO specs (key, path, name, version, created_at, updated_at, description, status)
VALUES ('bible-translation', 'bible/translation.toml', 'Bible Translation', 'a-01.00', '2025-12-11', '2025-12-14', 'KJV base + WEB variant marking system, Hebrew/Greek gematria tables', 'active');

INSERT INTO specs (key, path, name, version, created_at, updated_at, description, status)
VALUES ('bible-encryption', 'bible/encryption.toml', 'Bible Encryption', 'a-01.00', '2025-12-11', '2025-12-14', 'Letter → Position/Value transliteration (Hebrew + Greek gematria)', 'active');

INSERT INTO specs (key, path, name, version, created_at, updated_at, description, status)
VALUES ('bible-decryption', 'bible/decryption.toml', 'Bible Decryption', 'a-01.00', '2025-12-11', '2025-12-14', 'Position/Value → Letter transliteration (reverse of encryption)', 'active');

-- System 4: Health
INSERT INTO specs (key, path, name, version, created_at, updated_at, description, status)
VALUES ('health-score', 'os/health/score.toml', 'Health Score', 'a-01.00', '2025-12-11', '2025-12-14', 'Health score constants (-100 to +100 ternary mapping)', 'active');

INSERT INTO specs (key, path, name, version, created_at, updated_at, description, status)
VALUES ('health-diagnostics', 'os/health/diagnostics.toml', 'Health Diagnostics', 'a-01.00', '2025-12-11', '2025-12-14', 'Diagnostic categories and severity levels', 'active');

INSERT INTO specs (key, path, name, version, created_at, updated_at, description, status)
VALUES ('health-log', 'os/health/log.toml', 'Health Log', 'a-01.00', '2025-12-11', '2025-12-14', 'Health log format and retention', 'active');

INSERT INTO specs (key, path, name, version, created_at, updated_at, description, status)
VALUES ('health-provider', 'os/health/provider.toml', 'Health Provider', 'a-01.00', '2025-12-11', '2025-12-14', 'Health provider interface contract', 'active');

-- System 5: Permission
INSERT INTO specs (key, path, name, version, created_at, updated_at, description, status)
VALUES ('permission-access', 'os/permission/access.toml', 'Permission Access', 'a-01.00', '2025-12-11', '2025-12-14', 'Access levels and permission constants', 'active');

-- System 6: Identity
INSERT INTO specs (key, path, name, version, created_at, updated_at, description, status)
VALUES ('identity-model', 'identity/model.toml', 'Identity Model', 'a-01.00', '2025-12-11', '2025-12-14', 'Identity model fields and structure', 'active');

INSERT INTO specs (key, path, name, version, created_at, updated_at, description, status)
VALUES ('identity-contract', 'identity/contract.toml', 'Identity Contract', 'a-01.00', '2025-12-11', '2025-12-14', 'Identity contract interface', 'active');

-- System 7: Network
INSERT INTO specs (key, path, name, version, created_at, updated_at, description, status)
VALUES ('network-message', 'network/message.toml', 'Network Message', 'a-01.00', '2025-12-11', '2025-12-14', 'Message format constants and types', 'active');

INSERT INTO specs (key, path, name, version, created_at, updated_at, description, status)
VALUES ('network-timestamp', 'network/timestamp.toml', 'Network Timestamp', 'a-01.00', '2025-12-11', '2025-12-14', 'Ternary timestamp format', 'active');

INSERT INTO specs (key, path, name, version, created_at, updated_at, description, status)
VALUES ('network-contract', 'network/contract.toml', 'Network Contract', 'a-01.00', '2025-12-11', '2025-12-14', 'Network contract interface', 'active');

-- System 8: Filesystem
INSERT INTO specs (key, path, name, version, created_at, updated_at, description, status)
VALUES ('filesystem-types', 'filesystem/types.toml', 'Filesystem Types', 'a-01.00', '2025-12-11', '2025-12-14', 'Filesystem type constants', 'active');

INSERT INTO specs (key, path, name, version, created_at, updated_at, description, status)
VALUES ('filesystem-contract', 'filesystem/contract.toml', 'Filesystem Contract', 'a-01.00', '2025-12-11', '2025-12-14', 'Filesystem contract interface', 'active');

-- ═══════════════════════════════════════════════════════════════════════════
-- TYPES - Type definitions from primitives.toml
-- ═══════════════════════════════════════════════════════════════════════════

-- Get spec_id for types-primitives (will be 2)
-- Trit types
INSERT INTO types (spec_id, key, name, category, size_bits, description, created_at)
SELECT id, 'trit', 'trit', 'primitive', 2, 'Single balanced ternary digit — the atom of ternary computing', datetime('now')
FROM specs WHERE key = 'types-primitives';

INSERT INTO types (spec_id, key, name, category, size_bits, description, created_at)
SELECT id, 'trit5', 'trit5', 'packed', 8, '5-trit packed value — fits in one byte like a single letter', datetime('now')
FROM specs WHERE key = 'types-primitives';

-- Integer types
INSERT INTO types (spec_id, key, name, category, size_bits, description, created_at)
SELECT id, 'int9', 'int9', 'primitive', 16, 'Signed 9-trit integer — like a small number with positive/negative', datetime('now')
FROM specs WHERE key = 'types-primitives';

INSERT INTO types (spec_id, key, name, category, size_bits, description, created_at)
SELECT id, 'uint9', 'uint9', 'primitive', 16, 'Unsigned 9-trit integer — counts from zero upward', datetime('now')
FROM specs WHERE key = 'types-primitives';

INSERT INTO types (spec_id, key, name, category, size_bits, description, created_at)
SELECT id, 'int27', 'int27', 'primitive', 48, 'Signed 27-trit integer — Word size', datetime('now')
FROM specs WHERE key = 'types-primitives';

INSERT INTO types (spec_id, key, name, category, size_bits, description, created_at)
SELECT id, 'uint27', 'uint27', 'primitive', 48, 'Unsigned 27-trit integer — Word size, counts from zero', datetime('now')
FROM specs WHERE key = 'types-primitives';

-- Float type
INSERT INTO types (spec_id, key, name, category, size_bits, description, created_at)
SELECT id, 'float27', 'float27', 'primitive', 48, '27-trit decimal number — for measures like weight, temperature, distance', datetime('now')
FROM specs WHERE key = 'types-primitives';

-- Text types
INSERT INTO types (spec_id, key, name, category, size_bits, description, created_at)
SELECT id, 'text', 'text', 'primitive', NULL, 'Text string — words carrying meaning like Scripture carries truth', datetime('now')
FROM specs WHERE key = 'types-primitives';

INSERT INTO types (spec_id, key, name, category, size_bits, description, created_at)
SELECT id, 'char', 'char', 'primitive', 48, 'Single character — like Hebrew letters, each with meaning', datetime('now')
FROM specs WHERE key = 'types-primitives';

-- Boolean type
INSERT INTO types (spec_id, key, name, category, size_bits, description, created_at)
SELECT id, 'bool3', 'bool3', 'primitive', 2, 'Ternary truth — false, unknown, or true (honest about uncertainty)', datetime('now')
FROM specs WHERE key = 'types-primitives';

-- Collection types
INSERT INTO types (spec_id, key, name, category, size_bits, description, created_at)
SELECT id, 'list', 'list', 'compound', NULL, 'Ordered collection — like genealogy lists in Scripture', datetime('now')
FROM specs WHERE key = 'types-primitives';

INSERT INTO types (spec_id, key, name, category, size_bits, description, created_at)
SELECT id, 'map', 'map', 'compound', NULL, 'Key-value lookup — like census records mapping names to counts', datetime('now')
FROM specs WHERE key = 'types-primitives';

INSERT INTO types (spec_id, key, name, category, size_bits, description, created_at)
SELECT id, 'set', 'set', 'compound', NULL, 'Unique collection — like tribes where each member is distinct', datetime('now')
FROM specs WHERE key = 'types-primitives';

-- Special types
INSERT INTO types (spec_id, key, name, category, size_bits, description, created_at)
SELECT id, 'void', 'void', 'primitive', 0, 'Absence of value — tohu, formless before creation', datetime('now')
FROM specs WHERE key = 'types-primitives';

INSERT INTO types (spec_id, key, name, category, size_bits, description, created_at)
SELECT id, 'any', 'any', 'primitive', NULL, 'Any type — like whatsoever Adam called every creature', datetime('now')
FROM specs WHERE key = 'types-primitives';

-- Binary storage types
INSERT INTO types (spec_id, key, name, category, size_bits, description, created_at)
SELECT id, 'uint8', 'uint8', 'primitive', 8, 'Unsigned 8-bit binary integer — storage layer', datetime('now')
FROM specs WHERE key = 'types-primitives';

INSERT INTO types (spec_id, key, name, category, size_bits, description, created_at)
SELECT id, 'int8', 'int8', 'primitive', 8, 'Signed 8-bit binary integer — storage layer', datetime('now')
FROM specs WHERE key = 'types-primitives';

-- ═══════════════════════════════════════════════════════════════════════════
-- DEPENDENCIES - Spec dependency graph from index.toml
-- ═══════════════════════════════════════════════════════════════════════════

-- types-primitives depends on math-ternary
INSERT INTO dependencies (spec_id, depends_on_id, dependency_type, created_at)
SELECT s1.id, s2.id, 'requires', datetime('now')
FROM specs s1, specs s2
WHERE s1.key = 'types-primitives' AND s2.key = 'math-ternary';

-- types-composition depends on types-primitives
INSERT INTO dependencies (spec_id, depends_on_id, dependency_type, created_at)
SELECT s1.id, s2.id, 'requires', datetime('now')
FROM specs s1, specs s2
WHERE s1.key = 'types-composition' AND s2.key = 'types-primitives';

-- types-validation depends on types-primitives and types-composition
INSERT INTO dependencies (spec_id, depends_on_id, dependency_type, created_at)
SELECT s1.id, s2.id, 'requires', datetime('now')
FROM specs s1, specs s2
WHERE s1.key = 'types-validation' AND s2.key = 'types-primitives';

INSERT INTO dependencies (spec_id, depends_on_id, dependency_type, created_at)
SELECT s1.id, s2.id, 'requires', datetime('now')
FROM specs s1, specs s2
WHERE s1.key = 'types-validation' AND s2.key = 'types-composition';

-- language-keywords depends on math-ternary
INSERT INTO dependencies (spec_id, depends_on_id, dependency_type, created_at)
SELECT s1.id, s2.id, 'requires', datetime('now')
FROM specs s1, specs s2
WHERE s1.key = 'language-keywords' AND s2.key = 'math-ternary';

-- language-syntax depends on language-keywords and types-primitives
INSERT INTO dependencies (spec_id, depends_on_id, dependency_type, created_at)
SELECT s1.id, s2.id, 'requires', datetime('now')
FROM specs s1, specs s2
WHERE s1.key = 'language-syntax' AND s2.key = 'language-keywords';

INSERT INTO dependencies (spec_id, depends_on_id, dependency_type, created_at)
SELECT s1.id, s2.id, 'requires', datetime('now')
FROM specs s1, specs s2
WHERE s1.key = 'language-syntax' AND s2.key = 'types-primitives';

-- health-score depends on math-ternary and types-primitives
INSERT INTO dependencies (spec_id, depends_on_id, dependency_type, created_at)
SELECT s1.id, s2.id, 'requires', datetime('now')
FROM specs s1, specs s2
WHERE s1.key = 'health-score' AND s2.key = 'math-ternary';

INSERT INTO dependencies (spec_id, depends_on_id, dependency_type, created_at)
SELECT s1.id, s2.id, 'requires', datetime('now')
FROM specs s1, specs s2
WHERE s1.key = 'health-score' AND s2.key = 'types-primitives';

-- ═══════════════════════════════════════════════════════════════════════════
-- OPERATIONS - Key operations from math/ternary.toml
-- ═══════════════════════════════════════════════════════════════════════════

INSERT INTO operations (spec_id, key, name, operation_type, description, created_at)
SELECT id, 'half_adder', 'Half Adder', 'table', '(a,b) → (sum, carry) for single trit addition', datetime('now')
FROM specs WHERE key = 'math-ternary';

INSERT INTO operations (spec_id, key, name, operation_type, description, created_at)
SELECT id, 'full_adder', 'Full Adder', 'table', '(a,b,c_in) → (sum, c_out) for carry-chain addition', datetime('now')
FROM specs WHERE key = 'math-ternary';

INSERT INTO operations (spec_id, key, name, operation_type, description, created_at)
SELECT id, 'negation', 'Negation', 'table', 'Flip trit sign: -1↔+1, 0→0', datetime('now')
FROM specs WHERE key = 'math-ternary';

INSERT INTO operations (spec_id, key, name, operation_type, description, created_at)
SELECT id, 'multiplication', 'Multiplication', 'table', 'Single trit multiplication table', datetime('now')
FROM specs WHERE key = 'math-ternary';

INSERT INTO operations (spec_id, key, name, operation_type, description, created_at)
SELECT id, 'pack', 'Pack', 'algorithm', 'Trit[] → Integer using Horner method', datetime('now')
FROM specs WHERE key = 'math-ternary';

INSERT INTO operations (spec_id, key, name, operation_type, description, created_at)
SELECT id, 'unpack', 'Unpack', 'algorithm', 'Integer → Trit[] using repeated division', datetime('now')
FROM specs WHERE key = 'math-ternary';

-- ═══════════════════════════════════════════════════════════════════════════
-- CONSTANTS - Key constants from specs
-- ═══════════════════════════════════════════════════════════════════════════

-- Trit constants
INSERT INTO constants (spec_id, key, value, value_type, description, created_at)
SELECT id, 'trit_values', '[-1, 0, 1]', 'array', 'The three balanced ternary values', datetime('now')
FROM specs WHERE key = 'math-ternary';

INSERT INTO constants (spec_id, key, value, value_type, description, created_at)
SELECT id, 'trit_states', '3', 'integer', 'Number of states in a single trit', datetime('now')
FROM specs WHERE key = 'math-ternary';

INSERT INTO constants (spec_id, key, value, value_type, description, created_at)
SELECT id, 'identity_sum', '0', 'integer', '-1 + 0 + 1 = 0 (the balanced equation)', datetime('now')
FROM specs WHERE key = 'math-ternary';

-- Packing constants
INSERT INTO constants (spec_id, key, value, value_type, description, created_at)
SELECT id, 'trits_per_byte', '5', 'integer', '5 trits fit in one byte (3^5 = 243 < 256)', datetime('now')
FROM specs WHERE key = 'math-ternary';

INSERT INTO constants (spec_id, key, value, value_type, description, created_at)
SELECT id, 'trit5_max', '243', 'integer', '3^5 = 243 states in a packed trit5', datetime('now')
FROM specs WHERE key = 'math-ternary';

INSERT INTO constants (spec_id, key, value, value_type, description, created_at)
SELECT id, 'spare_states', '13', 'integer', '256 - 243 = 13 spare states for WEB variants', datetime('now')
FROM specs WHERE key = 'math-ternary';

-- Type range constants
INSERT INTO constants (spec_id, key, value, value_type, description, created_at)
SELECT id, 'trit5_range', '[-121, 121]', 'array', 'Balanced range for trit5 type', datetime('now')
FROM specs WHERE key = 'types-primitives';

INSERT INTO constants (spec_id, key, value, value_type, description, created_at)
SELECT id, 'int9_range', '[-9841, 9841]', 'array', 'Balanced range for int9 type', datetime('now')
FROM specs WHERE key = 'types-primitives';

INSERT INTO constants (spec_id, key, value, value_type, description, created_at)
SELECT id, 'int27_range', '[-3812798742493, 3812798742493]', 'array', 'Balanced range for int27 (Word size)', datetime('now')
FROM specs WHERE key = 'types-primitives';
