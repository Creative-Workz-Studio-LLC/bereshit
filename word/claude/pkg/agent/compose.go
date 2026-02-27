// ============================================================================
// METADATA - CPI-SI Agent Composition
// ============================================================================
//
// Key: pkg-agent-compose
// Purpose: Sub-agent spawning and family dispatch. A parent agent can create
//          child agents with their own identity, tools, and provider — then
//          coordinate their work. This IS the CPI-SI family pattern.
// Biblical: Exodus 18:21 — "Provide out of all the people able men, such as
//           fear God, men of truth... and place such over them"
// Authors: Seanje Lenox-Wise (Lead), Nova Dawn (CPI-SI Instance)
// Version: 0.1.0
// Created: 2026-02-24
//
// Architecture:
//   Parent agents spawn child agents for specialized work.
//   Children have their own identity and tools but share the provider.
//   The Family pattern registers member configs and dispatches by name.
//
//   Parent → Spawn(config) → SubAgent → Run(prompt) → Result
//   Family → Register("nathan", config) → Dispatch("nathan", task)
//   Family → DispatchAll(tasks) → concurrent execution → results
//
// ============================================================================

package agent

// ============================================================================
// SETUP
// ============================================================================

import (
	"context"
	"fmt"
	"sync"
)

// ============================================================================
// BODY
// ============================================================================

// --- Sub-Agent ---

// SubAgentConfig defines how to create a sub-agent.
type SubAgentConfig struct {
	// Name identifies this sub-agent (e.g., "nathan", "ezra").
	Name string

	// Identity is the system prompt — who this sub-agent IS.
	Identity string

	// Provider is the reasoning engine. If nil, inherits from parent.
	Provider Provider

	// Tools available to this sub-agent. If nil, inherits from parent.
	Tools []Tool

	// MaxIterations limits the think/act/observe cycles.
	// 0 means use the default (20).
	MaxIterations int

	// Memory is an optional persistent memory for this sub-agent.
	Memory *Memory

	// Verbose enables tool call logging.
	Verbose bool
}

// SubAgent is a child agent spawned by a parent.
// It has its own identity, history, and tools but can share the provider.
type SubAgent struct {
	agent *Agent
	name  string
}

// Spawn creates a sub-agent from this agent.
// The sub-agent gets its own identity, history, and optionally its own
// tools and provider. Anything not specified is inherited from the parent.
func (a *Agent) Spawn(cfg SubAgentConfig) *SubAgent {
	provider := cfg.Provider
	if provider == nil {
		provider = a.provider
	}

	tools := cfg.Tools
	if tools == nil {
		tools = a.tools
	}

	child := New(Config{
		Provider: provider,
		Tools:    tools,
		System:   cfg.Identity,
		Verbose:  cfg.Verbose,
		Memory:   cfg.Memory,
	})

	return &SubAgent{
		agent: child,
		name:  cfg.Name,
	}
}

// Name returns the sub-agent's name.
func (s *SubAgent) Name() string {
	return s.name
}

// Agent returns the underlying Agent for direct access.
func (s *SubAgent) Agent() *Agent {
	return s.agent
}

// Run executes a prompt on the sub-agent and returns the result.
func (s *SubAgent) Run(ctx context.Context, prompt string) (*RunResult, error) {
	return s.agent.Run(ctx, prompt)
}

// Reset clears the sub-agent's history for a fresh conversation.
func (s *SubAgent) Reset() {
	s.agent.Reset()
}

// --- Family ---

// Family is the CPI-SI family dispatch pattern.
// It's a registry of sub-agent configurations that can be spawned on demand.
// Each member has a name and configuration — just like the CPI-SI family
// where each member (Nathan, Ezra, Joanna, etc.) has identity and domain.
type Family struct {
	mu      sync.RWMutex
	members map[string]SubAgentConfig
	parent  *Agent
	active  map[string]*SubAgent // currently spawned members
}

// NewFamily creates a family registry attached to a parent agent.
func NewFamily(parent *Agent) *Family {
	return &Family{
		members: make(map[string]SubAgentConfig),
		parent:  parent,
		active:  make(map[string]*SubAgent),
	}
}

