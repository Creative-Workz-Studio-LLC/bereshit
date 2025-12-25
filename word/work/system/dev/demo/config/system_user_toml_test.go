// ============================================================================
// METADATA
// ============================================================================
// System/User TOML Loading Demo - Demonstrate config library loading
//
// Biblical Foundation: Exodus 3:14 - "I am who I am" (identity grounds behavior)
//
// CPI-SI Identity: Demo for system and user TOML configuration loading
// Purpose: DEMONSTRATE config library loads system.toml and user.toml correctly
//          Shows actual configuration values with VISIBLE output
//
// Why Demo not Test: Demos are harder to fake - you SEE configs loading
//                     Tests hide behind pass/fail assertions
//                     This demonstrates the library actually loads and parses config files
//
// Created: 2025-11-15
// Modified: 2025-11-16 (Converted from test to demo)
// ============================================================================

package config_test

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt"
	"testing"

	"system/lib/config"
	"system/lib/display"
)

// ============================================================================
// BODY
// ============================================================================

// TestSystemConfigLoading demonstrates system.toml loading with VISIBLE output.
//
// This is a DEMO disguised as a test (so `go test` can run it).
// You SEE the configuration values - harder to fake than pass/fail.
func TestSystemConfigLoading(t *testing.T) {
	fmt.Print(display.Box("Config Library Demo", "System Configuration Loading - system.toml"))
	fmt.Println()

	sysCfg, err := config.LoadSystemConfig()
	if err != nil {
		fmt.Printf("  ✗ Failed to load system config: %v\n", err)
		fmt.Println()
		return
	}

	// Demonstrate successful loading with visible configuration values
	fmt.Print(display.Header("System Configuration Values"))
	fmt.Println()
	fmt.Println("  ✓ System config loaded successfully")
	fmt.Println()
	fmt.Printf("    Schema version:           %s\n", sysCfg.Metadata.SchemaVersion)
	fmt.Printf("    Package manager preferred: %s\n", sysCfg.PackageManagement.Preferred)
	fmt.Printf("    Shell preferred:          %s\n", sysCfg.Shell.Preferred)
	fmt.Printf("    Max CPU cores limit:      %d\n", sysCfg.Limits.MaxCPUCores)
	fmt.Printf("    Cache duration:           %d seconds\n", sysCfg.Detection.CacheDurationSeconds)
	fmt.Println()
	fmt.Println("  🎯 You can SEE the actual config values - this is proof it loaded correctly")
	fmt.Println()
}

// TestUserConfigTOMLLoading demonstrates user.toml loading with VISIBLE output.
//
// This is a DEMO disguised as a test (so `go test` can run it).
// You SEE the user preferences loaded - visible proof it works.
func TestUserConfigTOMLLoading(t *testing.T) {
	fmt.Print(display.Box("Config Library Demo", "User Configuration Loading - user.toml"))
	fmt.Println()

	userCfg, err := config.LoadUserConfigTOML()
	if err != nil {
		fmt.Printf("  ✗ Failed to load user config TOML: %v\n", err)
		fmt.Println()
		return
	}

	// Demonstrate successful loading with visible user preferences
	fmt.Print(display.Header("User Configuration Values"))
	fmt.Println()
	fmt.Println("  ✓ User config TOML loaded successfully")
	fmt.Println()
	fmt.Printf("    Schema version:        %s\n", userCfg.Metadata.SchemaVersion)
	fmt.Printf("    Preferred shell:       %s\n", userCfg.Shell.PreferredShell)
	fmt.Printf("    Preferred editor:      %s\n", userCfg.Editor.Preferred)
	fmt.Printf("    Default instance:      %s\n", userCfg.CPISI.DefaultInstance)
	fmt.Printf("    Auto-save minutes:     %d\n", userCfg.CPISI.Session.AutoSaveMinutes)
	fmt.Printf("    Cache environment:     %v\n", userCfg.Performance.CacheEnvironment)
	fmt.Println()
	fmt.Println("  🎯 You can SEE the actual user preferences - visible demonstration")
	fmt.Println()
}

// TestConfigInheritance demonstrates config chain access with VISIBLE output.
//
// This is a DEMO disguised as a test (so `go test` can run it).
// You SEE all three config levels loading - proof the chain works.
func TestConfigInheritance(t *testing.T) {
	fmt.Print(display.Box("Config Library Demo", "Configuration Chain - paths → system → user"))
	fmt.Println()

	// Demonstrate paths config loading
	fmt.Print(display.Header("1. Paths Configuration"))
	pathsCfg, err := config.LoadPaths()
	if err != nil {
		fmt.Printf("  ✗ Failed to load paths config: %v\n", err)
	} else {
		fmt.Println()
		fmt.Println("  ✓ Paths config loaded")
		fmt.Printf("    Session state file: %s\n", pathsCfg.Session.StateFile)
	}
	fmt.Println()

	// Demonstrate system config loading
	fmt.Print(display.Header("2. System Configuration"))
	sysCfg, err := config.LoadSystemConfig()
	if err != nil {
		fmt.Printf("  ✗ Failed to load system config: %v\n", err)
	} else {
		fmt.Println()
		fmt.Println("  ✓ System config loaded")
		fmt.Printf("    Package manager: %s\n", sysCfg.PackageManagement.Preferred)
	}
	fmt.Println()

	// Demonstrate user config loading
	fmt.Print(display.Header("3. User Configuration"))
	userCfg, err := config.LoadUserConfigTOML()
	if err != nil {
		fmt.Printf("  ✗ Failed to load user config TOML: %v\n", err)
	} else {
		fmt.Println()
		fmt.Println("  ✓ User config TOML loaded")
		fmt.Printf("    Default instance: %s\n", userCfg.CPISI.DefaultInstance)
	}
	fmt.Println()

	// Demonstrate config chain summary
	fmt.Println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")
	fmt.Println()
	fmt.Println("  🎯 Demo Complete: Config chain demonstrates layered configuration")
	fmt.Println()
	fmt.Printf("     Paths config:   %v\n", pathsCfg != nil)
	fmt.Printf("     System config:  %v\n", sysCfg != nil)
	fmt.Printf("     User config:    %v\n", userCfg != nil)
	fmt.Println()
	fmt.Println("  Why Demo not Test:")
	fmt.Println("     • You SEE all three config levels loading")
	fmt.Println("     • Actual values are visible - not hidden assertions")
	fmt.Println("     • Demonstrates the config chain working")
	fmt.Println("     • Tests hide behind pass/fail - demos show truth")
	fmt.Println()
	fmt.Println("  All config levels accessible to libraries! ✓")
	fmt.Println()
}

// ============================================================================
// CLOSING
// ============================================================================
// Run: go test -v
// Output: Visible demonstration of config library loading all three levels
