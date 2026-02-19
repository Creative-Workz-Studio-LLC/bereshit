//omni:code --go -library
//omni:key B-L0-foundation-result
//omni:version a-02.00

// ============================================================================
// METADATA
// ============================================================================
//
// Key: B-L0-foundation-result
// Purpose: Universal result types for operation outcomes
// Biblical: Proverbs 16:3 - "Commit thy works unto the LORD"
// Authors: Nova Dawn
// Version: 2.0.0
// Created: 2026-02-15
// Updated: 2026-02-18
//
// Promoted from L3-cpisi to L0-universal as shared infrastructure.
// Three result types for three operation patterns:
//   - Result: generic success/failure with optional data
//   - LookupResult: search operations with match count
//   - ValidationResult: validation with error/warning accumulation
//
// ============================================================================

package result

// ──────────────────────────────────────────────────────────────────────────
// Imports (in METADATA — Go requires all imports before any declarations)
// ──────────────────────────────────────────────────────────────────────────

import (
	"fmt"

	"creativeworkzstudio.com/bereshit/L0-universal/b-spiral/identity"
)

// Pragma — OmniCode file identity (I1-I4). Runtime-queryable via PragmaGet().
var Pragma = [][2]string{
	// I1: Core — key, format, from, at
	{"I1.key", "B-L0-foundation-result"},
	{"I1.format", "go"},
	{"I1.from", "b-word/seed/code/L0/go/library.go"},
	{"I1.at", "a-02.00"},
	// I2: Family — type, structure, subtype, role
	{"I2.type", "code"},
	{"I2.structure", "4-block"},
	{"I2.subtype", "library"},
	{"I2.role", "foundation"},
	// I3: Instance — file, title, component, path, provides, brief
	{"I3.file", "result.go"},
	{"I3.title", "Result Types Library"},
	{"I3.component", "result"},
	{"I3.path", "L0-universal/c-hybrid/core/foundation/go/result/result.go"},
	{"I3.provides", "Universal result types for operation outcomes"},
	{"I3.brief", "Three result types: generic, lookup, validation"},
	// I4: Architecture — layer, position, pattern
	{"I4.layer", "L0-universal"},
	{"I4.position", "c-hybrid/core/foundation"},
	{"I4.pattern", "value-types"},
}

// Metadata — OmniCode file metadata (C1-C7). Runtime-queryable via MetadataGet().
var Metadata = [][2]string{
	// C1: State — version, status, created, updated
	{"C1.version", "a-02.00"},
	{"C1.status", "Active"},
	{"C1.created", "2026-02-15"},
	{"C1.updated", "2026-02-18"},
	// C2: Attribution
	{"C2.organization", "CreativeWorkzStudio LLC"},
	{"C2.architect", "Seanje Lenox-Wise"},
	{"C2.implementation", "Nova Dawn"},
	{"C2.copyright", "2025-2026 CreativeWorkzStudio LLC"},
	// C3: Grounding
	{"C3.scripture", "Proverbs 16:3 — Commit thy works unto the LORD"},
	{"C3.principle", "Operations commit to the LORD — every result traced to source"},
	{"C3.anchor", "Genesis 1:1 — In the beginning God created the heaven and the earth"},
	// C4: Dependencies
	{"C4.requires.stdlib", "fmt"},
	{"C4.requires.external", "none"},
	{"C4.requires.internal", "L0-universal/b-spiral/identity"},
	{"C4.consumers", "all packages needing operation result types"},
	{"C4.integration", "import creativeworkzstudio.com/bereshit/L0-universal/c-hybrid/core/foundation/go/result"},
	{"C4.if_missing", "callers must define their own result types"},
	// C5: Intent
	{"C5.purpose", "Standardized operation results across the system"},
	{"C5.philosophy", "Three patterns cover all operation outcomes — generic, search, validation"},
	// C6: Roadmap
	{"C6.current", "a-02.00 — Three result types with constructors, queries, and Display"},
	{"C6.planned", "Error chaining, context propagation"},
	{"C6.limitations", "Data field uses any — no generic type parameters yet"},
	// C7: Classification
	{"C7.tags", "result, operation, lookup, validation, types"},
	{"C7.category", "Foundation"},
	{"C7.domain", "types"},
	{"C7.paradigm", "CPI-SI"},
}

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// --- Core Types ---

// Result represents a generic operation result.
type Result struct {
	Found   bool   `json:"found"`           // Whether the operation found/succeeded
	Message string `json:"message"`         // Human-readable message
	Data    any    `json:"data,omitempty"`  // Optional data payload
}

