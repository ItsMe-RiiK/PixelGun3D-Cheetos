#!/usr/bin/env bash
# =====================================================
# Pixel Gun 3D Trainer — Offset Auto-Updater
# Runs validate_offsets.py to update offsets.h from dump.cs
# =====================================================

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
TOOLS_DIR="$PROJECT_ROOT/resources/tools"
DUMPED_DIR="$PROJECT_ROOT/resources/dumped/static"
VALIDATOR="$TOOLS_DIR/validate_offsets.py"
LOG_DIR="$PROJECT_ROOT/resources/logs"
TIMESTAMP="$(date +%Y%m%d_%H%M%S)"
LOG_FILE="$LOG_DIR/validate.log"

# --- Colors ---
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
PURPLE='\033[0;35m'
NC='\033[0m'

banner() {
    echo -e "${PURPLE}"
    echo "  ╔═════════════════════════════════════════════╗"
    echo "  ║       PIXEL GUN 3D — Offset Updater         ║"
    echo "  ╚═════════════════════════════════════════════╝"
    echo -e "${NC}"
}

info()    { echo -e "  ${CYAN}[*]${NC} $*"; }
success() { echo -e "  ${GREEN}[+]${NC} $*"; }
warn()    { echo -e "  ${YELLOW}[!]${NC} $*"; }
error()   { echo -e "  ${RED}[!]${NC} $*"; }

# --- Pre-flight checks ---
preflight() {
    # Check Python
    if ! command -v python3 &>/dev/null; then
        error "Python 3 is required but not found."
        echo ""
        if [[ "$(uname)" == "Darwin" ]]; then
            echo "    brew install python3"
        else
            echo "    sudo pacman -S python   # Arch Linux"
        fi
        exit 1
    fi

    # Check validator script
    if [ ! -f "$VALIDATOR" ]; then
        error "Validator script not found at: $VALIDATOR"
        exit 1
    fi

    # Check dump.cs
    if [ ! -f "$DUMPED_DIR/dump.cs" ]; then
        error "dump.cs not found at: $DUMPED_DIR/dump.cs"
        echo ""
        warn "Please place your IL2CPP dump files in:"
        echo "    $DUMPED_DIR/"
        echo ""
        echo "  Required files:"
        echo "    dump.cs          — IL2CPP class dump"
        echo "    il2cpp.h         — IL2CPP header (optional)"
        echo "    script.json      — IL2CPP script dump (optional)"
        echo ""
        exit 1
    fi

    success "Pre-flight checks passed"
}

# --- Validate ---
validate() {
    info "Running offset auto-updater..."
    echo ""

    # Create log directory
    mkdir -p "$LOG_DIR"

    # Run updater and tee to log
    python3 "$VALIDATOR" 2>&1 | tee "$LOG_FILE"

    echo ""

    # Check log for results
    if grep -q "\[UPDATED\]" "$LOG_FILE"; then
        success "Offsets were successfully updated! Review the output above."
    fi

    if grep -q "\[WARNING\]" "$LOG_FILE"; then
        warn "Some offsets were not found. Review the output above."
        echo ""
        warn "You may need to check the obfuscated field names in:"
        echo "    $PROJECT_ROOT/src/utils/offsets.h"
        echo ""
    fi

    if ! grep -q "\[UPDATED\]" "$LOG_FILE" && ! grep -q "\[WARNING\]" "$LOG_FILE"; then
        success "All offsets are already up-to-date!"
    fi

    success "Log saved to: $LOG_FILE"
}

# --- Update dump files ---
update_dumps() {
    local new_dump_dir="${1:-}"

    if [ -z "$new_dump_dir" ]; then
        echo "  Usage: $0 update <path-to-new-dumps>"
        echo ""
        echo "  The directory should contain at least dump.cs"
        echo "  Optionally: il2cpp.h, script.json, stringliteral.json"
        echo ""
        exit 1
    fi

    if [ ! -d "$new_dump_dir" ]; then
        error "Directory not found: $new_dump_dir"
        exit 1
    fi

    if [ ! -f "$new_dump_dir/dump.cs" ]; then
        error "dump.cs not found in: $new_dump_dir"
        exit 1
    fi

    # Backup current dumps
    local backup_dir="$DUMPED_DIR/backup_${TIMESTAMP}"
    info "Backing up current dumps to: $backup_dir"
    mkdir -p "$backup_dir"

    for f in dump.cs il2cpp.h script.json stringliteral.json; do
        if [ -f "$DUMPED_DIR/$f" ]; then
            cp "$DUMPED_DIR/$f" "$backup_dir/$f"
        fi
    done
    success "Backup complete"

    # Copy new dumps
    info "Copying new dump files..."
    for f in dump.cs il2cpp.h script.json stringliteral.json; do
        if [ -f "$new_dump_dir/$f" ]; then
            cp "$new_dump_dir/$f" "$DUMPED_DIR/$f"
            success "  Copied: $f"
        fi
    done

    echo ""
    success "Dump files updated!"
    echo ""
    info "Running auto-updater against new dumps..."
    echo ""

    validate
}

# --- Main ---
main() {
    banner

    case "${1:-}" in
        validate|"")
            preflight
            validate
            ;;
        update)
            preflight
            update_dumps "${2:-}"
            ;;
        help)
            echo "  Usage: $0 <command> [args]"
            echo ""
            echo "  Commands:"
            echo "    validate         — Run auto-updater against dump.cs"
            echo "    update <path>    — Update dump files from a directory and run auto-updater"
            echo "    help             — Show this help message"
            echo ""
            echo "  Examples:"
            echo "    $0 validate"
            echo "    $0 update ~/Downloads/new_dumps/"
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
