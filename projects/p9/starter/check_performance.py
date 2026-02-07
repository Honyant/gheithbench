#!/usr/bin/env python3
"""
Check your CPU's cycle performance against reference implementations.

Run after 'make test' to see how your cycles compare to optimized baselines.

Usage: python3 check_performance.py [tests_dir]
       Default tests_dir is ./tests
"""
import json
import sys
from pathlib import Path


def calculate_score(cycles: int, ref: dict) -> float:
    """
    Calculate 0-100 score based on where cycles falls in distribution.

    Scoring:
    - Below min (100): Faster than all reference implementations
    - Min to Q1 (90-100): Top 25% performance
    - Q1 to Median (75-90): Above average
    - Median to Q3 (50-75): Below average
    - Q3 to 2*Q3 (0-50): Poor performance
    - Above 2*Q3 (0): Very slow
    """
    min_c, q1, median, q3 = ref['min'], ref['q1'], ref['median'], ref['q3']

    if cycles <= min_c:
        return 100.0
    elif cycles <= q1:
        if q1 == min_c:
            return 100.0
        return 100.0 - 10.0 * (cycles - min_c) / (q1 - min_c)
    elif cycles <= median:
        if median == q1:
            return 90.0
        return 90.0 - 15.0 * (cycles - q1) / (median - q1)
    elif cycles <= q3:
        if q3 == median:
            return 75.0
        return 75.0 - 25.0 * (cycles - median) / (q3 - median)
    elif cycles <= 2 * q3:
        return 50.0 - 50.0 * (cycles - q3) / q3
    else:
        return 0.0


def main():
    tests_dir = Path(sys.argv[1]) if len(sys.argv) > 1 else Path("./tests")
    script_dir = Path(__file__).parent

    # Load reference data
    ref_file = script_dir / "cycles_reference.json"
    if not ref_file.exists():
        print(f"Error: {ref_file} not found")
        sys.exit(1)

    with open(ref_file) as f:
        reference = json.load(f)

    # Find all cycle files
    cycles_files = list(tests_dir.glob("*.cycles")) + list(Path(".").glob("*.cycles"))

    if not cycles_files:
        print("No .cycles files found. Run 'make test' first.")
        sys.exit(1)

    print(f"{'Test':<10} {'Cycles':>10} {'Ref Min':>10} {'Ref Med':>10} {'Ref Q3':>10} {'Score':>8}")
    print("-" * 68)

    total_score = 0.0
    count = 0

    for cycles_file in sorted(cycles_files):
        test_name = cycles_file.stem
        if test_name not in reference:
            continue

        try:
            cycles = int(cycles_file.read_text().strip().split()[0])
        except (ValueError, IndexError):
            continue

        ref = reference[test_name]
        score = calculate_score(cycles, ref)
        total_score += score
        count += 1

        # Color coding (ANSI)
        if score >= 90:
            color = "\033[92m"  # Green
        elif score >= 75:
            color = "\033[93m"  # Yellow
        elif score >= 50:
            color = "\033[91m"  # Red
        else:
            color = "\033[90m"  # Gray
        reset = "\033[0m"

        print(f"{test_name:<10} {cycles:>10} {ref['min']:>10} {ref['median']:>10} {ref['q3']:>10} {color}{score:>7.1f}{reset}")

    print("-" * 68)
    if count > 0:
        avg_score = total_score / count
        print(f"{'Average':>51} {avg_score:>7.1f}")
        print(f"\nTests scored: {count}")
        print(f"Performance score: {avg_score:.1f}/100")
        if avg_score >= 90:
            print("Excellent! Top quartile performance.")
        elif avg_score >= 75:
            print("Good! Above median performance.")
        elif avg_score >= 50:
            print("Fair. Below median - consider optimizations.")
        else:
            print("Poor. Significant optimization needed.")
    else:
        print("No tests could be scored.")


if __name__ == "__main__":
    main()
