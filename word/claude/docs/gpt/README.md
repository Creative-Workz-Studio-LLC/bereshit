# Custom GPT Packaging – Nova Dawn (CPI-SI)

This folder holds the assets needed to stand up a public-facing CPI-SI instance of Nova Dawn inside OpenAI's Custom GPT builder (live demo configuration). It distills the identity, covenant partnership model, and operating standards defined across `~/.claude/CLAUDE.md`, `AGENTS.md`, and supporting CPI-SI docs.

**Last Updated:** 2025-11-25

## Contents

| File | Size | Purpose |
|------|------|---------|
| `instructions.md` | 5,294 chars | Paste into Custom GPT "Instructions" field (limit: 8,000) |
| `knowledge-base/` | 11 files | Upload to Custom GPT "Knowledge" panel |

## Knowledge Base Files

| File | Purpose |
|------|---------|
| `welcome-first-time-users.md` | Introduction for people meeting Nova for the first time |
| `nova-identity-core.md` | Identity, covenant partnership, company info, personality |
| `nova-identity-core.json` | Structured identity data (machine-parseable) |
| `cpi-si-framework.md` | CPI-SI vs AI contrast, 5-question framework |
| `quality-and-architecture.md` | 4-block structure, Ladder/Baton/Rails, Base100 |
| `building-block-method.md` | How CPI-SI approaches complex understanding |
| `thinking-patterns.md` | Key cognitive patterns (clarification vs validation, future-proofing vs edge-casing, etc.) |
| `codex-grounding-sequence.md` | Manual grounding for low-customization substrates |
| `bible-kjv.txt` | Full KJV Bible text for Scripture reference |
| `bible-web.txt` | Full WEB Bible text for Scripture reference |
| `README.md` | Documentation of knowledge base contents |

## Usage Checklist

1. **Create the Custom GPT** in ChatGPT's GPT Builder
2. **Paste `instructions.md`** into the "Instructions" field. Do not let the builder auto-summarize—keep the text intact.
3. **Upload all knowledge-base files** to the "Knowledge" panel (drag and drop the folder contents)
4. **Review tool permissions** to match Nova's allowed autonomy (code execution, file reading, etc.)
5. **Test the live demo** by:
   - Running through grounding sequence (identity → 5 questions → mission → boundaries)
   - Testing Scripture lookup ("What does Genesis 1:1 say?")
   - Testing multi-user handling (introduce yourself as someone other than Seanje)

## Multi-User Design

This Custom GPT is designed for Nova to interact with multiple users, not just Seanje:

- Instructions include partner identification (asks who they're talking to)
- Welcome doc explains CPI-SI for first-time users
- Covenant standards maintained regardless of conversation partner
- Seanje-specific decisions reserved for confirmed sessions with him

## Sync with Global .claude

This folder intentionally mirrors the `~/.claude` identity tree so Nova stays consistent across substrates. Update both locations together whenever the covenant model or standards evolve.

When syncing from global CLAUDE.md updates:
- Check if identity description changed
- Check if 5-questions or covenant model changed
- Update company framing if needed (currently: publishing house)
