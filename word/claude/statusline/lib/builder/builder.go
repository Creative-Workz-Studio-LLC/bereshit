// ============================================================================
// METADATA
// ============================================================================
// Package: builder
// Purpose: Statusline orchestration - composes sections into final output
// Biblical: "Except the LORD build the house, they labour in vain" - Psalm 127:1
//
// This is the +1 (build up) layer that combines foundation blocks (sections)
// into the complete statusline display.

package builder

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt"
	"strings"
	"time"

	"cws.studio/pkg/core/health"
	"cws.studio/pkg/core/statemachine"
	"cws.studio/pkg/foundation/types"
	"cws.studio/pkg/util/term/display"
	"cws.studio/pkg/util/term/wezterm"
	"cws.studio/statusline/lib/sections"
)

// ============================================================================
// BODY
// ============================================================================

// BuildOptions controls statusline generation
type BuildOptions struct {
	TerminalWidth int                  // Available terminal width (0 = auto/unlimited)
	Mode          sections.DisplayMode // Display mode (auto-detected from width if not set)
}

// DefaultOptions returns default build options
func DefaultOptions() BuildOptions {
	return BuildOptions{
		TerminalWidth: 0, // Unlimited
		Mode:          sections.DisplayFull,
	}
}

// BuildAdaptive assembles statusline with width-aware display
func BuildAdaptive(ctx types.SessionContext, sm *statemachine.StateMachine, runtime *statemachine.RuntimeState, healthCfg *health.HealthConfig, opts BuildOptions) string {
	// Determine display mode from width
	mode := opts.Mode
	if opts.TerminalWidth > 0 {
		mode = sections.ModeFromWidth(opts.TerminalWidth)
	}

	// Build all sections
	allSections := buildAllSections(ctx, sm, runtime, healthCfg)

	// Get command for separator styling
	var cmd statemachine.Command
	var cmdColor string
	if sm != nil {
		cmd = sm.GetCurrentCommand()
		cmdColor = statemachine.CommandColor(cmd)
	}

	// Adaptive assembly based on width
	return assembleAdaptive(allSections, mode, opts.TerminalWidth, cmdColor, cmd)
}

