// #!omni code --go -library
// #!omni meta.key = claude-global-pkg-config-config
// #!omni meta.from = bereshit/word/seed/code/go/library.go
// #!omni meta.at = a-01.00
// ============================================================================
// METADATA
// ============================================================================
//
// Claude Global JSON Schema Validator
// See: system/config/schemas/*.schema.json for JSON schemas

// Package config provides JSON schema validation for CPI-SI configurations.
//
// # M.1 Core Identity [IDENTITY]
//
//	Key:       claude-global-pkg-config-config
//
//	Scripture: "Through wisdom is a house built...by knowledge
//	           shall the chambers be filled" — Proverbs 24:3-4
//	Principle: Validation ensures structural integrity before content is trusted
//	Anchor:    "Precept upon precept, line upon line" — Isaiah 28:10
//
//	Type:      Validator (complements loader.go TOML paths)
//
//	Architect:       Seanje Lenox-Wise
//	Implementation:  Nova Dawn
//	Created:         2025-12-10
//	Version:         a-01.00
//
// # M.2 Version History [HISTORY]
//
//   - a-01.00 (2025-12-15) — Structural alignment with loader.go pattern
//   - a-00.50 (2025-12-10) — Initial creation
//
// # M.3 Interface [INTERFACE]
//
//	Requires: stdlib (1): encoding/json, regexp, strings, os, path/filepath
//	Used by:  Identity config validation, any JSONC needing schema validation
//	Import:   import "creativeworkzstudio.com/bereshit/word/work/pkg/orchestration/config"
//	Pattern:  NewLoader(schemaDir) → LoadConfig(path, schema) → ValidationResult
//
// # M.4 Public API [API]
//
// M.4.1 Loader Construction [CONSTRUCT]
//
//	NewLoader(schemaDir) *Loader                    Create with schema dir
//	DefaultLoader() *Loader                         Use default schema path
//
// M.4.2 Schema Operations [ACCESS]
//
//	LoadSchema(path) (*Schema, error)               Load JSON schema
//	GetSchema(name) (*Schema, error)                Get cached or load
//
// M.4.3 JSONC Operations [LOAD]
//
//	LoadJSONC(path) (map[string]interface{}, error) Load JSON with comments
//	ParseJSONC(content) (map[string]interface{}, error)
//
// M.4.4 Validation [CHECK]
//
//	Validate(data, schema) ValidationResult         Validate against schema
//	LoadConfig(path, schema) (data, result, error)  Load and validate
//
// # M.5 Operational [OPERATIONAL]
//
//	Blocking: no (-1) — file I/O only, no network or long-running operations
//	Health:   Validator | granted (1): valid | deferred (0): warnings | denied (-1): invalid
package config

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================
//
// Section order: Imports → Types → Type Methods → Constants → Variables → Package-Level State
// See: bereshit/word/seed/code/go/library.go > SETUP

// ────────────────────────────────────────────────────────────────
// Imports
// ────────────────────────────────────────────────────────────────

//--- I.1 Standard Library [IMPORT] (1) ---
// JSON parsing, error handling, filesystem, string ops, regex, sorting
import (
	"encoding/json" // schema and config parsing
	"fmt"           // error formatting
	"os"            // file reading
	"path/filepath" // path construction
	"regexp"        // JSONC comment stripping, pattern validation
	"sort"          // deterministic category ordering
	"strings"       // JSONC processing
)

//--- I.2 External Packages [IMPORT] (-1) ---
// [Reserved: No external dependencies — uses standard library JSON]

//--- I.3 Internal Packages [IMPORT] (0) ---
// Same package as loader.go — uses Root(), Extensions() for config-driven paths

// ────────────────────────────────────────────────────────────────
// Types
// ────────────────────────────────────────────────────────────────
//
// Subsections: Building Blocks, Composed Types, Loader Type, Error Types
// Complements loader.go Config types — this handles JSON schema validation

//--- T.1 Building Blocks [TYPE] (1) ---
// Fundamental units: schema representation
// Loads from system/config/schemas/*.schema.json

// Schema represents a loaded JSON schema.
// Access validation rules via Properties["field"].
//
// Example:
//
//	schema, _ := loader.LoadSchema("instance/instance.schema.json")
//	schema.Required        // ["identity", "workspace", "calling"]
//	schema.Properties      // map of field -> Property
type Schema struct {
	ID          string                 `json:"$id"`
	Schema      string                 `json:"$schema"`
	Title       string                 `json:"title"`
	Description string                 `json:"description"`
	Type        string                 `json:"type"`
	Required    []string               `json:"required"`
	Properties  map[string]Property    `json:"properties"`
	Raw         map[string]interface{} `json:"-"` // Full schema for advanced validation
}

