// ============================================================================
// METADATA - CPI-SI Middleware Tests
// ============================================================================
//
// Key: pkg-agent-middleware-test
// Purpose: Verify middleware chain — retry, logging, usage, guardrails, timeout.
// Biblical: Nehemiah 4:17 — "With one hand wrought in the work"
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
	"bytes"
	"context"
	"fmt"
	"strings"
	"testing"
	"time"
)

// ============================================================================
// BODY
// ============================================================================

// failingProvider fails N times then succeeds.
type failingProvider struct {
	failCount  int
	callCount  int
	failErr    error
	successMsg string
}

func (p *failingProvider) Chat(_ context.Context, req ChatRequest) (*ChatResponse, error) {
	p.callCount++
	if p.callCount <= p.failCount {
		return nil, p.failErr
	}
	return &ChatResponse{
		Content:      p.successMsg,
		InputTokens:  10,
		OutputTokens: 5,
	}, nil
}

func (p *failingProvider) Name() string  { return "failing" }
func (p *failingProvider) Model() string { return "fail-v1" }

// --- Chain Tests ---

func TestChain_Order(t *testing.T) {
	var order []string
	inner := &mockProvider{
		responses: []*ChatResponse{{Content: "ok"}},
	}

	// Create two tracking middlewares
	first := func(p Provider) Provider {
		return &trackingMiddleware{inner: p, name: "first", order: &order}
	}
	second := func(p Provider) Provider {
		return &trackingMiddleware{inner: p, name: "second", order: &order}
	}

	chained := Chain(inner, first, second)
	chained.Chat(context.Background(), ChatRequest{})

	// first should execute before second (outermost wraps innermost)
	if len(order) != 2 || order[0] != "first" || order[1] != "second" {
		t.Errorf("execution order = %v, want [first, second]", order)
	}
}

// trackingMiddleware records execution order for testing.
type trackingMiddleware struct {
	inner Provider
	name  string
	order *[]string
}

func (m *trackingMiddleware) Chat(ctx context.Context, req ChatRequest) (*ChatResponse, error) {
	*m.order = append(*m.order, m.name)
	return m.inner.Chat(ctx, req)
}
func (m *trackingMiddleware) Name() string  { return m.inner.Name() }
func (m *trackingMiddleware) Model() string { return m.inner.Model() }

// --- Retry Middleware ---

func TestRetryMiddleware_SuccessOnFirstTry(t *testing.T) {
	provider := &failingProvider{failCount: 0, successMsg: "ok"}
	retried := WithRetry(RetryStrategy{
		MaxAttempts:   3,
		BaseDelay:     time.Millisecond,
		BackoffFactor: 1.0,
	})(provider)

	resp, err := retried.Chat(context.Background(), ChatRequest{})
	if err != nil {
		t.Fatal(err)
	}
	if resp.Content != "ok" {
		t.Errorf("Content = %q", resp.Content)
	}
	if provider.callCount != 1 {
		t.Errorf("callCount = %d, want 1", provider.callCount)
	}
}

func TestRetryMiddleware_SuccessAfterRetry(t *testing.T) {
	retryableErr := NewTransientError("test", "temporary", nil)
	provider := &failingProvider{failCount: 2, failErr: retryableErr, successMsg: "recovered"}
	retried := WithRetry(RetryStrategy{
		MaxAttempts:   3,
		BaseDelay:     time.Millisecond,
		BackoffFactor: 1.0,
	})(provider)

	resp, err := retried.Chat(context.Background(), ChatRequest{})
	if err != nil {
		t.Fatal(err)
	}
	if resp.Content != "recovered" {
		t.Errorf("Content = %q", resp.Content)
	}
	if provider.callCount != 3 {
		t.Errorf("callCount = %d, want 3 (2 fails + 1 success)", provider.callCount)
	}
}

func TestRetryMiddleware_AllAttemptsFail(t *testing.T) {
	retryableErr := NewTransientError("test", "permanent-transient", nil)
	provider := &failingProvider{failCount: 10, failErr: retryableErr}
	retried := WithRetry(RetryStrategy{
		MaxAttempts:   3,
		BaseDelay:     time.Millisecond,
		BackoffFactor: 1.0,
	})(provider)

	_, err := retried.Chat(context.Background(), ChatRequest{})
	if err == nil {
		t.Fatal("expected error after all retries exhausted")
	}
	if provider.callCount != 3 {
		t.Errorf("callCount = %d, want 3", provider.callCount)
	}
	if !strings.Contains(err.Error(), "retry attempts failed") {
		t.Errorf("error should mention retry failure, got: %q", err)
	}
}

