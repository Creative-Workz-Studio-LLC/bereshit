package agent

import (
	"encoding/json"
	"os"
	"path/filepath"
	"testing"
)

// --- LoadProfile ---

func TestLoadProfile_Basic(t *testing.T) {
	dir := t.TempDir()
	path := filepath.Join(dir, "test.json")

	profile := AgentProfile{
		Name:     "Nathan Emet",
		Role:     "test validator",
		Identity: "You are Nathan, the test validator.",
		Provider: "gemini",
		Model:    "gemini-2.5-flash",
		Version:  "0.1.0",
	}

	data, _ := json.MarshalIndent(profile, "", "  ")
	os.WriteFile(path, data, 0o644)

	loaded, err := LoadProfile(path)
	if err != nil {
		t.Fatal(err)
	}

	if loaded.Name != "Nathan Emet" {
		t.Errorf("name = %q", loaded.Name)
	}
	if loaded.Role != "test validator" {
		t.Errorf("role = %q", loaded.Role)
	}
	if loaded.Provider != "gemini" {
		t.Errorf("provider = %q", loaded.Provider)
	}
}

func TestLoadProfile_IdentityFromFile(t *testing.T) {
	dir := t.TempDir()

	// Write identity file
	identityPath := filepath.Join(dir, "identity.md")
	os.WriteFile(identityPath, []byte("# Nathan Emet\nYou are the test validator."), 0o644)

	// Write profile referencing identity file
	profile := AgentProfile{
		Name:     "Nathan",
		Identity: "@identity.md", // relative path with @ prefix
	}
	data, _ := json.MarshalIndent(profile, "", "  ")
	profilePath := filepath.Join(dir, "config.json")
	os.WriteFile(profilePath, data, 0o644)

	loaded, err := LoadProfile(profilePath)
	if err != nil {
		t.Fatal(err)
	}

	if loaded.Identity != "# Nathan Emet\nYou are the test validator." {
		t.Errorf("identity = %q", loaded.Identity)
	}
}

func TestLoadProfile_IdentityFileAbsolute(t *testing.T) {
	dir := t.TempDir()
	identityPath := filepath.Join(dir, "bio.md")
	os.WriteFile(identityPath, []byte("absolute identity"), 0o644)

	profile := AgentProfile{
		Name:     "Test",
		Identity: "@" + identityPath,
	}
	data, _ := json.MarshalIndent(profile, "", "  ")
	profilePath := filepath.Join(dir, "config.json")
	os.WriteFile(profilePath, data, 0o644)

	loaded, err := LoadProfile(profilePath)
	if err != nil {
		t.Fatal(err)
	}
	if loaded.Identity != "absolute identity" {
		t.Errorf("identity = %q", loaded.Identity)
	}
}

func TestLoadProfile_IdentityFileMissing(t *testing.T) {
	dir := t.TempDir()
	profile := AgentProfile{
		Name:     "Test",
		Identity: "@missing.md",
	}
	data, _ := json.MarshalIndent(profile, "", "  ")
	path := filepath.Join(dir, "config.json")
	os.WriteFile(path, data, 0o644)

	_, err := LoadProfile(path)
	if err == nil {
		t.Error("expected error for missing identity file")
	}
}

func TestLoadProfile_ResolvesContextPaths(t *testing.T) {
	dir := t.TempDir()
	profile := AgentProfile{
		Name:         "Test",
		ContextFiles: []string{"docs/guide.md", "/absolute/path.md"},
	}
	data, _ := json.MarshalIndent(profile, "", "  ")
	path := filepath.Join(dir, "config.json")
	os.WriteFile(path, data, 0o644)

	loaded, err := LoadProfile(path)
	if err != nil {
		t.Fatal(err)
	}

	if loaded.ContextFiles[0] != filepath.Join(dir, "docs/guide.md") {
		t.Errorf("relative path not resolved: %q", loaded.ContextFiles[0])
	}
	if loaded.ContextFiles[1] != "/absolute/path.md" {
		t.Errorf("absolute path should be unchanged: %q", loaded.ContextFiles[1])
	}
}

func TestLoadProfile_ResolvesMemoryPath(t *testing.T) {
	dir := t.TempDir()
	profile := AgentProfile{
		Name:       "Test",
		MemoryPath: "memory.json",
	}
	data, _ := json.MarshalIndent(profile, "", "  ")
	path := filepath.Join(dir, "config.json")
	os.WriteFile(path, data, 0o644)

	loaded, err := LoadProfile(path)
	if err != nil {
		t.Fatal(err)
	}

	if loaded.MemoryPath != filepath.Join(dir, "memory.json") {
		t.Errorf("memory path not resolved: %q", loaded.MemoryPath)
	}
}

