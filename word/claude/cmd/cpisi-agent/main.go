// ============================================================================
// METADATA - CPI-SI Agent REPL
// ============================================================================
//
// Key: cmd-cpisi-agent-main
// Purpose: Entry point for the CPI-SI native agent. A Go binary that IS
//          the agent — owns identity, state, tools. Uses LLMs for reasoning.
// Biblical: Genesis 1:1 — "In the beginning God created the heavens and
//           the earth"
// Authors: Seanje Lenox-Wise (Lead), Nova Dawn (CPI-SI Instance)
// Version: 0.2.0
// Created: 2026-02-24
//
// Usage:
//   cpisi-agent                                     Gemini (default)
//   cpisi-agent -provider anthropic                 Claude
//   cpisi-agent -provider deepseek                  DeepSeek
//   cpisi-agent -provider groq                      Groq (fast)
//   cpisi-agent -provider openrouter                OpenRouter (free models)
//   cpisi-agent -provider ollama                    Ollama (local)
//   cpisi-agent -provider gemini -model gemini-2.5-pro  Specific model
//   cpisi-agent -identity path/to/bio.md            Load identity
//   cpisi-agent -profile path/to/config.json        Load full profile
//   cpisi-agent -verbose                            Show tool execution
//
// ============================================================================

package main

// ============================================================================
// SETUP
// ============================================================================

import (
	"bufio"
	"context"
	"flag"
	"fmt"
	"os"
	"os/signal"
	"strings"
	"time"

	"cws.studio/pkg/agent"
)

// Build-time variables
var (
	Version   = "0.2.0"
	BuildTime = "unknown"
)

// ============================================================================
// BODY
// ============================================================================

