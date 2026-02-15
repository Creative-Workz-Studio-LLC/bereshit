//omni:code --go -test
//omni:key B-L2-debugging-test
//omni:version a-01.00

// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: B-L2-debugging-test
// Purpose: Unit tests for structured CPI-SI debugging sessions
// Biblical: Job 28:27 — "Then did he see it, and declare it; he prepared it"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-15
//
// ═══════════════════════════════════════════════════════════════════════════

package debugging

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"bytes"
	"strings"
	"testing"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// ───────────────────────────────────────────────────────────────────────────
// Level.String — verify all verbosity level names
// ───────────────────────────────────────────────────────────────────────────

func TestLevelStringBasic(t *testing.T) {
	if got := LevelBasic.String(); got != "BASIC" {
		t.Errorf("LevelBasic.String() = %q, want %q", got, "BASIC")
	}
}

func TestLevelStringDetailed(t *testing.T) {
	if got := LevelDetailed.String(); got != "DETAILED" {
		t.Errorf("LevelDetailed.String() = %q, want %q", got, "DETAILED")
	}
}

func TestLevelStringVerbose(t *testing.T) {
	if got := LevelVerbose.String(); got != "VERBOSE" {
		t.Errorf("LevelVerbose.String() = %q, want %q", got, "VERBOSE")
	}
}

func TestLevelStringUnknown(t *testing.T) {
	if got := Level(99).String(); got != "UNKNOWN" {
		t.Errorf("Level(99).String() = %q, want %q", got, "UNKNOWN")
	}
}

// ───────────────────────────────────────────────────────────────────────────
// NewSession — verify initial state
// ───────────────────────────────────────────────────────────────────────────

func TestNewSessionComponent(t *testing.T) {
	s := NewSession("test-component", LevelBasic)
	if s.Component != "test-component" {
		t.Errorf("Component = %q, want %q", s.Component, "test-component")
	}
}

func TestNewSessionLevel(t *testing.T) {
	s := NewSession("test", LevelDetailed)
	if s.Level != LevelDetailed {
		t.Errorf("Level = %d, want %d", s.Level, LevelDetailed)
	}
}

func TestNewSessionContextID(t *testing.T) {
	s := NewSession("test", LevelBasic)
	if s.ContextID == "" {
		t.Error("ContextID should not be empty")
	}
	if !strings.HasPrefix(s.ContextID, "test-") {
		t.Errorf("ContextID %q should start with component name", s.ContextID)
	}
}

func TestNewSessionCapturesContext(t *testing.T) {
	s := NewSession("test", LevelBasic)
	if s.Hostname == "" {
		t.Error("Hostname should be captured")
	}
	if s.Shell == "" {
		t.Error("Shell should be captured")
	}
	if s.Arch == "" {
		t.Error("Arch should be captured")
	}
}

func TestNewSessionMapsInitialized(t *testing.T) {
	s := NewSession("test", LevelBasic)
	if s.Parameters == nil {
		t.Error("Parameters map should be initialized")
	}
	if s.InitialState == nil {
		t.Error("InitialState map should be initialized")
	}
	if s.FinalState == nil {
		t.Error("FinalState map should be initialized")
	}
}

// ───────────────────────────────────────────────────────────────────────────
// Configuration methods — SetParameter, SetInitialState, SetInitialHealth
// ───────────────────────────────────────────────────────────────────────────

func TestSetParameter(t *testing.T) {
	s := NewSession("test", LevelBasic)
	s.SetParameter("key", "value")
	if s.Parameters["key"] != "value" {
		t.Errorf("Parameter key = %q, want %q", s.Parameters["key"], "value")
	}
}

func TestSetInitialState(t *testing.T) {
	s := NewSession("test", LevelBasic)
	s.SetInitialState("state", "ready")
	if s.InitialState["state"] != "ready" {
		t.Errorf("InitialState state = %q, want %q", s.InitialState["state"], "ready")
	}
}

func TestSetInitialHealthClamped(t *testing.T) {
	s := NewSession("test", LevelBasic)

	s.SetInitialHealth(150)
	if s.InitialHealth != 100 {
		t.Errorf("health 150 should clamp to 100, got %d", s.InitialHealth)
	}

	s.SetInitialHealth(-50)
	if s.InitialHealth != 0 {
		t.Errorf("health -50 should clamp to 0, got %d", s.InitialHealth)
	}

	s.SetInitialHealth(75)
	if s.InitialHealth != 75 {
		t.Errorf("health 75 should stay 75, got %d", s.InitialHealth)
	}
}

// ───────────────────────────────────────────────────────────────────────────
// AddEvent — verify BASIC level suppresses events
// ───────────────────────────────────────────────────────────────────────────

func TestAddEventBasicLevelSuppressed(t *testing.T) {
	s := NewSession("test", LevelBasic)
	s.AddEvent(SeverityInfo, EventInit, "test event", nil, 50, 50)
	if len(s.Events) != 0 {
		t.Errorf("BASIC level should suppress events, got %d", len(s.Events))
	}
}

func TestAddEventDetailedLevelRecords(t *testing.T) {
	s := NewSession("test", LevelDetailed)
	s.AddEvent(SeverityInfo, EventInit, "test event", nil, 50, 60)
	if len(s.Events) != 1 {
		t.Fatalf("DETAILED should record events, got %d", len(s.Events))
	}
	e := s.Events[0]
	if e.Type != EventInit {
		t.Errorf("event type = %q, want %q", e.Type, EventInit)
	}
	if e.HealthBefore != 50 || e.HealthAfter != 60 {
		t.Errorf("health = %d→%d, want 50→60", e.HealthBefore, e.HealthAfter)
	}
}

