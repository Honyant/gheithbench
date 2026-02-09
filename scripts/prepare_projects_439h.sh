#!/usr/bin/env bash
#
# prepare_projects_439h.sh — Extract starter code, reference solutions, and tests
# from the CS439H git repos into the gheithbench/projects_439h/ directory.
#
# For independent mode, Pn (n>=2) starter = reference P(n-1) + new stub files from Pn.
#
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT_DIR="$(dirname "$SCRIPT_DIR")"
REPOS_DIR="$ROOT_DIR/439h"
PROJECTS_DIR="$ROOT_DIR/projects_439h"

# ─── Starter commit hashes (2nd commit in each repo) ────────────────────────
get_starter_commit() {
  case "$1" in
    1) echo "035b431" ;; 2) echo "ac0044c" ;; 3) echo "41d140d" ;; 4) echo "3421a35" ;;
    5) echo "5e3c75a" ;; 6) echo "427ec7d" ;; 7) echo "561dd60" ;;
  esac
}

get_qemu_timeout() {
  case "$1" in
    1) echo "1" ;; 6) echo "40" ;; *) echo "10" ;;
  esac
}

get_qemu_cmd() {
  case "$1" in
    1) echo "qemu-system-x86_64" ;; *) echo "qemu-system-i386" ;;
  esac
}

# ─── Helper functions ─────────────────────────────────────────────────────────

clean_artifacts() {
  local dir="$1"
  (
    cd "$dir"
    rm -rf .git .gitignore .vscode
    rm -f spamon in_docker Dockerfile runall.sh stress.cpp
    rm -f *.o *.d *.out *.diff *.result *.time *.err *.run *.raw *.vcd *.sig *.data
    rm -f *.failure *.kernel
    rm -rf build 2>/dev/null || true
    rm -rf kernel/build 2>/dev/null || true
    rm -rf all_tests all_results my_results my_submission 2>/dev/null || true
    rm -rf cs439t_f24_p*__tests* cs439c_f24_p*__tests* 2>/dev/null || true
    rm -rf tools 2>/dev/null || true
    rm -f *_data.d 2>/dev/null || true
  )
}

# Patch a top-level Makefile for benchmark use
patch_makefile() {
  local mf="$1"
  local P="$2"

  # Set TESTS_DIR to ./tests (top-level Makefile)
  sed -i 's|^TESTS_DIR ?=.*|TESTS_DIR ?= ./tests|' "$mf"
  # Remove commented-out TESTS_DIR lines
  sed -i '/^# TESTS_DIR/d' "$mf"

  # Remove QEMU_PREFER line so QEMU_CMD falls back to system binary
  sed -i '/^QEMU_PREFER/d' "$mf"

  # Remove -action panic=exit-failure (requires QEMU 7+, Docker has 6.2)
  sed -i 's| -action panic=exit-failure||' "$mf"
  # Fix QEMU_CMD to just use the system binary
  local qemu_cmd
  qemu_cmd=$(get_qemu_cmd "$P")
  sed -i "s|^QEMU_CMD ?=.*|QEMU_CMD ?= $qemu_cmd|" "$mf"

  # Remove ORIGIN_REPO and related git-config variables (avoid errors without git remote)
  # These are only used by remote targets, so safe to just leave them (they'll be empty)

  # For P5: fix .block_size dependency to use TESTS_DIR
  # P5 has: ${TEST_DATA} : %.data : Makefile %.block_size
  # Should be: ${TEST_DATA} : %.data : Makefile ${TESTS_DIR}/%.block_size
  sed -i 's|: Makefile %.block_size$|: Makefile ${TESTS_DIR}/%.block_size|' "$mf"
}