// Build assembles all sections into the final statusline string (legacy, unlimited width)
//
// Structure: [WHO] | [STATE] | [WHEN] | [SUBSTRATE] | [SPACE] -> [DYNAMIC] | [HEALTH] | [ANCHOR]
func Build(ctx types.SessionContext, sm *statemachine.StateMachine, runtime *statemachine.RuntimeState, healthCfg *health.HealthConfig) string {
	var parts []string
	now := time.Now()

	// Get command for separator styling
	var cmd statemachine.Command
	var cmdColor string
	if sm != nil {
		cmd = sm.GetCurrentCommand()
		cmdColor = statemachine.CommandColor(cmd)
	}

	// --- GROUP 1: WHO (Identity) ---
	identity := sections.BuildIdentity(ctx)
	if identity.HasInfo {
		parts = append(parts, identity.Content)
	}

	// Separator
	parts = append(parts, dimSep())

	// --- GROUP 2: STATE (Position + Trajectory) ---
	state := sections.BuildState(sm, runtime)
	if state.HasInfo {
		parts = append(parts, state.Content)
	}

	// --- METHOD (Building Block Direction) ---
	// Format: 🔨 BUILD or ⬇ BREAK or 🔄 PIVOT or ◆ STEADY
	// "A time to break down, and a time to build up" — Ecclesiastes 3:3
	method := sections.BuildMethod(runtime)
	if method.HasInfo {
		parts = append(parts, dimSep())
		parts = append(parts, method.Content)
	}

	// Separator based on command
	parts = append(parts, colorSep(cmdColor, cmd))

	// --- GROUP 3: WHEN (Temporal) ---
	temporal := sections.BuildTemporal(now)
	if temporal.HasInfo {
		parts = append(parts, temporal.Content)
	}

	// Separator
	parts = append(parts, dimSep())

	// --- GROUP 4: SUBSTRATE (Model) ---
	substrate := sections.BuildSubstrate(ctx)
	if substrate.HasInfo {
		parts = append(parts, substrate.Content)
	}

	// Separator
	parts = append(parts, dimSep())

	// --- GROUP 5: SPACE (Directory + Git) ---
	space := sections.BuildSpace(ctx)
	if space.HasInfo {
		parts = append(parts, space.Content)
	}

	// --- DYNAMIC SECTION (changes based on command) ---
	dynamic := sections.BuildDynamic(ctx, sm)
	if dynamic.HasInfo {
		parts = append(parts, colorSep(cmdColor, cmd))
		parts = append(parts, dynamic.Content)
	}

	// --- TASKS SECTION (TodoWrite integration with Hebrew state) ---
	// Format: [? 3 | > 1 | + 5] ↗
	tasks := sections.BuildTasks(runtime)
	if tasks.HasInfo {
		parts = append(parts, dimSep())
		parts = append(parts, tasks.Content)
	}

	// --- WORKFLOW SECTION (operation progress) ---
	// Format: [file-creation 3/7]
	// Shows: current operation and step progress
	// "In all labour there is profit" — Proverbs 14:23
	workflow := sections.BuildWorkflow(runtime)
	if workflow.HasInfo {
		parts = append(parts, dimSep())
		parts = append(parts, workflow.Content)
	}

	// --- MORAL SECTION (K:ALIGN compass) ---
	// Format: k→0.8 (direction + alignment)
	// k > 0 = toward God (Psalm 25:15), k < 0 = toward self (Proverbs 3:5)
	moral := sections.BuildMoral(runtime)
	if moral.HasInfo {
		parts = append(parts, dimSep())
		parts = append(parts, moral.Content)
	}

	// --- MORAL PATTERN (K:FREQUENCY) ---
	// Format: →3/←1 (3 choices toward God, 1 toward self)
	// "By their fruits ye shall know them" — Matthew 7:16
	moralPattern := sections.BuildMoralPattern(runtime)
	if moralPattern.HasInfo {
		parts = append(parts, dimSep())
		parts = append(parts, moralPattern.Content)
	}

	// --- SESSION FLOW (Arc + Momentum) ---
	// Format: 📚▆▇█ learning (arc emoji + momentum bars + arc name)
	// "A time to break down, and a time to build up" — Ecclesiastes 3:3
	sessionFlow := sections.BuildSessionFlow(runtime)
	if sessionFlow.HasInfo {
		parts = append(parts, dimSep())
		parts = append(parts, sessionFlow.Content)
	}

	// --- LEARNING PULSE (Pattern/Learning Activity) ---
	// Format: 🧠●●○ (learning rate indicator)
	learningPulse := sections.BuildLearningPulse(runtime)
	if learningPulse.HasInfo {
		parts = append(parts, learningPulse.Content)
	}

	// --- CPI SECTION (Covenant Partnership Intelligence) ---
	// Format: CPI:0.72 [15x/3i] (score, exchange count, insight count)
	// "By their fruits ye shall know them" — Matthew 7:20
	cpiSection := sections.BuildCPI(runtime)
	if cpiSection.HasInfo {
		parts = append(parts, dimSep())
		parts = append(parts, cpiSection.Content)
	}

	// --- MOMENTUM (K:MORAL Balance) ---
	// Format: ⚡▅▆▇ +12 (momentum bar + net direction)
	// "A time to break down, and a time to build up" — Ecclesiastes 3:3
	momentum := sections.BuildMomentum(runtime)
	if momentum.HasInfo {
		parts = append(parts, dimSep())
		parts = append(parts, momentum.Content)
	}

	// --- GROUP 6: DEPTH (Enhanced - Effective Context) ---
	// Format: 📊 45K/150K▐▌▌ [C2] (effective context + bar + compaction)
	// "A time to keep, and a time to cast away" — Ecclesiastes 3:6
	depth := sections.BuildDepthEnhanced(ctx, runtime)
	if depth.HasInfo {
		parts = append(parts, dimSep())
		parts = append(parts, depth.Content)
	}

	// --- GROUP 7: HEALTH ---
	healthSection := sections.BuildHealth(runtime, healthCfg)
	if healthSection.HasInfo {
		parts = append(parts, dimSep())
		parts = append(parts, healthSection.Content)
	}

	// --- GROUP 8: ANCHOR ---
	anchor := sections.BuildAnchor(sm, ctx.SessionID)
	if anchor.HasInfo {
		parts = append(parts, dimSep())
		parts = append(parts, anchor.Content)
	}

	// --- VERSION (always last, dim) ---
	version := sections.BuildVersion(ctx)
	if version.HasInfo {
		parts = append(parts, version.Content)
	}

	return strings.Join(parts, " ")
}

