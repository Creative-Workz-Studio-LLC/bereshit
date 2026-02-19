//omni:code --go -demo-test
//omni:key B-L0-hybrid-paths-test
//omni:version b-03.00

// ============================================================================
// METADATA
// ============================================================================
//
// Key:     B-L0-hybrid-paths-test
// Purpose: Comprehensive tests for path resolution library
// Biblical: Proverbs 4:26 — "Ponder the path of thy feet"
// Authors: Nova Dawn
// Version: 3.0.0
// Created: 2026-02-18
// Updated: 2026-02-18

package paths

// ──────────────────────────────────────────────────────────────────────────
// Imports (in METADATA — Go requires all imports before any declarations)
// ──────────────────────────────────────────────────────────────────────────

import (
	"errors"
	"os"
	"path/filepath"
	"strings"
	"testing"
)

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================
//
// section-order: [s01]-imports
// (imports declared in METADATA section — Go language constraint)
// (test helper function in BODY §2 — funcs cannot live in SETUP)

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// ──────────────────────────────────────────────────────────────────────────
// 1. Org Chart
// ──────────────────────────────────────────────────────────────────────────
//
// Helpers:
//   newTestResolver
//
// Test Functions (41 tests in Cv zone):
//   Identity:          TestPragmaSliceNotEmpty, TestPragmaContainsKey,
//                      TestPragmaContainsFormat, TestPragmaGetMissingReturnsFalse,
//                      TestMetadataSliceNotEmpty, TestMetadataContainsVersion,
//                      TestMetadataContainsScripture, TestMetadataGetMissingReturnsFalse
//   Construction:      TestNewResolverReturnsNonNil,
//                      TestNewResolverWithRootsStoresExactPaths, TestResolverString
//   Bereshit:          TestBereshitRootExact, TestBereshitWordExact,
//                      TestBereshitSeedIsChildOfWord, TestBereshitOmniSeedExact
//   Claude:            TestClaudeHomeExact, TestClaudeSessionExact,
//                      TestClaudeSystemIsChildOfClaudeHome, TestClaudeSkillsExact
//   Claude Global:     TestClaudeGlobalRootExact, TestClaudeGlobalConfigExact
//   CPI-SI Schema:     TestCPISISchemaConfigExact
//   State Machine:     TestStateMachineSchemaDir, TestStateMachineRuntimeDirExact,
//                      TestStateMachineRuntimeFilesShareParent
//   Database:          TestDatabaseDirExact, TestDatabasePathExact,
//                      TestDatabasePathParentIsDatabaseDir
//   Logging:           TestLogsDirIsChildOfData, TestLogsSubdirsShareParent,
//                      TestLogsSubdirsHaveCorrectNames
//   Health:            TestHealthConfigDirExact,
//                      TestHealthRuntimeDirIsChildOfHealthConfig
//   Debug/Restore:     TestDebugRestoreFormatsAreChildrenOfData,
//                      TestRestoreCheckpointsIsChildOfRestore
//   FindConfigDir:     TestFindConfigDirDelegatesToNamed,
//                      TestFindConfigDirNamedNotFound
//   Ensure:            TestEnsureDatabaseDirCreatesData,
//                      TestEnsureLogsDirCreatesAllSubdirs,
//                      TestEnsureAllDataDirsCreatesCompleteTree
//   Package Delegation: TestPackageFunctionsReturnNonEmpty

// ──────────────────────────────────────────────────────────────────────────
// 2. Helpers
// ──────────────────────────────────────────────────────────────────────────

// newTestResolver creates a Resolver with deterministic roots for testing.
// All path assertions use these known values.
func newTestResolver() *Resolver {
	return NewResolverWithRoots("/home/test", "/bereshit", "/claude-global")
}

// ──────────────────────────────────────────────────────────────────────────
// 3. Core Operations
// ──────────────────────────────────────────────────────────────────────────
// [Reserved: Test logic lives in Cv zone — test functions ARE the operations]

