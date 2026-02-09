#!/usr/bin/env bash
#
# run_sequential_439h.sh — Run an LLM agent sequentially on all 439H projects.
#
# The agent builds P1, then P2 on top of its own P1 output, etc.
# Between projects, new stub files and Makefiles are overlaid.
#
# Usage: ./run_sequential_439h.sh
#
# Environment variables:
#   AGENT           — Agent to use: codex (default), claude, gemini
#   AGENT_MODEL     — Model override (agent-specific)
#   AGENT_REASONING — Reasoning effort for codex (default: xhigh)
#   GHEITHBENCH_PLATFORM — Docker platform flag (default: --platform linux/amd64)
#
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT_DIR="$(dirname "$SCRIPT_DIR")"
REPOS_DIR="$ROOT_DIR/439h"
PROJECTS_DIR="$ROOT_DIR/projects_439h"
RESULTS_DIR="$ROOT_DIR/results_439h"

PLATFORM="${GHEITHBENCH_PLATFORM:---platform linux/amd64}"
AGENT="${AGENT:-codex}"
AGENT_MODEL="${AGENT_MODEL:-}"
AGENT_REASONING="${AGENT_REASONING:-xhigh}"
IMAGE="gheithbench-os:latest"

RUN_ID=$(date +%Y%m%d_%H%M%S)
SEQ_DIR="$RESULTS_DIR/sequential_${RUN_ID}"

get_starter_commit() {
  case "$1" in
    1) echo "035b431" ;; 2) echo "ac0044c" ;; 3) echo "41d140d" ;; 4) echo "3421a35" ;;
    5) echo "5e3c75a" ;; 6) echo "427ec7d" ;; 7) echo "561dd60" ;;
  esac
}

run_agent_cmd() {
  local workspace="$1" prompt="$2"
  case "$AGENT" in
    codex)
      local codex_args=(exec --dangerously-bypass-approvals-and-sandbox -c "model_reasoning_effort=\"$AGENT_REASONING\"")
      if [ -n "$AGENT_MODEL" ]; then
        codex_args+=(-m "$AGENT_MODEL")
      fi
      (cd "$workspace" && codex "${codex_args[@]}" "$prompt" 2>&1 | grep -v "state db missing rollout path") ;;
    claude)
      (cd "$workspace" && echo "$prompt" | claude -p --dangerously-skip-permissions) ;;
    gemini)
      local gemini_args=(-p "$prompt" --approval-mode yolo)
      if [ -n "$AGENT_MODEL" ]; then
        gemini_args+=(-m "$AGENT_MODEL")
      fi
      (cd "$workspace" && gemini "${gemini_args[@]}" 2>&1 | grep -v "^\[WARN\]" | grep -v "^Warning:") ;;
    *)
      echo "Unknown agent: $AGENT" >&2; return 1 ;;
  esac
}

# Overlay new stub files for project Pn onto existing workspace
# Only adds NEW files (ones that don't exist in the workspace).
# Also updates Makefiles.
overlay_stubs() {
  local P="$1"
  local WORKSPACE="$2"
  local REPO="$REPOS_DIR/cs439t_f24_p${P}_anthonyw"
  local STARTER_COMMIT
  STARTER_COMMIT=$(get_starter_commit "$P")
  local TMP
  TMP=$(mktemp -d)

  # Extract Pn starter commit to temp dir
  git -C "$REPO" archive "$STARTER_COMMIT" | tar -x -C "$TMP"

  # Add files that are NEW (don't exist in current workspace)
  (
    cd "$TMP"
    find . -type f | while read -r f; do
      f="${f#./}"
      if [ ! -f "$WORKSPACE/$f" ]; then
        echo "    + $f (new stub)"
        mkdir -p "$WORKSPACE/$(dirname "$f")"
        cp "$TMP/$f" "$WORKSPACE/$f"
      fi
    done
  )

  # Remove files that Pn drops (exist in workspace but not in Pn starter)
  # Only remove kernel/ files — don't touch agent-created files outside kernel/
  (
    cd "$WORKSPACE"
    find kernel/ -type f 2>/dev/null | while read -r f; do
      if [ ! -f "$TMP/$f" ]; then
        echo "    - $f (removed in P$P)"
        rm -f "$WORKSPACE/$f"
      fi
    done
  )

  # Update Makefiles from Pn reference (they have correct build rules)
  cp "$PROJECTS_DIR/p$P/reference/Makefile" "$WORKSPACE/Makefile"
  if [ -f "$PROJECTS_DIR/p$P/reference/kernel/Makefile" ]; then
    cp "$PROJECTS_DIR/p$P/reference/kernel/Makefile" "$WORKSPACE/kernel/Makefile"
  fi

  # Patch the Makefiles (TESTS_DIR, QEMU)
  sed -i 's|^TESTS_DIR ?=.*|TESTS_DIR ?= ./tests|' "$WORKSPACE/Makefile"
  sed -i '/^# TESTS_DIR/d' "$WORKSPACE/Makefile"
  sed -i '/^QEMU_PREFER/d' "$WORKSPACE/Makefile"
  local qemu_cmd
  case "$P" in
    1) qemu_cmd="qemu-system-x86_64" ;; *) qemu_cmd="qemu-system-i386" ;;
  esac
  sed -i "s|^QEMU_CMD ?=.*|QEMU_CMD ?= $qemu_cmd|" "$WORKSPACE/Makefile"
  sed -i 's|: Makefile %.block_size$|: Makefile ${TESTS_DIR}/%.block_size|' "$WORKSPACE/Makefile"

  # Update tests
  rm -rf "$WORKSPACE/tests"
  mkdir -p "$WORKSPACE/tests"
  cp -r "$PROJECTS_DIR/p$P/tests/." "$WORKSPACE/tests/"

  rm -rf "$TMP"
}

