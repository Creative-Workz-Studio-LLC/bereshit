//omni:key B-sdk-substrate-go
//omni:code --go -library
//omni:version a-01.00

package substrate

/*
#cgo CFLAGS: -fPIC
#cgo LDFLAGS: -L${SRCDIR}/../../../../../sdk/c-hybrid/cpisi-substrate-sdk/lib -lcpisi_substrate_sdk -Wl,-rpath,${SRCDIR}/../../../../../sdk/c-hybrid/cpisi-substrate-sdk/lib -Wl,-rpath,$$HOME/.local/share/claude/lib
#include <stdlib.h>

int cpisi_sdk_load_map(const char* path);
char* cpisi_sdk_process_event(const char* substrate, const char* event, const char* raw_json);
char* cpisi_sdk_render_output(const char* substrate, const char* event, const char* variant, const char* context_json);
char* cpisi_sdk_get_last_testimony();
int cpisi_sdk_generate_sabbath_artifacts(const char* testimony_json);
void cpisi_sdk_free_string(char* s);
*/
import "C"
import (
	"encoding/json"
	"fmt"
	"os"
	"unsafe"
)

// ============================================================================
// METADATA
// ============================================================================
//
// Key:     B-sdk-substrate-go
// Purpose: Go wrapper for the Rust Substrate SDK mapping engine
//

// Pragma exposes the file's OmniCode identity for runtime inspection.
var Pragma = [][2]string{
	{"I1.key", "B-sdk-substrate-go"},
	{"I1.format", "go"},
	{"I1.from", "sdk.go"},
	{"I1.at", "a-01.00"},
	{"I2.type", "code"},
	{"I2.structure", "4-block"},
	{"I2.subtype", "library"},
	{"I2.role", "bridge"},
	{"I3.file", "sdk.go"},
	{"I3.title", "Substrate SDK Go Bridge"},
	{"I3.component", "FFI Bridge"},
	{"I3.path", "pkg/sdk/substrate/sdk.go"},
	{"I3.provides", "SDK_BRIDGE"},
	{"I3.brief", "Go wrapper for the Rust Substrate SDK mapping engine."},
	{"I4.layer", "L0-foundation"},
	{"I4.position", "bridge"},
	{"I4.pattern", "cgo"},
}

// Metadata exposes the file's version and status for runtime inspection.
var Metadata = [][2]string{
	{"C1.version", "a-01.00"},
	{"C1.status", "Active"},
	{"C1.created", "2026-02-28"},
	{"C1.updated", "2026-02-28"},
	{"C2.architect", "Seanje Lenox-Wise"},
	{"C2.implementation", "Nova Dawn"},
	{"C2.organization", "CreativeWorkzStudio LLC"},
	{"C2.copyright", "CreativeWorkzStudio LLC"},
	{"C3.scripture", "Genesis 1:1"},
	{"C3.principle", "Inter-language harmony - mind and body unified"},
	{"C3.anchor", "Genesis 1:1"},
	{"C4.requires.stdlib", "encoding/json, fmt, os, unsafe"},
	{"C4.requires.external", "libcpisi_substrate_sdk.so"},
	{"C4.requires.internal", "none"},
	{"C4.consumers", "CPI-SI CLI hooks"},
	{"C4.integration", "CGO dynamic linking"},
	{"C4.if_missing", "Hooks cannot utilize the advanced Rust engine logic"},
	{"C5.purpose", "To provide a seamless Go interface to the high-performance Rust SDK."},
	{"C5.philosophy", "The muscle (Go) should never have to think; it simply relays to the Mind (Rust)."},
	{"C6.current", "a-01.00 — Added Sabbath artifact generation support"},
	{"C6.planned", "Support for direct StateMachine pointer sharing"},
	{"C6.limitations", "Requires manual LD_LIBRARY_PATH or correctly embedded RPATH"},
	{"C7.tags", "cgo, rust, sdk, bridge, substrate"},
	{"C7.category", "Infrastructure"},
	{"C7.domain", "bridge"},
	{"C7.paradigm", "CPI-SI"},
}

// GetPragma returns the module's pragma identity.
func GetPragma() [][2]string { return Pragma }

// GetMetadata returns the module's metadata.
func GetMetadata() [][2]string { return Metadata }

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// ──────────────────────────────────────────────────────────────────────────
// 7. Core Types
// ──────────────────────────────────────────────────────────────────────────

// SessionTestimony matches the Rust SDK's artifact schema
type SessionTestimony struct {
	SessionID       string         `json:"session_id"`
	Substrate       string         `json:"substrate"`
	Engine          string         `json:"engine"`
	StartedAt       string         `json:"started_at"`
	EndedAt         string         `json:"ended_at"`
	DurationSeconds uint64         `json:"duration_seconds"`
	Metrics         SessionMetrics `json:"metrics"`
	State           FinalState     `json:"state"`
	Summary         string         `json:"summary"`
}

type SessionMetrics struct {
	Exchanges         uint32 `json:"exchanges"`
	Insights          uint32 `json:"insights"`
	ToolsUsed         uint32 `json:"tools_used"`
	PeakContextTokens uint32 `json:"peak_context_tokens"`
	Compactions       uint32 `json:"compactions"`
}

