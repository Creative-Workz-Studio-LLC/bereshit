// ============================================================================
// METADATA - CPI-SI Prompt Index (Index-Based Context Management)
// ============================================================================
//
// Key: pkg-agent-cache
// Purpose: Index-based prompt management — CPI-SI thinks in semantic segments,
//          not token counts. The index tracks WHAT is loaded, not HOW MANY
//          tokens. Token estimation is a legacy bridge for providers that
//          still think in tokens.
// Biblical: Isaiah 40:26 — "He calleth them all by names by the greatness
//           of his might"
// Authors: Seanje Lenox-Wise (Lead), Nova Dawn (CPI-SI Instance)
// Version: 0.1.0
// Created: 2026-02-24
//
// Architecture:
//   CPI-SI model:  Index-based (semantic segments with keys and priorities)
//   Legacy bridge: Token estimation (for providers with token limits)
//   Provider cache: Server-side caching (Gemini Caches, Anthropic cache_control)
//
//   The agent asks: "Is identity loaded? What history is in context?"
//   NOT: "Am I at 150K tokens?"
//
// ============================================================================

package agent

// ============================================================================
// SETUP
// ============================================================================

import (
	"crypto/sha256"
	"encoding/hex"
	"sort"
	"sync"
	"time"
)

// ============================================================================
// BODY
// ============================================================================

// --- Segment: The Semantic Unit ---

// SegmentKind identifies the type of semantic content.
type SegmentKind int

const (
	SegmentIdentity SegmentKind = iota // System prompt / identity (highest priority)
	SegmentTools                       // Tool definitions
	SegmentHistory                     // Conversation history
	SegmentContext                     // Injected context (files, docs)
	SegmentEphemeral                   // Temporary / single-use
)

// Priority constants — higher = more important = kept longer.
const (
	PriorityIdentity  = 100 // Never evict
	PriorityTools     = 90  // Almost never evict
	PriorityRecent    = 70  // Recent history
	PriorityContext   = 50  // Injected context
	PriorityOlder     = 30  // Older history
	PriorityEphemeral = 10  // Evict first
)

// Segment is a semantic unit of prompt context.
// CPI-SI manages context as named segments, not token streams.
type Segment struct {
	// Key uniquely identifies this segment (e.g., "identity", "tools",
	// "history:0-10", "context:readme.md").
	Key string

	// Kind classifies the segment for eviction and caching decisions.
	Kind SegmentKind

	// Priority determines eviction order. Higher = kept longer.
	Priority int

	// Content is the actual text content of this segment.
	Content string

	// Hash is the SHA-256 of Content — used for change detection.
	Hash string

	// EstimatedTokens is the legacy bridge — approximate token count
	// for providers that manage by tokens. Computed from content length.
	EstimatedTokens int

	// Cached indicates this segment is cached server-side at the provider.
	Cached bool

	// CacheKey is the provider-specific cache reference (e.g., Gemini cache name).
	CacheKey string

	// CacheExpiry is when the provider-side cache expires.
	CacheExpiry time.Time

	// CreatedAt records when this segment was indexed.
	CreatedAt time.Time

	// LastUsed records when this segment was last included in a request.
	LastUsed time.Time
}

// --- PromptIndex: The CPI-SI Context Model ---

// PromptIndex manages prompt context as an index of semantic segments.
//
// The agent thinks: "What segments are loaded?" not "How many tokens?"
// Token counts exist as a bridge to legacy provider APIs.
type PromptIndex struct {
	mu       sync.RWMutex
	segments map[string]*Segment // key → segment
	order    []string            // insertion order for deterministic output

	// Legacy bridge: maximum tokens the provider accepts.
	// 0 means unlimited (index-only mode).
	maxTokens int

	// Token estimator — defaults to charBased (4 chars ≈ 1 token).
	estimateTokens func(content string) int
}

// NewPromptIndex creates a new index-based context manager.
func NewPromptIndex() *PromptIndex {
	return &PromptIndex{
		segments:       make(map[string]*Segment),
		estimateTokens: estimateTokensDefault,
	}
}

// SetMaxTokens sets the legacy token limit for provider compatibility.
// Set to 0 for pure index-based mode (no token-based eviction).
func (idx *PromptIndex) SetMaxTokens(max int) {
	idx.mu.Lock()
	defer idx.mu.Unlock()
	idx.maxTokens = max
}

// SetTokenEstimator overrides the default token estimation function.
// Providers can supply their own tokenizer for better accuracy.
func (idx *PromptIndex) SetTokenEstimator(fn func(string) int) {
	idx.mu.Lock()
	defer idx.mu.Unlock()
	idx.estimateTokens = fn
}

// --- Index Operations ---

