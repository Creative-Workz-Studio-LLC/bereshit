// ============================================================================
// METADATA - CPI-SI Middleware Chain
// ============================================================================
//
// Key: pkg-agent-middleware
// Purpose: Composable pre/post processing for provider calls. Each middleware
//          wraps the provider — logging, retry, rate limiting, guardrails.
//          The chain is the agent's immune system for provider interactions.
// Biblical: Nehemiah 4:17 — "With one of his hands wrought in the work, and
//           with the other hand held a weapon"
// Authors: Seanje Lenox-Wise (Lead), Nova Dawn (CPI-SI Instance)
// Version: 0.1.0
// Created: 2026-02-24
//
// Architecture:
//   Middleware wraps Provider. Each middleware IS a Provider — composable
//   by nesting. The pattern is the decorator pattern applied to providers.
//
//   Request → [Logging → Retry → RateLimit → Actual Provider] → Response
//
//   Middlewares can:
//   - Modify the request before it reaches the provider
//   - Modify the response before it reaches the agent
//   - Short-circuit the call entirely (e.g., guardrails)
//   - Retry failed calls (using the error classification system)
//
// ============================================================================

package agent

// ============================================================================
// SETUP
// ============================================================================

import (
	"context"
	"fmt"
	"io"
	"sync"
	"time"
)

// ============================================================================
// BODY
// ============================================================================

// Middleware wraps a Provider with additional behavior.
// The function takes the inner provider and returns the wrapped provider.
type Middleware func(Provider) Provider

// Chain applies middlewares to a provider in order.
// The first middleware in the list is the outermost wrapper.
//
//	Chain(provider, logging, retry, rateLimit)
//	→ logging(retry(rateLimit(provider)))
func Chain(provider Provider, middlewares ...Middleware) Provider {
	for i := len(middlewares) - 1; i >= 0; i-- {
		provider = middlewares[i](provider)
	}
	return provider
}

// --- Retry Middleware ---

// RetryMiddleware retries transient errors using the error classification system.
type RetryMiddleware struct {
	inner    Provider
	strategy RetryStrategy
	onRetry  func(attempt int, err error, delay time.Duration) // optional callback
}

// WithRetry creates a middleware that retries transient failures.
func WithRetry(strategy RetryStrategy) Middleware {
	return func(inner Provider) Provider {
		return &RetryMiddleware{
			inner:    inner,
			strategy: strategy,
		}
	}
}

// WithRetryCallback creates a retry middleware with an attempt callback.
func WithRetryCallback(strategy RetryStrategy, onRetry func(attempt int, err error, delay time.Duration)) Middleware {
	return func(inner Provider) Provider {
		return &RetryMiddleware{
			inner:    inner,
			strategy: strategy,
			onRetry:  onRetry,
		}
	}
}

func (m *RetryMiddleware) Chat(ctx context.Context, req ChatRequest) (*ChatResponse, error) {
	var lastErr error
	for attempt := 0; attempt < m.strategy.MaxAttempts; attempt++ {
		if attempt > 0 {
			delay := m.strategy.DelayForAttempt(attempt, lastErr)
			if m.onRetry != nil {
				m.onRetry(attempt, lastErr, delay)
			}
			select {
			case <-ctx.Done():
				return nil, NewFatalError("retry", "context cancelled during retry", ctx.Err())
			case <-time.After(delay):
			}
		}

		resp, err := m.inner.Chat(ctx, req)
		if err == nil {
			return resp, nil
		}

		lastErr = err

		// Non-retryable errors bail immediately
		if !IsRetryable(err) {
			return nil, err
		}
	}

	// All attempts exhausted
	return nil, fmt.Errorf("all %d retry attempts failed: %w", m.strategy.MaxAttempts, lastErr)
}

func (m *RetryMiddleware) Name() string  { return m.inner.Name() }
func (m *RetryMiddleware) Model() string { return m.inner.Model() }

// --- Logging Middleware ---

// LoggingMiddleware logs provider calls for observability.
type LoggingMiddleware struct {
	inner  Provider
	writer io.Writer
}

// WithLogging creates a middleware that logs provider interactions.
func WithLogging(w io.Writer) Middleware {
	return func(inner Provider) Provider {
		return &LoggingMiddleware{inner: inner, writer: w}
	}
}

func (m *LoggingMiddleware) Chat(ctx context.Context, req ChatRequest) (*ChatResponse, error) {
	start := time.Now()
	msgCount := len(req.Messages)
	toolCount := len(req.Tools)

	resp, err := m.inner.Chat(ctx, req)
	elapsed := time.Since(start)

	if err != nil {
		fmt.Fprintf(m.writer, "  [%s] ERROR %dms msgs=%d tools=%d: %v\n",
			m.inner.Name(), elapsed.Milliseconds(), msgCount, toolCount, err)
	} else {
		fmt.Fprintf(m.writer, "  [%s] OK %dms msgs=%d tools=%d in=%d out=%d\n",
			m.inner.Name(), elapsed.Milliseconds(), msgCount, toolCount,
			resp.InputTokens, resp.OutputTokens)
	}

	return resp, err
}