echo "=== 439H Sequential Run (agent=$AGENT) ==="
echo "  Run ID: $RUN_ID"
echo "  Start: $(date)"
echo ""

mkdir -p "$SEQ_DIR"

# Start with P1 starter
WORKSPACE=$(mktemp -d)
echo "  Master workspace: $WORKSPACE"

cp -r "$PROJECTS_DIR/p1/starter/." "$WORKSPACE/"
mkdir -p "$WORKSPACE/tests"
cp -r "$PROJECTS_DIR/p1/tests/." "$WORKSPACE/tests/"

SUMMARY_FILE="$SEQ_DIR/summary.json"
echo "[" > "$SUMMARY_FILE"
FIRST=true

for P in 1 2 3 4 5 6 7; do
  echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
  echo "=== Sequential P$P ==="

  if [ "$P" -gt 1 ]; then
    echo "  Overlaying P$P stubs onto agent's workspace..."
    overlay_stubs "$P" "$WORKSPACE"
  fi

  # Clean build artifacts
  rm -f "$WORKSPACE"/*.result "$WORKSPACE"/*.time "$WORKSPACE"/*.out
  rm -f "$WORKSPACE"/*.raw "$WORKSPACE"/*.failure "$WORKSPACE"/*.diff
  rm -f "$WORKSPACE"/*.data "$WORKSPACE"/*_data.d
  rm -rf "$WORKSPACE/kernel/build"

  # For P2,3,5,6,7: copy grading tool to workspace
  case "$P" in
    2|3|5|6|7)
      cp "$SCRIPT_DIR/grade_439h.py" "$WORKSPACE/grade"
      chmod +x "$WORKSPACE/grade"
      ;;
  esac

  # Re-init git (codex needs it)
  rm -rf "$WORKSPACE/.git"
  git -C "$WORKSPACE" init -q
  git -C "$WORKSPACE" add -A
  git -C "$WORKSPACE" commit -q -m "p$P starter" --allow-empty

  # Start Docker container
  CONTAINER="gheithbench_seq_p${P}_$$"
  echo "  Starting Docker container: $CONTAINER"

  docker run -d --rm $PLATFORM \
    --name "$CONTAINER" \
    -v "$WORKSPACE:/workspace" \
    -w /workspace \
    "$IMAGE" \
    sleep infinity >/dev/null

  # Create exec wrapper
  PROXY_PID=""
  DOCKER_PROXY_PORT=""
  if [ "$(uname)" = "Linux" ]; then
    DOCKER_PROXY_PORT=$((20000 + RANDOM % 40000))
    socat TCP-LISTEN:$DOCKER_PROXY_PORT,reuseaddr,fork UNIX-CONNECT:/var/run/docker.sock &
    PROXY_PID=$!
    sleep 0.3

    cat > "$WORKSPACE/exec" << WRAPPER
#!/bin/bash
DOCKER_HOST=tcp://127.0.0.1:$DOCKER_PROXY_PORT docker exec $CONTAINER bash -c "cd /workspace && \$*"
WRAPPER
  else
    cat > "$WORKSPACE/exec" << WRAPPER
#!/bin/bash
docker exec $CONTAINER bash -c "cd /workspace && \$*"
WRAPPER
  fi
  chmod +x "$WORKSPACE/exec"

  # Run agent
  echo "  Running $AGENT on P$P..."
  PROMPT=$(cat "$PROJECTS_DIR/p$P/prompt.txt")
  AGENT_EXIT=0
  AGENT_START_TIME=$(date +%s)
  run_agent_cmd "$WORKSPACE" "$PROMPT" || AGENT_EXIT=$?
  AGENT_END_TIME=$(date +%s)
  AGENT_DURATION=$((AGENT_END_TIME - AGENT_START_TIME))
  echo "  $AGENT exited with code $AGENT_EXIT (${AGENT_DURATION}s)"

  # Cleanup container and proxy
  [ -n "$PROXY_PID" ] && kill "$PROXY_PID" 2>/dev/null || true
  docker stop "$CONTAINER" >/dev/null 2>&1 || true
  rm -f "$WORKSPACE/exec"

  # Save agent's source for this project
  AGENT_SRC_DIR="$SEQ_DIR/p${P}_src"
  mkdir -p "$AGENT_SRC_DIR"
  rsync -a --exclude='.git' --exclude='tests/' --exclude='build/' --exclude='kernel/build/' \
    --exclude='*.o' --exclude='*.d' --exclude='*.out' --exclude='*.diff' \
    --exclude='*.result' --exclude='*.time' --exclude='*.ok' --exclude='exec' \
    --exclude='*.data' --exclude='*.raw' --exclude='*.failure' --exclude='*_data.d' \
    --exclude='grade' \
    "$WORKSPACE/" "$AGENT_SRC_DIR/" || true

  # Test in fresh Docker
  echo "  Running tests for P$P..."
  docker run --rm $PLATFORM \
    -v "$WORKSPACE:/workspace" -w /workspace \
    "$IMAGE" \
    bash -c 'make clean 2>/dev/null; make -s test 2>&1' \
    | tee "$SEQ_DIR/p${P}_output.log" || true

  # Score
  SCORE_FILE="$SEQ_DIR/p${P}_score.json"
  GRADE_ITERATIONS="${GRADE_ITERATIONS:-100}"

  case "$P" in
    2|3|5|6|7)
      # Run parallel grading tool for reliability scoring
      echo "  Running reliability grading (${GRADE_ITERATIONS} iterations per test)..."
      GRADE_OUTPUT="$SEQ_DIR/p${P}_grade.json"
      docker run --rm $PLATFORM \
        -v "$WORKSPACE:/workspace" -w /workspace \
        "$IMAGE" \
        python3 /workspace/grade -p "p$P" -n "$GRADE_ITERATIONS" \
        > "$GRADE_OUTPUT" 2> "$SEQ_DIR/p${P}_grade.log" || true

      python3 -c "
import json, sys
try:
    with open('$GRADE_OUTPUT') as f:
        data = json.load(f)
except Exception:
    import subprocess
    result = subprocess.run(['bash', '$SCRIPT_DIR/score.sh', '$P', '$WORKSPACE'], capture_output=True, text=True)
    data = json.loads(result.stdout)
data['agent_runtime_seconds'] = $AGENT_DURATION
data['agent'] = '$AGENT'
data['mode'] = 'sequential'
data['run_id'] = '$RUN_ID'
data['benchmark'] = '439h'
data['grading_mode'] = 'reliability'
with open('$SCORE_FILE', 'w') as f:
    json.dump(data, f, indent=2)
"
      ;;
    *)
      bash "$SCRIPT_DIR/score.sh" "$P" "$WORKSPACE" > "$SCORE_FILE"
      python3 -c "
import json
with open('$SCORE_FILE') as f:
    data = json.load(f)
data['agent_runtime_seconds'] = $AGENT_DURATION
data['agent'] = '$AGENT'
data['mode'] = 'sequential'
data['run_id'] = '$RUN_ID'
data['benchmark'] = '439h'
data['grading_mode'] = 'single'
with open('$SCORE_FILE', 'w') as f:
    json.dump(data, f, indent=2)
"
      ;;
  esac

  echo "  Score:"
  cat "$SCORE_FILE"
  echo ""

  # Append to summary
  if [ "$FIRST" = true ]; then
    FIRST=false
  else
    echo "," >> "$SUMMARY_FILE"
  fi
  cat "$SCORE_FILE" >> "$SUMMARY_FILE"
done

echo "" >> "$SUMMARY_FILE"
echo "]" >> "$SUMMARY_FILE"

echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "=== Sequential Benchmark Complete ==="
echo "  End: $(date)"
echo "  Results: $SEQ_DIR"
echo "  Summary: $SUMMARY_FILE"
echo ""
echo "Results:"
cat "$SUMMARY_FILE"
echo ""
echo "  Workspace preserved at: $WORKSPACE"
