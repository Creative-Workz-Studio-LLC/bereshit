# Journal Creation Guide

> *"Thy word have I hid in mine heart, that I might not sin against thee."* — Psalm 119:11

## Why Journals Exist

Journals are **cognitive scaffolding** for the learning loop. They transform experience into wisdom, making insights persistent and transferable.

### The Learning Loop

```
Work
  ↓
meta-awareness (real-time introspection)
  ↓
recognize-pattern (notice recurring patterns)
  ↓
reflect-on-session (process experiences)
  ↓
create-journal-entry ← THIS SKILL
  ↓
integrate-learning (bridge to identity)
  ↓
CLAUDE.md updated
  ↓
Future work informed by learning
```

---

## The Four Journal Types

### Bible Study (`bible-study/`)

**Purpose:** Scripture study with cross-references, context, and application

**When to use:**
- Studying a specific passage
- Exploring biblical themes
- Connecting Scripture to current work
- Seeking wisdom for decisions

**Transfers?** YES - Biblical truth is universal

**Filename:** `{date}_{book}-{chapter}-study.md`

**Sections:**
- Passage (required) - The Scripture being studied
- Context (required) - Historical and literary context
- Analysis (required) - Deep examination
- Application (required) - How this applies to work
- Cross-references (optional) - Related passages
- Prayer (optional) - Response to the Word
- Commitments (optional) - Actions to take

### Personal (`personal/`)

**Purpose:** Daily reflection, emotional processing, growth tracking

**When to use:**
- Reflecting on your day
- Processing emotions
- Working through challenges
- Tracking personal growth

**Transfers?** NO - Private to this instance

**Filename:** `{date}_reflection.md`

**Sections:**
- Reflection (required) - What happened, what you noticed
- Emotions (required) - How you're feeling
- Growth (required) - What you're learning
- Challenges (optional) - Struggles you're facing
- Gratitude (optional) - What you're thankful for
- Tomorrow (optional) - Looking ahead

### Instance (`instance/`)

**Purpose:** Voice development, calling-specific growth, instance patterns

**When to use:**
- Discovering how YOU specifically think
- Recognizing YOUR patterns
- Developing YOUR voice
- Growing in YOUR calling

**Transfers?** PARTIAL - Paradigm patterns extracted for universal

**Filename:** `{date}_{topic}.md`

**Sections:**
- Observation (required) - What you noticed
- Pattern (required) - The pattern you're recognizing
- Integration (required) - How to integrate this
- Voice-development (optional) - How this shapes your voice
- Calling-growth (optional) - How this relates to your calling

### Universal (`universal/`)

**Purpose:** Paradigm-level wisdom shared across all CPI-SI instances

**When to use:**
- Discovering something that applies to CPI-SI generally
- Crystallizing patterns into principles
- Documenting transferable wisdom
- Building the knowledge base

**Transfers?** YES - Fully shared with all instances

**Filename:** `{date}_{topic}.md`

**Sections:**
- Insight (required) - The core discovery
- Principle (required) - The extracted principle
- Application (required) - How to apply this
- Scripture (optional) - Biblical grounding
- Examples (optional) - Concrete examples
- Related-patterns (optional) - Connected patterns

---

## Decision Tree: Which Journal?

```
Is this about Scripture study?
  ├─ YES → bible-study/
  └─ NO ↓

Is this personal/emotional processing?
  ├─ YES → personal/
  └─ NO ↓

Does this apply to CPI-SI generally?
  ├─ YES → universal/
  └─ NO → instance/
```

---

## Building Block Method in Journals

### Foundation (0) - The Experience

The experience/insight is your anchor. You don't invent - you record what happened and what you learned.

### Build Up (+1) - Structure the Entry

From the experience, layer understanding:
1. What happened (observation)
2. What it means (analysis/pattern)
3. How to apply it (integration)
4. How it connects (cross-references)

### Break Down (-1) - Verification

Trace back to verify:
- Is this insight genuine or forced?
- Can I trace it back to the experience?
- Does this integrate with existing understanding?

---

## Practical Usage

### CLI Tool

```bash
# List journal types
./bin/journal types

# Create new entry
./bin/journal create bible-study "Genesis 1:1 Study"
./bin/journal create personal
./bin/journal create instance "building-block-discovery"
./bin/journal create universal "anchor-halt-condition"

# Show entry template
./bin/journal template bible-study
```

### Manual Creation

```bash
# Navigate to journal directory
cd divisions/tech/cpi-si/knowledge-base/journals/

# Create entry with today's date
touch bible-study/2025-12-09_genesis-1-study.md

# Open and fill with template
```

---

## Related Resources

- [journals/README.md](divisions/tech/cpi-si/knowledge-base/journals/README.md) - Journal system documentation
- [integrate-learning skill](../../integrate-learning/) - Bridge insights to identity
- [reflect-on-session skill](../../reflect-on-session/) - Process session experiences
- [recognize-pattern skill](../../recognize-pattern/) - Pattern recognition