func (m *LoggingMiddleware) Name() string  { return m.inner.Name() }
func (m *LoggingMiddleware) Model() string { return m.inner.Model() }

// --- Usage Tracking Middleware ---

// UsageTracker accumulates token usage across multiple calls.
type UsageTracker struct {
	inner Provider
	mu    sync.Mutex
	stats UsageStats
}

// UsageStats holds cumulative usage across provider calls.
type UsageStats struct {
	TotalCalls   int
	InputTokens  int
	OutputTokens int
	TotalLatency time.Duration
	Errors       int
}

// WithUsageTracking creates a middleware that tracks cumulative usage.
func WithUsageTracking() Middleware {
	return func(inner Provider) Provider {
		return &UsageTracker{inner: inner}
	}
}

func (m *UsageTracker) Chat(ctx context.Context, req ChatRequest) (*ChatResponse, error) {
	start := time.Now()
	resp, err := m.inner.Chat(ctx, req)
	elapsed := time.Since(start)

	m.mu.Lock()
	m.stats.TotalCalls++
	m.stats.TotalLatency += elapsed
	if err != nil {
		m.stats.Errors++
	} else {
		m.stats.InputTokens += resp.InputTokens
		m.stats.OutputTokens += resp.OutputTokens
	}
	m.mu.Unlock()

	return resp, err
}

func (m *UsageTracker) Name() string  { return m.inner.Name() }
func (m *UsageTracker) Model() string { return m.inner.Model() }

// Stats returns a snapshot of the current usage statistics.
func (m *UsageTracker) Stats() UsageStats {
	m.mu.Lock()
	defer m.mu.Unlock()
	return m.stats
}

// Reset zeroes the usage statistics.
func (m *UsageTracker) Reset() {
	m.mu.Lock()
	defer m.mu.Unlock()
	m.stats = UsageStats{}
}

// --- Guardrail Middleware ---

// GuardrailFunc inspects a request and returns an error to block it,
// or nil to allow it through.
type GuardrailFunc func(req ChatRequest) error

// GuardrailMiddleware applies safety checks before forwarding to the provider.
type GuardrailMiddleware struct {
	inner  Provider
	checks []GuardrailFunc
}

// WithGuardrails creates a middleware that applies pre-flight checks.
func WithGuardrails(checks ...GuardrailFunc) Middleware {
	return func(inner Provider) Provider {
		return &GuardrailMiddleware{inner: inner, checks: checks}
	}
}

func (m *GuardrailMiddleware) Chat(ctx context.Context, req ChatRequest) (*ChatResponse, error) {
	for _, check := range m.checks {
		if err := check(req); err != nil {
			return nil, NewProviderError("guardrail", m.inner.Name(), "request blocked", err)
		}
	}
	return m.inner.Chat(ctx, req)
}

func (m *GuardrailMiddleware) Name() string  { return m.inner.Name() }
func (m *GuardrailMiddleware) Model() string { return m.inner.Model() }

// --- Built-in Guardrails ---

// MaxTokensGuardrail blocks requests that would exceed a token budget.
func MaxTokensGuardrail(maxTokens int) GuardrailFunc {
	return func(req ChatRequest) error {
		total := 0
		for _, m := range req.Messages {
			total += len(m.Content) / 4 // rough token estimate
		}
		if total > maxTokens {
			return fmt.Errorf("estimated %d tokens exceeds max %d", total, maxTokens)
		}
		return nil
	}
}

// MaxMessagesGuardrail blocks requests with too many messages.
func MaxMessagesGuardrail(maxMessages int) GuardrailFunc {
	return func(req ChatRequest) error {
		if len(req.Messages) > maxMessages {
			return fmt.Errorf("%d messages exceeds max %d", len(req.Messages), maxMessages)
		}
		return nil
	}
}

// --- Timeout Middleware ---

// TimeoutMiddleware enforces a maximum duration for provider calls.
type TimeoutMiddleware struct {
	inner   Provider
	timeout time.Duration
}

// WithTimeout creates a middleware that limits provider call duration.
func WithTimeout(timeout time.Duration) Middleware {
	return func(inner Provider) Provider {
		return &TimeoutMiddleware{inner: inner, timeout: timeout}
	}
}

func (m *TimeoutMiddleware) Chat(ctx context.Context, req ChatRequest) (*ChatResponse, error) {
	ctx, cancel := context.WithTimeout(ctx, m.timeout)
	defer cancel()

	resp, err := m.inner.Chat(ctx, req)
	if ctx.Err() == context.DeadlineExceeded {
		return nil, NewTransientError("timeout", fmt.Sprintf("provider call exceeded %s", m.timeout), ctx.Err())
	}
	return resp, err
}

func (m *TimeoutMiddleware) Name() string  { return m.inner.Name() }
func (m *TimeoutMiddleware) Model() string { return m.inner.Model() }

// ============================================================================
// CLOSING
// ============================================================================
// "With one of his hands wrought in the work, and with the other hand held a
// weapon." — Nehemiah 4:17
// The middleware works AND guards. Protection built into the work itself.
