// ============================================================================
// METADATA
// ============================================================================
//
// Key: claude-global-pkg-result
// Purpose: Common result types for skill operations
// Biblical: Proverbs 16:3 - "Commit thy works unto the LORD"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2025-12-10
//
// Used by: Skills that return operation results
//
// ============================================================================

package result

// ============================================================================
// SETUP
// ============================================================================

// Result represents a generic operation result
type Result struct {
	Found   bool        // Whether the operation found/succeeded
	Message string      // Human-readable message
	Data    interface{} // Optional data payload
}

// LookupResult represents a lookup operation result
type LookupResult struct {
	Found    bool        // Whether lookup found matches
	Query    string      // What was searched for
	Format   string      // Target format (if applicable)
	Message  string      // Human-readable message
	Matches  interface{} // Matching items
	Count    int         // Number of matches
}

// ValidationResult represents a validation operation result
type ValidationResult struct {
	Valid    bool     // Whether validation passed
	Errors   []string // List of errors
	Warnings []string // List of warnings
	FilePath string   // File that was validated
}

// ============================================================================
// BODY
// ============================================================================

// --- Result Constructors ---

// OK creates a successful result
func OK(message string, data interface{}) Result {
	return Result{
		Found:   true,
		Message: message,
		Data:    data,
	}
}

// NotFound creates a not-found result
func NotFound(message string) Result {
	return Result{
		Found:   false,
		Message: message,
	}
}

// Error creates an error result
func Error(message string) Result {
	return Result{
		Found:   false,
		Message: message,
	}
}

// --- LookupResult Constructors ---

// Found creates a successful lookup result
func Found(query string, matches interface{}, count int) LookupResult {
	return LookupResult{
		Found:   true,
		Query:   query,
		Matches: matches,
		Count:   count,
	}
}

// NoMatch creates an empty lookup result
func NoMatch(query, format string) LookupResult {
	return LookupResult{
		Found:   false,
		Query:   query,
		Format:  format,
		Message: "No matches found",
	}
}

// --- ValidationResult Constructors ---

// Valid creates a passing validation result
func Valid(filePath string) ValidationResult {
	return ValidationResult{
		Valid:    true,
		FilePath: filePath,
	}
}

// Invalid creates a failing validation result
func Invalid(filePath string, errors []string) ValidationResult {
	return ValidationResult{
		Valid:    false,
		FilePath: filePath,
		Errors:   errors,
	}
}

// --- ValidationResult Methods ---

// AddError adds an error to validation result
func (v *ValidationResult) AddError(err string) {
	v.Errors = append(v.Errors, err)
	v.Valid = false
}

// AddWarning adds a warning to validation result
func (v *ValidationResult) AddWarning(warn string) {
	v.Warnings = append(v.Warnings, warn)
}

// HasErrors returns true if there are errors
func (v *ValidationResult) HasErrors() bool {
	return len(v.Errors) > 0
}

// HasWarnings returns true if there are warnings
func (v *ValidationResult) HasWarnings() bool {
	return len(v.Warnings) > 0
}

// ============================================================================
// CLOSING
// ============================================================================
