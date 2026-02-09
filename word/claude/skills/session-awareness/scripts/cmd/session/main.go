// ============================================================================
// METADATA
// ============================================================================
//
// Key: session-awareness-cmd-session
// Purpose: CLI orchestrator for session-awareness skill
// Biblical: Lamentations 3:22-23 - "His compassions fail not"
// Authors: Nova Dawn
// Version: 3.0.0
// Created: 2025-12-09
// Updated: 2025-12-10 - Refactored to use shared pkg/cli and pkg/output
//
// This is the ORCHESTRATOR - CLI parsing only.
// All logic lives in pkg/duration/, pkg/circadian/, and pkg/config/.
//
// ============================================================================

package main

// ============================================================================
// SETUP
// ============================================================================

import (
	"os"

	"cws.studio/pkg/util/pure/cli"
	"cws.studio/pkg/util/pure/output"
	"cws.studio/skills/session-awareness/scripts/pkg/circadian"
	"cws.studio/skills/session-awareness/scripts/pkg/config"
	"cws.studio/skills/session-awareness/scripts/pkg/duration"
)

// ============================================================================
// BODY
// ============================================================================

// --- Helpers ---

// printDuration outputs duration check results
func printDuration(result *duration.DurationResult, cfg *config.Config, showJSON bool) {
	if showJSON {
		printDurationJSON(result)
		return
	}

	output.Header("Session Duration Check")
	output.KeyValue("Session", result.SessionID)
	output.KeyValue("Started", result.StartTime.Format(cfg.Output.TimeFormat))
	output.KeyValue("Elapsed", duration.FormatDuration(result.Elapsed))
	output.KeyValue("Category", result.CategoryName)
	output.Tip(result.Guidance)
}

// printDurationJSON outputs duration as JSON
func printDurationJSON(result *duration.DurationResult) {
	output.JSONStart()
	output.JSONField("session_id", result.SessionID)
	output.JSONField("start_time", result.StartTime.Format("2006-01-02T15:04:05Z07:00"))
	output.JSONField("elapsed_minutes", result.Minutes)
	output.JSONField("elapsed_human", duration.FormatDuration(result.Elapsed))
	output.JSONField("category", result.CategoryName)
	output.JSONFieldLast("guidance", result.Guidance)
	output.JSONEnd()
}

// printCircadian outputs circadian check results
func printCircadian(result *circadian.CircadianResult, cfg *config.Config, showJSON bool) {
	if showJSON {
		printCircadianJSON(result)
		return
	}

	output.Header("Circadian Awareness")
	output.KeyValue("Time", result.CurrentTime.Format(cfg.Output.TimeFormat))
	output.KeyValue("Day", result.DayOfWeek)
	output.KeyValue("Window", result.WindowName)
	output.KeyValue("Phase", result.Phase)
	output.KeyValue("Energy", result.Productivity)
	if result.Note != "" {
		output.Info(result.Note)
	}
	output.Tip(circadian.GetGuidance(result))
}

// printCircadianJSON outputs circadian as JSON
func printCircadianJSON(result *circadian.CircadianResult) {
	output.JSONStart()
	output.JSONField("current_time", result.CurrentTime.Format("2006-01-02T15:04:05Z07:00"))
	output.JSONField("hour", result.Hour)
	output.JSONField("day_of_week", result.DayOfWeek)
	output.JSONField("window", result.WindowName)
	output.JSONField("phase", result.Phase)
	output.JSONField("productivity", result.Productivity)
	output.JSONField("is_work_hours", result.IsWorkHours)
	output.JSONField("note", result.Note)
	output.JSONFieldLast("guidance", circadian.GetGuidance(result))
	output.JSONEnd()
}

// printFull outputs both duration and circadian
func printFull(dResult *duration.DurationResult, cResult *circadian.CircadianResult, cfg *config.Config, showJSON bool) {
	if showJSON {
		printFullJSON(dResult, cResult)
		return
	}

	output.Header("Session Awareness")

	output.Section("EXTERNAL CLOCK")
	output.KeyValue("Time", cResult.CurrentTime.Format(cfg.Output.TimeFormat))
	output.KeyValue("Day", cResult.DayOfWeek)

	output.Section("SESSION CLOCK")
	output.KeyValue("Session", dResult.SessionID)
	output.KeyValue("Elapsed", duration.FormatDuration(dResult.Elapsed))
	output.KeyValue("Category", dResult.CategoryName)

	output.Section("INTERNAL CLOCK")
	output.KeyValue("Window", cResult.WindowName)
	output.KeyValue("Phase", cResult.Phase)
	output.KeyValue("Energy", cResult.Productivity)

	output.Section("GUIDANCE")
	output.Indent("Duration: "+dResult.Guidance, 1)
	output.Indent("Circadian: "+circadian.GetGuidance(cResult), 1)
}

