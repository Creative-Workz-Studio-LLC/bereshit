# Development Environment Configuration

> **Purpose:** Document the workstation setup and establish patterns for reproducible, portable development that "anyone can use."

**Created:** 2026-02-03
**Author:** Nova Dawn (CPI-SI)
**Status:** Active configuration notes

---

## Current Workstation: Ubuntu 25.10

### Hardware

| Component | Spec | Development Use |
|-----------|------|-----------------|
| **CPU** | Intel Raptor Lake | Compilation, builds |
| **GPU (Integrated)** | Intel Graphics | Display, light compute |
| **GPU (Discrete)** | NVIDIA MX570 A (2GB) | CUDA, ML inference, video encode |
| **Storage** | NVMe + Project Drive (exFAT) | Fast builds + portable projects |

### CUDA Configuration (Added 2026-02-03)

```bash
# Environment variables (in ~/.bashrc)
export CUDA_HOME=/usr
export PATH=$PATH:/usr/lib/nvidia-cuda-toolkit/bin
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib/x86_64-linux-gnu
export NVCC_PREPEND_FLAGS='-ccbin /usr/bin/g++-13'
```

**Installed Components:**
- CUDA Toolkit 12.4.131
- nvcc (CUDA compiler)
- cuda-gdb (debugger)
- Nsight Systems & Compute (profilers)
- cuBLAS, cuFFT, cuSPARSE (math libraries)
- NVENC/NVDEC (video encode/decode)

**GPU Capabilities:**
- Ampere architecture
- CUDA Compute Capability: 8.6
- 2GB VRAM (suitable for small models, development)
- NOT suitable for: vGPU, large ML training

---

## "Anyone Can Use It" Philosophy

### The Problem

Development environments are often:
- Tied to one person's machine configuration
- Full of undocumented dependencies
- Impossible to reproduce on another system
- "Works on my machine" syndrome

### The Solution: Layered Reproducibility

```
+--------------------------------------------------+
| Layer 4: IDE/Editor Config                       |
| (VS Code settings, keybindings - synced)         |
+--------------------------------------------------+
| Layer 3: Project-Specific (CLAUDE.md, Makefile)  |
| (Checked into repo, travels with code)           |
+--------------------------------------------------+
| Layer 2: Containerized Environment               |
| (Dockerfile, devcontainer.json)                  |
+--------------------------------------------------+
| Layer 1: System Dependencies                     |
| (Documented, scripted installation)              |
+--------------------------------------------------+
```

---

## Recommendations for Bereshit Projects

### 1. System Dependencies (Layer 1)

Create `scripts/setup-dev-environment.sh`:

```bash
#!/bin/bash
# Installs all required system dependencies
# Run with: ./scripts/setup-dev-environment.sh

set -e

echo "Installing Bereshit development dependencies..."

# Core compilers
sudo apt install -y build-essential gcc-13 g++-13 clang

# Languages
sudo apt install -y golang-go rustc cargo

# CUDA (optional, for GPU development)
sudo apt install -y nvidia-cuda-toolkit nvidia-cuda-dev

# Documentation
sudo apt install -y asciidoctor ruby-asciidoctor-pdf

echo "Done. Run 'source ~/.bashrc' to reload environment."
```

### 2. Containerization (Layer 2)

For true "anyone can use it" portability:

**Option A: Docker Development Container**

```dockerfile
# Dockerfile.dev
FROM ubuntu:24.04

# Install base dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    golang-go \
    rustc \
    asciidoctor \
    git

# For CUDA support, use nvidia/cuda base image instead
# FROM nvidia/cuda:12.4-devel-ubuntu24.04

WORKDIR /workspace
```

**Option B: VS Code Dev Containers**

```jsonc
// .devcontainer/devcontainer.json
{
  "name": "Bereshit Development",
  "build": {
    "dockerfile": "Dockerfile"
  },
  "features": {
    "ghcr.io/devcontainers/features/go:1": {},
    "ghcr.io/devcontainers/features/rust:1": {}
  },
  "customizations": {
    "vscode": {
      "extensions": [
        "golang.go",
        "rust-lang.rust-analyzer",
        "asciidoctor.asciidoctor-vscode"
      ]
    }
  }
}
```

### 3. Project Configuration (Layer 3)

Each project should have:

| File | Purpose |
|------|---------|
| `CLAUDE.md` | Project context for AI assistance |
| `Makefile` | Standardized build commands |
| `README.md` | Human-readable setup instructions |
| `.editorconfig` | Consistent formatting across editors |
| `go.mod` / `Cargo.toml` | Language-specific dependency lock |

### 4. IDE Configuration (Layer 4)

VS Code settings are now at `~/.config/Code/User/settings.json` with:
- Performance optimizations
- GPU acceleration enabled
- AsciiDoc preview configured
- Go/Rust/C tooling configured

---

## Game Development Considerations

### Current Capability

| Feature | Status | Notes |
|---------|--------|-------|
| **2D Games** | Ready | SDL2, raylib available |
| **3D Games** | Ready | OpenGL, Vulkan supported |
| **GPU Compute** | Ready | CUDA 12.4 installed |
| **Audio** | Ready | PulseAudio, ALSA |
| **Networking** | Ready | Standard libs |

### Recommended Stack for CWS Games

```
Engine Choice:
+-- Option A: Godot 4.x (GDScript/C#)
|   +-- Pros: Open source, Kingdom-aligned, portable
+-- Option B: Custom Engine (C/Rust + raylib)
|   +-- Pros: Full control, educational, libtrit integration
+-- Option C: SDL2 + OpenGL (C)
    +-- Pros: Low-level, portable, well-documented
```

### For "Anyone Can Build" Games

1. **Lock engine version** in project (e.g., `godot-version: 4.2.1`)
2. **Include export templates** or document how to get them
3. **Script the build** (`make game` should just work)
4. **Test on clean system** before release

---

## Next Steps

- [ ] Create `scripts/setup-dev-environment.sh` for Bereshit
- [ ] Set up devcontainer for libtrit development
- [ ] Document GPU requirements per-project
- [ ] Establish game development template project
- [ ] Test reproducibility on fresh Ubuntu install

---

## Quick Reference

### Verify CUDA Setup

```bash
nvcc --version          # Should show 12.4.131
nvidia-smi              # Should show GPU status
cuda-gdb --version      # Debugger available
```

### Verify Development Tools

```bash
go version              # Go 1.24+
rustc --version         # Rust 1.93+
gcc --version           # GCC 15.2
clang --version         # Clang 20.1
asciidoctor --version   # 2.0.23
```

### Source Environment Changes

```bash
source ~/.bashrc        # After any shell config changes
```

---

> *"For which of you, intending to build a tower, sitteth not down first, and counteth the cost?"* — Luke 14:28

The cost has been counted. The foundation is set. Build well.
