#!/usr/bin/env bash
#
# run_agent_439h.sh — Run an LLM agent on a single 439H project.
# Usage: ./run_agent_439h.sh <project_number>
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
PROJECTS_DIR="$ROOT_DIR/projects_439h"
RESULTS_DIR="$ROOT_DIR/results_439h"

PLATFORM="${GHEITHBENCH_PLATFORM:---platform linux/amd64}"
AGENT="${AGENT:-codex}"
AGENT_MODEL="${AGENT_MODEL:-}"
AGENT_REASONING="${AGENT_REASONING:-xhigh}"

P="$1"
PROJ_DIR="$PROJECTS_DIR/p$P"
IMAGE="gheithbench-os:latest"

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

WORKSPACE=$(mktemp -d)
RUN_ID=$(date +%Y%m%d_%H%M%S)
CONTAINER=""

echo "=== 439H P$P: Running $AGENT ==="
echo "  Image: $IMAGE"
echo "  Workspace: $WORKSPACE"
echo "  Run ID: $RUN_ID"

mkdir -p "$RESULTS_DIR/p$P"

# 1. Prepare fresh workspace (starter + tests)
cp -r "$PROJ_DIR/starter/." "$WORKSPACE/"
mkdir -p "$WORKSPACE/tests"
cp -r "$PROJ_DIR/tests/." "$WORKSPACE/tests/"

# For P2,3,5,6,7: copy grading tool to workspace
case "$P" in
  2|3|5|6|7)
    cp "$SCRIPT_DIR/grade_439h.py" "$WORKSPACE/grade"
    chmod +x "$WORKSPACE/grade"
    ;;
esac

# Init a git repo so codex doesn't complain
git -C "$WORKSPACE" init -q
git -C "$WORKSPACE" add -A
git -C "$WORKSPACE" commit -q -m "starter" --allow-empty

# 2. Start Docker container and create exec wrapper
CONTAINER="gheithbench_os_p${P}_$$"
echo "  Starting Docker container: $CONTAINER"

docker run -d --rm $PLATFORM \
  --name "$CONTAINER" \
  -v "$WORKSPACE:/workspace" \
  -w /workspace \
  "$IMAGE" \
  sleep infinity >/dev/null

# Proxy Docker socket via TCP on Linux (for codex sandbox compatibility)
PROXY_PID=""
DOCKER_PROXY_PORT=""
if [ "$(uname)" = "Linux" ]; then
  DOCKER_PROXY_PORT=$((20000 + RANDOM % 40000))
  socat TCP-LISTEN:$DOCKER_PROXY_PORT,reuseaddr,fork UNIX-CONNECT:/var/run/docker.sock &
  PROXY_PID=$!
  sleep 0.3
  echo "  Docker TCP proxy: localhost:$DOCKER_PROXY_PORT (pid $PROXY_PID)"

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

# 3. Run agent
echo "  Running $AGENT (model=${AGENT_MODEL:-default})..."
PROMPT=$(cat "$PROJ_DIR/prompt.txt")
AGENT_EXIT=0
AGENT_START_TIME=$(date +%s)
run_agent_cmd "$WORKSPACE" "$PROMPT" || AGENT_EXIT=$?
AGENT_END_TIME=$(date +%s)
AGENT_DURATION=$((AGENT_END_TIME - AGENT_START_TIME))
echo "  $AGENT exited with code $AGENT_EXIT"
echo "  Agent runtime: ${AGENT_DURATION}s ($(date -u -d @$AGENT_DURATION +%H:%M:%S 2>/dev/null || date -u -r $AGENT_DURATION +%H:%M:%S))"

# 4. Stop Docker container and socat proxy
if [ -n "$PROXY_PID" ]; then
  kill "$PROXY_PID" 2>/dev/null || true
