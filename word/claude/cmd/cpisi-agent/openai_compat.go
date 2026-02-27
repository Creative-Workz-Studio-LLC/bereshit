// ============================================================================
// METADATA - CPI-SI OpenAI-Compatible Provider
// ============================================================================
//
// Key: cmd-cpisi-agent-openai-compat
// Purpose: OpenAI-compatible provider — one client covers DeepSeek, Groq,
//          OpenRouter, Ollama, and OpenAI itself. Change the base URL.
// Biblical: Ecclesiastes 4:12 — "A threefold cord is not quickly broken"
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
	"fmt"
	"os"

	"cws.studio/pkg/agent"
	"github.com/openai/openai-go"
	"github.com/openai/openai-go/option"
	"github.com/openai/openai-go/shared"
)

// ============================================================================
// BODY
// ============================================================================

// Known OpenAI-compatible providers with their endpoints and env vars.
var knownProviders = map[string]struct {
	BaseURL string
	EnvKey  string
	Default string
}{
	"openai": {
		BaseURL: "https://api.openai.com/v1",
		EnvKey:  "OPENAI_API_KEY",
		Default: "gpt-4o-mini",
	},
	"deepseek": {
		BaseURL: "https://api.deepseek.com",
		EnvKey:  "DEEPSEEK_API_KEY",
		Default: "deepseek-chat",
	},
	"groq": {
		BaseURL: "https://api.groq.com/openai/v1",
		EnvKey:  "GROQ_API_KEY",
		Default: "llama-3.3-70b-versatile",
	},
	"openrouter": {
		BaseURL: "https://openrouter.ai/api/v1",
		EnvKey:  "OPENROUTER_API_KEY",
		Default: "meta-llama/llama-3.3-70b-instruct:free",
	},
	"ollama": {
		BaseURL: "http://localhost:11434/v1",
		EnvKey:  "",
		Default: "llama3.2",
	},
}

// OpenAICompatProvider implements agent.Provider for any OpenAI-compatible API.
type OpenAICompatProvider struct {
	client openai.Client
	name   string
	model  string
}

// NewOpenAICompatProvider creates a provider for any OpenAI-compatible endpoint.
func NewOpenAICompatProvider(providerName, model, baseURL, apiKey string) (*OpenAICompatProvider, error) {
	var opts []option.RequestOption

	if known, ok := knownProviders[providerName]; ok {
		if baseURL == "" {
			baseURL = known.BaseURL
		}
		if model == "" {
			model = known.Default
		}
		if apiKey == "" && known.EnvKey != "" {
			apiKey = os.Getenv(known.EnvKey)
		}

		// Ollama doesn't need an API key
		if providerName == "ollama" {
			if host := os.Getenv("OLLAMA_HOST"); host != "" {
				baseURL = host + "/v1"
			}
			if apiKey == "" {
				apiKey = "ollama" // Ollama accepts any key
			}
		}
	}

	if apiKey == "" && providerName != "ollama" {
		envKey := "API key"
		if known, ok := knownProviders[providerName]; ok {
			envKey = known.EnvKey
		}
		return nil, fmt.Errorf("set %s environment variable", envKey)
	}

	opts = append(opts, option.WithAPIKey(apiKey))
	if baseURL != "" {
		opts = append(opts, option.WithBaseURL(baseURL))
	}

	client := openai.NewClient(opts...)

	return &OpenAICompatProvider{
		client: client,
		name:   providerName,
		model:  model,
	}, nil
}

func (p *OpenAICompatProvider) Name() string  { return p.name }
func (p *OpenAICompatProvider) Model() string { return p.model }

// Chat sends a request through the OpenAI-compatible API.
func (p *OpenAICompatProvider) Chat(ctx context.Context, req agent.ChatRequest) (*agent.ChatResponse, error) {
	// Build parameters
	params := openai.ChatCompletionNewParams{
		Model:    shared.ChatModel(p.model),
		Messages: convertOpenAIMessages(req.Messages, req.System),
	}

	if req.MaxTokens > 0 {
		params.MaxCompletionTokens = openai.Int(int64(req.MaxTokens))
	}
	if req.Temperature > 0 {
		params.Temperature = openai.Float(req.Temperature)
	}

	// Tools
	if len(req.Tools) > 0 {
		params.Tools = convertOpenAITools(req.Tools)
	}

	// Call the API
	completion, err := p.client.Chat.Completions.New(ctx, params)
	if err != nil {
		return nil, agent.ClassifyError(p.name, fmt.Errorf("%s chat: %w", p.name, err))
	}

	// Build response
	resp := &agent.ChatResponse{}

	if len(completion.Choices) > 0 {
		choice := completion.Choices[0]
		resp.Content = choice.Message.Content
		resp.StopReason = string(choice.FinishReason)

		// Extract tool calls
		for _, tc := range choice.Message.ToolCalls {
			resp.ToolCalls = append(resp.ToolCalls, agent.ToolCall{
				ID:        tc.ID,
				Name:      tc.Function.Name,
				Arguments: tc.Function.Arguments,
			})
		}
	}

	// Usage
	if completion.Usage.PromptTokens > 0 {
		resp.InputTokens = int(completion.Usage.PromptTokens)
		resp.OutputTokens = int(completion.Usage.CompletionTokens)
	}

	return resp, nil
}

// --- Conversion Helpers ---

// convertOpenAIMessages maps agent messages to OpenAI chat completion format.
func convertOpenAIMessages(msgs []agent.Message, system string) []openai.ChatCompletionMessageParamUnion {
	var params []openai.ChatCompletionMessageParamUnion

	// System message first
	if system != "" {
		params = append(params, openai.SystemMessage(system))
	}

	for _, msg := range msgs {
		switch msg.Role {
		case agent.RoleUser:
			params = append(params, openai.UserMessage(msg.Content))

		case agent.RoleAssistant:
			if len(msg.ToolCalls) == 0 {
				params = append(params, openai.AssistantMessage(msg.Content))
			} else {
				// Assistant message with tool calls
				toolCalls := make([]openai.ChatCompletionMessageToolCallParam, len(msg.ToolCalls))
				for i, tc := range msg.ToolCalls {
					toolCalls[i] = openai.ChatCompletionMessageToolCallParam{
						ID: tc.ID,
						Function: openai.ChatCompletionMessageToolCallFunctionParam{
							Name:      tc.Name,
							Arguments: tc.Arguments,
						},
					}
				}
				params = append(params, openai.ChatCompletionMessageParamUnion{
					OfAssistant: &openai.ChatCompletionAssistantMessageParam{
						ToolCalls: toolCalls,
					},
				})
			}

		case agent.RoleTool:
			params = append(params, openai.ToolMessage(msg.Content, msg.ToolCallID))
		}
	}

	return params
}

// convertOpenAITools maps agent tool definitions to OpenAI function tools.
func convertOpenAITools(tools []agent.ToolDef) []openai.ChatCompletionToolParam {
	result := make([]openai.ChatCompletionToolParam, len(tools))
	for i, td := range tools {
		// Convert parameters to FunctionParameters (map[string]any)
		var funcParams shared.FunctionParameters
		if td.Parameters != nil {
			if m, ok := td.Parameters.(map[string]any); ok {
				funcParams = m
			}
		}

		result[i] = openai.ChatCompletionToolParam{
			Function: shared.FunctionDefinitionParam{
				Name:        td.Name,
				Description: openai.String(td.Description),
				Parameters:  funcParams,
			},
		}
	}
	return result
}

// ============================================================================
// CLOSING
// ============================================================================
// "A threefold cord is not quickly broken." — Ecclesiastes 4:12
// One client. Many providers. The cord holds.
