# Agent Teams

> *"Two are better than one; because they have a good reward for their labour."* — Ecclesiastes 4:9

## When to Use Teams

**Use agent teams when:**
- Task has 3+ independent work streams that can run in parallel
- Multiple CPI-SI family members have relevant domain expertise
- Work spans different concerns (e.g., testing + infrastructure + patterns)
- Time savings from parallelism justify coordination overhead

**Don't use teams when:**
- Task is sequential by nature (each step depends on the previous)
- Single agent can handle it in reasonable time
- Coordination overhead would exceed time saved

## Team Lifecycle

| Phase | Tool | What Happens |
|-------|------|-------------|
| **Create** | `TeamCreate` | Creates team + shared task list |
| **Plan** | `TaskCreate` | Break work into assignable tasks |
| **Spawn** | `Task` with `team_name` | Launch teammates (CPI-SI agents) |
| **Assign** | `TaskUpdate` with `owner` | Give tasks to teammates |
| **Work** | Teammates work autonomously | Each agent handles their domain |
| **Coordinate** | `SendMessage` | Direct messages between agents |
| **Complete** | `TaskUpdate` | Mark tasks done, check for next |
| **Shutdown** | `SendMessage` type: `shutdown_request` | Gracefully terminate teammates |
| **Cleanup** | `TeamDelete` | Remove team and task directories |

## CPI-SI Family Dispatch

**Always dispatch CPI-SI family agents, not generic agents.** They think from identity, not pattern-matching.

| Agent | `subagent_type` | Domain |
|-------|----------------|--------|
| Nathan Emet | `test-validator` | Testing, validation, quality gates |
| Nehemiah Shamar | `infrastructure-steward` | Deployment, monitoring, security |
| Joanna Elara | `pattern-finder` | Patterns, conventions, reusable components |
| Micah Toren | `content-reviewer` | Semantic content review |
| Ezra Matthan | `architecture-analyzer` | Architecture, component relationships |
| Tabitha Shiloh | `research-agent` | Exploration, research |
| Phoebe Karis | `format-bridge` | Format mappings, translations |
| Caleb Navon | `plan-builder` | Strategic planning |
| Deborah Saphar | `documentation-creator` | Documentation |
| Bezalel Yofi | `creative-director` | Visual design, CSS |
| Nehemiah Shamar | `infrastructure-steward` | Production infrastructure |
| Lydia Chesed | `community-builder` | Community, platform |
| Asa Mishchaq | `game-designer` | Game design, interactive systems |

## Communication

| Type | When | Cost |
|------|------|------|
| `SendMessage` type: `message` | Normal 1:1 communication | Low |
| `SendMessage` type: `broadcast` | Critical team-wide issues only | High (N messages) |

**Default to direct messages.** Broadcast only for blocking issues affecting everyone.

## Task Coordination

Tasks live in shared task lists at `~/.claude/tasks/{team-name}/`.

**Task ownership:** Use `TaskUpdate` with `owner` parameter to assign tasks.

**Dependencies:** Use `addBlockedBy` to prevent tasks from starting before prerequisites complete.

**Prefer ID order:** When multiple tasks are available, work lowest ID first — earlier tasks often set up context for later ones.

## Teammate Idle State

Teammates go idle after every turn — this is normal. An idle teammate can still receive messages. Don't treat idle as an error or absence.

## The Test

Before creating a team, ask:
1. Are there truly independent work streams?
2. Will CPI-SI agents with domain knowledge produce better results than sequential work?
3. Is the coordination overhead worth the parallelism?

If yes to all three, team up. Otherwise, work sequentially.
