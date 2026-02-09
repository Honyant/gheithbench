#!/usr/bin/env python3
"""
grade_439h.py -- Parallel reliability grader for CS 439H kernel projects.

Runs each test N times in parallel via QEMU and grades based on reliability.
Outputs JSON to stdout and human-readable progress to stderr.

Usage:
    ./grade_439h.py [-p PROJECT] [-n ITERATIONS] [-j PARALLEL_JOBS]

Project Types (auto-detected from workspace structure):
    Type A (P1-P4): Per-test kernels, no data files
    Type B (P5-P6): Per-test kernels + data files (ext2)
    Type C (P7):    Single kernel + data files (ext2)

Grading formula per test:
    0 failures -> 100%
    1 failure  ->  50%
    2 failures ->  25%
    3+ failures->   0%
"""

import argparse
import glob
import json
import os
import re
import signal
import subprocess
import sys
import tempfile
import threading
import time
from concurrent.futures import ThreadPoolExecutor, as_completed
from pathlib import Path


# ---------------------------------------------------------------------------
# Output helpers
# ---------------------------------------------------------------------------

def log(msg):
    """Print a message to stderr."""
    print(msg, file=sys.stderr, flush=True)


def error_exit(msg):
    """Print error to stderr and exit with code 1."""
    print(f"Error: {msg}", file=sys.stderr, flush=True)
    sys.exit(1)


# ---------------------------------------------------------------------------
# Project type detection and configuration
# ---------------------------------------------------------------------------

class ProjectType:
    """Constants for project types."""
    A = "A"  # P1-P4: per-test kernels, no data files
    B = "B"  # P5-P6: per-test kernels + data files
    C = "C"  # P7:    single kernel + data files


class ProjectConfig:
    """Holds all configuration for a detected project."""

    def __init__(self, project_type, qemu_cmd, timeout, tests_dir, tests):
        self.project_type = project_type
        self.qemu_cmd = qemu_cmd
        self.timeout = timeout
        self.tests_dir = tests_dir
        self.tests = tests  # sorted list of test names


def parse_makefile_var(makefile_path, var_name):
    """Extract a ?= or = variable assignment from a Makefile.

    Returns the value string or None if not found.
    """
    try:
        with open(makefile_path, "r") as f:
            for line in f:
                m = re.match(
                    r"^\s*" + re.escape(var_name) + r"\s*\??=\s*(.*?)\s*$", line
                )
                if m:
                    return m.group(1).strip()
    except FileNotFoundError:
        pass
    return None


def detect_project(workdir):
    """Auto-detect project type, QEMU command, timeout, and tests.

    Returns a ProjectConfig. workdir is the root of the built project workspace.
    """
    tests_dir = os.path.join(workdir, "tests")

    if not os.path.isdir(tests_dir):
        error_exit("tests/ directory not found in workspace")

    # Discover .ok files to get test names
    ok_files = sorted(glob.glob(os.path.join(tests_dir, "*.ok")))
    if not ok_files:
        error_exit("No .ok test files found in tests/")

    test_names = sorted(
        os.path.splitext(os.path.basename(f))[0] for f in ok_files
    )

    # Detect project type based on file presence
    has_cc = len(glob.glob(os.path.join(tests_dir, "*.cc"))) > 0
    has_block_size = len(glob.glob(os.path.join(tests_dir, "*.block_size"))) > 0
    has_dir = len(glob.glob(os.path.join(tests_dir, "*.dir"))) > 0

    if has_cc:
        if has_block_size:
            project_type = ProjectType.B
        else:
            project_type = ProjectType.A
    elif has_dir:
        project_type = ProjectType.C
    else:
        # Fallback: treat as Type A
        project_type = ProjectType.A

    # Read QEMU_CMD from Makefile (default: qemu-system-i386)
    makefile_path = os.path.join(workdir, "Makefile")
    qemu_cmd = parse_makefile_var(makefile_path, "QEMU_CMD")
    if not qemu_cmd:
        qemu_cmd = "qemu-system-i386"

    # Read QEMU_TIMEOUT from Makefile (default: 10)
    timeout_str = parse_makefile_var(makefile_path, "QEMU_TIMEOUT")
    if timeout_str:
        try:
            timeout = int(timeout_str)
        except ValueError:
            timeout = 10
    else:
        timeout = 10

    return ProjectConfig(
        project_type=project_type,
        qemu_cmd=qemu_cmd,
        timeout=timeout,
        tests_dir=tests_dir,
        tests=test_names,
    )


