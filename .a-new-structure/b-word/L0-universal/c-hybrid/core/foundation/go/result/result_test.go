//omni:code --go -demo-test
//omni:key B-L0-foundation-result-test
//omni:version a-02.00

// ============================================================================
// METADATA
// ============================================================================
//
// Key:     B-L0-foundation-result-test
// Purpose: Comprehensive tests for universal result types
// Biblical: Proverbs 16:11 — "A just weight and balance are the LORD's"
// Authors: Nova Dawn
// Version: 2.0.0
// Created: 2026-02-18
// Updated: 2026-02-18

package result

// ──────────────────────────────────────────────────────────────────────────
// Imports (in METADATA — Go requires all imports before any declarations)
// ──────────────────────────────────────────────────────────────────────────

import (
	"encoding/json"
	"testing"
)

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================
//
// section-order: [s01]-imports
// (imports declared in METADATA section — Go language constraint)

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// ──────────────────────────────────────────────────────────────────────────
// 1. Org Chart
// ──────────────────────────────────────────────────────────────────────────
//
// Test Functions (24 tests in Cv zone):
//   Result Constructors: TestOK, TestOKWithData, TestNotFound, TestErr
//   Result Queries:      TestResult_IsOK, TestResult_HasData
//   Result String:       TestResult_String
//   Result JSON:         TestResult_JSON_OmitsNilData, TestResult_JSON_IncludesData
//   LookupResult:        TestFound, TestNoMatch, TestLookupResult_IsFound,
//                        TestLookupResult_HasMatches, TestLookupResult_MatchCount,
//                        TestLookupResult_String
//   ValidationResult:    TestValid, TestInvalid, TestValidationResult_AddError,
//                        TestValidationResult_AddWarning, TestValidationResult_Counts,
//                        TestValidationResult_HasErrors, TestValidationResult_HasWarnings,
//                        TestValidationResult_String, TestValidationResult_JSONRoundtrip

// ──────────────────────────────────────────────────────────────────────────
// 2. Helpers
// ──────────────────────────────────────────────────────────────────────────
// [Reserved: No test helpers needed — all tests use package constructors directly]

// ──────────────────────────────────────────────────────────────────────────
// 3. Core Operations
// ──────────────────────────────────────────────────────────────────────────
// [Reserved: Test logic lives in Cv zone — test functions ARE the operations]

// ──────────────────────────────────────────────────────────────────────────
// 4. Error Handling
// ──────────────────────────────────────────────────────────────────────────
// [Reserved: Tests validate errors from main package, no custom test errors]

// ──────────────────────────────────────────────────────────────────────────
// 5. Test Functions
// ──────────────────────────────────────────────────────────────────────────
// [Reserved: All Test* functions in Cv zone below]

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// ──────────────────────────────────────────────────────────────────────────
// Cv — Validation
// ──────────────────────────────────────────────────────────────────────────
//
// 24 tests covering:
//   - Result constructors: OK, OKWithData, NotFound, Err
//   - Result queries: IsOK, HasData
//   - Result string representation
//   - Result JSON serialization (nil data omission, data inclusion)
//   - LookupResult: Found, NoMatch, queries, string
//   - ValidationResult: Valid, Invalid, mutations, counts, string, JSON roundtrip

// --- Result Constructor Tests ---

func TestOK(t *testing.T) {
	r := OK("success")
	if !r.Found {
		t.Error("OK().Found should be true")
	}
	if r.Message != "success" {
		t.Errorf("Message = %q, want %q", r.Message, "success")
	}
	if r.Data != nil {
		t.Error("OK().Data should be nil")
	}
}

func TestOKWithData(t *testing.T) {
	data := map[string]string{"key": "value"}
	r := OKWithData("found it", data)

	if !r.Found {
		t.Error("OKWithData().Found should be true")
	}
	if r.Data == nil {
		t.Error("OKWithData().Data should not be nil")
	}
}

func TestNotFound(t *testing.T) {
	r := NotFound("resource missing")
	if r.Found {
		t.Error("NotFound().Found should be false")
	}
	if r.Message != "resource missing" {
		t.Errorf("Message = %q", r.Message)
	}
}

func TestErr(t *testing.T) {
	r := Err("something failed")
	if r.Found {
		t.Error("Err().Found should be false")
	}
}

// --- Result Query Tests ---

func TestResult_IsOK(t *testing.T) {
	ok := OK("yes")
	if !ok.IsOK() {
		t.Error("IsOK() should be true for OK result")
	}

	fail := NotFound("no")
	if fail.IsOK() {
		t.Error("IsOK() should be false for NotFound result")
	}
}

