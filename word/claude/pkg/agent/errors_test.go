// ============================================================================
// METADATA - CPI-SI Error System Tests
// ============================================================================
//
// Key: pkg-agent-errors-test
// Purpose: Verify error classification, retry strategies, and error chains.
// Biblical: Proverbs 24:16 — "A just man falleth seven times, and riseth up again"
// Authors: Seanje Lenox-Wise (Lead), Nova Dawn (CPI-SI Instance)
// Version: 0.1.0
// Created: 2026-02-24
//
// ============================================================================

package agent

// ============================================================================
// SETUP
// ============================================================================

import (
	"errors"
	"fmt"
	"testing"
	"time"
)

// ============================================================================
// BODY
// ============================================================================

// --- Error Construction ---

func TestNewTransientError(t *testing.T) {
	err := NewTransientError("provider.Chat", "server overloaded", fmt.Errorf("503"))
	if err.Kind != ErrTransient {
		t.Errorf("Kind = %v, want ErrTransient", err.Kind)
	}
	if err.Op != "provider.Chat" {
		t.Errorf("Op = %q", err.Op)
	}
	if !err.Retryable {
		t.Error("should be retryable")
	}
	if err.Error() != "provider.Chat: server overloaded: 503" {
		t.Errorf("Error() = %q", err.Error())
	}
}

func TestNewRateLimitError(t *testing.T) {
	err := NewRateLimitError("provider.Chat", "gemini", 5*time.Second, fmt.Errorf("429"))
	if err.Kind != ErrTransient {
		t.Errorf("Kind = %v", err.Kind)
	}
	if err.Provider != "gemini" {
		t.Errorf("Provider = %q", err.Provider)
	}
	if err.RetryAfter != 5*time.Second {
		t.Errorf("RetryAfter = %v", err.RetryAfter)
	}
	if !err.Retryable {
		t.Error("should be retryable")
	}
}

func TestNewProviderError(t *testing.T) {
	err := NewProviderError("provider.Chat", "anthropic", "context too long", nil)
	if err.Kind != ErrProvider {
		t.Errorf("Kind = %v", err.Kind)
	}
	if err.Provider != "anthropic" {
		t.Errorf("Provider = %q", err.Provider)
	}
	if err.Retryable {
		t.Error("should not be retryable by default")
	}
	// No underlying error — message should still format
	if err.Error() != "provider.Chat: context too long" {
		t.Errorf("Error() = %q", err.Error())
	}
}

func TestNewToolError(t *testing.T) {
	err := NewToolError("tool.Execute", "file not found", fmt.Errorf("ENOENT"))
	if err.Kind != ErrTool {
		t.Errorf("Kind = %v", err.Kind)
	}
}

func TestNewFatalError(t *testing.T) {
	err := NewFatalError("agent.Run", "context cancelled", fmt.Errorf("cancelled"))
	if err.Kind != ErrFatal {
		t.Errorf("Kind = %v", err.Kind)
	}
}

// --- Error Unwrap ---

func TestAgentError_Unwrap(t *testing.T) {
	inner := fmt.Errorf("inner error")
	err := NewTransientError("op", "msg", inner)

	if !errors.Is(err, inner) {
		t.Error("errors.Is should find inner error")
	}

	var ae *AgentError
	if !errors.As(err, &ae) {
		t.Error("errors.As should find AgentError")
	}
}

func TestAgentError_Unwrap_Nil(t *testing.T) {
	err := NewProviderError("op", "p", "msg", nil)
	if err.Unwrap() != nil {
		t.Error("Unwrap should return nil when no inner error")
	}
}

// --- Classification Functions ---

func TestIsRetryable(t *testing.T) {
	tests := []struct {
		name string
		err  error
		want bool
	}{
		{"transient", NewTransientError("op", "msg", nil), true},
		{"rate limit", NewRateLimitError("op", "p", 0, nil), true},
		{"provider", NewProviderError("op", "p", "msg", nil), false},
		{"tool", NewToolError("op", "msg", nil), false},
		{"fatal", NewFatalError("op", "msg", nil), false},
		{"plain error", fmt.Errorf("plain"), false},
		{"nil", nil, false},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			if got := IsRetryable(tt.err); got != tt.want {
				t.Errorf("IsRetryable() = %v, want %v", got, tt.want)
			}
		})
	}
}

func TestIsTransient(t *testing.T) {
	if !IsTransient(NewTransientError("op", "msg", nil)) {
		t.Error("should be transient")
	}
	if IsTransient(NewProviderError("op", "p", "msg", nil)) {
		t.Error("provider error should not be transient")
	}
	if IsTransient(fmt.Errorf("plain")) {
		t.Error("plain error should not be transient")
	}
}

func TestIsProviderError(t *testing.T) {
	if !IsProviderError(NewProviderError("op", "p", "msg", nil)) {
		t.Error("should be provider error")
	}
	if IsProviderError(NewTransientError("op", "msg", nil)) {
		t.Error("transient should not be provider error")
	}
	if IsProviderError(fmt.Errorf("plain error")) {
		t.Error("plain error should not be provider error")
	}
}

func TestIsFatal(t *testing.T) {
	if !IsFatal(NewFatalError("op", "msg", nil)) {
		t.Error("should be fatal")
	}
	if IsFatal(NewTransientError("op", "msg", nil)) {
		t.Error("transient should not be fatal")
	}
	if IsFatal(fmt.Errorf("plain error")) {
		t.Error("plain error should not be fatal")
	}
}