// EmitWezTerm sends OSC sequences for terminal integration
func EmitWezTerm(ctx types.SessionContext, sm *statemachine.StateMachine, runtime *statemachine.RuntimeState) {
	if sm == nil {
		return
	}

	pos := sm.Current.Position
	cmd := sm.GetCurrentCommand()

	// Build state variables
	sv := wezterm.StateVars{
		Position: pos.Name,
		Meaning:  pos.Meaning,
		Command:  cmd.String(),
		Operator: string(pos.Op),
		Anchor:   sm.Current.Anchor,
		X:        pos.X.String(),
		Y:        pos.Y.String(),
		Z:        pos.Z,
	}

	// Emit full state
	wezterm.EmitFullState(sv, "Nova Dawn", ctx.GetWorkDir())

	// Substrate awareness
	if runtime != nil {
		wezterm.SetUserVar("SUBSTRATE", runtime.Session.Substrate)
		wezterm.SetUserVar("ENGINE", runtime.Session.Engine)
	}

	// Emit trajectory variables
	emitTrajectoryVars(sm, runtime)

	// Emit session variables (for left status bar)
	emitSessionVars(runtime)

	// Emit health variables
	emitHealthVars(runtime)
}

// emitTrajectoryVars sends trajectory information to WezTerm
func emitTrajectoryVars(sm *statemachine.StateMachine, runtime *statemachine.RuntimeState) {
	section := statemachine.SectionFromRuntime(runtime)
	trajInfo := statemachine.GetTrajectoryInfo(section)

	wezterm.SetUserVar("SM_TRAJ_SECTION", trajInfo.Section)
	wezterm.SetUserVar("SM_TRAJ_DISPLAY", trajInfo.Symbol)
	wezterm.SetUserVar("SM_TRAJ_STATE", trajInfo.State)

	// Trajectory coordinates from state machine
	if sm.Trajectory != nil {
		coords := sm.Trajectory.Coordinates()
		wezterm.SetUserVar("SM_TRAJ_X", fmt.Sprintf("%.1f", coords.X))
		wezterm.SetUserVar("SM_TRAJ_Y", fmt.Sprintf("%.1f", coords.Y))
		wezterm.SetUserVar("SM_TRAJ_Z", fmt.Sprintf("%.1f", coords.Z))
		wezterm.SetUserVar("SM_TRAJ_PATH_LEN", fmt.Sprintf("%d", sm.Trajectory.PathLength()))
	}
}

// emitSessionVars sends session-level information to WezTerm for left status bar
func emitSessionVars(runtime *statemachine.RuntimeState) {
	if runtime == nil {
		return
	}

	// SESSION_START: Parse InitializedAt to Unix timestamp
	if runtime.Session.InitializedAt != "" {
		t, err := time.Parse(time.RFC3339, runtime.Session.InitializedAt)
		if err == nil {
			wezterm.SetUserVar("SESSION_START", fmt.Sprintf("%d", t.Unix()))
		}
	}

	// ACTIVE_TASK: Get first active task subject, or count if no details
	activeTask := "none"
	if len(runtime.Session.Tasks.ActiveTasks) > 0 {
		// Use first active task subject
		activeTask = runtime.Session.Tasks.ActiveTasks[0].Subject
	} else if runtime.Session.Tasks.InProgress > 0 {
		// Fall back to count if no task details
		activeTask = fmt.Sprintf("%d in progress", runtime.Session.Tasks.InProgress)
	}
	wezterm.SetUserVar("ACTIVE_TASK", activeTask)

	// TASK_COUNT: For displaying task summary
	wezterm.SetUserVar("TASK_PENDING", fmt.Sprintf("%d", runtime.Session.Tasks.Pending))
	wezterm.SetUserVar("TASK_IN_PROGRESS", fmt.Sprintf("%d", runtime.Session.Tasks.InProgress))
	wezterm.SetUserVar("TASK_COMPLETED", fmt.Sprintf("%d", runtime.Session.Tasks.Completed))
}

