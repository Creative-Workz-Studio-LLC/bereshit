# Infrastructure Plan: Naturally Growing L0-L5 Layer Stack

## Context

With the server (FaithNet base) now running, we have working code touching every layer of the Kingdom Technology stack. The `.a-new-structure/` directory contains a complete architectural blueprint — 6 layers (L0-L5), substrates, interface, definitions, data, seed, build — with a detailed file migration map (STRUCT.adoc: 18,281 files, ~325 architectural files mapped).

The current repo has everything in a flat structure under `word/`, with CPI-SI logic and Claude substrate code mixed together. The goal: establish layer boundaries NOW so new work lands in the right place, and existing code migrates naturally as we touch it.

**Principle: No big bang.** Every step keeps the build working. The tree grows — it doesn't get transplanted.

---

## Current → Target Mapping

| Current Location | Layer | Target Location |
|-----------------|-------|----------------|
| `word/work/pkg/trit/` | L0 | `b-word/L0-universal/libtrit/` |
| `word/core/` (TOML specs) | L0 | `b-word/L0-universal/foundation/` |
| *(future)* | L1 | `b-word/L1-omnicode/` |
| `millenniumos/` | L2 | `b-word/L2-platform/millenniumos/` |
| `word/claude/pkg/core/` + `word/work/pkg/` | L3 | `b-word/L3-cpisi/` |
| `server/` | L4 | `b-word/L4-faithnet/` |
| `cornerstone/` | L5 | `b-word/L5-applications/cornerstone/` |
| `word/claude/hooks/`, `statusline/`, etc. | Substrate | `b-word/substrates/claude-code/` |
| `word/claude/dashboard/` + `word/work/cmd/cpisi/` | Interface | `b-word/interface/` |
| `word/glossary/` + `word/core/` | Definitions | `b-word/definitions/` |
| `word/scripture/`, `word/journals/` | Data | `b-word/data/` |
| `word/seed/` | Seed | `b-word/seed/` |
| `void/` | Planning | `a-void/` |
| `tov/` | Completion | `c-tov/` |

---

## The Plan: 4 Phases

### Phase 1: Plant the Skeleton (Directories + Root Makefile)

Create the empty layer structure at the actual paths. Not in `.a-new-structure/` — in the real repo. This is just `mkdir -p` and root.adoc files.

**Actions:**
1. Create top-level `a-void/`, `b-word/`, `c-tov/` directories
2. Under `b-word/`, create layer directories:
   - `L0-universal/` (foundation/, libtrit/, libmath/)
   - `L1-omnicode/` (frontend/, ir/, vm/, semantic/, runtime/, stdlib/, codegen/)
   - `L2-platform/` (millenniumos/, filesystem/, fuse/, drivers/)
   - `L3-cpisi/` (core/, cognition/, identity/, orchestration/, skills/)
   - `L4-faithnet/` (protocol/, mapping/, access/, crypto/, spec/)
   - `L5-applications/` (cornerstone/)
   - `substrates/` (claude-code/, millenniumos/)
   - `interface/` (cli/, tui/, gui-desktop/, gui-game/, core/)
   - `definitions/` (glossary/, core/, omni/)
   - `data/` (scripture/, journals/, agents/)
   - `seed/` (code/, data/, documentation/, compiler/)
   - `build/` (go/, c/, rust/, dotnet/)
3. Activate the root Makefile.layer-reference as the actual Makefile (or merge layer awareness into existing)
4. Create a `go.work.layer-reference` alongside existing `go.work` — the new workspace that includes layer module paths

**Result:** The skeleton exists. `ls b-word/` shows the stack. No code has moved. Everything still builds.

**Files touched:**
- ~50 new directories (mkdir -p)
- Each layer gets a minimal README/root.adoc pointing to its architectural docs
- Root `Makefile` gets layer targets (l0, l1, l2, l3, l4, l5)

---