func TestRetryMiddleware_NonRetryableSkipsRetry(t *testing.T) {
	fatalErr := NewFatalError("test", "auth failed", nil)
	provider := &failingProvider{failCount: 10, failErr: fatalErr}
	retried := WithRetry(RetryStrategy{
		MaxAttempts:   3,
		BaseDelay:     time.Millisecond,
		BackoffFactor: 1.0,
	})(provider)

	_, err := retried.Chat(context.Background(), ChatRequest{})
	if err == nil {
		t.Fatal("expected error")
	}
	if provider.callCount != 1 {
		t.Errorf("callCount = %d, want 1 (no retry for fatal)", provider.callCount)
	}
}

func TestRetryMiddleware_ContextCancellation(t *testing.T) {
	retryableErr := NewTransientError("test", "temp", nil)
	provider := &failingProvider{failCount: 10, failErr: retryableErr}
	retried := WithRetry(RetryStrategy{
		MaxAttempts:   5,
		BaseDelay:     100 * time.Millisecond,
		BackoffFactor: 1.0,
	})(provider)

	ctx, cancel := context.WithCancel(context.Background())
	cancel() // cancel immediately

	_, err := retried.Chat(ctx, ChatRequest{})
	if err == nil {
		t.Fatal("expected error from cancelled context")
	}
	if !IsFatal(err) {
		t.Errorf("expected fatal error from cancellation, got: %v", err)
	}
}

func TestRetryMiddleware_WithCallback(t *testing.T) {
	var attempts []int
	retryableErr := NewTransientError("test", "temp", nil)
	provider := &failingProvider{failCount: 2, failErr: retryableErr, successMsg: "ok"}

	retried := WithRetryCallback(
		RetryStrategy{MaxAttempts: 3, BaseDelay: time.Millisecond, BackoffFactor: 1.0},
		func(attempt int, err error, delay time.Duration) {
			attempts = append(attempts, attempt)
		},
	)(provider)

	_, err := retried.Chat(context.Background(), ChatRequest{})
	if err != nil {
		t.Fatal(err)
	}
	if len(attempts) != 2 {
		t.Errorf("callback called %d times, want 2", len(attempts))
	}
}

func TestRetryMiddleware_NameModel(t *testing.T) {
	provider := &mockProvider{}
	retried := WithRetry(DefaultRetryStrategy())(provider)
	if retried.Name() != "mock" {
		t.Errorf("Name() = %q", retried.Name())
	}
	if retried.Model() != "mock-v1" {
		t.Errorf("Model() = %q", retried.Model())
	}
}

// --- Logging Middleware ---

func TestLoggingMiddleware_Success(t *testing.T) {
	var buf bytes.Buffer
	provider := &mockProvider{
		responses: []*ChatResponse{{Content: "ok", InputTokens: 100, OutputTokens: 50}},
	}
	logged := WithLogging(&buf)(provider)

	resp, err := logged.Chat(context.Background(), ChatRequest{
		Messages: []Message{{Role: RoleUser, Content: "Hi"}},
	})
	if err != nil {
		t.Fatal(err)
	}
	if resp.Content != "ok" {
		t.Errorf("Content = %q", resp.Content)
	}

	output := buf.String()
	if !strings.Contains(output, "[mock]") {
		t.Errorf("log should contain provider name, got: %s", output)
	}
	if !strings.Contains(output, "OK") {
		t.Errorf("log should contain OK, got: %s", output)
	}
	if !strings.Contains(output, "in=100") {
		t.Errorf("log should contain token count, got: %s", output)
	}
}

func TestLoggingMiddleware_Error(t *testing.T) {
	var buf bytes.Buffer
	provider := &failingProvider{failCount: 1, failErr: fmt.Errorf("boom")}
	logged := WithLogging(&buf)(provider)

	logged.Chat(context.Background(), ChatRequest{})

	output := buf.String()
	if !strings.Contains(output, "ERROR") {
		t.Errorf("log should contain ERROR, got: %s", output)
	}
	if !strings.Contains(output, "boom") {
		t.Errorf("log should contain error message, got: %s", output)
	}
}

