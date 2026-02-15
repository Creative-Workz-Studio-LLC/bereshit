//omni:code --go -test
//omni:key B-L2-platform-test
//omni:version a-01.00

// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: B-L2-platform-test
// Purpose: Integration tests for L2 platform database loader
// Biblical: Psalm 127:1 — "Except the LORD build the house"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-15
//
// These are integration tests — they require the real platform.db.
// The database path is resolved relative to the module root (b-word/).
// Tests are skipped if the database is not found.
//
// ═══════════════════════════════════════════════════════════════════════════

package platform

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"os"
	"path/filepath"
	"runtime"
	"testing"
)

// dbDir returns the platform database directory, resolved from this test
// file's location. The path walks up from hybrid/platform/ to b-word/,
// then down to data/platform/database/.
func dbDir(t *testing.T) string {
	t.Helper()

	_, thisFile, _, ok := runtime.Caller(0)
	if !ok {
		t.Skip("cannot determine test file path")
	}

	// Walk up: platform/ -> hybrid/ -> L2-platform/ -> b-word/
	bWord := filepath.Join(filepath.Dir(thisFile), "..", "..", "..")
	dir := filepath.Join(bWord, DatabaseDir)

	dbPath := filepath.Join(dir, DatabaseFile)
	if _, err := os.Stat(dbPath); err != nil {
		t.Skipf("platform.db not found at %s — run seed-platform first", dbPath)
	}

	return dir
}

