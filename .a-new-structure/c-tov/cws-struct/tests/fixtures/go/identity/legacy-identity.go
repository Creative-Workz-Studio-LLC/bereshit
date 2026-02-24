//omni:key B-test-legacy-identity
//omni:code --go -library
//omni:version a-01.00

// Package fixture provides test fixtures.
package fixture

// ============================================================================
// METADATA
// ============================================================================
//
// Key:     B-test-legacy-identity
// Purpose: Go fixture with old _pragma/_metadata maps.
//          Triggers CWS-R25-012 (identity/upgrade).
//

var _pragma = map[string]string{
	"key": "B-test-legacy-identity",
}

var _metadata = map[string]string{
	"version": "a-01.00",
}

// ============================================================================
// SETUP
// ============================================================================

import "fmt"

// ============================================================================
// BODY
// ============================================================================

// 4. Core Logic
func Process() string {
	return fmt.Sprintf("hello")
}

// ============================================================================
// CLOSING
// ============================================================================
//
// X1 Policy
// Scripture: Psalm 119:130
//
// X5 Note
// This file tests legacy identity format detection.
//
// ============================================================================