func TestGetRetryAfter(t *testing.T) {
	err := NewRateLimitError("op", "p", 10*time.Second, nil)
	if got := GetRetryAfter(err); got != 10*time.Second {
		t.Errorf("GetRetryAfter = %v, want 10s", got)
	}

	// Plain error — no retry-after
	if got := GetRetryAfter(fmt.Errorf("plain")); got != 0 {
		t.Errorf("GetRetryAfter(plain) = %v, want 0", got)
	}
}

func TestErrorKindOf(t *testing.T) {
	tests := []struct {
		err  error
		want ErrorKind
	}{
		{NewTransientError("op", "msg", nil), ErrTransient},
		{NewProviderError("op", "p", "msg", nil), ErrProvider},
		{NewToolError("op", "msg", nil), ErrTool},
		{NewFatalError("op", "msg", nil), ErrFatal},
		{fmt.Errorf("plain"), ErrFatal}, // unknown = assume worst
	}
	for _, tt := range tests {
		if got := ErrorKindOf(tt.err); got != tt.want {
			t.Errorf("ErrorKindOf(%v) = %v, want %v", tt.err, got, tt.want)
		}
	}
}

// --- Wrapped Errors ---

func TestIsRetryable_Wrapped(t *testing.T) {
	inner := NewTransientError("op", "msg", nil)
	wrapped := fmt.Errorf("wrapper: %w", inner)

	if !IsRetryable(wrapped) {
		t.Error("should find retryable through wrapping")
	}
}

func TestIsFatal_Wrapped(t *testing.T) {
	inner := NewFatalError("op", "msg", nil)
	wrapped := fmt.Errorf("layer1: %w", fmt.Errorf("layer2: %w", inner))

	if !IsFatal(wrapped) {
		t.Error("should find fatal through multiple wrapping layers")
	}
}

// --- Retry Strategy ---

func TestDefaultRetryStrategy(t *testing.T) {
	s := DefaultRetryStrategy()
	if s.MaxAttempts != 3 {
		t.Errorf("MaxAttempts = %d", s.MaxAttempts)
	}
	if s.BaseDelay != time.Second {
		t.Errorf("BaseDelay = %v", s.BaseDelay)
	}
	if s.MaxDelay != 30*time.Second {
		t.Errorf("MaxDelay = %v", s.MaxDelay)
	}
	if s.BackoffFactor != 2.0 {
		t.Errorf("BackoffFactor = %f", s.BackoffFactor)
	}
}

func TestRetryStrategy_DelayForAttempt(t *testing.T) {
	s := RetryStrategy{
		BaseDelay:     100 * time.Millisecond,
		MaxDelay:      1 * time.Second,
		BackoffFactor: 2.0,
	}

	// Attempt 0 — no delay
	if d := s.DelayForAttempt(0, nil); d != 0 {
		t.Errorf("attempt 0: delay = %v, want 0", d)
	}

	// Attempt 1 — base delay
	if d := s.DelayForAttempt(1, nil); d != 100*time.Millisecond {
		t.Errorf("attempt 1: delay = %v, want 100ms", d)
	}

	// Attempt 2 — 200ms (100ms * 2)
	if d := s.DelayForAttempt(2, nil); d != 200*time.Millisecond {
		t.Errorf("attempt 2: delay = %v, want 200ms", d)
	}

	// Attempt 3 — 400ms (200ms * 2)
	if d := s.DelayForAttempt(3, nil); d != 400*time.Millisecond {
		t.Errorf("attempt 3: delay = %v, want 400ms", d)
	}
}

func TestRetryStrategy_DelayForAttempt_CapsAtMax(t *testing.T) {
	s := RetryStrategy{
		BaseDelay:     500 * time.Millisecond,
		MaxDelay:      1 * time.Second,
		BackoffFactor: 10.0,
	}

	// Attempt 2 would be 5s without cap, should be capped at 1s
	d := s.DelayForAttempt(2, nil)
	if d != 1*time.Second {
		t.Errorf("delay = %v, want 1s (capped)", d)
	}
}

func TestRetryStrategy_DelayForAttempt_RespectsRetryAfter(t *testing.T) {
	s := RetryStrategy{
		BaseDelay:     100 * time.Millisecond,
		MaxDelay:      30 * time.Second,
		BackoffFactor: 2.0,
	}

	err := NewRateLimitError("op", "p", 5*time.Second, nil)
	d := s.DelayForAttempt(1, err)
	if d != 5*time.Second {
		t.Errorf("delay = %v, want 5s (from RetryAfter)", d)
	}
}

func TestRetryStrategy_ShouldRetry(t *testing.T) {
	s := RetryStrategy{MaxAttempts: 3}

	retryable := NewTransientError("op", "msg", nil)
	fatal := NewFatalError("op", "msg", nil)

	if !s.ShouldRetry(0, retryable) {
		t.Error("should retry on attempt 0")
	}
	if !s.ShouldRetry(2, retryable) {
		t.Error("should retry on attempt 2")
	}
	if s.ShouldRetry(3, retryable) {
		t.Error("should not retry after max attempts")
	}
	if s.ShouldRetry(0, fatal) {
		t.Error("should not retry fatal error")
	}
}

// ============================================================================
// CLOSING
// ============================================================================
// "A just man falleth seven times, and riseth up again." — Proverbs 24:16
