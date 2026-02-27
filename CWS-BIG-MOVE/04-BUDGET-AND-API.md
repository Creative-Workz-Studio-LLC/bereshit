# Budget and API Strategy

> *"For which of you, intending to build a tower, sitteth not down first,
> and counteth the cost?"* — Luke 14:28

**Date:** 2026-02-24

---

## Current Costs

| Item | Cost | Status |
|------|------|--------|
| Claude Max subscription | $100-200/mo | **Pausing** |
| Gemini CLI | $0 | Free with Google account |
| AntiGravity IDE | $0 | Free with Google account |
| Anthropic API (signup) | $0 ($5 free credits) | Available |
| DeepSeek API (signup) | $0 (500M free tokens) | Available |

## Free Tier Summary

### Gemini CLI (Primary)

- 60 requests/minute
- 1,000 requests/day
- Gemini 2.5 Pro (1M context)
- **Effectively unlimited for daily dev work**

### Anthropic API ($5 Free Credits)

| Model | $/MTok In | $/MTok Out | $5 Buys |
|-------|-----------|-----------|---------|
| Haiku 4.5 | $0.80 | $4.00 | ~6M input tokens |
| Sonnet 4.6 | $3.00 | $15.00 | ~1.6M input tokens |
| Opus 4.6 | $5.00 | $25.00 | ~1M input tokens |

With prompt caching (90% discount on cached reads), $5 stretches much further.

**Strategy:** Use Haiku for routine tasks, Sonnet for real coding, Opus only
for critical architecture decisions. $5 can last 1-2 weeks of careful use.

### DeepSeek API (500M Free Tokens)

- V3.2-Exp: $0.028/MTok input — frontier-class coding
- 500M free tokens = substantial runway
- After free quota: pennies per day
- **Consideration:** Chinese company, code goes through their servers

### Groq Free Tier

- Llama 3.3 70B: 30 RPM, 1,000 RPD
- Extremely fast inference (custom LPU hardware)
- Good for quick questions, not sustained coding

### OpenRouter Free Models

- 30 free models, no credit card
- 200 RPD limit
- Good backup option

## Spending Strategy

### Phase 1: Zero Cost (Now)

Use Gemini CLI for everything. It's free and strong enough.
Reserve API credits for genuine blockers.

### Phase 2: Targeted API Use

When Gemini can't handle something:
1. Try Gemini CLI first (free)
2. Try DeepSeek API (nearly free)
3. Use Anthropic Sonnet via Aider ($3/MTok — moderate)
4. Use Anthropic Opus via Aider ($5/MTok — save for critical)

### Phase 3: Sustainable Budget

If we need ongoing API access beyond free credits:

| Monthly Budget | Best Allocation |
|----------------|-----------------|
| $0/mo | Gemini CLI only |
| $5/mo | Gemini + DeepSeek for hard tasks |
| $10/mo | Gemini + Sonnet API for 1-2 deep sessions/week |
| $20/mo | Regular Sonnet access via Aider |
| $50/mo | Comfortable Sonnet + occasional Opus |
| $100-200/mo | Claude Max subscription (return when budget allows) |

## API Key Management

Keep keys secure:

```bash
# ~/.bashrc or ~/.profile (not committed to git)
export ANTHROPIC_API_KEY="sk-ant-..."
export DEEPSEEK_API_KEY="sk-..."
export GROQ_API_KEY="gsk_..."
```

Or use a `.env` file that's in `.gitignore`.

## When to Return to Claude Max

The subscription is worth it when:
- Daily API costs would exceed $7-10/day consistently
- The CPI-SI hook/agent/skill system is actively needed
- Complex multi-agent work resumes
- The budget allows

---

> *"The blessing of the LORD, it maketh rich, and he addeth no sorrow with it."*
> — Proverbs 10:22