# ---------------------------------------------------------------------------
# Data file generation (Types B and C)
# ---------------------------------------------------------------------------

def generate_data_file(config, test_name, workdir):
    """Generate a .data ext2 filesystem image for a test.

    Only generates if the test needs a data file and it does not already exist.
    Returns the path to the data file, or None on failure.
    """
    data_path = os.path.join(workdir, f"{test_name}.data")
    if os.path.exists(data_path):
        return data_path

    if config.project_type == ProjectType.B:
        # Read block size from tests/TEST.block_size
        bs_path = os.path.join(config.tests_dir, f"{test_name}.block_size")
        if not os.path.isfile(bs_path):
            log(f"Warning: {bs_path} not found for test {test_name}, skipping data generation")
            return None
        with open(bs_path, "r") as f:
            block_size = f.read().strip()
        dir_path = os.path.join(config.tests_dir, f"{test_name}.dir")
    elif config.project_type == ProjectType.C:
        block_size = "4096"
        dir_path = os.path.join(config.tests_dir, f"{test_name}.dir")
    else:
        return None

    if not os.path.isdir(dir_path):
        log(f"Warning: {dir_path} not found for test {test_name}, skipping data generation")
        return None

    cmd = [
        "mkfs.ext2", "-q",
        "-b", block_size,
        "-i", block_size,
        "-d", dir_path,
        "-I", "128",
        "-r", "0",
        "-t", "ext2",
        data_path, "10m",
    ]
    result = subprocess.run(cmd, capture_output=True, text=True)
    if result.returncode != 0:
        log(f"Warning: mkfs.ext2 failed for {test_name}: {result.stderr.strip()}")
        return None

    return data_path


# ---------------------------------------------------------------------------
# QEMU execution
# ---------------------------------------------------------------------------

def build_qemu_cmd(config, test_name, serial_path, workdir):
    """Build the QEMU command list for a single test invocation."""
    cmd = [
        "timeout", str(config.timeout),
        config.qemu_cmd,
        "-no-reboot",
        "-accel", "tcg,thread=multi",
        "-cpu", "max",
        "-smp", "4",
        "-m", "128m",
        "-nographic",
        "--monitor", "none",
        "--serial", f"file:{serial_path}",
    ]

    if config.project_type == ProjectType.A:
        kernel_img = os.path.join(workdir, "kernel", "build", f"{test_name}.img")
        cmd += [
            "-drive", f"file={kernel_img},index=0,media=disk,format=raw,file.locking=off",
        ]
    elif config.project_type == ProjectType.B:
        kernel_img = os.path.join(workdir, "kernel", "build", f"{test_name}.img")
        data_file = os.path.join(workdir, f"{test_name}.data")
        cmd += [
            "-drive", f"file={kernel_img},index=0,media=disk,format=raw,file.locking=off",
            "-drive", f"file={data_file},index=1,media=disk,format=raw,file.locking=off",
        ]
    elif config.project_type == ProjectType.C:
        kernel_img = os.path.join(workdir, "kernel", "build", "kernel.img")
        data_file = os.path.join(workdir, f"{test_name}.data")
        cmd += [
            "-drive", f"file={kernel_img},index=0,media=disk,format=raw,file.locking=off",
            "-drive", f"file={data_file},index=1,media=disk,format=raw,file.locking=off",
        ]

    cmd += [
        "-device", "isa-debug-exit,iobase=0xf4,iosize=0x04",
    ]

    return cmd


def normalize_for_diff(text):
    """Normalize text for whitespace-ignoring comparison (like diff -wBb).

    - Strip trailing whitespace from each line
    - Collapse runs of whitespace within each line to a single space
    - Remove blank lines
    """
    lines = []
    for line in text.splitlines():
        line = line.rstrip()
        line = re.sub(r"\s+", " ", line).strip()
        if line:
            lines.append(line)
    return "\n".join(lines)


