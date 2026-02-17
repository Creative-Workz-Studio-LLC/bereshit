//omni:code --go -library
//omni:key B-L0-hybrid-cli
//omni:version b-01.01
//omni:pragma _pragma
//omni:metadata _metadata

// ============================================================================
// METADATA
// ============================================================================

// Package cli provides lightweight CLI scaffolding for CPI-SI skill commands.
//
// Wraps Go's flag package with standard conventions: -help/-h flags, -json
// output mode, biblical references in help text, and structured usage builders.
// Pure stdlib — no external dependencies.
//
// Usage:
//
//	app := cli.New("mytool", "Does something useful")
//	app.WithBiblical("Proverbs 15:28")
//	app.ParseOS()
//	if app.WantsHelp() { cli.NewUsage(app).Print(); return }
//
// See _pragma and _metadata for OmniCode identity.
package cli

// ────────────────────────────────────────────────────────────────
// Imports
// ────────────────────────────────────────────────────────────────

//--- I.1 Standard Library [IMPORT] (1) ---
// Flag parsing, output, process control
import (
	"flag" // CLI flag parsing
	"fmt"  // formatted output
	"os"   // stderr, exit, args
)

//--- I.3 Internal Packages [IMPORT] (0) ---
// L0 identity registry — package self-awareness
import (
	"creativeworkzstudio.com/bereshit/L0-universal/spiral/identity" // Register — package identity registry
)

// ────────────────────────────────────────────────────────────────
// Pragma
// ────────────────────────────────────────────────────────────────

// _pragma — OmniCode file identity (P1-P5). Runtime-queryable.
var _pragma = map[string]string{
	// P1: Core Identity
	"P1.key":    "B-L0-hybrid-cli",
	"P1.type":   "code",
	"P1.format": "go",
	"P1.style":  "library",
	// P2: Structure
	"P2.blocks": "4-block",
	// P3: Derivation
	"P3.from":    "b-word/seed/code/L0/go/library.go",
	"P3.derives": "B-L0-seed-go-library",
	// P4: Version
	"P4.at": "b-01.01",
	// P5: Summary
	"P5.title":   "CLI Scaffolding Library",
	"P5.summary": "Lightweight CLI infrastructure for CPI-SI skill commands",
}

// ────────────────────────────────────────────────────────────────
// Metadata
// ────────────────────────────────────────────────────────────────

// _metadata — OmniCode file metadata (M1-M10). Runtime-queryable.
var _metadata = map[string]string{

	// ── M1: Core Identity ─────────────────────────────────────────
	"M1.key":            "B-L0-hybrid-cli",
	"M1.component_type": "Foundation",
	"M1.architect":      "Seanje Lenox-Wise",
	"M1.implementation": "Nova Dawn",
	"M1.created":        "2025-12-10",

	// ── M2: Version History ───────────────────────────────────────
	"M2.b-01.01": "2026-02-14 — REORG: promoted from hybrid/util/ to hybrid/ top-level domain",
	"M2.b-01.00": "2026-02-14 — BASELINE: 4-block, identity registration, pragma/metadata",
	"M2.a-01.00": "2025-12-10 — Initial creation",

	// ── M3: Interface ─────────────────────────────────────────────
	"M3.requires.stdlib":   "flag, fmt, os",
	"M3.requires.external": "none",
	"M3.requires.internal": "L0-universal/spiral/identity",
	"M3.used_by":           "all CPI-SI skill CLI commands",
	"M3.import":            "creativeworkzstudio.com/bereshit/L0-universal/hybrid/cli",
	"M3.pattern":           "cli.New(name, desc) → app.ParseOS() → app.WantsHelp()",

	// ── M4: Public API ────────────────────────────────────────────
	"M4.create":  "New",
	"M4.config":  "WithBiblical, WithJSON, Flag",
	"M4.parse":   "Parse, ParseOS, WantsHelp, WantsJSON, Args",
	"M4.output":  "Fatal, FatalCode, Error, Warn",
	"M4.usage":   "NewUsage, AddSection, Print",
	"M4.self":    "Pragma, Metadata",

	// ── M5: Operational ───────────────────────────────────────────
	"M5.blocking": "no — flag parsing only",
	"M5.health":   "Foundation | granted: parsed | deferred: n/a | denied: error",

	// ── M6: Classification ────────────────────────────────────────
	"M6.tags":     "cli, flags, usage, pure, stdlib-only",
	"M6.category": "Foundation",
	"M6.domain":   "cli-infrastructure",
	"M6.layer":    "L0-universal",
	"M6.paradigm": "CPI-SI",

	// ── M7: Intent ────────────────────────────────────────────────
	"M7.purpose":    "Standard CLI conventions for all CPI-SI skills",
	"M7.philosophy": "Convention over configuration — consistent flags across all tools",
	"M7.provides":   "Flag parsing, help output, usage builder, error functions",

	// ── M8: Grounding ─────────────────────────────────────────────
	"M8.scripture": "Proverbs 15:28 — The heart of the righteous studieth to answer",
	"M8.principle": "Study before answering — parse input before acting",
	"M8.anchor":    "Genesis 1:1 — In the beginning God created the heaven and the earth",

	// ── M9: Dependencies ──────────────────────────────────────────
	"M9.needs.stdlib":   "flag, fmt, os",
	"M9.needs.external": "none",
	"M9.needs.internal": "L0-universal/spiral/identity",
	"M9.used_by":        "all CPI-SI skill CLI commands",
	"M9.layer_deps":     "L0 only — no L1+ dependencies",

	// ── M10: Roadmap ──────────────────────────────────────────────
	"M10.current":     "b-01.00 — BASELINE: standard CLI conventions with identity",
	"M10.planned":     "Subcommand support, completion generation, config file integration",
	"M10.limitations": "No subcommands, no shell completion, no config file loading",
}

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================
//
// Section order: Types → Constants → Variables

