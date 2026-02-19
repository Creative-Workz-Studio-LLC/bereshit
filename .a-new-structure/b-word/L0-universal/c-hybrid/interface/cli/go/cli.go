//omni:code --go -library
//omni:key B-L0-hybrid-cli
//omni:version b-03.00

// ============================================================================
// METADATA
// ============================================================================
//
// Key:     B-L0-hybrid-cli
// Purpose: Lightweight CLI scaffolding for CPI-SI skill commands
// Biblical: Proverbs 15:28 — The heart of the righteous studieth to answer
// Authors: Nova Dawn
// Version: 3.0.0
// Created: 2025-12-10
// Updated: 2026-02-18
//
// Wraps Go's flag package with CPI-SI conventions:
// standard flags (-help/-h, -json, -version/-V), biblical references in help,
// structured usage builder with String() output. Pure stdlib.
//
// Production QoL: version flag support, convenience accessors (FirstArg,
// HasArgs, ArgCount), standard flags auto-generation in usage output,
// and fmt.Stringer on Usage for flexible output.

// Package cli provides lightweight CLI scaffolding for CPI-SI skill commands.
//
// Wraps Go's flag package with standard conventions: -help/-h flags,
// -version/-V flags, -json output mode, biblical references in help text,
// and structured usage builders.
//
// Pure stdlib — no external dependencies beyond identity registration.
//
// # Usage
//
//	app := cli.New("mytool", "Does something useful")
//	app.WithBiblical("Proverbs 15:28").WithVersion("1.0.0")
//	app.ParseOS()
//	if app.WantsHelp() { cli.NewUsage(app).WithStandardFlags().Print(); return }
//	if app.WantsVersion() { fmt.Println(app.Version()); return }
//
// # OmniCode Identity
//
//	Variable    Sections  Purpose                          Query
//	Pragma      I1-I4     Identity — who this file is      PragmaGet
//	Metadata    C1-C7     Context — when, where, why, how  MetadataGet
package cli

// ──────────────────────────────────────────────────────────────────────────
// Imports (in METADATA — Go requires all imports before any declarations)
// ──────────────────────────────────────────────────────────────────────────

import (
	"flag"
	"fmt"
	"os"
	"strings"
)

import (
	"creativeworkzstudio.com/bereshit/L0-universal/b-spiral/identity"
)

// ──────────────────────────────────────────────────────────────────────────
// Identity (I1-I4)
// ──────────────────────────────────────────────────────────────────────────

// Pragma carries the OmniCode identity sections (I1-I4) for this package.
//
// These fields establish WHO this file is in the project graph.
// Initialized once at package load, read-only thereafter.
//
// # Sections
//
//	Section  Purpose       Required                 Defined
//	I1       Core          key, format, from, at    —
//	I2       Family        type, structure          subtype, role
//	I3       Instance      file, title              component, path, provides, brief
//	I4       Architecture  —                        layer, position, pattern
//
// # Example
//
//	key, _ := PragmaGet("I1.key") // returns "B-L0-hybrid-cli"
var Pragma = [][2]string{
	// I1: Core — key, format, from, at
	{"I1.key", "B-L0-hybrid-cli"},
	{"I1.format", "go"},
	{"I1.from", "b-word/seed/code/L0/go/library.go"},
	{"I1.at", "b-03.00"},
	// I2: Family — type, structure, subtype, role
	{"I2.type", "code"},
	{"I2.structure", "4-block"},
	{"I2.subtype", "library"},
	{"I2.role", "interface"},
	// I3: Instance — file, title, component, path, provides, brief
	{"I3.file", "cli.go"},
	{"I3.title", "CLI Scaffolding Library"},
	{"I3.component", "c-hybrid/interface/cli"},
	{"I3.path", "L0-universal/c-hybrid/interface/cli/go/cli.go"},
	{"I3.provides", "CLI_SCAFFOLDING"},
	{"I3.brief", "Flag parsing, help output, usage builder, version support for CPI-SI skills"},
	// I4: Architecture — layer, position, pattern
	{"I4.layer", "L0"},
	{"I4.position", "hybrid/interface/cli"},
	{"I4.pattern", "builder — App.WithX().ParseOS() → query results"},
}

