// ============================================================================
// METADATA - Code CLI Tool
// ============================================================================
// Purpose: CLI for create-code skill
// Biblical: Proverbs 25:2 - "the honour of kings is to search out a matter"
// Usage: code [command] [args]

package main

import (
	"fmt"
	"os"

	"cws.studio/skills/create-code/scripts/pkg/config"
	"cws.studio/skills/create-code/scripts/pkg/codegen"
)

// ============================================================================
// SETUP - Constants
// ============================================================================

const usage = `code - Code file creation helper

Usage:
  code languages       List all language types
  code language <name> Show language details
  code tree            Show decision tree
  code blocks          Show 4-block structure
  code guide           Show creation process
  code help            Show this help

Languages:
  go         Go source files (.go)
  c          C source files (.c, .h)
  bash       Shell scripts (.sh)
  python     Python files (.py)

4-Block Structure:
  METADATA  - Identity, purpose, health scoring
  SETUP     - Imports, constants, types
  BODY      - Core logic and functions
  CLOSING   - Validation, execution, cleanup

Options:
  -h, -help   Show help
`

// ============================================================================
// BODY - Main
// ============================================================================

func main() {
	if len(os.Args) < 2 {
		fmt.Println(usage)
		os.Exit(0)
	}

	arg := os.Args[1]
	if arg == "-h" || arg == "-help" || arg == "help" {
		fmt.Println(usage)
		os.Exit(0)
	}

	cfg, err := config.Load()
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error loading config: %v\n", err)
		os.Exit(1)
	}

	switch arg {
	case "languages":
		cmdLanguages(cfg)
	case "language":
		cmdLanguage(cfg, os.Args[2:])
	case "tree":
		cmdTree(cfg)
	case "blocks":
		cmdBlocks(cfg)
	case "guide":
		cmdGuide(cfg)
	default:
		fmt.Fprintf(os.Stderr, "Unknown command: %s\n", arg)
		fmt.Println(usage)
		os.Exit(1)
	}
}

// ============================================================================
// BODY - Commands
// ============================================================================

func cmdLanguages(cfg *config.Config) {
	fmt.Println(codegen.ListLanguages(cfg))
}

func cmdLanguage(cfg *config.Config, args []string) {
	if len(args) < 1 {
		fmt.Println("Usage: code language <name>")
		fmt.Println("Languages:", cfg.GetLanguageNames())
		os.Exit(1)
	}
	fmt.Println(codegen.GetLanguageInfo(cfg, args[0]))
}

func cmdTree(cfg *config.Config) {
	fmt.Println(codegen.ShowTree(cfg))
}

func cmdBlocks(cfg *config.Config) {
	fmt.Println(codegen.ShowBlocks(cfg))
}

func cmdGuide(cfg *config.Config) {
	fmt.Println(codegen.Guide(cfg))
}