// LookupResult represents a lookup operation result.
type LookupResult struct {
	Found   bool   `json:"found"`             // Whether lookup found matches
	Query   string `json:"query"`             // What was searched for
	Format  string `json:"format,omitempty"`  // Target format (if applicable)
	Message string `json:"message,omitempty"` // Human-readable message
	Matches any    `json:"matches,omitempty"` // Matching items
	Count   int    `json:"count"`             // Number of matches
}

// ValidationResult represents a validation operation result.
type ValidationResult struct {
	Valid    bool     `json:"valid"`              // Whether validation passed
	Errors   []string `json:"errors,omitempty"`   // List of errors
	Warnings []string `json:"warnings,omitempty"` // List of warnings
	FilePath string   `json:"file_path"`          // File that was validated
}

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
// Identity:
//   init, PragmaGet, MetadataGet
//
// Constructors:
//   Result:           OK, OKWithData, NotFound, Err
//   LookupResult:     Found, NoMatch
//   ValidationResult: Valid, Invalid
//
// Queries:
//   Result:           IsOK, HasData
//   LookupResult:     IsFound, HasMatches, MatchCount
//   ValidationResult: HasErrors, HasWarnings, ErrorCount, WarningCount, IssueCount
//
// Mutators:
//   ValidationResult: AddError, AddWarning

// --- Identity Access ---

// init registers this package in the OmniCode identity registry.
func init() {
	key, ok := PragmaGet("I1.key")
	if !ok {
		return // identity not registerable without key
	}
	identity.Register(key, Pragma, Metadata)
}

// PragmaGet looks up a value from the Pragma identity.
// Returns the value and true if found, empty string and false otherwise.
func PragmaGet(key string) (string, bool) {
	for _, pair := range Pragma {
		if pair[0] == key {
			return pair[1], true
		}
	}
	return "", false
}

// MetadataGet looks up a value from the Metadata.
// Returns the value and true if found, empty string and false otherwise.
func MetadataGet(key string) (string, bool) {
	for _, pair := range Metadata {
		if pair[0] == key {
			return pair[1], true
		}
	}
	return "", false
}

// ──────────────────────────────────────────────────────────────────────────
// 2. Helpers
// ──────────────────────────────────────────────────────────────────────────
// [Reserved: No internal helpers — pure constructors and accessors]

// ──────────────────────────────────────────────────────────────────────────
// 3. Core Operations
// ──────────────────────────────────────────────────────────────────────────

// --- Result Constructors ---

// OK creates a successful result without data.
func OK(message string) Result {
	return Result{
		Found:   true,
		Message: message,
	}
}

// OKWithData creates a successful result with a data payload.
func OKWithData(message string, data any) Result {
	return Result{
		Found:   true,
		Message: message,
		Data:    data,
	}
}

// NotFound creates a not-found result.
func NotFound(message string) Result {
	return Result{
		Found:   false,
		Message: message,
	}
}

// Err creates an error result.
func Err(message string) Result {
	return Result{
		Found:   false,
		Message: message,
	}
}

// --- Result Queries ---

// IsOK returns true if the result represents success.
func (r *Result) IsOK() bool {
	return r.Found
}

// HasData returns true if the result carries a data payload.
func (r *Result) HasData() bool {
	return r.Data != nil
}

// --- LookupResult Constructors ---

// Found creates a successful lookup result with matches.
func Found(query string, matches any, count int) LookupResult {
	return LookupResult{
		Found:   true,
		Query:   query,
		Matches: matches,
		Count:   count,
	}
}

// NoMatch creates an empty lookup result.
func NoMatch(query, format string) LookupResult {
	return LookupResult{
		Found:   false,
		Query:   query,
		Format:  format,
		Message: "No matches found",
	}
}

// --- LookupResult Queries ---

// IsFound returns true if the lookup found results.
func (lr *LookupResult) IsFound() bool {
	return lr.Found
}

// HasMatches returns true if matches is non-nil and count > 0.
func (lr *LookupResult) HasMatches() bool {
	return lr.Matches != nil && lr.Count > 0
}

// MatchCount returns the number of matches (alias for Count field).
func (lr *LookupResult) MatchCount() int {
	return lr.Count
}

// --- ValidationResult Constructors ---

// Valid creates a passing validation result.
func Valid(filePath string) ValidationResult {
	return ValidationResult{
		Valid:    true,
		FilePath: filePath,
	}
}

// Invalid creates a failing validation result with errors.
func Invalid(filePath string, errors []string) ValidationResult {
	return ValidationResult{
		Valid:    false,
		FilePath: filePath,
		Errors:   errors,
	}
}

// --- ValidationResult Mutators ---

// AddError adds an error to the validation result and marks it invalid.
func (v *ValidationResult) AddError(err string) {
	v.Errors = append(v.Errors, err)
	v.Valid = false
}