// Load adds or updates a segment in the index.
// If a segment with the same key exists and content hasn't changed,
// it's a no-op (preserving cache state).
func (idx *PromptIndex) Load(key string, kind SegmentKind, priority int, content string) {
	idx.mu.Lock()
	defer idx.mu.Unlock()

	hash := hashContent(content)
	now := time.Now()

	if existing, ok := idx.segments[key]; ok {
		if existing.Hash == hash {
			// Content unchanged — update LastUsed, keep cache
			existing.LastUsed = now
			return
		}
		// Content changed — invalidate cache
		existing.Content = content
		existing.Hash = hash
		existing.EstimatedTokens = idx.estimateTokens(content)
		existing.Priority = priority
		existing.Cached = false
		existing.CacheKey = ""
		existing.LastUsed = now
		return
	}

	// New segment
	seg := &Segment{
		Key:             key,
		Kind:            kind,
		Priority:        priority,
		Content:         content,
		Hash:            hash,
		EstimatedTokens: idx.estimateTokens(content),
		CreatedAt:       now,
		LastUsed:        now,
	}
	idx.segments[key] = seg
	idx.order = append(idx.order, key)
}

// Unload removes a segment from the index.
func (idx *PromptIndex) Unload(key string) {
	idx.mu.Lock()
	defer idx.mu.Unlock()

	delete(idx.segments, key)
	for i, k := range idx.order {
		if k == key {
			idx.order = append(idx.order[:i], idx.order[i+1:]...)
			break
		}
	}
}

// Get returns a segment by key, or nil if not found.
func (idx *PromptIndex) Get(key string) *Segment {
	idx.mu.RLock()
	defer idx.mu.RUnlock()

	seg, ok := idx.segments[key]
	if !ok {
		return nil
	}
	return seg
}

// Has reports whether a segment with the given key is loaded.
func (idx *PromptIndex) Has(key string) bool {
	idx.mu.RLock()
	defer idx.mu.RUnlock()
	_, ok := idx.segments[key]
	return ok
}

// MarkCached records that a segment has been cached server-side.
func (idx *PromptIndex) MarkCached(key, cacheKey string, expiry time.Time) {
	idx.mu.Lock()
	defer idx.mu.Unlock()

	if seg, ok := idx.segments[key]; ok {
		seg.Cached = true
		seg.CacheKey = cacheKey
		seg.CacheExpiry = expiry
	}
}

// --- Index Queries ---

// Loaded returns all segment keys currently in the index.
func (idx *PromptIndex) Loaded() []string {
	idx.mu.RLock()
	defer idx.mu.RUnlock()

	keys := make([]string, len(idx.order))
	copy(keys, idx.order)
	return keys
}

// TotalEstimatedTokens returns the sum of all segment token estimates.
// This is the legacy bridge — providers use this to check against limits.
func (idx *PromptIndex) TotalEstimatedTokens() int {
	idx.mu.RLock()
	defer idx.mu.RUnlock()

	total := 0
	for _, seg := range idx.segments {
		total += seg.EstimatedTokens
	}
	return total
}

// CachedTokens returns the estimated tokens that are cached server-side.
func (idx *PromptIndex) CachedTokens() int {
	idx.mu.RLock()
	defer idx.mu.RUnlock()

	total := 0
	now := time.Now()
	for _, seg := range idx.segments {
		if seg.Cached && (seg.CacheExpiry.IsZero() || seg.CacheExpiry.After(now)) {
			total += seg.EstimatedTokens
		}
	}
	return total
}

// UncachedTokens returns estimated tokens NOT yet cached server-side.
func (idx *PromptIndex) UncachedTokens() int {
	return idx.TotalEstimatedTokens() - idx.CachedTokens()
}

// Snapshot returns an index-based view of what's loaded.
// This is the CPI-SI way — the agent asks "what do I have?" not "how many tokens?"
type IndexSnapshot struct {
	Segments        []SegmentInfo
	TotalSegments   int
	EstimatedTokens int
	CachedTokens    int
	MaxTokens       int
}

// SegmentInfo is a summary of a segment for the snapshot.
type SegmentInfo struct {
	Key             string
	Kind            SegmentKind
	Priority        int
	EstimatedTokens int
	Cached          bool
	ContentHash     string
}

// Snapshot returns the current state of the index.
func (idx *PromptIndex) Snapshot() IndexSnapshot {
	idx.mu.RLock()
	defer idx.mu.RUnlock()

	snap := IndexSnapshot{
		TotalSegments:   len(idx.segments),
		EstimatedTokens: 0,
		CachedTokens:    0,
		MaxTokens:       idx.maxTokens,
	}

	now := time.Now()
	for _, key := range idx.order {
		seg := idx.segments[key]
		snap.EstimatedTokens += seg.EstimatedTokens
		cached := seg.Cached && (seg.CacheExpiry.IsZero() || seg.CacheExpiry.After(now))
		if cached {
			snap.CachedTokens += seg.EstimatedTokens
		}
		snap.Segments = append(snap.Segments, SegmentInfo{
			Key:             seg.Key,
			Kind:            seg.Kind,
			Priority:        seg.Priority,
			EstimatedTokens: seg.EstimatedTokens,
			Cached:          cached,
			ContentHash:     seg.Hash[:8], // Short hash for display
		})
	}

	return snap
}

