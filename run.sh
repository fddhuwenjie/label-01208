#!/bin/bash
set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

log_info() { echo -e "${GREEN}[INFO]${NC} $1"; }
log_warn() { echo -e "${YELLOW}[WARN]${NC} $1"; }
log_error() { echo -e "${RED}[ERROR]${NC} $1"; }

# Detect OS
detect_os() {
    case "$(uname -s)" in
        Darwin*) echo "macos" ;;
        Linux*)  echo "linux" ;;
        MINGW*|MSYS*|CYGWIN*) echo "windows" ;;
        *) echo "unknown" ;;
    esac
}

OS=$(detect_os)
log_info "Detected OS: $OS"

# Check command exists
has_cmd() { command -v "$1" &>/dev/null; }

# Install dependencies based on OS
install_deps() {
    log_info "Checking dependencies..."
    
    case "$OS" in
        macos)
            if ! has_cmd brew; then
                log_warn "Homebrew not found. Installing..."
                /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
            fi
            
            if ! has_cmd cmake; then
                log_info "Installing cmake..."
                brew install cmake
            fi
            
            if ! pkg-config --exists lua5.4 2>/dev/null && ! pkg-config --exists lua 2>/dev/null; then
                log_info "Installing lua..."
                brew install lua pkg-config
            fi
            ;;
            
        linux)
            if has_cmd apt-get; then
                log_info "Using apt-get..."
                sudo apt-get update
                sudo apt-get install -y build-essential cmake pkg-config liblua5.4-dev
            elif has_cmd yum; then
                log_info "Using yum..."
                sudo yum install -y gcc-c++ cmake pkgconfig lua-devel
            elif has_cmd pacman; then
                log_info "Using pacman..."
                sudo pacman -Sy --noconfirm base-devel cmake lua pkgconf
            else
                log_error "Unsupported package manager"
                exit 1
            fi
            ;;
            
        windows)
            if ! has_cmd choco; then
                log_error "Please install Chocolatey first: https://chocolatey.org/install"
                log_info "Or use WSL/MSYS2 to run this script"
                exit 1
            fi
            
            if ! has_cmd cmake; then
                log_info "Installing cmake..."
                choco install cmake -y
            fi
            
            if ! has_cmd lua; then
                log_info "Installing lua..."
                choco install lua -y
            fi
            ;;
            
        *)
            log_error "Unsupported OS"
            exit 1
            ;;
    esac
    
    log_info "Dependencies ready."
}

# Build project
build_project() {
    log_info "Building project..."
    
    cd backend
    
    if [ -d "build" ]; then
        rm -rf build
    fi
    
    mkdir build && cd build
    
    cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF ..
    make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 2)
    
    cd ../..
    log_info "Build complete."
}

# Run engine
run_engine() {
    log_info "Starting Lua Script Engine..."
    echo ""
    
    cd backend/build
    ./lua_engine ../scripts "$@"
}

# Main
main() {
    install_deps
    build_project
    run_engine "$@"
}

main "$@"
