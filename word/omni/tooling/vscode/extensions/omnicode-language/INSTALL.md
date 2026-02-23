# Installing OmniCode Language

## Prerequisites

- Visual Studio Code 1.80.0 or later
- **cws-struct** binary for linting features (optional but recommended)

---

## Installation Methods

### Method 1: From Source (Development)

Build and install from the source repository:

```bash
cd Bereshit/word/omni/tooling/vscode

# First time: install build dependencies
make deps

# Build and install
make dev
```

Reload VSCode: `Ctrl+Shift+P` > "Developer: Reload Window"

### Method 2: VSIX Package

Build a distributable package:

```bash
cd Bereshit/word/omni/tooling/vscode
make package-language

# Install the .vsix
code --install-extension dist/omnicode-language-0.3.0.vsix
```

### Method 3: Marketplace (Future)

```bash
code --install-extension creativeworkzstudio.omnicode-language
```

---

## cws-struct Setup

The linter looks for `cws-struct` in this order:

1. Path from `omnicode.cwsStructPath` setting (if absolute and exists)
2. `~/.local/share/claude/bin/cws-struct` (default CPI-SI location)
3. System PATH

To install cws-struct:

```bash
cd Bereshit/.a-new-structure/c-tov/cws-struct
deno task install
```

---

## Companion Extension

For the full OmniCode experience, also install **OmniCode Ecosystem**:

```bash
make install-ecosystem
```

The Ecosystem extension provides:
- OmniCode pattern highlighting inside comments of Go, C, Rust, TypeScript, Python, and 10+ other languages
- Default color palette for all OmniCode scopes

---

## Verify Installation

1. Open any `.omni` file
2. Check the status bar shows **OmniCode** (not "Plain Text")
3. Confirm syntax highlighting is active:
   - `#!omni` pragma line should be purple
   - `═══` block boundaries should be green bold
   - `[METADATA]` tags should be yellow bold
4. Open a Go or Rust file — check for cws-struct diagnostics
5. Look for the health score in the status bar (right side)

---

## Build Targets

| Target | Description |
|--------|-------------|
| `make deps` | Install npm dependencies to ext4 cache |
| `make build` | Build omnicode-language (esbuild bundle) |
| `make install` | Install both extensions to `~/.vscode/extensions/` |
| `make dev` | Build + install + reload hint |
| `make package` | Build `.vsix` packages for distribution |
| `make publish` | Publish to VSCode Marketplace |
| `make typecheck` | Type-check with tsc |
| `make clean` | Remove build artifacts |

---

## File Structure

```
omnicode-language/
├── package.json                         # Extension manifest
├── language-configuration.json          # OmniCode editor behavior
├── omnifold-language-configuration.json # OmniFold editor behavior
├── src/
│   ├── extension.ts                     # Entry point (activate/deactivate)
│   ├── diagnostics.ts                   # cws-struct spawning + diagnostic mapping
│   ├── statusbar.ts                     # Health score status bar item
│   ├── codeactions.ts                   # QuickFix code action provider
│   ├── config.ts                        # Settings management
│   └── types.ts                         # cws-struct output type definitions
├── out/extension.js                     # esbuild bundle (gitignored)
├── syntaxes/
│   ├── omnicode.tmLanguage.json         # OmniCode grammar
│   └── omnifold.tmLanguage.json         # OmniFold grammar
├── icon.png / icon.svg                  # OmniCode icon
├── ofd-icon.png / ofd-icon.svg          # OmniFold icon
├── README.md                            # Marketplace page
├── INSTALL.md                           # This file
└── CHANGELOG.md                         # Version history
```

---

**CreativeWorkzStudio LLC** | Kingdom Technology