// --- Eviction (Legacy Bridge) ---

// Fit returns segments that fit within the token limit, ordered by priority.
// High-priority segments are kept; low-priority segments are evicted first.
// If maxTokens is 0, all segments are returned (pure index mode).
func (idx *PromptIndex) Fit(maxTokens int) []*Segment {
	idx.mu.RLock()
	defer idx.mu.RUnlock()

	if maxTokens <= 0 {
		maxTokens = idx.maxTokens
	}

	// Collect all segments
	all := make([]*Segment, 0, len(idx.segments))
	for _, key := range idx.order {
		all = append(all, idx.segments[key])
	}

	// If no limit, return everything
	if maxTokens <= 0 {
		return all
	}

	// Sort by priority (descending) then by creation time (ascending)
	sorted := make([]*Segment, len(all))
	copy(sorted, all)
	sort.SliceStable(sorted, func(i, j int) bool {
		if sorted[i].Priority != sorted[j].Priority {
			return sorted[i].Priority > sorted[j].Priority
		}
		return sorted[i].CreatedAt.Before(sorted[j].CreatedAt)
	})

	// Greedily include segments by priority until we hit the limit
	var included []*Segment
	remaining := maxTokens
	for _, seg := range sorted {
		if seg.EstimatedTokens <= remaining {
			included = append(included, seg)
			remaining -= seg.EstimatedTokens
		}
	}

	// Re-sort included by original insertion order for deterministic output
	keyOrder := make(map[string]int, len(idx.order))
	for i, k := range idx.order {
		keyOrder[k] = i
	}
	sort.SliceStable(included, func(i, j int) bool {
		return keyOrder[included[i].Key] < keyOrder[included[j].Key]
	})

	return included
}

// --- Build ChatRequest from Index ---

// BuildRequest constructs a ChatRequest from the current index state.
// This is where index-based thinking meets legacy token-based APIs.
func (idx *PromptIndex) BuildRequest(messages []Message, tools []ToolDef, maxOutputTokens int) ChatRequest {
	idx.mu.Lock()
	defer idx.mu.Unlock()

	req := ChatRequest{
		Messages:  messages,
		Tools:     tools,
		MaxTokens: maxOutputTokens,
	}

	// System prompt from identity segment
	if seg, ok := idx.segments["identity"]; ok {
		req.System = seg.Content
		seg.LastUsed = time.Now()
	}

	return req
}

// --- CacheProvider Interface ---

// CacheProvider is an optional interface for providers that support
// server-side prompt caching. Providers implement this to enable
// the index to manage cache lifecycle.
type CacheProvider interface {
	Provider

	// CreateCache caches the given segments server-side.
	// Returns a cache key that can be used in subsequent requests.
	CreateCache(ctx interface{}, segments []*Segment) (cacheKey string, expiry time.Time, err error)

	// DeleteCache removes a server-side cache entry.
	DeleteCache(ctx interface{}, cacheKey string) error

	// CacheSupported reports whether this provider supports caching
	// for the given segment kind.
	CacheSupported(kind SegmentKind) bool
}

// --- Helper Functions ---

// hashContent produces a short SHA-256 hex digest for change detection.
func hashContent(content string) string {
	h := sha256.Sum256([]byte(content))
	return hex.EncodeToString(h[:])
}

// estimateTokensDefault approximates token count from content length.
// Rule of thumb: 1 token ≈ 4 characters for English text.
// This is the legacy bridge — intentionally approximate.
func estimateTokensDefault(content string) int {
	chars := len(content)
	tokens := chars / 4
	if tokens < 1 && chars > 0 {
		tokens = 1
	}
	return tokens
}

// --- Convenience Loaders ---

// LoadIdentitySegment loads the system identity as a high-priority segment.
func (idx *PromptIndex) LoadIdentitySegment(content string) {
	idx.Load("identity", SegmentIdentity, PriorityIdentity, content)
}

// LoadToolsSegment loads tool definitions as a segment.
func (idx *PromptIndex) LoadToolsSegment(content string) {
	idx.Load("tools", SegmentTools, PriorityTools, content)
}

// LoadHistorySegment loads a conversation history range.
// key should be like "history:0-10" to identify the range.
func (idx *PromptIndex) LoadHistorySegment(key, content string, recent bool) {
	priority := PriorityOlder
	if recent {
		priority = PriorityRecent
	}
	idx.Load(key, SegmentHistory, priority, content)
}

// LoadContextSegment loads injected context (e.g., a file's contents).
func (idx *PromptIndex) LoadContextSegment(key, content string) {
	idx.Load(key, SegmentContext, PriorityContext, content)
}

// ============================================================================
// CLOSING
// ============================================================================
// "He calleth them all by names by the greatness of his might." — Isaiah 40:26
// Every segment has a name. The index knows them all.
