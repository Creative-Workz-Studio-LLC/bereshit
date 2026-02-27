// ============================================================================
// METADATA
// ============================================================================
//
//omni:key   cws-gui-modules-sdkinspector-update
//omni:code  --go -package->sdkinspector
//omni:version a-00.10
//
// Purpose: Build SDK Inspector ViewModels showing hook I/O structure,
//          substrate detection results, event types, and permission values.
// Biblical: 1 Chronicles 28:19 - "All this, said David, the LORD made me
//           understand in writing by his hand upon me"
//
// ============================================================================
// END METADATA

package sdkinspector

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt"

	"cws.studio/gui/render"
	"cws.studio/pkg/sdk/hookoutput"
)

// ============================================================================
// END SETUP

// ============================================================================
// BODY
// ============================================================================

// BuildSubstrateView creates the substrate detection ViewModel.
func BuildSubstrateView() render.View {
	substrate := hookoutput.DetectSubstrate()
	substrateName := "Claude Code"
	if substrate == hookoutput.SubstrateGemini {
		substrateName = "Gemini CLI"
	}

	return render.View{
		ID:    ViewSubstrate,
		Title: "Substrate",
		Icon:  "sdk",
		Sections: []render.Section{
			{
				Title: "Substrate Detection",
				Fields: []render.Field{
					{Key: "Active Substrate", Value: substrateName, Style: render.StyleEmphasized},
					{Key: "Is Claude", Value: fmt.Sprintf("%v", hookoutput.IsClaude()), Style: boolStyle(hookoutput.IsClaude())},
					{Key: "Is Gemini", Value: fmt.Sprintf("%v", hookoutput.IsGemini()), Style: boolStyle(hookoutput.IsGemini())},
				},
			},
			{
				Title: "Package Info",
				Fields: []render.Field{
					{Key: "Key", Value: hookoutput.Pragma[1], Style: render.StyleCode},
					{Key: "Version", Value: hookoutput.Metadata[1], Style: render.StyleCode},
				},
			},
		},
	}
}

// BuildHookIOView creates the hook I/O inspection ViewModel.
func BuildHookIOView() render.View {
	return render.View{
		ID:    ViewHookIO,
		Title: "Hook I/O",
		Icon:  "sdk",
		Sections: []render.Section{
			buildClaudeEventsSection(),
			buildGeminiEventsSection(),
			buildPermissionsSection(),
			buildResponseTypesSection(),
		},
	}
}

// --- Section builders ---

func buildClaudeEventsSection() render.Section {
	return render.Section{
		Title: "Claude Code Events",
		Fields: []render.Field{
			{Key: "SessionStart", Value: hookoutput.EventSessionStart, Style: render.StyleCode},
			{Key: "PreToolUse", Value: hookoutput.EventPreToolUse, Style: render.StyleCode},
			{Key: "PostToolUse", Value: hookoutput.EventPostToolUse, Style: render.StyleCode},
			{Key: "Stop", Value: hookoutput.EventStop, Style: render.StyleCode},
			{Key: "SubagentStart", Value: hookoutput.EventSubagentStart, Style: render.StyleCode},
			{Key: "SubagentStop", Value: hookoutput.EventSubagentStop, Style: render.StyleCode},
			{Key: "UserPromptSubmit", Value: hookoutput.EventUserPromptSubmit, Style: render.StyleCode},
			{Key: "Setup", Value: hookoutput.EventSetup, Style: render.StyleCode},
		},
	}
}

func buildGeminiEventsSection() render.Section {
	return render.Section{
		Title: "Gemini CLI Events",
		Fields: []render.Field{
			{Key: "SessionStart", Value: hookoutput.GeminiEventSessionStart, Style: render.StyleCode},
			{Key: "SessionEnd", Value: hookoutput.GeminiEventSessionEnd, Style: render.StyleCode},
			{Key: "BeforeTool", Value: hookoutput.GeminiEventBeforeTool, Style: render.StyleCode},
			{Key: "AfterTool", Value: hookoutput.GeminiEventAfterTool, Style: render.StyleCode},
			{Key: "BeforeAgent", Value: hookoutput.GeminiEventBeforeAgent, Style: render.StyleCode},
			{Key: "AfterAgent", Value: hookoutput.GeminiEventAfterAgent, Style: render.StyleCode},
			{Key: "PreCompress", Value: hookoutput.GeminiEventPreCompress, Style: render.StyleCode},
			{Key: "Notification", Value: hookoutput.GeminiEventNotification, Style: render.StyleCode},
		},
	}
}

func buildPermissionsSection() render.Section {
	return render.Section{
		Title: "Permission Values",
		Fields: []render.Field{
			{Key: "Allow", Value: hookoutput.PermissionAllow, Style: render.StyleSuccess},
			{Key: "Deny", Value: hookoutput.PermissionDeny, Style: render.StyleError},
			{Key: "Ask", Value: hookoutput.PermissionAsk, Style: render.StyleWarning},
			{Key: "Block", Value: hookoutput.DecisionBlock, Style: render.StyleError},
		},
	}
}

func buildResponseTypesSection() render.Section {
	return render.Section{
		Title: "Response Constructors",
		Fields: []render.Field{
			{Key: "Context", Value: "NewContextResponse, NewSessionStartResponse, NewPostToolUseResponse", Style: render.StyleMuted},
			{Key: "PreTool", Value: "NewPreToolAllow, NewPreToolDeny, NewPreToolAsk, NewPreToolModified", Style: render.StyleMuted},
			{Key: "Stop", Value: "NewStopAllow, NewStopBlock", Style: render.StyleMuted},
			{Key: "Setup", Value: "NewSetupResponse", Style: render.StyleMuted},
		},
	}
}

// --- Helpers ---

func boolStyle(v bool) render.FieldStyle {
	if v {
		return render.StyleSuccess
	}
	return render.StyleMuted
}

// ============================================================================
// END BODY

// ============================================================================
// CLOSING
// ============================================================================
// ============================================================================
// END CLOSING
