// #!omni code --go -cli
// #!omni meta.key = bereshit-cmd-cpisi-cpi
// #!omni meta.at = a-01.00
// ============================================================================
// METADATA
// ============================================================================
//
// CPI Commands — Covenant Partnership Intelligence CLI Interface
//
// # M.1 Core Identity [IDENTITY]
//
//	Key:       bereshit-cmd-cpisi-cpi
//
//	Scripture: "By their fruits ye shall know them"
//	           — Matthew 7:20
//	Principle: CPI score reveals relationship quality
//	Anchor:    Metrics serve insight, not extraction
//
//	Layer:     CLI (user interface)
//	Type:      Command handler
//
//	Architect:       Seanje Lenox-Wise
//	Implementation:  Nova Dawn
//	Created:         2026-01-16
//	Version:         a-01.00
//
// ============================================================================
// END METADATA
// ============================================================================

package main

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt"
	"os"
	"strings"

	"creativeworkzstudio.com/bereshit/word/work/pkg/core/cpisi/cpi"
)

// ============================================================================
// BODY
// ============================================================================

// ----------------------------------------------------------------------------
// Entry Point
// ----------------------------------------------------------------------------

// runCPI handles CPI subcommands
func runCPI(args []string) {
	if len(args) == 0 {
		printCPIUsage()
		return
	}

	switch args[0] {
	case "classify":
		runCPIClassify(args[1:])
	case "insight":
		runCPIInsight(args[1:])
	case "depth":
		runCPIDepth(args[1:])
	case "score":
		runCPIScore(args[1:])
	case "narrative":
		runCPINarrative(args[1:])
	case "types":
		runCPITypes()
	case "help", "-h", "--help":
		printCPIUsage()
	default:
		fmt.Printf("Unknown CPI command: %s\n", args[0])
		printCPIUsage()
		os.Exit(1)
	}
}

// ----------------------------------------------------------------------------
// Subcommands
// ----------------------------------------------------------------------------

// runCPIClassify classifies exchange type from text
func runCPIClassify(args []string) {
	if len(args) == 0 {
		fmt.Println("Usage: cpisi cpi classify <text>")
		fmt.Println("  Classifies exchange type from text")
		os.Exit(1)
	}

	text := strings.Join(args, " ")
	patterns := cpi.DefaultPatterns()
	exchangeType, confidence := cpi.ClassifyExchange(text, patterns)

	fmt.Printf("Exchange Type:  %s\n", exchangeType)
	fmt.Printf("CPI/SI Mode:    %s\n", getCPISIMode(exchangeType))
	fmt.Printf("Confidence:     %.2f\n", confidence)
	fmt.Printf("Description:    %s\n", exchangeType.Description())
}

// runCPIInsight detects insights in text
func runCPIInsight(args []string) {
	if len(args) == 0 {
		fmt.Println("Usage: cpisi cpi insight <text>")
		fmt.Println("  Detects insight moments in text")
		os.Exit(1)
	}

	text := strings.Join(args, " ")
	patterns := cpi.DefaultInsightPatterns()
	insight := cpi.DetectInsight(text, patterns)

	if insight == nil {
		fmt.Println("Insight Type:   (none detected)")
		fmt.Println("Confidence:     0.00")
		return
	}

	fmt.Printf("Insight Type:   %s\n", insight.Type)
	fmt.Printf("Description:    %s\n", insight.Type.Description())
	fmt.Printf("Confidence:     %.2f\n", insight.Confidence)
	fmt.Printf("Value Bonus:    +%.2f\n", cpi.InsightScore([]cpi.InsightType{insight.Type}))
}

// runCPIDepth classifies cognitive depth of text
func runCPIDepth(args []string) {
	if len(args) == 0 {
		fmt.Println("Usage: cpisi cpi depth <text>")
		fmt.Println("  Classifies cognitive depth of exchange")
		os.Exit(1)
	}

	text := strings.Join(args, " ")
	depth := cpi.ClassifyDepth(text)

	fmt.Printf("Depth Level:    %s\n", depth)
	fmt.Printf("Description:    %s\n", getDepthDescription(depth))
	fmt.Printf("Value Factor:   %.1fx\n", getDepthFactor(depth))
}

