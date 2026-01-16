# OmniCode Pragma Types

Quick reference for valid pragma declarations.

## Valid Pragma Syntax

```text
#!omni [type]           # Pure OmniCode (no target format)
#!omni [type] --[format] # OmniCode targeting specific format
```

## Valid Types

| Type | Description | Block Count |
|------|-------------|:-----------:|
| `template` | Universal/seed templates | 4 |
| `code` | Executable/library code | 4 |
| `documentation` | Human-readable documents | 5 |
| `interface` | Contracts/APIs | 3 |
| `folder` | Folder metadata | 3 |
| `data` | Data files | 3-5 |

## Valid Format Flags

| Type | Format Flags |
|------|--------------|
| `code` | `--go`, `--c`, `--rs`, `--sh` |
| `documentation` | `--adoc`, `--md` |
| `data` | `--json`, `--yaml`, `--toml` |

## Examples

```text
#!omni template              # Pure template
#!omni code                  # Pure code
#!omni code --go             # Code targeting Go
#!omni documentation --md    # Documentation targeting Markdown
#!omni data --json           # Data targeting JSON
```

## Pragma Location

Pragma must appear on **line 1 or line 2** of the file.