// ────────────────────────────────────────────────────────────────
// Types
// ────────────────────────────────────────────────────────────────

// App represents a CLI application with standard flags.
// Wraps flag.FlagSet with CPI-SI conventions: -help/-h, -json, biblical refs.
type App struct {
	Name        string       // command name
	Description string       // one-line description
	Biblical    string       // scripture reference for help output
	flagSet     *flag.FlagSet // underlying flag parser
	helpFlag    *bool        // --help flag
	hFlag       *bool        // -h flag
	jsonFlag    *bool        // --json flag (nil until WithJSON called)
	parsed      bool         // true after Parse/ParseOS succeeds
}

// Usage represents a structured help/usage message.
type Usage struct {
	Name        string         // command name
	Description string         // one-line description
	Biblical    string         // scripture reference
	Sections    []UsageSection // ordered sections
}

// UsageSection is a titled section in usage output.
type UsageSection struct {
	Title string   // section header
	Lines []string // section content lines
}

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================
//
// Section order: Identity → Constructor → Configuration → Parsing → Output → Usage Builder

// ────────────────────────────────────────────────────────────────
// Identity — Package Self-Registration
// ────────────────────────────────────────────────────────────────

// init registers this package in the OmniCode identity registry.
func init() {
	identity.Register(_pragma["P1.key"], _pragma, _metadata)
}

// Pragma returns this package's OmniCode pragma identity (P1-P5).
func Pragma() map[string]string { return _pragma }

// Metadata returns this package's OmniCode metadata (M1-M10).
func Metadata() map[string]string { return _metadata }

// ────────────────────────────────────────────────────────────────
// Constructor
// ────────────────────────────────────────────────────────────────

// New creates a new CLI app with standard -help and -h flags.
func New(name, description string) *App {
	app := &App{
		Name:        name,
		Description: description,
		flagSet:     flag.NewFlagSet(name, flag.ExitOnError),
	}

	// Standard flags — every CPI-SI tool gets these
	app.helpFlag = app.flagSet.Bool("help", false, "Show help")
	app.hFlag = app.flagSet.Bool("h", false, "Show help")

	return app
}

// ────────────────────────────────────────────────────────────────
// Configuration
// ────────────────────────────────────────────────────────────────

// WithBiblical sets the biblical reference for help output.
func (a *App) WithBiblical(ref string) *App {
	a.Biblical = ref
	return a
}

// WithJSON adds a -json flag to the app for JSON output mode.
func (a *App) WithJSON() *App {
	a.jsonFlag = a.flagSet.Bool("json", false, "Output as JSON")
	return a
}