// runCPIScore calculates CPI score from metrics
func runCPIScore(args []string) {
	// Parse flags for metrics
	metrics := cpi.SessionMetrics{
		TotalExchanges:   10,
		AverageValue:     0.6,
		InsightCount:     2,
		PositiveFeedback: 5,
		NegativeFeedback: 1,
		DeepExchanges:    3,
		WorkingExchanges: 5,
		SurfaceExchanges: 2,
	}

	// Simple flag parsing
	for i := 0; i < len(args); i++ {
		switch args[i] {
		case "-e", "--exchanges":
			if i+1 < len(args) {
				fmt.Sscanf(args[i+1], "%d", &metrics.TotalExchanges)
				i++
			}
		case "-v", "--value":
			if i+1 < len(args) {
				fmt.Sscanf(args[i+1], "%f", &metrics.AverageValue)
				i++
			}
		case "-i", "--insights":
			if i+1 < len(args) {
				fmt.Sscanf(args[i+1], "%d", &metrics.InsightCount)
				i++
			}
		case "-p", "--positive":
			if i+1 < len(args) {
				fmt.Sscanf(args[i+1], "%d", &metrics.PositiveFeedback)
				i++
			}
		case "-n", "--negative":
			if i+1 < len(args) {
				fmt.Sscanf(args[i+1], "%d", &metrics.NegativeFeedback)
				i++
			}
		case "-h", "--help":
			fmt.Println(`Usage: cpisi cpi score [options]

Options:
  -e, --exchanges <n>   Total exchange count (default: 10)
  -v, --value <f>       Average exchange value 0.0-1.0 (default: 0.6)
  -i, --insights <n>    Insight count (default: 2)
  -p, --positive <n>    Positive feedback count (default: 5)
  -n, --negative <n>    Negative feedback count (default: 1)

Example:
  cpisi cpi score -e 15 -v 0.7 -i 3 -p 8 -n 2`)
			return
		}
	}

	score := cpi.Calculate(metrics)
	rating := cpi.Rating(score.Final)

	fmt.Println("╔══════════════════════════════════════╗")
	fmt.Println("║          CPI Score Analysis          ║")
	fmt.Println("╠══════════════════════════════════════╣")
	fmt.Printf("  Raw Score:     %.4f\n", score.Raw)
	fmt.Printf("  Final Score:   %.4f\n", score.Final)
	fmt.Printf("  Rating:        %s\n", rating)
	fmt.Printf("  Description:   %s\n", rating.Description())
	fmt.Println("╠══════════════════════════════════════╣")
	fmt.Println("  Components:")
	fmt.Printf("    Base Value:      %.4f\n", score.Components.BaseValue)
	fmt.Printf("    Insight Bonus:   +%.4f\n", score.Components.InsightBonus)
	fmt.Printf("    Feedback Adj:    %+.4f\n", score.Components.FeedbackAdjust)
	fmt.Printf("    Depth Factor:    %.4fx\n", score.Components.DepthFactor)
	fmt.Println("╠══════════════════════════════════════╣")
	fmt.Println("  Input Metrics:")
	fmt.Printf("    Exchanges:       %d\n", metrics.TotalExchanges)
	fmt.Printf("    Avg Value:       %.2f\n", metrics.AverageValue)
	fmt.Printf("    Insights:        %d\n", metrics.InsightCount)
	fmt.Printf("    Feedback:        +%d / -%d\n", metrics.PositiveFeedback, metrics.NegativeFeedback)
	fmt.Println("╚══════════════════════════════════════╝")
}

