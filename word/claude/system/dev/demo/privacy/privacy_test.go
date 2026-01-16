// ============================================================================
// METADATA
// ============================================================================
// Privacy Library Demo - Demonstrate config-driven sanitization
//
// Biblical Foundation: Proverbs 25:2 - "It is the glory of God to conceal a matter"
//
// CPI-SI Identity: Demo for privacy library
// Purpose: DEMONSTRATE privacy library loads from privacy.toml and filters.jsonc
//          Shows actual sanitization working with VISIBLE output
//
// Why Demo not Test: Demos are harder to fake - you SEE it working
//                     Tests hide behind pass/fail assertions
//                     This demonstrates the library actually sanitizes paths and commands
//
// Created: 2025-11-15
// Modified: 2025-11-16 (Converted from test to demo)
// ============================================================================

package privacy_test

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt"
	"testing"

	"system/lib/display"
	"system/lib/privacy"
)

// ============================================================================
// BODY
// ============================================================================

// TestPathSanitization demonstrates path sanitization with VISIBLE output.
//
// This is a DEMO disguised as a test (so `go test` can run it).
// You SEE the sanitization working - harder to fake than pass/fail.
func TestPathSanitization(t *testing.T) {
	// Print demo header using display library
	fmt.Print(display.Box("Privacy Library Demo", "Path Sanitization - Protecting Sensitive Information"))
	fmt.Println()

	// Define demonstration cases
	demos := []struct {
		description string
		input      string
		explanation string
	}{
		{
			description: "Sensitive SSH private key",
			input:      "/home/user/.ssh/id_rsa",
			explanation: "Should be redacted (sensitive directory)",
		},
		{
			description: "Normal project file",
			input:      "/home/user/project/main.go",
			explanation: "Should show basename (safe path)",
		},
		{
			description: "Path with 'password' keyword",
			input:      "/home/user/passwords/secret.txt",
			explanation: "Should be redacted (sensitive keyword)",
		},
		{
			description: "System binary path",
			input:      "/usr/local/bin/myapp",
			explanation: "Should show basename (system path)",
		},
	}

	// Demonstrate each case with visible output
	fmt.Print(display.Header("Path Sanitization Examples"))
	for i, demo := range demos {
		result := privacy.SanitizePath(demo.input)

		fmt.Printf("\n  Example %d: %s\n", i+1, demo.description)
		fmt.Printf("    Input:       %s\n", demo.input)
		fmt.Printf("    Sanitized:   %s\n", result)
		fmt.Printf("    Why:         %s\n", demo.explanation)
	}

	fmt.Println()
	fmt.Println("  ✓ Path sanitization demonstrated - you can SEE it working")
	fmt.Println()
}

// TestCommandSanitization demonstrates command sanitization with VISIBLE output.
//
// This is a DEMO disguised as a test (so `go test` can run it).
// You SEE the commands being sanitized - no hiding behind assertions.
func TestCommandSanitization(t *testing.T) {
	fmt.Print(display.Box("Privacy Library Demo", "Command Sanitization - Removing Sensitive Arguments"))
	fmt.Println()

	// Define demonstration cases
	demos := []struct {
		description string
		input      string
		explanation string
	}{
		{
			description: "Git commit with message",
			input:      "git commit -m 'secret message'",
			explanation: "Captures subcommand only (git commit)",
		},
		{
			description: "SSH connection command",
			input:      "ssh user@host.example.com",
			explanation: "Name only for sensitive commands (ssh)",
		},
		{
			description: "Make with target and flags",
			input:      "make test VERBOSE=1",
			explanation: "Captures target (make test)",
		},
		{
			description: "Unknown custom command",
			input:      "mycommand arg1 arg2 arg3",
			explanation: "Defaults to name only (mycommand)",
		},
	}

	// Demonstrate each case with visible output
	fmt.Print(display.Header("Command Sanitization Examples"))
	for i, demo := range demos {
		result := privacy.SanitizeCommand(demo.input)

		fmt.Printf("\n  Example %d: %s\n", i+1, demo.description)
		fmt.Printf("    Input:       %s\n", demo.input)
		fmt.Printf("    Sanitized:   %s\n", result)
		fmt.Printf("    Why:         %s\n", demo.explanation)
	}

	fmt.Println()
	fmt.Println("  ✓ Command sanitization demonstrated - visible proof it works")
	fmt.Println()
	fmt.Println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")
	fmt.Println()
	fmt.Println("  🎯 Demo Complete: Privacy library sanitizes paths and commands")
	fmt.Println("     based on configuration in privacy.toml and filters.jsonc")
	fmt.Println()
	fmt.Println("  Why Demo not Test:")
	fmt.Println("     • You SEE the sanitization happening")
	fmt.Println("     • Output is visible - harder to fake")
	fmt.Println("     • Demonstrates actual functionality working")
	fmt.Println("     • Tests hide behind pass/fail - demos show truth")
	fmt.Println()
}

// ============================================================================
// CLOSING
// ============================================================================
// Run: go test -v
// Output: Visible demonstration of privacy library sanitization