// Property represents a schema property definition.
// Nested structure mirrors JSON Schema spec for recursive validation.
//
// Example:
//
//	prop := schema.Properties["identity"]
//	prop.Type          // "object"
//	prop.Required      // ["name", "creator"]
//	prop.Properties    // nested properties for object types
type Property struct {
	Type        string              `json:"type"`
	Description string              `json:"description"`
	Required    []string            `json:"required,omitempty"`
	Properties  map[string]Property `json:"properties,omitempty"`
	Items       *Property           `json:"items,omitempty"`
	MinLength   int                 `json:"minLength,omitempty"`
	MaxLength   int                 `json:"maxLength,omitempty"`
	Minimum     *float64            `json:"minimum,omitempty"`
	Maximum     *float64            `json:"maximum,omitempty"`
	Pattern     string              `json:"pattern,omitempty"`
	Enum        []string            `json:"enum,omitempty"`
	Format      string              `json:"format,omitempty"`
	Default     interface{}         `json:"default,omitempty"`
}

//--- T.2 Composed Types [TYPE] (1) ---
// Aggregates returned by validation operations

// ValidationResult holds the result of validating a config against a schema.
// Returned by Validate(), ValidateFile(), and batch validation functions.
//
// Example:
//
//	result := loader.ValidateFile("config/instance.jsonc", "instance.schema.json")
//	if !result.Valid {
//	    for _, err := range result.Errors { log.Println(err) }
//	}
//	fmt.Printf("Health: %d%%\n", result.Health)
type ValidationResult struct {
	Valid    bool              // true only if ALL validations pass
	Health   int               // 0-100, see Constants > Validation Thresholds
	Errors   []ValidationError // all errors encountered
	Warnings []string          // non-fatal issues
}

//--- T.3 Loader Type [TYPE] (1) ---
// Main entry point for schema validation

// Loader handles configuration loading with schema validation.
// Creates cached access to JSON schemas for validating config files.
//
// Example:
//
//	loader := config.DefaultLoader()
//	result := loader.ValidateFile("config/instance.jsonc", "identity.schema.json")
type Loader struct {
	schemaDir string
	schemas   map[string]*Schema // Cached schemas by path
}

//--- T.4 Error Types [TYPE] (1) ---
// Config-driven errors: originate in DATA, manifest in CODE
// Each traces back to source. See: BODY > Schema Loading, BODY > JSONC Ops, BODY > Validation
// Used by health scoring to categorize and weight failures.

// SchemaError wraps schema file loading failures.
// Format: "schema instance.schema.json: parse: unexpected EOF"
type SchemaError struct {
	File string // schema path, e.g., "instance.schema.json"
	Op   string // operation: "read", "parse"
	Err  error  // underlying error
}

func (e *SchemaError) Error() string {
	return fmt.Sprintf("schema %s: %s: %v", e.File, e.Op, e.Err)
}

func (e *SchemaError) Unwrap() error { return e.Err }

// JSONCError wraps JSONC file loading failures.
// Format: "jsonc config/instance.jsonc: read: file not found"
type JSONCError struct {
	File string // JSONC file path, e.g., "config/instance.jsonc"
	Op   string // operation: "read", "parse"
	Err  error  // underlying error
}

func (e *JSONCError) Error() string {
	return fmt.Sprintf("jsonc %s: %s: %v", e.File, e.Op, e.Err)
}

func (e *JSONCError) Unwrap() error { return e.Err }

// ValidationError represents a single validation error.
// Format: "identity.name: required field missing"
type ValidationError struct {
	Path    string      // JSON path to the error (e.g., "identity.name")
	Message string      // what's wrong
	Value   interface{} // actual value (if applicable)
}

func (e *ValidationError) Error() string {
	if e.Path != "" {
		return fmt.Sprintf("%s: %s", e.Path, e.Message)
	}
	return e.Message
}

// ────────────────────────────────────────────────────────────────
// Type Methods
// ────────────────────────────────────────────────────────────────

//--- TM Type Methods [METHOD] (0) ---
// [Reserved: Phase 3 — typed accessors, conversion methods]
// SchemaError implements error interface (Error(), Unwrap())
// JSONCError implements error interface (Error(), Unwrap())
// ValidationError implements error interface (Error())

