//omni:code --go -test
//omni:key B-L0-foundation-db-test
//omni:version a-01.00

// ============================================================================
// METADATA
// ============================================================================

// Package foundation_test verifies and assesses the foundation database layer.
//
// Purpose: Assessment API for foundation queries — tests ARE the assessment.
//
// These are not disposable tests. They are the assessment API.
// The same Open(), AllSpecs(), BookByName() calls that tests use are the same
// calls a dashboard, CI, or health system would use. Tests happen to run
// through `go test` — assessments happen to run through whatever calls them.
//
// Nine assessment categories:
//
//	A. Connection  — Can we open and close the database? (Open, OpenFromRoot, Close)
//	B. Identity    — Does the package know itself? (Pragma, Metadata)
//	C. Specs       — Can we query specification metadata? (AllSpecs, SpecByKey, SpecsByDomain, SpecCount)
//	D. Types       — Can we query type families and types? (AllFamilies, FamilyByName, AllTypes, TypesByFamily)
//	E. Operations  — Can we query ternary operations? (AllOperations, OperationsByCategory, OperationTable)
//	F. Books       — Can we query Bible books? (AllBooks, BookByOrdinal, BookByName, BookByAbbreviation)
//	G. Scales      — Can we query ternary scales? (AllScales)
//	H. Dependencies — Can we query cross-references? (DependenciesOf)
//	I. WebVariants — Can we query WEB-only verses? (AllWebVariants)
//
// Each category is both a test AND a reusable assessment.
// The pattern: call public API → verify against declared contract → report.
//
// # Core Identity
//
//	Key:     B-L0-foundation-db-test
//	Type:    Assessment (validates AND serves as runtime health API)
//	Version: a-01.00 (2026-02-19)
//
//	Architect:       Seanje Lenox-Wise
//	Implementation:  Nova Dawn
//	Created:         2026-02-19
//
//	Scripture: "Prove all things; hold fast that which is good" — 1 Thessalonians 5:21
//	Anchor:    "By their fruits ye shall know them" — Matthew 7:20
//
// # Version History
//
//   - a-01.00 (2026-02-19) — Initial creation: 23 tests across 9 assessment categories
//
// # Interface
//
//	Needs:   testing, fmt, os, path/filepath | foundation (under test)
//	Tests:   foundation.go — Open, Close, all 8 table readers, Pragma, Metadata
//	Run:     cd b-word && go test -v -count=1 -race ./L0-universal/c-hybrid/spec/database/go/...
package foundation_test

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// ──────────────────────────────────────────────────────────────────────────
// 1. Imports
// ──────────────────────────────────────────────────────────────────────────

//--- I.1 Standard Library ---
import (
	"fmt"
	"os"
	"path/filepath"
	"testing"
)

//--- I.2 Internal Packages ---
import (
	foundation "creativeworkzstudio.com/bereshit/L0-universal/c-hybrid/spec/database/go"
)

// ──────────────────────────────────────────────────────────────────────────
// 3. Variables
// ──────────────────────────────────────────────────────────────────────────

// _pragma — OmniCode file identity for this test file.
// Test files carry identity for linter compliance but do not register
// with the identity registry (tests are verification, not production).
var _pragma = map[string]string{
	"P1.key":     "B-L0-foundation-db-test",
	"P1.type":    "code",
	"P1.format":  "go",
	"P1.style":   "test",
	"P2.blocks":  "4-block",
	"P3.from":    "b-word/seed/code/L0/go/library.go",
	"P4.at":      "a-01.00",
	"P5.title":   "Foundation Database Assessment Suite",
	"P5.summary": "9-category assessment: connection, identity, specs, types, operations, books, scales, deps, web",
}

// _metadata — OmniCode file metadata for this test file.
var _metadata = map[string]string{
	"M1.key":            "B-L0-foundation-db-test",
	"M1.component_type": "Assessment",
	"M1.architect":      "Seanje Lenox-Wise",
	"M1.implementation": "Nova Dawn",
	"M1.created":        "2026-02-19",
	"M6.tags":           "test, assessment, database, foundation, queries",
	"M6.layer":          "L0-universal",
}

