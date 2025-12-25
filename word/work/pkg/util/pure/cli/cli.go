// ============================================================================
// METADATA
// ============================================================================
//
// Key: claude-global-pkg-cli
// Purpose: Shared CLI infrastructure for skill commands
// Biblical: Proverbs 15:28 - "The heart of the righteous studieth to answer"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2025-12-10
//
// Used by: All skill CLI commands
//
// ============================================================================

package cli

// ============================================================================
// SETUP
// ============================================================================

import (
	"flag"
	"fmt"
	"os"
)

// App represents a CLI application with standard flags
type App struct {
	Name        string
	Description string
	Biblical    string
	flagSet     *flag.FlagSet
	helpFlag    *bool
	hFlag       *bool
	jsonFlag    *bool
	parsed      bool
}

// ============================================================================
// BODY
// ============================================================================

// --- Constructor ---

// New creates a new CLI app with standard flags
func New(name, description string) *App {
	app := &App{
		Name:        name,
		Description: description,
		flagSet:     flag.NewFlagSet(name, flag.ExitOnError),
	}

	// Standard flags
	app.helpFlag = app.flagSet.Bool("help", false, "Show help")
	app.hFlag = app.flagSet.Bool("h", false, "Show help")

	return app
}

// --- Configuration ---

// WithBiblical sets the biblical reference for help output
func (a *App) WithBiblical(ref string) *App {
	a.Biblical = ref
	return a
}

// WithJSON adds a -json flag to the app
func (a *App) WithJSON() *App {
	a.jsonFlag = a.flagSet.Bool("json", false, "Output as JSON")
	return a
}

// Flag adds a custom flag
func (a *App) Flag() *flag.FlagSet {
	return a.flagSet
}

// --- Parsing ---

// Parse parses command line arguments
func (a *App) Parse(args []string) error {
	if err := a.flagSet.Parse(args); err != nil {
		return err
	}
	a.parsed = true
	return nil
}

// ParseOS parses os.Args[1:]
func (a *App) ParseOS() error {
	return a.Parse(os.Args[1:])
}

// WantsHelp returns true if help was requested
func (a *App) WantsHelp() bool {
	return (a.helpFlag != nil && *a.helpFlag) || (a.hFlag != nil && *a.hFlag)
}

// WantsJSON returns true if JSON output was requested
func (a *App) WantsJSON() bool {
	return a.jsonFlag != nil && *a.jsonFlag
}

// Args returns non-flag arguments
func (a *App) Args() []string {
	return a.flagSet.Args()
}

// --- Output Helpers ---

// Fatal prints to stderr and exits with code 1
func Fatal(format string, args ...interface{}) {
	fmt.Fprintf(os.Stderr, format+"\n", args...)
	os.Exit(1)
}

// FatalCode prints to stderr and exits with specified code
func FatalCode(code int, format string, args ...interface{}) {
	fmt.Fprintf(os.Stderr, format+"\n", args...)
	os.Exit(code)
}

// Error prints to stderr without exiting
func Error(format string, args ...interface{}) {
	fmt.Fprintf(os.Stderr, format+"\n", args...)
}

// Warn prints a warning to stderr
func Warn(format string, args ...interface{}) {
	fmt.Fprintf(os.Stderr, "Warning: "+format+"\n", args...)
}

// --- Usage Builder ---

// Usage represents a help/usage message
type Usage struct {
	Name        string
	Description string
	Biblical    string
	Sections    []UsageSection
}

// UsageSection is a section in the usage output
type UsageSection struct {
	Title string
	Lines []string
}

// NewUsage creates a new usage builder from an App
func NewUsage(app *App) *Usage {
	return &Usage{
		Name:        app.Name,
		Description: app.Description,
		Biblical:    app.Biblical,
	}
}

// AddSection adds a section to the usage
func (u *Usage) AddSection(title string, lines ...string) *Usage {
	u.Sections = append(u.Sections, UsageSection{Title: title, Lines: lines})
	return u
}

// Print outputs the usage message
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
// CLOSING
// ============================================================================
