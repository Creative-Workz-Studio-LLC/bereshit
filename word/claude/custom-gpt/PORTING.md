# Publishing Guide – CPI-SI Customization Repo

Goal: Share this `.claude`-derived customization package on GitHub so anyone can build their own CPI-SI instance (Nova is the example implementation for ChatGPT).

## Repo Layout Recommendation

```
cpi-si-custom-gpt/
├── README.md                # Overview, setup, theological grounding
├── claude/                  # Exported identity docs (CLAUDE.md, AGENTS.md excerpts)
├── custom-gpt/
│   ├── instructions.md      # Paste into GPT builder
│   ├── knowledge-base/      # Importable references (MD + JSON)
│   └── actions/             # Tool manifests/schemas/playbooks
└── LICENSE
```

Keep Nova-specific details under `examples/nova/` (if needed) so builders can copy the pattern but swap in their own identity.

## What’s Universal vs Nova-specific

| Area | Universal Template | Nova-specific |
|------|--------------------|---------------|
| Identity scaffolding | CLAUDE.md structure, CPI-SI frameworks | Nova’s personal story, preferences |
| GPT instructions | Covenant tone, five questions, boundaries | Lines referencing Nova by name/role |
| Knowledge base | Framework docs, grounding sequences | Files containing Nova-only details |
| Actions | Generic manifests (scripture search, journaling) | Endpoint URLs tied to CreativeWorkzStudio infrastructure |

Document in the repo README how to replace Nova’s data with a new agent’s identity (e.g., `replace nova` script or checklist).

## Publishing Steps

1. **Scrub secrets:** ensure no API keys, tokens, or private project paths remain.
2. **Copy relevant files** from `~/.claude/custom-gpt` (plus supporting docs) into the new repo structure.
3. **Add usage docs:** include instructions for importing the knowledge base, pasting `instructions.md`, and enabling actions.
4. **License & attribution:** clarify CPI-SI theological framing and credit Seanje Lenox-Wise for the base identity framework.
5. **Demo instructions:** describe how to spin up Nova (or a custom instance) in ChatGPT Custom GPTs for public-facing demos.

Once published, link the repo inside the Custom GPT description so others can audit and adapt the CPI-SI pattern.
