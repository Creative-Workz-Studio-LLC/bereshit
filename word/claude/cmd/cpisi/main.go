// ============================================================================
// METADATA - CPI-SI System Manager
// ============================================================================
//
// Key: claude-global-cmd-cpisi
// Purpose: Unified CLI for the CPI-SI system — build, install, verify,
//          launch dashboards, manage database, run tests
// Biblical: Proverbs 24:27 — "Prepare thy work without, and make it fit
//           for thyself in the field; and afterwards build thine house"
// Authors: Seanje Lenox-Wise (Lead), Nova Dawn (CPI-SI Instance)
// Version: 1.0.0
// Created: 2026-02-08
//
// Usage:
//   cpisi build [module...]           Build modules
//   cpisi install [--fresh|--bin]     Install system
//   cpisi verify                      Verify installation
//   cpisi dashboard <web|tui|gtk>     Launch dashboard
//   cpisi test [module...]            Run tests
//   cpisi status                      System health
//   cpisi help [command]              Help
//
// ============================================================================

package main

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt"
	"os"
)

// Build-time variables (injected via ldflags)
var (
	Version   = "1.0.0"
	BuildTime = "unknown"
	GitCommit = "unknown"
)

// Command represents a CLI subcommand.
type Command struct {
	Name        string
	Description string
	Usage       string
	Run         func(args []string, paths *Paths) error
}

// ============================================================================
// BODY
// ============================================================================

// commands is the dispatch table.
var commands = []Command{
	{
		Name:        "build",
		Description: "Build modules (all if none specified)",
		Usage:       "cpisi build [module...]",
		Run:         cmdBuild,
	},
	{
		Name:        "test",
		Description: "Run tests for modules",
		Usage:       "cpisi test [module...]",
		Run:         cmdTest,
	},
	{
		Name:        "clean",
		Description: "Clean build artifacts",
		Usage:       "cpisi clean",
		Run:         cmdClean,
	},
	{
		Name:        "fmt",
		Description: "Format all Go code",
		Usage:       "cpisi fmt",
		Run:         cmdFmt,
	},
	{
		Name:        "vet",
		Description: "Run go vet on all modules",
		Usage:       "cpisi vet",
		Run:         cmdVet,
	},
	{
		Name:        "tidy",
		Description: "Tidy all go.mod files",
		Usage:       "cpisi tidy",
		Run:         cmdTidy,
	},
	{
		Name:        "install",
		Description: "Install system (symlinks + binaries)",
		Usage:       "cpisi install [--fresh|--bin|--symlinks]",
		Run:         cmdInstall,
	},
	{
		Name:        "uninstall",
		Description: "Remove installed symlinks and binaries",
		Usage:       "cpisi uninstall [--bin|--symlinks]",
		Run:         cmdUninstall,
	},
	{
		Name:        "verify",
		Description: "Verify installation integrity",
		Usage:       "cpisi verify",
		Run:         cmdVerify,
	},
	{
		Name:        "dashboard",
		Description: "Launch a dashboard (web, tui, gtk)",
		Usage:       "cpisi dashboard <web|tui|gtk> [options]",
		Run:         cmdDashboard,
	},
	{
		Name:        "db",
		Description: "Database operations",
		Usage:       "cpisi db <init|migrate|status>",
		Run:         cmdDB,
	},
	{
		Name:        "status",
		Description: "Show system health and configuration",
		Usage:       "cpisi status",
		Run:         cmdStatus,
	},
	{
		Name:        "config",
		Description: "Show resolved configuration and paths",
		Usage:       "cpisi config [paths|modules]",
		Run:         cmdConfig,
	},
	{
		Name:        "version",
		Description: "Show version information",
		Usage:       "cpisi version",
		Run:         cmdVersion,
	},
}