// ────────────────────────────────────────────────────────────────
// Constants
// ────────────────────────────────────────────────────────────────
//
// Schema organization and validation thresholds.
// Primary source is Helpers > Fallback Data > fallbackSchemaManifest; constants here for direct access.
//
// 8-Category Schema Architecture (mirrors data layer):
//   1. instance   — personhood: instance identity (config/instance/)
//   2. user       — personhood: user preferences (config/user/)
//   3. session    — operational: session state (data/session/)
//   4. temporal   — operational: time awareness (data/temporal/)
//   5. projects   — operational: project tracking (data/projects/)
//   6. project    — per-project: resolved at runtime
//   7. dev        — development: debug configs (dev/)
//   8. privacy    — filters: privacy rules (config/privacy/)

//--- K.1 Schema Organization [DATA] (1) ---
// Schema directory structure under system/config/schemas/
const (
	SchemaDir      = "system/config/schemas" // relative to claude-global root
	SchemaExt      = ".schema.json"          // schema file extension
	SchemaMetaFile = "README.md"             // schema documentation
)

//--- K.2 Schema Categories [DATA] (1) ---
// See: 8-Category Schema Architecture above for full manifest.
// Category paths relative to SchemaDir, data paths relative to claude-global root.
// Tripwire: Helpers > Fallback Data > fallbackSchemaManifest

//--- K.3 JSON Type Constants [DATA] (1) ---
// JSON Schema type strings. Used by Helpers > checkType and applyDefaults.
// See: https://json-schema.org/understanding-json-schema/reference/type.html
const (
	TypeString  = "string"
	TypeInteger = "integer"
	TypeNumber  = "number"
	TypeBoolean = "boolean"
	TypeObject  = "object"
	TypeArray   = "array"
	TypeNull    = "null"
)

//--- K.4 Validation Thresholds [DATA] (1) ---
// Health score boundaries for validation results
const (
	HealthPerfect  = 100 // All validations pass
	HealthGood     = 80  // Minor warnings only
	HealthDegraded = 50  // Some errors, still functional
	HealthCritical = 20  // Major errors, may not function
	HealthFailed   = 0   // Cannot validate or load
)

// ────────────────────────────────────────────────────────────────
// Variables
// ────────────────────────────────────────────────────────────────
//
// Package-level mutable state. Kept minimal — uses loader.go's configuration state.
//
// Subsections: Configuration State, Sentinel Errors, Cached State

//--- V.1 Configuration State [DATA] (0) ---
// [Reserved: config.go reads Root()/Extensions() from loader.go]

//--- V.2 Sentinel Errors [DATA] (1) ---
// Unexported — wrapped by public error types for context.
// See: BODY > Public APIs > GetSchema (errSchemaNotFound)
// See: BODY > Public APIs > ParseJSONC (errInvalidJSON)
var (
	errSchemaNotFound = fmt.Errorf("schema not found")
	errInvalidJSON    = fmt.Errorf("invalid JSON")
)

//--- V.3 Cached State [DATA] (1) ---
// Default loader instance, initialized on first use.
var defaultLoader *Loader

// ────────────────────────────────────────────────────────────────
// Package-Level State
// ────────────────────────────────────────────────────────────────
//
// Cross-package coordination via loader.go SetRoot().
// This package reads paths from loader.go, doesn't set them.
//
// Subsections: Coordination Pattern, Initialization Order, Reserved Features

//--- PS.1 Coordination Pattern [DOC] (1) ---
// config.go reads Root()/Extensions() from loader.go.
// Lazy initialization: defaultLoader created on first DefaultLoader() call.

//--- PS.2 Initialization Order [DOC] (1) ---
// Sequence of operations for proper initialization.
//   1. loader.SetRoot() called by main/hook
//   2. DefaultLoader() creates validator using Extensions().System path
//   3. Validate*() functions use cached defaultLoader

//--- PS.3 Reserved Features [DOC] (0) ---
// [Reserved: config.go is consumer, not coordinator — no init() needed]

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================
//
// Section order: Org Chart → Helpers → Core Operations → Error Handling → Public APIs
// See: bereshit/word/seed/code/go/library.go > BODY

// ────────────────────────────────────────────────────────────────
// Organizational Chart
// ────────────────────────────────────────────────────────────────
//
// [Reserved: Create after functions are reorganized into proper sections]
// Will contain: Ladder Structure, Baton Flow, APU Inventory

