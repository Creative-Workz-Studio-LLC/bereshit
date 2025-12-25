// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: claude-global-pkg-validation
// Purpose: Health-aware validation for CPI-SI - supports immune system paradigm
// Biblical: 1 Thessalonians 5:21 - "Prove all things; hold fast that which is good"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2025-12-10
//
// Design Goals:
//   - Health-scored validation results
//   - Composable validator chains
//   - Clear error messages
//   - Support for maps, slices, and primitives
//
// ═══════════════════════════════════════════════════════════════════════════

package validation

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"fmt"
	"regexp"
	"strings"
)

// --- Types ---

// Result represents the outcome of validation
type Result struct {
	Valid    bool
	Health   int      // 0-100, where 100 is fully valid
	Errors   []string // Validation errors
	Warnings []string // Non-critical issues
}

// Check represents a single validation check
type Check struct {
	Name   string
	Weight int // Health impact (positive for success, negative for failure)
	Fn     func(value any) error
}

// Validator validates values with health scoring
type Validator struct {
	checks      []Check
	totalWeight int
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// --- Validator Construction ---

// New creates a new validator
func New() *Validator {
	return &Validator{
		checks: make([]Check, 0),
	}
}

// Add adds a check to the validator
func (v *Validator) Add(name string, weight int, fn func(value any) error) *Validator {
	v.checks = append(v.checks, Check{
		Name:   name,
		Weight: weight,
		Fn:     fn,
	})
	v.totalWeight += abs(weight)
	return v
}

// --- Validation Execution ---

// Validate runs all checks and returns result
func (v *Validator) Validate(value any) Result {
	result := Result{
		Valid:    true,
		Errors:   make([]string, 0),
		Warnings: make([]string, 0),
	}

	if len(v.checks) == 0 {
		result.Health = 100
		return result
	}

	earnedWeight := 0

	for _, check := range v.checks {
		err := check.Fn(value)
		if err != nil {
			result.Errors = append(result.Errors, fmt.Sprintf("%s: %v", check.Name, err))
			result.Valid = false
		} else {
			earnedWeight += abs(check.Weight)
		}
	}

	// Calculate health as percentage of earned weight
	if v.totalWeight > 0 {
		result.Health = (earnedWeight * 100) / v.totalWeight
	} else {
		result.Health = 100
	}

	return result
}

// --- Built-in Checks ---

// Required checks that value is not nil/empty
func Required() func(any) error {
	return func(value any) error {
		if value == nil {
			return fmt.Errorf("value is required")
		}

		// Check for empty strings
		if s, ok := value.(string); ok && strings.TrimSpace(s) == "" {
			return fmt.Errorf("value cannot be empty")
		}

		// Check for empty slices
		if s, ok := value.([]any); ok && len(s) == 0 {
			return fmt.Errorf("value cannot be empty")
		}

		// Check for empty maps
		if m, ok := value.(map[string]any); ok && len(m) == 0 {
			return fmt.Errorf("value cannot be empty")
		}

		return nil
	}
}

// StringMinLen checks string minimum length
func StringMinLen(min int) func(any) error {
	return func(value any) error {
		s, ok := value.(string)
		if !ok {
			return fmt.Errorf("expected string, got %T", value)
		}
		if len(s) < min {
			return fmt.Errorf("minimum length is %d, got %d", min, len(s))
		}
		return nil
	}
}

// StringMaxLen checks string maximum length
func StringMaxLen(max int) func(any) error {
	return func(value any) error {
		s, ok := value.(string)
		if !ok {
			return fmt.Errorf("expected string, got %T", value)
		}
		if len(s) > max {
			return fmt.Errorf("maximum length is %d, got %d", max, len(s))
		}
		return nil
	}
}

// StringPattern checks string matches regex pattern
func StringPattern(pattern string) func(any) error {
	re := regexp.MustCompile(pattern)
	return func(value any) error {
		s, ok := value.(string)
		if !ok {
			return fmt.Errorf("expected string, got %T", value)
		}
		if !re.MatchString(s) {
			return fmt.Errorf("does not match pattern %s", pattern)
		}
		return nil
	}
}

// IntRange checks integer is within range
func IntRange(min, max int) func(any) error {
	return func(value any) error {
		var n int
		switch v := value.(type) {
		case int:
			n = v
		case int64:
			n = int(v)
		case float64:
			n = int(v)
		default:
			return fmt.Errorf("expected integer, got %T", value)
		}

		if n < min || n > max {
			return fmt.Errorf("must be between %d and %d, got %d", min, max, n)
		}
		return nil
	}
}

// OneOf checks value is one of allowed values
func OneOf(allowed ...string) func(any) error {
	return func(value any) error {
		s, ok := value.(string)
		if !ok {
			return fmt.Errorf("expected string, got %T", value)
		}

		for _, a := range allowed {
			if s == a {
				return nil
			}
		}

		return fmt.Errorf("must be one of [%s], got %s", strings.Join(allowed, ", "), s)
	}
}

// --- Map Validation ---

// MapValidator validates map[string]any structures
type MapValidator struct {
	fieldChecks map[string]*Validator
	required    []string
}

// NewMapValidator creates a map validator
func NewMapValidator() *MapValidator {
	return &MapValidator{
		fieldChecks: make(map[string]*Validator),
		required:    make([]string, 0),
	}
}

// Field adds field validation
func (mv *MapValidator) Field(name string, v *Validator) *MapValidator {
	mv.fieldChecks[name] = v
	return mv
}

// RequiredField marks a field as required
func (mv *MapValidator) RequiredField(name string, v *Validator) *MapValidator {
	mv.fieldChecks[name] = v
	mv.required = append(mv.required, name)
	return mv
}

// Validate validates a map
func (mv *MapValidator) Validate(m map[string]any) Result {
	result := Result{
		Valid:    true,
		Health:   100,
		Errors:   make([]string, 0),
		Warnings: make([]string, 0),
	}

	totalWeight := 0
	earnedWeight := 0

	// Check required fields
	for _, name := range mv.required {
		totalWeight += 10
		if _, ok := m[name]; !ok {
			result.Errors = append(result.Errors, fmt.Sprintf("missing required field: %s", name))
			result.Valid = false
		} else {
			earnedWeight += 10
		}
	}

	// Validate each field
	for name, validator := range mv.fieldChecks {
		if value, ok := m[name]; ok {
			fieldResult := validator.Validate(value)
			if !fieldResult.Valid {
				result.Valid = false
				for _, err := range fieldResult.Errors {
					result.Errors = append(result.Errors, fmt.Sprintf("%s: %s", name, err))
				}
			}
			// Weight proportional to field validators
			weight := len(validator.checks) * 10
			totalWeight += weight
			earnedWeight += (fieldResult.Health * weight) / 100
		}
	}

	// Calculate health
	if totalWeight > 0 {
		result.Health = (earnedWeight * 100) / totalWeight
	}

	return result
}

// --- Convenience Functions ---

// Quick validates a single value with a validator
func Quick(value any, checks ...func(any) error) Result {
	v := New()
	for i, check := range checks {
		v.Add(fmt.Sprintf("check%d", i+1), 10, check)
	}
	return v.Validate(value)
}

// IsValid returns true if value passes all checks
func IsValid(value any, checks ...func(any) error) bool {
	return Quick(value, checks...).Valid
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════

// --- Helpers ---

func abs(n int) int {
	if n < 0 {
		return -n
	}
	return n
}
