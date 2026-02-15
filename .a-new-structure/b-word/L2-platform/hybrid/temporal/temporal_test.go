//omni:code --go -test
//omni:key B-L2-temporal-test
//omni:version a-01.00

// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: B-L2-temporal-test
// Purpose: Unit tests for temporal awareness — time-of-day, session phase, circadian
// Biblical: Ecclesiastes 3:1 — "To every thing there is a season"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-15
//
// ═══════════════════════════════════════════════════════════════════════════

package temporal

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"testing"
	"time"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// ───────────────────────────────────────────────────────────────────────────
// TimeOfDay — verify all hour ranges classify correctly
// ───────────────────────────────────────────────────────────────────────────

func makeTime(hour int) time.Time {
	return time.Date(2026, 2, 15, hour, 30, 0, 0, time.UTC)
}

func TestTimeOfDayMorning(t *testing.T) {
	tod := GetTimeOfDay(makeTime(6))
	if tod.Name != "morning" {
		t.Errorf("hour 6: got %q, want %q", tod.Name, "morning")
	}
}

func TestTimeOfDayMidMorning(t *testing.T) {
	tod := GetTimeOfDay(makeTime(10))
	if tod.Name != "mid-morning" {
		t.Errorf("hour 10: got %q, want %q", tod.Name, "mid-morning")
	}
}

func TestTimeOfDayMidday(t *testing.T) {
	tod := GetTimeOfDay(makeTime(12))
	if tod.Name != "midday" {
		t.Errorf("hour 12: got %q, want %q", tod.Name, "midday")
	}
}

func TestTimeOfDayAfternoon(t *testing.T) {
	tod := GetTimeOfDay(makeTime(15))
	if tod.Name != "afternoon" {
		t.Errorf("hour 15: got %q, want %q", tod.Name, "afternoon")
	}
}

func TestTimeOfDayEvening(t *testing.T) {
	tod := GetTimeOfDay(makeTime(18))
	if tod.Name != "evening" {
		t.Errorf("hour 18: got %q, want %q", tod.Name, "evening")
	}
}

func TestTimeOfDayNight(t *testing.T) {
	tod := GetTimeOfDay(makeTime(21))
	if tod.Name != "night" {
		t.Errorf("hour 21: got %q, want %q", tod.Name, "night")
	}
}

func TestTimeOfDayLateNight(t *testing.T) {
	for _, hour := range []int{0, 2, 4, 23} {
		tod := GetTimeOfDay(makeTime(hour))
		if tod.Name != "late-night" {
			t.Errorf("hour %d: got %q, want %q", hour, tod.Name, "late-night")
		}
	}
}

func TestTimeOfDayHasIcon(t *testing.T) {
	tod := GetTimeOfDay(makeTime(10))
	if tod.Icon == "" {
		t.Error("TimeOfDay should have an icon")
	}
}

func TestTimeOfDayHasColor(t *testing.T) {
	tod := GetTimeOfDay(makeTime(10))
	if tod.Color == "" {
		t.Error("TimeOfDay should have a color")
	}
}

// ───────────────────────────────────────────────────────────────────────────
// SessionPhase — verify duration classifications
// ───────────────────────────────────────────────────────────────────────────

func TestSessionPhaseFresh(t *testing.T) {
	phase := GetSessionPhase(5 * time.Minute)
	if phase.Name != "fresh" {
		t.Errorf("5min: got %q, want %q", phase.Name, "fresh")
	}
}

func TestSessionPhaseActive(t *testing.T) {
	phase := GetSessionPhase(30 * time.Minute)
	if phase.Name != "active" {
		t.Errorf("30min: got %q, want %q", phase.Name, "active")
	}
}

func TestSessionPhaseSustained(t *testing.T) {
	phase := GetSessionPhase(90 * time.Minute)
	if phase.Name != "sustained" {
		t.Errorf("90min: got %q, want %q", phase.Name, "sustained")
	}
}

func TestSessionPhaseLong(t *testing.T) {
	phase := GetSessionPhase(180 * time.Minute)
	if phase.Name != "long" {
		t.Errorf("180min: got %q, want %q", phase.Name, "long")
	}
}

func TestSessionPhaseExtended(t *testing.T) {
	phase := GetSessionPhase(300 * time.Minute)
	if phase.Name != "extended" {
		t.Errorf("300min: got %q, want %q", phase.Name, "extended")
	}
}

func TestSessionPhaseDisplayZeroTime(t *testing.T) {
	d := GetSessionPhaseDisplay(time.Time{})
	if d.HasInfo {
		t.Error("zero time should yield HasInfo=false")
	}
}

// ───────────────────────────────────────────────────────────────────────────
// Circadian — verify phases across the day
// ───────────────────────────────────────────────────────────────────────────

func TestCircadianPeak(t *testing.T) {
	c := GetCircadianPhase(10)
	if c.Name != "peak" {
		t.Errorf("hour 10: got %q, want %q", c.Name, "peak")
	}
	if c.Productivity != "high" {
		t.Errorf("peak productivity: got %q, want %q", c.Productivity, "high")
	}
}

func TestCircadianRest(t *testing.T) {
	c := GetCircadianPhase(23)
	if c.Name != "rest" {
		t.Errorf("hour 23: got %q, want %q", c.Name, "rest")
	}
}

func TestCircadianRising(t *testing.T) {
	c := GetCircadianPhase(7)
	if c.Name != "rising" {
		t.Errorf("hour 7: got %q, want %q", c.Name, "rising")
	}
}

func TestCircadianDip(t *testing.T) {
	c := GetCircadianPhase(13)
	if c.Name != "dip" {
		t.Errorf("hour 13: got %q, want %q", c.Name, "dip")
	}
}

// ───────────────────────────────────────────────────────────────────────────
// Duration Formatting — verify human-readable output
// ───────────────────────────────────────────────────────────────────────────

func TestFormatDurationMinutesOnly(t *testing.T) {
	s := FormatDuration(45 * time.Minute)
	if s != "45m" {
		t.Errorf("got %q, want %q", s, "45m")
	}
}

func TestFormatDurationHoursAndMinutes(t *testing.T) {
	s := FormatDuration(90 * time.Minute)
	if s != "1h 30m" {
		t.Errorf("got %q, want %q", s, "1h 30m")
	}
}

func TestFormatDurationShortCompact(t *testing.T) {
	s := FormatDurationShort(90 * time.Minute)
	if s != "1h30m" {
		t.Errorf("got %q, want %q", s, "1h30m")
	}
}

func TestFormatDurationZero(t *testing.T) {
	s := FormatDuration(0)
	if s != "0m" {
		t.Errorf("got %q, want %q", s, "0m")
	}
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
