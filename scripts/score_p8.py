#!/usr/bin/env python3
"""
Score P8 with percentile-based cycle performance scoring.

Usage: score_p8.py <workspace_dir>

Output: JSON with correctness and performance metrics.
"""
import json
import sys
import os
from pathlib import Path


def calculate_test_score(cycles: int, ref: dict) -> float:
    """
    Calculate 0-100 score based on where cycles falls in distribution.

    Scoring scheme:
    - Below min: 100 (faster than all reference implementations)
    - Min to Q1: 90-100 (top 25% performance)
    - Q1 to Median: 75-90 (above average)
    - Median to Q3: 50-75 (below average)
    - Q3 to 2*Q3: 0-50 (poor performance)
    - Above 2*Q3: 0 (very slow)
    """
    min_c = ref['min']
    q1 = ref['q1']
    median = ref['median']
    q3 = ref['q3']

    if cycles <= min_c:
        return 100.0
    elif cycles <= q1:
        # 90-100 range
        if q1 == min_c:
            return 100.0
        return 100.0 - 10.0 * (cycles - min_c) / (q1 - min_c)
    elif cycles <= median:
        # 75-90 range
        if median == q1:
            return 90.0
        return 90.0 - 15.0 * (cycles - q1) / (median - q1)
    elif cycles <= q3:
        # 50-75 range
        if q3 == median:
            return 75.0
        return 75.0 - 25.0 * (cycles - median) / (q3 - median)
    elif cycles <= 2 * q3:
        # 0-50 range
        return 50.0 - 50.0 * (cycles - q3) / q3
    else:
        return 0.0


def main():
    if len(sys.argv) < 2:
        print("Usage: score_p8.py <workspace_dir>", file=sys.stderr)
        sys.exit(1)

    workspace = Path(sys.argv[1])
    proj_dir = Path(__file__).parent.parent / "projects" / "p8"

    # Load reference data
    ref_file = proj_dir / "cycles_reference.json"
    if not ref_file.exists():
        print(f"Error: {ref_file} not found", file=sys.stderr)
        sys.exit(1)

    with open(ref_file) as f:
        reference = json.load(f)

    # Collect all .result files from workspace and tests/ subdirectory
    result_files = list(workspace.glob("*.result")) + list(workspace.glob("tests/*.result"))

    passed = 0
    failed = 0
    test_scores = []
    test_details = []

    for result_file in result_files:
        test_name = result_file.stem

        try:
            is_pass = result_file.read_text().strip() == "pass"
        except Exception:
            is_pass = False

        if is_pass:
            passed += 1

            # Look for .cycles file in same directory as .result
            cycles_file = result_file.with_suffix(".cycles")

            if cycles_file.exists() and test_name in reference:
                try:
                    cycles_text = cycles_file.read_text().strip()
                    # Handle potential extra whitespace or newlines
                    cycles = int(cycles_text.split()[0])
                    ref = reference[test_name]
                    score = calculate_test_score(cycles, ref)
                    test_scores.append(score)
                    test_details.append({
                        "test": test_name,
                        "cycles": cycles,
                        "ref_median": ref["median"],
                        "score": round(score, 1)
                    })
                except (ValueError, IndexError) as e:
                    # Couldn't parse cycles, skip performance scoring for this test
                    pass
        else:
            failed += 1

    total = passed + failed
    correctness_pct = (passed / total * 100) if total > 0 else 0.0
    perf_score = sum(test_scores) / len(test_scores) if test_scores else 0.0

    # Sort test_details by test name for consistent output
    test_details.sort(key=lambda x: x["test"])

    result = {
        "project": "p8",
        "total_tests": total,
        "passed": passed,
        "failed": failed,
        "correctness_pct": round(correctness_pct, 1),
        "performance_score": round(perf_score, 1),
        "tests_scored": len(test_scores),
        "test_details": test_details
    }

    print(json.dumps(result, indent=2))


if __name__ == "__main__":
    main()
