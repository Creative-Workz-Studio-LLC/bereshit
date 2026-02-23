# Asa Mishchaq — Agent Memory

## Data Layer (cws-struct)

- **42 containers** across 4 blocks (METADATA:6, SETUP:15, BODY:13, CLOSING:8)
- **11 concepts** forming a 462-cell ternary matrix (granted/denied/defer)
- **5 forms** (bare-bone, module, library, executable, demo-test) — class system, not progression
- **Detection weight** = |avg| of concept_map — measures decisiveness, not diagnostic value
- **Transpose invariant** is the system's strongest integrity check — `verifyTranspose()` in types.ts

## Game Design Observations (2026-02-22)

- concept_map = CHARACTER IDENTITY MATRIX, not permission system
- `defer` = conditional immunity — needs documented trigger conditions for automated use
- Detection weight symmetry problem: all-granted and all-denied both = 1.00, but diagnostic value is opposite
- Forms are orthogonal class selection (fighter/wizard), not linear difficulty (level 1-5)
- Section budget is conserved: 37 total per form, differing in required/available/reserved allocation
- Three overlapping games: designer (constraint satisfaction), linter (pattern matching), developer (construction)

## Universal GUI/CLI/TUI Design (2026-02-22) — DRAFT

- **Architecture:** Command Bus + Adapters (game engine pattern: input -> command -> logic -> render)
- **Language:** Go (matches cws-server, cross-compiles ARM64, BubbleTea + cobra)
- **Five modules:** CLI (`cws`), TUI (`cws tui`), API Server (`cws serve`), Tauri GUI (existing), Web (existing)
- **Command Bus:** Typed Command struct -> Core Engine -> Typed Result. Interface-agnostic.
- **Form system for the tool itself:** workstation (full), vm (ops subset), ci (headless), remote (TUI-only)
- **Phases:** 0=lint wrapper, 1=status+TUI, 2=API server, 3=Tauri connect, 4=full suite
- **Key insight:** The health scoring system IS a game UI — Hebrew states as XP bar, lint results as combat log
- **Existing infrastructure:** CWS Server (Go, Oracle ARM64), Tauri Dashboard (Rust, Phase 0), ops-frontend (Vite+TS)
- **Awaiting Seanje's input:** naming (`cws` vs `cwst`), priority (Phase 0 vs 1), Go module path, scope
- **Design doc written:** `aside/asa-designs/universal-interface-system.md` (full design with Go types, TUI mockup, Makefile, module structure)

## Key Files

- Types: `lib/data/types.ts`
- Schemas: `.a-new-structure/b-word/L1-omnicode/ladder/schemas/code/`
  - Content: `R25_blocks/{block}/{section}/_content.jsonc`
  - Structure: `R25_blocks/{block}/{section}/_structure.jsonc`
  - Forms: `forms/{name}/_structure.jsonc`
