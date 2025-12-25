// ============================================================================
// METADATA - Code Generation Logic
// ============================================================================
// Purpose: Decision tree navigation and code file helpers
// Biblical: Proverbs 25:2 - "the honour of kings is to search out a matter"

package codegen

import (
	"fmt"
	"strings"

	"github.com/creativeworkzstudio/claude-global/skills/create-code/scripts/pkg/config"
)

// ============================================================================
// BODY - Languages
// ============================================================================

// ListLanguages returns formatted list of languages
func ListLanguages(cfg *config.Config) string {
	var sb strings.Builder
	sb.WriteString("Programming Languages\n")
	sb.WriteString("=====================\n\n")

	order := []string{"go", "c", "bash", "python"}
	for _, name := range order {
		if lang, ok := cfg.GetLanguage(name); ok {
			sb.WriteString(fmt.Sprintf("%-12s  %s\n", name, lang.Name))
			sb.WriteString(fmt.Sprintf("              %s\n", lang.Description))
			sb.WriteString(fmt.Sprintf("              Extension: %s\n", lang.Extension))
			sb.WriteString(fmt.Sprintf("              Template: %s\n\n", lang.Template))
		}
	}

	return sb.String()
}

// GetLanguageInfo returns detailed info for a specific language
func GetLanguageInfo(cfg *config.Config, langName string) string {
	lang, ok := cfg.GetLanguage(langName)
	if !ok {
		return fmt.Sprintf("Unknown language: %s\n\nAvailable languages: %s",
			langName, strings.Join(cfg.GetLanguageNames(), ", "))
	}

	var sb strings.Builder
	sb.WriteString(fmt.Sprintf("%s\n", lang.Name))
	sb.WriteString(strings.Repeat("=", len(lang.Name)) + "\n\n")
	sb.WriteString(fmt.Sprintf("Description:   %s\n", lang.Description))
	sb.WriteString(fmt.Sprintf("Extension:     %s\n", lang.Extension))
	sb.WriteString(fmt.Sprintf("Comment Style: %s\n", lang.CommentStyle))
	sb.WriteString(fmt.Sprintf("Template:      %s\n\n", lang.Template))

	sb.WriteString("Use when:\n")
	for _, use := range lang.UseWhen {
		sb.WriteString(fmt.Sprintf("  • %s\n", use))
	}

	return sb.String()
}

// ============================================================================
// BODY - Decision Tree
// ============================================================================

// ShowTree returns the complete decision tree as text
func ShowTree(cfg *config.Config) string {
	var sb strings.Builder
	sb.WriteString("Language Decision Tree\n")
	sb.WriteString("======================\n\n")
	sb.WriteString("Walk through to find the right language:\n\n")

	// Start from root
	rootName := cfg.DecisionTree.Root
	renderNode(&sb, cfg, rootName, 0, "")

	sb.WriteString("\nComment Styles:\n")
	sb.WriteString("  // block   - C-style (Go, C)\n")
	sb.WriteString("  # line     - Hash comments (Bash, Python)\n")

	return sb.String()
}

// renderNode recursively renders a decision tree node
func renderNode(sb *strings.Builder, cfg *config.Config, nodeName string, depth int, prefix string) {
	node, ok := cfg.GetDecisionNode(nodeName)
	if !ok {
		// This is a leaf - language
		if lang, ok := cfg.GetLanguage(nodeName); ok {
			sb.WriteString(fmt.Sprintf("%s└─ → %s (%s)\n", prefix, lang.Name, lang.Extension))
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
			// Leaf node - language
			if lang, ok := cfg.GetLanguage(opt.Next); ok {
				sb.WriteString(fmt.Sprintf("%s%s %s → %s (%s)\n",
					prefix, connector, opt.Answer, lang.Name, lang.Extension))
			}
		}
	}
}

