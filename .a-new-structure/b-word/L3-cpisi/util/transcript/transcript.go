// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: cpisi-util-transcript
// Purpose: Parse Claude Code transcript JSONL to extract assistant response and thinking text
// Biblical: Proverbs 15:23 - "A word spoken in due season, how good is it!"
// Authors: Nova Dawn
// Version: 2.0.0
// Created: 2026-02-08
// Updated: 2026-02-08
//
// The transcript file (~/.claude/projects/<project>/<session>.jsonl) contains
// the full conversation: user messages, assistant responses (with text/thinking/
// tool_use content blocks), and progress events. This package extracts both
// the assistant's visible response text AND internal thinking from the last
// exchange for database recording.
//
// Three layers of assistant output:
//   - Thought (type:"thinking") — reasoning before responding (the "what behind what")
//   - Word (type:"text") — visible response text
//   - Action (type:"tool_use") — tool invocations (not captured here)
//
// ═══════════════════════════════════════════════════════════════════════════

package transcript

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"bufio"
	"encoding/json"
	"io"
	"os"
	"strings"
)

// Constants
const (
	// MaxReadBytes is how far back from EOF to start reading (100KB).
	// Transcript lines average ~1-5KB; 100KB covers the last ~50 exchanges.
	MaxReadBytes = 100 * 1024

	// MaxResponseLength is the maximum response text to store.
	// Truncates long responses to keep database rows manageable.
	MaxResponseLength = 4000

	// MaxThinkingLength is the maximum thinking text to store.
	// Thinking blocks tend to be longer than responses — they contain
	// the full reasoning process. 8KB captures substantial thought chains.
	MaxThinkingLength = 8000
)