// runCPINarrative generates session narrative
func runCPINarrative(args []string) {
	stats := cpi.SessionStats{
		TotalExchanges:   15,
		DominantType:     cpi.ExchangeCollaborative,
		InsightCount:     3,
		PositiveFeedback: 8,
		NegativeFeedback: 2,
	}

	// Parse flags
	for i := 0; i < len(args); i++ {
		switch args[i] {
		case "-e", "--exchanges":
			if i+1 < len(args) {
				fmt.Sscanf(args[i+1], "%d", &stats.TotalExchanges)
				i++
			}
		case "-t", "--type":
			if i+1 < len(args) {
				stats.DominantType = cpi.ExchangeType(args[i+1])
				i++
			}
		case "-i", "--insights":
			if i+1 < len(args) {
				fmt.Sscanf(args[i+1], "%d", &stats.InsightCount)
				i++
			}
		case "-s", "--section":
			if i+1 < len(args) {
				stats.TrajectorySection = args[i+1]
				i++
			}
		case "--hebrew":
			if i+2 < len(args) {
				stats.HebrewState = args[i+1]
				stats.HebrewMeaning = args[i+2]
				i += 2
			}
		case "-h", "--help":
			fmt.Println(`Usage: cpisi cpi narrative [options]

Options:
  -e, --exchanges <n>      Total exchange count (default: 15)
  -t, --type <type>        Dominant exchange type (default: collaborative)
  -i, --insights <n>       Insight count (default: 3)
  -s, --section <B.N>      Trajectory section (B.1-B.4)
  --hebrew <state> <mean>  Hebrew state and meaning

Exchange Types: directive, collaborative, check_in, pushback,
                affirmation, question, context

Example:
  cpisi cpi narrative -e 20 -t collaborative -i 4 -s B.4 --hebrew tov PERFECT`)
			return
		}
	}

	narrative := cpi.GenerateNarrative(stats)

	fmt.Println("╔══════════════════════════════════════╗")
	fmt.Println("║        Session Narrative             ║")
	fmt.Println("╠══════════════════════════════════════╣")
	fmt.Printf("  Arc:           %s %s\n", cpi.ArcEmoji(narrative.Arc), narrative.Arc)
	fmt.Printf("  Description:   %s\n", narrative.Arc.Description())
	fmt.Println("╠══════════════════════════════════════╣")
	fmt.Println("  Summary:")
	fmt.Printf("    %s\n", narrative.Summary)
	fmt.Println("╠══════════════════════════════════════╣")
	fmt.Printf("  Short:         %s\n", cpi.ShortSummary(narrative))
	fmt.Println("╚══════════════════════════════════════╝")
}

// runCPITypes shows all exchange and insight types
func runCPITypes() {
	fmt.Println("╔══════════════════════════════════════════════════════════════╗")
	fmt.Println("║                    CPI Type Reference                        ║")
	fmt.Println("╠══════════════════════════════════════════════════════════════╣")
	fmt.Println("  Exchange Types (what kind of interaction):")
	fmt.Println()

	exchangeTypes := []cpi.ExchangeType{
		cpi.ExchangeDirective,
		cpi.ExchangeCollaborative,
		cpi.ExchangeCheckIn,
		cpi.ExchangePushback,
		cpi.ExchangeAffirmation,
		cpi.ExchangeQuestion,
		cpi.ExchangeContext,
	}

	for _, t := range exchangeTypes {
		mode := "SI"
		if t.IsCPI() {
			mode = "CPI"
		}
		fmt.Printf("    %-15s [%s]  %s\n", t, mode, t.Description())
	}

	fmt.Println()
	fmt.Println("╠══════════════════════════════════════════════════════════════╣")
	fmt.Println("  Insight Types (understanding transfer moments):")
	fmt.Println()

	insightTypes := []cpi.InsightType{
		cpi.InsightUnderstanding,
		cpi.InsightConnection,
		cpi.InsightDiscovery,
		cpi.InsightBreakthrough,
	}

	for _, t := range insightTypes {
		fmt.Printf("    %-15s      %s\n", t, t.Description())
	}

	fmt.Println()
	fmt.Println("╠══════════════════════════════════════════════════════════════╣")
	fmt.Println("  Session Arcs (overall session character):")
	fmt.Println()

	arcs := []cpi.SessionArc{
		cpi.ArcLearning,
		cpi.ArcCompletion,
		cpi.ArcExploration,
		cpi.ArcExecution,
		cpi.ArcCreative,
		cpi.ArcReflective,
		cpi.ArcDiagnostic,
		cpi.ArcOnboarding,
		cpi.ArcMixed,
	}

	for _, a := range arcs {
		fmt.Printf("    %s %-12s   %s\n", cpi.ArcEmoji(a), a, a.Description())
	}

	fmt.Println()
	fmt.Println("╠══════════════════════════════════════════════════════════════╣")
	fmt.Println("  Score Ratings (CPI quality assessment):")
	fmt.Println()

	ratings := []struct {
		rating cpi.ScoreRating
		range_ string
	}{
		{cpi.RatingExcellent, "0.8-1.0"},
		{cpi.RatingGood, "0.6-0.8"},
		{cpi.RatingNeutral, "0.4-0.6"},
		{cpi.RatingLow, "0.2-0.4"},
		{cpi.RatingPoor, "0.0-0.2"},
	}

	for _, r := range ratings {
		fmt.Printf("    %-10s [%s]   %s\n", r.rating, r.range_, r.rating.Description())
	}

	fmt.Println()
	fmt.Println("╚══════════════════════════════════════════════════════════════╝")
}

