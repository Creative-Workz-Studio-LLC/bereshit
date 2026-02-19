//omni:code --go -demo-test
//omni:key B-L0-foundation-types-sessioncontext-test
//omni:version a-02.00

// ============================================================================
// METADATA
// ============================================================================
//
// Key:     B-L0-foundation-types-sessioncontext-test
// Purpose: Comprehensive tests for session context types
// Biblical: 1 Corinthians 14:33 — "God is not the author of confusion"
// Authors: Nova Dawn
// Version: 2.0.0
// Created: 2026-02-18
// Updated: 2026-02-18

package types

// ──────────────────────────────────────────────────────────────────────────
// Imports (in METADATA — Go requires all imports before any declarations)
// ──────────────────────────────────────────────────────────────────────────

import (
	"encoding/json"
	"math"
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
// Test Functions (22 tests in Cv zone):
//   Named Types:      TestModel_String
//   JSON Contract:    TestSessionContext_JSONRoundtrip, TestSessionContext_JSONFieldNames
//   GetWorkDir:       TestGetWorkDir_PrefersCWD, TestGetWorkDir_FallsBackToCurrentDir,
//                     TestGetWorkDir_FallsBackToProjectDir, TestGetWorkDir_AllEmpty
//   Cost Queries:     TestTotalLinesChanged, TestDurationSeconds, TestAPIDurationSeconds,
//                     TestAPIEfficiency, TestAPIEfficiency_ZeroDuration
//   Context Window:   TestCurrentContextTokens, TestContextPercentage_UsesCurrentUsage,
//                     TestContextPercentage_FallbackToCumulative,
//                     TestContextPercentage_ZeroWindowSize
//   CacheEfficiency:  TestCacheEfficiency_BothZero, TestCacheEfficiency_AllCache,
//                     TestCacheEfficiency_NoCache, TestCacheEfficiency_Equal,
//                     TestCacheEfficiency_Balanced
//   String:           TestSessionContext_String

// ──────────────────────────────────────────────────────────────────────────
// 2. Helpers
// ──────────────────────────────────────────────────────────────────────────
// [Reserved: No test helpers needed — all tests construct SessionContext directly]

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
// 22 tests covering:
//   - Named types: Model.String()
//   - JSON contract: roundtrip, field name verification
//   - GetWorkDir: priority cascade (CWD > CurrentDir > ProjectDir > empty)
//   - Cost queries: TotalLinesChanged, DurationSeconds, APIDurationSeconds,
//     APIEfficiency (including zero-duration edge case)
//   - Context window: CurrentContextTokens, ContextPercentage (current usage,
//     cumulative fallback, zero window size)
//   - CacheEfficiency: base-3 log-odds scaling (both zero, all cache, no cache,
//     equal, balanced 3x ratio)
//   - String representation

// --- Named Type Tests ---

func TestModel_String(t *testing.T) {
	tests := []struct {
		name  string
		model Model
		want  string
	}{
		{"full", Model{ID: "claude-opus-4-6", DisplayName: "Claude Opus 4.6"}, "Claude Opus 4.6 (claude-opus-4-6)"},
		{"id only", Model{ID: "claude-opus-4-6"}, "claude-opus-4-6"},
		{"empty", Model{}, "(unknown model)"},
	}
	for _, tc := range tests {
		t.Run(tc.name, func(t *testing.T) {
			if got := tc.model.String(); got != tc.want {
				t.Errorf("String() = %q, want %q", got, tc.want)
			}
		})
	}
}

// --- JSON Contract Tests ---

func TestSessionContext_JSONRoundtrip(t *testing.T) {
	ctx := SessionContext{
		HookEventName: "Notification",
		SessionID:     "test-session",
		CWD:           "/home/user/project",
		Version:       "1.0.0",
		Model: Model{
			ID:          "claude-opus-4-6",
			DisplayName: "Claude Opus 4.6",
		},
		Workspace: Workspace{
			CurrentDir: "/home/user/project",
			ProjectDir: "/home/user/project",
		},
		OutputStyle: OutputStyle{Name: "Nova Dawn"},
		Cost: Cost{
			TotalCostUSD:       0.42,
			TotalDurationMS:    60000,
			TotalAPIDurationMS: 30000,
			TotalLinesAdded:    100,
			TotalLinesRemoved:  50,
		},
		ContextWindow: ContextWindow{
			TotalInputTokens:  50000,
			TotalOutputTokens: 10000,
			ContextWindowSize: 200000,
			CurrentUsage: CurrentUsage{
				InputTokens:              40000,
				OutputTokens:             8000,
				CacheCreationInputTokens: 5000,
				CacheReadInputTokens:     30000,
			},
		},
	}

	data, err := json.Marshal(ctx)
	if err != nil {
		t.Fatal(err)
	}

	var decoded SessionContext
	if err := json.Unmarshal(data, &decoded); err != nil {
		t.Fatal(err)
	}

	if decoded.SessionID != ctx.SessionID {
		t.Errorf("SessionID = %q, want %q", decoded.SessionID, ctx.SessionID)
	}
	if decoded.Model.ID != ctx.Model.ID {
		t.Errorf("Model.ID = %q, want %q", decoded.Model.ID, ctx.Model.ID)
	}
	if decoded.Workspace.CurrentDir != ctx.Workspace.CurrentDir {
		t.Errorf("Workspace.CurrentDir = %q, want %q", decoded.Workspace.CurrentDir, ctx.Workspace.CurrentDir)
	}
	if decoded.Cost.TotalCostUSD != ctx.Cost.TotalCostUSD {
		t.Errorf("Cost.TotalCostUSD = %f, want %f", decoded.Cost.TotalCostUSD, ctx.Cost.TotalCostUSD)
	}
	if decoded.ContextWindow.CurrentUsage.CacheReadInputTokens != ctx.ContextWindow.CurrentUsage.CacheReadInputTokens {
		t.Errorf("CacheReadInputTokens = %d, want %d",
			decoded.ContextWindow.CurrentUsage.CacheReadInputTokens,
			ctx.ContextWindow.CurrentUsage.CacheReadInputTokens)
	}
}

// Verify JSON field names match original inline-struct layout
func TestSessionContext_JSONFieldNames(t *testing.T) {
	ctx := SessionContext{
		Model:     Model{ID: "test"},
		Workspace: Workspace{CurrentDir: "/test"},
	}

	data, err := json.Marshal(ctx)
	if err != nil {
		t.Fatal(err)
	}

	var raw map[string]json.RawMessage
	if err := json.Unmarshal(data, &raw); err != nil {
		t.Fatal(err)
	}

	// Top-level field names must match the API contract
	required := []string{"model", "workspace", "output_style", "cost", "context_window"}
	for _, key := range required {
		if _, ok := raw[key]; !ok {
			t.Errorf("missing top-level JSON field %q", key)
		}
	}
}

// --- GetWorkDir Tests ---

func TestGetWorkDir_PrefersCWD(t *testing.T) {
	ctx := SessionContext{
		CWD:       "/cwd",
		Workspace: Workspace{CurrentDir: "/current", ProjectDir: "/project"},
	}
	if got := ctx.GetWorkDir(); got != "/cwd" {
		t.Errorf("GetWorkDir() = %q, want /cwd", got)
	}
}

func TestGetWorkDir_FallsBackToCurrentDir(t *testing.T) {
	ctx := SessionContext{
		Workspace: Workspace{CurrentDir: "/current", ProjectDir: "/project"},
	}
	if got := ctx.GetWorkDir(); got != "/current" {
		t.Errorf("GetWorkDir() = %q, want /current", got)
	}
}

func TestGetWorkDir_FallsBackToProjectDir(t *testing.T) {
	ctx := SessionContext{
		Workspace: Workspace{ProjectDir: "/project"},
	}
	if got := ctx.GetWorkDir(); got != "/project" {
		t.Errorf("GetWorkDir() = %q, want /project", got)
	}
}

func TestGetWorkDir_AllEmpty(t *testing.T) {
	ctx := SessionContext{}
	if got := ctx.GetWorkDir(); got != "" {
		t.Errorf("GetWorkDir() = %q, want empty", got)
	}
}

// --- Cost Queries Tests ---

func TestTotalLinesChanged(t *testing.T) {
	ctx := SessionContext{Cost: Cost{TotalLinesAdded: 100, TotalLinesRemoved: 50}}
	if got := ctx.TotalLinesChanged(); got != 150 {
		t.Errorf("TotalLinesChanged() = %d, want 150", got)
	}
}

func TestDurationSeconds(t *testing.T) {
	ctx := SessionContext{Cost: Cost{TotalDurationMS: 65000}}
	if got := ctx.DurationSeconds(); got != 65 {
		t.Errorf("DurationSeconds() = %d, want 65", got)
	}
}

func TestAPIDurationSeconds(t *testing.T) {
	ctx := SessionContext{Cost: Cost{TotalAPIDurationMS: 30500}}
	if got := ctx.APIDurationSeconds(); got != 30 {
		t.Errorf("APIDurationSeconds() = %d, want 30", got)
	}
}

func TestAPIEfficiency(t *testing.T) {
	ctx := SessionContext{Cost: Cost{TotalDurationMS: 100000, TotalAPIDurationMS: 50000}}
	if got := ctx.APIEfficiency(); got != 50.0 {
		t.Errorf("APIEfficiency() = %f, want 50.0", got)
	}
}

func TestAPIEfficiency_ZeroDuration(t *testing.T) {
	ctx := SessionContext{}
	if got := ctx.APIEfficiency(); got != 0 {
		t.Errorf("APIEfficiency() = %f, want 0", got)
	}
}

// --- Context Window Tests ---

func TestCurrentContextTokens(t *testing.T) {
	ctx := SessionContext{
		ContextWindow: ContextWindow{
			CurrentUsage: CurrentUsage{InputTokens: 40000, OutputTokens: 8000},
		},
	}
	if got := ctx.CurrentContextTokens(); got != 48000 {
		t.Errorf("CurrentContextTokens() = %d, want 48000", got)
	}
}

func TestContextPercentage_UsesCurrentUsage(t *testing.T) {
	ctx := SessionContext{
		ContextWindow: ContextWindow{
			ContextWindowSize: 200000,
			TotalInputTokens:  100000,
			TotalOutputTokens: 50000,
			CurrentUsage:      CurrentUsage{InputTokens: 40000, OutputTokens: 10000},
		},
	}
	// Should use CurrentUsage (50000/200000 = 25%)
	got := ctx.ContextPercentage()
	if got != 25.0 {
		t.Errorf("ContextPercentage() = %f, want 25.0", got)
	}
}

func TestContextPercentage_FallbackToCumulative(t *testing.T) {
	ctx := SessionContext{
		ContextWindow: ContextWindow{
			ContextWindowSize: 200000,
			TotalInputTokens:  60000,
			TotalOutputTokens: 40000,
			// CurrentUsage is zero
		},
	}
	// Should fallback to cumulative (100000/200000 = 50%)
	got := ctx.ContextPercentage()
	if got != 50.0 {
		t.Errorf("ContextPercentage() = %f, want 50.0", got)
	}
}

func TestContextPercentage_ZeroWindowSize(t *testing.T) {
	ctx := SessionContext{}
	if got := ctx.ContextPercentage(); got != 0 {
		t.Errorf("ContextPercentage() = %f, want 0", got)
	}
}

// --- CacheEfficiency Tests ---

func TestCacheEfficiency_BothZero(t *testing.T) {
	ctx := SessionContext{}
	if got := ctx.CacheEfficiency(); got != 0 {
		t.Errorf("CacheEfficiency(0,0) = %f, want 0", got)
	}
}

func TestCacheEfficiency_AllCache(t *testing.T) {
	ctx := SessionContext{
		ContextWindow: ContextWindow{
			CurrentUsage: CurrentUsage{CacheReadInputTokens: 1000},
		},
	}
	if got := ctx.CacheEfficiency(); got != 100 {
		t.Errorf("CacheEfficiency(0,1000) = %f, want 100", got)
	}
}

func TestCacheEfficiency_NoCache(t *testing.T) {
	ctx := SessionContext{
		ContextWindow: ContextWindow{
			CurrentUsage: CurrentUsage{InputTokens: 1000},
		},
	}
	if got := ctx.CacheEfficiency(); got != -100 {
		t.Errorf("CacheEfficiency(1000,0) = %f, want -100", got)
	}
}

func TestCacheEfficiency_Equal(t *testing.T) {
	ctx := SessionContext{
		ContextWindow: ContextWindow{
			CurrentUsage: CurrentUsage{InputTokens: 1000, CacheReadInputTokens: 1000},
		},
	}
	got := ctx.CacheEfficiency()
	if math.Abs(got) > 0.001 {
		t.Errorf("CacheEfficiency(1000,1000) = %f, want ~0", got)
	}
}

func TestCacheEfficiency_Balanced(t *testing.T) {
	// 3x cache = log3(3) = 1, mapped to 100/3 ≈ 33.33
	ctx := SessionContext{
		ContextWindow: ContextWindow{
			CurrentUsage: CurrentUsage{InputTokens: 1000, CacheReadInputTokens: 3000},
		},
	}
	got := ctx.CacheEfficiency()
	expected := 100.0 / 3.0
	if math.Abs(got-expected) > 0.01 {
		t.Errorf("CacheEfficiency(1000,3000) = %f, want ~%f", got, expected)
	}
}

// --- String Tests ---

func TestSessionContext_String(t *testing.T) {
	ctx := SessionContext{
		SessionID:     "test-123",
		HookEventName: "Notification",
		Model:         Model{ID: "claude-opus-4-6", DisplayName: "Claude Opus 4.6"},
		Cost:          Cost{TotalCostUSD: 0.42},
		ContextWindow: ContextWindow{
			ContextWindowSize: 200000,
			CurrentUsage:      CurrentUsage{InputTokens: 40000, OutputTokens: 10000},
		},
	}
	got := ctx.String()
	expected := "[test-123] Claude Opus 4.6 (claude-opus-4-6) — Notification (25.0% context, $0.42)"
	if got != expected {
		t.Errorf("String() = %q, want %q", got, expected)
	}
}

//
// ──────────────────────────────────────────────────────────────────────────
// Ce — Execution
// ──────────────────────────────────────────────────────────────────────────
//
// go test -v -run TestSessionContext ./L0-universal/c-hybrid/core/foundation/go/types/
// go test -v -run TestCacheEfficiency ./L0-universal/c-hybrid/core/foundation/go/types/
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
//   - Skip JSON field name tests (API contract verification)
//   - Remove CacheEfficiency edge cases (mathematical correctness)
//   - Change GetWorkDir cascade tests (priority order is contract)
//
// Careful:
//   - Changing expected String() output (format is a contract)
//   - Modifying CacheEfficiency thresholds (mathematical precision)
//
// Safe:
//   - Adding new test cases for edge conditions
//   - Adding tests for new query methods
//
// ──────────────────────────────────────────────────────────────────────────
// X5: Note
// ──────────────────────────────────────────────────────────────────────────
//
// Note: Test suite for session context types in types package — verifies
// Model.String(), JSON roundtrip/field names, GetWorkDir cascade, cost
// queries, context window calculations, CacheEfficiency base-3 log-odds
// scaling, and SessionContext.String(). Shares types package with
// workflow_test.go.
//
// Scripture: 1 Corinthians 14:33 — "God is not the author of confusion,
// but of peace."
//
// ============================================================================
// END CLOSING
// ============================================================================