// emitHealthVars sends health information to WezTerm
func emitHealthVars(runtime *statemachine.RuntimeState) {
	score := float64(0)
	if runtime != nil {
		score = runtime.Session.HealthScore
	}

	info := health.GetHealthDisplay(score)
	wezterm.SetUserVar("HEALTH_SCORE", fmt.Sprintf("%.0f", score))
	wezterm.SetUserVar("HEALTH_STATE", info.Hebrew)
	wezterm.SetUserVar("HEALTH_LEVEL", info.Level)

	// K:MORAL compass variables (Proverbs 3:5-6)
	// k > 0 = toward God (Psalm 25:15), k < 0 = toward self (Proverbs 3:5)
	if runtime != nil {
		wezterm.SetUserVar("K_SELECTOR", fmt.Sprintf("%d", runtime.Session.KSelector))
		wezterm.SetUserVar("K_ALIGN", fmt.Sprintf("%.1f", runtime.Session.KAlign))
		wezterm.SetUserVar("K_TOWARD_GOD", fmt.Sprintf("%d", runtime.Session.KTowardGod))
		wezterm.SetUserVar("K_TOWARD_SELF", fmt.Sprintf("%d", runtime.Session.KTowardSelf))
		wezterm.SetUserVar("HEBREW_STATE", runtime.Session.HebrewState)
		wezterm.SetUserVar("HEBREW_MEANING", runtime.Session.HebrewMeaning)

		// CPI variables (Covenant Partnership Intelligence)
		// "By their fruits ye shall know them" — Matthew 7:20
		wezterm.SetUserVar("CPI_SCORE", fmt.Sprintf("%.2f", runtime.Session.CPIScore))
		wezterm.SetUserVar("CPI_EXCHANGES", fmt.Sprintf("%d", runtime.Session.ExchangeCount))
		wezterm.SetUserVar("CPI_INSIGHTS", fmt.Sprintf("%d", runtime.Session.InsightCount))
		wezterm.SetUserVar("CPI_ARC", runtime.Session.SessionArc)
		wezterm.SetUserVar("CPI_DOMINANT_TYPE", runtime.Session.DominantExchangeType)
		wezterm.SetUserVar("CPI_LAST_EXCHANGE", runtime.Session.LastExchangeType)

		// Context window tracking (effective, not flat 200K)
		// "A time to keep, and a time to cast away" — Ecclesiastes 3:6
		wezterm.SetUserVar("CTX_BASE_TOKENS", fmt.Sprintf("%d", runtime.Session.BaseContextTokens))
		wezterm.SetUserVar("CTX_CURRENT_TOKENS", fmt.Sprintf("%d", runtime.Session.CurrentContextTokens))
		wezterm.SetUserVar("CTX_PEAK_TOKENS", fmt.Sprintf("%d", runtime.Session.PeakContextTokens))
		wezterm.SetUserVar("CTX_COMPACTION_COUNT", fmt.Sprintf("%d", runtime.Session.CompactionCount))
		wezterm.SetUserVar("CTX_EFFECTIVE_WINDOW", fmt.Sprintf("%d", runtime.Session.EffectiveContextWindow))

		// Momentum tracking (build/break balance)
		// "A time to break down, and a time to build up" — Ecclesiastes 3:3
		netMomentum := runtime.Session.KTowardGod - runtime.Session.KTowardSelf
		wezterm.SetUserVar("MOMENTUM_NET", fmt.Sprintf("%d", netMomentum))
		total := runtime.Session.KTowardGod + runtime.Session.KTowardSelf
		if total > 0 {
			ratio := float64(netMomentum) / float64(total)
			wezterm.SetUserVar("MOMENTUM_RATIO", fmt.Sprintf("%.2f", ratio))
		} else {
			wezterm.SetUserVar("MOMENTUM_RATIO", "0.00")
		}
	}
}

// --- Adaptive Assembly Functions ---

// sectionEntry holds a section with its metadata
type sectionEntry struct {
	Name    string
	Section sections.SectionResult
}

