// ============================================================================
// METADATA - CPI-SI Memory Tests
// ============================================================================
//
// Key: pkg-agent-memory-test
// Purpose: Verify persistent memory — set, get, search, persistence, categories.
// Biblical: Deuteronomy 6:6-7 — "These words shall be in thine heart"
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
	"os"
	"path/filepath"
	"strings"
	"testing"
)

// ============================================================================
// BODY
// ============================================================================

// --- Core Operations ---

func TestMemory_SetGet(t *testing.T) {
	m := NewMemory("")

	m.Set("facts", "language", "Go is preferred")
	val, ok := m.Get("facts", "language")
	if !ok {
		t.Fatal("expected to find entry")
	}
	if val != "Go is preferred" {
		t.Errorf("value = %q", val)
	}
}

func TestMemory_Get_NotFound(t *testing.T) {
	m := NewMemory("")
	_, ok := m.Get("facts", "missing")
	if ok {
		t.Error("should not find missing key")
	}
}

func TestMemory_SetUpdate(t *testing.T) {
	m := NewMemory("")
	m.Set("facts", "key", "value1")
	m.Set("facts", "key", "value2")

	val, _ := m.Get("facts", "key")
	if val != "value2" {
		t.Errorf("updated value = %q, want 'value2'", val)
	}
	if m.Size() != 1 {
		t.Errorf("Size = %d, want 1 (update, not duplicate)", m.Size())
	}
}

func TestMemory_Delete(t *testing.T) {
	m := NewMemory("")
	m.Set("facts", "key", "value")

	if !m.Delete("facts", "key") {
		t.Error("Delete should return true for existing key")
	}
	if m.Delete("facts", "key") {
		t.Error("Delete should return false for already-deleted key")
	}
	if m.Size() != 0 {
		t.Errorf("Size = %d after delete", m.Size())
	}
}

func TestMemory_Has(t *testing.T) {
	m := NewMemory("")
	m.Set("facts", "key", "value")

	if !m.Has("facts", "key") {
		t.Error("Has should return true for existing key")
	}
	if m.Has("facts", "missing") {
		t.Error("Has should return false for missing key")
	}
}

func TestMemory_Get_IncrementsAccessCount(t *testing.T) {
	m := NewMemory("")
	m.Set("facts", "key", "value")

	m.Get("facts", "key")
	m.Get("facts", "key")
	m.Get("facts", "key")

	entries := m.List("facts")
	if len(entries) != 1 {
		t.Fatalf("expected 1 entry, got %d", len(entries))
	}
	if entries[0].AccessCount != 3 {
		t.Errorf("AccessCount = %d, want 3", entries[0].AccessCount)
	}
}

// --- Category Operations ---

func TestMemory_List(t *testing.T) {
	m := NewMemory("")
	m.Set("facts", "b", "2")
	m.Set("facts", "a", "1")
	m.Set("prefs", "x", "y")

	entries := m.List("facts")
	if len(entries) != 2 {
		t.Fatalf("List(facts) = %d entries, want 2", len(entries))
	}
	// Should be sorted by key
	if entries[0].Key != "a" || entries[1].Key != "b" {
		t.Errorf("entries not sorted: %s, %s", entries[0].Key, entries[1].Key)
	}
}

func TestMemory_List_EmptyCategory(t *testing.T) {
	m := NewMemory("")
	entries := m.List("empty")
	if len(entries) != 0 {
		t.Errorf("expected 0 entries, got %d", len(entries))
	}
}

func TestMemory_Categories(t *testing.T) {
	m := NewMemory("")
	m.Set("facts", "k1", "v1")
	m.Set("prefs", "k2", "v2")
	m.Set("patterns", "k3", "v3")

	cats := m.Categories()
	if len(cats) != 3 {
		t.Fatalf("Categories = %v, want 3", cats)
	}
	// Should be sorted
	if cats[0] != "facts" || cats[1] != "patterns" || cats[2] != "prefs" {
		t.Errorf("Categories = %v, want [facts, patterns, prefs]", cats)
	}
}

func TestMemory_Size(t *testing.T) {
	m := NewMemory("")
	if m.Size() != 0 {
		t.Errorf("empty Size = %d", m.Size())
	}
	m.Set("a", "1", "v")
	m.Set("b", "2", "v")
	if m.Size() != 2 {
		t.Errorf("Size = %d, want 2", m.Size())
	}
}

// --- Search ---

func TestMemory_Search(t *testing.T) {
	m := NewMemory("")
	m.Set("facts", "go", "Go is a compiled language")
	m.Set("facts", "rust", "Rust is a systems language")
	m.Set("prefs", "editor", "Use Go for services")

	results := m.Search("Go")
	if len(results) != 2 {
		t.Fatalf("Search('Go') = %d results, want 2", len(results))
	}
}

func TestMemory_Search_CaseInsensitive(t *testing.T) {
	m := NewMemory("")
	m.Set("facts", "key", "Go is GREAT")

	results := m.Search("great")
	if len(results) != 1 {
		t.Fatalf("case-insensitive search = %d results, want 1", len(results))
	}
}

