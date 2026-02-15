//omni:code --go -library
//omni:key B-L0-config-util-errors
//omni:version b-01.00

// ============================================================================
// METADATA
// ============================================================================
//
// Key: B-L0-config-util-errors
// Purpose: Shared error types for config loading — LoadError, ValidationError
// Biblical: Proverbs 27:6 - "Faithful are the wounds of a friend"
// Authors: Nova Dawn
// Version: b-01.00
// Created: 2026-02-13
//
// Extracted from: 3 LoadError copies + 2 ValidationError copies
// Replaces:
//   - L0-universal/hybrid/config/loader.go (LoadError, ValidationError)
//   - L3-cpisi/orchestration/config/loader.go (LoadError, ConfigError)
//   - L3-cpisi/orchestration/config/config.go (SchemaError, JSONCError)
//
// Design:
//   - LoadError: file-level loading failures (read, parse, validate)
//   - ValidationError: field-level validation problems (wrong type, missing, invalid)
//   - Domain-specific errors (DependencyError, ConfigError) stay in their packages
//     and can embed these shared types for consistency
//
// ============================================================================
// END METADATA
// ============================================================================

package util

// ============================================================================
// SETUP
// ============================================================================

import (
	"errors"
	"fmt"
)

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// Error Types
// ────────────────────────────────────────────────────────────────

// LoadError wraps file-level loading failures.
// Format: "load math/ternary.toml: parse: unexpected EOF"
//
// Implements error and Unwrap() for errors.Is/As chains.
type LoadError struct {
	File string // file path, e.g., "math/ternary.toml" or "config.toml"
	Op   string // operation: "read", "parse", "validate", "stat", "find", "detect"
	Err  error  // underlying error
}

func (e *LoadError) Error() string {
	return fmt.Sprintf("load %s: %s: %v", e.File, e.Op, e.Err)
}

func (e *LoadError) Unwrap() error { return e.Err }

// ValidationError represents a field-level validation failure.
// Tiered format based on specificity:
//   - Full:    "types/primitives.toml [int9.min_value]: expected int, got string (got: 42)"
//   - Section: "types/primitives.toml [int9]: missing required fields"
//   - File:    "types/primitives.toml: empty file"
//
// When Value is set, it appears in the message for diagnostic clarity.
type ValidationError struct {
	Path    string // file path, e.g., "types/primitives.toml"
	Section string // TOML/JSON section, e.g., "int9"
	Field   string // field name, e.g., "min_value"
	Message string // what's wrong
	Value   any    // optional: the offending value
}

func (e *ValidationError) Error() string {
	base := e.baseMessage()
	if e.Value != nil {
		return fmt.Sprintf("%s (got: %v)", base, e.Value)
	}
	return base
}

func (e *ValidationError) baseMessage() string {
	if e.Field != "" {
		return fmt.Sprintf("%s [%s.%s]: %s", e.Path, e.Section, e.Field, e.Message)
	}
	if e.Section != "" {
		return fmt.Sprintf("%s [%s]: %s", e.Path, e.Section, e.Message)
	}
	return fmt.Sprintf("%s: %s", e.Path, e.Message)
}

// ────────────────────────────────────────────────────────────────
// Constructors
// ────────────────────────────────────────────────────────────────

// NewLoadError creates a LoadError for file-level failures.
func NewLoadError(file, op string, err error) *LoadError {
	return &LoadError{File: file, Op: op, Err: err}
}

// NewValidationError creates a ValidationError for field-level problems.
func NewValidationError(path, section, field, message string) *ValidationError {
	return &ValidationError{
		Path:    path,
		Section: section,
		Field:   field,
		Message: message,
	}
}

// NewValidationErrorWithValue creates a ValidationError that includes the
// offending value in the error message. Use when the caller has the value
// that failed validation — makes debugging faster.
func NewValidationErrorWithValue(path, section, field, message string, value any) *ValidationError {
	return &ValidationError{
		Path:    path,
		Section: section,
		Field:   field,
		Message: message,
		Value:   value,
	}
}

// ────────────────────────────────────────────────────────────────
// Error Inspection
// ────────────────────────────────────────────────────────────────

// AsLoadError extracts a *LoadError from an error chain.
// Returns nil if the chain doesn't contain a LoadError.
//
// Shorthand for errors.As — saves the caller from declaring the target variable.
func AsLoadError(err error) *LoadError {
	var target *LoadError
	if errors.As(err, &target) {
		return target
	}
	return nil
}

// AsValidationError extracts a *ValidationError from an error chain.
// Returns nil if the chain doesn't contain a ValidationError.
func AsValidationError(err error) *ValidationError {
	var target *ValidationError
	if errors.As(err, &target) {
		return target
	}
	return nil
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// "Faithful are the wounds of a friend; but the kisses of an enemy
// are deceitful." — Proverbs 27:6
//
// Errors are friends. They tell you the truth about what went wrong.
// LoadError names the file and operation. ValidationError names the field.
// Honest diagnosis before treatment.
//
// ============================================================================
// END CLOSING
// ============================================================================