func TestLoggingMiddleware_NameModel(t *testing.T) {
	var buf bytes.Buffer
	provider := &mockProvider{}
	logged := WithLogging(&buf)(provider)
	if logged.Name() != "mock" {
		t.Errorf("Name() = %q", logged.Name())
	}
	if logged.Model() != "mock-v1" {
		t.Errorf("Model() = %q", logged.Model())
	}
}

// --- Usage Tracking Middleware ---

func TestUsageTracker_AccumulatesUsage(t *testing.T) {
	provider := &mockProvider{
		responses: []*ChatResponse{
			{Content: "a", InputTokens: 10, OutputTokens: 5},
			{Content: "b", InputTokens: 20, OutputTokens: 10},
		},
	}
	tracked := WithUsageTracking()(provider)
	tracker := tracked.(*UsageTracker)

	tracked.Chat(context.Background(), ChatRequest{})
	tracked.Chat(context.Background(), ChatRequest{})

	stats := tracker.Stats()
	if stats.TotalCalls != 2 {
		t.Errorf("TotalCalls = %d", stats.TotalCalls)
	}
	if stats.InputTokens != 30 {
		t.Errorf("InputTokens = %d", stats.InputTokens)
	}
	if stats.OutputTokens != 15 {
		t.Errorf("OutputTokens = %d", stats.OutputTokens)
	}
	if stats.Errors != 0 {
		t.Errorf("Errors = %d", stats.Errors)
	}
}

func TestUsageTracker_TracksErrors(t *testing.T) {
	provider := &failingProvider{failCount: 1, failErr: fmt.Errorf("err")}
	tracked := WithUsageTracking()(provider)
	tracker := tracked.(*UsageTracker)

	tracked.Chat(context.Background(), ChatRequest{})

	stats := tracker.Stats()
	if stats.Errors != 1 {
		t.Errorf("Errors = %d, want 1", stats.Errors)
	}
	if stats.TotalCalls != 1 {
		t.Errorf("TotalCalls = %d", stats.TotalCalls)
	}
}

func TestUsageTracker_Reset(t *testing.T) {
	provider := &mockProvider{
		responses: []*ChatResponse{{Content: "ok", InputTokens: 100}},
	}
	tracked := WithUsageTracking()(provider)
	tracker := tracked.(*UsageTracker)

	tracked.Chat(context.Background(), ChatRequest{})
	tracker.Reset()

	stats := tracker.Stats()
	if stats.TotalCalls != 0 {
		t.Errorf("after reset: TotalCalls = %d", stats.TotalCalls)
	}
	if stats.InputTokens != 0 {
		t.Errorf("after reset: InputTokens = %d", stats.InputTokens)
	}
}

func TestUsageTracker_NameModel(t *testing.T) {
	provider := &mockProvider{}
	tracked := WithUsageTracking()(provider)
	if tracked.Name() != "mock" {
		t.Errorf("Name() = %q", tracked.Name())
	}
	if tracked.Model() != "mock-v1" {
		t.Errorf("Model() = %q", tracked.Model())
	}
}

// --- Guardrail Middleware ---

func TestGuardrailMiddleware_AllowsCleanRequest(t *testing.T) {
	provider := &mockProvider{
		responses: []*ChatResponse{{Content: "ok"}},
	}
	guarded := WithGuardrails(MaxMessagesGuardrail(100))(provider)

	resp, err := guarded.Chat(context.Background(), ChatRequest{
		Messages: []Message{{Role: RoleUser, Content: "Hi"}},
	})
	if err != nil {
		t.Fatal(err)
	}
	if resp.Content != "ok" {
		t.Errorf("Content = %q", resp.Content)
	}
}

func TestGuardrailMiddleware_BlocksOverLimit(t *testing.T) {
	provider := &mockProvider{}
	guarded := WithGuardrails(MaxMessagesGuardrail(2))(provider)

	_, err := guarded.Chat(context.Background(), ChatRequest{
		Messages: []Message{
			{Role: RoleUser, Content: "1"},
			{Role: RoleAssistant, Content: "2"},
			{Role: RoleUser, Content: "3"},
		},
	})
	if err == nil {
		t.Fatal("expected error from guardrail")
	}
	if !IsProviderError(err) {
		t.Errorf("expected provider error type, got: %v", err)
	}
	if !strings.Contains(err.Error(), "request blocked") {
		t.Errorf("error = %q", err)
	}
}

