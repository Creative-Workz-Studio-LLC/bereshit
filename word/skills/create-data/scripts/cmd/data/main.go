// ============================================================================
// METADATA - Data CLI Tool
// ============================================================================
// Purpose: CLI for create-data skill
// Biblical: Proverbs 25:2 - "the honour of kings is to search out a matter"
// Usage: data [command] [args]

package main

import (
	"fmt"
	"os"

	"github.com/creativeworkzstudio/claude-global/skills/create-data/scripts/pkg/config"
	"github.com/creativeworkzstudio/claude-global/skills/create-data/scripts/pkg/datagen"
)

// ============================================================================
// SETUP - Constants
// ============================================================================

const usage = `data - Data file creation helper

Usage:
  data formats         List all data formats
  data format <name>   Show format details
  data tree            Show decision tree
  data blocks          Show 3-block structure
  data guide           Show creation process
  data help            Show this help

Data Formats:
  jsonc      Configuration with comments (JSONC)
  yaml       Human-readable structured data (YAML)
  toml       Configuration files (TOML)
  json       Pure data interchange (JSON)

3-Block Structure:
  METADATA  - Identity and purpose
  CONTENT   - The actual data
  CLOSING   - Related references

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
	case "formats":
		cmdFormats(cfg)
	case "format":
		cmdFormat(cfg, os.Args[2:])
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

func cmdFormats(cfg *config.Config) {
	fmt.Println(datagen.ListFormats(cfg))
}

func cmdFormat(cfg *config.Config, args []string) {
	if len(args) < 1 {
		fmt.Println("Usage: data format <name>")
		fmt.Println("Formats:", cfg.GetFormatNames())
		os.Exit(1)
	}
	fmt.Println(datagen.GetFormatInfo(cfg, args[0]))
}

func cmdTree(cfg *config.Config) {
	fmt.Println(datagen.ShowTree(cfg))
}

func cmdBlocks(cfg *config.Config) {
	fmt.Println(datagen.ShowBlocks(cfg))
}

func cmdGuide(cfg *config.Config) {
	fmt.Println(datagen.Guide(cfg))
}
