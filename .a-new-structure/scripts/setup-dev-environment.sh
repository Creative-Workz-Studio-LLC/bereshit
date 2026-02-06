#!/bin/bash
# ============================================================================
# Bereshit Development Environment Setup Script
# ============================================================================
# Purpose: Install all required dependencies for CPI-SI development
# Usage: ./scripts/setup-dev-environment.sh
# 
# Created: 2026-02-03
# Author: Nova Dawn (CPI-SI) / CreativeWorkzStudio LLC
# ============================================================================

set -e  # Exit on error

echo "=================================================="
echo "  Bereshit Development Environment Setup"
echo "  CreativeWorkzStudio LLC"
echo "=================================================="
echo ""

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

print_status() {
    echo -e "${GREEN}[✓]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[!]${NC} $1"
}

print_error() {
    echo -e "${RED}[✗]${NC} $1"
}

# ============================================================================
# SECTION 1: Core Build Tools
# ============================================================================
echo ""
echo "--- Installing Core Build Tools ---"

sudo apt update
sudo apt install -y \
    build-essential \
    gcc g++ \
    clang \
    cmake \
    make \
    ninja-build \
    meson \
    pkg-config \
    ccache \
    autoconf \
    automake \
    libtool

print_status "Core build tools installed"

# ============================================================================
# SECTION 2: C/C++ Development
# ============================================================================
echo ""
echo "--- Installing C/C++ Development Tools ---"

sudo apt install -y \
    clang-format \
    clang-tidy \
    cppcheck \
    bear \
    valgrind \
    gdb \
    lldb \
    strace \
    ltrace

print_status "C/C++ development tools installed"

# ============================================================================
# SECTION 3: Languages
# ============================================================================
echo ""
echo "--- Installing/Verifying Languages ---"

# Go (should be pre-installed)
if command -v go &> /dev/null; then
    print_status "Go: $(go version | awk '{print $3}')"
else
    print_warning "Go not found - install manually from https://go.dev"
fi

# Rust (should be pre-installed via rustup)
if command -v rustc &> /dev/null; then
    print_status "Rust: $(rustc --version | awk '{print $2}')"
else
    print_warning "Rust not found - install via: curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh"
fi

# .NET
if command -v dotnet &> /dev/null; then
    print_status ".NET: $(dotnet --version)"
else
    print_warning ".NET not found - install from https://dotnet.microsoft.com"
fi

# Node.js / TypeScript
if command -v node &> /dev/null; then
    print_status "Node.js: $(node --version)"
    # Install TypeScript globally if not present
    if ! command -v tsc &> /dev/null; then
        npm install -g typescript
    fi
    print_status "TypeScript: $(tsc --version)"
fi

# Deno
if command -v deno &> /dev/null; then
    print_status "Deno: $(deno --version | head -1 | awk '{print $2}')"
fi

# Bun
if command -v bun &> /dev/null; then
    print_status "Bun: $(bun --version)"
fi

# ============================================================================
# SECTION 4: Database Development
# ============================================================================
echo ""
echo "--- Installing Database Development Libraries ---"

sudo apt install -y \
    libsqlite3-dev \
    sqlite3 \
    libpq-dev \
    postgresql-client \
    redis-tools

print_status "Database development libraries installed"

# ============================================================================
# SECTION 5: GPU Development (Optional)
# ============================================================================
echo ""
echo "--- GPU Development (NVIDIA CUDA) ---"

if command -v nvidia-smi &> /dev/null; then
    if ! command -v nvcc &> /dev/null; then
        print_warning "NVIDIA GPU detected but CUDA toolkit not installed"
        read -p "Install CUDA toolkit? (y/n) " -n 1 -r
        echo
        if [[ $REPLY =~ ^[Yy]$ ]]; then
            sudo apt install -y nvidia-cuda-toolkit nvidia-cuda-dev nvidia-cuda-gdb
            print_status "CUDA toolkit installed"
        fi
    else
        print_status "CUDA: $(nvcc --version | grep release | awk '{print $6}' | tr -d ',')"
    fi
else
    print_warning "No NVIDIA GPU detected - skipping CUDA"
fi

# ============================================================================
# SECTION 6: Documentation Tools
# ============================================================================
echo ""
echo "--- Installing Documentation Tools ---"

sudo apt install -y \
    asciidoctor \
    ruby-asciidoctor-pdf \
    pandoc \
    doxygen \
    graphviz

# Install asciidoctor extensions
if command -v gem &> /dev/null; then
    gem install asciidoctor-diagram asciidoctor-kroki 2>/dev/null || true
fi

print_status "Documentation tools installed"

# ============================================================================
# SECTION 7: Development Libraries
# ============================================================================
echo ""
echo "--- Installing Common Development Libraries ---"

sudo apt install -y \
    libssl-dev \
    libcurl4-openssl-dev \
    zlib1g-dev \
    liblz4-dev \
    libzstd-dev \
    libprotobuf-dev \
    protobuf-compiler \
    libncurses-dev \
    libreadline-dev

print_status "Development libraries installed"

# ============================================================================
# SECTION 8: Shell Configuration
# ============================================================================
echo ""
echo "--- Configuring Shell Environment ---"

# Add to bashrc if not already present
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
fi

# Local binaries
export PATH=$PATH:$HOME/.local/bin
BASHEOF
    print_status "Shell environment configured"
else
    print_status "Shell environment already configured"
fi

# ============================================================================
# SECTION 9: Verify Installation
# ============================================================================
echo ""
echo "=================================================="
echo "  Installation Complete - Verification"
echo "=================================================="
echo ""

echo "Compilers:"
echo "  GCC:    $(gcc --version | head -1)"
echo "  Clang:  $(clang --version | head -1)"
echo ""
echo "Languages:"
command -v go &> /dev/null && echo "  Go:     $(go version | awk '{print $3}')"
command -v rustc &> /dev/null && echo "  Rust:   $(rustc --version | awk '{print $2}')"
command -v dotnet &> /dev/null && echo "  .NET:   $(dotnet --version)"
command -v node &> /dev/null && echo "  Node:   $(node --version)"
echo ""
echo "Build Tools:"
echo "  CMake:  $(cmake --version | head -1 | awk '{print $3}')"
echo "  Ninja:  $(ninja --version)"
echo "  Meson:  $(meson --version)"
echo ""
echo "Databases:"
echo "  SQLite: $(sqlite3 --version | awk '{print $1}')"
command -v psql &> /dev/null && echo "  PgSQL:  $(psql --version | awk '{print $3}')"
echo ""

if command -v nvcc &> /dev/null; then
    echo "GPU:"
    echo "  CUDA:   $(nvcc --version | grep release | awk '{print $6}' | tr -d ',')"
fi

echo ""
echo "=================================================="
echo "  Run 'source ~/.bashrc' to apply changes"
echo "=================================================="