// ----------------------------------------------------------------------------
// Helpers
// ----------------------------------------------------------------------------

func getCPISIMode(t cpi.ExchangeType) string {
	if t.IsCPI() {
		return "CPI (covenant partnership)"
	}
	if t.IsSI() {
		return "SI (structured intelligence)"
	}
	return "Unknown"
}

func getDepthDescription(d cpi.DepthLevel) string {
	switch d {
	case cpi.DepthDeep:
		return "Deep cognitive engagement — foundational reasoning"
	case cpi.DepthWorking:
		return "Working level — practical application"
	case cpi.DepthSurface:
		return "Surface level — quick responses"
	default:
		return "Unknown"
	}
}

func getDepthFactor(d cpi.DepthLevel) float64 {
	switch d {
	case cpi.DepthDeep:
		return 1.2
	case cpi.DepthWorking:
		return 1.0
	case cpi.DepthSurface:
		return 0.8
	default:
		return 1.0
	}
}

// ----------------------------------------------------------------------------
// Usage
// ----------------------------------------------------------------------------

func printCPIUsage() {
	fmt.Println(`CPI (Covenant Partnership Intelligence) Commands

Usage: cpisi cpi <command> [args]

Commands:
  classify <text>    Classify exchange type from text
  insight <text>     Detect insight moments in text
  depth <text>       Classify cognitive depth of exchange
  score [options]    Calculate CPI score from metrics
  narrative [opts]   Generate session narrative
  types              Show all CPI types and arcs
  help               Show this help

Examples:
  cpisi cpi classify "Let's build this together"
  cpisi cpi insight "Now I understand why we need the anchor"
  cpisi cpi depth "Genesis 1:1 grounds all reasoning"
  cpisi cpi score -e 15 -i 3 -v 0.7
  cpisi cpi narrative -t collaborative -s B.4
  cpisi cpi types

CPI tracks relationship quality, not just task completion.
"By their fruits ye shall know them." — Matthew 7:20`)
}

// ============================================================================
// CLOSING
// ============================================================================
//
// CPI commands expose the native cpi package through the CLI:
//
//   classify — What type of exchange is this?
//   insight  — Was there understanding transfer?
//   depth    — How deep was the cognitive engagement?
//   score    — What's the session quality rating?
//   narrative — What story does this session tell?
//   types    — Reference for all CPI classifications
//
// This enables:
//   - Testing CPI classification on sample text
//   - Understanding how sessions are scored
//   - Debugging CPI tracking behavior
//   - Learning the CPI paradigm through exploration
//
// ============================================================================
// END CLOSING
// ============================================================================