func TestResult_HasData(t *testing.T) {
	noData := OK("plain")
	if noData.HasData() {
		t.Error("HasData() should be false for OK without data")
	}

	withData := OKWithData("rich", "payload")
	if !withData.HasData() {
		t.Error("HasData() should be true for OKWithData")
	}
}

// --- Result String Tests ---

func TestResult_String(t *testing.T) {
	ok := OK("it worked")
	if got := ok.String(); got != "OK: it worked" {
		t.Errorf("OK String() = %q", got)
	}

	fail := NotFound("gone")
	if got := fail.String(); got != "FAIL: gone" {
		t.Errorf("NotFound String() = %q", got)
	}
}

// --- Result JSON Tests ---

func TestResult_JSON_OmitsNilData(t *testing.T) {
	r := OK("success")
	data, err := json.Marshal(r)
	if err != nil {
		t.Fatal(err)
	}

	// "data" field should not appear when nil
	var raw map[string]json.RawMessage
	if err := json.Unmarshal(data, &raw); err != nil {
		t.Fatal(err)
	}
	if _, ok := raw["data"]; ok {
		t.Error("JSON should omit data field when nil")
	}
}

func TestResult_JSON_IncludesData(t *testing.T) {
	r := OKWithData("found", "payload")
	data, err := json.Marshal(r)
	if err != nil {
		t.Fatal(err)
	}

	var raw map[string]json.RawMessage
	if err := json.Unmarshal(data, &raw); err != nil {
		t.Fatal(err)
	}
	if _, ok := raw["data"]; !ok {
		t.Error("JSON should include data field when set")
	}
}

// --- LookupResult Tests ---

func TestFound(t *testing.T) {
	items := []string{"a", "b", "c"}
	lr := Found("auth", items, 3)

	if !lr.Found {
		t.Error("Found().Found should be true")
	}
	if lr.Count != 3 {
		t.Errorf("Count = %d, want 3", lr.Count)
	}
	if lr.Query != "auth" {
		t.Errorf("Query = %q, want %q", lr.Query, "auth")
	}
}

func TestNoMatch(t *testing.T) {
	lr := NoMatch("auth", "go")

	if lr.Found {
		t.Error("NoMatch().Found should be false")
	}
	if lr.Format != "go" {
		t.Errorf("Format = %q, want %q", lr.Format, "go")
	}
}

func TestLookupResult_IsFound(t *testing.T) {
	found := Found("q", nil, 1)
	if !found.IsFound() {
		t.Error("IsFound() should be true")
	}

	miss := NoMatch("q", "")
	if miss.IsFound() {
		t.Error("IsFound() should be false for NoMatch")
	}
}

func TestLookupResult_HasMatches(t *testing.T) {
	// Has matches: non-nil data + count > 0
	lr := Found("q", []string{"a"}, 1)
	if !lr.HasMatches() {
		t.Error("HasMatches() should be true")
	}

	// No matches: nil data
	lr2 := Found("q", nil, 0)
	if lr2.HasMatches() {
		t.Error("HasMatches() should be false with nil matches")
	}
}

func TestLookupResult_MatchCount(t *testing.T) {
	lr := Found("q", "data", 5)
	if lr.MatchCount() != 5 {
		t.Errorf("MatchCount() = %d, want 5", lr.MatchCount())
	}
}

func TestLookupResult_String(t *testing.T) {
	found := Found("auth", nil, 3)
	if got := found.String(); got != "Found 3 matches for 'auth'" {
		t.Errorf("found String() = %q", got)
	}

	noMatch := NoMatch("auth", "go")
	if got := noMatch.String(); got != "No matches for 'auth' in format go" {
		t.Errorf("noMatch String() = %q", got)
	}

	noMatchNoFormat := NoMatch("auth", "")
	if got := noMatchNoFormat.String(); got != "No matches for 'auth'" {
		t.Errorf("noMatchNoFormat String() = %q", got)
	}
}

// --- ValidationResult Tests ---

func TestValid(t *testing.T) {
	v := Valid("test.go")
	if !v.Valid {
		t.Error("Valid().Valid should be true")
	}
	if v.FilePath != "test.go" {
		t.Errorf("FilePath = %q", v.FilePath)
	}
}

func TestInvalid(t *testing.T) {
	v := Invalid("test.go", []string{"missing import", "bad format"})
	if v.Valid {
		t.Error("Invalid().Valid should be false")
	}
	if len(v.Errors) != 2 {
		t.Errorf("Errors length = %d, want 2", len(v.Errors))
	}
}