func TestMemory_Search_ByKey(t *testing.T) {
	m := NewMemory("")
	m.Set("facts", "golang", "A programming language")

	results := m.Search("golang")
	if len(results) != 1 {
		t.Fatalf("search by key = %d results, want 1", len(results))
	}
}

func TestMemory_Search_NoResults(t *testing.T) {
	m := NewMemory("")
	m.Set("facts", "key", "value")

	results := m.Search("nonexistent")
	if len(results) != 0 {
		t.Errorf("expected 0 results, got %d", len(results))
	}
}

func TestMemory_Search_OrderByAccessCount(t *testing.T) {
	m := NewMemory("")
	m.Set("facts", "less-used", "Go info")
	m.Set("facts", "more-used", "Go info too")

	// Access the second one more
	m.Get("facts", "more-used")
	m.Get("facts", "more-used")
	m.Get("facts", "more-used")
	m.Get("facts", "less-used")

	results := m.Search("Go")
	if len(results) < 2 {
		t.Fatalf("got %d results", len(results))
	}
	if results[0].Key != "more-used" {
		t.Errorf("most accessed should come first, got %q", results[0].Key)
	}
}

// --- Persistence ---

func TestMemory_SaveLoadRoundTrip(t *testing.T) {
	dir := t.TempDir()
	path := filepath.Join(dir, "memory.json")

	// Save — include two entries in the same category to exercise sort tie-breaker
	m1 := NewMemory(path)
	m1.Set("facts", "language", "Go")
	m1.Set("facts", "database", "PostgreSQL")
	m1.Set("prefs", "theme", "dark")
	if err := m1.SaveToDisk(); err != nil {
		t.Fatal(err)
	}

	// Load into fresh instance
	m2 := NewMemory(path)
	if err := m2.LoadFromDisk(); err != nil {
		t.Fatal(err)
	}

	val, ok := m2.Get("facts", "language")
	if !ok || val != "Go" {
		t.Errorf("after load: language = %q, ok = %v", val, ok)
	}
	val, ok = m2.Get("prefs", "theme")
	if !ok || val != "dark" {
		t.Errorf("after load: theme = %q, ok = %v", val, ok)
	}
}

func TestMemory_LoadFromDisk_FileNotExists(t *testing.T) {
	m := NewMemory("/tmp/nonexistent-memory-" + t.Name() + ".json")
	if err := m.LoadFromDisk(); err != nil {
		t.Errorf("missing file should not error: %v", err)
	}
	if m.Size() != 0 {
		t.Errorf("Size = %d, want 0 after loading nonexistent", m.Size())
	}
}

func TestMemory_LoadFromDisk_InvalidJSON(t *testing.T) {
	dir := t.TempDir()
	path := filepath.Join(dir, "bad.json")
	os.WriteFile(path, []byte("not json"), 0o644)

	m := NewMemory(path)
	if err := m.LoadFromDisk(); err == nil {
		t.Error("should error on invalid JSON")
	}
}

func TestMemory_LoadFromDisk_EmptyPath(t *testing.T) {
	m := NewMemory("")
	if err := m.LoadFromDisk(); err != nil {
		t.Errorf("empty path should be no-op: %v", err)
	}
}

func TestMemory_SaveToDisk_EmptyPath(t *testing.T) {
	m := NewMemory("")
	m.Set("facts", "key", "value")
	if err := m.SaveToDisk(); err != nil {
		t.Errorf("empty path should be no-op: %v", err)
	}
}

func TestMemory_SaveToDisk_NoDirtyNoWrite(t *testing.T) {
	dir := t.TempDir()
	path := filepath.Join(dir, "memory.json")

	m := NewMemory(path)
	// No Set calls — dirty = false
	if err := m.SaveToDisk(); err != nil {
		t.Fatal(err)
	}
	// File should not exist (nothing to write)
	if _, err := os.Stat(path); !os.IsNotExist(err) {
		t.Error("file should not be created when not dirty")
	}
}

func TestMemory_SaveToDisk_CreatesDirectory(t *testing.T) {
	dir := t.TempDir()
	path := filepath.Join(dir, "sub", "deep", "memory.json")

	m := NewMemory(path)
	m.Set("facts", "key", "value")
	if err := m.SaveToDisk(); err != nil {
		t.Fatal(err)
	}

	// Verify file exists
	if _, err := os.Stat(path); err != nil {
		t.Errorf("file should exist: %v", err)
	}
}

func TestMemory_DirtyFlag(t *testing.T) {
	dir := t.TempDir()
	path := filepath.Join(dir, "memory.json")

	m := NewMemory(path)
	snap := m.Snapshot()
	if snap.Dirty {
		t.Error("should not be dirty initially")
	}

	m.Set("facts", "key", "value")
	snap = m.Snapshot()
	if !snap.Dirty {
		t.Error("should be dirty after Set")
	}

	m.SaveToDisk()
	snap = m.Snapshot()
	if snap.Dirty {
		t.Error("should not be dirty after save")
	}

	m.Delete("facts", "key")
	snap = m.Snapshot()
	if !snap.Dirty {
		t.Error("should be dirty after Delete")
	}
}