def run_single_test(config, test_name, workdir, shutdown_event):
    """Run one QEMU invocation for test_name.

    Returns True on pass, False on fail. Checks shutdown_event before running.
    """
    if shutdown_event.is_set():
        return False

    # Create a unique temp file for serial output
    fd, serial_path = tempfile.mkstemp(
        prefix=f"grade_{test_name}_", suffix=".raw"
    )
    os.close(fd)

    try:
        cmd = build_qemu_cmd(config, test_name, serial_path, workdir)
        subprocess.run(
            cmd,
            capture_output=True,
            cwd=workdir,
        )

        # Read serial output
        try:
            with open(serial_path, "r", errors="replace") as f:
                raw_output = f.read()
        except FileNotFoundError:
            return False

        # Extract lines starting with ***
        star_lines = []
        for line in raw_output.splitlines():
            if re.match(r"^\*\*\*", line):
                star_lines.append(line)
        actual = "\n".join(star_lines)

        # Read expected output
        ok_path = os.path.join(config.tests_dir, f"{test_name}.ok")
        try:
            with open(ok_path, "r") as f:
                expected = f.read()
        except FileNotFoundError:
            return False

        # Compare with whitespace-ignoring normalization
        return normalize_for_diff(actual) == normalize_for_diff(expected)

    finally:
        # Clean up temp file
        try:
            os.unlink(serial_path)
        except OSError:
            pass


# ---------------------------------------------------------------------------
# Grading
# ---------------------------------------------------------------------------

def compute_grade(passed, total):
    """Compute grade for a test given pass/total counts.

    Grading formula:
        0 failures -> 100%
        1 failure  ->  50%
        2 failures ->  25%
        3+ failures->   0%
    """
    failures = total - passed
    if failures == 0:
        return 100.0
    elif failures == 1:
        return 50.0
    elif failures == 2:
        return 25.0
    else:
        return 0.0


# ---------------------------------------------------------------------------
# Kernel image verification
# ---------------------------------------------------------------------------

def verify_kernel_images(config, workdir):
    """Check that the required kernel images exist. Exit with error if missing."""
    if config.project_type == ProjectType.C:
        kernel_img = os.path.join(workdir, "kernel", "build", "kernel.img")
        if not os.path.isfile(kernel_img):
            error_exit(
                f"Kernel image not found: {kernel_img}\n"
                "  Run 'make' first to build the project."
            )
    else:
        missing = []
        for test_name in config.tests:
            img = os.path.join(workdir, "kernel", "build", f"{test_name}.img")
            if not os.path.isfile(img):
                missing.append(img)
        if missing:
            error_exit(
                f"Kernel images not found ({len(missing)} missing):\n"
                + "\n".join(f"  {m}" for m in missing[:10])
                + ("\n  ..." if len(missing) > 10 else "")
                + "\n  Run 'make' first to build the project."
            )


# ---------------------------------------------------------------------------
# Auto-detect project name
# ---------------------------------------------------------------------------