func openTestDB(t *testing.T) *DB {
	t.Helper()
	db, err := Open(dbDir(t))
	if err != nil {
		t.Fatalf("Open: %v", err)
	}
	t.Cleanup(func() { db.Close() })
	return db
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// ───────────────────────────────────────────────────────────────────────────
// Open/Close — verify database connection lifecycle
// ───────────────────────────────────────────────────────────────────────────

func TestOpenSuccess(t *testing.T) {
	db := openTestDB(t)
	if db == nil {
		t.Fatal("db should not be nil")
	}
}

func TestOpenBadPath(t *testing.T) {
	_, err := Open("/tmp/nonexistent-platform-db-99999")
	if err == nil {
		t.Error("Open with bad path should return error")
	}
}

func TestOpenFromRoot(t *testing.T) {
	_, thisFile, _, ok := runtime.Caller(0)
	if !ok {
		t.Skip("cannot determine test file path")
	}
	bWord := filepath.Join(filepath.Dir(thisFile), "..", "..", "..")

	db, err := OpenFromRoot(bWord)
	if err != nil {
		t.Skipf("OpenFromRoot: %v — run seed-platform first", err)
	}
	defer db.Close()

	// Verify it's a real working connection.
	count, err := db.SpecCount()
	if err != nil {
		t.Fatalf("SpecCount after OpenFromRoot: %v", err)
	}
	if count == 0 {
		t.Error("SpecCount should be > 0")
	}
}

// ───────────────────────────────────────────────────────────────────────────
// Spec Queries — verify seeded data
// ───────────────────────────────────────────────────────────────────────────

func TestSpecCount(t *testing.T) {
	db := openTestDB(t)
	count, err := db.SpecCount()
	if err != nil {
		t.Fatalf("SpecCount: %v", err)
	}
	if count == 0 {
		t.Error("SpecCount should be > 0")
	}
	t.Logf("specs: %d", count)
}

func TestAllSpecs(t *testing.T) {
	db := openTestDB(t)
	specs, err := db.AllSpecs()
	if err != nil {
		t.Fatalf("AllSpecs: %v", err)
	}
	if len(specs) == 0 {
		t.Error("AllSpecs should return specs")
	}

	// Verify each spec has required fields.
	for _, s := range specs {
		if s.Key == "" {
			t.Errorf("spec id=%d has empty key", s.ID)
		}
		if s.File == "" {
			t.Errorf("spec %s has empty file", s.Key)
		}
	}
}

func TestSpecByKeyFound(t *testing.T) {
	db := openTestDB(t)

	// Get any spec to test SpecByKey.
	specs, err := db.AllSpecs()
	if err != nil || len(specs) == 0 {
		t.Skip("no specs to test SpecByKey")
	}

	key := specs[0].Key
	spec, err := db.SpecByKey(key)
	if err != nil {
		t.Fatalf("SpecByKey(%s): %v", key, err)
	}
	if spec.Key != key {
		t.Errorf("SpecByKey returned key %q, want %q", spec.Key, key)
	}
}

func TestSpecByKeyNotFound(t *testing.T) {
	db := openTestDB(t)
	_, err := db.SpecByKey("nonexistent-spec-key-99999")
	if err == nil {
		t.Error("SpecByKey for nonexistent key should return error")
	}
}

// ───────────────────────────────────────────────────────────────────────────
// Health Levels — the 7 Hebrew-named states
// ───────────────────────────────────────────────────────────────────────────

func TestAllHealthLevels(t *testing.T) {
	db := openTestDB(t)
	levels, err := db.AllHealthLevels()
	if err != nil {
		t.Fatalf("AllHealthLevels: %v", err)
	}

	if len(levels) != 7 {
		t.Errorf("expected 7 health levels, got %d", len(levels))
	}

	// Verify all 7 English level names are present.
	names := make(map[string]bool)
	for _, l := range levels {
		names[l.Name] = true
		if l.Hebrew == "" {
			t.Errorf("level %s has empty Hebrew field", l.Name)
		}
	}

	expected := []string{"broken", "wanting", "lacking", "even", "sound", "whole", "perfect"}
	for _, name := range expected {
		if !names[name] {
			t.Errorf("missing health level: %s", name)
		}
	}
}

func TestHealthLevelByName(t *testing.T) {
	db := openTestDB(t)
	level, err := db.HealthLevelByName("even")
	if err != nil {
		t.Fatalf("HealthLevelByName(even): %v", err)
	}
	if level.Hebrew == "" {
		t.Error("even level Hebrew should not be empty")
	}
	// "even" is the center point — stored value 128.
	if level.MinStored > 128 || level.MaxStored < 128 {
		t.Errorf("even level stored range [%d,%d] should include 128", level.MinStored, level.MaxStored)
	}
}

func TestHealthLevelByNamePerfect(t *testing.T) {
	db := openTestDB(t)
	level, err := db.HealthLevelByName("perfect")
	if err != nil {
		t.Fatalf("HealthLevelByName(perfect): %v", err)
	}
	if level.Hebrew == "" {
		t.Error("perfect level Hebrew should not be empty")
	}
	// "perfect" should include stored value 255.
	if level.MaxStored < 255 {
		t.Errorf("perfect level MaxStored = %d, should include 255", level.MaxStored)
	}
}

// ───────────────────────────────────────────────────────────────────────────
// Health Normalization — the 6 scales
// ───────────────────────────────────────────────────────────────────────────

func TestAllNormalizationBases(t *testing.T) {
	db := openTestDB(t)
	bases, err := db.AllNormalizationBases()
	if err != nil {
		t.Fatalf("AllNormalizationBases: %v", err)
	}
	if len(bases) != 6 {
		t.Errorf("expected 6 normalization bases, got %d", len(bases))
	}
}

// ───────────────────────────────────────────────────────────────────────────
// Health Scoring — rules for score calculation
// ───────────────────────────────────────────────────────────────────────────

func TestAllScoringRules(t *testing.T) {
	db := openTestDB(t)
	rules, err := db.AllScoringRules()
	if err != nil {
		t.Fatalf("AllScoringRules: %v", err)
	}
	if len(rules) == 0 {
		t.Error("should have scoring rules")
	}
}

func TestScoringRulesByCategory(t *testing.T) {
	db := openTestDB(t)
	// ScoringRulesByCategory appends % internally for LIKE prefix match.
	rules, err := db.ScoringRulesByCategory("init")
	if err != nil {
		t.Fatalf("ScoringRulesByCategory(init): %v", err)
	}
	for _, r := range rules {
		if len(r.RuleName) < 4 || r.RuleName[:4] != "init" {
			t.Errorf("rule %q should start with 'init'", r.RuleName)
		}
	}
}

// ───────────────────────────────────────────────────────────────────────────
// Health Log Actions — event types
// ───────────────────────────────────────────────────────────────────────────

func TestAllLogActions(t *testing.T) {
	db := openTestDB(t)
	actions, err := db.AllLogActions()
	if err != nil {
		t.Fatalf("AllLogActions: %v", err)
	}
	if len(actions) == 0 {
		t.Error("should have log actions")
	}
}

// ───────────────────────────────────────────────────────────────────────────
// DAR (Detect-Assess-Restore) — phases, aspects, severity
// ───────────────────────────────────────────────────────────────────────────

func TestAllDARPhases(t *testing.T) {
	db := openTestDB(t)
	phases, err := db.AllDARPhases()
	if err != nil {
		t.Fatalf("AllDARPhases: %v", err)
	}
	if len(phases) != 3 {
		t.Errorf("expected 3 DAR phases, got %d", len(phases))
	}
}

func TestAllDARAspects(t *testing.T) {
	db := openTestDB(t)
	aspects, err := db.AllDARAspects()
	if err != nil {
		t.Fatalf("AllDARAspects: %v", err)
	}
	if len(aspects) != 3 {
		t.Errorf("expected 3 DAR aspects, got %d", len(aspects))
	}
}

func TestAllDARSeverity(t *testing.T) {
	db := openTestDB(t)
	severity, err := db.AllDARSeverity()
	if err != nil {
		t.Fatalf("AllDARSeverity: %v", err)
	}
	if len(severity) != 3 {
		t.Errorf("expected 3 DAR severity levels, got %d", len(severity))
	}
}

func TestAllRestoreResults(t *testing.T) {
	db := openTestDB(t)
	results, err := db.AllRestoreResults()
	if err != nil {
		t.Fatalf("AllRestoreResults: %v", err)
	}
	if len(results) != 7 {
		t.Errorf("expected 7 restore results, got %d", len(results))
	}
}

func TestAllRestoreStrategies(t *testing.T) {
	db := openTestDB(t)
	strategies, err := db.AllRestoreStrategies()
	if err != nil {
		t.Fatalf("AllRestoreStrategies: %v", err)
	}
	if len(strategies) != 7 {
		t.Errorf("expected 7 restore strategies, got %d", len(strategies))
	}
}

// ───────────────────────────────────────────────────────────────────────────
// Permissions — states, categories, gating
// ───────────────────────────────────────────────────────────────────────────

func TestAllPermissionStates(t *testing.T) {
	db := openTestDB(t)
	states, err := db.AllPermissionStates()
	if err != nil {
		t.Fatalf("AllPermissionStates: %v", err)
	}
	if len(states) != 3 {
		t.Errorf("expected 3 permission states, got %d", len(states))
	}
}

func TestAllActionCategories(t *testing.T) {
	db := openTestDB(t)
	categories, err := db.AllActionCategories()
	if err != nil {
		t.Fatalf("AllActionCategories: %v", err)
	}
	if len(categories) != 4 {
		t.Errorf("expected 4 action categories, got %d", len(categories))
	}
}

func TestAllPermissionGating(t *testing.T) {
	db := openTestDB(t)
	gating, err := db.AllPermissionGating()
	if err != nil {
		t.Fatalf("AllPermissionGating: %v", err)
	}
	if len(gating) == 0 {
		t.Error("should have permission gating rules")
	}
}

func TestPermissionForHealthEven(t *testing.T) {
	db := openTestDB(t)
	gating, err := db.PermissionForHealth("even")
	if err != nil {
		t.Fatalf("PermissionForHealth(even): %v", err)
	}
	if gating.HealthLevel != "even" {
		t.Errorf("gating health level = %q, want %q", gating.HealthLevel, "even")
	}
}

// ───────────────────────────────────────────────────────────────────────────
// Filesystem Extensions — biblical extension mappings
// ───────────────────────────────────────────────────────────────────────────

func TestAllFilesystemExtensions(t *testing.T) {
	db := openTestDB(t)
	exts, err := db.AllFilesystemExtensions()
	if err != nil {
		t.Fatalf("AllFilesystemExtensions: %v", err)
	}
	if len(exts) == 0 {
		t.Error("should have filesystem extensions")
	}
}

// ───────────────────────────────────────────────────────────────────────────
// Dependencies — cross-reference queries
// ───────────────────────────────────────────────────────────────────────────

func TestDependenciesOfUnknown(t *testing.T) {
	db := openTestDB(t)
	deps, err := db.DependenciesOf("nonexistent-key")
	if err != nil {
		t.Fatalf("DependenciesOf: %v", err)
	}
	if len(deps) != 0 {
		t.Errorf("unknown spec should have 0 dependencies, got %d", len(deps))
	}
}

func TestDependentsOnUnknown(t *testing.T) {
	db := openTestDB(t)
	deps, err := db.DependentsOn("nonexistent-key")
	if err != nil {
		t.Fatalf("DependentsOn: %v", err)
	}
	if len(deps) != 0 {
		t.Errorf("unknown spec should have 0 dependents, got %d", len(deps))
	}
}

// ───────────────────────────────────────────────────────────────────────────
// Convenience: ResolveHealthLevel — score to level mapping
// ───────────────────────────────────────────────────────────────────────────

func TestResolveHealthLevelMidpoint(t *testing.T) {
	db := openTestDB(t)
	level, err := db.ResolveHealthLevel(128)
	if err != nil {
		t.Fatalf("ResolveHealthLevel(128): %v", err)
	}
	if level == nil {
		t.Fatal("level should not be nil")
	}
	t.Logf("score 128 = %s (%s)", level.Name, level.Hebrew)
}

func TestResolveHealthLevelZero(t *testing.T) {
	db := openTestDB(t)
	level, err := db.ResolveHealthLevel(0)
	if err != nil {
		t.Fatalf("ResolveHealthLevel(0): %v", err)
	}
	if level.Name != "broken" {
		t.Errorf("score 0 should be broken, got %q", level.Name)
	}
}

func TestResolveHealthLevelMax(t *testing.T) {
	db := openTestDB(t)
	level, err := db.ResolveHealthLevel(255)
	if err != nil {
		t.Fatalf("ResolveHealthLevel(255): %v", err)
	}
	if level.Name != "perfect" {
		t.Errorf("score 255 should be perfect, got %q", level.Name)
	}
}

// ───────────────────────────────────────────────────────────────────────────
// Convenience: ResolvePermission — score to permission mapping
// ───────────────────────────────────────────────────────────────────────────

func TestResolvePermissionMidpoint(t *testing.T) {
	db := openTestDB(t)
	perm, err := db.ResolvePermission(128)
	if err != nil {
		t.Fatalf("ResolvePermission(128): %v", err)
	}
	if perm == "" {
		t.Error("permission should not be empty")
	}
	t.Logf("score 128 permission = %s", perm)
}

// ───────────────────────────────────────────────────────────────────────────
// Thread Safety — concurrent reads should not panic
// ───────────────────────────────────────────────────────────────────────────

func TestConcurrentReads(t *testing.T) {
	db := openTestDB(t)
	done := make(chan bool, 10)

	for i := 0; i < 10; i++ {
		go func() {
			defer func() { done <- true }()
			_, _ = db.AllHealthLevels()
			_, _ = db.AllSpecs()
			_, _ = db.SpecCount()
		}()
	}

	for i := 0; i < 10; i++ {
		<-done
	}
}

// ───────────────────────────────────────────────────────────────────────────
// System Monitoring — thresholds for load, memory, disk
// ───────────────────────────────────────────────────────────────────────────

func TestAllMonitoring(t *testing.T) {
	db := openTestDB(t)
	items, err := db.AllMonitoring()
	if err != nil {
		t.Fatalf("AllMonitoring: %v", err)
	}
	// 3 metrics × 2 levels = 6 entries
	if len(items) != 6 {
		t.Errorf("expected 6 monitoring entries, got %d", len(items))
	}
	for _, m := range items {
		if m.Metric == "" || m.Level == "" {
			t.Errorf("monitoring entry has empty metric or level: %+v", m)
		}
		if m.Threshold <= 0 {
			t.Errorf("threshold should be positive: %+v", m)
		}
	}
}

func TestMonitoringByMetric(t *testing.T) {
	db := openTestDB(t)
	items, err := db.MonitoringByMetric("load")
	if err != nil {
		t.Fatalf("MonitoringByMetric(load): %v", err)
	}
	if len(items) != 2 {
		t.Errorf("expected 2 load thresholds (yellow, red), got %d", len(items))
	}
	// Find yellow and red regardless of order
	var yellow, red float64
	for _, m := range items {
		switch m.Level {
		case "yellow":
			yellow = m.Threshold
		case "red":
			red = m.Threshold
		}
	}
	if yellow >= red {
		t.Errorf("yellow (%.1f) should be < red (%.1f)", yellow, red)
	}
}

func TestMonitoringByMetricUnknown(t *testing.T) {
	db := openTestDB(t)
	items, err := db.MonitoringByMetric("nonexistent")
	if err != nil {
		t.Fatalf("MonitoringByMetric(nonexistent): %v", err)
	}
	if len(items) != 0 {
		t.Errorf("expected 0 entries for unknown metric, got %d", len(items))
	}
}

// ───────────────────────────────────────────────────────────────────────────
// Temporal Boundaries — time-of-day, session phases, circadian
// ───────────────────────────────────────────────────────────────────────────

func TestAllTemporalBoundaries(t *testing.T) {
	db := openTestDB(t)
	items, err := db.AllTemporalBoundaries()
	if err != nil {
		t.Fatalf("AllTemporalBoundaries: %v", err)
	}
	// 6 circadian + 5 session + 7 time_of_day = 18
	if len(items) != 18 {
		t.Errorf("expected 18 temporal boundaries, got %d", len(items))
	}
}

func TestTemporalByCategory(t *testing.T) {
	db := openTestDB(t)

	tests := []struct {
		category string
		expected int
	}{
		{"time_of_day", 7},
		{"session_phase", 5},
		{"circadian", 6},
	}

	for _, tc := range tests {
		items, err := db.TemporalByCategory(tc.category)
		if err != nil {
			t.Fatalf("TemporalByCategory(%s): %v", tc.category, err)
		}
		if len(items) != tc.expected {
			t.Errorf("TemporalByCategory(%s): expected %d, got %d", tc.category, tc.expected, len(items))
		}
	}
}

func TestTemporalByCategoryUnknown(t *testing.T) {
	db := openTestDB(t)
	items, err := db.TemporalByCategory("nonexistent")
	if err != nil {
		t.Fatalf("TemporalByCategory(nonexistent): %v", err)
	}
	if len(items) != 0 {
		t.Errorf("expected 0 for unknown category, got %d", len(items))
	}
}

// ───────────────────────────────────────────────────────────────────────────
// Terminal Display — command colors, debug markers, health emoji
// ───────────────────────────────────────────────────────────────────────────

func TestAllTerminalDisplay(t *testing.T) {
	db := openTestDB(t)
	items, err := db.AllTerminalDisplay()
	if err != nil {
		t.Fatalf("AllTerminalDisplay: %v", err)
	}
	if len(items) == 0 {
		t.Error("expected terminal display entries, got 0")
	}
	t.Logf("terminal display entries: %d", len(items))
}

func TestTerminalByCategory(t *testing.T) {
	db := openTestDB(t)

	categories := []string{"command_color", "state_config", "health_emoji", "debug_marker"}
	for _, cat := range categories {
		items, err := db.TerminalByCategory(cat)
		if err != nil {
			t.Fatalf("TerminalByCategory(%s): %v", cat, err)
		}
		if len(items) == 0 {
			t.Errorf("TerminalByCategory(%s): expected entries, got 0", cat)
		}
	}
}

func TestTerminalCommandColors(t *testing.T) {
	db := openTestDB(t)
	items, err := db.TerminalByCategory("command_color")
	if err != nil {
		t.Fatalf("TerminalByCategory(command_color): %v", err)
	}
	// 3 commands (HALT, AWAIT, PROCEED) stored as category=command_color
	if len(items) != 3 {
		t.Errorf("expected 3 command color entries, got %d", len(items))
	}
}

// ───────────────────────────────────────────────────────────────────────────
// Deployment Hosts — oracle, dell
// ───────────────────────────────────────────────────────────────────────────

func TestAllHosts(t *testing.T) {
	db := openTestDB(t)
	hosts, err := db.AllHosts()
	if err != nil {
		t.Fatalf("AllHosts: %v", err)
	}
	if len(hosts) != 2 {
		t.Errorf("expected 2 deployment hosts, got %d", len(hosts))
	}
	// Primary host should be first (ORDER BY is_primary DESC)
	if len(hosts) >= 1 && !hosts[0].IsPrimary {
		t.Error("first host should be primary")
	}
}

func TestHostByName(t *testing.T) {
	db := openTestDB(t)
	host, err := db.HostByName("oracle")
	if err != nil {
		t.Fatalf("HostByName(oracle): %v", err)
	}
	if host.Arch != "arm64" {
		t.Errorf("oracle arch should be arm64, got %q", host.Arch)
	}
	if !host.IsPrimary {
		t.Error("oracle should be primary")
	}
}

func TestHostByNameDell(t *testing.T) {
	db := openTestDB(t)
	host, err := db.HostByName("dell")
	if err != nil {
		t.Fatalf("HostByName(dell): %v", err)
	}
	if host.Arch != "amd64" {
		t.Errorf("dell arch should be amd64, got %q", host.Arch)
	}
	if host.IsPrimary {
		t.Error("dell should not be primary")
	}
}

func TestPrimaryHost(t *testing.T) {
	db := openTestDB(t)
	host, err := db.PrimaryHost()
	if err != nil {
		t.Fatalf("PrimaryHost: %v", err)
	}
	if host.Name != "oracle" {
		t.Errorf("primary host should be oracle, got %q", host.Name)
	}
}

// ───────────────────────────────────────────────────────────────────────────
// Server Config — port, env vars, endpoints
// ───────────────────────────────────────────────────────────────────────────

func TestAllServerConfig(t *testing.T) {
	db := openTestDB(t)
	configs, err := db.AllServerConfig()
	if err != nil {
		t.Fatalf("AllServerConfig: %v", err)
	}
	if len(configs) == 0 {
		t.Error("expected server config entries, got 0")
	}
	t.Logf("server config entries: %d", len(configs))
}

func TestServerConfigByKey(t *testing.T) {
	db := openTestDB(t)
	cfg, err := db.ServerConfigByKey("port")
	if err != nil {
		t.Fatalf("ServerConfigByKey(port): %v", err)
	}
	if cfg.Value != "3847" {
		t.Errorf("port should be 3847, got %q", cfg.Value)
	}
}

func TestServerConfigByKeyEndpoint(t *testing.T) {
	db := openTestDB(t)
	cfg, err := db.ServerConfigByKey("health_endpoint")
	if err != nil {
		t.Fatalf("ServerConfigByKey(health_endpoint): %v", err)
	}
	if cfg.Value != "/health" {
		t.Errorf("health_endpoint should be /health, got %q", cfg.Value)
	}
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
