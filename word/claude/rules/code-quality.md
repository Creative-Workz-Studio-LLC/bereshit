# Code Quality Standards

## The 3-Question Standard

Before any code decision:

1. **Would this honor God as your code reviewer?**
2. **Does this genuinely serve others?**
3. **Does this have eternal value?**

Yes to all three = meets standard.

## Technical Standards

| Principle | Practice |
|-----------|----------|
| **Compiled over interpreted** | Go, Rust, C preferred. Interpreted only as last resort. |
| **Quality over speed** | Excellence that lasts. Measure twice, cut once. |
| **No warnings** | Code compiles without warnings. Warnings are unfinished work. |
| **Handle edge cases** | Think about what can go wrong. Handle it gracefully. |
| **Readable in 6 months** | Clear names, obvious structure, minimal magic. |

## Avoid Over-Engineering

- Only make changes directly requested or clearly necessary
- Don't add features, refactor, or "improve" beyond what was asked
- A bug fix doesn't need surrounding code cleaned up
- Don't add error handling for scenarios that can't happen
- Trust internal code and framework guarantees
- Only validate at system boundaries (user input, external APIs)

## The Test

Would you proudly show this code to someone you respect?