// ────────────────────────────────────────────────────────────────
// Helpers/Utilities - Internal Support
// ────────────────────────────────────────────────────────────────
//
// Foundation layer. Higher rungs depend on these, never the reverse.
// See: Org Chart > Ladder Structure > Helpers
//
// Subsections: Pure Functions, Guard Functions, Domain Functions, Fallback Data
//
// Note: Guard functions not needed — config.go uses loader.go's SetRoot()/Root()
// for path state. This package is a CONSUMER of loader.go's config state.

//--- H.1 Pure Functions [TRANSFORM] (1) ---
// No side effects. Same input → same output. Safe to call from anywhere.
// See: loader.go > H.1 > fileExists (shared package-level helper)

// checkType checks if value matches expected JSON type.
//
// Parameters:
//   - value: the value to check (from parsed JSON)
//   - expectedType: JSON Schema type string (use SETUP > Constants > Type*)
//
// Returns:
//   - bool: true if value matches expected type, false otherwise
//
// Uses: SETUP > Constants > JSON Type Constants (TypeString, TypeObject, etc.)
// Used by: Core Operations > Validation (validateProperty)
func (l *Loader) checkType(value interface{}, expectedType string) bool {
	switch expectedType {
	case TypeString:
		_, ok := value.(string)
		return ok
	case TypeInteger:
		_, ok := value.(float64)
		return ok
	case TypeNumber:
		_, ok := value.(float64)
		return ok
	case TypeBoolean:
		_, ok := value.(bool)
		return ok
	case TypeObject:
		_, ok := value.(map[string]interface{})
		return ok
	case TypeArray:
		_, ok := value.([]interface{})
		return ok
	case TypeNull:
		return value == nil
	default:
		return true // Unknown type, allow
	}
}

// toFloat64 converts numeric types to float64 for comparison.
//
// Parameters:
//   - v: value to convert (expected numeric)
//
// Returns:
//   - float64: converted value
//   - bool: true if conversion succeeded, false otherwise
//
// Used by: Core Operations > Validation (validateProperty - range checks)
func toFloat64(v interface{}) (float64, bool) {
	switch n := v.(type) {
	case float64:
		return n, true
	case int:
		return float64(n), true
	case int64:
		return float64(n), true
	default:
		return 0, false
	}
}

//--- H.2 Guard Functions [CHECK] (0) ---
// Reserved: config.go reads state from loader.go.
// See: loader.go > Helpers > Guard Functions > checkRoot

//--- H.3 Domain Functions [DOMAIN] (1) ---
// Work with fallbackSchemaManifest to iterate over 8-category schema architecture.
// See: Helpers > Fallback Data > fallbackSchemaManifest

// extractCategories returns sorted category names from fallbackSchemaManifest.
//
// Returns:
//   - []string: sorted category names (e.g., ["dev", "instance", "privacy", ...])
//
// Uses: Helpers > Fallback Data > fallbackSchemaManifest
// Reserved for: Future Core Ops that iterate all schema categories
func extractCategories() []string {
	categories := make([]string, 0, len(fallbackSchemaManifest))
	for category := range fallbackSchemaManifest {
		categories = append(categories, category)
	}
	sort.Strings(categories) // deterministic order
	return categories
}

// getCategoryDataDir returns the data directory for a schema category.
//
// Parameters:
//   - category: category name (e.g., "instance", "session")
//
// Returns:
//   - string: data directory path relative to claude-global root
//   - bool: true if category exists, false otherwise
//
// Uses: Helpers > Fallback Data > fallbackSchemaManifest
// Reserved for: Future Core Ops that need category data paths
func getCategoryDataDir(category string) (string, bool) {
	dir, ok := fallbackSchemaManifest[category]
	return dir, ok
}

// buildSchemaPath constructs the schema file path for a category.
//
// Parameters:
//   - category: category name (e.g., "instance")
//
// Returns:
//   - string: schema file path (e.g., "instance/instance.schema.json")
//
// Uses: SETUP > Constants > SchemaExt
// Used by: Public APIs > Schema Access > GetSchema
func buildSchemaPath(category string) string {
	return filepath.Join(category, category+SchemaExt)
}

//--- H.4 Fallback Data [DATA] (1) ---
// Static data for tripwire fallback when config-driven source unavailable.
// Tripwire: K.2 references this section. If K.2 Tripwire breaks, check here.
// See: loader.go > Helpers > Fallback Data for pattern.