// ──────────────────────────────────────────────────────────────────────────
// 4. Error Handling
// ──────────────────────────────────────────────────────────────────────────
// [Reserved: Tests validate errors from main package, no custom test errors]

// ──────────────────────────────────────────────────────────────────────────
// 5. Test Functions
// ──────────────────────────────────────────────────────────────────────────
// [Reserved: All Test* functions in Cv zone below]

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// ──────────────────────────────────────────────────────────────────────────
// Cv — Validation
// ──────────────────────────────────────────────────────────────────────────
//
// 41 tests covering:
//   - Identity: Pragma/Metadata var access, PragmaGet/MetadataGet lookups
//   - Resolver: construction, String(), exact path composition
//   - Domains: bereshit, claude, claude-global, cpisi, statemachine,
//     database, logging, health, debug/restore, formats
//   - Ensure: real filesystem creation with temp dirs
//   - Package-level: delegation to global resolver

// --- Identity Tests ---

func TestPragmaSliceNotEmpty(t *testing.T) {
	if len(Pragma) == 0 {
		t.Fatal("Pragma slice is empty")
	}
}

func TestPragmaContainsKey(t *testing.T) {
	val, ok := PragmaGet("I1.key")
	if !ok {
		t.Fatal("PragmaGet(I1.key) not found")
	}
	if val != "B-L0-hybrid-paths" {
		t.Errorf("I1.key = %q, want %q", val, "B-L0-hybrid-paths")
	}
}

func TestPragmaContainsFormat(t *testing.T) {
	val, ok := PragmaGet("I1.format")
	if !ok {
		t.Fatal("PragmaGet(I1.format) not found")
	}
	if val != "go" {
		t.Errorf("I1.format = %q, want %q", val, "go")
	}
}

func TestPragmaGetMissingReturnsFalse(t *testing.T) {
	_, ok := PragmaGet("nonexistent.key")
	if ok {
		t.Error("PragmaGet(nonexistent.key) should return false")
	}
}

func TestMetadataSliceNotEmpty(t *testing.T) {
	if len(Metadata) == 0 {
		t.Fatal("Metadata slice is empty")
	}
}

func TestMetadataContainsVersion(t *testing.T) {
	val, ok := MetadataGet("C1.version")
	if !ok {
		t.Fatal("MetadataGet(C1.version) not found")
	}
	if val != "b-03.00" {
		t.Errorf("C1.version = %q, want %q", val, "b-03.00")
	}
}

func TestMetadataContainsScripture(t *testing.T) {
	val, ok := MetadataGet("C3.scripture")
	if !ok {
		t.Fatal("MetadataGet(C3.scripture) not found")
	}
	if !strings.Contains(val, "Proverbs 4:26") {
		t.Errorf("C3.scripture should contain 'Proverbs 4:26', got %q", val)
	}
}

func TestMetadataGetMissingReturnsFalse(t *testing.T) {
	_, ok := MetadataGet("nonexistent.key")
	if ok {
		t.Error("MetadataGet(nonexistent.key) should return false")
	}
}

// --- Resolver Construction Tests ---

func TestNewResolverReturnsNonNil(t *testing.T) {
	r := NewResolver()
	if r == nil {
		t.Fatal("NewResolver() returned nil")
	}
}

func TestNewResolverWithRootsStoresExactPaths(t *testing.T) {
	r := NewResolverWithRoots("/test/home", "/test/bereshit", "/test/claude")
	if r.home != "/test/home" {
		t.Errorf("home = %q, want %q", r.home, "/test/home")
	}
	if r.bereshit != "/test/bereshit" {
		t.Errorf("bereshit = %q, want %q", r.bereshit, "/test/bereshit")
	}
	if r.claudeGlobal != "/test/claude" {
		t.Errorf("claudeGlobal = %q, want %q", r.claudeGlobal, "/test/claude")
	}
}

