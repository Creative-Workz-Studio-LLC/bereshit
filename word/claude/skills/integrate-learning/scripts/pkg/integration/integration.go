// ============================================================================
// METADATA - Integration Package
// ============================================================================
// Purpose: Integration proposal and analysis logic
// Biblical: Proverbs 4:7 - "Wisdom is the principal thing"

package integration

import (
	"fmt"
	"strings"
	"time"

	"cws.studio/skills/integrate-learning/scripts/pkg/config"
)

// ============================================================================
// SETUP - Types
// ============================================================================

// Proposal represents an integration proposal
type Proposal struct {
	Date        string
	Source      string
	Level       string
	Summary     string
	Evidence    []string
	QualityCheck map[string]bool
	ProposedText string
	TargetFile  string
	TargetSection string
}

// QualityResult represents quality assessment
type QualityResult struct {
	Passed   bool
	Criteria map[string]bool
	Warnings []string
}

// ============================================================================
// BODY - Core Operations
// ============================================================================

// NewProposal creates a new integration proposal
func NewProposal(cfg *config.Config, level, source, summary string) *Proposal {
	return &Proposal{
		Date:         time.Now().Format("2006-01-02"),
		Source:       source,
		Level:        level,
		Summary:      summary,
		Evidence:     []string{},
		QualityCheck: make(map[string]bool),
	}
}

// AddEvidence adds evidence to a proposal
func (p *Proposal) AddEvidence(evidence string) {
	p.Evidence = append(p.Evidence, evidence)
}

// CheckQuality assesses proposal against quality criteria
func (p *Proposal) CheckQuality(cfg *config.Config) *QualityResult {
	result := &QualityResult{
		Passed:   true,
		Criteria: make(map[string]bool),
		Warnings: []string{},
	}

	// Check evidence count
	hasEnoughEvidence := len(p.Evidence) >= cfg.Quality.MinObservations
	result.Criteria["min_observations"] = hasEnoughEvidence
	if !hasEnoughEvidence {
		result.Warnings = append(result.Warnings,
			fmt.Sprintf("Only %d observations (need %d)", len(p.Evidence), cfg.Quality.MinObservations))
	}

	// Check for warning signs
	for _, warning := range cfg.Quality.WarningSigns {
		if p.hasWarningSign(warning) {
			result.Warnings = append(result.Warnings, warning)
		}
	}

	// Overall pass/fail
	result.Passed = hasEnoughEvidence && len(result.Warnings) == 0

	return result
}

// hasWarningSign checks if proposal exhibits a warning sign
func (p *Proposal) hasWarningSign(warning string) bool {
	switch warning {
	case "single_observation":
		return len(p.Evidence) == 1
	default:
		return false
	}
}

// Format returns the proposal as formatted text
func (p *Proposal) Format() string {
	var sb strings.Builder

	sb.WriteString("## Integration Proposal\n\n")
	sb.WriteString(fmt.Sprintf("**Date:** %s\n", p.Date))
	sb.WriteString(fmt.Sprintf("**Source:** %s\n", p.Source))
	sb.WriteString(fmt.Sprintf("**Level:** %s\n\n", p.Level))

	sb.WriteString("### The Learning\n\n")
	sb.WriteString(p.Summary + "\n\n")

	sb.WriteString("### Evidence\n\n")
	for _, e := range p.Evidence {
		sb.WriteString(fmt.Sprintf("- %s\n", e))
	}
	sb.WriteString("\n")

	sb.WriteString("### Quality Check\n\n")
	sb.WriteString("| Criterion | Status |\n")
	sb.WriteString("|-----------|--------|\n")
	for criterion, passed := range p.QualityCheck {
		status := "❌"
		if passed {
			status = "✅"
		}
		sb.WriteString(fmt.Sprintf("| %s | %s |\n", criterion, status))
	}
	sb.WriteString("\n")

	if p.ProposedText != "" {
		sb.WriteString("### Proposed Text\n\n")
		sb.WriteString(p.ProposedText + "\n\n")
	}

	if p.TargetFile != "" {
		sb.WriteString("### Target Location\n\n")
		target := p.TargetFile
		if p.TargetSection != "" {
			target += ":" + p.TargetSection
		}
		sb.WriteString(target + "\n")
	}

	return sb.String()
}

// ListLevels returns formatted list of integration levels
func ListLevels(cfg *config.Config) string {
	var sb strings.Builder
	sb.WriteString("Integration Levels\n")
	sb.WriteString("==================\n\n")

	for name, level := range cfg.Levels {
		sb.WriteString(fmt.Sprintf("📊 %s (%s)\n", level.Name, name))
		sb.WriteString(fmt.Sprintf("   %s\n", level.Description))
		sb.WriteString(fmt.Sprintf("   Target: %s\n", level.Target))
		sb.WriteString(fmt.Sprintf("   Requires: %s\n\n", strings.Join(level.Requires, ", ")))
	}

	return sb.String()
}

// ListQuestions returns integration questions for reflection
func ListQuestions(cfg *config.Config) string {
	var sb strings.Builder
	sb.WriteString("Integration Questions\n")
	sb.WriteString("=====================\n\n")
	sb.WriteString("Ask these before integrating:\n\n")

	for i, q := range cfg.Process.Questions {
		sb.WriteString(fmt.Sprintf("%d. %s\n", i+1, q))
	}

	return sb.String()
}

// ListProcess returns the integration process steps
func ListProcess(cfg *config.Config) string {
	var sb strings.Builder
	sb.WriteString("Integration Process\n")
	sb.WriteString("===================\n\n")

	for i, step := range cfg.Process.Steps {
		sb.WriteString(fmt.Sprintf("%d. %s\n", i+1, formatStep(step)))
	}

	return sb.String()
}

// formatStep makes step names human-readable
func formatStep(step string) string {
	step = strings.ReplaceAll(step, "_", " ")
	if len(step) > 0 {
		step = strings.ToUpper(step[:1]) + step[1:]
	}
	return step
}

// GetTemplate returns a blank proposal template
func GetTemplate(cfg *config.Config, level string) string {
	lvl, ok := cfg.GetLevel(level)
	if !ok {
		return fmt.Sprintf("Unknown level: %s\nValid levels: %v", level, cfg.GetLevelNames())
	}

	var sb strings.Builder
	sb.WriteString("## Integration Proposal\n\n")
	sb.WriteString(fmt.Sprintf("**Date:** %s\n", time.Now().Format("2006-01-02")))
	sb.WriteString("**Source:** [journal entry or pattern file]\n")
	sb.WriteString(fmt.Sprintf("**Level:** %s\n\n", lvl.Name))

	sb.WriteString("### The Learning\n\n")
	sb.WriteString("[Summary of what was learned]\n\n")

	sb.WriteString("### Evidence\n\n")
	sb.WriteString("- [Observation 1]\n")
	sb.WriteString("- [Observation 2]\n")
	sb.WriteString("- [Observation 3]\n\n")

	sb.WriteString("### Quality Check\n\n")
	sb.WriteString("| Criterion | Status |\n")
	sb.WriteString("|-----------|--------|\n")
	sb.WriteString("| Scripture alignment | ⬜ |\n")
	sb.WriteString("| Repeated observation | ⬜ |\n")
	sb.WriteString("| Tested in practice | ⬜ |\n\n")

	sb.WriteString("### Proposed Text\n\n")
	sb.WriteString("[Exact text to add to target document]\n\n")

	sb.WriteString("### Target Location\n\n")
	sb.WriteString(fmt.Sprintf("%s:[section name]\n", lvl.Target))

	return sb.String()
}
