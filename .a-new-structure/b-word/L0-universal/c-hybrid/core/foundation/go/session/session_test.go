//omni:code --go -demo-test
//omni:key B-L0-foundation-session-test
//omni:version a-02.00

// ============================================================================
// METADATA
// ============================================================================
//
// Key:     B-L0-foundation-session-test
// Purpose: Comprehensive tests for session management types
// Biblical: Ecclesiastes 3:1 — "To every thing there is a season"
// Authors: Nova Dawn
// Version: 2.0.0
// Created: 2026-02-18
// Updated: 2026-02-18

package session

// ──────────────────────────────────────────────────────────────────────────
// Imports (in METADATA — Go requires all imports before any declarations)
// ──────────────────────────────────────────────────────────────────────────

import (
	"encoding/json"
	"testing"
	"time"
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
//   Constructors:     TestNewCurrent, TestNewLogEntry, TestNewLog, TestPathsFromDir
//   Current Queries:  TestCurrent_HasCompacted, TestCurrent_RecordCompaction
//   Log Queries:      TestLog_EntryCount, TestLog_IsEmpty, TestLog_CountByType,
//                     TestLog_AddEntry
//   FormatDuration:   TestFormatDuration_Zero, TestFormatDuration_SubMinute,
//                     TestFormatDuration_ExactMinute, TestFormatDuration_Minutes,
//                     TestFormatDuration_HoursAndMinutes, TestFormatDuration_MultipleHours,
//                     TestFormatDuration_ExactHour, TestFormatDuration_59Seconds
//   String:           TestCurrent_String, TestLogEntry_String, TestLog_String
//   JSON Roundtrip:   TestCurrent_JSONRoundtrip, TestLog_JSONRoundtrip,
//                     TestCurrent_JSON_OmitsZeroCompactions

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
//   - Constructors: NewCurrent, NewLogEntry, NewLog, PathsFromDir
//   - Current queries: HasCompacted, RecordCompaction
//   - Log queries: EntryCount, IsEmpty, CountByType, AddEntry
//   - FormatDuration: zero, seconds, minutes, hours, edge cases
//   - String representations: Current, LogEntry, Log
//   - JSON roundtrip: Current, Log, omitempty behavior

// --- Constructor Tests ---

func TestNewCurrent(t *testing.T) {
	now := time.Now()
	c := NewCurrent("test-session", now)

	if c.SessionID != "test-session" {
		t.Errorf("SessionID = %q, want %q", c.SessionID, "test-session")
	}
	if c.StartTime != now {
		t.Errorf("StartTime mismatch")
	}
	if c.Compactions != 0 {
		t.Errorf("Compactions = %d, want 0", c.Compactions)
	}
}

func TestNewLogEntry(t *testing.T) {
	ts := time.Date(2026, 2, 18, 15, 0, 0, 0, time.UTC)
	e := NewLogEntry(ts, "task", "implemented feature")

	if e.Type != "task" {
		t.Errorf("Type = %q, want %q", e.Type, "task")
	}
	if e.Content != "implemented feature" {
		t.Errorf("Content = %q", e.Content)
	}
	if e.Timestamp != ts {
		t.Error("Timestamp mismatch")
	}
}

func TestNewLog(t *testing.T) {
	l := NewLog("test-session")

	if l.SessionID != "test-session" {
		t.Errorf("SessionID = %q, want %q", l.SessionID, "test-session")
	}
	if l.Entries == nil {
		t.Error("Entries should be initialized, not nil")
	}
	if len(l.Entries) != 0 {
		t.Errorf("Entries length = %d, want 0", len(l.Entries))
	}
}

func TestPathsFromDir(t *testing.T) {
	p := PathsFromDir("/home/user/.claude/session")

	if p.SessionDir != "/home/user/.claude/session" {
		t.Errorf("SessionDir = %q", p.SessionDir)
	}
	if p.CurrentSession != "/home/user/.claude/session/current.json" {
		t.Errorf("CurrentSession = %q", p.CurrentSession)
	}
	if p.SessionLog != "/home/user/.claude/session/current-log.json" {
		t.Errorf("SessionLog = %q", p.SessionLog)
	}
	if p.PatternFile != "/home/user/.claude/session/patterns.json" {
		t.Errorf("PatternFile = %q", p.PatternFile)
	}
}

// --- Current Query Tests ---

func TestCurrent_HasCompacted(t *testing.T) {
	c := NewCurrent("test", time.Now())

	if c.HasCompacted() {
		t.Error("should not have compacted initially")
	}

	c.RecordCompaction()
	if !c.HasCompacted() {
		t.Error("should have compacted after RecordCompaction")
	}
}

func TestCurrent_RecordCompaction(t *testing.T) {
	c := NewCurrent("test", time.Now())

	c.RecordCompaction()
	if c.Compactions != 1 {
		t.Errorf("Compactions = %d, want 1", c.Compactions)
	}

	c.RecordCompaction()
	c.RecordCompaction()
	if c.Compactions != 3 {
		t.Errorf("Compactions = %d, want 3", c.Compactions)
	}
}

// --- Log Query Tests ---

func TestLog_EntryCount(t *testing.T) {
	l := NewLog("test")
	if l.EntryCount() != 0 {
		t.Errorf("EntryCount = %d, want 0", l.EntryCount())
	}

	l.AddEntry(NewLogEntry(time.Now(), "task", "did thing"))
	if l.EntryCount() != 1 {
		t.Errorf("EntryCount = %d, want 1", l.EntryCount())
	}
}

func TestLog_IsEmpty(t *testing.T) {
	l := NewLog("test")
	if !l.IsEmpty() {
		t.Error("should be empty initially")
	}

	l.AddEntry(NewLogEntry(time.Now(), "note", "hi"))
	if l.IsEmpty() {
		t.Error("should not be empty after AddEntry")
	}
}

func TestLog_CountByType(t *testing.T) {
	l := NewLog("test")
	now := time.Now()
	l.AddEntry(NewLogEntry(now, "task", "a"))
	l.AddEntry(NewLogEntry(now, "note", "b"))
	l.AddEntry(NewLogEntry(now, "task", "c"))
	l.AddEntry(NewLogEntry(now, "milestone", "d"))
	l.AddEntry(NewLogEntry(now, "task", "e"))

	if got := l.CountByType("task"); got != 3 {
		t.Errorf("CountByType(task) = %d, want 3", got)
	}
	if got := l.CountByType("note"); got != 1 {
		t.Errorf("CountByType(note) = %d, want 1", got)
	}
	if got := l.CountByType("tool"); got != 0 {
		t.Errorf("CountByType(tool) = %d, want 0", got)
	}
}

func TestLog_AddEntry(t *testing.T) {
	l := NewLog("test")
	e := NewLogEntry(time.Now(), "task", "content")
	l.AddEntry(e)

	if len(l.Entries) != 1 {
		t.Fatalf("Entries length = %d, want 1", len(l.Entries))
	}
	if l.Entries[0].Content != "content" {
		t.Errorf("Entry content = %q", l.Entries[0].Content)
	}
}

// --- FormatDuration Tests ---

func TestFormatDuration_Zero(t *testing.T) {
	if got := FormatDuration(0); got != "0s" {
		t.Errorf("FormatDuration(0) = %q, want %q", got, "0s")
	}
}

func TestFormatDuration_SubMinute(t *testing.T) {
	if got := FormatDuration(30 * time.Second); got != "30s" {
		t.Errorf("FormatDuration(30s) = %q, want %q", got, "30s")
	}
}

func TestFormatDuration_ExactMinute(t *testing.T) {
	if got := FormatDuration(60 * time.Second); got != "1m" {
		t.Errorf("FormatDuration(60s) = %q, want %q", got, "1m")
	}
}

func TestFormatDuration_Minutes(t *testing.T) {
	if got := FormatDuration(5 * time.Minute); got != "5m" {
		t.Errorf("FormatDuration(5m) = %q, want %q", got, "5m")
	}
}

func TestFormatDuration_HoursAndMinutes(t *testing.T) {
	d := 1*time.Hour + 5*time.Minute
	if got := FormatDuration(d); got != "1h 5m" {
		t.Errorf("FormatDuration(1h5m) = %q, want %q", got, "1h 5m")
	}
}

func TestFormatDuration_MultipleHours(t *testing.T) {
	d := 3*time.Hour + 30*time.Minute
	if got := FormatDuration(d); got != "3h 30m" {
		t.Errorf("FormatDuration(3h30m) = %q, want %q", got, "3h 30m")
	}
}

func TestFormatDuration_ExactHour(t *testing.T) {
	if got := FormatDuration(2 * time.Hour); got != "2h 0m" {
		t.Errorf("FormatDuration(2h) = %q, want %q", got, "2h 0m")
	}
}

func TestFormatDuration_59Seconds(t *testing.T) {
	if got := FormatDuration(59 * time.Second); got != "59s" {
		t.Errorf("FormatDuration(59s) = %q, want %q", got, "59s")
	}
}

// --- String Tests ---

func TestCurrent_String(t *testing.T) {
	ts := time.Date(2026, 2, 18, 15, 30, 0, 0, time.UTC)
	c := Current{SessionID: "abc-123", StartTime: ts, Compactions: 2}

	expected := "[abc-123] started 2026-02-18T15:30:00Z (compactions: 2)"
	if got := c.String(); got != expected {
		t.Errorf("String() = %q, want %q", got, expected)
	}
}

func TestLogEntry_String(t *testing.T) {
	ts := time.Date(2026, 2, 18, 15, 30, 0, 0, time.UTC)
	e := LogEntry{Timestamp: ts, Type: "task", Content: "did thing"}

	expected := "[2026-02-18T15:30:00Z] task: did thing"
	if got := e.String(); got != expected {
		t.Errorf("String() = %q, want %q", got, expected)
	}
}

func TestLog_String(t *testing.T) {
	l := NewLog("abc-123")
	l.AddEntry(NewLogEntry(time.Now(), "task", "a"))
	l.AddEntry(NewLogEntry(time.Now(), "note", "b"))

	expected := "Session abc-123 (2 entries)"
	if got := l.String(); got != expected {
		t.Errorf("String() = %q, want %q", got, expected)
	}
}

// --- JSON Roundtrip Tests ---

func TestCurrent_JSONRoundtrip(t *testing.T) {
	ts := time.Date(2026, 2, 18, 15, 30, 0, 0, time.UTC)
	c := Current{SessionID: "test-123", StartTime: ts, Compactions: 1}

	data, err := json.Marshal(c)
	if err != nil {
		t.Fatal(err)
	}

	var decoded Current
	if err := json.Unmarshal(data, &decoded); err != nil {
		t.Fatal(err)
	}

	if decoded.SessionID != c.SessionID {
		t.Errorf("SessionID = %q, want %q", decoded.SessionID, c.SessionID)
	}
	if !decoded.StartTime.Equal(c.StartTime) {
		t.Errorf("StartTime mismatch")
	}
	if decoded.Compactions != c.Compactions {
		t.Errorf("Compactions = %d, want %d", decoded.Compactions, c.Compactions)
	}
}

func TestLog_JSONRoundtrip(t *testing.T) {
	ts := time.Date(2026, 2, 18, 15, 30, 0, 0, time.UTC)
	l := NewLog("test-123")
	l.AddEntry(NewLogEntry(ts, "task", "did thing"))
	l.AddEntry(NewLogEntry(ts, "note", "noticed pattern"))

	data, err := json.Marshal(l)
	if err != nil {
		t.Fatal(err)
	}

	var decoded Log
	if err := json.Unmarshal(data, &decoded); err != nil {
		t.Fatal(err)
	}

	if decoded.SessionID != l.SessionID {
		t.Errorf("SessionID = %q, want %q", decoded.SessionID, l.SessionID)
	}
	if len(decoded.Entries) != 2 {
		t.Fatalf("Entries length = %d, want 2", len(decoded.Entries))
	}
	if decoded.Entries[0].Type != "task" {
		t.Errorf("Entry[0].Type = %q, want %q", decoded.Entries[0].Type, "task")
	}
	if decoded.Entries[1].Content != "noticed pattern" {
		t.Errorf("Entry[1].Content = %q", decoded.Entries[1].Content)
	}
}

func TestCurrent_JSON_OmitsZeroCompactions(t *testing.T) {
	c := NewCurrent("test", time.Now())
	data, err := json.Marshal(c)
	if err != nil {
		t.Fatal(err)
	}

	var raw map[string]json.RawMessage
	if err := json.Unmarshal(data, &raw); err != nil {
		t.Fatal(err)
	}

	if _, ok := raw["compactions"]; ok {
		t.Error("JSON should omit compactions when zero (omitempty)")
	}
}

//
// ──────────────────────────────────────────────────────────────────────────
// Ce — Execution
// ──────────────────────────────────────────────────────────────────────────
//
// go test -v ./L0-universal/c-hybrid/core/foundation/go/session/
// go test -run TestFormatDuration ./L0-universal/c-hybrid/core/foundation/go/session/
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
//   - Remove FormatDuration edge case tests (boundary conditions)
//
// Careful:
//   - Changing expected String() output (format is a contract)
//   - Modifying time-based test fixtures (UTC required for determinism)
//
// Safe:
//   - Adding new test cases for edge conditions
//   - Adding tests for new session types
//
// ──────────────────────────────────────────────────────────────────────────
// X5: Note
// ──────────────────────────────────────────────────────────────────────────
//
// Note: Test suite for session package — verifies constructors, queries,
// mutations, string representations, JSON serialization, and FormatDuration
// across zero, seconds, minutes, hours, and edge cases.
//
// Scripture: Ecclesiastes 3:1 — "To every thing there is a season, and a
// time to every purpose under the heaven."
//
// ============================================================================
// END CLOSING
// ============================================================================