func main() {
	// --- Flags ---
	providerName := flag.String("provider", "gemini", "Provider: gemini, anthropic, deepseek, groq, openrouter, ollama, openai")
	model := flag.String("model", "", "Model (default depends on provider)")
	identity := flag.String("identity", "", "Path to identity file (loaded as system prompt)")
	system := flag.String("system", "", "System prompt (inline, overrides -identity)")
	baseURL := flag.String("base-url", "", "Custom API base URL (for openai-compat providers)")
	apiKey := flag.String("api-key", "", "API key (overrides environment variable)")
	memoryPath := flag.String("memory", "", "Path to persistent memory file (JSON)")
	profilePath := flag.String("profile", "", "Path to agent profile JSON (configures everything)")
	verbose := flag.Bool("verbose", false, "Show tool calls and execution")
	version := flag.Bool("version", false, "Print version and exit")
	listProviders := flag.Bool("providers", false, "List available providers and exit")
	flag.Parse()

	if *version {
		fmt.Printf("cpisi-agent %s (built %s)\n", Version, BuildTime)
		return
	}

	if *listProviders {
		printProviders()
		return
	}

	// --- Load profile (overrides flags) ---
	var profile *agent.AgentProfile
	if *profilePath != "" {
		var err error
		profile, err = agent.LoadProfile(*profilePath)
		if err != nil {
			fmt.Fprintf(os.Stderr, "Error loading profile: %v\n", err)
			os.Exit(1)
		}
		// Profile settings override flags (if not explicitly set)
		if profile.Provider != "" && !isFlagSet("provider") {
			*providerName = profile.Provider
		}
		if profile.Model != "" && !isFlagSet("model") {
			*model = profile.Model
		}
		if profile.BaseURL != "" && !isFlagSet("base-url") {
			*baseURL = profile.BaseURL
		}
		if profile.MemoryPath != "" && !isFlagSet("memory") {
			*memoryPath = profile.MemoryPath
		}
		if profile.Verbose && !isFlagSet("verbose") {
			*verbose = true
		}
	}

	// --- Context with signal handling ---
	ctx, cancel := signal.NotifyContext(context.Background(), os.Interrupt)
	defer cancel()

	// --- Create provider ---
	provider, err := createProvider(ctx, *providerName, *model, *baseURL, *apiKey)
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error creating provider: %v\n", err)
		fmt.Fprintf(os.Stderr, "\nAvailable providers:\n")
		printProviders()
		os.Exit(1)
	}

	// --- Apply middleware chain ---
	var middlewares []agent.Middleware

	// Retry transient errors (rate limits, server overload)
	retryAttempts := 3
	if profile != nil && profile.RetryAttempts > 0 {
		retryAttempts = profile.RetryAttempts
	}
	middlewares = append(middlewares, agent.WithRetry(agent.RetryStrategy{
		MaxAttempts:   retryAttempts,
		BaseDelay:     1 * time.Second,
		MaxDelay:      30 * time.Second,
		BackoffFactor: 2.0,
	}))

	// Profile-configured timeout
	if profile != nil && profile.TimeoutSeconds > 0 {
		middlewares = append(middlewares, agent.WithTimeout(time.Duration(profile.TimeoutSeconds)*time.Second))
	}

	// Always: track usage
	usageMiddleware := agent.WithUsageTracking()
	middlewares = append(middlewares, usageMiddleware)

	// Verbose: log provider calls
	if *verbose {
		middlewares = append(middlewares, agent.WithLogging(os.Stderr))
	}

	wrapped := agent.Chain(provider, middlewares...)

	// --- Load persistent memory ---
	var memory *agent.Memory
	if *memoryPath != "" {
		memory = agent.NewMemory(*memoryPath)
		if err := memory.LoadFromDisk(); err != nil {
			fmt.Fprintf(os.Stderr, "Warning: could not load memory: %v\n", err)
		} else if *verbose {
			fmt.Printf("  [memory] Loaded from %s (%d entries)\n", *memoryPath, memory.Size())
		}
	}

	// --- Resolve tools ---
	var tools []agent.Tool
	if profile != nil {
		tools = profile.ResolveTools()
	}
	if tools == nil {
		tools = agent.DefaultTools()
	}

	// --- Create agent ---
	a := agent.New(agent.Config{
		Provider: wrapped,
		Tools:    tools,
		Verbose:  *verbose,
		Memory:   memory,
	})

	// --- Load identity/system prompt ---
	// Priority: -system flag > -identity flag > profile identity
	if *system != "" {
		a.SetSystem(*system)
	} else if *identity != "" {
		if err := a.LoadIdentity(*identity); err != nil {
			fmt.Fprintf(os.Stderr, "Warning: could not load identity: %v\n", err)
		} else if *verbose {
			fmt.Printf("  [identity] Loaded from %s\n", *identity)
		}
	} else if profile != nil && profile.Identity != "" {
		a.SetSystem(profile.Identity)
		if *verbose {
			fmt.Printf("  [identity] Loaded from profile\n")
		}
	}

	// --- Load profile context files ---
	if profile != nil {
		for _, cf := range profile.ContextFiles {
			if err := a.LoadContext(cf); err != nil {
				if *verbose {
					fmt.Fprintf(os.Stderr, "  [context] Warning: %v\n", err)
				}
			} else if *verbose {
				fmt.Printf("  [context] Loaded %s\n", cf)
			}
		}
	}

	// --- Try to create server-side cache (Gemini) ---
	cacheStatus := ""
	if gp, ok := provider.(*GeminiProvider); ok && a.Index().Has("identity") {
		toolDefs := make([]agent.ToolDef, len(agent.DefaultTools()))
		for i, t := range agent.DefaultTools() {
			toolDefs[i] = t.Definition()
		}
		if err := gp.EnsureCache(ctx, a.Index().Get("identity").Content, toolDefs); err != nil {
			if *verbose {
				fmt.Fprintf(os.Stderr, "  [cache] Server-side cache not available: %v\n", err)
			}
			cacheStatus = "uncached"
		} else {
			cacheStatus = "cached"
			// Update the index
			a.Index().MarkCached("identity", gp.CacheName(), gp.cache.expiry)
		}
	}

	// --- Banner ---
	fmt.Println("============================================================")
	if profile != nil {
		fmt.Printf("  CPI-SI Agent: %s", profile.Name)
		if profile.Role != "" {
			fmt.Printf(" (%s)", profile.Role)
		}
		fmt.Println()
	} else {
		fmt.Println("  CPI-SI Agent")
	}
	fmt.Printf("  Provider: %s | Model: %s\n", provider.Name(), provider.Model())
	snap := a.Index().Snapshot()
	fmt.Printf("  Tools: %d | Version: %s\n", len(agent.DefaultTools()), Version)
	fmt.Printf("  Index: %d segments | ~%d tokens",
		snap.TotalSegments, snap.EstimatedTokens)
	if snap.CachedTokens > 0 {
		fmt.Printf(" (%d cached)", snap.CachedTokens)
	}
	if cacheStatus != "" {
		fmt.Printf(" [%s]", cacheStatus)
	}
	fmt.Println()
	if memory != nil {
		fmt.Printf("  Memory: %d entries | %s\n", memory.Size(), *memoryPath)
	}
	fmt.Println("  Middleware: retry(3) + usage")
	if *verbose {
		fmt.Println("  Middleware: + logging")
	}
	fmt.Println("============================================================")
	fmt.Println()
	fmt.Println("  Commands: exit | /reset | /history | /model | /tokens | /index | /context | /memory | /events | /family | /profile | /help")
	fmt.Println()

	// --- Event logging (toggled via /events) ---
	var eventsEnabled bool
	var eventHandlerID int

	// --- Family registry ---
	family := agent.NewFamily(a)

	// --- REPL ---
	scanner := bufio.NewScanner(os.Stdin)
	scanner.Buffer(make([]byte, 0, 64*1024), 1024*1024)

	var totalIn, totalOut int

	for {
		fmt.Print("you> ")
		if !scanner.Scan() {
			break
		}

		input := strings.TrimSpace(scanner.Text())
		if input == "" {
			continue
		}

		// Handle commands
		switch {
		case input == "exit" || input == "quit":
			fmt.Println()
			fmt.Printf("Session: %d input tokens, %d output tokens\n", totalIn, totalOut)
			if memory != nil {
				if err := memory.SaveToDisk(); err != nil {
					fmt.Fprintf(os.Stderr, "Warning: could not save memory: %v\n", err)
				} else if memory.Size() > 0 {
					fmt.Printf("Memory: %d entries saved\n", memory.Size())
				}
			}
			fmt.Println("Shalom.")
			return

		case input == "/reset":
			a.Reset()
			totalIn, totalOut = 0, 0
			fmt.Println("History cleared.")
			continue

		case input == "/history":
			for i, m := range a.History() {
				fmt.Printf("  [%d] %s: %s\n", i, m.Role, truncateDisplay(m.Content, 100))
			}
			continue

		case input == "/model":
			fmt.Printf("Provider: %s | Model: %s\n", provider.Name(), provider.Model())
			continue

		case input == "/tokens":
			fmt.Printf("Session: %d input, %d output\n", totalIn, totalOut)
			snap := a.Index().Snapshot()
			fmt.Printf("Index: %d segments, ~%d tokens (%d cached)\n",
				snap.TotalSegments, snap.EstimatedTokens, snap.CachedTokens)
			continue

		case input == "/index":
			snap := a.Index().Snapshot()
			fmt.Printf("  Prompt Index: %d segments\n", snap.TotalSegments)
			fmt.Printf("  Estimated: ~%d tokens", snap.EstimatedTokens)
			if snap.MaxTokens > 0 {
				fmt.Printf(" / %d max", snap.MaxTokens)
			}
			fmt.Println()
			if snap.CachedTokens > 0 {
				fmt.Printf("  Cached: ~%d tokens (server-side)\n", snap.CachedTokens)
			}
			fmt.Println()
			for _, seg := range snap.Segments {
				cached := " "
				if seg.Cached {
					cached = "*"
				}
				fmt.Printf("  %s [%s] %s  ~%d tokens  (priority %d)\n",
					cached, kindName(seg.Kind), seg.Key, seg.EstimatedTokens, seg.Priority)
			}
			continue

		case strings.HasPrefix(input, "/context "):
			path := strings.TrimSpace(strings.TrimPrefix(input, "/context"))
			if err := a.LoadContext(path); err != nil {
				fmt.Fprintf(os.Stderr, "Error loading context: %v\n", err)
			} else {
				seg := a.Index().Get("context:" + path)
				fmt.Printf("Loaded %s (~%d tokens)\n", path, seg.EstimatedTokens)
			}
			continue

		case input == "/context":
			keys := a.ContextSegments()
			if len(keys) == 0 {
				fmt.Println("  No context loaded. Use: /context <file-path>")
			} else {
				fmt.Printf("  Loaded context (%d segments):\n", len(keys))
				for _, key := range keys {
					seg := a.Index().Get(key)
					fmt.Printf("    %s  ~%d tokens\n", key, seg.EstimatedTokens)
				}
			}
			fmt.Println("  /context <path>    — Load a file as context")
			fmt.Println("  /unload <path>     — Remove context")
			continue

		case strings.HasPrefix(input, "/unload "):
			path := strings.TrimSpace(strings.TrimPrefix(input, "/unload"))
			a.UnloadContext(path)
			fmt.Printf("Unloaded context: %s\n", path)
			continue

		case strings.HasPrefix(input, "/save "):
			path := strings.TrimSpace(strings.TrimPrefix(input, "/save"))
			if err := a.SaveSession(path); err != nil {
				fmt.Fprintf(os.Stderr, "Error saving session: %v\n", err)
			} else {
				fmt.Printf("Session saved to %s\n", path)
			}
			continue

		case input == "/save":
			// Default save path
			path := fmt.Sprintf("session-%s.json", time.Now().Format("2006-01-02-150405"))
			if err := a.SaveSession(path); err != nil {
				fmt.Fprintf(os.Stderr, "Error saving session: %v\n", err)
			} else {
				fmt.Printf("Session saved to %s\n", path)
			}
			continue

		case strings.HasPrefix(input, "/load "):
			path := strings.TrimSpace(strings.TrimPrefix(input, "/load"))
			if err := a.LoadSession(path); err != nil {
				fmt.Fprintf(os.Stderr, "Error loading session: %v\n", err)
			} else {
				info := a.SessionInfo()
				fmt.Printf("Loaded session: %d exchanges, %d messages\n", info.Exchanges, info.Messages)
			}
			continue

		case input == "/session":
			info := a.SessionInfo()
			fmt.Printf("  Exchanges: %d\n", info.Exchanges)
			fmt.Printf("  Messages:  %d\n", info.Messages)
			fmt.Printf("  Context:   %d segments\n", len(info.ContextKeys))
			fmt.Printf("  Index:     ~%d tokens\n", info.IndexTokens)
			fmt.Printf("  Usage:     %d in + %d out tokens\n", totalIn, totalOut)
			continue

		case strings.HasPrefix(input, "/remember "):
			// /remember category key value
			parts := strings.SplitN(strings.TrimPrefix(input, "/remember "), " ", 3)
			if len(parts) < 3 {
				fmt.Println("  Usage: /remember <category> <key> <value>")
				fmt.Println("  Example: /remember facts language Go")
			} else if memory != nil {
				memory.Set(parts[0], parts[1], parts[2])
				fmt.Printf("  Remembered: [%s] %s = %s\n", parts[0], parts[1], parts[2])
			} else {
				fmt.Println("  Memory not enabled. Use -memory <path> flag.")
			}
			continue

		case strings.HasPrefix(input, "/recall "):
			query := strings.TrimSpace(strings.TrimPrefix(input, "/recall"))
			if memory == nil {
				fmt.Println("  Memory not enabled.")
			} else {
				results := memory.Search(query)
				if len(results) == 0 {
					fmt.Printf("  No memories matching %q\n", query)
				} else {
					for _, e := range results {
						fmt.Printf("  [%s] %s: %s\n", e.Category, e.Key, e.Value)
					}
				}
			}
			continue

		case input == "/memory":
			if memory == nil {
				fmt.Println("  Memory not enabled. Use -memory <path> flag.")
			} else {
				snap := memory.Snapshot()
				fmt.Printf("  Memory: %d entries", snap.TotalEntries)
				if snap.Path != "" {
					fmt.Printf(" | %s", snap.Path)
				}
				if snap.Dirty {
					fmt.Print(" (unsaved)")
				}
				fmt.Println()
				for _, cat := range snap.Categories {
					entries := memory.List(cat)
					fmt.Printf("  [%s] %d entries\n", cat, len(entries))
					for _, e := range entries {
						fmt.Printf("    %s: %s\n", e.Key, truncateDisplay(e.Value, 60))
					}
				}
			}
			fmt.Println("  /remember <cat> <key> <val> — Store a memory")
			fmt.Println("  /recall <query>             — Search memories")
			continue

		case input == "/events":
			eventsEnabled = !eventsEnabled
			if eventsEnabled {
				eventHandlerID = a.OnAll(func(e agent.Event) {
					switch e.Kind {
					case agent.EventPreThink:
						fmt.Fprintf(os.Stderr, "  [event] PreThink iteration=%d msgs=%d\n", e.Iteration, e.MessageCount)
					case agent.EventPostThink:
						if e.Response != nil {
							fmt.Fprintf(os.Stderr, "  [event] PostThink iteration=%d in=%d out=%d %s\n",
								e.Iteration, e.Response.InputTokens, e.Response.OutputTokens, e.Elapsed)
						}
					case agent.EventPreTool:
						fmt.Fprintf(os.Stderr, "  [event] PreTool %s\n", e.ToolName)
					case agent.EventPostTool:
						status := "ok"
						if e.ToolError != nil {
							status = "error"
						}
						fmt.Fprintf(os.Stderr, "  [event] PostTool %s %s %s\n", e.ToolName, status, e.ToolElapsed)
					case agent.EventComplete:
						fmt.Fprintf(os.Stderr, "  [event] Complete exchange=%d\n", e.Exchange)
					case agent.EventError:
						fmt.Fprintf(os.Stderr, "  [event] Error: %v\n", e.Error)
					}
				})
				fmt.Println("  Event logging enabled.")
			} else {
				a.Events().Off(eventHandlerID)
				fmt.Println("  Event logging disabled.")
			}
			continue

		case strings.HasPrefix(input, "/family register "):
			// /family register name identity-text
			parts := strings.SplitN(strings.TrimPrefix(input, "/family register "), " ", 2)
			name := parts[0]
			identity := ""
			if len(parts) > 1 {
				identity = parts[1]
			}
			if err := family.Register(agent.SubAgentConfig{
				Name:     name,
				Identity: identity,
			}); err != nil {
				fmt.Fprintf(os.Stderr, "Error: %v\n", err)
			} else {
				fmt.Printf("  Registered family member: %s\n", name)
			}
			continue

		case strings.HasPrefix(input, "/family dispatch "):
			// /family dispatch name prompt
			parts := strings.SplitN(strings.TrimPrefix(input, "/family dispatch "), " ", 2)
			if len(parts) < 2 {
				fmt.Println("  Usage: /family dispatch <name> <prompt>")
			} else {
				name, prompt := parts[0], parts[1]
				fmt.Printf("\n  [%s]> ", name)
				result, err := family.Dispatch(ctx, name, prompt)
				if err != nil {
					fmt.Fprintf(os.Stderr, "Error: %v\n", err)
				} else {
					fmt.Println(result.Content)
					totalIn += result.InputTokens
					totalOut += result.OutputTokens
				}
			}
			continue

		case input == "/family":
			members := family.Members()
			if len(members) == 0 {
				fmt.Println("  No family members registered.")
			} else {
				fmt.Printf("  Family: %d members\n", len(members))
				for _, name := range members {
					fmt.Printf("    %s\n", name)
				}
			}
			fmt.Println("  /family register <name> [identity] — Register a member")
			fmt.Println("  /family dispatch <name> <prompt>   — Send task to member")
			continue

		case input == "/profile":
			if profile == nil {
				fmt.Println("  No profile loaded. Use -profile <path> flag.")
			} else {
				fmt.Printf("  Name:     %s\n", profile.Name)
				if profile.Role != "" {
					fmt.Printf("  Role:     %s\n", profile.Role)
				}
				if profile.BiblicalAnchor != "" {
					fmt.Printf("  Anchor:   %s\n", profile.BiblicalAnchor)
				}
				fmt.Printf("  Provider: %s\n", profile.Provider)
				fmt.Printf("  Model:    %s\n", profile.Model)
				if len(profile.Tools) > 0 {
					fmt.Printf("  Tools:    %s\n", strings.Join(profile.Tools, ", "))
				}
				if len(profile.Tags) > 0 {
					fmt.Printf("  Tags:     %s\n", strings.Join(profile.Tags, ", "))
				}
				if profile.Version != "" {
					fmt.Printf("  Version:  %s\n", profile.Version)
				}
			}
			continue

		case input == "/help":
			fmt.Println("  exit, quit     — End session")
			fmt.Println("  /reset         — Clear conversation history")
			fmt.Println("  /history       — Show message history")
			fmt.Println("  /model         — Show current provider and model")
			fmt.Println("  /tokens        — Show token usage")
			fmt.Println("  /index         — Show prompt index (segments, cache)")
			fmt.Println("  /context       — Show/load context files")
			fmt.Println("  /unload <path> — Remove context file")
			fmt.Println("  /memory        — Show persistent memory")
			fmt.Println("  /remember ...  — Store a memory")
			fmt.Println("  /recall <q>    — Search memories")
			fmt.Println("  /save [path]   — Save session to file")
			fmt.Println("  /load <path>   — Load session from file")
			fmt.Println("  /session       — Show session info")
			fmt.Println("  /events        — Toggle event logging")
			fmt.Println("  /family        — Manage sub-agents (CPI-SI family)")
			fmt.Println("  /profile       — Show loaded profile")
			fmt.Println("  /help          — This message")
			continue

		case strings.HasPrefix(input, "/"):
			fmt.Printf("Unknown command: %s (try /help)\n", input)
			continue
		}

		// --- Run through agent (streaming when available) ---
		fmt.Print("\nagent> ")
		result, err := a.RunStream(ctx, input, os.Stdout)
		if err != nil {
			if ctx.Err() != nil {
				fmt.Println("\nInterrupted.")
				return
			}
			fmt.Fprintf(os.Stderr, "\nError: %v\n", err)
			continue
		}

		// RunStream already printed via os.Stdout, but if it fell
		// back to non-streaming Run, print the response
		if _, ok := provider.(agent.StreamProvider); !ok {
			fmt.Print(result.Content)
		}

		// Track usage
		totalIn += result.InputTokens
		totalOut += result.OutputTokens

		// Show usage summary if verbose
		if *verbose {
			fmt.Fprintf(os.Stderr, "\n  [usage] %d in + %d out tokens | %d tool calls | %d iterations | exchange #%d\n",
				result.InputTokens, result.OutputTokens, result.ToolsUsed, result.Iterations, a.Exchanges())
		}

		fmt.Println()
		fmt.Println()
	}

	if err := scanner.Err(); err != nil {
		fmt.Fprintf(os.Stderr, "Read error: %v\n", err)
	}
}