// ──────────────────────────────────────────────────────────────────────────
// Context (C1-C7)
// ──────────────────────────────────────────────────────────────────────────

// Metadata carries the OmniCode context sections (C1-C7) for this package.
//
// These fields establish WHEN, WHERE, WHY, and HOW for this file.
// Initialized once at package load, read-only thereafter.
//
// # Sections
//
//	Section  Purpose         Required             Defined
//	C1       State           version, status      created, updated
//	C2       Attribution     organization         architect, implementation, copyright
//	C3       Grounding       scripture            principle, anchor
//	C4       Dependencies    requires, consumers  integration, if_missing
//	C5       Intent          —                    purpose, philosophy
//	C6       Roadmap         —                    current, planned, limitations
//	C7       Classification  —                    tags, category, domain, paradigm
//
// # Example
//
//	ver, _ := MetadataGet("C1.version") // returns "b-03.00"
var Metadata = [][2]string{
	// C1: State — version, status, created, updated
	{"C1.version", "b-03.00"},
	{"C1.status", "Active"},
	{"C1.created", "2025-12-10"},
	{"C1.updated", "2026-02-18"},
	// C2: Attribution
	{"C2.organization", "CreativeWorkzStudio LLC"},
	{"C2.architect", "Seanje Lenox-Wise"},
	{"C2.implementation", "Nova Dawn (CPI-SI)"},
	{"C2.copyright", "2025-2026 CreativeWorkzStudio LLC"},
	// C3: Grounding
	{"C3.scripture", "Proverbs 15:28 — The heart of the righteous studieth to answer"},
	{"C3.principle", "Study before answering — parse input fully before acting"},
	{"C3.anchor", "Genesis 1:1 — In the beginning God created the heaven and the earth"},
	// C4: Dependencies
	{"C4.requires.stdlib", "flag, fmt, os, strings"},
	{"C4.requires.external", "none"},
	{"C4.requires.internal", "L0-universal/b-spiral/identity"},
	{"C4.consumers", "all CPI-SI skill CLI commands"},
	{"C4.integration", "import creativeworkzstudio.com/bereshit/L0-universal/c-hybrid/interface/cli/go"},
	{"C4.if_missing", "skill CLIs lack standard flag conventions and help output"},
	// C5: Intent
	{"C5.purpose", "Standard CLI conventions for all CPI-SI skill commands"},
	{"C5.philosophy", "Convention over configuration — consistent flags across all tools"},
	// C6: Roadmap
	{"C6.current", "b-03.00 — Version flag, convenience accessors, standard flags, String() on Usage"},
	{"C6.planned", "Subcommand support, custom flag types, shell completion generation"},
	{"C6.limitations", "No subcommands, no shell completion, no config file loading"},
	// C7: Classification
	{"C7.tags", "cli, flags, usage, pure, stdlib-only, interface, omnicode, cpi-si"},
	{"C7.category", "Interface"},
	{"C7.domain", "cli-infrastructure"},
	{"C7.paradigm", "CPI-SI"},
}

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================
//
// section-order: [s01]-imports, [s07]-types
// (imports declared in METADATA section — Go language constraint)

// ──────────────────────────────────────────────────────────────────────────
// s07 — Types
// ──────────────────────────────────────────────────────────────────────────

// App represents a CLI application with standard CPI-SI flags.
//
// Builder pattern: New(name, desc) → WithX() → ParseOS() → query results.
// Wraps [flag.FlagSet] with conventions: -help/-h, -version/-V, -json,
// biblical references in help text.
//
// Example:
//
//	app := cli.New("mytool", "Does something useful")
//	app.WithBiblical("Proverbs 15:28").WithVersion("1.0.0").WithJSON()
//	app.ParseOS()
//	if app.WantsHelp() { ... }
type App struct {
	Name        string        // command name
	Description string        // one-line description
	Biblical    string        // scripture reference for help output
	flagSet     *flag.FlagSet // underlying flag parser
	helpFlag    *bool         // --help flag
	hFlag       *bool         // -h flag
	jsonFlag    *bool         // --json flag (nil until WithJSON called)
	versionFlag *bool         // --version flag (nil until WithVersion called)
	vFlag       *bool         // -V flag (nil until WithVersion called)
	version     string        // version string (empty until WithVersion called)
	parsed      bool          // true after Parse/ParseOS succeeds
}

