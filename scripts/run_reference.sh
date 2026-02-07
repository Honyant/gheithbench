#!/usr/bin/env bash
#
# run_reference.sh — Validate reference solutions pass all tests in Docker.
# Usage: ./run_reference.sh [project_number]
#   If no project number given, runs all projects.
#
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT_DIR="$(dirname "$SCRIPT_DIR")"
PROJECTS_DIR="$ROOT_DIR/projects"
RESULTS_DIR="$ROOT_DIR/results"
PLATFORM="${GHEITHBENCH_PLATFORM:---platform linux/amd64}"

get_docker_image() {
  case "$1" in
    1|2|3|5|6) echo "gheithbench-base:latest" ;;
    4) echo "gheithbench-p4:latest" ;;
    7|8|9) echo "gheithbench-verilog:latest" ;;
  esac
}

get_test_layout() {
  case "$1" in
    5) echo "flat" ;;
    *) echo "tests_dir" ;;
  esac
}

run_one() {
  local P="$1"
  local PROJ_DIR="$PROJECTS_DIR/p$P"
  local IMAGE=$(get_docker_image "$P")
  local LAYOUT=$(get_test_layout "$P")
  local WORKSPACE=$(mktemp -d)

  echo "=== P$P: Testing reference solution ==="
  echo "  Image: $IMAGE"
  echo "  Workspace: $WORKSPACE"

  # Copy reference solution to workspace
  cp -r "$PROJ_DIR/reference/." "$WORKSPACE/"

  # Copy tests to appropriate location
  if [ "$LAYOUT" = "flat" ]; then
    # P5: tests go flat alongside source in workspace root
    cp "$PROJ_DIR/tests/"*.c "$WORKSPACE/" 2>/dev/null || true
    cp "$PROJ_DIR/tests/"*.ok "$WORKSPACE/" 2>/dev/null || true
  else
    # All others: tests go in ./tests/ subdirectory
    mkdir -p "$WORKSPACE/tests"
    cp -r "$PROJ_DIR/tests/." "$WORKSPACE/tests/"
  fi

  # Create results directory
  mkdir -p "$RESULTS_DIR/p$P"

  # Run in Docker
  echo "  Running make clean && make -s test ..."
  docker run --rm $PLATFORM \
    -v "$WORKSPACE:/workspace" -w /workspace \
    "$IMAGE" \
    bash -c 'make clean 2>/dev/null; make -s test 2>&1' \
    | tee "$RESULTS_DIR/p$P/reference_output.log" || true

  # Score
  if [ "$P" = "8" ] || [ "$P" = "9" ]; then
    python3 "$SCRIPT_DIR/score_p8.py" "$WORKSPACE" > "$RESULTS_DIR/p$P/reference_score.json"
  else
    bash "$SCRIPT_DIR/score.sh" "$P" "$WORKSPACE" > "$RESULTS_DIR/p$P/reference_score.json"
  fi
  echo ""
  echo "  Score:"
  cat "$RESULTS_DIR/p$P/reference_score.json"
  echo ""

  # Cleanup
  rm -rf "$WORKSPACE"
}

# Main
if [ $# -ge 1 ]; then
  run_one "$1"
else
  for p in 1 2 3 4 5 6 7 8 9; do
    run_one "$p"
    echo ""
  done

  echo "=== All Reference Results ==="
  for p in 1 2 3 4 5 6 7 8 9; do
    if [ -f "$RESULTS_DIR/p$p/reference_score.json" ]; then
      echo -n "  p$p: "
      cat "$RESULTS_DIR/p$p/reference_score.json"
    fi
  done
fi
