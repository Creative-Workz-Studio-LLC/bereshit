# CWS Infrastructure — Ansible Playbooks

Reproducible setup for CreativeWorkzStudio LLC development environments.

## Quick Start

```bash
# Full workstation setup
ansible-playbook cws-workstation.yaml --ask-become-pass

# Specific components only
ansible-playbook cws-workstation.yaml --ask-become-pass --tags "go,rust"
ansible-playbook cws-workstation.yaml --ask-become-pass --tags "dotnet,csharp"
ansible-playbook cws-workstation.yaml --ask-become-pass --tags "node,typescript"
ansible-playbook cws-workstation.yaml --ask-become-pass --tags "system,tuning"
```

## Available Tags

| Tag | Installs |
|-----|----------|
| `system` | System packages, monitoring, SSH |
| `c`, `cpp` | GCC, Clang, LLVM, build tools, debug tools |
| `go` | Go runtime + development tools (gopls, delve, golangci-lint, etc.) |
| `rust` | Rust nightly + components (analyzer, clippy, rustfmt) |
| `node`, `typescript` | Node.js (LTS + Current via nvm), npm globals, TS tooling |
| `dotnet`, `csharp` | .NET SDK, ASP.NET, Mono, global tools |
| `proto`, `grpc` | Protobuf compiler, Go/Rust gRPC plugins, buf |
| `cross` | Cross-compilation (ARM64, musl, WASM targets) |
| `wasm` | WASM tooling (wasm-pack, wasm-bindgen) |
| `cuda`, `gpu` | NVIDIA CUDA toolkit |
| `docker` | Docker CE, Compose, BuildX, daemon config |
| `databases` | PostgreSQL, Redis, SQLite |
| `docs`, `asciidoc` | AsciiDoc toolchain, Pandoc, Doxygen |
| `config`, `lint` | jq, yq, shellcheck, yamllint, jsonlint |
| `tuning`, `power` | sysctl, TLP battery/CPU config |
| `gh`, `github` | GitHub CLI from official repo |
| `cpisi`, `structure` | CPI-SI directory structure |
| `services` | Enable PostgreSQL, Redis, SSH, TLP, Docker |
| `python`, `tools` | pipx, ansible, toml-sort, pre-commit |
| `monitoring` | htop, btop, iotop, ncdu, powertop, hyperfine |

## Customizing

Override variables at runtime:

```bash
ansible-playbook cws-workstation.yaml --ask-become-pass \
  -e "go_version=1.26.0" \
  -e "node_lts_version=24" \
  -e "rust_channel=nightly"
```

## Files

| File | Purpose |
|------|---------|
| `cws-workstation.yaml` | Main playbook — full workstation setup |
| `inventory.ini` | Ansible inventory (local + servers) |
| `README.md` | This file |

## What This Does NOT Install

- Claude Code itself (subscription-based, manual install)
- CPI-SI source code (cloned from repo)
- Identity files (bio.md, config.jsonc — instance-specific)
- SSH keys (generated per-machine)
- Git config (user-specific)
- WezTerm config (separate dotfiles)
- Project drive mount (hardware-specific)