// fallbackSchemaManifest maps schema categories to their data directories.
// Used by Domain Functions to iterate over 8-category schema architecture.
// Tripwire: Future versions may load from config.toml; this is the fallback.
var fallbackSchemaManifest = map[string]string{
	"instance": "config/instance", // personhood: instance identity
	"user":     "config/user",     // personhood: user preferences
	"session":  "data/session",    // operational: session state
	"temporal": "data/temporal",   // operational: time awareness
	"projects": "data/projects",   // operational: project tracking
	"project":  "",                // per-project: resolved at runtime
	"dev":      "dev",             // development: debug configs
	"privacy":  "config/privacy",  // filters: privacy rules
}

// ────────────────────────────────────────────────────────────────
// Core Operations - Business Logic
// ────────────────────────────────────────────────────────────────
//
// Middle rung. Depends on Helpers below, used by Public APIs above.
// See: Org Chart > Ladder Structure > Core Operations
//
// Subsections: Data Transformation, Schema Loading, Validation Logic

//--- C.1 Data Transformation [TRANSFORM] (1) ---
// Functions that modify data in place or call other layers.
// Not pure functions — have side effects or cross-layer dependencies.
// Contrast: Helpers > Pure Functions (stateless transforms)

// processValue recursively substitutes variables in template values.
//
// Parameters:
//   - value: value to process (string, map, array, or other)
//   - vars: variable map for ${VAR} substitution
//
// Returns:
//   - interface{}: processed value with variables substituted
//
// Uses: Public APIs > Template Processing > ProcessTemplate (for map types)
//
// Note: Cross-layer call to Public API — returns here for array recursion.
func processValue(value interface{}, vars map[string]string) interface{} {
	switch v := value.(type) {
	case string:
		// Substitute ${VAR} patterns
		result := v
		for name, val := range vars {
			result = strings.ReplaceAll(result, "${"+name+"}", val)
		}
		return result
	case map[string]interface{}:
		return ProcessTemplate(v, vars) // Public APIs > Template Processing
	case []interface{}:
		arr := make([]interface{}, len(v))
		for i, item := range v {
			arr[i] = processValue(item, vars)
		}
		return arr
	default:
		return value
	}
}

// applyDefaults fills in missing fields with schema defaults.
//
// Parameters:
//   - data: config data map to fill defaults into (MODIFIED IN PLACE)
//   - props: schema properties with Default values
//
// Uses: SETUP > Types > Property, SETUP > Constants > TypeObject
//
// Note: Modifies data in place — side effect.
func applyDefaults(data map[string]interface{}, props map[string]Property) {
	for name, prop := range props {
		if _, exists := data[name]; !exists && prop.Default != nil {
			data[name] = prop.Default
		}
		// Recurse into nested objects
		if prop.Type == TypeObject && prop.Properties != nil {
			if nested, ok := data[name].(map[string]interface{}); ok {
				applyDefaults(nested, prop.Properties)
			}
		}
	}
}

//--- C.2 Schema Loading [LOAD] (1) ---
// Load JSON schemas from filesystem. Cached access.
// Uses: Helpers > Pure Functions > fileExists, SETUP > Types > Schema

// LoadSchema loads a JSON schema from file (internal, cached).
//
// Parameters:
//   - path: schema path (relative to schemaDir or absolute)
//
// Returns:
//   - *Schema: parsed schema, cached for subsequent calls
//   - error: SchemaError for structured logging/health scoring
//
// Uses: Helpers > Pure Functions > fileExists
func (l *Loader) LoadSchema(path string) (*Schema, error) {
	// Check cache
	if schema, ok := l.schemas[path]; ok {
		return schema, nil
	}

	// Resolve path
	fullPath := path
	if !filepath.IsAbs(path) {
		fullPath = filepath.Join(l.schemaDir, path)
	}

	// Check file exists — explicit error before read
	if !fileExists(fullPath) { // Helpers > Pure Functions
		return nil, newSchemaError(path, "not found", fmt.Errorf("schema file not found: %s", fullPath))
	}

	// Read file
	data, err := os.ReadFile(fullPath)
	if err != nil {
		return nil, newSchemaError(path, "read", err) // Error Handling > Error Constructors
	}

	// Parse JSON
	var schema Schema
	if err := json.Unmarshal(data, &schema); err != nil {
		return nil, newSchemaError(path, "parse", err) // Error Handling > Error Constructors
	}

	// Also store raw for advanced validation
	var raw map[string]interface{}
	json.Unmarshal(data, &raw)
	schema.Raw = raw

	// Cache and return
	l.schemas[path] = &schema
	return &schema, nil
}

//--- C.3 Validation Logic [CHECK] (1) ---
// Property validation against JSON Schema. Core validation engine.
// Uses: Helpers > Pure Functions > checkType, toFloat64

