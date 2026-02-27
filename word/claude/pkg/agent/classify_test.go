package agent

import (
	"errors"
	"testing"
	"time"
)

func TestClassifyError_Nil(t *testing.T) {
	if err := ClassifyError("test", nil); err != nil {
		t.Errorf("nil error should return nil, got %v", err)
	}
}

func TestClassifyError_AlreadyClassified(t *testing.T) {
	original := NewTransientError("chat", "already classified", nil)
	result := ClassifyError("test", original)
	if result != original {
		t.Error("already-classified errors should pass through")
	}
}

func TestClassifyError_RateLimit(t *testing.T) {
	tests := []string{
		"rate limit exceeded",
		"429 Too Many Requests",
		"quota exceeded for project",
		"resource_exhausted",
		"Rate_Limit hit",
	}

	for _, msg := range tests {
		err := ClassifyError("gemini", errors.New(msg))
		if !IsRetryable(err) {
			t.Errorf("%q should be retryable", msg)
		}
		if !IsTransient(err) {
			t.Errorf("%q should be transient (rate limit)", msg)
		}
		after := GetRetryAfter(err)
		if after == 0 {
			t.Errorf("%q should have retry-after", msg)
		}
	}
}

func TestClassifyError_RateLimit_QuotaLongerDelay(t *testing.T) {
	err := ClassifyError("test", errors.New("quota exceeded"))
	after := GetRetryAfter(err)
	if after < 30*time.Second {
		t.Errorf("quota exceeded should have longer delay, got %s", after)
	}
}

func TestClassifyError_Authentication(t *testing.T) {
	tests := []string{
		"401 Unauthorized",
		"invalid api key",
		"forbidden: 403",
		"permission denied",
		"API_KEY is invalid",
	}

	for _, msg := range tests {
		err := ClassifyError("test", errors.New(msg))
		if IsFatal(err) == false {
			t.Errorf("%q should be fatal", msg)
		}
		if IsRetryable(err) {
			t.Errorf("%q should not be retryable", msg)
		}
	}
}

func TestClassifyError_ModelNotFound(t *testing.T) {
	tests := []string{
		"model not found: gpt-5",
		"model_not_found",
		"not found: model xyz",
		"invalid model specified",
		"404 model not found",
	}

	for _, msg := range tests {
		err := ClassifyError("test", errors.New(msg))
		if !IsFatal(err) {
			t.Errorf("%q should be fatal", msg)
		}
	}
}

func TestClassifyError_ContextLimit(t *testing.T) {
	tests := []string{
		"token limit exceeded",
		"context length exceeded",
		"context_length_exceeded",
		"input too long for model",
	}

	for _, msg := range tests {
		err := ClassifyError("test", errors.New(msg))
		if !IsProviderError(err) {
			t.Errorf("%q should be provider error", msg)
		}
		if IsRetryable(err) {
			t.Errorf("%q should not be retryable", msg)
		}
	}
}

func TestClassifyError_ServerError(t *testing.T) {
	tests := []string{
		"500 Internal Server Error",
		"502 Bad Gateway",
		"503 Service Unavailable",
		"server is overloaded",
		"server_error",
	}

	for _, msg := range tests {
		err := ClassifyError("test", errors.New(msg))
		if !IsRetryable(err) {
			t.Errorf("%q should be retryable", msg)
		}
		if !IsTransient(err) {
			t.Errorf("%q should be transient", msg)
		}
	}
}

func TestClassifyError_Timeout(t *testing.T) {
	tests := []string{
		"connection timeout",
		"context deadline exceeded",
		"request timed out",
		"context canceled",
	}

	for _, msg := range tests {
		err := ClassifyError("test", errors.New(msg))
		if !IsTransient(err) {
			t.Errorf("%q should be transient", msg)
		}
	}
}

func TestClassifyError_Network(t *testing.T) {
	tests := []string{
		"connection refused",
		"connection reset by peer",
		"no such host",
		"network unreachable",
		"unexpected EOF",
	}

	for _, msg := range tests {
		err := ClassifyError("test", errors.New(msg))
		if !IsTransient(err) {
			t.Errorf("%q should be transient", msg)
		}
	}
}

func TestClassifyError_ContentFilter(t *testing.T) {
	tests := []string{
		"content filter triggered",
		"content_filter",
		"safety settings blocked",
		"blocked by moderation",
	}

	for _, msg := range tests {
		err := ClassifyError("test", errors.New(msg))
		if !IsProviderError(err) {
			t.Errorf("%q should be provider error", msg)
		}
	}
}

func TestClassifyError_InvalidRequest(t *testing.T) {
	tests := []string{
		"invalid request body",
		"invalid_request_error",
		"400 Bad Request",
		"malformed JSON",
	}

	for _, msg := range tests {
		err := ClassifyError("test", errors.New(msg))
		if !IsProviderError(err) {
			t.Errorf("%q should be provider error", msg)
		}
		if IsRetryable(err) {
			t.Errorf("%q should not be retryable", msg)
		}
	}
}

func TestClassifyError_Default(t *testing.T) {
	err := ClassifyError("test", errors.New("something completely unexpected"))
	if !IsProviderError(err) {
		t.Error("unknown errors should default to provider error")
	}
}

func TestClassifyError_PreservesProvider(t *testing.T) {
	err := ClassifyError("gemini", errors.New("rate limit"))
	var ae *AgentError
	if errors.As(err, &ae) {
		if ae.Provider != "gemini" {
			t.Errorf("provider = %q, want %q", ae.Provider, "gemini")
		}
	} else {
		t.Error("should be AgentError")
	}
}

// --- containsAny ---

func TestContainsAny_Match(t *testing.T) {
	if !containsAny("hello world", "world", "mars") {
		t.Error("should match 'world'")
	}
}

func TestContainsAny_NoMatch(t *testing.T) {
	if containsAny("hello world", "mars", "venus") {
		t.Error("should not match")
	}
}

func TestContainsAny_Empty(t *testing.T) {
	if containsAny("hello") {
		t.Error("no substrings should not match")
	}
}

// --- asAgentError ---

func TestAsAgentError_Direct(t *testing.T) {
	ae := NewTransientError("test", "msg", nil)
	var target *AgentError
	if !asAgentError(ae, &target) {
		t.Error("should find AgentError")
	}
}

func TestAsAgentError_NotAgentError(t *testing.T) {
	var target *AgentError
	if asAgentError(errors.New("plain"), &target) {
		t.Error("should not find AgentError in plain error")
	}
}