func TestLoadProfile_FileNotFound(t *testing.T) {
	_, err := LoadProfile("/nonexistent/path.json")
	if err == nil {
		t.Error("expected error for missing file")
	}
}

func TestLoadProfile_InvalidJSON(t *testing.T) {
	dir := t.TempDir()
	path := filepath.Join(dir, "bad.json")
	os.WriteFile(path, []byte("{not valid json}"), 0o644)

	_, err := LoadProfile(path)
	if err == nil {
		t.Error("expected error for invalid JSON")
	}
}

// --- SaveProfile ---

func TestSaveProfile(t *testing.T) {
	dir := t.TempDir()
	path := filepath.Join(dir, "output.json")

	profile := &AgentProfile{
		Name:     "Saved Agent",
		Role:     "test",
		Provider: "gemini",
		Tags:     []string{"test", "validation"},
	}

	err := SaveProfile(path, profile)
	if err != nil {
		t.Fatal(err)
	}

	// Verify the file exists and is valid JSON
	data, err := os.ReadFile(path)
	if err != nil {
		t.Fatal(err)
	}

	var loaded AgentProfile
	if err := json.Unmarshal(data, &loaded); err != nil {
		t.Fatalf("saved file is not valid JSON: %v", err)
	}
	if loaded.Name != "Saved Agent" {
		t.Errorf("name = %q", loaded.Name)
	}
}

func TestSaveProfile_CreatesDirectory(t *testing.T) {
	dir := t.TempDir()
	path := filepath.Join(dir, "nested", "deep", "config.json")

	err := SaveProfile(path, &AgentProfile{Name: "Test"})
	if err != nil {
		t.Fatal(err)
	}

	if _, err := os.Stat(path); os.IsNotExist(err) {
		t.Error("file should exist")
	}
}

func TestSaveProfile_RoundTrip(t *testing.T) {
	dir := t.TempDir()
	path := filepath.Join(dir, "roundtrip.json")

	original := &AgentProfile{
		Name:           "Round Trip",
		Role:           "tester",
		Identity:       "You are a test agent.",
		BiblicalAnchor: "Genesis 1:1",
		Provider:       "anthropic",
		Model:          "claude-sonnet-4-6",
		MaxIterations:  10,
		MaxTokens:      2048,
		Temperature:    0.7,
		Verbose:        true,
		Tools:          []string{"read_file", "shell"},
		MemoryPath:     "/tmp/test-memory.json",
		RetryAttempts:  3,
		TimeoutSeconds: 30,
		ContextFiles:   []string{"/tmp/context.md"},
		Version:        "0.1.0",
		Tags:           []string{"test", "round-trip"},
	}

	if err := SaveProfile(path, original); err != nil {
		t.Fatal(err)
	}

	loaded, err := LoadProfile(path)
	if err != nil {
		t.Fatal(err)
	}

	if loaded.Name != original.Name {
		t.Errorf("name mismatch")
	}
	if loaded.MaxIterations != original.MaxIterations {
		t.Errorf("max_iterations mismatch")
	}
	if loaded.Temperature != original.Temperature {
		t.Errorf("temperature mismatch")
	}
	if len(loaded.Tags) != len(original.Tags) {
		t.Errorf("tags mismatch")
	}
}

// --- Validate ---

func TestValidate_Valid(t *testing.T) {
	p := &AgentProfile{Name: "Valid"}
	if err := p.Validate(); err != nil {
		t.Errorf("unexpected error: %v", err)
	}
}

func TestValidate_MissingName(t *testing.T) {
	p := &AgentProfile{}
	if err := p.Validate(); err == nil {
		t.Error("expected error for missing name")
	}
}

// --- ResolveTools ---

func TestResolveTools_Default(t *testing.T) {
	p := &AgentProfile{Name: "Test"}
	tools := p.ResolveTools()
	if len(tools) != len(DefaultTools()) {
		t.Errorf("empty tools should return DefaultTools, got %d", len(tools))
	}
}

func TestResolveTools_None(t *testing.T) {
	p := &AgentProfile{Name: "Test", Tools: []string{"none"}}
	tools := p.ResolveTools()
	if tools != nil {
		t.Errorf("'none' should return nil, got %d tools", len(tools))
	}
}