// validateProperty validates a single property against schema.
//
// Parameters:
//   - result: ValidationResult to append errors to (MODIFIED IN PLACE)
//   - path: property path for error messages (e.g., "server.port")
//   - value: actual value from config
//   - prop: Property schema to validate against
//   - weight: health penalty weight for errors
//
// Uses: Helpers > Pure Functions > checkType, toFloat64
//
// Note: Modifies result in place — appends errors, decrements health.
func (l *Loader) validateProperty(result *ValidationResult, path string, value interface{}, prop Property, weight int) {
	// Type validation
	if !l.checkType(value, prop.Type) { // Helpers > Pure Functions
		result.Errors = append(result.Errors, ValidationError{
			Path:    path,
			Message: fmt.Sprintf("expected type %s", prop.Type),
			Value:   value,
		})
		result.Health -= weight / 2
		return
	}

	// String validations
	if str, ok := value.(string); ok {
		if prop.MinLength > 0 && len(str) < prop.MinLength {
			result.Errors = append(result.Errors, ValidationError{
				Path:    path,
				Message: fmt.Sprintf("minimum length is %d", prop.MinLength),
				Value:   str,
			})
			result.Health -= weight / 4
		}
		if prop.MaxLength > 0 && len(str) > prop.MaxLength {
			result.Errors = append(result.Errors, ValidationError{
				Path:    path,
				Message: fmt.Sprintf("maximum length is %d", prop.MaxLength),
				Value:   str,
			})
			result.Health -= weight / 4
		}
		if prop.Pattern != "" {
			if matched, _ := regexp.MatchString(prop.Pattern, str); !matched {
				result.Errors = append(result.Errors, ValidationError{
					Path:    path,
					Message: fmt.Sprintf("does not match pattern %s", prop.Pattern),
					Value:   str,
				})
				result.Health -= weight / 4
			}
		}
		if len(prop.Enum) > 0 {
			found := false
			for _, allowed := range prop.Enum {
				if str == allowed {
					found = true
					break
				}
			}
			if !found {
				result.Errors = append(result.Errors, ValidationError{
					Path:    path,
					Message: fmt.Sprintf("must be one of %v", prop.Enum),
					Value:   str,
				})
				result.Health -= weight / 4
			}
		}
	}

	// Number validations
	if num, ok := toFloat64(value); ok { // Helpers > Pure Functions
		if prop.Minimum != nil && num < *prop.Minimum {
			result.Errors = append(result.Errors, ValidationError{
				Path:    path,
				Message: fmt.Sprintf("minimum is %v", *prop.Minimum),
				Value:   num,
			})
			result.Health -= weight / 4
		}
		if prop.Maximum != nil && num > *prop.Maximum {
			result.Errors = append(result.Errors, ValidationError{
				Path:    path,
				Message: fmt.Sprintf("maximum is %v", *prop.Maximum),
				Value:   num,
			})
			result.Health -= weight / 4
		}
	}

	// Nested object validation
	if prop.Type == TypeObject && prop.Properties != nil {
		if obj, ok := value.(map[string]interface{}); ok {
			// Validate required
			for _, req := range prop.Required {
				if _, ok := obj[req]; !ok {
					result.Errors = append(result.Errors, ValidationError{
						Path:    path + "." + req,
						Message: "required field missing",
					})
					result.Health -= weight / 4
				}
			}
			// Validate nested properties
			for name, nestedProp := range prop.Properties {
				if nestedValue, ok := obj[name]; ok {
					l.validateProperty(result, path+"."+name, nestedValue, nestedProp, weight/2)
				}
			}
		}
	}

	// Array validation
	if prop.Type == TypeArray && prop.Items != nil {
		if arr, ok := value.([]interface{}); ok {
			for i, item := range arr {
				l.validateProperty(result, fmt.Sprintf("%s[%d]", path, i), item, *prop.Items, weight/len(arr))
			}
		}
	}

	// Update valid flag
	if len(result.Errors) > 0 {
		result.Valid = false
	}
}

// ────────────────────────────────────────────────────────────────
// Error Handling/Recovery Patterns
// ────────────────────────────────────────────────────────────────
//
// Config-driven errors: originate in DATA, manifest in CODE, trace back to source.
// See: SETUP > Types > Error Types for type definitions
//
// Subsections: Design Principles, Error Constructors, Recovery Patterns