// Ensure pragma/metadata are referenced to satisfy the compiler.
var (
	_ = _pragma
	_ = _metadata
)

// testDB is the shared database connection for all assessment tests.
// Opened in TestMain, closed after all tests complete. Read-only access
// is safe for concurrent test execution.
var testDB *foundation.DB

// ──────────────────────────────────────────────────────────────────────────
// 6. Core Types
// ──────────────────────────────────────────────────────────────────────────

// TestMain opens the shared database connection before any tests run.
// All assessment tests share this connection — the database is read-only.
func TestMain(m *testing.M) {
	wd, err := os.Getwd()
	if err != nil {
		fmt.Fprintf(os.Stderr, "foundation_test: os.Getwd: %v\n", err)
		os.Exit(1)
	}

	// cwd = L0-universal/c-hybrid/spec/database/go/
	// Module root (b-word/) is 5 levels up.
	root := filepath.Clean(filepath.Join(wd, "..", "..", "..", "..", ".."))

	testDB, err = foundation.OpenFromRoot(root)
	if err != nil {
		fmt.Fprintf(os.Stderr, "foundation_test: OpenFromRoot(%s): %v\n", root, err)
		os.Exit(1)
	}

	code := m.Run()
	testDB.Close()
	os.Exit(code)
}

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// ──────────────────────────────────────────────────────────────────────────
// A. Connection Assessment
// ──────────────────────────────────────────────────────────────────────────

// TestOpenFromRoot verifies that OpenFromRoot resolves the database path
// from a module root directory and returns a valid connection.
func TestOpenFromRoot(t *testing.T) {
	wd, err := os.Getwd()
	if err != nil {
		t.Fatalf("os.Getwd: %v", err)
	}
	root := filepath.Clean(filepath.Join(wd, "..", "..", "..", "..", ".."))

	db, err := foundation.OpenFromRoot(root)
	if err != nil {
		t.Fatalf("OpenFromRoot(%s): %v", root, err)
	}
	defer db.Close()

	// Verify connection works by running a simple query.
	count, err := db.SpecCount()
	if err != nil {
		t.Fatalf("SpecCount after open: %v", err)
	}
	if count < 1 {
		t.Errorf("SpecCount = %d; want >= 1", count)
	}
}

// TestOpenInvalidPath verifies that Open returns an error for a non-existent path.
func TestOpenInvalidPath(t *testing.T) {
	_, err := foundation.Open("/nonexistent/path/to/nowhere")
	if err == nil {
		t.Fatal("Open(/nonexistent/...): expected error, got nil")
	}
}

// TestCloseReleasesConnection verifies that Close shuts down the connection.
// After closing, subsequent queries should fail.
func TestCloseReleasesConnection(t *testing.T) {
	wd, err := os.Getwd()
	if err != nil {
		t.Fatalf("os.Getwd: %v", err)
	}
	root := filepath.Clean(filepath.Join(wd, "..", "..", "..", "..", ".."))

	db, err := foundation.OpenFromRoot(root)
	if err != nil {
		t.Fatalf("OpenFromRoot: %v", err)
	}

	if err := db.Close(); err != nil {
		t.Fatalf("Close: %v", err)
	}

	// Query after close should fail.
	_, err = db.SpecCount()
	if err == nil {
		t.Error("SpecCount after Close: expected error, got nil")
	}
}

// ──────────────────────────────────────────────────────────────────────────
// B. Identity Assessment
// ──────────────────────────────────────────────────────────────────────────

