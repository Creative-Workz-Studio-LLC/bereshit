// ============================================================================
// METADATA - Documentation Generation Logic
// ============================================================================
// Purpose: Decision tree navigation and documentation helpers
// Biblical: Proverbs 25:2 - "the honour of kings is to search out a matter"

package docgen

import (
	"fmt"
	"strings"

	"github.com/creativeworkzstudio/claude-global/skills/create-documentation/scripts/pkg/config"
)

// ============================================================================
// BODY - Document Types
// ============================================================================

// ListTypes returns formatted list of document types
func ListTypes(cfg *config.Config) string {
	var sb strings.Builder
	sb.WriteString("Document Types\n")
	sb.WriteString("==============\n\n")

	order := []string{"base", "research", "index", "community"}
	for _, name := range order {
		if dt, ok := cfg.GetType(name); ok {
			sb.WriteString(fmt.Sprintf("%-12s  %s\n", name, dt.Name))
			sb.WriteString(fmt.Sprintf("              %s\n", dt.Description))
			sb.WriteString(fmt.Sprintf("              Strictness: %s\n", dt.Strictness))
			sb.WriteString(fmt.Sprintf("              Template: %s\n\n", dt.Template))
		}
	}

	return sb.String()
}

// GetTypeInfo returns detailed info for a specific type
func GetTypeInfo(cfg *config.Config, typeName string) string {
	dt, ok := cfg.GetType(typeName)
	if !ok {
		return fmt.Sprintf("Unknown document type: %s\n\nAvailable types: %s",
			typeName, strings.Join(cfg.GetTypeNames(), ", "))
	}

	var sb strings.Builder
	sb.WriteString(fmt.Sprintf("%s\n", dt.Name))
	sb.WriteString(strings.Repeat("=", len(dt.Name)) + "\n\n")
	sb.WriteString(fmt.Sprintf("Description: %s\n", dt.Description))
	sb.WriteString(fmt.Sprintf("When to use: %s\n", dt.WhenToUse))
	sb.WriteString(fmt.Sprintf("Strictness:  %s\n", dt.Strictness))
	sb.WriteString(fmt.Sprintf("Template:    %s\n", dt.Template))

	return sb.String()
}

// ============================================================================
// BODY - Decision Tree
// ============================================================================

// ShowTree returns the complete decision tree as text
func ShowTree(cfg *config.Config) string {
	var sb strings.Builder
	sb.WriteString("Documentation Decision Tree\n")
	sb.WriteString("===========================\n\n")
	sb.WriteString("Walk through to find the right document type:\n\n")

	// Start from root
	rootName := cfg.DecisionTree.Root
	renderNode(&sb, cfg, rootName, 0, "")

	sb.WriteString("\nStrictness Levels:\n")
	sb.WriteString("  T (Tight)  - Follow structure exactly\n")
	sb.WriteString("  G (Guided) - Adapt thoughtfully\n")
	sb.WriteString("  F (Free)   - Principles only\n")

	return sb.String()
}

// renderNode recursively renders a decision tree node
func renderNode(sb *strings.Builder, cfg *config.Config, nodeName string, depth int, prefix string) {
	node, ok := cfg.GetDecisionNode(nodeName)
	if !ok {
		// This is a leaf - document type
		if dt, ok := cfg.GetType(nodeName); ok {
			sb.WriteString(fmt.Sprintf("%s└─ → %s (%s)\n", prefix, dt.Name, dt.Strictness))
		}
		return
	}

	sb.WriteString(fmt.Sprintf("%s%s\n", prefix, node.Question))

	for i, opt := range node.Options {
		isLast := i == len(node.Options)-1
		connector := "├─"
		childPrefix := prefix + "│  "
		if isLast {
			connector = "└─"
			childPrefix = prefix + "   "
		}

		// Check if next is a node or leaf
		if _, ok := cfg.GetDecisionNode(opt.Next); ok {
			sb.WriteString(fmt.Sprintf("%s%s %s\n", prefix, connector, opt.Answer))
			renderNode(sb, cfg, opt.Next, depth+1, childPrefix)
		} else {
			// Leaf node - document type
			if dt, ok := cfg.GetType(opt.Next); ok {
				sb.WriteString(fmt.Sprintf("%s%s %s → %s (%s)\n",
					prefix, connector, opt.Answer, dt.Name, dt.Strictness))
			}
		}
	}
}

// WalkTree interactively walks the decision tree (returns each step)
func WalkTree(cfg *config.Config, nodeName string) (string, []config.Option, bool) {
	node, ok := cfg.GetDecisionNode(nodeName)
	if !ok {
		// Leaf node - check if it's a document type
		if dt, ok := cfg.GetType(nodeName); ok {
			return fmt.Sprintf("Result: %s\nTemplate: %s", dt.Name, dt.Template), nil, true
		}
		return "Unknown node", nil, true
	}

	return node.Question, node.Options, false
}

// ============================================================================
// BODY - 5-Block Structure
// ============================================================================