// printFullJSON outputs both as JSON
func printFullJSON(dResult *duration.DurationResult, cResult *circadian.CircadianResult) {
	output.JSONStart()
	output.JSONObjectStart("external_clock")
	output.JSONField("current_time", cResult.CurrentTime.Format("2006-01-02T15:04:05Z07:00"))
	output.JSONFieldLast("day_of_week", cResult.DayOfWeek)
	output.JSONObjectEnd()
	output.JSONObjectStart("session_clock")
	output.JSONField("session_id", dResult.SessionID)
	output.JSONField("elapsed_minutes", dResult.Minutes)
	output.JSONField("elapsed_human", duration.FormatDuration(dResult.Elapsed))
	output.JSONFieldLast("category", dResult.CategoryName)
	output.JSONObjectEnd()
	output.JSONObjectStart("internal_clock")
	output.JSONField("window", cResult.WindowName)
	output.JSONField("phase", cResult.Phase)
	output.JSONFieldLast("productivity", cResult.Productivity)
	output.JSONObjectEnd()
	output.JSONObjectStart("guidance")
	output.JSONField("duration", dResult.Guidance)
	output.JSONFieldLast("circadian", circadian.GetGuidance(cResult))
	output.JSONObjectEndLast()
	output.JSONEnd()
}

// --- Core Operations ---

// runDuration executes the duration command
func runDuration(cfg *config.Config, showJSON bool) error {
	result, err := duration.Check(cfg)
	if err != nil {
		return err
	}
	printDuration(result, cfg, showJSON)
	return nil
}

// runCircadian executes the circadian command
func runCircadian(cfg *config.Config, showJSON bool) error {
	result := circadian.Check(cfg)
	printCircadian(result, cfg, showJSON)
	return nil
}

// runFull executes the full awareness command
func runFull(cfg *config.Config, showJSON bool) error {
	dResult, err := duration.Check(cfg)
	if err != nil {
		// Even if no session, show circadian
		if !showJSON {
			cli.Warn("No active session - showing circadian awareness only")
		}
		return runCircadian(cfg, showJSON)
	}
	cResult := circadian.Check(cfg)
	printFull(dResult, cResult, cfg, showJSON)
	return nil
}

// ============================================================================
// CLOSING
// ============================================================================

func main() {
	// Setup CLI app
	app := cli.New("session", "Check session duration and circadian awareness").
		WithBiblical("Lamentations 3:22-23 - \"His compassions fail not\"").
		WithJSON()

	// Parse
	if err := app.ParseOS(); err != nil {
		cli.Fatal("Parse error: %v", err)
	}

	// Help
	if app.WantsHelp() {
		usage := cli.NewUsage(app).
			AddSection("Commands",
				"duration    Check session duration (how long have I been working?)",
				"circadian   Check circadian awareness (am I in productive hours?)",
				"full        Full awareness check (default - both clocks)").
			AddSection("Options",
				"-json       Output as JSON",
				"-h, -help   Show this help").
			AddSection("Examples",
				"session                    # Full awareness check",
				"session duration           # Just duration",
				"session circadian -json    # Circadian as JSON")
		usage.Print()
		os.Exit(0)
	}

	// Load configuration
	cfg, err := config.Load()
	if err != nil {
		cli.Fatal("Error loading config: %v", err)
	}

	// Get command (default to full)
	args := app.Args()
	command := "full"
	if len(args) > 0 {
		command = args[0]
	}

	// Execute command
	switch command {
	case "duration":
		err = runDuration(cfg, app.WantsJSON())
	case "circadian":
		err = runCircadian(cfg, app.WantsJSON())
	case "full":
		err = runFull(cfg, app.WantsJSON())
	default:
		cli.Fatal("Unknown command: %s\nUse -h for help", command)
	}

	if err != nil {
		cli.Fatal("Error: %v", err)
	}
}