//--- E.1 Design Principles [DOC] (1) ---
// Config-driven systems separate DATA from CODE.
//
// Error Categories:
//   - SchemaError:     schema file-level (not found, parse failed)
//   - JSONCError:      JSONC file-level (not found, parse failed)
//   - ValidationError: validation-level (wrong type, missing field)
//
// Recovery Strategy:
//   - Health-aware: every error impacts ValidationResult.Health score
//   - Path-traced: errors include JSON path to problematic field
//   - Accumulating: collects ALL errors, not just first
//   - Structured: error types enable health scoring and structured logging

//--- E.2 Error Constructors [CONSTRUCT] ---
// Factory functions for error types. Used by Core Operations and Public APIs.

// newSchemaError creates a SchemaError with file and operation context.
//
// Parameters:
//   - file: schema file path (e.g., "instance.schema.json")
//   - op: operation that failed ("read", "parse")
//   - err: underlying error
//
// Returns:
//   - *SchemaError: wrapped error with context
func newSchemaError(file, op string, err error) *SchemaError {
	return &SchemaError{File: file, Op: op, Err: err}
}

// newJSONCError creates a JSONCError with file and operation context.
//
// Parameters:
//   - file: JSONC file path (e.g., "config/instance.jsonc")
//   - op: operation that failed ("read", "parse")
//   - err: underlying error
//
// Returns:
//   - *JSONCError: wrapped error with context
func newJSONCError(file, op string, err error) *JSONCError {
	return &JSONCError{File: file, Op: op, Err: err}
}

//--- E.3 Recovery Patterns [DOC] (1) ---
// Tripwire fallback: when schema/config files missing, degrade with LOUD warning.
// See: Public APIs > LoadJSONCSafe() for safe loading with fallbacks.

// ────────────────────────────────────────────────────────────────
// Public APIs - Exported Interface
// ────────────────────────────────────────────────────────────────
//
// Top rung. Orchestrates Core Operations for external callers.
// See: Org Chart > Ladder Structure > Public APIs
//
// Subsections: Loader Construction, Schema Access, JSONC Operations,
//              Validation, Template Processing, Config Loading, Convenience

//--- P.1 Loader Construction [CONSTRUCT] ---
// Create Loader instances for schema validation.

// NewLoader creates a config loader with schema directory
func NewLoader(schemaDir string) *Loader {
	return &Loader{
		schemaDir: schemaDir,
		schemas:   make(map[string]*Schema),
	}
}

// DefaultLoader creates loader with schema path from config.toml.
// Uses loader.go Extensions() for config-driven path.
// Tripwire: Falls back to hardcoded ~/.claude path if root not set.
func DefaultLoader() *Loader {
	// Try config-driven path first (single source of truth)
	ext, err := Extensions()
	if err == nil {
		root := Root()
		return NewLoader(filepath.Join(root, ext.System, "config", "schemas"))
	}

	// Tripwire fallback: root not set, use hardcoded path
	// Uses SchemaDir from SETUP > Constants
	home, _ := os.UserHomeDir()
	return NewLoader(filepath.Join(home, ".claude", SchemaDir))
}

//--- P.2 Schema Access [ACCESS] ---

// GetSchema returns a cached schema or loads it
func (l *Loader) GetSchema(name string) (*Schema, error) {
	// Try common paths
	// Uses: Helpers > Domain Functions > buildSchemaPath
	paths := []string{
		name,
		name + SchemaExt,        // direct file: name.schema.json
		buildSchemaPath(name),   // nested: name/name.schema.json
	}

	for _, path := range paths {
		schema, err := l.LoadSchema(path)
		if err == nil {
			return schema, nil
		}
	}

	return nil, fmt.Errorf("%w: %s", errSchemaNotFound, name)
}

//--- P.3 JSONC Operations [LOAD] ---

// LoadJSONC loads a JSONC file (JSON with comments).
// Returns JSONCError for structured logging/health scoring.
// Uses: Helpers > Pure Functions > fileExists
func LoadJSONC(path string) (map[string]interface{}, error) {
	// Check file exists — explicit error before read
	if !fileExists(path) { // Helpers > Pure Functions
		return nil, newJSONCError(path, "not found", fmt.Errorf("file not found: %s", path))
	}

	data, err := os.ReadFile(path)
	if err != nil {
		return nil, newJSONCError(path, "read", err) // Error Handling > Error Constructors
	}

	result, err := ParseJSONC(string(data))
	if err != nil {
		return nil, newJSONCError(path, "parse", err) // Error Handling > Error Constructors
	}
	return result, nil
}

