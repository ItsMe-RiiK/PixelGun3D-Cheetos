#!/usr/bin/env bash
# =====================================================
# Pixel Gun 3D Trainer — Launch Script (Linux / macOS)
# Locates the game process and injects trainer.dll
# =====================================================

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Determine if running from source tree (scripts/) or release package (root)
if [ -f "$SCRIPT_DIR/PG3D_Injector.exe" ]; then
    # Running from release package (script is in same dir as binaries)
    BIN_DIR="$SCRIPT_DIR"
else
    # Running from source tree
    PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
    BIN_DIR="$PROJECT_ROOT/build/bin"
fi

# --- Colors ---
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
PURPLE='\033[0;35m'
NC='\033[0m'

banner() {
    echo -e "${PURPLE}"
    echo "  ╔══════════════════════════════════════════════╗"
    echo "  ║       PIXEL GUN 3D PC TRAINER                ║"
    echo "  ║       Launcher ($(uname -s))                       ║"
    echo "  ╚══════════════════════════════════════════════╝"
    echo -e "${NC}"
}

info()    { echo -e "  ${CYAN}[*]${NC} $*"; }
success() { echo -e "  ${GREEN}[+]${NC} $*"; }
warn()    { echo -e "  ${YELLOW}[!]${NC} $*"; }
error()   { echo -e "  ${RED}[!]${NC} $*"; }

# --- Locate Steam game directory ---
find_game_dir() {
    local steam_paths=(
        # Linux Standard
        "$HOME/.local/share/Steam/steamapps/common/Pixel Gun 3D"
        "$HOME/.steam/steam/steamapps/common/Pixel Gun 3D"
        # Linux Flatpak
        "$HOME/.var/app/com.valvesoftware.Steam/.local/share/Steam/steamapps/common/Pixel Gun 3D"
        # macOS Standard
        "$HOME/Library/Application Support/Steam/steamapps/common/Pixel Gun 3D"
    )

    for p in "${steam_paths[@]}"; do
        if [ -d "$p" ]; then
            echo "$p"
            return 0
        fi
    done

    return 1
}

check_build() {
    if [ ! -f "$BIN_DIR/trainer.dll" ] && [ -f "$BIN_DIR/libtrainer.dll" ]; then
        info "Found libtrainer.dll, copying to trainer.dll for injector compatibility..."
        cp "$BIN_DIR/libtrainer.dll" "$BIN_DIR/trainer.dll"
    fi

    if [ ! -f "$BIN_DIR/trainer.dll" ]; then
        error "trainer.dll not found."
        if [[ "$BIN_DIR" == *"build/bin"* ]]; then
            echo "    Run build first: ./scripts/build.sh"
        fi
        exit 1
    fi

    if [ ! -f "$BIN_DIR/PG3D_Injector.exe" ]; then
        error "PG3D_Injector.exe not found."
        if [[ "$BIN_DIR" == *"build/bin"* ]]; then
            echo "    Run build first: ./scripts/build.sh"
        fi
        exit 1
    fi

    success "Binaries found"
}

# --- Launch via Wine / Protontricks ---
launch_wine() {
    cd "$BIN_DIR"
    local APP_ID="2524890"
    local OS_TYPE="$(uname -s)"

    if [[ "$OS_TYPE" == "Darwin" ]]; then
        # macOS handling
        warn "macOS detected. Protontricks is not used here."
        info "If you are using CrossOver or Whisky, you must run PG3D_Injector.exe manually"
        info "inside the exact same bottle/prefix where Pixel Gun 3D is running."
        echo ""
        info "Injector path: $BIN_DIR/PG3D_Injector.exe"
        exit 1
    fi

    # Check protontricks (Native or Flatpak)
    local PROTON_CMD=""
    if command -v protontricks &>/dev/null; then
        PROTON_CMD="protontricks"
    elif command -v flatpak &>/dev/null && flatpak list | grep -q com.github.Matoking.protontricks; then
        PROTON_CMD="flatpak run com.github.Matoking.protontricks"
    else
        error "protontricks is not installed."
        warn "It is required to inject into a Steam (Proton) game process."
        echo "    Arch Linux: sudo pacman -S protontricks"
        echo "    Flatpak:    flatpak install flathub com.github.Matoking.protontricks"
        exit 1
    fi

    echo ""
    warn "Make sure Pixel Gun 3D is running via Steam (Proton)"
    warn "before the injector starts scanning for the process."
    echo ""

    info "Launching PG3D_Injector.exe via Protontricks..."
    
    # Execute with Protontricks
    $PROTON_CMD -c "wine ./PG3D_Injector.exe" "$APP_ID"
}

# --- Show game location ---
locate_game() {
    if game_dir=$(find_game_dir); then
        success "Game found at: $game_dir"
    else
        warn "Game not found in default Steam paths."
        echo ""
        info "Searched in:"
        echo "    ~/.local/share/Steam/steamapps/common/Pixel Gun 3D"
        echo "    ~/.steam/steam/steamapps/common/Pixel Gun 3D"
        echo "    ~/.var/app/com.valvesoftware.Steam/..."
    fi
}

# --- Show status ---
show_status() {
    info "Binaries:"
    if [ -f "$BIN_DIR/trainer.dll" ] || [ -f "$BIN_DIR/libtrainer.dll" ]; then
        success "  trainer.dll      ✓"
    else
        error "  trainer.dll      ✗ (Missing)"
    fi

    if [ -f "$BIN_DIR/PG3D_Injector.exe" ]; then
        success "  PG3D_Injector    ✓"
    else
        error "  PG3D_Injector    ✗ (Missing)"
    fi

    echo ""
    info "Wine:"
    if command -v wine &>/dev/null; then
        success "  wine installed   ✓  ($(wine --version 2>/dev/null || echo 'unknown version'))"
    else
        warn "  wine installed   ✗"
    fi

    echo ""
    info "Game location:"
    locate_game
}

# --- Main ---
main() {
    banner

    case "${1:-inject}" in
        inject)
            check_build
            launch_wine
            ;;
        locate)
            locate_game
            ;;
        status)
            show_status
            ;;
        help|-h|--help)
            echo "  Usage: $0 [command]"
            echo ""
            echo "  Commands:"
            echo "    inject     — Launch injector via Wine (default)"
            echo "    locate     — Find Steam game installation"
            echo "    status     — Check build artifacts and dependencies"
            echo "    help       — Show this help message"
            echo ""
            echo "  Prerequisites:"
            echo "    1. Ensure trainer.dll and PG3D_Injector.exe exist"
            echo "    2. Start Pixel Gun 3D via Steam"
            echo "    3. Run this script to inject"
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