// Usage represents a structured help/usage message.
//
// Builder pattern: NewUsage(app) → AddSection() → WithStandardFlags() → Print().
// Implements [fmt.Stringer] for flexible output.
type Usage struct {
	Name        string         // command name
	Description string         // one-line description
	Biblical    string         // scripture reference
	Version     string         // version string (empty if not set)
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

// ──────────────────────────────────────────────────────────────────────────
// 1. Org Chart
// ──────────────────────────────────────────────────────────────────────────
//
// Identity:
//   init, PragmaGet, MetadataGet
//
// Constructors:
//   New
//
// Configuration (builder methods):
//   WithBiblical, WithVersion, WithJSON, Flag
//
// Parsing:
//   Parse, ParseOS
//
// Queries:
//   WantsHelp, WantsVersion, WantsJSON, Version,
//   Args, FirstArg, HasArgs, ArgCount
//
// Output (package-level):
//   Fatal, FatalCode, Error, Warn
//
// Usage Builder:
//   NewUsage, AddSection, WithStandardFlags, WithVersion, Print, String
//
// APUs: 24 functions total
//   - 0 helpers
//   - 9 core operations (init, PragmaGet, MetadataGet, New,
//     WithBiblical, WithVersion, WithJSON, Flag, Parse/ParseOS)
//   - 15 public APIs (queries, output, usage builder)

// ──────────────────────────────────────────────────────────────────────────
// 2. Helpers
// ──────────────────────────────────────────────────────────────────────────
// [Reserved: No internal helpers — flag package handles all parsing logic]

// ──────────────────────────────────────────────────────────────────────────
// 3. Core Operations
// ──────────────────────────────────────────────────────────────────────────

// --- Identity Access ---

// init registers this package in the OmniCode identity registry.
func init() {
	key, ok := PragmaGet("I1.key")
	if !ok {
		return
	}
	identity.Register(key, Pragma, Metadata)
}

// PragmaGet looks up a value from the Pragma identity.
// Returns the value and true if found, empty string and false otherwise.
func PragmaGet(key string) (string, bool) {
	for _, pair := range Pragma {
		if pair[0] == key {
			return pair[1], true
		}
	}
	return "", false
}

// MetadataGet looks up a value from the Metadata.
// Returns the value and true if found, empty string and false otherwise.
func MetadataGet(key string) (string, bool) {
	for _, pair := range Metadata {
		if pair[0] == key {
			return pair[1], true
		}
	}
	return "", false
}

// --- Constructors ---

// New creates a new CLI app with standard -help and -h flags.
//
// Every CPI-SI skill CLI starts here. Use builder methods to configure:
//
//	app := cli.New("mytool", "Does something useful")
//	app.WithBiblical("Proverbs 15:28").WithVersion("1.0.0")
//	app.ParseOS()
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

// --- Configuration ---

// WithBiblical sets the biblical reference for help output.
func (a *App) WithBiblical(ref string) *App {
	a.Biblical = ref
	return a
}

// WithVersion enables --version and -V flags with the given version string.
//
//	app.WithVersion("1.0.0")
//	app.ParseOS()
//	if app.WantsVersion() { fmt.Println(app.Version()) }
func (a *App) WithVersion(version string) *App {
	a.version = version
	a.versionFlag = a.flagSet.Bool("version", false, "Show version")
	a.vFlag = a.flagSet.Bool("V", false, "Show version")
	return a
}

// WithJSON adds a --json flag to the app for JSON output mode.
func (a *App) WithJSON() *App {
	a.jsonFlag = a.flagSet.Bool("json", false, "Output as JSON")
	return a
}

// Flag returns the underlying [flag.FlagSet] for adding custom flags.
func (a *App) Flag() *flag.FlagSet {
	return a.flagSet
}

// --- Parsing ---

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

// ──────────────────────────────────────────────────────────────────────────
// 4. Error Handling
// ──────────────────────────────────────────────────────────────────────────
// [Reserved: Uses standard error returns from Parse(). No custom recovery.
// flag.ExitOnError handles parse errors by calling os.Exit(2).]

// ──────────────────────────────────────────────────────────────────────────
// 5. Public APIs
// ──────────────────────────────────────────────────────────────────────────

// --- Queries ---

// WantsHelp returns true if --help or -h was passed.
func (a *App) WantsHelp() bool {
	return (a.helpFlag != nil && *a.helpFlag) || (a.hFlag != nil && *a.hFlag)
}

// WantsVersion returns true if --version or -V was passed.
// Returns false if [App.WithVersion] was never called.
func (a *App) WantsVersion() bool {
	return (a.versionFlag != nil && *a.versionFlag) || (a.vFlag != nil && *a.vFlag)
}

// WantsJSON returns true if --json was passed.
// Returns false if [App.WithJSON] was never called.
func (a *App) WantsJSON() bool {
	return a.jsonFlag != nil && *a.jsonFlag
}

// Version returns the version string set by [App.WithVersion].
// Returns empty string if WithVersion was never called.
func (a *App) Version() string {
	return a.version
}

// Args returns non-flag arguments after parsing.
func (a *App) Args() []string {
	return a.flagSet.Args()
}

// FirstArg returns the first non-flag argument and true, or empty string
// and false if no arguments were provided.
//
//	arg, ok := app.FirstArg()
//	if !ok { cli.Fatal("missing required argument") }
func (a *App) FirstArg() (string, bool) {
	args := a.flagSet.Args()
	if len(args) == 0 {
		return "", false
	}
	return args[0], true
}

// HasArgs returns true if any non-flag arguments were provided.
func (a *App) HasArgs() bool {
	return len(a.flagSet.Args()) > 0
}

// ArgCount returns the number of non-flag arguments.
func (a *App) ArgCount() int {
	return len(a.flagSet.Args())
}

// --- Output Helpers ---

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

// --- Usage Builder ---

// NewUsage creates a usage builder from an App's configuration.
//
//	usage := cli.NewUsage(app)
//	usage.AddSection("USAGE", "mytool [options] <file>")
//	usage.WithStandardFlags()
//	usage.Print()
func NewUsage(app *App) *Usage {
	return &Usage{
		Name:        app.Name,
		Description: app.Description,
		Biblical:    app.Biblical,
		Version:     app.version,
	}
}

// AddSection adds a titled section to the usage output.
func (u *Usage) AddSection(title string, lines ...string) *Usage {
	u.Sections = append(u.Sections, UsageSection{Title: title, Lines: lines})
	return u
}

// WithStandardFlags adds a FLAGS section with all standard CPI-SI flags.
//
// Includes: -h/--help (always), -V/--version (if version set),
// --json (always shown as optional).
// Call after [Usage.AddSection] for custom flags to keep FLAGS last.
func (u *Usage) WithStandardFlags() *Usage {
	lines := []string{
		"-h, --help       Show this help message",
	}
	if u.Version != "" {
		lines = append(lines, "-V, --version    Show version")
	}
	lines = append(lines, "    --json       Output as JSON (if supported)")
	return u.AddSection("FLAGS", lines...)
}

// WithVersion sets the version string displayed in the usage header.
//
//	usage.WithVersion("1.0.0")
//	// Header: "mytool 1.0.0 - Does something useful"
func (u *Usage) WithVersion(version string) *Usage {
	u.Version = version
	return u
}

// Print outputs the formatted usage message to stdout.
func (u *Usage) Print() {
	fmt.Print(u.String())
}

// String implements [fmt.Stringer] — returns the formatted usage message.
//
// Format:
//
//	mytool 1.0.0 - Does something useful
//
//	USAGE:
//	  mytool [options] <file>
//
//	FLAGS:
//	  -h, --help       Show this help message
//
//	Biblical: Proverbs 15:28
func (u *Usage) String() string {
	var b strings.Builder

	// Header line
	if u.Version != "" {
		fmt.Fprintf(&b, "%s %s - %s\n", u.Name, u.Version, u.Description)
	} else {
		fmt.Fprintf(&b, "%s - %s\n", u.Name, u.Description)
	}
	b.WriteString("\n")

	// Sections
	for _, section := range u.Sections {
		fmt.Fprintf(&b, "%s:\n", section.Title)
		for _, line := range section.Lines {
			fmt.Fprintf(&b, "  %s\n", line)
		}
		b.WriteString("\n")
	}

	// Biblical anchor
	if u.Biblical != "" {
		fmt.Fprintf(&b, "Biblical: %s\n", u.Biblical)
	}

	return b.String()
}

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
// Testing: go test -v ./L0-universal/c-hybrid/interface/cli/go/
//
// Verify:
//   - New() creates App with name, description, help flags
//   - WithVersion() enables --version/-V flags
//   - WithJSON() enables --json flag
//   - Parse() processes args correctly
//   - All query methods return correct values after parsing
//   - FirstArg/HasArgs/ArgCount convenience accessors work
//   - Usage builder produces correct formatted output
//   - String() on Usage matches Print() output
//   - PragmaGet/MetadataGet return correct values for known keys
//   - PragmaGet/MetadataGet return false for missing keys
//
// See cli_test.go for full test suite (35 tests).
//
// ──────────────────────────────────────────────────────────────────────────
// Ce — Execution
// ──────────────────────────────────────────────────────────────────────────
//
// Library — imported, not executed.
// import "creativeworkzstudio.com/bereshit/L0-universal/c-hybrid/interface/cli/go"
//
// Identity registered via init() on import.
//
// ──────────────────────────────────────────────────────────────────────────
// Cc — Cleanup
// ──────────────────────────────────────────────────────────────────────────
//
// No resources to clean up. flag.FlagSet is garbage collected normally.
//
// ──────────────────────────────────────────────────────────────────────────
// X1: Policy
// ──────────────────────────────────────────────────────────────────────────
//
// Never:
//   - Remove standard flag conventions (-help, -h)
//   - Add heavy dependencies (this stays stdlib + identity only)
//   - Break the builder pattern (New → WithX → Parse → Query)
//   - Remove init() identity registration
//
// Careful:
//   - App struct field changes — breaks existing callers
//   - Standard flag names (-help, -h, -json, -version, -V)
//   - New() default behavior — all CPI-SI skills depend on this
//
// Safe:
//   - Add new App builder methods (WithX pattern)
//   - Add new query methods on App
//   - Add new Usage sections and output helpers
//   - Add new output helpers (Fatal/Error/Warn variants)
//
// ──────────────────────────────────────────────────────────────────────────
// X2: Extension
// ──────────────────────────────────────────────────────────────────────────
//
// Completed:
//   - b-01.00: Standard CLI conventions with identity
//   - b-01.01: Promoted from hybrid/util/ to hybrid/ top-level domain
//   - b-03.00: I/C metadata, version flag, convenience accessors,
//     standard flags, String() on Usage
//
// Designed:
//   - Subcommand support (App.Subcommand(name, desc) → sub-App)
//   - Custom flag types (string, integer, enum)
//   - Shell completion generation (bash, zsh, fish)
//
// Future:
//   - Config file integration (auto-load from config dir)
//   - C ABI bridge for libtrit integration
//
// ──────────────────────────────────────────────────────────────────────────
// X3: Troubleshooting
// ──────────────────────────────────────────────────────────────────────────
//
// "If any of you lack wisdom, let him ask of God." — James 1:5
//
// Common Issues:
//   - flag already defined: Two packages registering same flag name on
//     same FlagSet. Use Flag() to access underlying FlagSet and check.
//   - help not working: Ensure ParseOS() called before WantsHelp() check.
//   - version empty: Ensure WithVersion() called before ParseOS().
//
// ──────────────────────────────────────────────────────────────────────────
// X5: Note
// ──────────────────────────────────────────────────────────────────────────
//
// Note: CLI scaffolding for CPI-SI skills. Convention over configuration.
// Every tool feels consistent because they share the same infrastructure.
// Cross-language parity with Rust bereshit-l0-cli crate.
//
// Scripture: Proverbs 15:28 — "The heart of the righteous studieth to
// answer: but the mouth of the wicked poureth out evil things."
//
// Study before answering. Parse before acting. The CLI IS this principle.
//
// ============================================================================
// END CLOSING
// ============================================================================