// TestPragmaIdentity verifies the package Pragma variable contains
// all required OmniCode identity fields (I1-I4).
func TestPragmaIdentity(t *testing.T) {
	pragma := foundation.Pragma
	if len(pragma) < 15 {
		t.Errorf("Pragma: len = %d; want >= 15", len(pragma))
	}

	// Verify I1.key is present and correct.
	found := false
	for _, kv := range pragma {
		if kv[0] == "I1.key" {
			found = true
			if kv[1] != "B-L0-foundation-loader" {
				t.Errorf("Pragma I1.key = %q; want %q", kv[1], "B-L0-foundation-loader")
			}
			break
		}
	}
	if !found {
		t.Error("Pragma: I1.key not found")
	}

	// Verify all 4 identity sections present.
	sections := map[string]bool{"I1": false, "I2": false, "I3": false, "I4": false}
	for _, kv := range pragma {
		for prefix := range sections {
			if len(kv[0]) >= len(prefix) && kv[0][:len(prefix)] == prefix {
				sections[prefix] = true
			}
		}
	}
	for prefix, present := range sections {
		if !present {
			t.Errorf("Pragma: section %s not found", prefix)
		}
	}
}

// TestMetadataContext verifies the package Metadata variable contains
// all required OmniCode context fields (C1-C7).
func TestMetadataContext(t *testing.T) {
	metadata := foundation.Metadata
	if len(metadata) < 19 {
		t.Errorf("Metadata: len = %d; want >= 19", len(metadata))
	}

	// Verify all 7 context sections present.
	sections := map[string]bool{
		"C1": false, "C2": false, "C3": false, "C4": false,
		"C5": false, "C6": false, "C7": false,
	}
	for _, kv := range metadata {
		for prefix := range sections {
			if len(kv[0]) >= len(prefix) && kv[0][:len(prefix)] == prefix {
				sections[prefix] = true
			}
		}
	}
	for prefix, present := range sections {
		if !present {
			t.Errorf("Metadata: section %s not found", prefix)
		}
	}
}

// ──────────────────────────────────────────────────────────────────────────
// C. Spec Assessment
// ──────────────────────────────────────────────────────────────────────────

// TestAllSpecs verifies that the database contains specifications.
func TestAllSpecs(t *testing.T) {
	specs, err := testDB.AllSpecs()
	if err != nil {
		t.Fatalf("AllSpecs: %v", err)
	}
	if len(specs) < 1 {
		t.Error("AllSpecs: no specs returned")
	}

	// Verify each spec has required fields populated.
	for i, s := range specs {
		if s.Key == "" {
			t.Errorf("AllSpecs[%d]: empty Key", i)
		}
		if s.Domain == "" {
			t.Errorf("AllSpecs[%d] (%s): empty Domain", i, s.Key)
		}
	}
}

// TestSpecCount verifies the count matches the actual number of specs.
func TestSpecCount(t *testing.T) {
	count, err := testDB.SpecCount()
	if err != nil {
		t.Fatalf("SpecCount: %v", err)
	}

	specs, err := testDB.AllSpecs()
	if err != nil {
		t.Fatalf("AllSpecs: %v", err)
	}

	if count != len(specs) {
		t.Errorf("SpecCount = %d; AllSpecs returned %d", count, len(specs))
	}
}

// TestSpecByKey verifies round-trip: AllSpecs → pick first → SpecByKey → same data.
func TestSpecByKey(t *testing.T) {
	specs, err := testDB.AllSpecs()
	if err != nil {
		t.Fatalf("AllSpecs: %v", err)
	}
	if len(specs) == 0 {
		t.Skip("no specs in database")
	}

	// Round-trip: use the first spec's key to query directly.
	want := specs[0]
	got, err := testDB.SpecByKey(want.Key)
	if err != nil {
		t.Fatalf("SpecByKey(%s): %v", want.Key, err)
	}
	if got.Key != want.Key {
		t.Errorf("SpecByKey: Key = %q; want %q", got.Key, want.Key)
	}
	if got.ID != want.ID {
		t.Errorf("SpecByKey: ID = %d; want %d", got.ID, want.ID)
	}
}

