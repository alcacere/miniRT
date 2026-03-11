#!/bin/bash
# ============================================================
# miniRT Scene Viewer — open all scenes for visual comparison
#
# Usage:
#   ./assets/scenes/run_all.sh           open all scenes (3 at a time)
#   ./assets/scenes/run_all.sh --kill    kill all tracked windows
#   ./assets/scenes/run_all.sh -k        same as --kill
#
# Windows stay open in background so you can compare them.
# PIDs are stored in .miniRT_pids at the project root.
# Re-running the script always kills any previous batch first.
# ============================================================

BINARY="./build/bin/miniRT"
SCENE_DIR="assets/scenes"
PID_FILE=".miniRT_pids"
BATCH_SIZE=3          # windows launched in parallel at once
BATCH_DELAY=2         # seconds to wait between batches

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
BOLD='\033[1m'
RESET='\033[0m'

# ── helpers ─────────────────────────────────────────────────

kill_all()
{
    if [ ! -f "$PID_FILE" ]; then
        echo -e "${YELLOW}No tracked windows found (${PID_FILE} missing).${RESET}"
        return 0
    fi
    echo -e "${BOLD}${CYAN}Killing all tracked miniRT windows...${RESET}"
    killed=0
    while IFS= read -r pid; do
        if kill -0 "$pid" 2>/dev/null; then
            kill "$pid" 2>/dev/null && killed=$((killed + 1))
        fi
    done < "$PID_FILE"
    rm -f "$PID_FILE"
    echo -e "${GREEN}Done — killed ${killed} window(s).${RESET}"
}

# ── entry ────────────────────────────────────────────────────

if [ "$1" = "--kill" ] || [ "$1" = "-k" ]; then
    kill_all
    exit 0
fi

# Sanity checks
if [ ! -f "$BINARY" ]; then
    echo -e "${RED}Error: ${BINARY} not found. Run 'make' first.${RESET}"
    exit 1
fi

if [ -z "$DISPLAY" ]; then
    echo -e "${RED}Error: no DISPLAY set — cannot open X11 windows.${RESET}"
    exit 1
fi

# Kill any previous batch before starting a fresh one
if [ -f "$PID_FILE" ]; then
    echo -e "${YELLOW}Previous session detected — cleaning up first...${RESET}"
    kill_all
fi

# Collect all scene files
mapfile -t SCENES < <(find "$SCENE_DIR" -maxdepth 1 -name "*.rt" | sort)

if [ ${#SCENES[@]} -eq 0 ]; then
    echo -e "${RED}No .rt files found in ${SCENE_DIR}/${RESET}"
    exit 1
fi

echo ""
echo -e "${BOLD}${CYAN}╔══════════════════════════════════════════════╗${RESET}"
echo -e "${BOLD}${CYAN}║         miniRT Scene Viewer — run_all        ║${RESET}"
echo -e "${BOLD}${CYAN}╚══════════════════════════════════════════════╝${RESET}"
echo -e "  Scenes found : ${#SCENES[@]}"
echo -e "  Batch size   : ${BATCH_SIZE} (parallel)"
echo -e "  Batch delay  : ${BATCH_DELAY}s between batches"
echo -e "  PID file     : ${PID_FILE}"
echo ""

> "$PID_FILE"   # create/truncate PID file

batch_num=0
launched=0

for (( i=0; i<${#SCENES[@]}; i+=BATCH_SIZE )); do
    batch_num=$((batch_num + 1))
    batch_pids=()

    echo -e "${BOLD}${YELLOW}── Batch ${batch_num} ──────────────────────────────────────${RESET}"

    for (( j=i; j<i+BATCH_SIZE && j<${#SCENES[@]}; j++ )); do
        scene="${SCENES[$j]}"
        name=$(basename "$scene")

        # Launch in background; redirect output to silence it
        "$BINARY" "$scene" > /dev/null 2>&1 &
        pid=$!
        echo "$pid" >> "$PID_FILE"
        batch_pids+=("$pid")
        launched=$((launched + 1))
        echo -e "  ${GREEN}▶ started${RESET}  $name  ${CYAN}(pid $pid)${RESET}"
    done

    # Wait between batches (unless it's the last one)
    if [ $((i + BATCH_SIZE)) -lt ${#SCENES[@]} ]; then
        echo -e "  ${CYAN}Waiting ${BATCH_DELAY}s before next batch...${RESET}"
        sleep "$BATCH_DELAY"
    fi
    echo ""
done

echo -e "${BOLD}${CYAN}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${RESET}"
echo -e "${BOLD}  Launched ${launched}/${#SCENES[@]} scenes.${RESET}"
echo -e "  All windows are running in the background."
echo -e "  To close everything: ${BOLD}./assets/scenes/run_all.sh --kill${RESET}"
echo -e "${BOLD}${CYAN}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${RESET}"
echo ""
