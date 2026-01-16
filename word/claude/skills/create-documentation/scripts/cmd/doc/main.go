// ============================================================================
// METADATA - Documentation CLI Tool
// ============================================================================
// Purpose: CLI for create-documentation skill
// Biblical: Proverbs 25:2 - "the honour of kings is to search out a matter"
// Usage: doc [command] [args]

package main

import (
	"fmt"
	"os"

	"github.com/creativeworkzstudio/claude-global/skills/create-documentation/scripts/pkg/config"
	"github.com/creativeworkzstudio/claude-global/skills/create-documentation/scripts/pkg/docgen"
)

// ============================================================================
// SETUP - Constants
// ============================================================================

const usage = `doc - Documentation creation helper

Usage:
  doc types           List all document types
  doc type <name>     Show document type details
  doc tree            Show decision tree
  doc blocks          Show 5-block structure
  doc quality         Show quality standard
  doc toolkit         Show AsciiDoc toolkit
  doc reading         Show layered reading levels
  doc guide           Show creation process
  doc help            Show this help

Document Types:
  base       Standard documentation, guides, specifications
  research   Original research, deep analysis, investigations
  index      Directory navigation, collection overviews
  community  README, CONTRIBUTING, public-facing docs

Quality Standard:
  PhD Grade by Bible Accessible
  - PhD Rigor: Complete, defensible, machine-parseable
  - Bible Accessibility: 5-second scan, clear navigation, timeless

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
	case "types":
		cmdTypes(cfg)
	case "type":
		cmdType(cfg, os.Args[2:])
	case "tree":
		cmdTree(cfg)
	case "blocks":
		cmdBlocks(cfg)
	case "quality":
		cmdQuality(cfg)
	case "toolkit":
		cmdToolkit(cfg)
	case "reading":
		cmdReading(cfg)
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

func cmdTypes(cfg *config.Config) {
	fmt.Println(docgen.ListTypes(cfg))
}

func cmdType(cfg *config.Config, args []string) {
	if len(args) < 1 {
		fmt.Println("Usage: doc type <name>")
		fmt.Println("Types:", cfg.GetTypeNames())
		os.Exit(1)
	}
	fmt.Println(docgen.GetTypeInfo(cfg, args[0]))
}

func cmdTree(cfg *config.Config) {
	fmt.Println(docgen.ShowTree(cfg))
}

func cmdBlocks(cfg *config.Config) {
	fmt.Println(docgen.ShowBlocks(cfg))
}

func cmdQuality(cfg *config.Config) {
	fmt.Println(docgen.ShowQuality(cfg))
}

func cmdToolkit(cfg *config.Config) {
	fmt.Println(docgen.ShowToolkit(cfg))
}

func cmdReading(cfg *config.Config) {
	fmt.Println(docgen.ShowReading(cfg))
}

func cmdGuide(cfg *config.Config) {
	fmt.Println(docgen.Guide(cfg))
}
