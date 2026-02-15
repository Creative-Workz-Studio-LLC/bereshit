//omni:code --go -test
//omni:key B-L2-display-test
//omni:version a-01.00

// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: B-L2-display-test
// Purpose: Unit tests for terminal display colors and formatting
// Biblical: Proverbs 25:11 — "A word fitly spoken is like apples of gold"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-15
//
// ═══════════════════════════════════════════════════════════════════════════

package display

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"strings"
	"testing"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// ───────────────────────────────────────────────────────────────────────────
// Constants — verify ANSI codes are non-empty and correctly structured
// ───────────────────────────────────────────────────────────────────────────

func TestResetCode(t *testing.T) {
	if Reset != "\033[0m" {
		t.Errorf("Reset = %q, want %q", Reset, "\033[0m")
	}
}

func TestColorConstants(t *testing.T) {
	colors := map[string]string{
		"Red":     Red,
		"Green":   Green,
		"Yellow":  Yellow,
		"Blue":    Blue,
		"Magenta": Magenta,
		"Cyan":    Cyan,
		"White":   White,
		"Gray":    Gray,
	}

	for name, code := range colors {
		if code == "" {
			t.Errorf("%s color is empty", name)
		}
		if !strings.HasPrefix(code, "\033[") {
			t.Errorf("%s color %q should start with ESC[", name, code)
		}
	}
}

func TestStyleConstants(t *testing.T) {
	if Bold == "" {
		t.Error("Bold should not be empty")
	}
	if Dim == "" {
		t.Error("Dim should not be empty")
	}
	if Italic == "" {
		t.Error("Italic should not be empty")
	}
	if Underline == "" {
		t.Error("Underline should not be empty")
	}
}

// ───────────────────────────────────────────────────────────────────────────
// Colorize — verify wrapping with color and reset
// ───────────────────────────────────────────────────────────────────────────

func TestColorize(t *testing.T) {
	result := Colorize("hello", Red)
	if !strings.HasPrefix(result, Red) {
		t.Error("Colorize should start with color code")
	}
	if !strings.HasSuffix(result, Reset) {
		t.Error("Colorize should end with reset")
	}
	if !strings.Contains(result, "hello") {
		t.Error("Colorize should contain the text")
	}
}

func TestBoldColorize(t *testing.T) {
	result := BoldColorize("test", Green)
	if !strings.HasPrefix(result, Bold) {
		t.Error("BoldColorize should start with bold")
	}
	if !strings.Contains(result, Green) {
		t.Error("BoldColorize should contain color code")
	}
	if !strings.HasSuffix(result, Reset) {
		t.Error("BoldColorize should end with reset")
	}
}

func TestDimText(t *testing.T) {
	result := DimText("faded")
	if !strings.HasPrefix(result, Dim) {
		t.Error("DimText should start with dim code")
	}
	if !strings.HasSuffix(result, Reset) {
		t.Error("DimText should end with reset")
	}
}

// ───────────────────────────────────────────────────────────────────────────
// Semantic helpers — Success/Warning/Error/Info
// ───────────────────────────────────────────────────────────────────────────

func TestSuccess(t *testing.T) {
	result := Success("ok")
	if !strings.Contains(result, Green) {
		t.Error("Success should use green")
	}
}

func TestWarning(t *testing.T) {
	result := Warning("caution")
	if !strings.Contains(result, Yellow) {
		t.Error("Warning should use yellow")
	}
}

func TestError(t *testing.T) {
	result := Error("fail")
	if !strings.Contains(result, Red) {
		t.Error("Error should use red")
	}
}

func TestInfo(t *testing.T) {
	result := Info("note")
	if !strings.Contains(result, Cyan) {
		t.Error("Info should use cyan")
	}
}

// ───────────────────────────────────────────────────────────────────────────
// Empty text — verify graceful handling
// ───────────────────────────────────────────────────────────────────────────

func TestColorizeEmptyString(t *testing.T) {
	result := Colorize("", Red)
	expected := Red + Reset
	if result != expected {
		t.Errorf("Colorize empty: got %q, want %q", result, expected)
	}
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