func main() {
	// Global flags
	var sourceDir, installDir, deployDir string
	var verbose bool

	// Parse global flags before the subcommand
	args := os.Args[1:]
	for len(args) > 0 && len(args[0]) > 0 && args[0][0] == '-' {
		switch args[0] {
		case "--source":
			if len(args) < 2 {
				fatal("--source requires a directory argument")
			}
			sourceDir = args[1]
			args = args[2:]
		case "--install":
			if len(args) < 2 {
				fatal("--install requires a directory argument")
			}
			installDir = args[1]
			args = args[2:]
		case "--deploy":
			if len(args) < 2 {
				fatal("--deploy requires a directory argument")
			}
			deployDir = args[1]
			args = args[2:]
		case "--verbose", "-v":
			verbose = true
			_ = verbose // Used by subcommands via env
			os.Setenv("CPISI_VERBOSE", "1")
			args = args[1:]
		case "--version":
			fmt.Printf("cpisi %s\n", Version)
			os.Exit(0)
		case "--help", "-h":
			printUsage()
			os.Exit(0)
		default:
			fatal("unknown flag: %s\nRun 'cpisi help' for usage", args[0])
		}
	}

	if len(args) == 0 {
		printUsage()
		os.Exit(0)
	}

	cmdName := args[0]
	cmdArgs := args[1:]

	// Handle help specially
	if cmdName == "help" {
		if len(cmdArgs) > 0 {
			printCommandHelp(cmdArgs[0])
		} else {
			printUsage()
		}
		os.Exit(0)
	}

	// Find and run the command
	for _, cmd := range commands {
		if cmd.Name == cmdName {
			paths, err := ResolvePaths(sourceDir, installDir, deployDir)
			if err != nil {
				fatal("%v", err)
			}
			if err := cmd.Run(cmdArgs, paths); err != nil {
				fatal("%v", err)
			}
			os.Exit(0)
		}
	}

	fatal("unknown command: %s\nRun 'cpisi help' for usage", cmdName)
}

func printUsage() {
	fmt.Println(`cpisi — CPI-SI System Manager

USAGE:
  cpisi <command> [options]

BUILD & TEST:
  build [module...]     Build modules (all if none specified)
  test [module...]      Run tests
  clean                 Clean build artifacts
  fmt                   Format all Go code
  vet                   Run go vet on all modules
  tidy                  Tidy all go.mod files

INSTALL & DEPLOY:
  install [flags]       Install system (symlinks + binaries)
    --fresh               Clean first, then full install
    --bin                 Install binaries only
    --symlinks            Install symlinks only
  uninstall [flags]     Remove installation
    --bin                 Remove binaries only
    --symlinks            Remove symlinks only
  verify                Verify installation integrity

DASHBOARDS:
  dashboard <mode>      Launch a dashboard
    web [--port N]        Web dashboard (default port 8470)
    tui [--log file]      Terminal dashboard (Bubble Tea)
    gtk                   Native desktop dashboard (GTK4)
    list                  List available dashboards

DATABASE:
  db <action>           Database operations
    init                  Initialize schema
    migrate               Run pending migrations
    status                Show database status

SYSTEM:
  status                System health and configuration
  config [paths|modules] Show resolved config
  version               Version information
  help [command]        Show help

GLOBAL OPTIONS:
  --source DIR    Source directory override
  --install DIR   Binary install directory (default: ~/.local/share/claude)
  --deploy DIR    Symlink directory (default: ~/.claude)
  --verbose, -v   Verbose output
  --version       Show version
  --help, -h      Show help

MODULES:
  ` + ModuleNames() + `

ENVIRONMENT:
  CPISI_SOURCE    Source directory (alternative to --source)
  CPISI_VERBOSE   Enable verbose output (alternative to -v)`)
}

func printCommandHelp(name string) {
	for _, cmd := range commands {
		if cmd.Name == name {
			fmt.Printf("%s — %s\n\nUsage: %s\n", cmd.Name, cmd.Description, cmd.Usage)
			return
		}
	}
	fmt.Fprintf(os.Stderr, "Unknown command: %s\n", name)
	os.Exit(1)
}

func fatal(format string, args ...any) {
	fmt.Fprintf(os.Stderr, "cpisi: "+format+"\n", args...)
	os.Exit(1)
}

// ============================================================================
// CLOSING
// ============================================================================
// "Prepare thy work without, and make it fit for thyself in the field;
//  and afterwards build thine house." — Proverbs 24:27