func TestResolverString(t *testing.T) {
	r := NewResolverWithRoots("/h", "/b", "/c")
	s := r.String()
	if !strings.Contains(s, "Resolver{") {
		t.Errorf("String() should start with Resolver{, got %q", s)
	}
	if !strings.Contains(s, "/h") || !strings.Contains(s, "/b") || !strings.Contains(s, "/c") {
		t.Errorf("String() should contain all roots, got %q", s)
	}
}

// --- Bereshit Path Tests ---

func TestBereshitRootExact(t *testing.T) {
	r := newTestResolver()
	if got := r.BereshitRoot(); got != "/bereshit" {
		t.Errorf("BereshitRoot() = %q, want %q", got, "/bereshit")
	}
}

func TestBereshitWordExact(t *testing.T) {
	r := newTestResolver()
	want := filepath.Join("/bereshit", "word")
	if got := r.BereshitWord(); got != want {
		t.Errorf("BereshitWord() = %q, want %q", got, want)
	}
}

func TestBereshitSeedIsChildOfWord(t *testing.T) {
	r := newTestResolver()
	word := r.BereshitWord()
	seed := r.BereshitSeed()
	if !strings.HasPrefix(seed, word+string(filepath.Separator)) {
		t.Errorf("BereshitSeed() %q is not child of BereshitWord() %q", seed, word)
	}
}

func TestBereshitOmniSeedExact(t *testing.T) {
	r := newTestResolver()
	want := filepath.Join("/bereshit", "word", "omni", "seed")
	if got := r.BereshitOmniSeed(); got != want {
		t.Errorf("BereshitOmniSeed() = %q, want %q", got, want)
	}
}

// --- Claude Path Tests ---

func TestClaudeHomeExact(t *testing.T) {
	r := newTestResolver()
	want := filepath.Join("/home/test", ".claude")
	if got := r.ClaudeHome(); got != want {
		t.Errorf("ClaudeHome() = %q, want %q", got, want)
	}
}

func TestClaudeSessionExact(t *testing.T) {
	r := newTestResolver()
	want := filepath.Join("/home/test", ".claude", "session")
	if got := r.ClaudeSession(); got != want {
		t.Errorf("ClaudeSession() = %q, want %q", got, want)
	}
}

func TestClaudeSystemIsChildOfClaudeHome(t *testing.T) {
	r := newTestResolver()
	home := r.ClaudeHome()
	system := r.ClaudeSystem()
	if !strings.HasPrefix(system, home+string(filepath.Separator)) {
		t.Errorf("ClaudeSystem() %q is not child of ClaudeHome() %q", system, home)
	}
}

func TestClaudeSkillsExact(t *testing.T) {
	r := newTestResolver()
	want := filepath.Join("/home/test", ".claude", "cpi-si", "skills")
	if got := r.ClaudeSkills(); got != want {
		t.Errorf("ClaudeSkills() = %q, want %q", got, want)
	}
}

// --- Claude Global Path Tests ---

func TestClaudeGlobalRootExact(t *testing.T) {
	r := newTestResolver()
	if got := r.ClaudeGlobalRoot(); got != "/claude-global" {
		t.Errorf("ClaudeGlobalRoot() = %q, want %q", got, "/claude-global")
	}
}

func TestClaudeGlobalConfigExact(t *testing.T) {
	r := newTestResolver()
	want := filepath.Join("/claude-global", "config")
	if got := r.ClaudeGlobalConfig(); got != want {
		t.Errorf("ClaudeGlobalConfig() = %q, want %q", got, want)
	}
}

// --- CPI-SI Schema Path Tests ---

func TestCPISISchemaConfigExact(t *testing.T) {
	r := newTestResolver()
	want := filepath.Join("/claude-global", "pkg", "cpisi", "schema", "config")
	if got := r.CPISISchemaConfig(); got != want {
		t.Errorf("CPISISchemaConfig() = %q, want %q", got, want)
	}
}

