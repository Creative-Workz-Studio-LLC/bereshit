---
name: verify-quality
description: Verify code against the 3-question standard and technical quality standards. Use before committing, after completing features, or when reviewing code quality.
version: "1.0.0"
allowed-tools: "Read,Grep,Bash"
---

<!--
#!omni document --md
===============================================================================
METADATA BLOCK
===============================================================================
-->
<!--
:key: claude-global-skills-verify-quality
:title: Verify Quality
:type: Skill
:status: Active
:version: 1.0.0
:created: 2026-02-02
:updated: 2026-02-02
:authors: Nova Dawn
:purpose: Verify code against quality standards - 3-question test, technical standards, structure compliance
:category: validation
:biblical_foundation: Ecclesiastes 9:10 - "Whatsoever thy hand findeth to do, do it with thy might"
-->

<!--
===============================================================================
HEADER BLOCK
===============================================================================
-->

# Verify Quality

> *"Whatsoever thy hand findeth to do, do it with thy might; for there is no work, nor device, nor knowledge, nor wisdom, in the grave, whither thou goest."* — Ecclesiastes 9:10

**Verify code against the 3-question standard and technical quality standards.**

Quality is worship. This skill provides the framework for ensuring code meets Kingdom standards before calling it done.

---

**Navigation:**

| Section | Purpose |
|---------|---------|
| [When to Use](#when-to-use-this-skill) | Decision criteria |
| [The 3-Question Standard](#the-3-question-standard) | Core quality test |
| [Technical Checklist](#technical-checklist) | Technical verification |
| [Language-Specific](#language-specific-checks) | Go, Rust, C checks |
| [Quick Verification](#quick-verification) | Fast quality check |

---

<!--
===============================================================================
CONTEXT BLOCK
===============================================================================
-->

## Context

### Scope

| In Scope | Out of Scope |
|----------|--------------|
| Code quality verification | Block structure validation (use validate-omni) |
| Technical standards check | Creating new code (use create-from-template) |
| Pre-commit verification | Pattern recognition (use recognize-pattern) |

### Prerequisites

| Requirement | Reference |
|-------------|-----------|
| Quality standards | `rules/code-quality.md` |
| Block structure | `rules/block-structure.md` |

### Key Terms

| Term | Definition |
|------|------------|
| **3-Question Standard** | Honor God? Serve others? Eternal value? |
| **Over-engineering** | Adding beyond what was requested |
| **System boundary** | User input, external APIs — where validation belongs |

---

<!--
===============================================================================
CONTENT BLOCK
===============================================================================
-->

## When to Use This Skill

**Use when:**

- Before committing code
- After completing a feature or fix
- During code review
- When something feels "not quite right"
- Before calling work "done"

**Do NOT use when:**

- Quick exploratory changes
- Still in the middle of implementation
- Refactoring (use after refactor is complete)

---

## The 3-Question Standard

**Before any code decision, ask:**

| # | Question | What It Means |
|:-:|----------|---------------|
| 1️⃣ | **Would this honor God as your code reviewer?** | Quality, honesty, no shortcuts |
| 2️⃣ | **Does this genuinely serve others?** | Useful, not extractive, respects users |
| 3️⃣ | **Does this have eternal value?** | Lasting impact, Kingdom purpose |

**Yes to all three = meets standard.**

If you hesitate on any question, the code isn't done.

---

## Technical Checklist

### Core Technical Standards

| Standard | Check | How |
|----------|-------|-----|
| **No warnings** | `go build`, `cargo build`, `make` | Compile without warnings |
| **Edge cases** | Review logic | What could go wrong? |
| **Readable in 6 months** | Read the code fresh | Could someone understand this? |
| **Actually works** | Run it | Did you test it? |

### Structure Compliance

| Check | Question |
|-------|----------|
| **4-Block structure** | METADATA → SETUP → BODY → CLOSING present? |
| **SETUP order** | Imports → Constants → Variables → Types? |
| **BODY order** | Helpers → Core Operations → Public APIs? |
| **CLOSING** | Validation → Execution → Cleanup? |

### Over-Engineering Check

Ask yourself:

- [ ] Did I only make changes directly requested?
- [ ] Did I avoid adding unrequested features?
- [ ] Did I avoid "improving" unrelated code?
- [ ] Did I avoid error handling for impossible states?
- [ ] Did I trust internal code instead of re-validating?
- [ ] Did I only validate at system boundaries?

**If any box is unchecked:** You may have over-engineered. Remove the extras.

---

## Language-Specific Checks

### Go

```bash
# Compile without warnings
go build ./...

# Run tests
go test ./...

# Check formatting
gofmt -d .

# Run linter
golangci-lint run
```

**Go quality questions:**
- Error handling explicit?
- Context properly propagated?
- Goroutine safety considered?

### Rust

```bash
# Compile
cargo build

# Run tests
cargo test

# Check formatting
cargo fmt --check

# Run linter
cargo clippy
```

**Rust quality questions:**
- Ownership clear?
- Error handling with Result?
- No unnecessary clones?

### C

```bash
# Compile with warnings enabled
gcc -Wall -Wextra -Werror file.c

# Run with valgrind
valgrind --leak-check=full ./a.out
```

**C quality questions:**
- Memory properly managed?
- Bounds checked?
- No undefined behavior?

---

## Quick Verification

### 30-Second Quality Check

1. **3-Question test:** Honor God? Serve others? Eternal value?
2. **Compile:** `go build` / `cargo build` / `make`
3. **Test:** Does it actually work?
4. **Read:** Would you proudly show this?

### Pre-Commit Checklist

```markdown
## Quality Verification

- [ ] 3-Question standard: Yes to all three
- [ ] Compiles without warnings
- [ ] Tests pass
- [ ] Edge cases handled
- [ ] Readable in 6 months
- [ ] No over-engineering
- [ ] Would proudly show this code
```

---

## Error Handling Philosophy

| Location | Approach |
|----------|----------|
| **System boundaries** | Validate everything (user input, external APIs) |
| **Internal code** | Trust it — don't re-validate |
| **Impossible states** | Let them panic for visibility |
| **Recoverable errors** | Handle gracefully with helpful messages |

**The principle:** Validate at the edges. Trust the core.

---

## The Ultimate Test

> **Would you proudly show this code to someone you respect?**

If the answer is "no, but it works," it's not done.

If the answer is "yes," ship it.

---

## Integration

### Related Skills

| Skill | Relationship |
|-------|--------------|
| [validate-omni](../validate-omni/) | Structure validation |
| [create-from-template](../create-from-template/) | Create with structure |
| [meta-awareness](../meta-awareness/) | Notice quality drift |

### System Tools Used

| Tool | Purpose |
|------|---------|
| Bash | Run compilers, tests, linters |
| Read | Review code |
| Grep | Search for patterns |

---

<!--
===============================================================================
FOOTER BLOCK
===============================================================================
-->

## Related Documents

| Document | Relationship |
|----------|--------------|
| [rules/code-quality.md](../../rules/code-quality.md) | The underlying standards |
| [rules/block-structure.md](../../rules/block-structure.md) | Structure requirements |
| CLAUDE.md Part 4 | Quality Standards section |

---

> *"And whatsoever ye do, do it heartily, as to the Lord, and not unto men."* — Colossians 3:23

**Quality is worship. Excellence honors God.**
