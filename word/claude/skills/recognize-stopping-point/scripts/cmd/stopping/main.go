// ============================================================================
// METADATA
// ============================================================================
//
// Key: recognize-stopping-point-cmd-stopping
// Purpose: CLI orchestrator for recognize-stopping-point skill
// Biblical: Ecclesiastes 3:1 - "To every thing there is a season"
// Authors: Nova Dawn
// Version: 3.0.0
// Created: 2025-12-09
// Updated: 2025-12-10 - Refactored to use shared pkg/cli and pkg/output
//
// This is the ORCHESTRATOR - CLI parsing only.
// All logic lives in pkg/analyze/ and pkg/config/.
//
// ============================================================================

package main

// ============================================================================
// SETUP
// ============================================================================

import (
	"os"

	"github.com/creativeworkzstudio/claude-global/pkg/util/pure/cli"
	"github.com/creativeworkzstudio/claude-global/pkg/util/pure/output"
	"github.com/creativeworkzstudio/claude-global/skills/recognize-stopping-point/scripts/pkg/analyze"
	"github.com/creativeworkzstudio/claude-global/skills/recognize-stopping-point/scripts/pkg/config"
)

// ============================================================================
// BODY
// ============================================================================

// --- Helpers ---

// printResult outputs analysis results
func printResult(result *analyze.AnalysisResult, showJSON bool) {
	if showJSON {
		printResultJSON(result)
		return
	}

	output.Header("Stopping Point Analysis")
	output.KeyValue("Session", result.SessionID)
	output.KeyValue("Duration", result.ElapsedMinutes)
	output.KeyValue("Time", result.DayOfWeek+" at hour "+string(rune('0'+result.CurrentHour/10))+string(rune('0'+result.CurrentHour%10)))

	output.Section("Signals")
	for _, s := range result.Signals {
		status := "  "
		if s.Present {
			status = "✓ "
		}
		output.Indent(status+"["+string(rune('0'+s.Weight))+"] "+s.Name+": "+s.Details, 0)
	}

	output.KeyValue("Total Weight", result.TotalWeight)

	// Recommendation with emoji
	emoji := "💡"
	switch {
	case result.TotalWeight >= 8:
		emoji = "🛑"
	case result.TotalWeight >= 4:
		emoji = "⚠️"
	}
	output.Info("%s %s", emoji, result.Recommendation)

	if len(result.SuggestedActions) > 0 {
		output.Section("Suggested Actions")
		for _, action := range result.SuggestedActions {
			output.Bullet(action)
		}
	}

	if result.RecommendedEnd != "" {
		output.KeyValue("Recommended end reason", result.RecommendedEnd)
	}
}

// printResultJSON outputs result as JSON
func printResultJSON(result *analyze.AnalysisResult) {
	output.JSONStart()
	output.JSONField("session_id", result.SessionID)
	output.JSONField("elapsed_minutes", result.ElapsedMinutes)
	output.JSONField("current_hour", result.CurrentHour)
	output.JSONField("day_of_week", result.DayOfWeek)

	// Signals array
	output.Indent("  \"signals\": [", 0)
	for i, s := range result.Signals {
		comma := ","
		if i == len(result.Signals)-1 {
			comma = ""
		}
		output.Indent("    {\"type\": \""+s.Type+"\", \"name\": \""+s.Name+"\", \"weight\": "+string(rune('0'+s.Weight))+", \"present\": "+boolStr(s.Present)+", \"details\": \""+s.Details+"\"}"+comma, 0)
	}
	output.Indent("  ],", 0)

	output.JSONField("total_weight", result.TotalWeight)
	output.JSONField("recommendation", result.Recommendation)
	output.JSONField("recommended_end", result.RecommendedEnd)

	// Actions array
	output.Indent("  \"suggested_actions\": [", 0)
	for i, a := range result.SuggestedActions {
		comma := ","
		if i == len(result.SuggestedActions)-1 {
			comma = ""
		}
		output.Indent("    \""+a+"\""+comma, 0)
	}
	output.Indent("  ]", 0)
	output.JSONEnd()
}

func boolStr(b bool) string {
	if b {
		return "true"
	}
	return "false"
}

// --- Core Operations ---

// runAnalysis performs the stopping point analysis
func runAnalysis(cfg *config.Config, isDeep, qualityDip, milestone, cleanBreak, showJSON bool) error {
	result, err := analyze.Analyze(cfg, isDeep, qualityDip, milestone, cleanBreak)
	if err != nil {
		return err
	}

	printResult(result, showJSON)
	return nil
}

// ============================================================================
// CLOSING
// ============================================================================

func main() {
	// Setup CLI app
	app := cli.New("stopping", "Analyze stopping point signals for current session").
		WithBiblical("Ecclesiastes 3:1 - \"To every thing there is a season\"").
		WithJSON()

	// Custom flags
	deepFlag := app.Flag().Bool("deep", false, "Mark as deep work session")
	qualityFlag := app.Flag().Bool("quality-dip", false, "Report quality decline")
	milestoneFlag := app.Flag().Bool("milestone", false, "Report milestone completion")
	breakFlag := app.Flag().Bool("break", false, "Report clean break point")

	// Parse
	if err := app.ParseOS(); err != nil {
		cli.Fatal("Parse error: %v", err)
	}

	// Help
	if app.WantsHelp() {
		usage := cli.NewUsage(app).
			AddSection("Usage",
				"stopping [options]").
			AddSection("Options",
				"-deep          Mark this as deep work session (affects duration limits)",
				"-quality-dip   Report quality decline observed",
				"-milestone     Report milestone completion",
				"-break         Report clean break point",
				"-json          Output as JSON",
				"-h, -help      Show this help").
			AddSection("Examples",
				"stopping                    # Basic analysis",
				"stopping -milestone         # Completed something",
				"stopping -quality-dip       # Quality declining",
				"stopping -deep -milestone   # Deep work, milestone done").
			AddSection("Stopping point types",
				"Natural Milestone  [5] - Completed meaningful work",
				"Clean Break        [4] - Transition between tasks",
				"End of Day         [4] - Circadian downtime window",
				"Quality Dip        [5] - Work quality declining",
				"Duration Limit     [3] - Session duration reached")
		usage.Print()
		os.Exit(0)
	}

	// Load configuration
	cfg, err := config.Load()
	if err != nil {
		cli.Fatal("Error loading config: %v", err)
	}

	// Run analysis
	err = runAnalysis(cfg, *deepFlag, *qualityFlag, *milestoneFlag, *breakFlag, app.WantsJSON())
	if err != nil {
		cli.Fatal("Error: %v", err)
	}
}