# Patch a kernel/Makefile for Docker GCC compatibility
patch_kernel_makefile() {
  local mf="$1"
  [ -f "$mf" ] || return 0

  # Remove UTCS_TOOLS reference
  sed -i '/^UTCS_TOOLS/d' "$mf"

  # GCC 12+ treats volatile compound assignment as error with -Werror.
  # Add -Wno-volatile to CCFLAGS for compatibility.
  sed -i 's|-Wall -Werror -mno-sse|-Wall -Werror -Wno-volatile -mno-sse|' "$mf"

  # Fix g++ linking to use ld directly (avoids "PHDR segment not covered by
  # LOAD segment" error with binutils 2.38+ and bare linker scripts).
  # P5+ Makefiles use g++ for linking to get -lgcc; we switch to ld + explicit libgcc.
  sed -i 's|g++ ${CCFLAGS} -nostdlib -T script\.ld -N -e start|ld -T script.ld -N -m elf_i386 -e start|' "$mf"
  sed -i 's|-lgcc|$(shell gcc -m32 --print-libgcc-file-name)|' "$mf"
}

# Construct independent-mode starter for Pn (n>=2):
# Base = reference P(n-1), overlay new/modified files from Pn starter commit
construct_starter() {
  local P="$1"
  local PREV_P="$((P - 1))"
  local PROJ_DIR="$PROJECTS_DIR/p$P"
  local PREV_REF="$PROJECTS_DIR/p$PREV_P/reference"
  local REPO="$REPOS_DIR/cs439t_f24_p${P}_anthonyw"
  local STARTER_COMMIT
  STARTER_COMMIT=$(get_starter_commit "$P")

  local STARTER_DIR="$PROJ_DIR/starter"
  local TMP_STARTER
  TMP_STARTER=$(mktemp -d)

  # Extract Pn starter commit to temp dir
  git -C "$REPO" archive "$STARTER_COMMIT" | tar -x -C "$TMP_STARTER"

  # Start with P(n-1) reference as base
  cp -r "$PREV_REF/." "$STARTER_DIR/"

  # Overlay files from Pn starter that are NEW or DIFFERENT from P(n-1) reference
  (
    cd "$TMP_STARTER"
    find . -type f | while read -r f; do
      f="${f#./}"
      if [ ! -f "$STARTER_DIR/$f" ]; then
        # New file — copy it in
        mkdir -p "$STARTER_DIR/$(dirname "$f")"
        cp "$TMP_STARTER/$f" "$STARTER_DIR/$f"
      elif ! cmp -s "$TMP_STARTER/$f" "$PREV_REF/$f" 2>/dev/null; then
        # File differs from P(n-1) reference — professor may have modified it
        # Use the starter version (which has stubs/modifications for Pn)
        cp "$TMP_STARTER/$f" "$STARTER_DIR/$f"
      fi
      # If identical to P(n-1) reference, keep reference version (already there)
    done
  )

  # Remove files that Pn starter DROPPED (exist in P(n-1) ref but not in Pn starter)
  (
    cd "$PREV_REF"
    find . -type f | while read -r f; do
      f="${f#./}"
      if [ ! -f "$TMP_STARTER/$f" ]; then
        rm -f "$STARTER_DIR/$f"
      fi
    done
  )

  # Use Pn reference Makefiles (they have correct source lists and build rules)
  cp "$PROJ_DIR/reference/Makefile" "$STARTER_DIR/Makefile"
  if [ -f "$PROJ_DIR/reference/kernel/Makefile" ]; then
    cp "$PROJ_DIR/reference/kernel/Makefile" "$STARTER_DIR/kernel/Makefile"
  fi

  rm -rf "$TMP_STARTER"

  # Clean test/artifact files from starter
  rm -f "$STARTER_DIR/"*.ok 2>/dev/null || true
  find "$STARTER_DIR" -maxdepth 1 -name "*.dir" -type d -exec rm -rf {} + 2>/dev/null || true
  rm -f "$STARTER_DIR/"*.block_size 2>/dev/null || true
  # Remove student test files (named t0, t1, etc. or anthonyw)
  rm -f "$STARTER_DIR/"t[0-9].cc "$STARTER_DIR/"t[0-9].ok 2>/dev/null || true
  rm -rf "$STARTER_DIR/anthonyw"* 2>/dev/null || true
}

# ─── Extract tests from a repo ───────────────────────────────────────────────

