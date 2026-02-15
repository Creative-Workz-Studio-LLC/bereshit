//omni:code --go -test
//omni:key B-L2-server-test
//omni:version a-01.00

// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: B-L2-server-test
// Purpose: Unit tests for HTTP helpers — JSON response, error writing, decoding
// Biblical: Proverbs 15:23 — "A word spoken in due season, how good is it!"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-15
//
// ═══════════════════════════════════════════════════════════════════════════

package server

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"encoding/json"
	"io"
	"net/http"
	"net/http/httptest"
	"strings"
	"testing"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// ───────────────────────────────────────────────────────────────────────────
// writeJSON — verify JSON serialization and headers
// ───────────────────────────────────────────────────────────────────────────

func TestWriteJSONStatus(t *testing.T) {
	w := httptest.NewRecorder()
	writeJSON(w, http.StatusOK, map[string]string{"key": "value"})

	if w.Code != http.StatusOK {
		t.Errorf("status = %d, want %d", w.Code, http.StatusOK)
	}
}

func TestWriteJSONContentType(t *testing.T) {
	w := httptest.NewRecorder()
	writeJSON(w, http.StatusOK, map[string]string{"key": "value"})

	ct := w.Header().Get("Content-Type")
	if ct != "application/json" {
		t.Errorf("Content-Type = %q, want %q", ct, "application/json")
	}
}

func TestWriteJSONBody(t *testing.T) {
	w := httptest.NewRecorder()
	writeJSON(w, http.StatusOK, map[string]string{"msg": "hello"})

	var body map[string]string
	if err := json.NewDecoder(w.Body).Decode(&body); err != nil {
		t.Fatalf("decode: %v", err)
	}
	if body["msg"] != "hello" {
		t.Errorf("body msg = %q, want %q", body["msg"], "hello")
	}
}

func TestWriteJSONCreatedStatus(t *testing.T) {
	w := httptest.NewRecorder()
	writeJSON(w, http.StatusCreated, map[string]string{"id": "123"})

	if w.Code != http.StatusCreated {
		t.Errorf("status = %d, want %d", w.Code, http.StatusCreated)
	}
}

// ───────────────────────────────────────────────────────────────────────────
// writeError — verify error JSON format
// ───────────────────────────────────────────────────────────────────────────

func TestWriteErrorStatus(t *testing.T) {
	w := httptest.NewRecorder()
	writeError(w, http.StatusNotFound, "not found")

	if w.Code != http.StatusNotFound {
		t.Errorf("status = %d, want %d", w.Code, http.StatusNotFound)
	}
}

func TestWriteErrorBody(t *testing.T) {
	w := httptest.NewRecorder()
	writeError(w, http.StatusBadRequest, "invalid input")

	var body map[string]string
	if err := json.NewDecoder(w.Body).Decode(&body); err != nil {
		t.Fatalf("decode: %v", err)
	}
	if body["error"] != "invalid input" {
		t.Errorf("error = %q, want %q", body["error"], "invalid input")
	}
}

func TestWriteErrorInternalServer(t *testing.T) {
	w := httptest.NewRecorder()
	writeError(w, http.StatusInternalServerError, "something broke")

	if w.Code != http.StatusInternalServerError {
		t.Errorf("status = %d, want 500", w.Code)
	}
}

// ───────────────────────────────────────────────────────────────────────────
// decodeJSON — verify request body parsing
// ───────────────────────────────────────────────────────────────────────────

func TestDecodeJSONSuccess(t *testing.T) {
	body := `{"name": "test", "value": 42}`
	r := httptest.NewRequest(http.MethodPost, "/", strings.NewReader(body))

	var result map[string]any
	if err := decodeJSON(r, &result); err != nil {
		t.Fatalf("decodeJSON: %v", err)
	}
	if result["name"] != "test" {
		t.Errorf("name = %v, want %q", result["name"], "test")
	}
	if result["value"] != float64(42) {
		t.Errorf("value = %v, want 42", result["value"])
	}
}

func TestDecodeJSONInvalidJSON(t *testing.T) {
	r := httptest.NewRequest(http.MethodPost, "/", strings.NewReader("{invalid"))

	var result map[string]any
	if err := decodeJSON(r, &result); err == nil {
		t.Error("decodeJSON should fail for invalid JSON")
	}
}

func TestDecodeJSONEmptyBody(t *testing.T) {
	r := httptest.NewRequest(http.MethodPost, "/", strings.NewReader(""))

	var result map[string]any
	if err := decodeJSON(r, &result); err == nil {
		t.Error("decodeJSON should fail for empty body")
	}
}

func TestDecodeJSONStruct(t *testing.T) {
	type payload struct {
		Name  string `json:"name"`
		Count int    `json:"count"`
	}

	body := `{"name": "test", "count": 5}`
	r := httptest.NewRequest(http.MethodPost, "/", strings.NewReader(body))

	var p payload
	if err := decodeJSON(r, &p); err != nil {
		t.Fatalf("decodeJSON: %v", err)
	}
	if p.Name != "test" {
		t.Errorf("Name = %q, want %q", p.Name, "test")
	}
	if p.Count != 5 {
		t.Errorf("Count = %d, want 5", p.Count)
	}
}

// ───────────────────────────────────────────────────────────────────────────
// Service struct — verify zero value is safe
// ───────────────────────────────────────────────────────────────────────────

func TestServiceZeroValue(t *testing.T) {
	// Service should be constructable without panic.
	s := Service{}
	_ = s.paths.ProjectDir // zero value access should be safe
}

// ───────────────────────────────────────────────────────────────────────────
// writeJSON — array and nested types
// ───────────────────────────────────────────────────────────────────────────

func TestWriteJSONArray(t *testing.T) {
	w := httptest.NewRecorder()
	writeJSON(w, http.StatusOK, []string{"a", "b", "c"})

	body, _ := io.ReadAll(w.Body)
	var result []string
	if err := json.Unmarshal(body, &result); err != nil {
		t.Fatalf("decode: %v", err)
	}
	if len(result) != 3 {
		t.Errorf("array len = %d, want 3", len(result))
	}
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
