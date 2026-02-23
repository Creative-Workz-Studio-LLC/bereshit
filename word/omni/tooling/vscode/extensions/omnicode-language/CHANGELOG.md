# Changelog — OmniCode Language

All notable changes to the OmniCode Language extension.

---

## [0.3.0] — 2026-02-21

### Added

- **Structural alignment linting** via cws-struct integration
  - Inline diagnostics (errors, warnings, info) on save and open
  - Supports Go, Rust, TOML, JSON, JSONC, Makefile, OmniCode, OmniFold
  - Ternary lint policy: strict / balanced / growth
  - Debounced to avoid rapid re-lints
- **Health scoring** in status bar
  - Ternary scale: -100 (shavar) to +100 (tov)
  - Color-coded: green (50+), yellow (0-49), red (<0)
  - Tooltip shows per-block breakdown
  - Click for full health report
- **QuickFix code actions** from cws-struct fix suggestions
  - Remove, insert, and replace operations
  - One-click structural corrections
- **Command palette commands**
  - `OmniCode: Lint Current File`
  - `OmniCode: Lint All Open Files`
  - `OmniCode: Show Health Details`
  - `OmniCode: Show Output Channel`
- **Settings** under `omnicode.*` namespace
  - `cwsStructPath` — binary path with fallback chain
  - `lintOnSave` / `lintOnOpen` — toggle automatic linting
  - `policy` — ternary severity control
  - `debounceMs` — lint delay after save

### Changed

- Extension now has TypeScript entry point (was purely declarative)
- Moved from `~/.vscode/extensions/` to `Bereshit/word/omni/tooling/vscode/` source
- esbuild bundling for production-grade packaging

## [0.2.0] — 2026-02-20

### Added

- OmniFold (.ofd) language support with dedicated grammar
- OmniFold language configuration
- Dedicated OmniFold icon

## [0.1.0] — 2026-02-19

### Added

- Initial release
- OmniCode (.omni) syntax highlighting with 25+ pattern categories
- Code folding for block boundaries
- Bracket matching for standard and OmniCode scope markers
- Smart editing (auto-close, auto-indent)
- Language configuration

---

**CreativeWorkzStudio LLC** | Kingdom Technology
