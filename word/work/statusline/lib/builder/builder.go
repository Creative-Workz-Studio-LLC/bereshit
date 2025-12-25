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

	"creativeworkzstudio.com/bereshit/word/work/pkg/util/term/display"
	"creativeworkzstudio.com/bereshit/word/work/pkg/core/health"
	"creativeworkzstudio.com/bereshit/word/work/pkg/core/statemachine"
	"creativeworkzstudio.com/bereshit/word/work/pkg/foundation/types"
	"creativeworkzstudio.com/bereshit/word/work/pkg/util/term/wezterm"
	"creativeworkzstudio.com/bereshit/word/work/statusline/lib/sections"
)

// ============================================================================
// BODY
// ============================================================================

// Build assembles all sections into the final statusline string
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

	// --- GROUP 6: DEPTH (Context + Cache) ---
	depth := sections.BuildDepth(ctx)
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

	// Emit trajectory variables
	emitTrajectoryVars(sm, runtime)

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
