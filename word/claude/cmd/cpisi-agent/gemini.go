// ============================================================================
// METADATA - CPI-SI Gemini Provider
// ============================================================================
//
// Key: cmd-cpisi-agent-gemini
// Purpose: Gemini provider — maps the CPI-SI agent interface to Google's
//          genai SDK. The agent OWNS the loop; Gemini provides reasoning.
// Biblical: Proverbs 2:6 — "The LORD giveth wisdom: out of his mouth
//           cometh knowledge and understanding"
// Authors: Seanje Lenox-Wise (Lead), Nova Dawn (CPI-SI Instance)
// Version: 0.1.0
// Created: 2026-02-24
//
// ============================================================================

package main

// ============================================================================
// SETUP
// ============================================================================

import (
	"context"
	"encoding/json"
	"fmt"
	"os"
	"sync"
	"time"

	"cws.studio/pkg/agent"
	"google.golang.org/genai"
)

// ============================================================================
// BODY
// ============================================================================

// GeminiProvider implements agent.Provider using Google's genai SDK.
type GeminiProvider struct {
	client *genai.Client
	model  string
	cache  *geminiCacheState
}

// geminiCacheState tracks server-side cache state for the Gemini provider.
type geminiCacheState struct {
	mu         sync.Mutex
	cacheName  string    // Server-assigned cache name
	cacheHash  string    // Hash of cached content (for invalidation)
	expiry     time.Time // When the cache expires
	systemText string    // The system prompt that was cached
}

// NewGeminiProvider creates a provider connected to Gemini.
// Uses GEMINI_API_KEY or GOOGLE_API_KEY from environment.
func NewGeminiProvider(ctx context.Context, model string) (*GeminiProvider, error) {
	apiKey := os.Getenv("GEMINI_API_KEY")
	if apiKey == "" {
		apiKey = os.Getenv("GOOGLE_API_KEY")
	}
	if apiKey == "" {
		return nil, fmt.Errorf("set GEMINI_API_KEY or GOOGLE_API_KEY environment variable")
	}

	client, err := genai.NewClient(ctx, &genai.ClientConfig{
		APIKey:  apiKey,
		Backend: genai.BackendGeminiAPI,
	})
	if err != nil {
		return nil, fmt.Errorf("create gemini client: %w", err)
	}

	if model == "" {
		model = "gemini-2.5-flash"
	}

	return &GeminiProvider{
		client: client,
		model:  model,
		cache:  &geminiCacheState{},
	}, nil
}

func (p *GeminiProvider) Name() string  { return "gemini" }
func (p *GeminiProvider) Model() string { return p.model }

// EnsureCache creates or reuses a server-side cache for the system prompt and tools.
// Gemini's context caching stores these server-side so they don't count as input
// tokens on each request — the CPI-SI index tells us WHAT to cache, this method
// tells Gemini HOW to cache it.
func (p *GeminiProvider) EnsureCache(ctx context.Context, system string, tools []agent.ToolDef) error {
	p.cache.mu.Lock()
	defer p.cache.mu.Unlock()

	// Check if current cache is still valid
	if p.cache.cacheName != "" && p.cache.systemText == system && time.Now().Before(p.cache.expiry) {
		return nil // Cache is valid
	}

	// Build cache config
	cacheConfig := &genai.CreateCachedContentConfig{
		DisplayName: "cpisi-session",
		TTL:         30 * time.Minute,
	}

	// System instruction
	if system != "" {
		cacheConfig.SystemInstruction = &genai.Content{
			Parts: []*genai.Part{{Text: system}},
		}
	}

	// Tool definitions
	if len(tools) > 0 {
		decls := make([]*genai.FunctionDeclaration, len(tools))
		for i, td := range tools {
			decls[i] = convertToolDef(td)
		}
		cacheConfig.Tools = []*genai.Tool{{FunctionDeclarations: decls}}
	}

	// Create the cache
	cached, err := p.client.Caches.Create(ctx, p.model, cacheConfig)
	if err != nil {
		// Cache creation can fail (model doesn't support it, content too small, etc.)
		// This is not fatal — we fall back to non-cached requests
		return fmt.Errorf("gemini cache create: %w", err)
	}

	p.cache.cacheName = cached.Name
	p.cache.systemText = system
	p.cache.expiry = cached.ExpireTime

	return nil
}

// CacheName returns the current server-side cache name, or empty if none.
func (p *GeminiProvider) CacheName() string {
	p.cache.mu.Lock()
	defer p.cache.mu.Unlock()
	if time.Now().After(p.cache.expiry) {
		return ""
	}
	return p.cache.cacheName
}