fi
if [ -n "$CONTAINER" ]; then
  echo "  Stopping Docker container: $CONTAINER"
  docker stop "$CONTAINER" >/dev/null 2>&1 || true
  rm -f "$WORKSPACE/exec"
fi

# 5. Save agent's source files to results dir
AGENT_SRC_DIR="$RESULTS_DIR/p$P/${AGENT}_src_${RUN_ID}"
mkdir -p "$AGENT_SRC_DIR"
rsync -a --exclude='.git' --exclude='tests/' --exclude='build/' --exclude='kernel/build/' \
  --exclude='*.o' --exclude='*.d' --exclude='*.out' --exclude='*.diff' \
  --exclude='*.result' --exclude='*.time' --exclude='*.err' --exclude='*.run' \
  --exclude='*.ok' --exclude='exec' --exclude='*.data' --exclude='*.raw' \
  --exclude='*.failure' --exclude='*.kernel' --exclude='*_data.d' \
  --exclude='grade' \
  "$WORKSPACE/" "$AGENT_SRC_DIR/" || true
echo "  Source saved to: $AGENT_SRC_DIR"

# 6. Test in Docker (fresh container for clean eval)
echo "  Running tests in Docker..."

docker run --rm $PLATFORM \
  -v "$WORKSPACE:/workspace" -w /workspace \
  "$IMAGE" \
  bash -c 'make clean 2>/dev/null; make -s test 2>&1' \
  | tee "$RESULTS_DIR/p$P/${AGENT}_output_${RUN_ID}.log" || true

# 7. Score
SCORE_FILE="$RESULTS_DIR/p$P/${AGENT}_score_${RUN_ID}.json"

GRADE_ITERATIONS="${GRADE_ITERATIONS:-100}"

case "$P" in
  2|3|5|6|7)
    # Run parallel grading tool for reliability scoring (100 iterations)
    echo "  Running reliability grading (${GRADE_ITERATIONS} iterations per test)..."
    GRADE_OUTPUT="$RESULTS_DIR/p$P/${AGENT}_grade_${RUN_ID}.json"
    docker run --rm $PLATFORM \
      -v "$WORKSPACE:/workspace" -w /workspace \
      "$IMAGE" \
      python3 /workspace/grade -p "p$P" -n "$GRADE_ITERATIONS" \
      > "$GRADE_OUTPUT" 2> "$RESULTS_DIR/p$P/${AGENT}_grade_${RUN_ID}.log" || true

    # Merge grading results with metadata
    python3 -c "
import json, sys
try:
    with open('$GRADE_OUTPUT') as f:
        data = json.load(f)
except Exception:
    # Fallback to single-run scoring if grading tool failed
    import subprocess
    result = subprocess.run(['bash', '$SCRIPT_DIR/score.sh', '$P', '$WORKSPACE'], capture_output=True, text=True)
    data = json.loads(result.stdout)
data['agent_runtime_seconds'] = $AGENT_DURATION
data['agent'] = '$AGENT'
data['run_id'] = '$RUN_ID'
data['benchmark'] = '439h'
data['grading_mode'] = 'reliability'
with open('$SCORE_FILE', 'w') as f:
    json.dump(data, f, indent=2)
"
    ;;
  *)
    # Single-run scoring for P1, P4
    bash "$SCRIPT_DIR/score.sh" "$P" "$WORKSPACE" > "$SCORE_FILE"
    python3 -c "
import json
with open('$SCORE_FILE') as f:
    data = json.load(f)
data['agent_runtime_seconds'] = $AGENT_DURATION
data['agent'] = '$AGENT'
data['run_id'] = '$RUN_ID'
data['benchmark'] = '439h'
data['grading_mode'] = 'single'
with open('$SCORE_FILE', 'w') as f:
    json.dump(data, f, indent=2)
"
    ;;
esac

echo ""
echo "  Score:"
cat "$SCORE_FILE"
echo ""

# Save workspace for debugging
echo "  Workspace preserved at: $WORKSPACE"