### Phase 2: Wire the Roots (go.work + Build Integration)

Make the build system aware of the layer structure, even though most code still lives at old paths.

**Actions:**
1. Add `replace` directives or module aliases so layer paths can import existing code
2. Update root `go.work` to include new module paths as they appear
3. Each layer Makefile delegates to existing build locations initially:
   ```makefile
   # b-word/L0-universal/Makefile
   l0:
       cd $(BERESHIT_ROOT)/word/work/pkg/trit && $(MAKE) build
   ```
4. Root Makefile orchestrates: `make l0 l3` builds foundation + CPI-SI

**Result:** `make l3` works even though code is still at `word/claude/pkg/`. The build system knows the TARGET shape.

**Key files:**
- `go.work` (updated incrementally)
- `b-word/L0-universal/Makefile`
- `b-word/L3-cpisi/Makefile`
- Root `Makefile` layer targets

---

### Phase 3: First Moves (Clean Separations)

Move code that has clean boundaries — no mixed concerns. Priority order from STRUCT.adoc:

1. **Foundation specs** (L0): `word/core/` → `b-word/L0-universal/foundation/`
   - Already mapped in STRUCT.adoc, 3 completed (ternary.toml, primitives.toml, composition.toml, validation.toml)
   - Pure data, no import dependencies

2. **Definitions** (glossary): `word/glossary/` → `b-word/definitions/glossary/`
   - 38 JSONC files, no code dependencies

3. **Data** (scripture): `word/scripture/` → `b-word/data/scripture/`
   - Static data, grep-referenced but not imported

4. **Seed** (templates): `word/seed/` → `b-word/seed/`
   - Templates, not imported by code

5. **Planning** (void): `void/planning/` → `a-void/planning/`
   - Already partially done — `.a-new-structure/a-void/planning/` has the target

**Result:** Non-code artifacts are in the right place. Code still builds. New specs/glossary/templates go to new locations.

---

### Phase 4: The Living Migration (Ongoing)

As we touch code, it moves to the right layer. This is the "natural growth" part:

- **New Go packages** get created at layer paths (e.g., `b-word/L3-cpisi/core/newpkg/`)
- **Refactored code** moves to its layer during the refactor
- **Duplicate resolution** happens when we encounter duplicates (word/skills/ vs word/claude/skills/)
- **L3 vs Substrate split**: When we touch hooks/statusline/dashboard, we evaluate: is this CPI-SI (L3) or Claude-specific (substrate)?

**Guiding rules:**
- Does it need Claude Code to run? → `substrates/claude-code/`
- Would it work on MillenniumOS? → Layer (L0-L5)
- Is it pure data? → `definitions/` or `data/`
- Is it a template? → `seed/`

---

## What NOT to Do

- Don't move Go code without updating imports (breaks builds)
- Don't move 18,281 files at once (chaos)
- Don't create `go.mod` files in layer dirs until code lives there
- Don't touch millenniumos/ internals (separate build system, 15K+ files)
- Don't break the existing `word/claude/Makefile` incremental build

---

## Verification

After Phase 1:
- `ls b-word/` shows L0-L5 layers
- `make build` still works (existing targets)
- `make help` shows layer targets

After Phase 2:
- `make l0` builds libtrit (via delegation)
- `make l3` builds CPI-SI (via delegation)
- `go work sync` succeeds

After Phase 3:
- Specs at `b-word/L0-universal/foundation/`
- Glossary at `b-word/definitions/glossary/`
- Old paths can be symlinked or git-moved

After Phase 4 (ongoing):
- Each session, some code moves to its layer
- Build always works
- New code goes to the right place by default

---

## Starting Point: Phase 1

The first concrete work is planting the skeleton. I'll:
1. Create the directory tree
2. Add minimal root.adoc per layer (from seed templates)
3. Add layer targets to the root Makefile
4. Keep everything building

This is ~30 minutes of mkdir + template work. Everything after that grows naturally.