// --- State Machine Path Tests ---

func TestStateMachineSchemaDir(t *testing.T) {
	r := newTestResolver()
	want := filepath.Join("/claude-global", "pkg", "cpisi", "schema", "config", "l2")
	if got := r.StateMachineSchemaDir(); got != want {
		t.Errorf("StateMachineSchemaDir() = %q, want %q", got, want)
	}
}

func TestStateMachineRuntimeDirExact(t *testing.T) {
	r := newTestResolver()
	want := filepath.Join("/claude-global", "config", "statemachine", "runtime")
	if got := r.StateMachineRuntimeDir(); got != want {
		t.Errorf("StateMachineRuntimeDir() = %q, want %q", got, want)
	}
}

func TestStateMachineRuntimeFilesShareParent(t *testing.T) {
	r := newTestResolver()
	runtimeDir := r.StateMachineRuntimeDir()

	files := []struct {
		name string
		path string
	}{
		{"state", r.StateMachineRuntimeState()},
		{"path", r.StateMachineRuntimePath()},
		{"history", r.StateMachineRuntimeHistory()},
		{"choice_history", r.StateMachineChoiceHistory()},
	}

	for _, f := range files {
		parent := filepath.Dir(f.path)
		if parent != runtimeDir {
			t.Errorf("%s parent = %q, want %q", f.name, parent, runtimeDir)
		}
	}
}

// --- Database Path Tests ---

func TestDatabaseDirExact(t *testing.T) {
	r := newTestResolver()
	want := filepath.Join("/claude-global", "data")
	if got := r.DatabaseDir(); got != want {
		t.Errorf("DatabaseDir() = %q, want %q", got, want)
	}
}

func TestDatabasePathExact(t *testing.T) {
	r := newTestResolver()
	want := filepath.Join("/claude-global", "data", "cpisi.db")
	if got := r.DatabasePath(); got != want {
		t.Errorf("DatabasePath() = %q, want %q", got, want)
	}
}

func TestDatabasePathParentIsDatabaseDir(t *testing.T) {
	r := newTestResolver()
	parent := filepath.Dir(r.DatabasePath())
	if parent != r.DatabaseDir() {
		t.Errorf("DatabasePath parent = %q, want %q", parent, r.DatabaseDir())
	}
}

// --- Logging Path Tests ---

func TestLogsDirIsChildOfData(t *testing.T) {
	r := newTestResolver()
	dataDir := r.DatabaseDir()
	logsDir := r.LogsDir()
	if !strings.HasPrefix(logsDir, dataDir+string(filepath.Separator)) {
		t.Errorf("LogsDir() %q is not child of DatabaseDir() %q", logsDir, dataDir)
	}
}

func TestLogsSubdirsShareParent(t *testing.T) {
	r := newTestResolver()
	logsDir := r.LogsDir()

	subdirs := []string{
		r.LogsSessionDir(),
		r.LogsTrajectoryDir(),
		r.LogsCognitionDir(),
		r.LogsHealthDir(),
		r.LogsToolsDir(),
		r.LogsContextDir(),
	}

	for _, dir := range subdirs {
		parent := filepath.Dir(dir)
		if parent != logsDir {
			t.Errorf("subdir %q parent = %q, want %q", dir, parent, logsDir)
		}
	}
}

func TestLogsSubdirsHaveCorrectNames(t *testing.T) {
	r := newTestResolver()

	cases := []struct {
		name string
		path string
	}{
		{"session", r.LogsSessionDir()},
		{"trajectory", r.LogsTrajectoryDir()},
		{"cognition", r.LogsCognitionDir()},
		{"health", r.LogsHealthDir()},
		{"tools", r.LogsToolsDir()},
		{"context", r.LogsContextDir()},
	}

	for _, c := range cases {
		base := filepath.Base(c.path)
		if base != c.name {
			t.Errorf("%s dir base = %q, want %q", c.name, base, c.name)
		}
	}
}