func TestMemory_SaveToDisk_WriteError(t *testing.T) {
	dir := t.TempDir()
	// Make the directory read-only so WriteFile fails
	path := filepath.Join(dir, "sub", "memory.json")
	m := NewMemory(path)
	m.Set("facts", "key", "value")

	// Make the parent dir, then make it read-only
	os.MkdirAll(filepath.Join(dir, "sub"), 0o755)
	os.Chmod(filepath.Join(dir, "sub"), 0o555)
	defer os.Chmod(filepath.Join(dir, "sub"), 0o755)

	err := m.SaveToDisk()
	if err == nil {
		t.Error("expected write error")
	}
}

func TestMemory_SaveToDisk_MkdirError(t *testing.T) {
	// Path under /proc — mkdir will fail
	m := NewMemory("/proc/fakedir/memory.json")
	m.Set("facts", "key", "value")

	err := m.SaveToDisk()
	if err == nil {
		t.Error("expected mkdir error")
	}
}

func TestMemory_SaveToDisk_RenameError(t *testing.T) {
	dir := t.TempDir()
	target := filepath.Join(dir, "memory.json")

	// Pre-create a non-empty directory at the target path
	os.Mkdir(target, 0o755)
	os.WriteFile(filepath.Join(target, "blocker.txt"), []byte("block"), 0o644)

	m := NewMemory(target)
	m.Set("facts", "key", "value")

	err := m.SaveToDisk()
	if err == nil {
		t.Error("expected rename error")
	}
}

func TestMemory_LoadFromDisk_ReadError(t *testing.T) {
	dir := t.TempDir()
	path := filepath.Join(dir, "memory.json")
	// Create the file but make it unreadable
	os.WriteFile(path, []byte("[]"), 0o644)
	os.Chmod(path, 0o000)
	defer os.Chmod(path, 0o644)

	m := NewMemory(path)
	err := m.LoadFromDisk()
	if err == nil {
		t.Error("expected read error")
	}
}

// --- Snapshot ---

func TestMemory_Snapshot(t *testing.T) {
	m := NewMemory("/tmp/test.json")
	m.Set("facts", "k1", "v1")
	m.Set("prefs", "k2", "v2")

	snap := m.Snapshot()
	if snap.TotalEntries != 2 {
		t.Errorf("TotalEntries = %d", snap.TotalEntries)
	}
	if snap.Path != "/tmp/test.json" {
		t.Errorf("Path = %q", snap.Path)
	}
	if len(snap.Categories) != 2 {
		t.Errorf("Categories = %v", snap.Categories)
	}
}

// --- Prompt Summary ---

func TestMemory_BuildPromptSummary(t *testing.T) {
	m := NewMemory("")
	m.Set("facts", "go", "Go is preferred")
	m.Set("facts", "rust", "Rust for performance")
	m.Set("prefs", "style", "Direct, no fluff")

	summary := m.BuildPromptSummary("facts", "prefs")
	if !strings.Contains(summary, "--- facts ---") {
		t.Errorf("should contain facts header: %s", summary)
	}
	if !strings.Contains(summary, "Go is preferred") {
		t.Errorf("should contain fact value: %s", summary)
	}
	if !strings.Contains(summary, "--- prefs ---") {
		t.Errorf("should contain prefs header: %s", summary)
	}
}

func TestMemory_BuildPromptSummary_EmptyCategory(t *testing.T) {
	m := NewMemory("")
	summary := m.BuildPromptSummary("empty")
	if summary != "" {
		t.Errorf("empty category summary should be empty, got: %q", summary)
	}
}

func TestMemory_BuildPromptSummary_MultipleCategories(t *testing.T) {
	m := NewMemory("")
	m.Set("a", "k", "v")
	m.Set("c", "k", "v")

	// Request categories in specific order
	summary := m.BuildPromptSummary("c", "a")
	cIdx := strings.Index(summary, "--- c ---")
	aIdx := strings.Index(summary, "--- a ---")
	if cIdx > aIdx {
		t.Error("categories should appear in requested order")
	}
}

// --- Contains Helper ---

func TestContains_CaseInsensitive(t *testing.T) {
	if !contains("Hello World", "hello") {
		t.Error("should match case-insensitively")
	}
	if !contains("UPPERCASE", "upper") {
		t.Error("should match mixed case")
	}
	if contains("Hello", "xyz") {
		t.Error("should not match non-existent")
	}
	if !contains("anything", "") {
		t.Error("empty substr should always match")
	}
}

func TestBytesContains(t *testing.T) {
	if !bytesContains([]byte("hello world"), []byte("world")) {
		t.Error("should find substring")
	}
	if bytesContains([]byte("hi"), []byte("hello")) {
		t.Error("should not find longer substring in shorter string")
	}
	if !bytesContains([]byte("test"), []byte("")) {
		t.Error("empty should always match")
	}
}

// ============================================================================
// CLOSING
// ============================================================================
// "These words shall be in thine heart." — Deuteronomy 6:6-7