// TestSpecsByDomain verifies domain-filtered spec queries.
func TestSpecsByDomain(t *testing.T) {
	// Get all specs and find a domain that exists.
	specs, err := testDB.AllSpecs()
	if err != nil {
		t.Fatalf("AllSpecs: %v", err)
	}
	if len(specs) == 0 {
		t.Skip("no specs in database")
	}

	domain := specs[0].Domain
	domainSpecs, err := testDB.SpecsByDomain(domain)
	if err != nil {
		t.Fatalf("SpecsByDomain(%s): %v", domain, err)
	}
	if len(domainSpecs) < 1 {
		t.Errorf("SpecsByDomain(%s): no specs returned", domain)
	}

	// Verify all returned specs belong to the requested domain.
	for i, s := range domainSpecs {
		if s.Domain != domain {
			t.Errorf("SpecsByDomain(%s)[%d]: Domain = %q", domain, i, s.Domain)
		}
	}
}

// TestSpecsByKeys verifies batch lookup of multiple specs by key.
func TestSpecsByKeys(t *testing.T) {
	// Get a few spec keys to batch-query.
	all, err := testDB.AllSpecs()
	if err != nil {
		t.Fatalf("AllSpecs: %v", err)
	}
	if len(all) < 3 {
		t.Skip("fewer than 3 specs in database")
	}

	keys := []string{all[0].Key, all[1].Key, all[2].Key}
	got, err := testDB.SpecsByKeys(keys)
	if err != nil {
		t.Fatalf("SpecsByKeys: %v", err)
	}
	if len(got) != 3 {
		t.Errorf("SpecsByKeys: returned %d; want 3", len(got))
	}

	// Empty keys should return nil.
	empty, err := testDB.SpecsByKeys(nil)
	if err != nil {
		t.Fatalf("SpecsByKeys(nil): %v", err)
	}
	if empty != nil {
		t.Errorf("SpecsByKeys(nil): returned %d; want nil", len(empty))
	}
}

// ──────────────────────────────────────────────────────────────────────────
// D. Type Assessment
// ──────────────────────────────────────────────────────────────────────────

// TestAllFamilies verifies exactly 10 type families exist.
func TestAllFamilies(t *testing.T) {
	families, err := testDB.AllFamilies()
	if err != nil {
		t.Fatalf("AllFamilies: %v", err)
	}
	if len(families) != 10 {
		t.Errorf("AllFamilies: count = %d; want 10", len(families))
	}

	// Each family must have a name.
	for i, f := range families {
		if f.Name == "" {
			t.Errorf("AllFamilies[%d]: empty Name", i)
		}
	}
}

// TestFamilyByName verifies lookup of a known type family.
func TestFamilyByName(t *testing.T) {
	families, err := testDB.AllFamilies()
	if err != nil {
		t.Fatalf("AllFamilies: %v", err)
	}
	if len(families) == 0 {
		t.Skip("no families in database")
	}

	// Round-trip: use first family's name.
	want := families[0]
	got, err := testDB.FamilyByName(want.Name)
	if err != nil {
		t.Fatalf("FamilyByName(%s): %v", want.Name, err)
	}
	if got.Name != want.Name {
		t.Errorf("FamilyByName: Name = %q; want %q", got.Name, want.Name)
	}
	if got.Count != want.Count {
		t.Errorf("FamilyByName(%s): Count = %d; want %d", want.Name, got.Count, want.Count)
	}
}

// TestAllTypes verifies the database contains the expected number of types.
// 23 types defined in Phase 0 — may grow, so use >=.
func TestAllTypes(t *testing.T) {
	types, err := testDB.AllTypes()
	if err != nil {
		t.Fatalf("AllTypes: %v", err)
	}
	if len(types) < 23 {
		t.Errorf("AllTypes: count = %d; want >= 23", len(types))
	}

	// Verify type structure: each type has a name and family.
	for i, ty := range types {
		if ty.Name == "" {
			t.Errorf("AllTypes[%d]: empty Name", i)
		}
		if ty.Family == "" {
			t.Errorf("AllTypes[%d] (%s): empty Family", i, ty.Name)
		}
	}
}

