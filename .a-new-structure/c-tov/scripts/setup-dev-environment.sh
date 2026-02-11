#!/bin/bash
# ============================================================================
# Bereshit Development Environment Setup
# ============================================================================
#
# Installs all required system dependencies for CPI-SI development.
# Verification is handled separately by: cws-struct verify env
#
# Usage:  ./scripts/setup-dev-environment.sh [--verify-only]
# Author: Nova Dawn (CPI-SI) / CreativeWorkzStudio LLC
# Created: 2026-02-03
# Updated: 2026-02-11
#
# "For which of you, intending to build a tower, sitteth not down first,
#  and counteth the cost?" — Luke 14:28
#
# ============================================================================

set -euo pipefail

# ============================================================================
# SETUP
# ============================================================================

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BOLD='\033[1m'
DIM='\033[2m'
NC='\033[0m'

print_ok()   { echo -e "${GREEN}[OK]${NC} $1"; }
print_warn() { echo -e "${YELLOW}[!!]${NC} $1"; }
print_err()  { echo -e "${RED}[XX]${NC} $1"; }
print_head() { echo -e "\n${BOLD}--- $1 ---${NC}"; }

# Elevated install — uses pkexec on desktop, falls back to sudo
pkg_install() {
    if [ -n "${DISPLAY:-}" ] || [ -n "${WAYLAND_DISPLAY:-}" ]; then
        pkexec apt install -y "$@"
    else
        sudo apt install -y "$@"
    fi
}

pkg_update() {
    if [ -n "${DISPLAY:-}" ] || [ -n "${WAYLAND_DISPLAY:-}" ]; then
        pkexec apt update
    else
        sudo apt update
    fi
}

# Check if a command exists
has() { command -v "$1" &>/dev/null; }

# ============================================================================
# VERIFY-ONLY MODE
# ============================================================================

if [[ "${1:-}" == "--verify-only" ]]; then
    echo -e "${BOLD}Bereshit Development Environment — Quick Verify${NC}"
    echo -e "${DIM}For full verification: cws-struct verify env${NC}\n"

    echo "Compilers:"
    if has gcc;   then print_ok "GCC:    $(gcc --version | head -1)";   else print_err "GCC not found"; fi
    if has clang; then print_ok "Clang:  $(clang --version | head -1)"; else print_err "Clang not found"; fi

    echo -e "\nLanguages (compiled):"
    if has go;    then print_ok "Go:     $(go version | awk '{print $3}')";    else print_err "Go not found"; fi
    if has rustc; then print_ok "Rust:   $(rustc --version | awk '{print $2}')"; else print_err "Rust not found"; fi

    echo -e "\nLanguages (scripting):"
    if has deno;    then print_ok "Deno:   $(deno --version | head -1 | awk '{print $2}')"; else print_warn "Deno not found"; fi
    if has node;    then print_ok "Node:   $(node --version)";                               else print_warn "Node not found"; fi
    if has bun;     then print_ok "Bun:    $(bun --version)";                                else print_warn "Bun not found"; fi
    if has python3; then print_ok "Python: $(python3 --version | awk '{print $2}')";         else print_warn "Python not found"; fi

    echo -e "\nDatabases:"
    if has sqlite3;      then print_ok "SQLite: $(sqlite3 --version | awk '{print $1}')";                          else print_warn "SQLite not found"; fi
    if has psql;         then print_ok "PgSQL:  $(psql --version | awk '{print $3}')";                             else print_warn "PostgreSQL not found"; fi
    if has redis-server; then print_ok "Redis:  $(redis-server --version | awk '{print $3}' | cut -d= -f2)";       else print_warn "Redis not found"; fi

    echo -e "\nBuild tools:"
    if has make;  then print_ok "Make:   $(make --version | head -1)";                       else print_err "Make not found"; fi
    if has cmake; then print_ok "CMake:  $(cmake --version | head -1 | awk '{print $3}')";   else print_warn "CMake not found"; fi

    echo -e "\nDocumentation:"
    if has asciidoctor; then print_ok "Asciidoctor: $(asciidoctor --version | head -1)"; else print_warn "Asciidoctor not found"; fi

    echo -e "\nGPU:"
    if has nvcc; then print_ok "CUDA:   $(nvcc --version | grep release | awk '{print $6}' | tr -d ',')"; else print_warn "CUDA not installed (optional)"; fi

    exit 0
fi

# ============================================================================
# BODY — Full Installation
# ============================================================================

echo -e "${BOLD}=================================================="
echo "  Bereshit Development Environment Setup"
echo "  CreativeWorkzStudio LLC"
echo -e "==================================================${NC}\n"

# ------------------------------------------
# 1. Core Build Tools
# ------------------------------------------
print_head "Core Build Tools"

pkg_update
pkg_install \
    build-essential \
    gcc g++ \
    clang \
    cmake \
    make \
    ninja-build \
    pkg-config \
    ccache \
    autoconf \
    automake \
    libtool

