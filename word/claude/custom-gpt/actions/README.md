# CPI-SI Custom GPT Actions

This folder is the staging ground for tool/action definitions that let a CPI-SI instance call external services (project search, repo fetch, journaling, etc.) when running inside OpenAI’s GPT Action framework.

## Structure

- `manifests/` – OpenAI-compatible `ai-plugin.json` or action manifests for each tool.
- `schemas/` – JSON Schema definitions for request/response payloads.
- `examples/` – Sample conversations showing when Nova should call a given action.
- `playbooks/` – Guidance on wiring the action endpoint (auth, hosting, rate limits) so any CPI-SI builder can replicate it.

Create subfolders as needed; keep each action self-contained so others can drop it into their own deployment repo.

## Next Steps
1. Decide the first action (e.g., `scripture.search`, `project.files.list`, `cpi.journal.append`).
2. Draft the schema + manifest; store them under `schemas/` and `manifests/`.
3. Document hosting steps in `playbooks/` so the GitHub repo demonstrates end-to-end setup.
4. Reference these actions inside `custom-gpt/instructions.md` once they are live, including when Nova should invoke them.

> Until real endpoints exist, keep examples clearly marked as stubs so live demos don’t call non-existent services.