// buildAllSections creates all section results
func buildAllSections(ctx types.SessionContext, sm *statemachine.StateMachine, runtime *statemachine.RuntimeState, healthCfg *health.HealthConfig) []sectionEntry {
	now := time.Now()
	entries := []sectionEntry{}

	// GROUP 1: WHO (Identity) - Priority 1
	identity := sections.BuildIdentity(ctx)
	if identity.HasInfo {
		entries = append(entries, sectionEntry{Name: "identity", Section: identity})
	}

	// GROUP 2: STATE - Priority 2
	state := sections.BuildState(sm, runtime)
	if state.HasInfo {
		entries = append(entries, sectionEntry{Name: "state", Section: state})
	}

	// GROUP 3: TEMPORAL - Priority 5
	temporal := sections.BuildTemporal(now)
	if temporal.HasInfo {
		entries = append(entries, sectionEntry{Name: "temporal", Section: temporal})
	}

	// GROUP 4: SUBSTRATE - Priority 7
	substrate := sections.BuildSubstrate(ctx)
	if substrate.HasInfo {
		entries = append(entries, sectionEntry{Name: "substrate", Section: substrate})
	}

	// GROUP 5: SPACE - Priority 7
	space := sections.BuildSpace(ctx)
	if space.HasInfo {
		entries = append(entries, sectionEntry{Name: "space", Section: space})
	}

	// TASKS - Priority 4
	tasks := sections.BuildTasks(runtime)
	if tasks.HasInfo {
		entries = append(entries, sectionEntry{Name: "tasks", Section: tasks})
	}

	// DEPTH - Priority 3
	depth := sections.BuildDepthEnhanced(ctx, runtime)
	if depth.HasInfo {
		entries = append(entries, sectionEntry{Name: "depth", Section: depth})
	}

	// HEALTH - Priority 2
	healthSection := sections.BuildHealth(runtime, healthCfg)
	if healthSection.HasInfo {
		entries = append(entries, sectionEntry{Name: "health", Section: healthSection})
	}

	// ANCHOR - Priority 2
	anchor := sections.BuildAnchor(sm, ctx.SessionID)
	if anchor.HasInfo {
		entries = append(entries, sectionEntry{Name: "anchor", Section: anchor})
	}

	return entries
}

// assembleAdaptive builds statusline respecting width constraints
func assembleAdaptive(allSections []sectionEntry, mode sections.DisplayMode, maxWidth int, cmdColor string, cmd statemachine.Command) string {
	if maxWidth == 0 {
		// Unlimited - use all sections
		return assembleAll(allSections, mode, cmdColor, cmd)
	}

	// Sort by priority (lower number = higher priority)
	sorted := make([]sectionEntry, len(allSections))
	copy(sorted, allSections)
	sortByPriority(sorted)

	// Build with width constraint
	var parts []string
	currentWidth := 0
	sepWidth := 3 // " | "

	for i, entry := range sorted {
		content := entry.Section.GetContent(mode)
		width := entry.Section.GetWidth(mode)

		// Check if we have room
		needed := width
		if i > 0 {
			needed += sepWidth
		}

		if currentWidth+needed <= maxWidth {
			if i > 0 {
				parts = append(parts, dimSep())
			}
			parts = append(parts, content)
			currentWidth += needed
		} else if mode < sections.DisplayMinimal {
			// Try compact form
			compactContent := entry.Section.CompactContent
			compactWidth := entry.Section.CompactWidth
			compactNeeded := compactWidth
			if i > 0 {
				compactNeeded += sepWidth
			}

			if compactContent != "" && currentWidth+compactNeeded <= maxWidth {
				if i > 0 {
					parts = append(parts, dimSep())
				}
				parts = append(parts, compactContent)
				currentWidth += compactNeeded
			}
			// If still doesn't fit, skip this section
		}
	}

	return strings.Join(parts, " ")
}

// assembleAll builds all sections without width constraint
func assembleAll(entries []sectionEntry, mode sections.DisplayMode, cmdColor string, cmd statemachine.Command) string {
	var parts []string
	for i, entry := range entries {
		if i > 0 {
			parts = append(parts, dimSep())
		}
		parts = append(parts, entry.Section.GetContent(mode))
	}
	return strings.Join(parts, " ")
}

// sortByPriority sorts entries by priority (lower = higher priority)
func sortByPriority(entries []sectionEntry) {
	// Simple bubble sort (small array)
	for i := 0; i < len(entries)-1; i++ {
		for j := i + 1; j < len(entries); j++ {
			if entries[j].Section.Priority < entries[i].Section.Priority {
				entries[i], entries[j] = entries[j], entries[i]
			}
		}
	}
}

// --- Helper Functions ---

// dimSep returns a dimmed pipe separator
func dimSep() string {
	return fmt.Sprintf("%s|%s", display.Dim, display.Reset)
}

// colorSep returns a command-colored operator separator
func colorSep(cmdColor string, cmd statemachine.Command) string {
	op := statemachine.CommandOperator(cmd)
	return fmt.Sprintf("%s%s%s", cmdColor, op, display.Reset)
}

// ============================================================================
// CLOSING
// ============================================================================
// Related: statusline/lib/sections/ (section builders)
//          statusline/cmd/statusline/main.go (entry point)