// WalkTree interactively walks the decision tree (returns each step)
func WalkTree(cfg *config.Config, nodeName string) (string, []config.Option, bool) {
	node, ok := cfg.GetDecisionNode(nodeName)
	if !ok {
		// Leaf node - check if it's a language
		if lang, ok := cfg.GetLanguage(nodeName); ok {
			return fmt.Sprintf("Result: %s\nTemplate: %s", lang.Name, lang.Template), nil, true
		}
		return "Unknown node", nil, true
	}

	return node.Question, node.Options, false
}

// ============================================================================
// BODY - 4-Block Structure
// ============================================================================

// ShowBlocks returns the 4-block structure documentation
func ShowBlocks(cfg *config.Config) string {
	var sb strings.Builder
	sb.WriteString("4-Block Code Structure\n")
	sb.WriteString("======================\n\n")

	sb.WriteString("┌─────────────────────────────────────────────────────────────────┐\n")
	sb.WriteString("│ METADATA   │ Identity, purpose, health scoring tiers           │\n")
	sb.WriteString("├─────────────────────────────────────────────────────────────────┤\n")
	sb.WriteString("│ SETUP      │ Imports, constants, variables, types              │\n")
	sb.WriteString("├─────────────────────────────────────────────────────────────────┤\n")
	sb.WriteString("│ BODY       │ Core logic - helpers, operations, public APIs     │\n")
	sb.WriteString("├─────────────────────────────────────────────────────────────────┤\n")
	sb.WriteString("│ CLOSING    │ Validation, execution, cleanup                    │\n")
	sb.WriteString("└─────────────────────────────────────────────────────────────────┘\n\n")

	for _, name := range cfg.GetBlockNames() {
		if block, ok := cfg.GetBlock(name); ok {
			sb.WriteString(fmt.Sprintf("%s - %s\n", strings.ToUpper(block.Name), block.Purpose))
			sb.WriteString("  Contains: " + strings.Join(block.Contains, ", ") + "\n\n")
		}
	}

	sb.WriteString("Boundary Markers:\n")
	sb.WriteString("  Comments mark block boundaries - enables grep/sed operations\n")
	sb.WriteString("  Example: // METADATA, // SETUP, // BODY, // CLOSING\n")

	return sb.String()
}

// ============================================================================
// BODY - Guide
// ============================================================================

// Guide returns the complete code creation guide
func Guide(cfg *config.Config) string {
	var sb strings.Builder
	sb.WriteString("Code File Creation Guide\n")
	sb.WriteString("========================\n\n")

	sb.WriteString("The Creation Process (Block-by-Block):\n\n")
	sb.WriteString("1. DETERMINE LANGUAGE - Walk the decision tree\n")
	sb.WriteString("2. COPY TEMPLATE      - cp bereshit/word/seed/code/[lang]/template.[ext] path/to/new.[ext]\n")
	sb.WriteString("3. CHANGE PRAGMA      - #!omni template → #!omni code\n")
	sb.WriteString("4. FILL METADATA      - Key, purpose, health scoring, biblical foundation\n")
	sb.WriteString("5. BUILD SETUP        - Imports, constants, types\n")
	sb.WriteString("6. DEVELOP BODY       - Core logic, following org chart\n")
	sb.WriteString("7. COMPLETE CLOSING   - Validation, execution, cleanup\n")
	sb.WriteString("8. VERIFY             - All blocks present, compiles, tests pass\n\n")

	sb.WriteString("Block-by-Block Principle:\n")
	sb.WriteString("  Work ONE BLOCK at a time - each block is a unit of work.\n")
	sb.WriteString("  Not section-by-section within block. Not wholesale across blocks.\n\n")

	sb.WriteString("Commands:\n")
	sb.WriteString("  code languages       - List languages\n")
	sb.WriteString("  code language NAME   - Show language details\n")
	sb.WriteString("  code tree            - Show decision tree\n")
	sb.WriteString("  code blocks          - Show 4-block structure\n")
	sb.WriteString("  code guide           - Show this guide\n")

	return sb.String()
}