def detect_project_name(workdir):
    """Determine the project name (e.g. 'p2') from the workspace directory name."""
    basename = os.path.basename(os.path.abspath(workdir))
    m = re.match(r"^(p\d+)$", basename, re.IGNORECASE)
    if m:
        return m.group(1).lower()
    return basename


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main():
    parser = argparse.ArgumentParser(
        description="Parallel reliability grader for CS 439H kernel projects."
    )
    parser.add_argument(
        "-p", "--project",
        help="Project name (auto-detected from directory if not specified)",
    )
    parser.add_argument(
        "-n", "--iterations",
        type=int,
        default=100,
        help="Number of iterations per test (default: 100)",
    )
    parser.add_argument(
        "-j", "--jobs",
        type=int,
        default=None,
        help="Max parallel QEMU jobs (default: ncpus/4, since each QEMU uses 4 vCPUs)",
    )
    args = parser.parse_args()

    workdir = os.getcwd()
    iterations = args.iterations
    cpu_count = os.cpu_count() or 4
    # Each QEMU uses -smp 4 (4 vCPU threads), so default to ncpus/4
    parallel_jobs = args.jobs if args.jobs else max(1, cpu_count // 4)

    # Auto-detect project name
    project_name = args.project if args.project else detect_project_name(workdir)

    # Detect project configuration
    config = detect_project(workdir)

    # Scale timeout for CPU oversubscription: each QEMU uses 4 vCPU threads
    total_vcpus = parallel_jobs * 4
    oversubscription = total_vcpus / cpu_count
    if oversubscription > 1.0:
        timeout_multiplier = max(1, int(oversubscription + 0.5))
        config.timeout = config.timeout * timeout_multiplier

    log(f"Project: {project_name} (Type {config.project_type})")
    log(f"QEMU: {config.qemu_cmd}, timeout: {config.timeout}s")

    # Verify kernel images exist
    verify_kernel_images(config, workdir)

    # Generate data files if needed (Types B and C)
    if config.project_type in (ProjectType.B, ProjectType.C):
        log("Generating data files...")
        data_failures = []
        for test_name in config.tests:
            dir_path = os.path.join(config.tests_dir, f"{test_name}.dir")
            if os.path.isdir(dir_path):
                data_path = generate_data_file(config, test_name, workdir)
                if data_path is None:
                    data_failures.append(test_name)
        if data_failures:
            log(f"Warning: Failed to generate data files for {len(data_failures)} tests")

    total_tests = len(config.tests)
    total_runs = total_tests * iterations
    log(f"Grading: {total_tests} tests x {iterations} iterations ({parallel_jobs} parallel jobs)")

    # Track results per test
    results_lock = threading.Lock()
    results = {name: {"passed": 0, "failed": 0} for name in config.tests}
    completed_count = [0]

    # Flag for graceful shutdown on SIGINT
    shutdown_event = threading.Event()

    def handle_sigint(signum, frame):
        log("\nInterrupted, cancelling remaining jobs...")
        shutdown_event.set()

    original_sigint = signal.getsignal(signal.SIGINT)
    signal.signal(signal.SIGINT, handle_sigint)

    start_time = time.monotonic()

    try:
        with ThreadPoolExecutor(max_workers=parallel_jobs) as executor:
            # Submit all jobs: each future is one QEMU invocation
            future_to_info = {}
            for test_name in config.tests:
                for i in range(iterations):
                    if shutdown_event.is_set():
                        break
                    future = executor.submit(
                        run_single_test, config, test_name, workdir, shutdown_event
                    )
                    future_to_info[future] = (test_name, i)
                if shutdown_event.is_set():
                    break

            # Process results as they complete
            for future in as_completed(future_to_info):
                if shutdown_event.is_set():
                    for f in future_to_info:
                        f.cancel()
                    break

                test_name, iteration = future_to_info[future]
                try:
                    passed = future.result(timeout=0.1)
                except Exception:
                    passed = False

                with results_lock:
                    if passed:
                        results[test_name]["passed"] += 1
                    else:
                        results[test_name]["failed"] += 1
                    completed_count[0] += 1

                    if completed_count[0] % 50 == 0 or completed_count[0] == total_runs:
                        elapsed = time.monotonic() - start_time
                        rate = completed_count[0] / elapsed if elapsed > 0 else 0
                        log(
                            f"  Progress: {completed_count[0]}/{total_runs} "
                            f"({rate:.1f} runs/s)"
                        )

    except KeyboardInterrupt:
        log("\nInterrupted.")
    finally:
        signal.signal(signal.SIGINT, original_sigint)

    elapsed_seconds = round(time.monotonic() - start_time, 1)

    # Build per-test results
    test_results = []
    for test_name in config.tests:
        p = results[test_name]["passed"]
        f = results[test_name]["failed"]
        total = p + f
        grade = compute_grade(p, total) if total > 0 else 0.0
        test_results.append({
            "name": test_name,
            "passed": p,
            "failed": f,
            "grade": grade,
        })

    # Print human-readable results to stderr
    log("")
    for tr in test_results:
        total = tr["passed"] + tr["failed"]
        failure_note = ""
        if tr["failed"] > 0:
            failure_note = f"  [{tr['failed']} failure{'s' if tr['failed'] != 1 else ''}]"
        log(
            f"  {tr['name']}: {tr['passed']:>3}/{total} passed  "
            f"grade={tr['grade']:>3.0f}%{failure_note}"
        )

    # Compute total grade = mean of all test grades
    if test_results:
        total_grade = round(
            sum(tr["grade"] for tr in test_results) / len(test_results), 1
        )
    else:
        total_grade = 0.0

    log(f"\nOverall grade: {total_grade}%")
    log(f"Elapsed: {elapsed_seconds}s")

    # Output JSON to stdout
    output = {
        "project": project_name,
        "iterations": iterations,
        "parallel_jobs": parallel_jobs,
        "tests": test_results,
        "total_tests": total_tests,
        "total_grade": total_grade,
        "elapsed_seconds": elapsed_seconds,
    }
    print(json.dumps(output, indent=2))


if __name__ == "__main__":
    main()
