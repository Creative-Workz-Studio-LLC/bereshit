// ============================================================================
// METADATA - Checkpoint Package
// ============================================================================
// Purpose: Awareness checkpoint logic
// Biblical: Proverbs 4:26 - "Ponder the path of thy feet"

package checkpoint

import (
	"fmt"
	"math/rand"
	"strings"
	"time"

	"cws.studio/skills/meta-awareness/scripts/pkg/config"
)

// ============================================================================
// SETUP - Types
// ============================================================================

func init() {
	rand.Seed(time.Now().UnixNano())
}

// ============================================================================
// BODY - Core Operations
// ============================================================================

// ListPrompts returns formatted list of all prompt categories
func ListPrompts(cfg *config.Config) string {
	var sb strings.Builder
	sb.WriteString("Awareness Checkpoints\n")
	sb.WriteString("=====================\n\n")

	for name, prompt := range cfg.Prompts {
		sb.WriteString(fmt.Sprintf("🔍 %s (%s)\n", prompt.Name, name))
		sb.WriteString(fmt.Sprintf("   %s\n\n", prompt.Description))
		for i, q := range prompt.Questions {
			sb.WriteString(fmt.Sprintf("   %d. %s\n", i+1, q))
		}
		sb.WriteString("\n")
	}

	return sb.String()
}

// ListStates returns formatted list of all states
func ListStates(cfg *config.Config) string {
	var sb strings.Builder
	sb.WriteString("Cognitive States\n")
	sb.WriteString("================\n\n")

	// Healthy states first
	sb.WriteString("✅ Healthy States:\n\n")
	for name, state := range cfg.States {
		if name == "focused" || name == "flowing" {
			sb.WriteString(fmt.Sprintf("   %s\n", state.Name))
			sb.WriteString(fmt.Sprintf("   Indicators: %s\n\n", strings.Join(state.Indicators, ", ")))
		}
	}

	// Warning states
	sb.WriteString("⚠️  Warning States:\n\n")
	for name, state := range cfg.States {
		if name != "focused" && name != "flowing" {
			sb.WriteString(fmt.Sprintf("   %s\n", state.Name))
			sb.WriteString(fmt.Sprintf("   Indicators: %s\n", strings.Join(state.Indicators, ", ")))
			if resp, ok := cfg.Responses[name]; ok {
				sb.WriteString(fmt.Sprintf("   Response: %s\n", strings.Join(resp.Actions, ", ")))
			}
			sb.WriteString("\n")
		}
	}

	return sb.String()
}

// ListTriggers returns formatted list of triggers
func ListTriggers(cfg *config.Config) string {
	var sb strings.Builder
	sb.WriteString("When to Check\n")
	sb.WriteString("=============\n\n")

	sb.WriteString("⏰ Time-Based:\n")
	for _, t := range cfg.Triggers.TimeBased {
		sb.WriteString(fmt.Sprintf("   • %s\n", t))
	}
	sb.WriteString("\n")

	sb.WriteString("📍 Event-Based:\n")
	for _, t := range cfg.Triggers.EventBased {
		sb.WriteString(fmt.Sprintf("   • %s\n", t))
	}
	sb.WriteString("\n")

	sb.WriteString("🔄 Pattern-Based:\n")
	for _, t := range cfg.Triggers.PatternBased {
		sb.WriteString(fmt.Sprintf("   • %s\n", t))
	}

	return sb.String()
}

// QuickCheck returns a random question for quick awareness check
func QuickCheck(cfg *config.Config, category string) string {
	var sb strings.Builder

	// If category specified, use that
	if category != "" {
		prompt, ok := cfg.Prompts[category]
		if !ok {
			return fmt.Sprintf("Unknown category: %s\nValid categories: %v", category, cfg.GetPromptCategories())
		}
		sb.WriteString(fmt.Sprintf("🔍 %s Check\n\n", prompt.Name))
		q := prompt.Questions[rand.Intn(len(prompt.Questions))]
		sb.WriteString(fmt.Sprintf("   %s\n", q))
		return sb.String()
	}

	// Random category
	categories := cfg.GetPromptCategories()
	cat := categories[rand.Intn(len(categories))]
	prompt := cfg.Prompts[cat]
	q := prompt.Questions[rand.Intn(len(prompt.Questions))]

	sb.WriteString(fmt.Sprintf("🔍 %s Check\n\n", prompt.Name))
	sb.WriteString(fmt.Sprintf("   %s\n", q))

	return sb.String()
}

// GetPrompt returns all questions for a category
func GetPrompt(cfg *config.Config, category string) string {
	prompt, ok := cfg.Prompts[category]
	if !ok {
		return fmt.Sprintf("Unknown category: %s\nValid categories: %v", category, cfg.GetPromptCategories())
	}

	var sb strings.Builder
	sb.WriteString(fmt.Sprintf("🔍 %s\n", prompt.Name))
	sb.WriteString(fmt.Sprintf("   %s\n\n", prompt.Description))

	for i, q := range prompt.Questions {
		sb.WriteString(fmt.Sprintf("%d. %s\n", i+1, q))
	}

	return sb.String()
}

// GetResponse returns recommended actions for a state
func GetResponse(cfg *config.Config, state string) string {
	resp, ok := cfg.Responses[state]
	if !ok {
		return fmt.Sprintf("No response defined for state: %s", state)
	}

	stateInfo, hasState := cfg.States[state]

	var sb strings.Builder
	if hasState {
		sb.WriteString(fmt.Sprintf("⚠️  %s State Detected\n\n", stateInfo.Name))
		sb.WriteString("Indicators:\n")
		for _, ind := range stateInfo.Indicators {
			sb.WriteString(fmt.Sprintf("   • %s\n", ind))
		}
		sb.WriteString("\n")
	}

	sb.WriteString("Recommended Actions:\n")
	for i, action := range resp.Actions {
		sb.WriteString(fmt.Sprintf("   %d. %s\n", i+1, action))
	}

	return sb.String()
}