// TestTypesByFamily verifies filtered type queries return correctly.
func TestTypesByFamily(t *testing.T) {
	families, err := testDB.AllFamilies()
	if err != nil {
		t.Fatalf("AllFamilies: %v", err)
	}
	if len(families) == 0 {
		t.Skip("no families in database")
	}

	// Use a family known to have types.
	family := families[0]
	types, err := testDB.TypesByFamily(family.Name)
	if err != nil {
		t.Fatalf("TypesByFamily(%s): %v", family.Name, err)
	}

	// Verify all returned types belong to the requested family.
	for i, ty := range types {
		if ty.Family != family.Name {
			t.Errorf("TypesByFamily(%s)[%d]: Family = %q", family.Name, i, ty.Family)
		}
	}

	// Family.Count should match actual type count for this family.
	if family.Count > 0 && len(types) != family.Count {
		t.Errorf("TypesByFamily(%s): got %d types; family declares Count = %d",
			family.Name, len(types), family.Count)
	}
}

// ──────────────────────────────────────────────────────────────────────────
// E. Operation Assessment
// ──────────────────────────────────────────────────────────────────────────

// TestAllOperations verifies operations are queryable.
func TestAllOperations(t *testing.T) {
	ops, err := testDB.AllOperations()
	if err != nil {
		t.Fatalf("AllOperations: %v", err)
	}
	if len(ops) < 1 {
		t.Error("AllOperations: no operations returned")
	}

	// Each operation must have name, category, and table data.
	for i, o := range ops {
		if o.Name == "" {
			t.Errorf("AllOperations[%d]: empty Name", i)
		}
		if o.Category == "" {
			t.Errorf("AllOperations[%d] (%s): empty Category", i, o.Name)
		}
		if o.TableData == "" {
			t.Errorf("AllOperations[%d] (%s): empty TableData", i, o.Name)
		}
	}
}

// TestOperationsByCategory verifies category-filtered operation queries.
func TestOperationsByCategory(t *testing.T) {
	ops, err := testDB.AllOperations()
	if err != nil {
		t.Fatalf("AllOperations: %v", err)
	}
	if len(ops) == 0 {
		t.Skip("no operations in database")
	}

	// Use the first operation's category.
	category := ops[0].Category
	catOps, err := testDB.OperationsByCategory(category)
	if err != nil {
		t.Fatalf("OperationsByCategory(%s): %v", category, err)
	}
	if len(catOps) < 1 {
		t.Errorf("OperationsByCategory(%s): no operations returned", category)
	}

	// Verify all returned operations belong to the requested category.
	for i, o := range catOps {
		if o.Category != category {
			t.Errorf("OperationsByCategory(%s)[%d]: Category = %q", category, i, o.Category)
		}
	}
}

// TestOperationTable verifies type-safe table parsing for both unary and binary.
// Uses UnaryTable and BinaryTable (type-safe) plus legacy OperationTable (any).
func TestOperationTable(t *testing.T) {
	ops, err := testDB.AllOperations()
	if err != nil {
		t.Fatalf("AllOperations: %v", err)
	}

	// Find a unary operation (InputSize == 1) and a binary operation (InputSize == 2).
	var unary, binary *foundation.Operation
	for i := range ops {
		switch ops[i].InputSize {
		case 1:
			if unary == nil {
				unary = &ops[i]
			}
		case 2:
			if binary == nil {
				binary = &ops[i]
			}
		}
		if unary != nil && binary != nil {
			break
		}
	}

	// Test type-safe UnaryTable.
	if unary != nil {
		table, err := foundation.UnaryTable(unary)
		if err != nil {
			t.Fatalf("UnaryTable(%s): %v", unary.Name, err)
		}
		if len(table) == 0 {
			t.Errorf("UnaryTable(%s): empty table", unary.Name)
		}
		// Ternary tables have exactly 3 entries (for input -1, 0, +1).
		if len(table) != 3 {
			t.Errorf("UnaryTable(%s): len = %d; want 3", unary.Name, len(table))
		}

		// Verify legacy OperationTable returns same data.
		legacy, err := testDB.OperationTable(unary)
		if err != nil {
			t.Fatalf("OperationTable(%s): %v", unary.Name, err)
		}
		legacyTable, ok := legacy.([]int)
		if !ok {
			t.Errorf("OperationTable(%s): expected []int, got %T", unary.Name, legacy)
		} else if len(legacyTable) != len(table) {
			t.Errorf("OperationTable(%s): len mismatch: legacy=%d, typed=%d",
				unary.Name, len(legacyTable), len(table))
		}
	} else {
		t.Error("no unary operations found")
	}

	// Test type-safe BinaryTable.
	if binary != nil {
		table, err := foundation.BinaryTable(binary)
		if err != nil {
			t.Fatalf("BinaryTable(%s): %v", binary.Name, err)
		}
		if len(table) == 0 {
			t.Errorf("BinaryTable(%s): empty table", binary.Name)
		}
		// Binary ternary tables have 3 rows of 3 entries each.
		if len(table) != 3 {
			t.Errorf("BinaryTable(%s): rows = %d; want 3", binary.Name, len(table))
		}
		for i, row := range table {
			if len(row) != 3 {
				t.Errorf("BinaryTable(%s)[%d]: cols = %d; want 3", binary.Name, i, len(row))
			}
		}
	} else {
		t.Error("no binary operations found")
	}

	// Test type mismatch errors.
	if unary != nil {
		_, err := foundation.BinaryTable(unary)
		if err == nil {
			t.Error("BinaryTable on unary op: expected error, got nil")
		}
	}
	if binary != nil {
		_, err := foundation.UnaryTable(binary)
		if err == nil {
			t.Error("UnaryTable on binary op: expected error, got nil")
		}
	}
}