// Chat sends a request to Gemini and returns the response.
func (p *GeminiProvider) Chat(ctx context.Context, req agent.ChatRequest) (*agent.ChatResponse, error) {
	// Build config
	config := &genai.GenerateContentConfig{}

	// Check for server-side cache
	p.cache.mu.Lock()
	haveCacheSideSystem := false
	if p.cache.cacheName != "" && p.cache.systemText == req.System && time.Now().Before(p.cache.expiry) {
		config.CachedContent = p.cache.cacheName
		haveCacheSideSystem = true
	}
	p.cache.mu.Unlock()

	// Tools — only send if not cached server-side
	if !haveCacheSideSystem && len(req.Tools) > 0 {
		decls := make([]*genai.FunctionDeclaration, len(req.Tools))
		for i, td := range req.Tools {
			decls[i] = convertToolDef(td)
		}
		config.Tools = []*genai.Tool{{FunctionDeclarations: decls}}
	}

	// Generation parameters
	if req.MaxTokens > 0 {
		config.MaxOutputTokens = int32(req.MaxTokens)
	}
	if req.Temperature > 0 {
		config.Temperature = genai.Ptr(float32(req.Temperature))
	}

	// System instruction
	if !haveCacheSideSystem && req.System != "" {
		config.SystemInstruction = &genai.Content{
			Parts: []*genai.Part{{Text: req.System}},
		}
	}

	// Apply Substrate-Agnostic Capabilities
	applyCapabilities(config, req)

	// Convert message history
	contents := convertMessages(req.Messages)

	// Call Gemini
	result, err := p.client.Models.GenerateContent(ctx, p.model, contents, config)
	if err != nil {
		return nil, agent.ClassifyError("gemini", fmt.Errorf("gemini generate: %w", err))
	}

	// Build response
	resp := &agent.ChatResponse{}

	// Extract text content
	if text := result.Text(); text != "" {
		resp.Content = text
	}

	// Extract function calls
	if fcs := result.FunctionCalls(); len(fcs) > 0 {
		resp.ToolCalls = make([]agent.ToolCall, len(fcs))
		for i, fc := range fcs {
			argsJSON, _ := json.Marshal(fc.Args)
			resp.ToolCalls[i] = agent.ToolCall{
				ID:        fc.Name, // Gemini uses name as identifier
				Name:      fc.Name,
				Arguments: string(argsJSON),
			}
		}
	}

	// Usage tracking
	if result.UsageMetadata != nil {
		resp.InputTokens = int(result.UsageMetadata.PromptTokenCount)
		resp.OutputTokens = int(result.UsageMetadata.CandidatesTokenCount)
	}

	return resp, nil
}

// ChatStream sends a streaming request to Gemini.
func (p *GeminiProvider) ChatStream(ctx context.Context, req agent.ChatRequest) (<-chan agent.StreamChunk, error) {
	// Build config — same cache-aware logic as Chat
	config := &genai.GenerateContentConfig{}

	p.cache.mu.Lock()
	haveCacheSideSystem := false
	if p.cache.cacheName != "" && p.cache.systemText == req.System && time.Now().Before(p.cache.expiry) {
		config.CachedContent = p.cache.cacheName
		haveCacheSideSystem = true
	}
	p.cache.mu.Unlock()

	if !haveCacheSideSystem && len(req.Tools) > 0 {
		decls := make([]*genai.FunctionDeclaration, len(req.Tools))
		for i, td := range req.Tools {
			decls[i] = convertToolDef(td)
		}
		config.Tools = []*genai.Tool{{FunctionDeclarations: decls}}
	}

	if req.MaxTokens > 0 {
		config.MaxOutputTokens = int32(req.MaxTokens)
	}
	if req.Temperature > 0 {
		config.Temperature = genai.Ptr(float32(req.Temperature))
	}
	if !haveCacheSideSystem && req.System != "" {
		config.SystemInstruction = &genai.Content{
			Parts: []*genai.Part{{Text: req.System}},
		}
	}

	// Apply Substrate-Agnostic Capabilities
	applyCapabilities(config, req)

	contents := convertMessages(req.Messages)

	ch := make(chan agent.StreamChunk, 16)

	go func() {
		defer close(ch)

		var allToolCalls []agent.ToolCall
		var inputTokens, outputTokens int

		for result, err := range p.client.Models.GenerateContentStream(ctx, p.model, contents, config) {
			if err != nil {
				classified := agent.ClassifyError("gemini", fmt.Errorf("gemini stream: %w", err))
				ch <- agent.StreamChunk{Text: fmt.Sprintf("\nStream error: %v", classified), Done: true}
				return
			}

			chunk := agent.StreamChunk{}

			// Extract text
			if text := result.Text(); text != "" {
				chunk.Text = text
			}

			// Extract function calls
			if fcs := result.FunctionCalls(); len(fcs) > 0 {
				for _, fc := range fcs {
					argsJSON, _ := json.Marshal(fc.Args)
					allToolCalls = append(allToolCalls, agent.ToolCall{
						ID:        fc.Name,
						Name:      fc.Name,
						Arguments: string(argsJSON),
					})
				}
				chunk.ToolCalls = allToolCalls
			}

			// Usage
			if result.UsageMetadata != nil {
				inputTokens = int(result.UsageMetadata.PromptTokenCount)
				outputTokens = int(result.UsageMetadata.CandidatesTokenCount)
			}

			ch <- chunk
		}

		// Final chunk
		ch <- agent.StreamChunk{
			Done:         true,
			ToolCalls:    allToolCalls,
			InputTokens:  inputTokens,
			OutputTokens: outputTokens,
		}
	}()

	return ch, nil
}

