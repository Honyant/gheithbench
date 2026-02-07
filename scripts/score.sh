#!/usr/bin/env bash
#
# score.sh — Parse test results from a workspace and produce JSON.
# Usage: ./score.sh <project_number> <workspace_dir>
#
# Looks for .result files (containing "pass" or "fail") in the workspace
# and its tests/ subdirectory.
#
set -euo pipefail

P="$1"
WORKSPACE="$2"

# Collect all .result files
RESULT_FILES=()
for f in "$WORKSPACE"/*.result "$WORKSPACE"/tests/*.result; do
  [ -f "$f" ] && RESULT_FILES+=("$f")
done

TOTAL=${#RESULT_FILES[@]}
PASSED=0
FAILED=0

for f in "${RESULT_FILES[@]+"${RESULT_FILES[@]}"}"; do
  result=$(cat "$f" 2>/dev/null | tr -d '[:space:]')
  if [ "$result" = "pass" ]; then
    PASSED=$((PASSED + 1))
  else
    FAILED=$((FAILED + 1))
  fi
done

if [ "$TOTAL" -gt 0 ]; then
  # Use awk for floating point
  PCT=$(awk "BEGIN {printf \"%.1f\", ($PASSED / $TOTAL) * 100}")
else
  PCT="0.0"
fi

cat << EOF
{"project": "p$P", "total_tests": $TOTAL, "passed": $PASSED, "failed": $FAILED, "score_pct": $PCT}
EOF
