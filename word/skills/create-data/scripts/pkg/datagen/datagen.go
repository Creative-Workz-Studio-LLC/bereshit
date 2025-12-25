// ============================================================================
// METADATA - Data Generation Logic
// ============================================================================
// Purpose: Decision tree navigation and data file helpers
// Biblical: Proverbs 25:2 - "the honour of kings is to search out a matter"

package datagen

import (
	"fmt"
	"strings"

	"github.com/creativeworkzstudio/claude-global/skills/create-data/scripts/pkg/config"
)

// ============================================================================
// BODY - Data Formats
// ============================================================================

// ListFormats returns formatted list of data formats
func ListFormats(cfg *config.Config) string {
	var sb strings.Builder
	sb.WriteString("Data Formats\n")
	sb.WriteString("============\n\n")

	order := []string{"jsonc", "yaml", "toml", "json"}
	for _, name := range order {
		if df, ok := cfg.GetFormat(name); ok {
			sb.WriteString(fmt.Sprintf("%-12s  %s\n", name, df.Name))
			sb.WriteString(fmt.Sprintf("              %s\n", df.Description))
			sb.WriteString(fmt.Sprintf("              Extension: %s\n", df.Extension))
			sb.WriteString(fmt.Sprintf("              Template: %s\n\n", df.Template))
		}
	}

	return sb.String()
}

// GetFormatInfo returns detailed info for a specific format
func GetFormatInfo(cfg *config.Config, formatName string) string {
	df, ok := cfg.GetFormat(formatName)
	if !ok {
		return fmt.Sprintf("Unknown data format: %s\n\nAvailable formats: %s",
			formatName, strings.Join(cfg.GetFormatNames(), ", "))
	}

	var sb strings.Builder
	sb.WriteString(fmt.Sprintf("%s\n", df.Name))
	sb.WriteString(strings.Repeat("=", len(df.Name)) + "\n\n")
	sb.WriteString(fmt.Sprintf("Description:   %s\n", df.Description))
	sb.WriteString(fmt.Sprintf("Extension:     %s\n", df.Extension))
	sb.WriteString(fmt.Sprintf("Comment Style: %s\n", df.CommentStyle))
	sb.WriteString(fmt.Sprintf("Template:      %s\n\n", df.Template))

	sb.WriteString("Use when:\n")
	for _, use := range df.UseWhen {
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
	sb.WriteString("Data Format Decision Tree\n")
	sb.WriteString("=========================\n\n")
	sb.WriteString("Walk through to find the right data format:\n\n")

	// Start from root
	rootName := cfg.DecisionTree.Root
	renderNode(&sb, cfg, rootName, 0, "")

	sb.WriteString("\nComment Styles:\n")
	sb.WriteString("  // line    - Single-line comments (JSONC, Go)\n")
	sb.WriteString("  # line     - Hash comments (YAML, TOML, Bash)\n")
	sb.WriteString("  none       - No comments (JSON)\n")

	return sb.String()
}

// renderNode recursively renders a decision tree node
func renderNode(sb *strings.Builder, cfg *config.Config, nodeName string, depth int, prefix string) {
	node, ok := cfg.GetDecisionNode(nodeName)
	if !ok {
		// This is a leaf - data format
		if df, ok := cfg.GetFormat(nodeName); ok {
			sb.WriteString(fmt.Sprintf("%s└─ → %s (%s)\n", prefix, df.Name, df.Extension))
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
			// Leaf node - data format
			if df, ok := cfg.GetFormat(opt.Next); ok {
				sb.WriteString(fmt.Sprintf("%s%s %s → %s (%s)\n",
					prefix, connector, opt.Answer, df.Name, df.Extension))
			}
		}
	}
}

// WalkTree interactively walks the decision tree (returns each step)
func WalkTree(cfg *config.Config, nodeName string) (string, []config.Option, bool) {
	node, ok := cfg.GetDecisionNode(nodeName)
	if !ok {
		// Leaf node - check if it's a data format
		if df, ok := cfg.GetFormat(nodeName); ok {
			return fmt.Sprintf("Result: %s\nTemplate: %s", df.Name, df.Template), nil, true
		}
		return "Unknown node", nil, true
	}

	return node.Question, node.Options, false
}

// ============================================================================
// BODY - 3-Block Structure
// ============================================================================

// ShowBlocks returns the 3-block structure documentation
func ShowBlocks(cfg *config.Config) string {
	var sb strings.Builder
	sb.WriteString("3-Block Data Structure\n")
	sb.WriteString("======================\n\n")

	sb.WriteString("┌─────────────────────────────────────────────────────────────────┐\n")
	sb.WriteString("│ METADATA   │ Identity - key, purpose, biblical foundation      │\n")
	sb.WriteString("├─────────────────────────────────────────────────────────────────┤\n")
	sb.WriteString("│ CONTENT    │ The actual data - structured information          │\n")
	sb.WriteString("├─────────────────────────────────────────────────────────────────┤\n")
	sb.WriteString("│ CLOSING    │ Related references and documentation              │\n")
	sb.WriteString("└─────────────────────────────────────────────────────────────────┘\n\n")

	for _, name := range cfg.GetBlockNames() {
		if block, ok := cfg.GetBlock(name); ok {
			sb.WriteString(fmt.Sprintf("%s - %s\n", strings.ToUpper(block.Name), block.Purpose))
			sb.WriteString("  Contains: " + strings.Join(block.Contains, ", ") + "\n\n")
		}
	}

	sb.WriteString("Boundary Markers:\n")
	sb.WriteString("  Comments mark block boundaries - enables grep/sed operations\n")
	sb.WriteString("  Example: // METADATA, // CONTENT, // CLOSING\n")

	return sb.String()
}

// ============================================================================
// BODY - Guide
// ============================================================================

// Guide returns the complete data creation guide
func Guide(cfg *config.Config) string {
	var sb strings.Builder
	sb.WriteString("Data File Creation Guide\n")
	sb.WriteString("========================\n\n")

	sb.WriteString("The Creation Process (Block-by-Block):\n\n")
	sb.WriteString("1. DETERMINE FORMAT  - Walk the decision tree\n")
	sb.WriteString("2. COPY TEMPLATE     - cp bereshit/word/seed/data/[format]/template.[ext] path/to/new.[ext]\n")
	sb.WriteString("3. CHANGE PRAGMA     - #!omni template → #!omni data\n")
	sb.WriteString("4. FILL METADATA     - Unique key, purpose, biblical foundation\n")
	sb.WriteString("5. DEVELOP CONTENT   - Add the actual data\n")
	sb.WriteString("6. COMPLETE CLOSING  - Related references\n")
	sb.WriteString("7. VERIFY            - All blocks present, structure valid\n\n")

	sb.WriteString("Block-by-Block Principle:\n")
	sb.WriteString("  Work ONE BLOCK at a time - each block is a unit of work.\n")
	sb.WriteString("  Not section-by-section within block. Not wholesale across blocks.\n\n")

	sb.WriteString("Commands:\n")
	sb.WriteString("  data formats       - List data formats\n")
	sb.WriteString("  data format NAME   - Show format details\n")
	sb.WriteString("  data tree          - Show decision tree\n")
	sb.WriteString("  data blocks        - Show 3-block structure\n")
	sb.WriteString("  data guide         - Show this guide\n")

	return sb.String()
}
