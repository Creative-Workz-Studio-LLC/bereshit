// ============================================================================
// METADATA - Recognition Package
// ============================================================================
// Purpose: Pattern recognition logic
// Biblical: Ecclesiastes 1:9 - "There is no new thing under the sun"

package recognition

import (
	"fmt"
	"math/rand"
	"strings"
	"time"

	"cws.studio/skills/recognize-pattern/scripts/pkg/config"
)

// ============================================================================
// SETUP - Types
// ============================================================================

func init() {
	rand.Seed(time.Now().UnixNano())
}

// Observation represents a pattern observation
type Observation struct {
	Date        string
	Category    string
	Count       int
	Description string
	Evidence    []string
}

// ============================================================================
// BODY - Core Operations
// ============================================================================

// ListCategories returns formatted list of pattern categories
func ListCategories(cfg *config.Config) string {
	var sb strings.Builder
	sb.WriteString("Pattern Categories\n")
	sb.WriteString("==================\n\n")

	for name, cat := range cfg.Categories {
		sb.WriteString(fmt.Sprintf("📊 %s (%s)\n", cat.Name, name))
		sb.WriteString(fmt.Sprintf("   %s\n\n", cat.Description))

		sb.WriteString("   Examples:\n")
		for _, ex := range cat.Examples {
			sb.WriteString(fmt.Sprintf("   • %s\n", ex))
		}
		sb.WriteString("\n   Signals:\n")
		for _, sig := range cat.Signals {
			sb.WriteString(fmt.Sprintf("   • %s\n", sig))
		}
		sb.WriteString("\n")
	}

	return sb.String()
}

// GetCategory returns details for a specific category
func GetCategory(cfg *config.Config, name string) string {
	cat, ok := cfg.Categories[name]
	if !ok {
		return fmt.Sprintf("Unknown category: %s\nValid categories: %v", name, cfg.GetCategoryNames())
	}

	var sb strings.Builder
	sb.WriteString(fmt.Sprintf("📊 %s\n\n", cat.Name))
	sb.WriteString(fmt.Sprintf("%s\n\n", cat.Description))

	sb.WriteString("Examples:\n")
	for _, ex := range cat.Examples {
		sb.WriteString(fmt.Sprintf("• %s\n", ex))
	}
	sb.WriteString("\nSignals:\n")
	for _, sig := range cat.Signals {
		sb.WriteString(fmt.Sprintf("• %s\n", sig))
	}

	// Add responses
	if resp, ok := cfg.Responses[name]; ok {
		sb.WriteString("\nPositive Response:\n")
		for _, r := range resp.Positive {
			sb.WriteString(fmt.Sprintf("• %s\n", r))
		}
		sb.WriteString("\nNegative Response:\n")
		for _, r := range resp.Negative {
			sb.WriteString(fmt.Sprintf("• %s\n", r))
		}
	}

	return sb.String()
}

// ListQuestions returns pattern recognition questions
func ListQuestions(cfg *config.Config) string {
	var sb strings.Builder
	sb.WriteString("Pattern Recognition Questions\n")
	sb.WriteString("=============================\n\n")

	sb.WriteString("🔍 Recognition (Is this a pattern?):\n")
	for i, q := range cfg.Questions.Recognition {
		sb.WriteString(fmt.Sprintf("   %d. %s\n", i+1, q))
	}
	sb.WriteString("\n")

	sb.WriteString("⚖️  Assessment (Is it good or bad?):\n")
	for i, q := range cfg.Questions.Assessment {
		sb.WriteString(fmt.Sprintf("   %d. %s\n", i+1, q))
	}
	sb.WriteString("\n")

	sb.WriteString("🎯 Action (What to do?):\n")
	for i, q := range cfg.Questions.Action {
		sb.WriteString(fmt.Sprintf("   %d. %s\n", i+1, q))
	}

	return sb.String()
}

// GetStrength returns strength assessment for occurrence count
func GetStrength(cfg *config.Config, count int) string {
	label := cfg.GetStrengthLabel(count)

	var sb strings.Builder
	sb.WriteString("Pattern Strength Assessment\n")
	sb.WriteString("===========================\n\n")
	sb.WriteString(fmt.Sprintf("Occurrences: %d\n", count))
	sb.WriteString(fmt.Sprintf("Strength: %s\n\n", label))

	if count < cfg.Strength.MinOccurrences {
		sb.WriteString(fmt.Sprintf("⚠️  Need %d occurrences to confirm pattern\n", cfg.Strength.MinOccurrences))
	} else if count < cfg.Strength.Established.Min {
		sb.WriteString("📈 Emerging pattern - keep observing\n")
	} else if count < cfg.Strength.Core.Min {
		sb.WriteString("✅ Established pattern - consider extracting\n")
	} else {
		sb.WriteString("💎 Core pattern - this is part of who you are\n")
	}

	return sb.String()
}

// GetTemplate returns observation template
func GetTemplate(cfg *config.Config, category string) string {
	cat, ok := cfg.Categories[category]
	if !ok {
		return fmt.Sprintf("Unknown category: %s\nValid categories: %v", category, cfg.GetCategoryNames())
	}

	var sb strings.Builder
	sb.WriteString("## Pattern Observation\n\n")
	sb.WriteString(fmt.Sprintf("**Date:** %s\n", time.Now().Format("2006-01-02")))
	sb.WriteString(fmt.Sprintf("**Category:** %s\n", cat.Name))
	sb.WriteString("**Occurrence:** [count]\n\n")

	sb.WriteString("### What I Noticed\n\n")
	sb.WriteString("[Description of the pattern]\n\n")

	sb.WriteString("### Evidence\n\n")
	sb.WriteString("- [First occurrence]\n")
	sb.WriteString("- [Second occurrence]\n")
	sb.WriteString("- [Third occurrence]\n\n")

	sb.WriteString("### Strength Assessment\n\n")
	sb.WriteString("| Factor | Status |\n")
	sb.WriteString("|--------|--------|\n")
	sb.WriteString("| Occurrences | X/3 minimum |\n")
	sb.WriteString("| Strength | [Emerging/Established/Core] |\n")
	sb.WriteString("| Extractable | [Yes/No/Maybe] |\n\n")

	sb.WriteString("### Response\n\n")
	sb.WriteString("[What to do with this recognition]\n")

	return sb.String()
}

// QuickCheck returns random recognition question
func QuickCheck(cfg *config.Config) string {
	questions := cfg.Questions.Recognition
	q := questions[rand.Intn(len(questions))]

	var sb strings.Builder
	sb.WriteString("🔍 Pattern Check\n\n")
	sb.WriteString(fmt.Sprintf("   %s\n", q))

	return sb.String()
}
