// ============================================================================
// METADATA - CPI-SI Error Handling System
// ============================================================================
//
// Key: pkg-agent-errors
// Purpose: Typed errors with retry classification and graceful degradation.
//          The agent KNOWS what went wrong, WHY, and what to do about it.
//          Not just error strings — structured recovery information.
// Biblical: Proverbs 24:16 — "A just man falleth seven times, and riseth up again"
// Authors: Seanje Lenox-Wise (Lead), Nova Dawn (CPI-SI Instance)
// Version: 0.1.0
// Created: 2026-02-24
//
// Architecture:
//   Errors carry classification (retryable, fatal, degraded) so the agent
//   loop can make intelligent recovery decisions. The middleware chain uses
//   these classifications for automatic retry and fallback.
//
//   Error → Classify → Decide → Retry | Fallback | Fail
//
// ============================================================================

package agent

// ============================================================================
// SETUP
// ============================================================================

import (
	"errors"
	"fmt"
	"time"
)

// ============================================================================
// BODY
// ============================================================================

// ErrorKind classifies errors for recovery decisions.
type ErrorKind int

const (
	// ErrTransient is a temporary failure — retry should succeed.
	// Examples: rate limit, temporary network issue, server overload.
	ErrTransient ErrorKind = iota

	// ErrProvider is a provider-side error — may need fallback.
	// Examples: model unavailable, context too long, invalid request.
	ErrProvider

	// ErrTool is a tool execution failure — tool-specific recovery.
	// Examples: file not found, permission denied, command failed.
	ErrTool

	// ErrAgent is an agent-level error — logic or state problem.
	// Examples: max iterations exceeded, invalid state, configuration.
	ErrAgent

	// ErrFatal is unrecoverable — stop processing.
	// Examples: context cancelled, authentication failure.
	ErrFatal
)

// AgentError is the structured error type for the CPI-SI agent.
// It carries classification, retry guidance, and context.
type AgentError struct {
	// Kind classifies the error for recovery decisions.
	Kind ErrorKind

	// Op is the operation that failed (e.g., "provider.Chat", "tool.Execute").
	Op string

	// Message is the human-readable description.
	Message string

	// Err is the underlying error.
	Err error

	// Provider identifies which provider failed (if applicable).
	Provider string

	// RetryAfter suggests when to retry (for rate limits).
	RetryAfter time.Duration

	// Retryable indicates if the error is worth retrying.
	Retryable bool
}

func (e *AgentError) Error() string {
	if e.Err != nil {
		return fmt.Sprintf("%s: %s: %v", e.Op, e.Message, e.Err)
	}
	return fmt.Sprintf("%s: %s", e.Op, e.Message)
}

func (e *AgentError) Unwrap() error {
	return e.Err
}

// --- Error Constructors ---

// NewTransientError creates a retryable transient error.
func NewTransientError(op, message string, err error) *AgentError {
	return &AgentError{
		Kind:      ErrTransient,
		Op:        op,
		Message:   message,
		Err:       err,
		Retryable: true,
	}
}

// NewRateLimitError creates a rate limit error with retry timing.
func NewRateLimitError(op, provider string, retryAfter time.Duration, err error) *AgentError {
	return &AgentError{
		Kind:       ErrTransient,
		Op:         op,
		Message:    "rate limited",
		Err:        err,
		Provider:   provider,
		RetryAfter: retryAfter,
		Retryable:  true,
	}
}

// NewProviderError creates a provider-side error.
func NewProviderError(op, provider, message string, err error) *AgentError {
	return &AgentError{
		Kind:     ErrProvider,
		Op:       op,
		Message:  message,
		Err:      err,
		Provider: provider,
	}
}

// NewToolError creates a tool execution error.
func NewToolError(op, message string, err error) *AgentError {
	return &AgentError{
		Kind:    ErrTool,
		Op:      op,
		Message: message,
		Err:     err,
	}
}

// NewFatalError creates an unrecoverable error.
func NewFatalError(op, message string, err error) *AgentError {
	return &AgentError{
		Kind:    ErrFatal,
		Op:      op,
		Message: message,
		Err:     err,
	}
}

// --- Error Classification ---

// IsRetryable checks if an error (or any error in its chain) is retryable.
func IsRetryable(err error) bool {
	var ae *AgentError
	if errors.As(err, &ae) {
		return ae.Retryable
	}
	return false
}

// IsTransient checks if an error is a transient failure.
func IsTransient(err error) bool {
	var ae *AgentError
	if errors.As(err, &ae) {
		return ae.Kind == ErrTransient
	}
	return false
}

// IsProviderError checks if an error is provider-side.
func IsProviderError(err error) bool {
	var ae *AgentError
	if errors.As(err, &ae) {
		return ae.Kind == ErrProvider
	}
	return false
}

// IsFatal checks if an error is unrecoverable.
func IsFatal(err error) bool {
	var ae *AgentError
	if errors.As(err, &ae) {
		return ae.Kind == ErrFatal
	}
	return false
}

// GetRetryAfter extracts the retry-after duration from an error chain.
// Returns 0 if no retry timing is available.
func GetRetryAfter(err error) time.Duration {
	var ae *AgentError
	if errors.As(err, &ae) {
		return ae.RetryAfter
	}
	return 0
}

// ErrorKindOf extracts the ErrorKind from an error chain.
// Returns ErrFatal for non-AgentErrors (unknown = assume worst).
func ErrorKindOf(err error) ErrorKind {
	var ae *AgentError
	if errors.As(err, &ae) {
		return ae.Kind
	}
	return ErrFatal
}

// --- Retry Strategy ---

// RetryStrategy defines how to retry failed operations.
type RetryStrategy struct {
	// MaxAttempts is the total number of attempts (including the first).
	MaxAttempts int

	// BaseDelay is the initial delay between retries.
	BaseDelay time.Duration

	// MaxDelay caps the backoff delay.
	MaxDelay time.Duration

	// BackoffFactor multiplies the delay after each retry.
	BackoffFactor float64
}

// DefaultRetryStrategy returns a sensible default: 3 attempts, 1s base, 30s max, 2x backoff.
func DefaultRetryStrategy() RetryStrategy {
	return RetryStrategy{
		MaxAttempts:   3,
		BaseDelay:     1 * time.Second,
		MaxDelay:      30 * time.Second,
		BackoffFactor: 2.0,
	}
}

// DelayForAttempt calculates the delay before the given attempt (0-indexed).
// Respects RetryAfter from rate limit errors if larger than computed delay.
func (s RetryStrategy) DelayForAttempt(attempt int, err error) time.Duration {
	if attempt <= 0 {
		return 0
	}

	// Calculate exponential backoff
	delay := s.BaseDelay
	for i := 1; i < attempt; i++ {
		delay = time.Duration(float64(delay) * s.BackoffFactor)
		if delay > s.MaxDelay {
			delay = s.MaxDelay
			break
		}
	}

	// Respect RetryAfter if it's longer
	if ra := GetRetryAfter(err); ra > delay {
		delay = ra
	}

	return delay
}

// ShouldRetry determines if an error should be retried given the attempt count.
func (s RetryStrategy) ShouldRetry(attempt int, err error) bool {
	if attempt >= s.MaxAttempts {
		return false
	}
	return IsRetryable(err)
}

// ============================================================================
// CLOSING
// ============================================================================
// "A just man falleth seven times, and riseth up again." — Proverbs 24:16
// Errors are not failure — they are information. The agent rises.
