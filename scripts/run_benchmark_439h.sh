#!/usr/bin/env bash
#
# run_benchmark_439h.sh — Run the full 439H benchmark suite.
# Usage: ./run_benchmark_439h.sh
#
# Environment variables:
#   AGENT           — Agent to use: codex (default), claude, gemini
#   AGENT_MODEL     — Model override (agent-specific)
#   AGENT_REASONING — Reasoning effort for codex (default: xhigh)
#   MODE            — independent (default), sequential, or both
#
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT_DIR="$(dirname "$SCRIPT_DIR")"
RESULTS_DIR="$ROOT_DIR/results_439h"

AGENT="${AGENT:-codex}"
MODE="${MODE:-independent}"
RUN_ID=$(date +%Y%m%d_%H%M%S)

echo "=== GheithBench 439H Full Run ==="
echo "  Agent: $AGENT"
echo "  Mode: $MODE"
echo "  Run ID: $RUN_ID"
echo "  Start: $(date)"
echo ""

mkdir -p "$RESULTS_DIR"

# ─── Independent mode ─────────────────────────────────────────────────────────
run_independent() {
  echo "=== Running Independent Mode ==="
  echo ""

  SUMMARY_FILE="$RESULTS_DIR/independent_summary_${RUN_ID}.json"
  echo "[" > "$SUMMARY_FILE"
  FIRST=true

  for p in 1 2 3 4 5 6 7; do
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    bash "$SCRIPT_DIR/run_agent_439h.sh" "$p" 2>&1 | tee "$RESULTS_DIR/p${p}/run_${RUN_ID}.log" || true

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

  echo ""
  echo "=== Independent Mode Complete ==="
  echo "  Summary: $SUMMARY_FILE"
  echo "Results:"
  cat "$SUMMARY_FILE"
  echo ""
}

# ─── Sequential mode ──────────────────────────────────────────────────────────
run_sequential() {
  echo "=== Running Sequential Mode ==="
  echo ""
  bash "$SCRIPT_DIR/run_sequential_439h.sh" 2>&1 | tee "$RESULTS_DIR/sequential_run_${RUN_ID}.log" || true
  echo ""
}

# ─── Main ─────────────────────────────────────────────────────────────────────
case "$MODE" in
  independent)
    run_independent
    ;;
  sequential)
    run_sequential
    ;;
  both)
    run_independent
    echo ""
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    echo ""
    run_sequential
    ;;
  *)
    echo "Unknown MODE: $MODE (use: independent, sequential, or both)" >&2
    exit 1
    ;;
esac

echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "=== 439H Benchmark Complete ==="
echo "  End: $(date)"
echo "  Results directory: $RESULTS_DIR"
