// ============================================================================
// METADATA
// ============================================================================
// Claude Code Hooks - Orchestrator & Help Tool
// Unified entry point for testing and documentation
//
// Usage:
//   hooks help              Show this help
//   hooks list              List all hooks
//   hooks info <hook>       Show detailed info for a hook
//   hooks <hook>            Run a hook (for testing)

package main

import (
	"fmt"
	"os"

	"cws.studio/claude/hooks/permission"
	"cws.studio/claude/hooks/prompt"
	"cws.studio/claude/hooks/session"
	"cws.studio/claude/hooks/tool"
)

// ============================================================================
// SETUP
// ============================================================================

type hookInfo struct {
	name        string
	event       string
	description string
	matchers    string
	canBlock    bool
	fn          func()
}

var hooks = []hookInfo{
	// Session hooks
	{
		name:        "start",
		event:       "SessionStart",
		description: "Fires when Claude Code starts or resumes a session",
		matchers:    "startup, resume, clear, compact",
		canBlock:    false,
		fn:          session.Start,
	},
	{
		name:        "end",
		event:       "SessionEnd",
		description: "Fires when Claude Code session ends",
		matchers:    "none (reason: clear, logout, prompt_input_exit, other)",
		canBlock:    false,
		fn:          session.End,
	},
	{
		name:        "stop",
		event:       "Stop",
		description: "Fires when main agent finishes responding",
		matchers:    "none",
		canBlock:    true,
		fn:          session.Stop,
	},
	{
		name:        "subagent-stop",
		event:       "SubagentStop",
		description: "Fires when a subagent (Task tool) finishes",
		matchers:    "none",
		canBlock:    true,
		fn:          session.SubagentStop,
	},
	{
		name:        "notification",
		event:       "Notification",
		description: "Fires when Claude Code sends notifications",
		matchers:    "permission_prompt, idle_prompt, auth_success, elicitation_dialog",
		canBlock:    false,
		fn:          session.Notification,
	},
	{
		name:        "pre-compact",
		event:       "PreCompact",
		description: "Fires before context compaction",
		matchers:    "manual, auto",
		canBlock:    false,
		fn:          session.PreCompact,
	},
	// Tool hooks
	{
		name:        "pre-use",
		event:       "PreToolUse",
		description: "Fires before tool execution",
		matchers:    "Task, Bash, Glob, Grep, Read, Edit, Write, WebFetch, WebSearch, mcp__*",
		canBlock:    true,
		fn:          tool.PreUse,
	},
	{
		name:        "post-use",
		event:       "PostToolUse",
		description: "Fires after tool execution completes",
		matchers:    "Same as PreToolUse",
		canBlock:    false,
		fn:          tool.PostUse,
	},
	// Prompt hooks
	{
		name:        "submit",
		event:       "UserPromptSubmit",
		description: "Fires when user submits a prompt",
		matchers:    "none",
		canBlock:    true,
		fn:          prompt.Submit,
	},
	// Permission hooks
	{
		name:        "request",
		event:       "PermissionRequest",
		description: "Fires when user is shown permission dialog",
		matchers:    "Same as PreToolUse",
		canBlock:    true,
		fn:          permission.Request,
	},
}

// ============================================================================
// BODY
// ============================================================================

func main() {
	if len(os.Args) < 2 {
		help()
		return
	}

	cmd := os.Args[1]

	switch cmd {
	case "help", "-h", "--help":
		help()
	case "list":
		list()
	case "info":
		if len(os.Args) < 3 {
			fmt.Fprintln(os.Stderr, "Usage: hooks info <hook>")
			os.Exit(1)
		}
		info(os.Args[2])
	default:
		// Try to run as hook
		for _, h := range hooks {
			if h.name == cmd {
				h.fn()
				return
			}
		}
		fmt.Fprintf(os.Stderr, "Unknown command: %s\n\n", cmd)
		help()
		os.Exit(1)
	}
}

func help() {
	fmt.Println(`Claude Code Hooks - CPI-SI Orchestrator & Help Tool

Usage: hooks <command>

Commands:
  help              Show this help
  list              List all hooks with brief descriptions
  info <hook>       Show detailed information for a hook
  <hook>            Run a hook directly (for testing)

Session Hooks:
  start             SessionStart - When session begins
  end               SessionEnd - When session ends
  stop              Stop - When agent finishes responding (can block)
  subagent-stop     SubagentStop - When subagent finishes (can block)
  notification      Notification - System notifications
  pre-compact       PreCompact - Before context compaction

Tool Hooks:
  pre-use           PreToolUse - Before tool execution (can block)
  post-use          PostToolUse - After tool execution

Prompt Hooks:
  submit            UserPromptSubmit - When user submits prompt (can block)

Permission Hooks:
  request           PermissionRequest - Permission dialog handling (can block)

Examples:
  hooks list                    # List all hooks
  hooks info pre-use            # Show PreToolUse details
  echo '{}' | hooks start       # Test start hook with empty input`)
}

func list() {
	fmt.Println("Claude Code Hooks:")
	fmt.Println()

	categories := map[string][]hookInfo{
		"Session":    {},
		"Tool":       {},
		"Prompt":     {},
		"Permission": {},
	}

	for _, h := range hooks {
		switch h.name {
		case "start", "end", "stop", "subagent-stop", "notification", "pre-compact":
			categories["Session"] = append(categories["Session"], h)
		case "pre-use", "post-use":
			categories["Tool"] = append(categories["Tool"], h)
		case "submit":
			categories["Prompt"] = append(categories["Prompt"], h)
		case "request":
			categories["Permission"] = append(categories["Permission"], h)
		}
	}

	for _, cat := range []string{"Session", "Tool", "Prompt", "Permission"} {
		fmt.Printf("%s Hooks:\n", cat)
		for _, h := range categories[cat] {
			blocking := ""
			if h.canBlock {
				blocking = " [blocking]"
			}
			fmt.Printf("  %-15s %s%s\n", h.name, h.event, blocking)
		}
		fmt.Println()
	}
}

func info(name string) {
	for _, h := range hooks {
		if h.name == name {
			blocking := "No"
			if h.canBlock {
				blocking = "Yes"
			}
			fmt.Printf(`Hook: %s
Event: %s

Description:
  %s

Matchers: %s
Can Block: %s

Binary: bin/%s
Source: See respective package (session/, tool/, prompt/, permission/)
`, h.name, h.event, h.description, h.matchers, blocking, h.name)
			return
		}
	}
	fmt.Fprintf(os.Stderr, "Unknown hook: %s\n", name)
	os.Exit(1)
}

// ============================================================================
// CLOSING
// ============================================================================
