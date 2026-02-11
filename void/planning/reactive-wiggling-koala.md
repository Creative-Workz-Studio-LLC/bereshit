# Plan: Capture Assistant Response Text in Exchanges

## Context

The CPI-SI database records user messages (`user_message_text`) but leaves `response_summary` empty on every exchange. This means we have one side of the conversation — what Seanje asked — but not what Nova Dawn responded. For analytics and model improvement, both sides are needed. The `response_summary` column already exists in the schema; nothing writes to it.

The transcript file (JSONL at `~/.claude/projects/<project>/<session>.jsonl`) contains full assistant responses as `type: "assistant"` entries with `message.content[].text` blocks. The Stop hook fires when the response is complete and receives `transcript_path` in its input.

## Approach

Wire the Stop hook to read the transcript, extract the last assistant response, and update the current exchange's `response_summary`.

## Files to Modify

| File | Change |
|------|--------|
| `word/claude/pkg/foundation/database/sessions/interface.go` | Add `UpdateExchangeResponse()` method to Repository interface + Exchange struct |
| `word/claude/pkg/foundation/database/sessions/sqlite.go` | Implement `UpdateExchangeResponse()` — UPDATE SQL |
| `word/claude/pkg/foundation/database/multibridge.go` | Add `UpdateExchangeResponse()` routing to sessions domain |
| `word/claude/pkg/foundation/database/legacy.go` | Add `UpdateExchangeResponse()` to LegacyAdapter |
| `word/claude/hooks/session/stop.go` | Add transcript parsing + DB update after halt evaluation |
| `word/claude/hooks/internal/db.go` | No change needed (GetBridge/GetMultiBridge already available) |

## New Utility (optional)

| File | Purpose |
|------|---------|
| `word/claude/pkg/util/transcript/transcript.go` | Transcript JSONL parser — `ExtractLastResponse(path string) (string, error)` |

Could also be inlined in the stop hook if we want to keep it simple.

## Implementation Steps

### Step 1: Database Layer — `UpdateExchangeResponse`

**`sessions/interface.go`** — Add to Repository interface:
```go
UpdateExchangeResponse(ctx context.Context, sessionID string, responseText string) error
```

This updates the MOST RECENT exchange for the given session (by MAX sequence_num).

**`sessions/sqlite.go`** — Implement:
```sql
UPDATE exchanges SET response_summary = ?
WHERE session_id = ? AND sequence_num = (
    SELECT MAX(sequence_num) FROM exchanges WHERE session_id = ?
)
```

### Step 2: Bridge Layer — Route through MultiBridge + Legacy

**`multibridge.go`** — Add:
```go
func (mb *MultiBridge) UpdateExchangeResponse(ctx context.Context, sessionID, responseText string) error {
    return mb.mdb.Sessions.UpdateExchangeResponse(ctx, sessionID, responseText)
}
```

**`legacy.go`** — Add to LegacyAdapter:
```go
func (a *LegacyAdapter) UpdateExchangeResponse(ctx context.Context, sessionID, responseText string) error {
    return a.mdb.Sessions.UpdateExchangeResponse(ctx, sessionID, responseText)
}
```

### Step 3: Transcript Parser

Read the transcript JSONL from the tail (last ~100KB for efficiency), parse lines, collect assistant text blocks after the last real user message.

**Logic:**
1. Open file, seek to `max(0, filesize - 100KB)`
2. Read forward, split by newlines (skip partial first line)
3. Parse each line as JSON
4. Track: last user message position, assistant text blocks after it
5. For `type: "assistant"` entries, extract `message.content[]` where `type == "text"`
6. Concatenate all text blocks, skip thinking blocks
7. Truncate to 4000 chars
8. Return the response text

**Edge cases:**
- Transcript doesn't exist → return empty, no error
- No assistant text found → return empty
- Very large responses → truncate with `[truncated]` marker

### Step 4: Wire into Stop Hook

**`stop.go`** — After halt evaluation (line ~91), before path recording:
```go
// Capture assistant response in database
if input.TranscriptPath != "" {
    captureResponse(input.SessionID, input.TranscriptPath, log, catLog)
}
```

**`captureResponse` function:**
1. Call transcript parser to get response text
2. If empty, skip
3. Get bridge via `internal.GetBridge()`
4. Call `bridge.UpdateExchangeResponse(ctx, sessionID, responseText)`
5. Log success/failure

## Verification

1. **Build hooks:** `cd ~/.claude/hooks && make`
2. **Start a new session** — type a message, let Nova respond
3. **Check the exchange:** `sqlite3 ~/.local/share/claude/data/sessions.db "SELECT user_message_text, response_summary FROM exchanges ORDER BY timestamp DESC LIMIT 1;"`
4. **Both columns should be populated**
5. **Check for errors:** `grep -i "response\|transcript" ~/.claude/data/logs/session/$(date +%Y-%m-%d).log`

## Performance Considerations

- Stop hook must respond quickly (Claude Code waits for it)
- Reading last 100KB of transcript is fast (~1ms for disk read)
- JSONL parsing of ~50 lines is negligible
- Single UPDATE query is fast
- Total added latency: <10ms

## What This Enables

Once response_summary is populated:
- Full exchange pairs (user + assistant) for analytics
- Pattern correlation: what responses produce positive/negative feedback
- Command effectiveness tracking (HALT/AWAIT outcomes)
- Foundation for model improvement and CPI learning