// --- Health Path Tests ---

func TestHealthConfigDirExact(t *testing.T) {
	r := newTestResolver()
	want := filepath.Join("/claude-global", "config", "health")
	if got := r.HealthConfigDir(); got != want {
		t.Errorf("HealthConfigDir() = %q, want %q", got, want)
	}
}

func TestHealthRuntimeDirIsChildOfHealthConfig(t *testing.T) {
	r := newTestResolver()
	config := r.HealthConfigDir()
	runtime := r.HealthRuntimeDir()
	if !strings.HasPrefix(runtime, config+string(filepath.Separator)) {
		t.Errorf("HealthRuntimeDir() %q is not child of HealthConfigDir() %q", runtime, config)
	}
}

// --- Debug, Restore, Formats Tests ---

func TestDebugRestoreFormatsAreChildrenOfData(t *testing.T) {
	r := newTestResolver()
	dataDir := r.DatabaseDir()

	dirs := map[string]string{
		"debug":   r.DebugDir(),
		"restore": r.RestoreDir(),
		"formats": r.FormatsDir(),
	}

	for name, dir := range dirs {
		if !strings.HasPrefix(dir, dataDir+string(filepath.Separator)) {
			t.Errorf("%s dir %q is not child of DatabaseDir() %q", name, dir, dataDir)
		}
	}
}

func TestRestoreCheckpointsIsChildOfRestore(t *testing.T) {
	r := newTestResolver()
	restore := r.RestoreDir()
	checkpoints := r.RestoreCheckpointsDir()
	if !strings.HasPrefix(checkpoints, restore+string(filepath.Separator)) {
		t.Errorf("RestoreCheckpointsDir() %q is not child of RestoreDir() %q",
			checkpoints, restore)
	}
}

// --- FindConfigDir Tests ---

func TestFindConfigDirDelegatesToNamed(t *testing.T) {
	// Both should produce the same error for a non-existent directory
	_, err1 := FindConfigDir()
	_, err2 := FindConfigDirNamed("config")

	// Both should either find or not find — they search the same name
	if (err1 == nil) != (err2 == nil) {
		t.Errorf("FindConfigDir and FindConfigDirNamed('config') should agree: err1=%v, err2=%v",
			err1, err2)
	}
}

func TestFindConfigDirNamedNotFound(t *testing.T) {
	_, err := FindConfigDirNamed("definitely-nonexistent-dir-xyzzy")
	if err == nil {
		t.Error("FindConfigDirNamed should return error for nonexistent dir")
	}
	if !errors.Is(err, os.ErrNotExist) {
		t.Errorf("error should be os.ErrNotExist, got %v", err)
	}
}

// --- Ensure* Tests (real filesystem with temp dirs) ---

func TestEnsureDatabaseDirCreatesData(t *testing.T) {
	tmp := t.TempDir()
	r := NewResolverWithRoots(tmp, tmp, filepath.Join(tmp, "claude-global"))

	if err := r.EnsureDatabaseDir(); err != nil {
		t.Fatalf("EnsureDatabaseDir() error: %v", err)
	}

	dataDir := r.DatabaseDir()
	if _, err := os.Stat(dataDir); os.IsNotExist(err) {
		t.Errorf("DatabaseDir %q not created", dataDir)
	}
}

func TestEnsureLogsDirCreatesAllSubdirs(t *testing.T) {
	tmp := t.TempDir()
	r := NewResolverWithRoots(tmp, tmp, filepath.Join(tmp, "claude-global"))

	if err := r.EnsureLogsDir(); err != nil {
		t.Fatalf("EnsureLogsDir() error: %v", err)
	}

	dirs := []string{
		r.LogsDir(),
		r.LogsSessionDir(),
		r.LogsTrajectoryDir(),
		r.LogsCognitionDir(),
		r.LogsHealthDir(),
		r.LogsToolsDir(),
		r.LogsContextDir(),
	}

	for _, dir := range dirs {
		if _, err := os.Stat(dir); os.IsNotExist(err) {
			t.Errorf("directory %q not created", dir)
		}
	}
}