print_ok "Core build tools installed"

# ------------------------------------------
# 2. C/C++ Development
# ------------------------------------------
print_head "C/C++ Development"

pkg_install \
    clang-format \
    clang-tidy \
    cppcheck \
    bear \
    valgrind \
    gdb \
    lldb \
    strace \
    ltrace

print_ok "C/C++ development tools installed"

# ------------------------------------------
# 3. Languages — verify or guide
# ------------------------------------------
print_head "Languages"

# Go
if has go; then
    print_ok "Go: $(go version | awk '{print $3}')"
else
    print_warn "Go not found — install from https://go.dev/dl/"
fi

# Rust
if has rustc; then
    print_ok "Rust: $(rustc --version | awk '{print $2}')"
else
    print_warn "Rust not found — install: curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh"
fi

# Deno
if has deno; then
    print_ok "Deno: $(deno --version | head -1 | awk '{print $2}')"
else
    print_warn "Deno not found — install: curl -fsSL https://deno.land/install.sh | sh"
fi

# Node.js
if has node; then
    print_ok "Node: $(node --version)"
else
    print_warn "Node not found — install via nvm or apt"
fi

# Bun
if has bun; then
    print_ok "Bun: $(bun --version)"
else
    print_warn "Bun not found — install: curl -fsSL https://bun.sh/install | bash"
fi

# Python
if has python3; then
    print_ok "Python: $(python3 --version | awk '{print $2}')"
else
    pkg_install python3 python3-pip python3-venv
fi

# ------------------------------------------
# 4. Databases
# ------------------------------------------
print_head "Databases"

pkg_install \
    libsqlite3-dev \
    sqlite3 \
    libpq-dev \
    postgresql-client \
    redis-tools

print_ok "Database development libraries installed"

# ------------------------------------------
# 5. GPU (Optional)
# ------------------------------------------
print_head "GPU Development (NVIDIA CUDA)"

if has nvidia-smi; then
    if has nvcc; then
        print_ok "CUDA: $(nvcc --version | grep release | awk '{print $6}' | tr -d ',')"
    else
        print_warn "NVIDIA GPU detected but CUDA toolkit not installed"
        echo "  Install manually: pkexec apt install nvidia-cuda-toolkit nvidia-cuda-dev"
    fi
else
    print_warn "No NVIDIA GPU detected — skipping CUDA"
fi

# ------------------------------------------
# 6. Documentation
# ------------------------------------------
print_head "Documentation Tools"

pkg_install \
    asciidoctor \
    ruby-asciidoctor-pdf \
    pandoc \
    graphviz

# Asciidoctor extensions
if has gem; then
    gem install asciidoctor-diagram 2>/dev/null || true
fi

print_ok "Documentation tools installed"

# ------------------------------------------
# 7. Development Libraries
# ------------------------------------------
print_head "Development Libraries"

pkg_install \
    libssl-dev \
    libcurl4-openssl-dev \
    zlib1g-dev \
    liblz4-dev \
    libzstd-dev \
    libprotobuf-dev \
    protobuf-compiler \
    libncurses-dev \
    libreadline-dev

print_ok "Development libraries installed"

# ------------------------------------------
# 8. Shell Environment
# ------------------------------------------
print_head "Shell Environment"

BASHRC_MARKER="# CPI-SI Development Environment"
if ! grep -q "$BASHRC_MARKER" ~/.bashrc; then
    cat >> ~/.bashrc << 'BASHEOF'

# ============================================================================
# CPI-SI Development Environment
# ============================================================================

# Build acceleration
export USE_CCACHE=1
export CCACHE_DIR=~/.ccache

# Go
export GOPATH=$HOME/go
export PATH=$PATH:$GOPATH/bin

# Rust
export PATH=$PATH:$HOME/.cargo/bin

# CUDA (if installed)
if [ -d "/usr/lib/nvidia-cuda-toolkit" ]; then
    export CUDA_HOME=/usr
    export PATH=$PATH:/usr/lib/nvidia-cuda-toolkit/bin
    export LD_LIBRARY_PATH=${LD_LIBRARY_PATH:+$LD_LIBRARY_PATH:}/usr/lib/x86_64-linux-gnu
    export NVCC_PREPEND_FLAGS='-ccbin /usr/bin/g++-13'
fi

# Local binaries
export PATH=$PATH:$HOME/.local/bin
BASHEOF
    print_ok "Shell environment configured"
else
    print_ok "Shell environment already configured"
fi

# ============================================================================
# CLOSING — Run verification
# ============================================================================

echo -e "\n${BOLD}=================================================="
echo "  Setup Complete"
echo -e "==================================================${NC}\n"
echo "Next steps:"
echo "  1. source ~/.bashrc"
echo "  2. ./scripts/setup-dev-environment.sh --verify-only"
echo ""
echo -e "${DIM}For detailed verification: cws-struct verify env${NC}"