// ShowBlocks returns the 5-block structure documentation
func ShowBlocks(cfg *config.Config) string {
	var sb strings.Builder
	sb.WriteString("5-Block Documentation Structure\n")
	sb.WriteString("================================\n\n")

	sb.WriteString("┌─────────────────────────────────────────────────────────────────┐\n")
	sb.WriteString("│ METADATA   │ The catalog card - parseable identity for systems │\n")
	sb.WriteString("├─────────────────────────────────────────────────────────────────┤\n")
	sb.WriteString("│ HEADER     │ The cover - visible identity in 5 seconds         │\n")
	sb.WriteString("├─────────────────────────────────────────────────────────────────┤\n")
	sb.WriteString("│ CONTEXT    │ The introduction - prepares understanding         │\n")
	sb.WriteString("├─────────────────────────────────────────────────────────────────┤\n")
	sb.WriteString("│ CONTENT    │ The chapters - value organized for discovery      │\n")
	sb.WriteString("├─────────────────────────────────────────────────────────────────┤\n")
	sb.WriteString("│ FOOTER     │ The appendix - resources and grounding            │\n")
	sb.WriteString("└─────────────────────────────────────────────────────────────────┘\n\n")

	for _, name := range cfg.GetBlockNames() {
		if block, ok := cfg.GetBlock(name); ok {
			sb.WriteString(fmt.Sprintf("%s - %s\n", strings.ToUpper(block.Name), block.Purpose))
			sb.WriteString("  Contains: " + strings.Join(block.Contains, ", ") + "\n\n")
		}
	}

	return sb.String()
}

// ============================================================================
// BODY - Quality Standard
// ============================================================================

// ShowQuality returns the quality standard documentation
func ShowQuality(cfg *config.Config) string {
	var sb strings.Builder
	sb.WriteString("Quality Standard\n")
	sb.WriteString("================\n\n")
	sb.WriteString(fmt.Sprintf("Standard: %s\n\n", cfg.Quality.Standard))

	sb.WriteString("Dimensions:\n")
	for name, desc := range cfg.Quality.Dimensions {
		sb.WriteString(fmt.Sprintf("  %-15s %s\n", name+":", desc))
	}

	sb.WriteString(fmt.Sprintf("\nInsight: %s\n", cfg.Quality.Insight))

	return sb.String()
}

// ============================================================================
// BODY - Toolkit
// ============================================================================

// ShowToolkit returns the AsciiDoc toolkit reference
func ShowToolkit(cfg *config.Config) string {
	var sb strings.Builder
	sb.WriteString("AsciiDoc Toolkit\n")
	sb.WriteString("================\n\n")

	for name, item := range cfg.Toolkit {
		sb.WriteString(fmt.Sprintf("%s\n", item.Name))
		sb.WriteString(fmt.Sprintf("  Syntax:  %s\n", item.Syntax))
		sb.WriteString(fmt.Sprintf("  When:    %s\n", item.WhenToUse))
		sb.WriteString(fmt.Sprintf("  Key:     %s\n\n", name))
	}

	return sb.String()
}

// ============================================================================
// BODY - Reading Levels
// ============================================================================

// ShowReading returns the layered reading documentation
func ShowReading(cfg *config.Config) string {
	var sb strings.Builder
	sb.WriteString("Layered Reading\n")
	sb.WriteString("===============\n\n")
	sb.WriteString("Write for multiple reading depths:\n\n")

	order := []string{"scan", "skim", "read", "study", "reference"}
	sb.WriteString(fmt.Sprintf("%-12s %-8s %-25s %s\n", "Level", "Time", "Focus", "Purpose"))
	sb.WriteString(strings.Repeat("-", 70) + "\n")

	for _, name := range order {
		if level, ok := cfg.Reading[name]; ok {
			sb.WriteString(fmt.Sprintf("%-12s %-8s %-25s %s\n",
				level.Name, level.Time, level.Focus, level.Purpose))
		}
	}

	return sb.String()
}

// ============================================================================
// BODY - Guide
// ============================================================================

// Guide returns the complete documentation creation guide
func Guide(cfg *config.Config) string {
	var sb strings.Builder
	sb.WriteString("Documentation Creation Guide\n")
	sb.WriteString("============================\n\n")

	sb.WriteString("The Creation Process:\n\n")
	sb.WriteString("1. DETERMINE TYPE - Walk the decision tree\n")
	sb.WriteString("2. COPY TEMPLATE  - cp bereshit/word/seed/documentation/adoc/asciidoc-[type].adoc path/to/new.adoc\n")
	sb.WriteString("3. FILL METADATA  - Unique key, clear title, biblical foundation\n")
	sb.WriteString("4. WRITE HEADER   - Tagline, badges, quick nav\n")
	sb.WriteString("5. BUILD CONTEXT  - Overview, important concept, scope\n")
	sb.WriteString("6. DEVELOP CONTENT - Use toolkit throughout, tables, collapsibles\n")
	sb.WriteString("7. COMPLETE FOOTER - Biblical foundation, references, see also\n")
	sb.WriteString("8. VERIFY         - All blocks present, layered reading works\n\n")

	sb.WriteString("Commands:\n")
	sb.WriteString("  doc types      - List document types\n")
	sb.WriteString("  doc type NAME  - Show type details\n")
	sb.WriteString("  doc tree       - Show decision tree\n")
	sb.WriteString("  doc blocks     - Show 5-block structure\n")
	sb.WriteString("  doc quality    - Show quality standard\n")
	sb.WriteString("  doc toolkit    - Show AsciiDoc toolkit\n")
	sb.WriteString("  doc reading    - Show layered reading\n")
	sb.WriteString("  doc guide      - Show this guide\n")

	return sb.String()
}