// ParseJSONC parses JSONC string (JSON with comments).
// Returns errInvalidJSON sentinel for errors.Is() checking.
// For file context, use LoadJSONC which wraps in JSONCError.
func ParseJSONC(content string) (map[string]interface{}, error) {
	// Remove single-line comments
	singleLine := regexp.MustCompile(`//.*$`)
	lines := strings.Split(content, "\n")
	for i, line := range lines {
		// Don't remove // inside strings
		if !strings.Contains(line, `"`) || strings.Index(line, "//") < strings.LastIndex(line, `"`) {
			lines[i] = singleLine.ReplaceAllString(line, "")
		}
	}
	content = strings.Join(lines, "\n")

	// Remove multi-line comments
	multiLine := regexp.MustCompile(`/\*[\s\S]*?\*/`)
	content = multiLine.ReplaceAllString(content, "")

	// Remove trailing commas before } or ]
	trailingComma := regexp.MustCompile(`,(\s*[}\]])`)
	content = trailingComma.ReplaceAllString(content, "$1")

	// Parse JSON
	var result map[string]interface{}
	if err := json.Unmarshal([]byte(content), &result); err != nil {
		return nil, fmt.Errorf("%w: %v", errInvalidJSON, err)
	}

	return result, nil
}

// LoadJSONCSafe loads JSONC with detailed error reporting.
// Wraps LoadJSONC errors with additional path context.
func LoadJSONCSafe(path string) (map[string]interface{}, error) {
	result, err := LoadJSONC(path)
	if err != nil {
		// LoadJSONC already returns JSONCError with path, just pass through
		return nil, err
	}
	return result, nil
}

//--- P.4 Validation [CHECK] ---

// Validate validates data against a schema
func (l *Loader) Validate(data map[string]interface{}, schemaPath string) ValidationResult {
	result := ValidationResult{
		Valid:    true,
		Health:   100,
		Errors:   make([]ValidationError, 0),
		Warnings: make([]string, 0),
	}

	schema, err := l.GetSchema(schemaPath)
	if err != nil {
		result.Valid = false
		result.Health = 0
		result.Errors = append(result.Errors, ValidationError{
			Path:    "",
			Message: fmt.Sprintf("schema load failed: %v", err),
		})
		return result
	}

	// Calculate weight per required field
	totalFields := len(schema.Required)
	if totalFields == 0 {
		totalFields = 1
	}
	weightPerField := 100 / totalFields

	// Validate required fields
	for _, field := range schema.Required {
		if _, ok := data[field]; !ok {
			result.Valid = false
			result.Health -= weightPerField
			result.Errors = append(result.Errors, ValidationError{
				Path:    field,
				Message: "required field missing",
			})
		}
	}

	// Validate properties
	for name, prop := range schema.Properties {
		if value, ok := data[name]; ok {
			l.validateProperty(&result, name, value, prop, weightPerField)
		}
	}

	// Clamp health
	if result.Health < 0 {
		result.Health = 0
	}

	return result
}

//--- P.5 Template Processing [TRANSFORM] ---

// ProcessTemplate substitutes variables in a template
func ProcessTemplate(template map[string]interface{}, vars map[string]string) map[string]interface{} {
	result := make(map[string]interface{})

	for key, value := range template {
		result[key] = processValue(value, vars)
	}

	return result
}

//--- P.6 Config Loading [LOAD] ---

// LoadConfig loads a JSONC config and validates against schema
func (l *Loader) LoadConfig(configPath, schemaPath string) (map[string]interface{}, ValidationResult, error) {
	// Load config
	data, err := LoadJSONC(configPath)
	if err != nil {
		return nil, ValidationResult{Valid: false, Health: 0}, err
	}

	// Validate
	result := l.Validate(data, schemaPath)

	return data, result, nil
}

// LoadConfigWithDefaults loads config and applies defaults from schema
func (l *Loader) LoadConfigWithDefaults(configPath, schemaPath string) (map[string]interface{}, ValidationResult, error) {
	data, result, err := l.LoadConfig(configPath, schemaPath)
	if err != nil {
		return nil, result, err
	}

	schema, _ := l.GetSchema(schemaPath)
	if schema != nil {
		applyDefaults(data, schema.Properties)
	}

	return data, result, nil
}

//--- P.7 Convenience [ACCESS] ---

// QuickLoad loads JSONC without validation
func QuickLoad(path string) (map[string]interface{}, error) {
	return LoadJSONC(path)
}

// QuickValidate validates data against schema path
func QuickValidate(data map[string]interface{}, schemaPath string) ValidationResult {
	return DefaultLoader().Validate(data, schemaPath)
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// [Reserved: Closing documentation - see loader.go for pattern]
