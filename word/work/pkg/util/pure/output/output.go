// ============================================================================
// METADATA
// ============================================================================
//
// Key: claude-global-pkg-output
// Purpose: Shared output formatting utilities
// Biblical: Colossians 4:6 - "Let your speech be alway with grace"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2025-12-10
//
// Used by: All skill CLI commands for formatted output
//
// ============================================================================

package output

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt"
	"strings"
)

// ============================================================================
// BODY
// ============================================================================

// --- Headers and Sections ---

// Header prints a title with double-line separator
func Header(title string) {
	width := len(title)
	if width < 40 {
		width = 40
	}
	fmt.Println(title)
	fmt.Println(strings.Repeat("=", width))
	fmt.Println()
}

// Section prints a section title with single-line separator
func Section(title string) {
	fmt.Println(title)
	fmt.Println(strings.Repeat("-", len(title)))
	fmt.Println()
}

// SubSection prints a subsection title
func SubSection(title string) {
	fmt.Printf("## %s\n\n", title)
}

// Divider prints a line of characters
func Divider(char string, width int) {
	fmt.Println(strings.Repeat(char, width))
}

// --- Formatted Output ---

// KeyValue prints a key-value pair with alignment
func KeyValue(key string, value interface{}) {
	fmt.Printf("%-12s %v\n", key+":", value)
}

// KeyValueWidth prints a key-value pair with custom key width
func KeyValueWidth(key string, value interface{}, width int) {
	format := fmt.Sprintf("%%-%ds %%v\n", width)
	fmt.Printf(format, key+":", value)
}

// Bullet prints a bulleted item
func Bullet(text string) {
	fmt.Printf("  - %s\n", text)
}

// Numbered prints a numbered item
func Numbered(n int, text string) {
	fmt.Printf("  %d. %s\n", n, text)
}

// Indent prints text with indentation
func Indent(text string, level int) {
	prefix := strings.Repeat("  ", level)
	fmt.Printf("%s%s\n", prefix, text)
}

// --- Tables ---

// TableHeader prints a table header row
func TableHeader(columns ...string) {
	var widths []int
	for _, col := range columns {
		widths = append(widths, len(col)+2)
	}
	TableRow(widths, columns...)
	var dashes []string
	for _, w := range widths {
		dashes = append(dashes, strings.Repeat("-", w))
	}
	fmt.Println(strings.Join(dashes, " "))
}

// TableRow prints a table row with specified column widths
func TableRow(widths []int, values ...string) {
	var parts []string
	for i, v := range values {
		width := 10
		if i < len(widths) {
			width = widths[i]
		}
		format := fmt.Sprintf("%%-%ds", width)
		parts = append(parts, fmt.Sprintf(format, v))
	}
	fmt.Println(strings.Join(parts, " "))
}

// --- Status and Indicators ---

// Success prints a success message with checkmark
func Success(format string, args ...interface{}) {
	fmt.Printf("OK "+format+"\n", args...)
}

// Info prints an info message
func Info(format string, args ...interface{}) {
	fmt.Printf("INFO "+format+"\n", args...)
}

// Tip prints a tip/guidance message
func Tip(format string, args ...interface{}) {
	fmt.Printf("TIP "+format+"\n", args...)
}

// --- JSON Helpers ---

// JSONStart starts a JSON object
func JSONStart() {
	fmt.Println("{")
}

// JSONEnd ends a JSON object
func JSONEnd() {
	fmt.Println("}")
}

// JSONField prints a JSON field (with trailing comma)
func JSONField(key string, value interface{}) {
	switch v := value.(type) {
	case string:
		fmt.Printf("  %q: %q,\n", key, v)
	case bool:
		fmt.Printf("  %q: %t,\n", key, v)
	case int, int64, float64:
		fmt.Printf("  %q: %v,\n", key, v)
	default:
		fmt.Printf("  %q: %v,\n", key, v)
	}
}

// JSONFieldLast prints a JSON field (without trailing comma)
func JSONFieldLast(key string, value interface{}) {
	switch v := value.(type) {
	case string:
		fmt.Printf("  %q: %q\n", key, v)
	case bool:
		fmt.Printf("  %q: %t\n", key, v)
	case int, int64, float64:
		fmt.Printf("  %q: %v\n", key, v)
	default:
		fmt.Printf("  %q: %v\n", key, v)
	}
}

// JSONObjectStart starts a nested JSON object
func JSONObjectStart(key string) {
	fmt.Printf("  %q: {\n", key)
}

// JSONObjectEnd ends a nested JSON object (with trailing comma)
func JSONObjectEnd() {
	fmt.Println("  },")
}

// JSONObjectEndLast ends a nested JSON object (without trailing comma)
func JSONObjectEndLast() {
	fmt.Println("  }")
}

// ============================================================================
// CLOSING
// ============================================================================