func TestEnsureAllDataDirsCreatesCompleteTree(t *testing.T) {
	tmp := t.TempDir()
	r := NewResolverWithRoots(tmp, tmp, filepath.Join(tmp, "claude-global"))

	if err := r.EnsureAllDataDirs(); err != nil {
		t.Fatalf("EnsureAllDataDirs() error: %v", err)
	}

	// Verify all leaf directories exist
	dirs := []string{
		r.DatabaseDir(),
		r.LogsDir(),
		r.LogsSessionDir(),
		r.LogsTrajectoryDir(),
		r.LogsCognitionDir(),
		r.LogsHealthDir(),
		r.LogsToolsDir(),
		r.LogsContextDir(),
		r.DebugDir(),
		r.RestoreDir(),
		r.RestoreCheckpointsDir(),
		r.FormatsDir(),
	}

	for _, dir := range dirs {
		if _, err := os.Stat(dir); os.IsNotExist(err) {
			t.Errorf("directory %q not created by EnsureAllDataDirs", dir)
		}
	}
}

// --- Package-Level Function Delegation ---

func TestPackageFunctionsReturnNonEmpty(t *testing.T) {
	// Package-level functions delegate to globalResolver() which uses
	// NewResolver(). On this workstation, all should return non-empty paths.
	fns := map[string]func() string{
		"BereshitRoot":             BereshitRoot,
		"BereshitWord":             BereshitWord,
		"ClaudeHome":               ClaudeHome,
		"ClaudeGlobalRoot":         ClaudeGlobalRoot,
		"DatabaseDir":              DatabaseDir,
		"DatabasePath":             DatabasePath,
		"LogsDir":                  LogsDir,
		"StateMachineRuntimeDir":   StateMachineRuntimeDir,
		"StateMachineRuntimeState": StateMachineRuntimeState,
	}

	for name, fn := range fns {
		if got := fn(); got == "" {
			t.Errorf("%s() returned empty string", name)
		}
	}
}

//
// ──────────────────────────────────────────────────────────────────────────
// Ce — Execution
// ──────────────────────────────────────────────────────────────────────────
//
// go test -v ./L0-universal/c-hybrid/core/paths/go/
// go test -run TestBereshit ./L0-universal/c-hybrid/core/paths/go/
//
// ──────────────────────────────────────────────────────────────────────────
// Cc — Cleanup
// ──────────────────────────────────────────────────────────────────────────
//
// t.TempDir() auto-cleans temp directories. No manual cleanup.
//
// ──────────────────────────────────────────────────────────────────────────
// X1: Policy
// ──────────────────────────────────────────────────────────────────────────
//
// Never:
//   - Skip identity tests (Pragma/Metadata validation is structural)
//   - Use hardcoded absolute paths (use newTestResolver with known roots)
//   - Test env var overrides without proper isolation
//
// Careful:
//   - Adding tests that depend on workstation layout
//   - Modifying newTestResolver roots (many tests depend on exact values)
//
// Safe:
//   - Adding new path composition tests
//   - Adding new Ensure* tests with t.TempDir()
//   - Expanding package-level delegation coverage
//
// ──────────────────────────────────────────────────────────────────────────
// X5: Note
// ──────────────────────────────────────────────────────────────────────────
//
// Note: Test suite for paths package — verifies path composition structure,
// resolver construction, directory creation, and identity registration.
// All path assertions use deterministic roots via NewResolverWithRoots.
//
// Scripture: Proverbs 4:26 — "Ponder the path of thy feet, and let all
// thy ways be established."
//
// ============================================================================
// END CLOSING
// ============================================================================
