// ============================================================================
// METADATA - Reflection Package
// ============================================================================
// Purpose: Session reflection logic
// Biblical: Psalm 77:12 - "I will meditate also of all thy work"

package reflection

import (
	"fmt"
	"strings"
	"time"

	"github.com/creativeworkzstudio/claude-global/skills/reflect-on-session/scripts/pkg/config"
)

// ============================================================================
// BODY - Core Operations
// ============================================================================

// ListDimensions returns formatted list of all reflection dimensions
func ListDimensions(cfg *config.Config) string {
	var sb strings.Builder
	sb.WriteString("Reflection Dimensions\n")
	sb.WriteString("=====================\n\n")

	for name, dim := range cfg.Dimensions {
		sb.WriteString(fmt.Sprintf("📝 %s (%s)\n", dim.Name, name))
		sb.WriteString(fmt.Sprintf("   %s\n\n", dim.Description))

		for i, q := range dim.Questions {
			sb.WriteString(fmt.Sprintf("   %d. %s\n", i+1, q))
		}
		sb.WriteString("\n")
	}

	return sb.String()
}

// GetDimension returns questions for a specific dimension
func GetDimension(cfg *config.Config, name string) string {
	dim, ok := cfg.Dimensions[name]
	if !ok {
		return fmt.Sprintf("Unknown dimension: %s\nValid dimensions: %v", name, cfg.GetDimensionNames())
	}

	var sb strings.Builder
	sb.WriteString(fmt.Sprintf("📝 %s\n\n", dim.Name))
	sb.WriteString(fmt.Sprintf("%s\n\n", dim.Description))

	sb.WriteString("Questions:\n")
	for i, q := range dim.Questions {
		sb.WriteString(fmt.Sprintf("%d. %s\n", i+1, q))
	}

	return sb.String()
}

// ListDepths returns formatted list of depth levels
func ListDepths(cfg *config.Config) string {
	var sb strings.Builder
	sb.WriteString("Reflection Depth Levels\n")
	sb.WriteString("=======================\n\n")

	// Order: quick, standard, deep
	order := []string{"quick", "standard", "deep"}
	for _, name := range order {
		depth, ok := cfg.Depth[name]
		if !ok {
			continue
		}
		sb.WriteString(fmt.Sprintf("⏱️  %s (%s)\n", depth.Name, name))
		sb.WriteString(fmt.Sprintf("   %s\n", depth.Description))
		sb.WriteString(fmt.Sprintf("   Dimensions: %s\n", strings.Join(depth.Dimensions, ", ")))
		sb.WriteString(fmt.Sprintf("   Output: %s\n\n", depth.Output))
	}

	return sb.String()
}

// ListTriggers returns when to reflect
func ListTriggers(cfg *config.Config) string {
	var sb strings.Builder
	sb.WriteString("When to Reflect\n")
	sb.WriteString("===============\n\n")

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

	sb.WriteString("💬 Prompted:\n")
	for _, t := range cfg.Triggers.Prompted {
		sb.WriteString(fmt.Sprintf("   • %s\n", t))
	}

	return sb.String()
}

// GetTemplate returns reflection template for depth level
func GetTemplate(cfg *config.Config, depth string) string {
	d, ok := cfg.Depth[depth]
	if !ok {
		return fmt.Sprintf("Unknown depth: %s\nValid depths: %v", depth, cfg.GetDepthNames())
	}

	var sb strings.Builder
	date := time.Now().Format("2006-01-02")

	switch depth {
	case "quick":
		sb.WriteString(fmt.Sprintf("## Quick Reflection - %s\n\n", date))
		sb.WriteString("**Done:** [what was accomplished]\n")
		sb.WriteString("**Learned:** [what was learned]\n")

	case "standard":
		sb.WriteString(fmt.Sprintf("## Session Reflection - %s\n\n", date))
		for _, dimName := range d.Dimensions {
			if dim, ok := cfg.Dimensions[dimName]; ok {
				sb.WriteString(fmt.Sprintf("### %s\n\n", dim.Name))
				sb.WriteString("[Answer here]\n\n")
			}
		}
		sb.WriteString("### Next Session\n\n")
		sb.WriteString("[What to continue]\n")

	case "deep":
		sb.WriteString(fmt.Sprintf("## Deep Reflection - %s\n\n", date))
		sb.WriteString("### Session Summary\n\n")
		sb.WriteString("**Duration:** [time]\n")
		sb.WriteString("**Focus:** [main work area]\n\n")

		for _, dimName := range d.Dimensions {
			if dim, ok := cfg.Dimensions[dimName]; ok {
				sb.WriteString(fmt.Sprintf("### %s\n\n", dim.Name))
				sb.WriteString("[Answer here]\n\n")
			}
		}

		sb.WriteString("### Integration Candidates\n\n")
		sb.WriteString("- [ ] [insight 1]\n")
		sb.WriteString("- [ ] [insight 2]\n\n")
		sb.WriteString("### For Next Session\n\n")
		sb.WriteString("[Next steps]\n")
	}

	return sb.String()
}

// ListIntegration returns integration signals
func ListIntegration(cfg *config.Config) string {
	var sb strings.Builder
	sb.WriteString("Integration Signals\n")
	sb.WriteString("===================\n\n")

	sb.WriteString("✅ Integrate When:\n")
	for _, s := range cfg.Integration.Signals {
		sb.WriteString(fmt.Sprintf("   • %s\n", s))
	}
	sb.WriteString("\n")

	sb.WriteString("⚠️  Skip When:\n")
	for _, s := range cfg.Integration.SkipWhen {
		sb.WriteString(fmt.Sprintf("   • %s\n", s))
	}

	return sb.String()
}

// Guide returns the reflection process guide
func Guide(cfg *config.Config) string {
	var sb strings.Builder
	sb.WriteString("Reflection Process Guide\n")
	sb.WriteString("========================\n\n")

	sb.WriteString("1. PAUSE - Create space, stop doing\n\n")

	sb.WriteString("2. CHOOSE DEPTH\n")
	sb.WriteString("   • Quick (5 min) - short session\n")
	sb.WriteString("   • Standard (15 min) - work session\n")
	sb.WriteString("   • Deep (30+ min) - breakthrough/significant\n\n")

	sb.WriteString("3. WALK THROUGH DIMENSIONS\n")
	for _, name := range []string{"accomplishments", "learning", "patterns", "quality", "growth"} {
		if dim, ok := cfg.Dimensions[name]; ok {
			sb.WriteString(fmt.Sprintf("   • %s: %s\n", dim.Name, dim.Questions[0]))
		}
	}
	sb.WriteString("\n")

	sb.WriteString("4. NOTICE INTEGRATION SIGNALS\n")
	sb.WriteString("   Anything worth integrating to identity?\n\n")

	sb.WriteString("5. DECIDE OUTPUT\n")
	sb.WriteString("   • Mental note (quick)\n")
	sb.WriteString("   • Journal entry (standard/deep)\n")
	sb.WriteString("   • Integration proposal (deep with signals)\n")

	return sb.String()
}