func TestResolveTools_Specific(t *testing.T) {
	p := &AgentProfile{Name: "Test", Tools: []string{"read_file", "shell"}}
	tools := p.ResolveTools()
	if len(tools) != 2 {
		t.Fatalf("expected 2 tools, got %d", len(tools))
	}
	names := make(map[string]bool)
	for _, tool := range tools {
		names[tool.Definition().Name] = true
	}
	if !names["read_file"] || !names["shell"] {
		t.Errorf("wrong tools: %v", names)
	}
}

func TestResolveTools_UnknownIgnored(t *testing.T) {
	p := &AgentProfile{Name: "Test", Tools: []string{"read_file", "nonexistent"}}
	tools := p.ResolveTools()
	if len(tools) != 1 {
		t.Errorf("expected 1 tool (unknown ignored), got %d", len(tools))
	}
}

// --- ToSubAgentConfig ---

func TestToSubAgentConfig(t *testing.T) {
	p := &AgentProfile{
		Name:          "Nathan",
		Identity:      "You are Nathan.",
		MaxIterations: 5,
		Verbose:       true,
	}

	cfg := p.ToSubAgentConfig()
	if cfg.Name != "Nathan" {
		t.Errorf("name = %q", cfg.Name)
	}
	if cfg.Identity != "You are Nathan." {
		t.Errorf("identity = %q", cfg.Identity)
	}
	if cfg.MaxIterations != 5 {
		t.Errorf("max_iterations = %d", cfg.MaxIterations)
	}
}

func TestToSubAgentConfig_WithMemory(t *testing.T) {
	p := &AgentProfile{
		Name:       "Test",
		MemoryPath: "/tmp/test-memory.json",
	}

	cfg := p.ToSubAgentConfig()
	if cfg.Memory == nil {
		t.Error("memory should be set when MemoryPath is provided")
	}
}

func TestToSubAgentConfig_NoMemory(t *testing.T) {
	p := &AgentProfile{Name: "Test"}
	cfg := p.ToSubAgentConfig()
	if cfg.Memory != nil {
		t.Error("memory should be nil when no MemoryPath")
	}
}

// --- LoadProfileDir ---

func TestLoadProfileDir(t *testing.T) {
	dir := t.TempDir()

	// Write some profiles
	for _, name := range []string{"alpha", "beta"} {
		data, _ := json.MarshalIndent(AgentProfile{
			Name: name,
			Role: name + "-role",
		}, "", "  ")
		os.WriteFile(filepath.Join(dir, name+".json"), data, 0o644)
	}

	// Write a non-json file (should be skipped)
	os.WriteFile(filepath.Join(dir, "readme.txt"), []byte("not a profile"), 0o644)

	// Write an invalid json file (should be skipped)
	os.WriteFile(filepath.Join(dir, "bad.json"), []byte("{invalid}"), 0o644)

	profiles, err := LoadProfileDir(dir)
	if err != nil {
		t.Fatal(err)
	}

	if len(profiles) != 2 {
		t.Fatalf("expected 2 profiles, got %d", len(profiles))
	}
	if profiles["alpha"].Role != "alpha-role" {
		t.Error("alpha profile not loaded correctly")
	}
}

func TestLoadProfileDir_Empty(t *testing.T) {
	dir := t.TempDir()
	profiles, err := LoadProfileDir(dir)
	if err != nil {
		t.Fatal(err)
	}
	if len(profiles) != 0 {
		t.Errorf("expected 0 profiles, got %d", len(profiles))
	}
}

func TestLoadProfileDir_NotFound(t *testing.T) {
	_, err := LoadProfileDir("/nonexistent/dir")
	if err == nil {
		t.Error("expected error for missing directory")
	}
}

// --- RegisterProfileDir ---

func TestRegisterProfileDir(t *testing.T) {
	dir := t.TempDir()

	for _, name := range []string{"nathan", "ezra"} {
		data, _ := json.MarshalIndent(AgentProfile{
			Name:     name,
			Identity: "You are " + name,
		}, "", "  ")
		os.WriteFile(filepath.Join(dir, name+".json"), data, 0o644)
	}

	parent := New(Config{Provider: &mockProvider{}})
	family := NewFamily(parent)

	count, err := RegisterProfileDir(family, dir)
	if err != nil {
		t.Fatal(err)
	}
	if count != 2 {
		t.Errorf("expected 2 registered, got %d", count)
	}
	if !family.Has("nathan") {
		t.Error("should have nathan")
	}
	if !family.Has("ezra") {
		t.Error("should have ezra")
	}
}