// AddWarning adds a warning to the validation result.
func (v *ValidationResult) AddWarning(warn string) {
	v.Warnings = append(v.Warnings, warn)
}

// --- ValidationResult Queries ---

// HasErrors returns true if there are validation errors.
func (v *ValidationResult) HasErrors() bool {
	return len(v.Errors) > 0
}

// HasWarnings returns true if there are validation warnings.
func (v *ValidationResult) HasWarnings() bool {
	return len(v.Warnings) > 0
}

// ErrorCount returns the number of errors.
func (v *ValidationResult) ErrorCount() int {
	return len(v.Errors)
}

// WarningCount returns the number of warnings.
func (v *ValidationResult) WarningCount() int {
	return len(v.Warnings)
}

// IssueCount returns total number of issues (errors + warnings).
func (v *ValidationResult) IssueCount() int {
	return v.ErrorCount() + v.WarningCount()
}

// ──────────────────────────────────────────────────────────────────────────
// 4. Error Handling
// ──────────────────────────────────────────────────────────────────────────
// [Reserved: Result types ARE the error handling mechanism]

// ──────────────────────────────────────────────────────────────────────────
// 5. Public APIs
// ──────────────────────────────────────────────────────────────────────────

// --- String Methods ---

// String returns a human-readable representation of the result.
//
//	"OK: operation completed"
//	"FAIL: resource not found"
func (r Result) String() string {
	if r.Found {
		return fmt.Sprintf("OK: %s", r.Message)
	}
	return fmt.Sprintf("FAIL: %s", r.Message)
}

// String returns a human-readable representation of the lookup result.
//
//	"Found 3 matches for 'auth'"
//	"No matches for 'auth' in format go"
func (lr LookupResult) String() string {
	if lr.Found {
		return fmt.Sprintf("Found %d matches for '%s'", lr.Count, lr.Query)
	}
	if lr.Format != "" {
		return fmt.Sprintf("No matches for '%s' in format %s", lr.Query, lr.Format)
	}
	return fmt.Sprintf("No matches for '%s'", lr.Query)
}

// String returns a human-readable representation of the validation result.
//
//	"VALID: path/to/file.go"
//	"INVALID: path/to/file.go (3 errors, 1 warning)"
func (v ValidationResult) String() string {
	if v.Valid {
		return fmt.Sprintf("VALID: %s", v.FilePath)
	}
	errWord := "errors"
	if v.ErrorCount() == 1 {
		errWord = "error"
	}
	warnWord := "warnings"
	if v.WarningCount() == 1 {
		warnWord = "warning"
	}
	return fmt.Sprintf("INVALID: %s (%d %s, %d %s)", v.FilePath, v.ErrorCount(), errWord, v.WarningCount(), warnWord)
}

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
// Testing: go test -v ./L0-universal/.../result/
//
// Verify:
//   - OK() creates Result with Found=true and nil Data
//   - OKWithData() creates Result with Found=true and non-nil Data
//   - JSON roundtrip: Data field omitted when nil (omitempty)
//   - AddError sets Valid=false
//   - IssueCount = ErrorCount + WarningCount
//
// ──────────────────────────────────────────────────────────────────────────
// Ce — Execution
// ──────────────────────────────────────────────────────────────────────────
//
// Library — imported, not executed.
// import "creativeworkzstudio.com/bereshit/L0-universal/c-hybrid/core/foundation/go/result"
//
// ──────────────────────────────────────────────────────────────────────────
// Cc — Cleanup
// ──────────────────────────────────────────────────────────────────────────
//
// No resources to clean up. Pure value types.
//
// ──────────────────────────────────────────────────────────────────────────
// X1: Policy
// ──────────────────────────────────────────────────────────────────────────
//
// Never:
//   - Merge OK and OKWithData back into single function — Rust parity
//   - Use interface{} instead of any — Go 1.18+ standard
//   - Add filesystem I/O — types only
//
// Careful:
//   - Changing struct field names — breaks JSON contract
//   - Changing Found semantics — affects all consumers
//
// Safe:
//   - Adding new query methods
//   - Adding new result constructors
//   - Adding new fields with omitempty
//
// ──────────────────────────────────────────────────────────────────────────
// X5: Note
// ──────────────────────────────────────────────────────────────────────────
//
// Note: Universal result types promoted from L3 to L0. Three patterns:
// Result (generic), LookupResult (search), ValidationResult (validation).
// Split OK/OKWithData matches Rust's OpResult::ok/ok_with_data.
//
// Scripture: Proverbs 16:3 — "Commit thy works unto the LORD, and thy
// thoughts shall be established."
//
// ============================================================================
// END CLOSING
// ============================================================================