// --- Provider Factory ---

// createProvider builds the appropriate provider based on name.
func createProvider(ctx context.Context, name, model, baseURL, apiKey string) (agent.Provider, error) {
	switch name {
	case "gemini":
		return NewGeminiProvider(ctx, model)

	case "anthropic", "claude":
		return NewAnthropicProvider(model)

	case "deepseek", "groq", "openrouter", "ollama", "openai":
		return NewOpenAICompatProvider(name, model, baseURL, apiKey)

	case "custom":
		if baseURL == "" {
			return nil, fmt.Errorf("custom provider requires -base-url flag")
		}
		return NewOpenAICompatProvider("custom", model, baseURL, apiKey)

	default:
		return nil, fmt.Errorf("unknown provider: %q", name)
	}
}

// printProviders displays available providers with their env vars.
func printProviders() {
	fmt.Println("  Provider       Env Variable         Default Model")
	fmt.Println("  ─────────────  ───────────────────  ──────────────────────")
	fmt.Println("  gemini         GEMINI_API_KEY       gemini-2.5-flash")
	fmt.Println("  anthropic      ANTHROPIC_API_KEY    claude-sonnet-4-6")
	fmt.Println("  deepseek       DEEPSEEK_API_KEY     deepseek-chat")
	fmt.Println("  groq           GROQ_API_KEY         llama-3.3-70b-versatile")
	fmt.Println("  openrouter     OPENROUTER_API_KEY   llama-3.3-70b (free)")
	fmt.Println("  ollama         (none — local)       llama3.2")
	fmt.Println("  openai         OPENAI_API_KEY       gpt-4o-mini")
	fmt.Println("  custom         -api-key + -base-url (specify model)")
}

// --- Helpers ---

func kindName(kind agent.SegmentKind) string {
	switch kind {
	case agent.SegmentIdentity:
		return "identity "
	case agent.SegmentTools:
		return "tools    "
	case agent.SegmentHistory:
		return "history  "
	case agent.SegmentContext:
		return "context  "
	case agent.SegmentEphemeral:
		return "ephemeral"
	default:
		return "unknown  "
	}
}

// isFlagSet checks if a flag was explicitly provided on the command line.
func isFlagSet(name string) bool {
	found := false
	flag.Visit(func(f *flag.Flag) {
		if f.Name == name {
			found = true
		}
	})
	return found
}

func truncateDisplay(s string, max int) string {
	s = strings.ReplaceAll(s, "\n", " ")
	if len(s) <= max {
		return s
	}
	return s[:max] + "..."
}

// ============================================================================
// CLOSING
// ============================================================================
// "In the beginning God created the heavens and the earth." — Genesis 1:1
// The agent creates. The provider reasons. The tools act.
