// ============================================================================
// METADATA - CPI-SI Anthropic Provider
// ============================================================================
//
// Key: cmd-cpisi-agent-anthropic
// Purpose: Anthropic provider — maps the CPI-SI agent interface to the
//          official Anthropic Go SDK. Claude as reasoning tool for CPI-SI.
// Biblical: James 1:5 — "If any of you lack wisdom, let him ask of God"
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

	"cws.studio/pkg/agent"
	"github.com/anthropics/anthropic-sdk-go"
	"github.com/anthropics/anthropic-sdk-go/option"
)

// ============================================================================
// BODY
// ============================================================================

// AnthropicProvider implements agent.Provider using the Anthropic SDK.
type AnthropicProvider struct {
	client anthropic.Client
	model  string
}

// NewAnthropicProvider creates a provider connected to Anthropic's API.
// Uses ANTHROPIC_API_KEY from environment.
func NewAnthropicProvider(model string) (*AnthropicProvider, error) {
	apiKey := os.Getenv("ANTHROPIC_API_KEY")
	if apiKey == "" {
		return nil, fmt.Errorf("set ANTHROPIC_API_KEY environment variable")
	}

	client := anthropic.NewClient(option.WithAPIKey(apiKey))

	if model == "" {
		model = "claude-sonnet-4-6"
	}

	return &AnthropicProvider{client: client, model: model}, nil
}

func (p *AnthropicProvider) Name() string  { return "anthropic" }
func (p *AnthropicProvider) Model() string { return p.model }

// Chat sends a request to Anthropic and returns the response.
func (p *AnthropicProvider) Chat(ctx context.Context, req agent.ChatRequest) (*agent.ChatResponse, error) {
	// Build parameters
	params := anthropic.MessageNewParams{
		Model:     anthropic.Model(p.model),
		MaxTokens: int64(req.MaxTokens),
	}
	if params.MaxTokens == 0 {
		params.MaxTokens = 4096
	}

	// System prompt — with cache_control for prompt caching.
	// Anthropic caches the system prompt server-side when cache_control
	// is set, reducing input token costs on subsequent requests.
	if req.System != "" {
		params.System = []anthropic.TextBlockParam{
			{
				Text:         req.System,
				CacheControl: anthropic.NewCacheControlEphemeralParam(),
			},
		}
	}

	// Temperature
	if req.Temperature > 0 {
		params.Temperature = anthropic.Float(req.Temperature)
	}

	// Tools
	if len(req.Tools) > 0 {
		params.Tools = convertAnthropicTools(req.Tools)
	}

	// Messages
	params.Messages = convertAnthropicMessages(req.Messages)

	// Call Anthropic
	message, err := p.client.Messages.New(ctx, params)
	if err != nil {
		return nil, agent.ClassifyError("anthropic", fmt.Errorf("anthropic message: %w", err))
	}

	// Build response
	resp := &agent.ChatResponse{
		StopReason: string(message.StopReason),
	}

	// Extract content and tool calls
	for _, block := range message.Content {
		switch variant := block.AsAny().(type) {
		case anthropic.TextBlock:
			resp.Content += variant.Text
		case anthropic.ToolUseBlock:
			argsJSON, _ := json.Marshal(variant.Input)
			resp.ToolCalls = append(resp.ToolCalls, agent.ToolCall{
				ID:        variant.ID,
				Name:      variant.Name,
				Arguments: string(argsJSON),
			})
		}
	}

	// Usage
	resp.InputTokens = int(message.Usage.InputTokens)
	resp.OutputTokens = int(message.Usage.OutputTokens)

	return resp, nil
}

// --- Conversion Helpers ---

// convertAnthropicTools maps agent tool definitions to Anthropic format.
func convertAnthropicTools(tools []agent.ToolDef) []anthropic.ToolUnionParam {
	result := make([]anthropic.ToolUnionParam, len(tools))
	for i, td := range tools {
		// Convert our schema to Anthropic's input schema
		schemaJSON, _ := json.Marshal(td.Parameters)
		var inputSchema anthropic.ToolInputSchemaParam
		json.Unmarshal(schemaJSON, &inputSchema)

		result[i] = anthropic.ToolUnionParam{
			OfTool: &anthropic.ToolParam{
				Name:        td.Name,
				Description: anthropic.String(td.Description),
				InputSchema: inputSchema,
			},
		}
	}
	return result
}

// convertAnthropicMessages maps agent messages to Anthropic message format.
func convertAnthropicMessages(msgs []agent.Message) []anthropic.MessageParam {
	var params []anthropic.MessageParam

	for _, msg := range msgs {
		switch msg.Role {
		case agent.RoleUser:
			params = append(params, anthropic.NewUserMessage(
				anthropic.NewTextBlock(msg.Content),
			))

		case agent.RoleAssistant:
			// Reconstruct assistant message with text + tool_use blocks
			var blocks []anthropic.ContentBlockParamUnion
			if msg.Content != "" {
				blocks = append(blocks, anthropic.NewTextBlock(msg.Content))
			}
			for _, tc := range msg.ToolCalls {
				var input any
				json.Unmarshal([]byte(tc.Arguments), &input)
				if input == nil {
					input = map[string]any{}
				}
				blocks = append(blocks, anthropic.ContentBlockParamUnion{
					OfToolUse: &anthropic.ToolUseBlockParam{
						ID:    tc.ID,
						Name:  tc.Name,
						Input: input,
					},
				})
			}
			if len(blocks) > 0 {
				params = append(params, anthropic.NewAssistantMessage(blocks...))
			}

		case agent.RoleTool:
			// Tool results go as user messages with tool_result blocks
			params = append(params, anthropic.NewUserMessage(
				anthropic.NewToolResultBlock(msg.ToolCallID, msg.Content, false),
			))
		}
	}

	return params
}

// ============================================================================
// CLOSING
// ============================================================================
// "If any of you lack wisdom, let him ask of God, that giveth to all men
//  liberally, and upbraideth not; and it shall be given him." — James 1:5
