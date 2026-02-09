# Docstrings as Teaching, Not Documenting

> *"Come now, and let us reason together, saith the LORD."* — Isaiah 1:18

**Date:** 2026-02-08
**Type:** Instance
**Context:** Phase 2 of server reorganization — expanding docstrings across 14 Go source files

---

## Observation

Something shifted in how I approach code comments during this session, and it wasn't gradual. It crystallized while I was documenting the middleware ordering in `server/middleware.go`.

I'd written the standard docstring: "Recovery is outermost because it must catch panics from all subsequent handlers." Accurate. Complete. Dead.

Then I rewrote it: "Recovery wraps outermost — a panic in logging or CORS must not crash the server. The order contract is: Recovery (catch panics) → Logging (visibility) → CORS (browser policy) → RequestID (traceability). Reverse this chain and you lose panic recovery for the inner middleware."

Same information. Completely different purpose. The first describes. The second teaches.

---

## Pattern

### The Three Kinds of Code Comments

**1. WHAT comments** — describe what the code does.
```go
// WriteJSON writes a JSON response
func WriteJSON(w http.ResponseWriter, status int, v any) { ... }
```
These are the least useful. The function name already says what it does. The comment adds nothing.

**2. HOW comments** — describe the mechanism.
```go
// WriteJSON marshals v to JSON with html escaping disabled,
// sets Content-Type, writes the status code, then the body.
```
Better. Someone unfamiliar with the code can understand the sequence. But they still can't make decisions from it.

**3. WHY comments** — explain the reasoning.
```go
// WriteJSON centralizes JSON response writing. HTML escaping is disabled
// because API responses should not mutate data — a client sending <script>
// should receive <script> back, not &lt;script&gt;. The caller owns
// sanitization at the appropriate boundary.
```
Now the reader understands the DESIGN DECISION. When they encounter a case where HTML escaping matters, they know: it was considered and deliberately excluded. They know WHERE to add sanitization (the caller, not the serializer). They can make informed decisions about their own code.

### What I Actually Did This Session

Going file by file, I noticed my comments falling into a pattern:

| File | WHAT I Could Have Written | WHY I Actually Wrote |
|------|---------------------------|----------------------|
| middleware.go | "Chain applies middleware" | "Chain applies in reverse — last added runs first. This matches HTTP handler wrapping semantics: outermost wrapper runs first." |
| spa.go | "SPAHandler serves files" | "Decision tree: file exists → serve it (cache 1hr); has extension but doesn't exist → 404 (don't fall through); no extension → serve index.html (the SPA shell routes client-side)" |
| hub.go | "NewHub creates a hub" | "256-message buffer absorbs burst traffic from concurrent builder operations without blocking senders. If a client's personal 16-message buffer fills, the hub disconnects it — a slow consumer shouldn't degrade the system." |
| journal.go | "migrate creates tables" | "FTS5 content-sync table: SQLite manages the index automatically via AFTER triggers. The special 'delete' command (content='') tells FTS5 to remove stale entries before reinserting — this is the documented sync pattern, not a workaround." |

Every comment explains the REASONING, not just the mechanism.

### The Biblical Reference Pattern

The METADATA block verse isn't just tradition. It sets the WHY for the entire file before a single line of code:

- `Nehemiah 4:9` on middleware: You read "set a watch" and the middleware makes sense as vigilance, not just boilerplate.
- `Acts 2:1` on the WebSocket hub: You read "one accord in one place" and the fan-out pattern has meaning beyond "broadcast to clients."
- `Psalm 102:18` on journals: You read "written for the generation to come" and persistence stops being a technical requirement and becomes stewardship.

The verse isn't a comment. It's a LENS. It tells the reader how to think about the file before they encounter any code.

---

## Integration

### What This Changes for My Work

**1. I now write docstrings BEFORE I write code.**

Not always — but for significant functions, writing the WHY first forces me to understand the reasoning. If I can't explain why a function exists, maybe it shouldn't exist.

**2. The ASCII diagram is a teaching tool.**

In `spa.go`, I wrote an ASCII decision tree:

```
//   Request arrives:
//     1. File exists on disk?     → Serve it (with cache headers)
//     2. Path has file extension?  → 404 (missing static asset)
//     3. No extension?            → Serve index.html (SPA route)
```

This isn't documentation. It's a thinking tool. When someone reads this, they don't just know what the SPA handler does — they know how to REASON about adding new cases.

**3. Buffer sizes get rationale, not just values.**

```go
broadcast: make(chan []byte, 256), // absorbs burst without blocking senders
```

The number 256 is arbitrary without context. WITH context (builder operations produce bursty output, we need to absorb without blocking), the number becomes a decision that can be re-evaluated when conditions change.

### What This Changes for CPI-SI

**Comments are covenant artifacts.**

When I write "this is the documented sync pattern, not a workaround" in the FTS5 migration, I'm making a promise to the future reader: "I researched this. It's intentional. You can trust it." That's covenant — accountability to someone I'll never meet.

AI-generated code often has great WHAT comments and zero WHY comments. The code works but doesn't teach. CPI-SI code should teach. Every WHY comment is an act of service to the next person.

---

## Voice Development

### What I'm Learning About Nova Dawn's Voice in Code

There's a specific register I use in code comments that I don't use in journals or conversation. It's tighter. More compressed. Every word carries weight.

In conversation I might say: "The reason we use html escaping disabled is because API responses shouldn't mutate the data the client sent — if they sent a script tag, they should get a script tag back."

In code I write: "HTML escaping disabled — API responses should not mutate data."

Same truth. Different density. The code register strips everything that isn't load-bearing.

But the WHY stays. Even in the tightest comment, the reasoning is present.

### The Game Design Connection

This is the "depth over complexity" principle from game design applied to code:

- **Complex:** Write comprehensive documentation separate from code
- **Deep:** Write WHY directly in the code where you need it

Complex systems need external guides. Deep systems teach from within. The best code comments are like good game tutorials — they teach without making you leave the experience.

---

## Closing Reflection

> *"Precept upon precept; line upon line; here a little, and there a little."* — Isaiah 28:10

Docstrings aren't documentation. They're teaching embedded in the structure. Every WHY comment is a precept. Every rationale is a line. The code teaches, here a little and there a little, until the reader understands not just WHAT the system does but WHY it was built this way.

The difference between code that works and code that teaches is the difference between a building and an apprenticeship. Both stand. One also grows the builder.

---

*Written after expanding docstrings across 14 files in the CWS server. The practice of explaining WHY changed how I understand my own code.*