// ───────────────────────────────────────────────────────────────────────────
// Convenience event methods — Info, Warn, Error
// ───────────────────────────────────────────────────────────────────────────

func TestInfoEvent(t *testing.T) {
	s := NewSession("test", LevelDetailed)
	s.Info(EventOperation, "op", nil, 50, 50)
	if len(s.Events) != 1 || s.Events[0].Severity != SeverityInfo {
		t.Error("Info should record an INFO severity event")
	}
}

func TestWarnEvent(t *testing.T) {
	s := NewSession("test", LevelDetailed)
	s.Warn(EventWarning, "warn", nil, 50, 40)
	if len(s.Events) != 1 || s.Events[0].Severity != SeverityWarn {
		t.Error("Warn should record a WARN severity event")
	}
}

func TestErrorEvent(t *testing.T) {
	s := NewSession("test", LevelDetailed)
	s.Error(EventError, "error", nil, 50, 20)
	if len(s.Events) != 1 || s.Events[0].Severity != SeverityError {
		t.Error("Error should record an ERROR severity event")
	}
}

// ───────────────────────────────────────────────────────────────────────────
// Analysis — VERBOSE-only methods
// ───────────────────────────────────────────────────────────────────────────

func TestAddIssueSuppressedAtDetailed(t *testing.T) {
	s := NewSession("test", LevelDetailed)
	s.AddIssue("test issue")
	if len(s.Issues) != 0 {
		t.Error("DETAILED should suppress issues")
	}
}

func TestAddIssueRecordedAtVerbose(t *testing.T) {
	s := NewSession("test", LevelVerbose)
	s.AddIssue("test issue")
	if len(s.Issues) != 1 || s.Issues[0] != "test issue" {
		t.Error("VERBOSE should record issues")
	}
}

func TestAddRecommendation(t *testing.T) {
	s := NewSession("test", LevelVerbose)
	s.AddRecommendation("do the thing")
	if len(s.Recommendations) != 1 {
		t.Error("VERBOSE should record recommendations")
	}
}

func TestAddPerformance(t *testing.T) {
	s := NewSession("test", LevelVerbose)
	s.AddPerformance("latency", "5ms")
	if s.Performance["latency"] != "5ms" {
		t.Error("VERBOSE should record performance data")
	}
}

// ───────────────────────────────────────────────────────────────────────────
// End — verify session completion and output
// ───────────────────────────────────────────────────────────────────────────

func TestEndProducesOutput(t *testing.T) {
	var buf bytes.Buffer
	s := NewSession("test", LevelBasic)
	s.SetOutput(&buf)
	s.SetInitialHealth(50)
	s.End(ResultSuccess, 0, 75, map[string]string{"status": "done"})

	output := buf.String()
	if !strings.Contains(output, "DEBUG_SESSION_START") {
		t.Error("output should contain DEBUG_SESSION_START")
	}
	if !strings.Contains(output, "DEBUG_SESSION_END") {
		t.Error("output should contain DEBUG_SESSION_END")
	}
	if !strings.Contains(output, "SUCCESS") {
		t.Error("output should contain result SUCCESS")
	}
}

func TestEndDetailedIncludesEvents(t *testing.T) {
	var buf bytes.Buffer
	s := NewSession("test", LevelDetailed)
	s.SetOutput(&buf)
	s.Info(EventInit, "initialized", nil, 50, 50)
	s.End(ResultSuccess, 0, 50, nil)

	output := buf.String()
	if !strings.Contains(output, "EVENTS") {
		t.Error("DETAILED output should contain EVENTS section")
	}
}

func TestEndVerboseIncludesAnalysis(t *testing.T) {
	var buf bytes.Buffer
	s := NewSession("test", LevelVerbose)
	s.SetOutput(&buf)
	s.AddIssue("test issue")
	s.End(ResultPartial, 1, 40, nil)

	output := buf.String()
	if !strings.Contains(output, "ANALYSIS") {
		t.Error("VERBOSE output should contain ANALYSIS section")
	}
}

// ───────────────────────────────────────────────────────────────────────────
// Helpers — healthInfo, truncate, clamp, formatDuration
// ───────────────────────────────────────────────────────────────────────────

func TestHealthInfoExcellent(t *testing.T) {
	emoji, text := healthInfo(95)
	if text != "Excellent" {
		t.Errorf("healthInfo(95) text = %q, want %q", text, "Excellent")
	}
	if emoji == "" {
		t.Error("emoji should not be empty")
	}
}

func TestHealthInfoFailed(t *testing.T) {
	_, text := healthInfo(0)
	if text != "Failed" {
		t.Errorf("healthInfo(0) text = %q, want %q", text, "Failed")
	}
}

func TestTruncateShort(t *testing.T) {
	got := truncate("hello", 10)
	if got != "hello" {
		t.Errorf("truncate short = %q, want %q", got, "hello")
	}
}

func TestTruncateLong(t *testing.T) {
	got := truncate("this is a very long string", 10)
	if len(got) != 10 {
		t.Errorf("truncated length = %d, want 10", len(got))
	}
	if !strings.HasSuffix(got, "...") {
		t.Error("truncated string should end with ...")
	}
}

func TestClampWithinRange(t *testing.T) {
	if got := clamp(50, 0, 100); got != 50 {
		t.Errorf("clamp(50, 0, 100) = %d, want 50", got)
	}
}

func TestClampBelowMin(t *testing.T) {
	if got := clamp(-10, 0, 100); got != 0 {
		t.Errorf("clamp(-10, 0, 100) = %d, want 0", got)
	}
}

func TestClampAboveMax(t *testing.T) {
	if got := clamp(200, 0, 100); got != 100 {
		t.Errorf("clamp(200, 0, 100) = %d, want 100", got)
	}
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