// Flag returns the underlying flag.FlagSet for adding custom flags.
func (a *App) Flag() *flag.FlagSet {
	return a.flagSet
}

// ────────────────────────────────────────────────────────────────
// Parsing
// ────────────────────────────────────────────────────────────────

// Parse parses the given command line arguments.
func (a *App) Parse(args []string) error {
	if err := a.flagSet.Parse(args); err != nil {
		return err
	}
	a.parsed = true
	return nil
}

// ParseOS parses os.Args[1:] — the standard entry point.
func (a *App) ParseOS() error {
	return a.Parse(os.Args[1:])
}

// WantsHelp returns true if -help or -h was passed.
func (a *App) WantsHelp() bool {
	return (a.helpFlag != nil && *a.helpFlag) || (a.hFlag != nil && *a.hFlag)
}

// WantsJSON returns true if -json was passed.
func (a *App) WantsJSON() bool {
	return a.jsonFlag != nil && *a.jsonFlag
}

// Args returns non-flag arguments after parsing.
func (a *App) Args() []string {
	return a.flagSet.Args()
}

// ────────────────────────────────────────────────────────────────
// Output Helpers
// ────────────────────────────────────────────────────────────────

// Fatal prints to stderr and exits with code 1.
func Fatal(format string, args ...any) {
	fmt.Fprintf(os.Stderr, format+"\n", args...)
	os.Exit(1)
}

// FatalCode prints to stderr and exits with the specified code.
func FatalCode(code int, format string, args ...any) {
	fmt.Fprintf(os.Stderr, format+"\n", args...)
	os.Exit(code)
}

// Error prints to stderr without exiting.
func Error(format string, args ...any) {
	fmt.Fprintf(os.Stderr, format+"\n", args...)
}

// Warn prints a warning to stderr with "Warning: " prefix.
func Warn(format string, args ...any) {
	fmt.Fprintf(os.Stderr, "Warning: "+format+"\n", args...)
}

// ────────────────────────────────────────────────────────────────
// Usage Builder
// ────────────────────────────────────────────────────────────────

// NewUsage creates a usage builder from an App's configuration.
func NewUsage(app *App) *Usage {
	return &Usage{
		Name:        app.Name,
		Description: app.Description,
		Biblical:    app.Biblical,
	}
}

// AddSection adds a titled section to the usage output.
func (u *Usage) AddSection(title string, lines ...string) *Usage {
	u.Sections = append(u.Sections, UsageSection{Title: title, Lines: lines})
	return u
}

// Print outputs the formatted usage message to stdout.
func (u *Usage) Print() {
	fmt.Printf("%s - %s\n", u.Name, u.Description)
	fmt.Println()

	for _, section := range u.Sections {
		fmt.Printf("%s:\n", section.Title)
		for _, line := range section.Lines {
			fmt.Printf("  %s\n", line)
		}
		fmt.Println()
	}

	if u.Biblical != "" {
		fmt.Printf("Biblical: %s\n", u.Biblical)
	}
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// ────────────────────────────────────────────────────────────────
// Library Overview
// ────────────────────────────────────────────────────────────────
//
// Lightweight CLI scaffolding for CPI-SI skills. Provides standard
// flag conventions so every tool feels consistent.
//
// ────────────────────────────────────────────────────────────────
// Modification Policy
// ────────────────────────────────────────────────────────────────
//
// Safe to Modify:
//   - Add new App methods (follow builder pattern)
//   - Add new output helpers (Fatal*, Error, Warn variants)
//   - Extend Usage builder with new section types
//
// Modify with Care:
//   - App struct fields — breaks existing callers
//   - Standard flag names (-help, -h, -json) — convention-breaking
//
// Never Modify:
//   - 4-block structure
//   - init() identity registration pattern
//
// ────────────────────────────────────────────────────────────────
// Related Components
// ────────────────────────────────────────────────────────────────
//
// Dependencies:  stdlib only (flag, fmt, os) + L0 identity
// Dependents:    all CPI-SI skill CLI commands
// Parallel:      util/output (formatting), util/format (values)
//
// "The heart of the righteous studieth to answer." — Proverbs 15:28
//
// ============================================================================
// END CLOSING
// ============================================================================
