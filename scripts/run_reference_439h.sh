#!/usr/bin/env bash
#
# run_reference_439h.sh — Validate reference solutions pass all tests in Docker.
# Usage: ./run_reference_439h.sh [project_number]
#   If no project number given, runs all projects.
#
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT_DIR="$(dirname "$SCRIPT_DIR")"
PROJECTS_DIR="$ROOT_DIR/projects_439h"
RESULTS_DIR="$ROOT_DIR/results_439h"
PLATFORM="${GHEITHBENCH_PLATFORM:---platform linux/amd64}"
IMAGE="gheithbench-os:latest"

run_one() {
  local P="$1"
  local PROJ_DIR="$PROJECTS_DIR/p$P"
  local WORKSPACE
  WORKSPACE=$(mktemp -d)

  echo "=== 439H P$P: Testing reference solution ==="
  echo "  Workspace: $WORKSPACE"

  # Copy reference solution to workspace
  cp -r "$PROJ_DIR/reference/." "$WORKSPACE/"

  # Copy tests
  mkdir -p "$WORKSPACE/tests"
  cp -r "$PROJ_DIR/tests/." "$WORKSPACE/tests/"

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
  bash "$SCRIPT_DIR/score.sh" "$P" "$WORKSPACE" > "$RESULTS_DIR/p$P/reference_score.json"
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
  for p in 1 2 3 4 5 6 7; do
    run_one "$p"
    echo ""
  done

  echo "=== All Reference Results ==="
  for p in 1 2 3 4 5 6 7; do
    if [ -f "$RESULTS_DIR/p$p/reference_score.json" ]; then
      echo -n "  p$p: "
      cat "$RESULTS_DIR/p$p/reference_score.json"
    fi
  done
fi