// ──────────────────────────────────────────────────────────────────────────
// F. Book Assessment
// ──────────────────────────────────────────────────────────────────────────

// TestAllBooks verifies exactly 66 Bible books exist.
func TestAllBooks(t *testing.T) {
	books, err := testDB.AllBooks()
	if err != nil {
		t.Fatalf("AllBooks: %v", err)
	}
	if len(books) != 66 {
		t.Errorf("AllBooks: count = %d; want 66", len(books))
	}
}

// TestBookByOrdinal verifies ordinal lookup — Genesis is book 1.
func TestBookByOrdinal(t *testing.T) {
	book, err := testDB.BookByOrdinal(1)
	if err != nil {
		t.Fatalf("BookByOrdinal(1): %v", err)
	}
	if book.Name != "Genesis" {
		t.Errorf("BookByOrdinal(1): Name = %q; want %q", book.Name, "Genesis")
	}
	if book.Abbreviation != "Gen" {
		t.Errorf("BookByOrdinal(1): Abbreviation = %q; want %q", book.Abbreviation, "Gen")
	}
}

// TestBookByName verifies name lookup — "Genesis" returns ordinal 1.
func TestBookByName(t *testing.T) {
	book, err := testDB.BookByName("Genesis")
	if err != nil {
		t.Fatalf("BookByName(Genesis): %v", err)
	}
	if book.ID != 1 {
		t.Errorf("BookByName(Genesis): ID = %d; want 1", book.ID)
	}
	if book.Chapters < 1 {
		t.Errorf("BookByName(Genesis): Chapters = %d; want > 0", book.Chapters)
	}
	if book.Verses < 1 {
		t.Errorf("BookByName(Genesis): Verses = %d; want > 0", book.Verses)
	}
}

// TestBookByAbbreviation verifies abbreviation lookup — "Rev" returns Revelation.
func TestBookByAbbreviation(t *testing.T) {
	book, err := testDB.BookByAbbreviation("Rev")
	if err != nil {
		t.Fatalf("BookByAbbreviation(Rev): %v", err)
	}
	if book.Name != "Revelation" {
		t.Errorf("BookByAbbreviation(Rev): Name = %q; want %q", book.Name, "Revelation")
	}
	if book.ID != 66 {
		t.Errorf("BookByAbbreviation(Rev): ID = %d; want 66", book.ID)
	}
}

// ──────────────────────────────────────────────────────────────────────────
// G. Scale Assessment
// ──────────────────────────────────────────────────────────────────────────