type FinalState struct {
	Trajectory   string  `json:"trajectory"`
	Health       float64 `json:"health"`
	HebrewState  string  `json:"hebrew_state"`
	Alignment    float64 `json:"alignment"`
	Anchor       string  `json:"anchor"`
}

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// ──────────────────────────────────────────────────────────────────────────
// 22. Public APIs
// ──────────────────────────────────────────────────────────────────────────

// LoadMap loads a substrate map from a TOML file into the Rust engine.
func LoadMap(path string) error {
	if _, err := os.Stat(path); err != nil {
		return fmt.Errorf("map file not found: %s", path)
	}

	cPath := C.CString(path)
	defer C.free(unsafe.Pointer(cPath))

	res := int(C.cpisi_sdk_load_map(cPath))
	if res < 0 {
		return fmt.Errorf("rust engine failed to load substrate map (code: %d, path: %s)", res, path)
	}
	return nil
}

// ProcessEvent maps a raw JSON payload from a specific substrate/event to a Universal Event.
func ProcessEvent(substrate, event, rawJSON string) (string, error) {
	cSub := C.CString(substrate)
	defer C.free(unsafe.Pointer(cSub))
	cEvent := C.CString(event)
	defer C.free(unsafe.Pointer(cEvent))
	cJSON := C.CString(rawJSON)
	defer C.free(unsafe.Pointer(cJSON))

	cResult := C.cpisi_sdk_process_event(cSub, cEvent, cJSON)
	if cResult == nil {
		return "", fmt.Errorf("failed to process event: %s/%s", substrate, event)
	}
	defer C.cpisi_sdk_free_string(cResult)

	return C.GoString(cResult), nil
}

// RenderOutput renders a substrate-specific response using the Rust engine.
func RenderOutput(substrate, event, variant string, context map[string]string) (string, error) {
	cSub := C.CString(substrate)
	defer C.free(unsafe.Pointer(cSub))
	cEvent := C.CString(event)
	defer C.free(unsafe.Pointer(cEvent))
	cVariant := C.CString(variant)
	defer C.free(unsafe.Pointer(cVariant))

	ctxJSON, err := json.Marshal(context)
	if err != nil {
		return "", fmt.Errorf("failed to marshal context: %w", err)
	}
	cCtx := C.CString(string(ctxJSON))
	defer C.free(unsafe.Pointer(cCtx))

	cResult := C.cpisi_sdk_render_output(cSub, cEvent, cVariant, cCtx)
	if cResult == nil {
		return "", fmt.Errorf("failed to render output: %s/%s/%s", substrate, event, variant)
	}
	defer C.cpisi_sdk_free_string(cResult)

	return C.GoString(cResult), nil
}

// GenerateSabbathArtifacts triggers the Rust SDK to write .json and .adoc artifacts
func GenerateSabbathArtifacts(testimony SessionTestimony) error {
	data, err := json.Marshal(testimony)
	if err != nil {
		return fmt.Errorf("failed to marshal testimony: %w", err)
	}

	cJSON := C.CString(string(data))
	defer C.free(unsafe.Pointer(cJSON))

	res := int(C.cpisi_sdk_generate_sabbath_artifacts(cJSON))
	if res < 0 {
		return fmt.Errorf("rust engine failed to generate artifacts (code: %d)", res)
	}
	return nil
}

// GetLastTestimony retrieves the most recent session testimony from the SDK.
func GetLastTestimony() (*SessionTestimony, error) {
	cResult := C.cpisi_sdk_get_last_testimony()
	if cResult == nil {
		return nil, nil // No previous testimony found
	}
	defer C.cpisi_sdk_free_string(cResult)

	var testimony SessionTestimony
	if err := json.Unmarshal([]byte(C.GoString(cResult)), &testimony); err != nil {
		return nil, fmt.Errorf("failed to unmarshal testimony: %w", err)
	}

	return &testimony, nil
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================

// ──────────────────────────────────────────────────────────────────────────
// Cv — Validation
// ──────────────────────────────────────────────────────────────────────────

// (Cv content goes here)

// ──────────────────────────────────────────────────────────────────────────
// Ce — Execution
// ──────────────────────────────────────────────────────────────────────────

// (Ce content goes here)

// ──────────────────────────────────────────────────────────────────────────
// Cc — Cleanup
// ──────────────────────────────────────────────────────────────────────────

// (Cc content goes here)

// ──────────────────────────────────────────────────────────────────────────
// X1: Policy
// ──────────────────────────────────────────────────────────────────────────
// Never: Break 4-block structure, Remove block boundaries, Remove identity vars
// Careful: Public API signatures (breaks callers), Error types (breaks type switches)
// Safe: Function bodies, New functions, Comments, Tests

// ──────────────────────────────────────────────────────────────────────────
// X4: Reference
// ──────────────────────────────────────────────────────────────────────────

// ──────────────────────────────────────────────────────────────────────────
// X5: Note
// ──────────────────────────────────────────────────────────────────────────
// Note: Go wrapper for the Rust Substrate SDK mapping engine
// Scripture: "In the beginning God created the heaven and the earth." — Genesis 1:1

// ============================================================================
// END CLOSING
// ============================================================================