func TestValidationResult_AddError(t *testing.T) {
	v := Valid("test.go")
	if !v.Valid {
		t.Error("should start valid")
	}

	v.AddError("something broke")
	if v.Valid {
		t.Error("should be invalid after AddError")
	}
	if len(v.Errors) != 1 {
		t.Errorf("Errors = %d, want 1", len(v.Errors))
	}
}

func TestValidationResult_AddWarning(t *testing.T) {
	v := Valid("test.go")
	v.AddWarning("minor issue")

	if !v.Valid {
		t.Error("AddWarning should not change Valid status")
	}
	if len(v.Warnings) != 1 {
		t.Errorf("Warnings = %d, want 1", len(v.Warnings))
	}
}

func TestValidationResult_Counts(t *testing.T) {
	v := Valid("test.go")
	v.AddError("err1")
	v.AddError("err2")
	v.AddWarning("warn1")

	if v.ErrorCount() != 2 {
		t.Errorf("ErrorCount() = %d, want 2", v.ErrorCount())
	}
	if v.WarningCount() != 1 {
		t.Errorf("WarningCount() = %d, want 1", v.WarningCount())
	}
	if v.IssueCount() != 3 {
		t.Errorf("IssueCount() = %d, want 3", v.IssueCount())
	}
}

func TestValidationResult_HasErrors(t *testing.T) {
	v := Valid("test.go")
	if v.HasErrors() {
		t.Error("should not have errors initially")
	}

	v.AddError("err")
	if !v.HasErrors() {
		t.Error("should have errors after AddError")
	}
}

func TestValidationResult_HasWarnings(t *testing.T) {
	v := Valid("test.go")
	if v.HasWarnings() {
		t.Error("should not have warnings initially")
	}

	v.AddWarning("warn")
	if !v.HasWarnings() {
		t.Error("should have warnings after AddWarning")
	}
}

func TestValidationResult_String(t *testing.T) {
	valid := Valid("test.go")
	if got := valid.String(); got != "VALID: test.go" {
		t.Errorf("valid String() = %q", got)
	}

	invalid := Invalid("test.go", []string{"err1", "err2", "err3"})
	invalid.AddWarning("warn1")
	expected := "INVALID: test.go (3 errors, 1 warning)"
	if got := invalid.String(); got != expected {
		t.Errorf("invalid String() = %q, want %q", got, expected)
	}
}

func TestValidationResult_JSONRoundtrip(t *testing.T) {
	v := Invalid("test.go", []string{"bad"})
	v.AddWarning("minor")

	data, err := json.Marshal(v)
	if err != nil {
		t.Fatal(err)
	}

	var decoded ValidationResult
	if err := json.Unmarshal(data, &decoded); err != nil {
		t.Fatal(err)
	}

	if decoded.Valid {
		t.Error("Valid should be false after roundtrip")
	}
	if decoded.FilePath != "test.go" {
		t.Errorf("FilePath = %q", decoded.FilePath)
	}
	if len(decoded.Errors) != 1 {
		t.Errorf("Errors = %d, want 1", len(decoded.Errors))
	}
	if len(decoded.Warnings) != 1 {
		t.Errorf("Warnings = %d, want 1", len(decoded.Warnings))
	}
}

//
// ──────────────────────────────────────────────────────────────────────────
// Ce — Execution
// ──────────────────────────────────────────────────────────────────────────
//
// go test -v ./L0-universal/c-hybrid/core/foundation/go/result/
// go test -run TestResult ./L0-universal/c-hybrid/core/foundation/go/result/
//
// ──────────────────────────────────────────────────────────────────────────
// Cc — Cleanup
// ──────────────────────────────────────────────────────────────────────────
//
// No resources to clean up. Pure value-type tests.
//
// ──────────────────────────────────────────────────────────────────────────
// X1: Policy
// ──────────────────────────────────────────────────────────────────────────
//
// Never:
//   - Skip JSON roundtrip tests (contract verification)
//   - Remove constructor coverage (structural completeness)
//
// Careful:
//   - Changing expected String() output (downstream may depend on format)
//
// Safe:
//   - Adding new test cases for edge conditions
//   - Adding tests for new result types
//
// ──────────────────────────────────────────────────────────────────────────
// X5: Note
// ──────────────────────────────────────────────────────────────────────────
//
// Note: Test suite for result package — verifies constructors, queries,
// string representations, and JSON serialization for Result, LookupResult,
// and ValidationResult types.
//
// Scripture: Proverbs 16:11 — "A just weight and balance are the LORD's:
// all the weights of the bag are his work."
//
// ============================================================================
// END CLOSING
// ============================================================================