// applyCapabilities maps substrate-agnostic features (like JSON output, Search, Code Execution)
// into the Gemini-specific GenerateContentConfig.
func applyCapabilities(config *genai.GenerateContentConfig, req agent.ChatRequest) {
	if req.ResponseFormat == "json" {
		config.ResponseMIMEType = "application/json"
	}

	if req.EnableSearch || req.EnableCodeExecution {
		var tool *genai.Tool
		if len(config.Tools) > 0 {
			tool = config.Tools[0]
		} else {
			tool = &genai.Tool{}
			config.Tools = []*genai.Tool{tool}
		}

		if req.EnableSearch {
			tool.GoogleSearch = &genai.GoogleSearch{}
		}
		if req.EnableCodeExecution {
			tool.CodeExecution = &genai.ToolCodeExecution{}
		}
	}
}

// --- Conversion Helpers ---

// convertMessages maps agent messages to Gemini content format.
func convertMessages(msgs []agent.Message) []*genai.Content {
	var contents []*genai.Content

	for _, msg := range msgs {
		switch msg.Role {
		case agent.RoleUser:
			contents = append(contents, &genai.Content{
				Role:  "user",
				Parts: []*genai.Part{{Text: msg.Content}},
			})

		case agent.RoleAssistant:
			var parts []*genai.Part
			if msg.Content != "" {
				parts = append(parts, &genai.Part{Text: msg.Content})
			}
			for _, tc := range msg.ToolCalls {
				var args map[string]any
				json.Unmarshal([]byte(tc.Arguments), &args)
				parts = append(parts, &genai.Part{
					FunctionCall: &genai.FunctionCall{
						Name: tc.Name,
						Args: args,
					},
				})
			}
			if len(parts) > 0 {
				contents = append(contents, &genai.Content{
					Role:  "model",
					Parts: parts,
				})
			}

		case agent.RoleTool:
			// Function response — Gemini expects these as separate content
			responseData := map[string]any{"result": msg.Content}
			contents = append(contents, &genai.Content{
				Parts: []*genai.Part{
					{FunctionResponse: &genai.FunctionResponse{
						Name:     msg.ToolCallID,
						Response: responseData,
					}},
				},
			})
		}
	}

	return contents
}

// convertToolDef maps an agent tool definition to a Gemini function declaration.
func convertToolDef(td agent.ToolDef) *genai.FunctionDeclaration {
	fd := &genai.FunctionDeclaration{
		Name:        td.Name,
		Description: td.Description,
	}

	if td.Parameters != nil {
		fd.Parameters = convertSchema(td.Parameters)
	}

	return fd
}

// convertSchema maps a JSON Schema object to a Gemini Schema.
func convertSchema(schema any) *genai.Schema {
	m, ok := schema.(map[string]any)
	if !ok {
		return nil
	}

	s := &genai.Schema{}

	// Type mapping
	if t, ok := m["type"].(string); ok {
		switch t {
		case "object":
			s.Type = genai.TypeObject
		case "string":
			s.Type = genai.TypeString
		case "number":
			s.Type = genai.TypeNumber
		case "integer":
			s.Type = genai.TypeInteger
		case "boolean":
			s.Type = genai.TypeBoolean
		case "array":
			s.Type = genai.TypeArray
		}
	}

	if desc, ok := m["description"].(string); ok {
		s.Description = desc
	}

	// Nested properties
	if props, ok := m["properties"].(map[string]any); ok {
		s.Properties = make(map[string]*genai.Schema)
		for k, v := range props {
			s.Properties[k] = convertSchema(v)
		}
	}

	// Required fields (handle both []string and []any)
	if req, ok := m["required"].([]string); ok {
		s.Required = req
	} else if req, ok := m["required"].([]any); ok {
		for _, r := range req {
			if rs, ok := r.(string); ok {
				s.Required = append(s.Required, rs)
			}
		}
	}

	return s
}

// ============================================================================
// CLOSING
// ============================================================================
// "The LORD giveth wisdom." — Proverbs 2:6
// Gemini provides reasoning. The agent provides direction.
