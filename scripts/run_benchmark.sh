#!/usr/bin/env bash
#
# run_benchmark.sh — Run an LLM agent on all projects and collect scores.
# Usage: ./run_benchmark.sh
#
# Environment variables:
#   AGENT           — Agent to use: codex (default), claude
#   AGENT_MODEL     — Model override (agent-specific)
#   AGENT_REASONING — Reasoning effort for codex (default: medium)
#
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT_DIR="$(dirname "$SCRIPT_DIR")"
RESULTS_DIR="$ROOT_DIR/results"
RUN_ID=$(date +%Y%m%d_%H%M%S)
AGENT="${AGENT:-codex}"

echo "=== GheithBench Full Run (agent=$AGENT) ==="
echo "  Run ID: $RUN_ID"
echo "  Start: $(date)"
echo ""

mkdir -p "$RESULTS_DIR"

SUMMARY_FILE="$RESULTS_DIR/summary_${RUN_ID}.json"

echo "[" > "$SUMMARY_FILE"

FIRST=true
for p in 1 2 3 4 5 6 7 8; do
  echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
  bash "$SCRIPT_DIR/run_agent.sh" "$p" 2>&1 | tee "$RESULTS_DIR/p${p}/run_${RUN_ID}.log" || true

  # Find the latest score file
  SCORE_FILE=$(ls -t "$RESULTS_DIR/p$p"/${AGENT}_score_*.json 2>/dev/null | head -1)
  if [ -n "$SCORE_FILE" ] && [ -f "$SCORE_FILE" ]; then
    if [ "$FIRST" = true ]; then
      FIRST=false
    else
      echo "," >> "$SUMMARY_FILE"
    fi
    cat "$SCORE_FILE" >> "$SUMMARY_FILE"
  fi
  echo ""
done

echo "" >> "$SUMMARY_FILE"
echo "]" >> "$SUMMARY_FILE"

echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "=== Benchmark Complete ==="
echo "  End: $(date)"
echo "  Summary: $SUMMARY_FILE"
echo ""
echo "Results:"
cat "$SUMMARY_FILE"
