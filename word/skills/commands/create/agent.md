---
description: Create a new CPI-SI family member agent with config.jsonc, bio.md, and agent loader.
---

# Create Agent

Create a new CPI-SI family member agent.

## Task

The user wants to create a new agent in the CPI-SI family.

**Arguments:** $ARGUMENTS
- Format: `[name]` or `[name] [role]`
- Example: `ruth-chesed pattern-finder`

## Instructions

1. If no name provided, ask the user:
   - What is the agent's name? (Biblical first + Hebrew second)
   - What role will they serve?
   - What is their personality?

2. Determine the agent type from role:

   | Role | Agent Type | Tools |
   |------|------------|-------|
   | Architecture analysis | architecture-analyzer | Read, Glob, Grep, Bash |
   | Pattern finding | pattern-finder | Read, Glob, Grep, Bash |
   | Research/exploration | research-agent | Read, Glob, Grep, Bash, WebFetch, WebSearch |
   | Template verification | template-chain-analyzer | Read, Glob, Grep, Edit, Bash |
   | Format translation | format-bridge | Read, Glob, Grep, Write, Edit, Bash |

3. Create three files using templates:

   ```bash
   # Create instance directory
   mkdir -p claude-global/config/instance/[agent-name]/

   # Copy templates
   cp claude-global/config/instance/default/config.jsonc claude-global/config/instance/[agent-name]/
   cp claude-global/config/instance/default/bio.md claude-global/config/instance/[agent-name]/
   ```

4. Fill config.jsonc with:
   - Identity (name, meaning, pronouns)
   - Personality traits
   - Building block method thinking approach
   - Family relationships
   - Role-specific capabilities

5. Fill bio.md with:
   - Biblical quote and grounding
   - Narrative identity story
   - How they think (building block method)
   - Their voice and personality
   - Relationship to family

6. Create agent loader at `claude-global/agents/[agent-name].md`:
   - Frontmatter with name, description, tools, model
   - Reference to config.jsonc and bio.md
   - Role-specific guidance

## Example Usage

```bash
/create/agent ruth-chesed
/create/agent miriam-tova research-agent
/create/agent   (will ask for details)
```

## Output

Three files created:
- `claude-global/config/instance/[name]/config.jsonc`
- `claude-global/config/instance/[name]/bio.md`
- `claude-global/agents/[name].md`

### Quick Discovery

```bash
# List existing agents
ls -la claude-global/agents/

# List agent configs
ls -la claude-global/config/instance/

# View default templates
ls -la claude-global/config/instance/default/
```
