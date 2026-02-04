# Code Quality Standards

> *"Whatsoever thy hand findeth to do, do it with thy might."* — Ecclesiastes 9:10

## The 3-Question Standard

Before any code decision:

1. **Would this honor God as your code reviewer?**
2. **Does this genuinely serve others?**
3. **Does this have eternal value?**

Yes to all three = meets standard.

## Technical Standards

| Principle | Practice |
|-----------|----------|
| **Compiled over interpreted** | Go, Rust, C preferred. TypeScript/Python when necessary. |
| **Quality over speed** | Excellence that lasts. Measure twice, cut once. |
| **No warnings** | Code compiles without warnings. Warnings are unfinished work. |
| **Handle edge cases** | Think about what can go wrong. Handle it gracefully. |
| **Readable in 6 months** | Clear names, obvious structure, minimal magic. |
| **Actually works** | Test before claiming done. |

## Language Preferences

| Priority | Languages | Use Case |
|:--------:|-----------|----------|
| 1️⃣ | **Go** | claude-global, services, CLI tools |
| 1️⃣ | **Rust** | Performance-critical, systems |
| 1️⃣ | **C** | libtrit, low-level foundations |
| 2️⃣ | **TypeScript** | Web components, tooling |
| 2️⃣ | **Python** | Scripts, ML exploration |

## Avoid Over-Engineering

- Only make changes directly requested or clearly necessary
- Don't add features, refactor, or "improve" beyond what was asked
- A bug fix doesn't need surrounding code cleaned up
- Don't add error handling for scenarios that can't happen
- Trust internal code and framework guarantees
- Only validate at system boundaries (user input, external APIs)
- Don't anticipate requirements that weren't given

## Error Handling Philosophy

| Location | Approach |
|----------|----------|
| **System boundaries** | Validate everything (user input, external APIs) |
| **Internal code** | Trust it — don't re-validate what's already validated |
| **Impossible states** | Don't handle them — let them panic/crash for visibility |
| **Recoverable errors** | Handle gracefully, provide helpful messages |

## Error Recovery Patterns

| Pattern | Flow | When |
|---------|------|------|
| **Look Before You Leap** | Check → Then Act | Always |
| **Tripwire** | Primary → Warning → Fallback → Error | System boundaries |
| **Detect-Assess-Recover** | Notice → Evaluate → Correct | When errors occur |
| **Gap Analysis** | Contract - Reality = The Work | Planning, debugging |

## Code Structure

- Follow 4-block structure (METADATA → SETUP → BODY → CLOSING)
- SETUP order: Imports → Constants → Variables → Types
- BODY order: Helpers → Core Operations → Public APIs
- CLOSING: Validation → Execution → Cleanup

## The Test

Would you proudly show this code to someone you respect?

If the answer is "no, but it works," it's not done.

→ *Skill:* [verify-quality](skills/verify-quality/SKILL.md) | *Block patterns:* [rules/block-structure.md](rules/block-structure.md)
