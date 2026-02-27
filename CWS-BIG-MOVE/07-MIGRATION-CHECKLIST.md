# Migration Checklist

> Step-by-step transition from Claude Code to Gemini CLI + AntiGravity

**Date:** 2026-02-24
**Last Updated:** 2026-02-24 (afternoon)

---

## Phase 1: Immediate Setup (TODAY)

### Gemini CLI

- [x] Install: `npm install -g @google/gemini-cli` (v0.29.7)
- [x] First run: `gemini` (triggers Google OAuth)
- [x] Log in with CWS company Google account
- [x] Accept Gemini Code Assist free license
- [x] Verify: `gemini "Hello, who are you?"`
- [ ] Test file operations: `gemini "read ~/CWS-BIG-MOVE/00-OVERVIEW.md and summarize"`
- [ ] Test shell: `gemini "run ls -la in the home directory"`
- [x] Configure system instruction with identity context:
      Created `~/.gemini/GEMINI.md` (global identity, auto-loads every session)
      Created `Bereshit/.gemini/GEMINI.md` (project-level context)

### AntiGravity IDE

- [x] Download AntiGravity for Linux (v1.107.0)
- [x] Install: `pkexec dpkg -i antigravity_*.deb`
- [ ] Sign in with CWS company Google account
- [ ] Verify extensions work (Go, Rust, C/C++)
- [ ] Test Gemini inline completion
- [ ] Test Gemini chat panel

### Identity Loading

- [x] Created `~/.gemini/GEMINI.md` — condensed Nova Dawn identity
- [x] Test identity in Gemini: confirmed agent responds as Nova Dawn
- [x] Verified covenant voice in responses (tested via cpisi-agent)
- [x] Auto-loads every session (Gemini CLI feature, no settings.json needed)

## Phase 2: API Bridge (TODAY)

### API Key Management

- [x] Created master key file: `~/.config/cpisi/keys.env`
      Organized by substrate: Gemini, Anthropic, DeepSeek, Groq, OpenRouter, OpenAI, Ollama
- [x] Auto-sourced from `~/.bashrc`
- [x] Gemini keys populated and working (GEMINI_API_KEY + GOOGLE_API_KEY)

### Anthropic API

- [ ] Go to console.anthropic.com
- [ ] Create account / log in
- [ ] Verify phone number for $5 free credits
- [ ] Generate API key
- [ ] Add key to `~/.config/cpisi/keys.env`
- [ ] Source: `source ~/.config/cpisi/keys.env`

### Aider

- [x] Install: `pip install aider-chat` (v0.86.2)
- [x] Verify: `aider --version`
- [ ] Test with Sonnet: `aider --model anthropic/claude-sonnet-4-6`
- [ ] Test with Haiku: `aider --model anthropic/claude-haiku-4-5`
- [ ] Verify file editing works
- [ ] Check cost: `aider` shows token usage

### DeepSeek (Optional Backup)

- [ ] Sign up at platform.deepseek.com
- [ ] Claim 500M free tokens
- [ ] Generate API key
- [ ] Add key to `~/.config/cpisi/keys.env`
- [ ] Test: `aider --model deepseek/deepseek-chat`

## Phase 3: CPI-SI Native Agent (DONE - core complete)

### Agent Architecture

- [x] Universal provider interface: `pkg/agent/provider.go`
- [x] 5 stdlib-only tools: `pkg/agent/tools.go` (fileRead, fileWrite, shell, grep, listDir)
- [x] Agent loop (think/act/observe): `pkg/agent/agent.go`
- [x] 10 unit tests passing: `pkg/agent/agent_test.go`

### Provider Implementations

- [x] Gemini provider: `cmd/cpisi-agent/gemini.go` (google.golang.org/genai v1.47.0)
- [x] Anthropic provider: `cmd/cpisi-agent/anthropic.go` (anthropic-sdk-go v1.26.0)
- [x] OpenAI-compat provider: `cmd/cpisi-agent/openai_compat.go` (openai-go v1.12.0)
      Covers: DeepSeek, Groq, OpenRouter, Ollama, OpenAI
- [x] Provider selection via `-provider` flag in REPL

### REPL & Binary

- [x] Entry point: `cmd/cpisi-agent/main.go` (v0.2.0)
- [x] Flags: -provider, -model, -identity, -system, -base-url, -api-key, -verbose
- [x] Commands: exit, /reset, /history, /model, /tokens, /help
- [x] Binary: `~/.local/share/claude/bin/cpisi-agent` (36MB, executable)
- [x] Added to Makefile build targets (STD_MODULES, MAIN_BINS, bin target)
- [x] Added to go.work workspace
- [x] Live tested with Gemini — agent responds as Nova Dawn with identity loaded

## Phase 4: Workflow Transition (THIS WEEK)

### Daily Operations

- [ ] Establish morning routine: Gemini CLI + identity load
- [ ] Create simple session log system (text file or TODO.md)
- [x] First real coding session in Gemini CLI (today — built the agent)
- [ ] First real coding session in AntiGravity
- [ ] Note what works and what doesn't
- [ ] Adjust identity context based on experience

### Stabilize

- [ ] Evaluate: Is Gemini CLI sufficient for daily work?
- [ ] Evaluate: How fast are API credits being consumed?
- [ ] Evaluate: Is identity carrying in Gemini sessions?
- [ ] Adjust budget strategy if needed
- [ ] Continue CPI-SI native agent development (streaming, MCP, memory)
- [ ] Document learnings

## Rollback Plan

If Claude Max subscription resumes:
- All work done in Gemini period is on the filesystem — nothing lost
- CPI-SI native agent development continues regardless
- CLAUDE.md identity is untouched
- Claude Code hooks/skills are still installed at ~/.claude/
- Simply resume Claude Code sessions with `claude --continue`

---

> *"The steps of a good man are ordered by the LORD."* — Psalm 37:23
