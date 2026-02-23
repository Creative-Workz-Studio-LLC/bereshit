# Installing OmniCode Ecosystem

## Prerequisites

- Visual Studio Code 1.80.0 or later

---

## Installation Methods

### Method 1: Local Development (Current)

The extension is already installed at:

```
~/.vscode/extensions/omnicode-0.1.0/
```

If you need to reinstall from the repository:

```bash
cp -r <repo>/word/omni/tooling/vscode/omnicode-ecosystem/ ~/.vscode/extensions/omnicode-0.1.0/
```

Reload VSCode: `Ctrl+Shift+P` > "Developer: Reload Window"

### Method 2: VSIX Package

Build and install a distributable package:

```bash
# Install vsce if not already available
npm install -g @vscode/vsce

# Build the package
cd ~/.vscode/extensions/omnicode-0.1.0
vsce package

# Install
code --install-extension omnicode-ecosystem-0.2.0.vsix
```

### Method 3: Marketplace (Future)

```bash
code --install-extension creativeworkzstudio.omnicode-ecosystem
```

---

## Companion Extension

For `.omni` file support, also install **OmniCode Language**:

```
~/.vscode/extensions/omnicode-language/
```

The Language extension provides:
- Full syntax highlighting for `.omni` files (25+ pattern categories)
- Code folding for OmniCode blocks
- Bracket matching and smart editing

---

## What This Extension Does

OmniCode Ecosystem is an **injection grammar** — it adds OmniCode highlighting into comments of other languages. No configuration needed. Once installed, OmniCode patterns in your Go, C, Rust, TypeScript, Python, TOML, JSONC, and other files light up automatically.

It also provides the **default color palette** (30+ token color rules) used by both extensions.

### Supported Languages

Go, C, C++, Rust, TypeScript, TSX, JavaScript, JSX, Python, Shell/Bash, Makefile, TOML, YAML, JSONC, CSS, C#

---

## Verify Installation

1. Open any Go, C, or TypeScript file that uses OmniCode block comments
2. Confirm OmniCode patterns are highlighted:
   - `// ═══════════` block boundaries should be green bold
   - `// METADATA BLOCK` should be teal bold
   - `// [PRAGMA]` tags should be yellow bold
   - `// P.1 CORE` section addresses should be light blue bold
3. Open a JSONC file with `_pragma` or `_P1_key` fields — they should appear gray italic

If nothing highlights:
- `Ctrl+Shift+P` > "Developer: Reload Window"
- Check the extension appears in `Ctrl+Shift+X` extensions list
- Verify `extensions.json` includes the `creativeworkzstudio.omnicode-ecosystem` entry

---

## Customizing Colors

The extension sets default colors via `configurationDefaults`. To override any color, add to your `settings.json`:

```jsonc
{
  "editor.tokenColorCustomizations": {
    "textMateRules": [
      {
        "scope": "omnicode.structure.block-boundary",
        "settings": {
          "foreground": "#YOUR_COLOR"
        }
      }
    ]
  }
}
```

See the README for the full list of scopes and their default colors.

---

## Uninstalling

```bash
rm -rf ~/.vscode/extensions/omnicode-0.1.0/
```

Then reload VSCode.

---

## File Structure

```
omnicode-0.1.0/
├── package.json                                # Extension manifest
├── icon.png                                    # Extension icon (128x128)
├── icon.svg                                    # Icon source
├── README.md                                   # Store page
├── INSTALL.md                                  # This file
└── syntaxes/
    ├── omnicode-injection.tmLanguage.json       # Injection grammar (16 host languages)
    └── omnicode-jsonc-meta.tmLanguage.json      # JSONC meta field highlighting
```

---

**CreativeWorkzStudio LLC** | Kingdom Technology