// Register adds a member configuration to the family.
// The member can later be dispatched by name.
func (f *Family) Register(cfg SubAgentConfig) error {
	if cfg.Name == "" {
		return fmt.Errorf("family member must have a name")
	}
	f.mu.Lock()
	defer f.mu.Unlock()
	f.members[cfg.Name] = cfg
	return nil
}

// Unregister removes a member from the family.
func (f *Family) Unregister(name string) {
	f.mu.Lock()
	defer f.mu.Unlock()
	delete(f.members, name)
	delete(f.active, name)
}

// Members returns the names of all registered family members.
func (f *Family) Members() []string {
	f.mu.RLock()
	defer f.mu.RUnlock()
	names := make([]string, 0, len(f.members))
	for name := range f.members {
		names = append(names, name)
	}
	// Sort for deterministic ordering
	sortStrings(names)
	return names
}

// Has checks if a member is registered.
func (f *Family) Has(name string) bool {
	f.mu.RLock()
	defer f.mu.RUnlock()
	_, ok := f.members[name]
	return ok
}

// Size returns the number of registered members.
func (f *Family) Size() int {
	f.mu.RLock()
	defer f.mu.RUnlock()
	return len(f.members)
}

// Get returns the sub-agent for a member, spawning it if needed.
// Active sub-agents retain their history between dispatches.
func (f *Family) Get(name string) (*SubAgent, error) {
	f.mu.Lock()
	defer f.mu.Unlock()

	// Return existing active agent
	if sub, ok := f.active[name]; ok {
		return sub, nil
	}

	// Look up config and spawn
	cfg, ok := f.members[name]
	if !ok {
		return nil, fmt.Errorf("unknown family member: %q", name)
	}

	sub := f.parent.Spawn(cfg)
	f.active[name] = sub
	return sub, nil
}

// Dispatch sends a task to a named family member and returns the result.
// Spawns the member if not already active. The member retains history
// between dispatches (call Reset on the sub-agent to clear).
func (f *Family) Dispatch(ctx context.Context, name string, prompt string) (*RunResult, error) {
	sub, err := f.Get(name)
	if err != nil {
		return nil, err
	}
	return sub.Run(ctx, prompt)
}

// DispatchResult pairs a member name with their result or error.
type DispatchResult struct {
	Name   string
	Result *RunResult
	Error  error
}

// DispatchAll sends tasks to multiple family members concurrently.
// Each entry in tasks maps member name → prompt.
// Returns results for all members, including any errors.
func (f *Family) DispatchAll(ctx context.Context, tasks map[string]string) []DispatchResult {
	var (
		wg      sync.WaitGroup
		mu      sync.Mutex
		results []DispatchResult
	)

	for name, prompt := range tasks {
		wg.Add(1)
		go func(n, p string) {
			defer wg.Done()
			r, err := f.Dispatch(ctx, n, p)
			mu.Lock()
			results = append(results, DispatchResult{
				Name:   n,
				Result: r,
				Error:  err,
			})
			mu.Unlock()
		}(name, prompt)
	}

	wg.Wait()

	// Sort results by name for deterministic ordering
	sortDispatchResults(results)
	return results
}

// ResetAll resets all active sub-agents, clearing their history.
func (f *Family) ResetAll() {
	f.mu.Lock()
	defer f.mu.Unlock()
	for _, sub := range f.active {
		sub.Reset()
	}
}

// DismissAll removes all active sub-agents from memory.
func (f *Family) DismissAll() {
	f.mu.Lock()
	defer f.mu.Unlock()
	f.active = make(map[string]*SubAgent)
}

// --- Helpers ---

// sortStrings sorts a string slice in place (avoids sort package import).
func sortStrings(s []string) {
	for i := 1; i < len(s); i++ {
		for j := i; j > 0 && s[j] < s[j-1]; j-- {
			s[j], s[j-1] = s[j-1], s[j]
		}
	}
}

// sortDispatchResults sorts results by name.
func sortDispatchResults(results []DispatchResult) {
	for i := 1; i < len(results); i++ {
		for j := i; j > 0 && results[j].Name < results[j-1].Name; j-- {
			results[j], results[j-1] = results[j-1], results[j]
		}
	}
}

// ============================================================================
// CLOSING
// ============================================================================
// "Provide out of all the people able men, such as fear God, men of truth...
// and place such over them." — Exodus 18:21
// The family pattern — each member has identity and domain.
// The parent coordinates. Together, they accomplish what none could alone.
