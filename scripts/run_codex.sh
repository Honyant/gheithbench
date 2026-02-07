#!/usr/bin/env bash
#
# run_codex.sh — Run Codex on a single project.
# Usage: ./run_codex.sh <project_number>
#
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT_DIR="$(dirname "$SCRIPT_DIR")"
PROJECTS_DIR="$ROOT_DIR/projects"
RESULTS_DIR="$ROOT_DIR/results"

PLATFORM="${GHEITHBENCH_PLATFORM:---platform linux/amd64}"
CODEX_MODEL="${CODEX_MODEL:-}"
CODEX_REASONING="${CODEX_REASONING:-medium}"

P="$1"
PROJ_DIR="$PROJECTS_DIR/p$P"

get_docker_image() {
  case "$1" in
    1|2|3|5|6) echo "gheithbench-base:latest" ;;
    4) echo "gheithbench-p4:latest" ;;
    7|8) echo "gheithbench-verilog:latest" ;;
  esac
}

get_test_layout() {
  case "$1" in
    5) echo "flat" ;;
    *) echo "tests_dir" ;;
  esac
}

IMAGE=$(get_docker_image "$P")
LAYOUT=$(get_test_layout "$P")
WORKSPACE=$(mktemp -d)
RUN_ID=$(date +%Y%m%d_%H%M%S)

echo "=== P$P: Running Codex ==="
echo "  Image: $IMAGE"
echo "  Layout: $LAYOUT"
echo "  Workspace: $WORKSPACE"
echo "  Run ID: $RUN_ID"

mkdir -p "$RESULTS_DIR/p$P"

# 1. Prepare fresh workspace (starter + tests)
cp -r "$PROJ_DIR/starter/." "$WORKSPACE/"

if [ "$LAYOUT" = "flat" ]; then
  cp "$PROJ_DIR/tests/"*.c "$WORKSPACE/" 2>/dev/null || true
  cp "$PROJ_DIR/tests/"*.ok "$WORKSPACE/" 2>/dev/null || true
else
  mkdir -p "$WORKSPACE/tests"
  cp -r "$PROJ_DIR/tests/." "$WORKSPACE/tests/"
fi

# Init a git repo so codex doesn't complain
git -C "$WORKSPACE" init -q
git -C "$WORKSPACE" add -A
git -C "$WORKSPACE" commit -q -m "starter" --allow-empty

# 2. Run Codex
CODEX_ARGS=(exec --full-auto -c "model_reasoning_effort=\"$CODEX_REASONING\"")
if [ -n "$CODEX_MODEL" ]; then
  CODEX_ARGS+=(-m "$CODEX_MODEL")
fi
echo "  Running Codex (reasoning=$CODEX_REASONING model=${CODEX_MODEL:-default})..."
PROMPT=$(cat "$PROJ_DIR/prompt.txt")
CODEX_EXIT=0
(cd "$WORKSPACE" && codex "${CODEX_ARGS[@]}" "$PROMPT" 2>&1 | grep -v "state db missing rollout path") || CODEX_EXIT=$?
echo "  Codex exited with code $CODEX_EXIT"

# 3. Save codex's source files to results dir (for inspection)
CODEX_SRC_DIR="$RESULTS_DIR/p$P/codex_src_${RUN_ID}"
mkdir -p "$CODEX_SRC_DIR"
# Copy all non-test, non-.git source files
rsync -a --exclude='.git' --exclude='tests/' --exclude='build/' \
  --exclude='*.o' --exclude='*.d' --exclude='*.out' --exclude='*.diff' \
  --exclude='*.result' --exclude='*.time' --exclude='*.err' --exclude='*.run' \
  --exclude='*.ok' \
  "$WORKSPACE/" "$CODEX_SRC_DIR/"
echo "  Codex source saved to: $CODEX_SRC_DIR"

# 4. Test in Docker
echo "  Running tests in Docker..."

docker run --rm $PLATFORM \
  -v "$WORKSPACE:/workspace" -w /workspace \
  "$IMAGE" \
  bash -c 'make clean 2>/dev/null; make -s test 2>&1' \
  | tee "$RESULTS_DIR/p$P/codex_output_${RUN_ID}.log" || true

# 5. Score
bash "$SCRIPT_DIR/score.sh" "$P" "$WORKSPACE" \
  > "$RESULTS_DIR/p$P/codex_score_${RUN_ID}.json"

echo ""
echo "  Score:"
cat "$RESULTS_DIR/p$P/codex_score_${RUN_ID}.json"
echo ""

# Save workspace for debugging
echo "  Workspace preserved at: $WORKSPACE"