func TestMaxTokensGuardrail(t *testing.T) {
	guard := MaxTokensGuardrail(100)

	// Short message — should pass
	shortReq := ChatRequest{
		Messages: []Message{{Content: "Hello"}},
	}
	if err := guard(shortReq); err != nil {
		t.Errorf("short message should pass: %v", err)
	}

	// Long message — should block
	longReq := ChatRequest{
		Messages: []Message{{Content: string(make([]byte, 1000))}},
	}
	if err := guard(longReq); err == nil {
		t.Error("long message should be blocked")
	}
}

func TestGuardrailMiddleware_MultipleChecks(t *testing.T) {
	provider := &mockProvider{responses: []*ChatResponse{{Content: "ok"}}}
	blocker := func(req ChatRequest) error {
		return fmt.Errorf("blocked")
	}
	passer := func(req ChatRequest) error {
		return nil
	}

	// First check passes, second blocks
	guarded := WithGuardrails(passer, blocker)(provider)
	_, err := guarded.Chat(context.Background(), ChatRequest{})
	if err == nil {
		t.Fatal("second guardrail should block")
	}
}

func TestGuardrailMiddleware_NameModel(t *testing.T) {
	provider := &mockProvider{}
	guarded := WithGuardrails()(provider)
	if guarded.Name() != "mock" {
		t.Errorf("Name() = %q", guarded.Name())
	}
	if guarded.Model() != "mock-v1" {
		t.Errorf("Model() = %q", guarded.Model())
	}
}

// --- Timeout Middleware ---

func TestTimeoutMiddleware_CompletesInTime(t *testing.T) {
	provider := &mockProvider{
		responses: []*ChatResponse{{Content: "fast"}},
	}
	timed := WithTimeout(5 * time.Second)(provider)

	resp, err := timed.Chat(context.Background(), ChatRequest{})
	if err != nil {
		t.Fatal(err)
	}
	if resp.Content != "fast" {
		t.Errorf("Content = %q", resp.Content)
	}
}

func TestTimeoutMiddleware_TimesOut(t *testing.T) {
	// slowProvider blocks until context cancellation
	slow := &slowProvider{delay: 5 * time.Second}
	timed := WithTimeout(10 * time.Millisecond)(slow)

	_, err := timed.Chat(context.Background(), ChatRequest{})
	if err == nil {
		t.Fatal("expected timeout error")
	}
	if !IsTransient(err) {
		t.Errorf("timeout should be transient, got: %v", err)
	}
}

func TestTimeoutMiddleware_NameModel(t *testing.T) {
	provider := &mockProvider{}
	timed := WithTimeout(time.Second)(provider)
	if timed.Name() != "mock" {
		t.Errorf("Name() = %q", timed.Name())
	}
	if timed.Model() != "mock-v1" {
		t.Errorf("Model() = %q", timed.Model())
	}
}

// slowProvider delays responses to test timeouts.
type slowProvider struct {
	delay time.Duration
}

func (p *slowProvider) Chat(ctx context.Context, req ChatRequest) (*ChatResponse, error) {
	select {
	case <-time.After(p.delay):
		return &ChatResponse{Content: "slow"}, nil
	case <-ctx.Done():
		return nil, ctx.Err()
	}
}
func (p *slowProvider) Name() string  { return "slow" }
func (p *slowProvider) Model() string { return "slow-v1" }

// --- Full Chain Integration ---

func TestFullChain_Integration(t *testing.T) {
	var logBuf bytes.Buffer
	retryableErr := NewTransientError("test", "temp", nil)
	provider := &failingProvider{failCount: 1, failErr: retryableErr, successMsg: "recovered"}

	chained := Chain(provider,
		WithLogging(&logBuf),
		WithRetry(RetryStrategy{
			MaxAttempts:   3,
			BaseDelay:     time.Millisecond,
			BackoffFactor: 1.0,
		}),
		WithUsageTracking(),
	)

	resp, err := chained.Chat(context.Background(), ChatRequest{
		Messages: []Message{{Role: RoleUser, Content: "test"}},
	})
	if err != nil {
		t.Fatal(err)
	}
	if resp.Content != "recovered" {
		t.Errorf("Content = %q", resp.Content)
	}

	// Logging wraps the outside, so it sees the retry's final result (success)
	log := logBuf.String()
	if !strings.Contains(log, "OK") {
		t.Errorf("log should show OK (retry succeeded): %s", log)
	}
	if provider.callCount != 2 {
		t.Errorf("provider should be called twice (1 fail + 1 success), got %d", provider.callCount)
	}
}

// ============================================================================
// CLOSING
// ============================================================================
// "With one hand wrought in the work." — Nehemiah 4:17