// ExchangeContent holds the extracted text from the last assistant exchange.
// Separates the visible response (Word) from internal reasoning (Thought).
type ExchangeContent struct {
	ResponseText string // type:"text" blocks — what the user sees
	ThinkingText string // type:"thinking" blocks — the reasoning process
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY - Types
// ═══════════════════════════════════════════════════════════════════════════

// transcriptEntry represents a single JSONL line from Claude Code's transcript.
// Only the fields we need are decoded — the rest is ignored.
type transcriptEntry struct {
	Type    string          `json:"type"`
	Message *messagePayload `json:"message,omitempty"`
}

// messagePayload is the assistant/user message structure within a transcript entry.
// For assistant: content is an array of content blocks (text, thinking, tool_use).
// For user: content can be a string (real message) or array with tool_result blocks.
type messagePayload struct {
	Content    json.RawMessage `json:"content"`
	StopReason string          `json:"stop_reason,omitempty"`
}

// contentBlock represents a single content block within an assistant message.
// Assistant messages have arrays of these: text, thinking, tool_use, tool_result.
//
// Field mapping by block type:
//   - type:"text"     → Text field contains the visible response
//   - type:"thinking" → Thinking field contains the reasoning (NOT the Text field)
//   - type:"tool_use" → neither (not captured)
type contentBlock struct {
	Type     string `json:"type"`
	Text     string `json:"text,omitempty"`
	Thinking string `json:"thinking,omitempty"`
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY - Core Operations
// ═══════════════════════════════════════════════════════════════════════════

// ExtractLastExchange reads a Claude Code transcript JSONL file and extracts
// both the visible response text and internal thinking from the last assistant
// exchange (after the last real user message).
//
// Strategy:
//   - Read the last MaxReadBytes of the file (efficient for large transcripts)
//   - Parse JSONL lines, tracking the last real user message position
//   - Collect all assistant text AND thinking blocks after that position
//   - Truncate each to their respective max lengths
//
// Returns nil (not error) for expected cases like missing file or no content.
func ExtractLastExchange(path string) (*ExchangeContent, error) {
	if path == "" {
		return nil, nil
	}

	f, err := os.Open(path)
	if err != nil {
		if os.IsNotExist(err) {
			return nil, nil
		}
		return nil, err
	}
	defer f.Close()

	// Seek to tail of file for efficiency
	lines, err := readTail(f, MaxReadBytes)
	if err != nil {
		return nil, err
	}

	// Parse entries, collecting assistant content blocks from the last exchange.
	//
	// Key insight: Claude Code's transcript has "user" entries for both real user
	// messages (text content) AND tool_result returns (tool_result blocks).
	// Only real user messages should reset the collection — tool_result entries
	// are part of the ongoing assistant response cycle.
	var assistantTexts []string
	var thinkingTexts []string

	for _, line := range lines {
		line = strings.TrimSpace(line)
		if line == "" {
			continue
		}

		var entry transcriptEntry
		if err := json.Unmarshal([]byte(line), &entry); err != nil {
			continue // Skip malformed lines (partial reads from seek)
		}

		switch entry.Type {
		case "user":
			// Only reset collection on REAL user messages (text content),
			// not tool_result returns which are part of the response cycle.
			if entry.Message != nil && isRealUserMessage(entry.Message.Content) {
				assistantTexts = nil
				thinkingTexts = nil
			}

		case "assistant":
			if entry.Message != nil {
				texts := extractTextBlocks(entry.Message.Content)
				assistantTexts = append(assistantTexts, texts...)

				thoughts := extractThinkingBlocks(entry.Message.Content)
				thinkingTexts = append(thinkingTexts, thoughts...)
			}
		}
	}

	// Build result — nil if nothing was captured
	responseText := strings.Join(assistantTexts, "\n")
	thinkingText := strings.Join(thinkingTexts, "\n")

	if responseText == "" && thinkingText == "" {
		return nil, nil
	}

	return &ExchangeContent{
		ResponseText: truncate(responseText, MaxResponseLength),
		ThinkingText: truncate(thinkingText, MaxThinkingLength),
	}, nil
}

// ExtractLastResponse reads a Claude Code transcript JSONL file and extracts
// the text content from the last assistant response after the last user message.
// Convenience wrapper around ExtractLastExchange for backward compatibility.
//
// Returns empty string (not error) for expected cases like missing file
// or no assistant text found.
func ExtractLastResponse(path string) (string, error) {
	content, err := ExtractLastExchange(path)
	if err != nil {
		return "", err
	}
	if content == nil {
		return "", nil
	}
	return content.ResponseText, nil
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY - Helpers
// ═══════════════════════════════════════════════════════════════════════════

// readTail reads the last maxBytes of a file and returns lines.
// Skips the first partial line if we seeked into the middle of one.
func readTail(f *os.File, maxBytes int64) ([]string, error) {
	info, err := f.Stat()
	if err != nil {
		return nil, err
	}

	size := info.Size()
	offset := int64(0)
	skipFirst := false

	if size > maxBytes {
		offset = size - maxBytes
		skipFirst = true // First line is likely partial
	}

	if _, err := f.Seek(offset, io.SeekStart); err != nil {
		return nil, err
	}

	var lines []string
	scanner := bufio.NewScanner(f)

	// Increase scanner buffer for large JSONL lines (assistant responses can be big)
	buf := make([]byte, 0, 256*1024)
	scanner.Buffer(buf, 1024*1024)

	for scanner.Scan() {
		if skipFirst {
			skipFirst = false
			continue // Discard partial first line
		}
		lines = append(lines, scanner.Text())
	}

	return lines, scanner.Err()
}

// isRealUserMessage returns true if the user message contains actual user text
// (not just tool_result blocks). In Claude's transcript:
//   - Real user messages: content is a string, or array with type:"text" blocks
//   - Tool results: content is an array with only type:"tool_result" blocks
func isRealUserMessage(raw json.RawMessage) bool {
	if len(raw) == 0 {
		return false
	}

	// If content is a string, it's a real user message
	var text string
	if err := json.Unmarshal(raw, &text); err == nil {
		return true
	}

	// If content is an array, check for text blocks (not just tool_result)
	var blocks []contentBlock
	if err := json.Unmarshal(raw, &blocks); err == nil {
		for _, b := range blocks {
			if b.Type == "text" {
				return true
			}
		}
		return false // Only tool_result blocks — not a real user message
	}

	return false
}

// extractTextBlocks pulls text content from a message's content field.
// Content can be a string or an array of content blocks.
// Only extracts type:"text" blocks — skips thinking, tool_use, tool_result.
func extractTextBlocks(raw json.RawMessage) []string {
	if len(raw) == 0 {
		return nil
	}

	// Try as array of content blocks (assistant messages)
	var blocks []contentBlock
	if err := json.Unmarshal(raw, &blocks); err == nil {
		var texts []string
		for _, b := range blocks {
			if b.Type == "text" && strings.TrimSpace(b.Text) != "" {
				texts = append(texts, strings.TrimSpace(b.Text))
			}
		}
		return texts
	}

	// Try as plain string (some message types)
	var text string
	if err := json.Unmarshal(raw, &text); err == nil && strings.TrimSpace(text) != "" {
		return []string{strings.TrimSpace(text)}
	}

	return nil
}

// extractThinkingBlocks pulls thinking content from a message's content field.
// Thinking blocks use field name "thinking" (not "text") — this is the
// assistant's internal reasoning before responding. The "what behind what."
func extractThinkingBlocks(raw json.RawMessage) []string {
	if len(raw) == 0 {
		return nil
	}

	var blocks []contentBlock
	if err := json.Unmarshal(raw, &blocks); err == nil {
		var thoughts []string
		for _, b := range blocks {
			if b.Type == "thinking" && strings.TrimSpace(b.Thinking) != "" {
				thoughts = append(thoughts, strings.TrimSpace(b.Thinking))
			}
		}
		return thoughts
	}

	return nil
}

// truncate shortens text to maxLen, appending a marker if truncated.
func truncate(s string, maxLen int) string {
	if len(s) <= maxLen {
		return s
	}
	return s[:maxLen-12] + " [truncated]"
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
