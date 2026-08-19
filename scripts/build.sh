#!/usr/bin/env bash
# =====================================================
# Pixel Gun 3D Trainer — Build Script (Linux / macOS)
# Cross-compiles trainer.dll & PG3D_Injector.exe
# using MinGW-w64 + vcpkg
# =====================================================

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
BUILD_DIR="$PROJECT_ROOT/build"
BIN_DIR="$BUILD_DIR/bin"
VCPKG_DIR="$PROJECT_ROOT/vcpkg"
TRIPLET="x64-mingw-static"

# --- Colors ---
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
PURPLE='\033[0;35m'
NC='\033[0m'

banner() {
    echo -e "${PURPLE}"
    echo "  ╔═══════════════════════════════════════════╗"
    echo "  ║       PIXEL GUN 3D PC - Builder           ║"
    echo "  ║       Build Script ($(uname -s))          ║"
    echo "  ╚═══════════════════════════════════════════╝"
    echo -e "${NC}"
}

info()    { echo -e "  ${CYAN}[*]${NC} $*"; }
success() { echo -e "  ${GREEN}[+]${NC} $*"; }
warn()    { echo -e "  ${YELLOW}[!]${NC} $*"; }
error()   { echo -e "  ${RED}[!]${NC} $*"; }

# --- Dependency checks ---
check_deps() {
    local missing=()

    for cmd in cmake make python3 x86_64-w64-mingw32-g++; do
        if ! command -v "$cmd" &>/dev/null; then
            missing+=("$cmd")
        fi
    done

    if [ ${#missing[@]} -ne 0 ]; then
        error "Missing dependencies: ${missing[*]}"
        echo ""
        if [[ "$(uname -s)" == "Darwin" ]]; then
            warn "Install on macOS:"
            echo "    brew install cmake mingw-w64 python3"
        else
            warn "Install on Linux:"
            echo "    sudo pacman -S cmake make mingw-w64-gcc python"
        fi
        echo ""
        exit 1
    fi

    success "Dependencies OK"
}

# --- vcpkg bootstrap ---
setup_vcpkg() {
    if [ ! -d "$VCPKG_DIR" ]; then
        info "vcpkg submodule not found. Initializing..."
        git -C "$PROJECT_ROOT" submodule update --init --recursive
    fi

    if [ ! -f "$VCPKG_DIR/vcpkg" ]; then
        info "Bootstrapping vcpkg..."
        "$VCPKG_DIR/bootstrap-vcpkg.sh" -disableMetrics
    fi

    success "vcpkg ready"
}

# --- Build ---
build_project() {
    local build_type="${1:-Release}"

    info "Configuring CMake (triplet: ${TRIPLET}, type: ${build_type})..."

    cmake -S "$PROJECT_ROOT" -B "$BUILD_DIR" \
        -DCMAKE_TOOLCHAIN_FILE="$VCPKG_DIR/scripts/buildsystems/vcpkg.cmake" \
        -DVCPKG_TARGET_TRIPLET="$TRIPLET" \
        -DCMAKE_BUILD_TYPE="$build_type" \
        -DCMAKE_SYSTEM_NAME=Windows \
        -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc \
        -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++ \
        -DCMAKE_RC_COMPILER=x86_64-w64-mingw32-windres \
        -G "Unix Makefiles" \
        2>&1 | sed 's/^/    /'

    local nproc_cmd="nproc"
    if [[ "$(uname -s)" == "Darwin" ]]; then
        nproc_cmd="sysctl -n hw.ncpu"
    fi
    local jobs
    jobs=$($nproc_cmd 2>/dev/null || echo 4)

    info "Building with ${jobs} jobs..."
    cmake --build "$BUILD_DIR" -j"$jobs" 2>&1 | sed 's/^/    /'

    if [ ! -f "$BIN_DIR/trainer.dll" ] && [ ! -f "$BIN_DIR/libtrainer.dll" ]; then
        error "Build failed — trainer.dll not produced."
        exit 1
    fi

    # MinGW may produce libtrainer.dll instead of trainer.dll
    if [ -f "$BIN_DIR/libtrainer.dll" ] && [ ! -f "$BIN_DIR/trainer.dll" ]; then
        cp "$BIN_DIR/libtrainer.dll" "$BIN_DIR/trainer.dll"
    fi

    echo ""
    success "Build complete!"
    success "  trainer.dll      → $BIN_DIR/trainer.dll"
    success "  PG3D_Injector    → $BIN_DIR/PG3D_Injector.exe"
    echo ""
    info "To launch, run: ./scripts/launch.sh"
}

# --- Clean ---
clean_build() {
    if [ -d "$BUILD_DIR" ]; then
        info "Removing build directory..."
        rm -rf "$BUILD_DIR"
        success "Clean complete"
    else
        info "Nothing to clean"
    fi
}

# --- Main ---
main() {
    banner
    check_deps

    case "${1:-release}" in
        release)
            setup_vcpkg
            build_project Release
            ;;
        debug)
            setup_vcpkg
            build_project Debug
            ;;
        clean)
            clean_build
            ;;
        rebuild)
            clean_build
            setup_vcpkg
            build_project Release
            ;;
        help|-h|--help)
            echo "  Usage: $0 [command]"
            echo ""
            echo "  Commands:"
            echo "    release    — Build in Release mode (default)"
            echo "    debug      — Build in Debug mode"
            echo "    clean      — Remove build directory"
            echo "    rebuild    — Clean + build Release"
            echo "    help       — Show this help message"
            echo ""
            ;;
        *)
            error "Unknown command: $1"
            echo "  Run '$0 help' for usage."
            exit 1
            ;;
    esac
}

main "$@"