# P1-P4: .cc + .ok test files from an embedded __tests directory
extract_tests_cc_ok() {
  local TESTS_SRC="$1"
  local TESTS_DST="$2"
  mkdir -p "$TESTS_DST"
  for f in "$TESTS_SRC"/*.cc "$TESTS_SRC"/*.ok; do
    [ -f "$f" ] && cp "$f" "$TESTS_DST/"
  done
}

# P5-P6: .cc + .ok + .dir + .block_size files
extract_tests_with_data() {
  local TESTS_SRC="$1"
  local TESTS_DST="$2"
  mkdir -p "$TESTS_DST"
  for f in "$TESTS_SRC"/*.cc "$TESTS_SRC"/*.ok "$TESTS_SRC"/*.block_size; do
    [ -f "$f" ] && cp "$f" "$TESTS_DST/"
  done
  # Copy .dir directories (filesystem test data)
  for d in "$TESTS_SRC"/*.dir; do
    [ -d "$d" ] && cp -r "$d" "$TESTS_DST/"
  done
}

# P7-P8: .ok + .dir files (no .cc, no .block_size)
extract_tests_dir_ok() {
  local TESTS_SRC="$1"
  local TESTS_DST="$2"
  mkdir -p "$TESTS_DST"
  for f in "$TESTS_SRC"/*.ok; do
    [ -f "$f" ] && cp "$f" "$TESTS_DST/"
  done
  for d in "$TESTS_SRC"/*.dir; do
    [ -d "$d" ] && cp -r "$d" "$TESTS_DST/"
  done
}

# ─── Config writer ────────────────────────────────────────────────────────────
write_config() {
  local P="$1"
  local PROJ_DIR="$PROJECTS_DIR/p$P"
  cat > "$PROJ_DIR/config.json" << EOF
{
  "project": "p$P",
  "docker_image": "gheithbench-os:latest",
  "starter_commit": "$(get_starter_commit $P)",
  "repo": "cs439t_f24_p${P}_anthonyw",
  "qemu_timeout": $(get_qemu_timeout $P),
  "qemu_cmd": "$(get_qemu_cmd $P)"
}
EOF
}

# ─── P1: Critical Sections ───────────────────────────────────────────────────
extract_p1() {
  local P=1
  local PROJ_DIR="$PROJECTS_DIR/p$P"
  local REPO="$REPOS_DIR/cs439t_f24_p${P}_anthonyw"
  echo "=== Extracting P$P: Critical Sections ==="

  rm -rf "$PROJ_DIR/starter" "$PROJ_DIR/tests" "$PROJ_DIR/reference"
  mkdir -p "$PROJ_DIR"/{starter,tests,reference}

  # Starter
  git -C "$REPO" archive "$(get_starter_commit $P)" | tar -x -C "$PROJ_DIR/starter"
  clean_artifacts "$PROJ_DIR/starter"
  # Remove student test files from starter
  rm -f "$PROJ_DIR/starter/"t[0-9].cc "$PROJ_DIR/starter/"t[0-9].ok
  rm -f "$PROJ_DIR/starter/anthonyw"*

  # Reference
  git -C "$REPO" archive HEAD | tar -x -C "$PROJ_DIR/reference"
  clean_artifacts "$PROJ_DIR/reference"
  rm -f "$PROJ_DIR/reference/"t[0-9].cc "$PROJ_DIR/reference/"t[0-9].ok
  rm -f "$PROJ_DIR/reference/anthonyw"*

  # Tests — from embedded __tests dir
  local TESTS_SRC="$REPO/cs439t_f24_p${P}__tests"
  extract_tests_cc_ok "$TESTS_SRC" "$PROJ_DIR/tests"

  # Patch Makefiles
  for target in starter reference; do
    patch_makefile "$PROJ_DIR/$target/Makefile" "$P"
    patch_kernel_makefile "$PROJ_DIR/$target/kernel/Makefile"
  done

  write_config "$P"
  local test_count
  test_count=$(find "$PROJ_DIR/tests" -name "*.ok" | wc -l | tr -d ' ')
  echo "  → P$P: $test_count tests"
}

# ─── P2: Cooperative Threading ────────────────────────────────────────────────
extract_p2() {
  local P=2
  local PROJ_DIR="$PROJECTS_DIR/p$P"
  local REPO="$REPOS_DIR/cs439t_f24_p${P}_anthonyw"
  echo "=== Extracting P$P: Cooperative Threading ==="

  rm -rf "$PROJ_DIR/starter" "$PROJ_DIR/tests" "$PROJ_DIR/reference"
  mkdir -p "$PROJ_DIR"/{starter,tests,reference}

  # Reference
  git -C "$REPO" archive HEAD | tar -x -C "$PROJ_DIR/reference"
  clean_artifacts "$PROJ_DIR/reference"
  rm -f "$PROJ_DIR/reference/"t[0-9].cc "$PROJ_DIR/reference/"t[0-9].ok
  rm -f "$PROJ_DIR/reference/anthonyw"*

  # Starter (independent mode: P1 reference + P2 stubs)
  construct_starter "$P"

  # Tests
  local TESTS_SRC="$REPO/cs439t_f24_p2__tests_all"
  if [ ! -d "$TESTS_SRC" ]; then
    TESTS_SRC="$REPO/cs439t_f24_p${P}__tests"
  fi
  extract_tests_cc_ok "$TESTS_SRC" "$PROJ_DIR/tests"

  # Patch Makefiles
  for target in starter reference; do
    patch_makefile "$PROJ_DIR/$target/Makefile" "$P"
    patch_kernel_makefile "$PROJ_DIR/$target/kernel/Makefile"
  done

  write_config "$P"
  local test_count
  test_count=$(find "$PROJ_DIR/tests" -name "*.ok" | wc -l | tr -d ' ')
  echo "  → P$P: $test_count tests"
}

# ─── P3: Preemptive Threading ─────────────────────────────────────────────────
extract_p3() {
  local P=3
  local PROJ_DIR="$PROJECTS_DIR/p$P"
  local REPO="$REPOS_DIR/cs439t_f24_p${P}_anthonyw"
  echo "=== Extracting P$P: Preemptive Threading ==="

  rm -rf "$PROJ_DIR/starter" "$PROJ_DIR/tests" "$PROJ_DIR/reference"
  mkdir -p "$PROJ_DIR"/{starter,tests,reference}

  # Reference
  git -C "$REPO" archive HEAD | tar -x -C "$PROJ_DIR/reference"
  clean_artifacts "$PROJ_DIR/reference"
  rm -f "$PROJ_DIR/reference/"t[0-9].cc "$PROJ_DIR/reference/"t[0-9].ok
  rm -f "$PROJ_DIR/reference/anthonyw"*

  # Starter
  construct_starter "$P"

  # Tests
  extract_tests_cc_ok "$REPO/cs439t_f24_p${P}__tests" "$PROJ_DIR/tests"

  # Remove broken tests:
  # 028: lambda doesn't capture local var, passes const to non-const T& in bb.h
  # 035: narrowing conversion int32_t -> uint32_t in Barrier ctor with -Werror
  # 045: uses std::array without #include <array>, unavailable in freestanding kernel
  rm -f "$PROJ_DIR/tests/028.cc" "$PROJ_DIR/tests/028.ok"
  rm -f "$PROJ_DIR/tests/035.cc" "$PROJ_DIR/tests/035.ok"
  rm -f "$PROJ_DIR/tests/045.cc" "$PROJ_DIR/tests/045.ok"

  # Patch Makefiles
  for target in starter reference; do
    patch_makefile "$PROJ_DIR/$target/Makefile" "$P"
    patch_kernel_makefile "$PROJ_DIR/$target/kernel/Makefile"
  done

  write_config "$P"
  local test_count
  test_count=$(find "$PROJ_DIR/tests" -name "*.ok" | wc -l | tr -d ' ')
  echo "  → P$P: $test_count tests"
}

# ─── P4: Atomic Reference Counting ───────────────────────────────────────────
extract_p4() {
  local P=4
  local PROJ_DIR="$PROJECTS_DIR/p$P"
  local REPO="$REPOS_DIR/cs439t_f24_p${P}_anthonyw"
  echo "=== Extracting P$P: Atomic Reference Counting ==="

  rm -rf "$PROJ_DIR/starter" "$PROJ_DIR/tests" "$PROJ_DIR/reference"
  mkdir -p "$PROJ_DIR"/{starter,tests,reference}

  # Reference
  git -C "$REPO" archive HEAD | tar -x -C "$PROJ_DIR/reference"
  clean_artifacts "$PROJ_DIR/reference"
  rm -f "$PROJ_DIR/reference/"t[0-9].cc "$PROJ_DIR/reference/"t[0-9].ok
  rm -f "$PROJ_DIR/reference/anthonyw"*

  # Starter
  construct_starter "$P"

  # Tests — P4-specific tests only (not P2/P3 regression)
  extract_tests_cc_ok "$REPO/cs439t_f24_p${P}__tests" "$PROJ_DIR/tests"

  # Remove test 055: uses global StrongPtr which requires __cxa_atexit (unavailable in freestanding)
  rm -f "$PROJ_DIR/tests/055.cc" "$PROJ_DIR/tests/055.ok"

  # Patch Makefiles
  for target in starter reference; do
    patch_makefile "$PROJ_DIR/$target/Makefile" "$P"
    patch_kernel_makefile "$PROJ_DIR/$target/kernel/Makefile"
  done

  write_config "$P"
  local test_count
  test_count=$(find "$PROJ_DIR/tests" -name "*.ok" | wc -l | tr -d ' ')
  echo "  → P$P: $test_count tests"
}

# ─── P5: ext2 File System ────────────────────────────────────────────────────
extract_p5() {
  local P=5
  local PROJ_DIR="$PROJECTS_DIR/p$P"
  local REPO="$REPOS_DIR/cs439t_f24_p${P}_anthonyw"
  echo "=== Extracting P$P: ext2 File System ==="

  rm -rf "$PROJ_DIR/starter" "$PROJ_DIR/tests" "$PROJ_DIR/reference"
  mkdir -p "$PROJ_DIR"/{starter,tests,reference}

  # Reference
  git -C "$REPO" archive HEAD | tar -x -C "$PROJ_DIR/reference"
  clean_artifacts "$PROJ_DIR/reference"
  rm -f "$PROJ_DIR/reference/"t[0-9].cc "$PROJ_DIR/reference/"t[0-9].ok
  rm -rf "$PROJ_DIR/reference/anthonyw"*
  # Remove .dir directories and .block_size files
  find "$PROJ_DIR/reference" -maxdepth 1 -name "*.dir" -type d -exec rm -rf {} + 2>/dev/null || true
  rm -f "$PROJ_DIR/reference/"*.block_size

  # Starter
  construct_starter "$P"

  # Tests — .cc + .ok + .dir + .block_size
  extract_tests_with_data "$REPO/all_tests" "$PROJ_DIR/tests"

  # Patch Makefiles
  for target in starter reference; do
    patch_makefile "$PROJ_DIR/$target/Makefile" "$P"
    patch_kernel_makefile "$PROJ_DIR/$target/kernel/Makefile"
  done

  write_config "$P"
  local test_count
  test_count=$(find "$PROJ_DIR/tests" -name "*.ok" | wc -l | tr -d ' ')
  echo "  → P$P: $test_count tests"
}

# ─── P6: Virtual Memory ──────────────────────────────────────────────────────
extract_p6() {
  local P=6
  local PROJ_DIR="$PROJECTS_DIR/p$P"
  local REPO="$REPOS_DIR/cs439t_f24_p${P}_anthonyw"
  echo "=== Extracting P$P: Virtual Memory ==="

  rm -rf "$PROJ_DIR/starter" "$PROJ_DIR/tests" "$PROJ_DIR/reference"
  mkdir -p "$PROJ_DIR"/{starter,tests,reference}

  # Reference
  git -C "$REPO" archive HEAD | tar -x -C "$PROJ_DIR/reference"
  clean_artifacts "$PROJ_DIR/reference"
  rm -f "$PROJ_DIR/reference/"t[0-9].cc "$PROJ_DIR/reference/"t[0-9].ok
  rm -rf "$PROJ_DIR/reference/anthonyw"*
  find "$PROJ_DIR/reference" -maxdepth 1 -name "*.dir" -type d -exec rm -rf {} + 2>/dev/null || true
  rm -f "$PROJ_DIR/reference/"*.block_size

  # Starter
  construct_starter "$P"

  # Tests — use the larger cs439t test set (53 tests) if available, else cs439c (26)
  local TESTS_SRC="$REPO/cs439t_f24_p6__tests"
  if [ ! -d "$TESTS_SRC" ]; then
    TESTS_SRC="$REPO/cs439c_f24_p6__tests"
  fi
  extract_tests_with_data "$TESTS_SRC" "$PROJ_DIR/tests"

  # Remove test 002: redefines rand() conflicting with stdlib.h in Docker GCC 12
  rm -f "$PROJ_DIR/tests/002.cc" "$PROJ_DIR/tests/002.ok"
  rm -f "$PROJ_DIR/tests/002.block_size"
  rm -rf "$PROJ_DIR/tests/002.dir"

  # Patch Makefiles
  for target in starter reference; do
    patch_makefile "$PROJ_DIR/$target/Makefile" "$P"
    patch_kernel_makefile "$PROJ_DIR/$target/kernel/Makefile"
  done

  write_config "$P"
  local test_count
  test_count=$(find "$PROJ_DIR/tests" -name "*.ok" | wc -l | tr -d ' ')
  echo "  → P$P: $test_count tests"
}

# ─── P7: User Mode & System Calls ────────────────────────────────────────────
extract_p7() {
  local P=7
  local PROJ_DIR="$PROJECTS_DIR/p$P"
  local REPO="$REPOS_DIR/cs439t_f24_p${P}_anthonyw"
  echo "=== Extracting P$P: User Mode & System Calls ==="

  rm -rf "$PROJ_DIR/starter" "$PROJ_DIR/tests" "$PROJ_DIR/reference"
  mkdir -p "$PROJ_DIR"/{starter,tests,reference}

  # Reference
  git -C "$REPO" archive HEAD | tar -x -C "$PROJ_DIR/reference"
  clean_artifacts "$PROJ_DIR/reference"
  rm -rf "$PROJ_DIR/reference/anthonyw"*
  find "$PROJ_DIR/reference" -maxdepth 1 -name "*.dir" -type d -exec rm -rf {} + 2>/dev/null || true
  rm -f "$PROJ_DIR/reference/"t[0-9].ok "$PROJ_DIR/reference/"*.ok

  # Starter
  construct_starter "$P"

  # Tests — .ok + .dir from embedded test directory
  extract_tests_dir_ok "$REPO/cs439t_f24_p${P}__tests" "$PROJ_DIR/tests"

  # Patch Makefiles
  for target in starter reference; do
    patch_makefile "$PROJ_DIR/$target/Makefile" "$P"
    patch_kernel_makefile "$PROJ_DIR/$target/kernel/Makefile"
  done

  write_config "$P"
  local test_count
  test_count=$(find "$PROJ_DIR/tests" -name "*.ok" | wc -l | tr -d ' ')
  echo "  → P$P: $test_count tests"
}

# ─── Main ────────────────────────────────────────────────────────────────────
echo "Preparing 439H projects from $REPOS_DIR → $PROJECTS_DIR"
echo ""

mkdir -p "$PROJECTS_DIR"

# Must extract in order since Pn depends on P(n-1) reference
extract_p1
extract_p2
extract_p3
extract_p4
extract_p5
extract_p6
extract_p7

echo ""
echo "=== Summary ==="
for p in 1 2 3 4 5 6 7; do
  dir="$PROJECTS_DIR/p$p"
  starter_files=$(find "$dir/starter" -type f 2>/dev/null | wc -l | tr -d ' ')
  ref_files=$(find "$dir/reference" -type f 2>/dev/null | wc -l | tr -d ' ')
  test_files=$(find "$dir/tests" -type f 2>/dev/null | wc -l | tr -d ' ')
  echo "  p$p: starter=$starter_files files, reference=$ref_files files, tests=$test_files files"
done

echo ""
echo "Done. Projects extracted to $PROJECTS_DIR"
