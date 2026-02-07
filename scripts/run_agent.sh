#!/usr/bin/env bash
#
# run_agent.sh — Run an LLM agent on a single project.
# Usage: ./run_agent.sh <project_number>
#
# Environment variables:
#   AGENT           — Agent to use: codex (default), claude
#   AGENT_MODEL     — Model override (agent-specific)
#   AGENT_REASONING — Reasoning effort for codex (default: medium)
#   GHEITHBENCH_PLATFORM — Docker platform flag (default: --platform linux/amd64)
#
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT_DIR="$(dirname "$SCRIPT_DIR")"
PROJECTS_DIR="$ROOT_DIR/projects"
RESULTS_DIR="$ROOT_DIR/results"

PLATFORM="${GHEITHBENCH_PLATFORM:---platform linux/amd64}"
AGENT="${AGENT:-codex}"
AGENT_MODEL="${AGENT_MODEL:-}"
AGENT_REASONING="${AGENT_REASONING:-xhigh}"

P="$1"
PROJ_DIR="$PROJECTS_DIR/p$P"

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
    *)
      echo "Unknown agent: $AGENT" >&2; return 1 ;;
  esac
}

IMAGE=$(get_docker_image "$P")
LAYOUT=$(get_test_layout "$P")
WORKSPACE=$(mktemp -d)
RUN_ID=$(date +%Y%m%d_%H%M%S)
CONTAINER=""

echo "=== P$P: Running $AGENT ==="
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

# 2. Start Docker container and create exec wrapper
CONTAINER="gheithbench_p${P}_$$"
echo "  Starting Docker container: $CONTAINER"

docker run -d --rm $PLATFORM \
  --name "$CONTAINER" \
  -v "$WORKSPACE:/workspace" \
  -w /workspace \
  "$IMAGE" \
  sleep infinity >/dev/null

# Proxy the Docker socket into /tmp so sandboxed agents (codex on Linux) can
# reach it.  On macOS the socket is already accessible; on Linux, codex's
# Landlock sandbox only allows workdir + /tmp.
# On Linux, codex's sandbox blocks the Docker socket (even in /tmp).
# Proxy via TCP on localhost which the sandbox does allow.
PROXY_PID=""
DOCKER_PROXY_PORT=""
if [ "$(uname)" = "Linux" ]; then
  # Pick a random high port
  DOCKER_PROXY_PORT=$((20000 + RANDOM % 40000))
  socat TCP-LISTEN:$DOCKER_PROXY_PORT,reuseaddr,fork UNIX-CONNECT:/var/run/docker.sock &
  PROXY_PID=$!
  sleep 0.3  # let socat bind
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
echo "  Agent runtime: ${AGENT_DURATION}s ($(date -u -d @$AGENT_DURATION +%H:%M:%S))"

# 4. Stop Docker container, socat proxy, and remove exec wrapper
if [ -n "$PROXY_PID" ]; then
  kill "$PROXY_PID" 2>/dev/null || true
fi
if [ -n "$CONTAINER" ]; then
  echo "  Stopping Docker container: $CONTAINER"
  docker stop "$CONTAINER" >/dev/null 2>&1 || true
  rm -f "$WORKSPACE/exec"
fi

# 5. Save agent's source files to results dir (for inspection)
AGENT_SRC_DIR="$RESULTS_DIR/p$P/${AGENT}_src_${RUN_ID}"
mkdir -p "$AGENT_SRC_DIR"
rsync -a --exclude='.git' --exclude='tests/' --exclude='build/' \
  --exclude='*.o' --exclude='*.d' --exclude='*.out' --exclude='*.diff' \
  --exclude='*.result' --exclude='*.time' --exclude='*.err' --exclude='*.run' \
  --exclude='*.ok' --exclude='exec' \
  --exclude='*.vcd' --exclude='mem.hex' --exclude='cpu' --exclude='mem_tb' \
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
if [ "$P" = "8" ] || [ "$P" = "9" ]; then
  python3 "$SCRIPT_DIR/score_p8.py" "$WORKSPACE" > "$SCORE_FILE"
else
  bash "$SCRIPT_DIR/score.sh" "$P" "$WORKSPACE" > "$SCORE_FILE"
fi

# Add timing to score JSON
python3 -c "
import json
with open('$SCORE_FILE') as f:
    data = json.load(f)
data['agent_runtime_seconds'] = $AGENT_DURATION
data['agent'] = '$AGENT'
data['run_id'] = '$RUN_ID'
with open('$SCORE_FILE', 'w') as f:
    json.dump(data, f, indent=2)
"

echo ""
echo "  Score:"
cat "$SCORE_FILE"
echo ""

# Save workspace for debugging
echo "  Workspace preserved at: $WORKSPACE"