// TestAllScales verifies exactly 6 ternary scales exist (trit → block_243).
func TestAllScales(t *testing.T) {
	scales, err := testDB.AllScales()
	if err != nil {
		t.Fatalf("AllScales: %v", err)
	}
	if len(scales) != 6 {
		t.Errorf("AllScales: count = %d; want 6", len(scales))
	}

	// Verify ordering: trit count should be ascending.
	for i := 1; i < len(scales); i++ {
		if scales[i].TritCount < scales[i-1].TritCount {
			t.Errorf("AllScales: not ordered by trit count — [%d]=%d < [%d]=%d",
				i, scales[i].TritCount, i-1, scales[i-1].TritCount)
		}
	}
}

// ──────────────────────────────────────────────────────────────────────────
// H. Dependency Assessment
// ──────────────────────────────────────────────────────────────────────────

// TestDependenciesOf verifies cross-reference queries work.
// Uses a round-trip: AllSpecs → pick a spec → DependenciesOf.
func TestDependenciesOf(t *testing.T) {
	specs, err := testDB.AllSpecs()
	if err != nil {
		t.Fatalf("AllSpecs: %v", err)
	}
	if len(specs) == 0 {
		t.Skip("no specs in database")
	}

	// Query dependencies for the first spec. Result may be empty
	// (not all specs have dependencies), but the query should not error.
	deps, err := testDB.DependenciesOf(specs[0].Key)
	if err != nil {
		t.Fatalf("DependenciesOf(%s): %v", specs[0].Key, err)
	}

	// If dependencies exist, verify structure.
	for i, d := range deps {
		if d.SpecKey != specs[0].Key {
			t.Errorf("DependenciesOf(%s)[%d]: SpecKey = %q", specs[0].Key, i, d.SpecKey)
		}
		if d.Relation == "" {
			t.Errorf("DependenciesOf(%s)[%d]: empty Relation", specs[0].Key, i)
		}
	}
}

// ──────────────────────────────────────────────────────────────────────────
// I. WebVariant Assessment
// ──────────────────────────────────────────────────────────────────────────

// TestAllWebVariants verifies exactly 13 WEB-only verses exist (trite 243-255).
func TestAllWebVariants(t *testing.T) {
	variants, err := testDB.AllWebVariants()
	if err != nil {
		t.Fatalf("AllWebVariants: %v", err)
	}
	if len(variants) != 13 {
		t.Errorf("AllWebVariants: count = %d; want 13", len(variants))
	}

	// Verify trite values are in the WEB-only range (243-255).
	for i, v := range variants {
		if v.Trite < 243 || v.Trite > 255 {
			t.Errorf("AllWebVariants[%d]: Trite = %d; want 243-255", i, v.Trite)
		}
		if v.Book == "" {
			t.Errorf("AllWebVariants[%d]: empty Book", i)
		}
	}
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================

// ──────────────────────────────────────────────────────────────────────────
// X1: Modification Policy
//
//   Never:
//     - Remove assessment categories (each covers a public API surface)
//     - Change test names (external consumers may reference them)
//     - Weaken assertions (tests ARE the contract)
//
//   Careful:
//     - Changing exact count assertions (only when database schema changes)
//     - Modifying TestMain (affects all tests)
//     - Adding test helpers (must follow assessment API pattern)
//
//   Safe:
//     - Adding new test functions for new query methods
//     - Improving error messages
//     - Adding sub-tests for edge cases
//
// ──────────────────────────────────────────────────────────────────────────

// ──────────────────────────────────────────────────────────────────────────
// X5: Closing Note
//
//   note: 23 tests across 9 categories. Tests ARE the assessment API —
//         the same calls a dashboard, CI, or health system would use.
//
//   scripture: "Prove all things; hold fast that which is good."
//              — 1 Thessalonians 5:21
//
//   anchor: The foundation database is read-only truth materialized from
//           TOML specs. These tests verify the code vertex of The Triangle.
//
// ──────────────────────────────────────────────────────────────────────────

// ============================================================================
// END CLOSING
// ============================================================================
