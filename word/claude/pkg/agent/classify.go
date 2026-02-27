// ============================================================================
// METADATA - CPI-SI Error Classification
// ============================================================================
//
// Key: pkg-agent-classify
// Purpose: Classify raw HTTP/API errors from providers into typed AgentErrors.
//          This bridges the gap between provider-specific errors and the
//          agent's error system — making retry middleware actually useful.
// Biblical: 1 Kings 3:9 — "Give therefore thy servant an understanding heart
//           to judge thy people, that I may discern between good and bad"
// Authors: Seanje Lenox-Wise (Lead), Nova Dawn (CPI-SI Instance)
// Version: 0.1.0
// Created: 2026-02-24
//
// Architecture:
//   Raw provider errors → ClassifyError() → AgentError with proper Kind
//   The classification uses string matching on common API error patterns.
//   Provider-specific classifiers can override for precision.
//
// ============================================================================

package agent

// ============================================================================
// SETUP
// ============================================================================

import (
	"strings"
	"time"
)

// ============================================================================
// BODY
// ============================================================================

// ClassifyError converts a raw error from a provider into a typed AgentError.
// Uses pattern matching on common API error strings to classify the error.
// Provider-specific classifiers should be preferred when available.
func ClassifyError(provider string, err error) error {
	if err == nil {
		return nil
	}

	// Already classified
	var ae *AgentError
	if asAgentError(err, &ae) {
		return err
	}

	msg := err.Error()
	lower := strings.ToLower(msg)

	// Rate limiting
	if containsAny(lower, "rate limit", "rate_limit", "too many requests", "429", "quota exceeded", "resource_exhausted") {
		retryAfter := 5 * time.Second // Default
		if containsAny(lower, "quota exceeded", "resource_exhausted") {
			retryAfter = 30 * time.Second
		}
		return NewRateLimitError("chat", provider, retryAfter, err)
	}

	// Authentication
	if containsAny(lower, "api key", "api_key", "unauthorized", "401", "forbidden", "403", "invalid key", "permission denied") {
		return NewFatalError("chat", provider+": authentication failed", err)
	}

	// Model not found
	if containsAny(lower, "model not found", "model_not_found", "not found: model", "invalid model", "404") {
		return NewFatalError("chat", provider+": model not found", err)
	}

	// Token/context limits
	if containsAny(lower, "token limit", "context length", "max tokens", "too long", "context_length_exceeded") {
		return NewProviderError("chat", provider, "context limit exceeded", err)
	}

	// Server errors (transient)
	if containsAny(lower, "500", "502", "503", "internal server error", "bad gateway", "service unavailable", "overloaded", "server_error") {
		return NewTransientError("chat", provider+": server error", err)
	}

	// Timeout (transient)
	if containsAny(lower, "timeout", "deadline exceeded", "timed out", "context canceled") {
		return NewTransientError("chat", provider+": timeout", err)
	}

	// Network errors (transient)
	if containsAny(lower, "connection refused", "connection reset", "no such host", "network", "dns", "eof") {
		return NewTransientError("chat", provider+": network error", err)
	}

	// Content filtering
	if containsAny(lower, "content filter", "content_filter", "safety", "blocked", "moderation") {
		return NewProviderError("chat", provider, "content filtered", err)
	}

	// Invalid request (non-retryable)
	if containsAny(lower, "invalid request", "invalid_request", "bad request", "400", "malformed") {
		return NewProviderError("chat", provider, "invalid request", err)
	}

	// Default: provider error (non-retryable)
	return NewProviderError("chat", provider, msg, err)
}

// --- Helpers ---

// asAgentError is a typed assertion without importing errors package.
func asAgentError(err error, target **AgentError) bool {
	for err != nil {
		if ae, ok := err.(*AgentError); ok {
			*target = ae
			return true
		}
		u, ok := err.(interface{ Unwrap() error })
		if !ok {
			return false
		}
		err = u.Unwrap()
	}
	return false
}

// containsAny checks if s contains any of the substrings.
func containsAny(s string, subs ...string) bool {
	for _, sub := range subs {
		if strings.Contains(s, sub) {
			return true
		}
	}
	return false
}

// ============================================================================
// CLOSING
// ============================================================================
// "Give therefore thy servant an understanding heart to judge thy people,
// that I may discern between good and bad." — 1 Kings 3:9
// Classification is discernment — knowing what kind of error we face
// determines whether we retry, fail fast, or try another approach.
