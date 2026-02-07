#!/usr/bin/env bash
#
# prepare_projects.sh — Extract starter code, reference solutions, and tests
# from the CS429H git repos into the gheithbench/projects/ directory.
#
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT_DIR="$(dirname "$SCRIPT_DIR")"
REPOS_DIR="$ROOT_DIR/429h"
PROJECTS_DIR="$ROOT_DIR/projects"

# ─── Project metadata ────────────────────────────────────────────────────────
get_starter_commit() {
  case "$1" in
    1) echo "a984029" ;; 2) echo "a8287cc" ;; 3) echo "8592e0f" ;; 4) echo "e0f0ae4" ;;
    5) echo "11b7e5c" ;; 6) echo "8821dfd" ;; 7) echo "6f11376" ;; 8) echo "5bb2e6d" ;;
  esac
}

get_docker_image() {
  case "$1" in
    1|2|3|5|6) echo "gheithbench-base" ;;
    4) echo "gheithbench-p4" ;;
    7|8) echo "gheithbench-verilog" ;;
  esac
}

# ─── Helper functions ─────────────────────────────────────────────────────────

clean_artifacts() {
  local dir="$1"
  (
    cd "$dir"
    rm -rf .git .gitignore .vscode
    rm -f spamon in_docker Dockerfile temp.py setup.gtkwave.gtkw
    rm -f *.o *.d *.out *.diff *.result *.time *.err *.run *.raw *.cycles *.vcd *.sig
    rm -f build/*.o build/*.d 2>/dev/null || true
    rm -rf build 2>/dev/null || true
    rm -f src/*.o src/*.d 2>/dev/null || true
    rm -f heap/*.o heap/*.d 2>/dev/null || true
    # Remove student junk files
    rm -f memleakdetector.sh temp
    rm -rf "report test" test tests tests2
    rm -f output_log*.txt t0debug trashmain.cccc
    rm -rf cppcode aux
  )
}

strip_remote_targets() {
  # Remove everything from "######### remote things ##########" onwards
  local makefile="$1"
  if grep -q "remote things" "$makefile" 2>/dev/null; then
    local line_num
    line_num=$(grep -n "remote things" "$makefile" | head -1 | cut -d: -f1)
    # Remove from 2 lines before (the blank line) to end
    local start=$((line_num - 1))
    if [ "$start" -lt 1 ]; then start=1; fi
    head -n "$((start - 1))" "$makefile" > "$makefile.tmp"
    mv "$makefile.tmp" "$makefile"
  fi
}

# ─── P1: Fun interpreter (C port) ────────────────────────────────────────────
extract_p1() {
  local P=1
  local PROJ_DIR="$PROJECTS_DIR/p$P"
  local REPO="$REPOS_DIR/cs429h_s24_p${P}_anthonyw"
  local TESTS_REPO="$REPOS_DIR/cs429h_s24_p${P}__tests"
  echo "=== Extracting P$P ==="

  rm -rf "$PROJ_DIR/starter" "$PROJ_DIR/tests" "$PROJ_DIR/reference"
  mkdir -p "$PROJ_DIR"/{starter,tests,reference}

  # Starter code
  git -C "$REPO" archive "$(get_starter_commit $P)" | tar -x -C "$PROJ_DIR/starter"
  clean_artifacts "$PROJ_DIR/starter"
  # Remove starter test files (will use external tests)
  rm -f "$PROJ_DIR/starter/"*.fun "$PROJ_DIR/starter/"*.ok

  # Reference solution
  git -C "$REPO" archive HEAD | tar -x -C "$PROJ_DIR/reference"
  clean_artifacts "$PROJ_DIR/reference"
  # Remove reference test files
  rm -f "$PROJ_DIR/reference/"*.fun "$PROJ_DIR/reference/"*.ok

  # Tests — from sibling __tests repo (just .fun + .ok files)
  for f in "$TESTS_REPO"/*.fun "$TESTS_REPO"/*.ok; do
    [ -f "$f" ] && cp "$f" "$PROJ_DIR/tests/"
  done

  # Patch Makefile — use HEAD version (has TEST_DIR), fix paths
  cp "$PROJ_DIR/reference/Makefile" "$PROJ_DIR/starter/Makefile"
  for target in starter reference; do
    local mf="$PROJ_DIR/$target/Makefile"
    # Patch TEST_DIR
    sed -i '' 's|TEST_DIR=\.\./cs429h_s24_p1__tests|TEST_DIR=./tests|' "$mf"
    # Fix time path: ensure /usr/bin/time is used everywhere
    # Handle: -/bin/time, /bin/time, -time (bare), time (bare)
    sed -i '' 's|/bin/time --quiet|/usr/bin/time --quiet|g' "$mf"
    # Handle bare "time" at start of recipe line (with optional make - prefix)
    perl -i -pe 's/^(\t-?)time --quiet/${1}\/usr\/bin\/time --quiet/' "$mf"
    # Strip remote targets
    strip_remote_targets "$mf"
  done

  # Config
  write_config "$P"
}

# ─── P2: Enhanced Fun interpreter ────────────────────────────────────────────
extract_p2() {
  local P=2
  local PROJ_DIR="$PROJECTS_DIR/p$P"
  local REPO="$REPOS_DIR/cs429h_s24_p${P}_anthonyw"
  local TESTS_REPO="$REPOS_DIR/cs429h_s24_p${P}__tests"
  echo "=== Extracting P$P ==="

  rm -rf "$PROJ_DIR/starter" "$PROJ_DIR/tests" "$PROJ_DIR/reference"
  mkdir -p "$PROJ_DIR"/{starter,tests,reference}

  # Starter
  git -C "$REPO" archive "$(get_starter_commit $P)" | tar -x -C "$PROJ_DIR/starter"
  clean_artifacts "$PROJ_DIR/starter"
  rm -f "$PROJ_DIR/starter/"*.fun "$PROJ_DIR/starter/"*.ok

  # Reference
  git -C "$REPO" archive HEAD | tar -x -C "$PROJ_DIR/reference"
  clean_artifacts "$PROJ_DIR/reference"
  rm -f "$PROJ_DIR/reference/"*.fun "$PROJ_DIR/reference/"*.ok

  # Tests
  for f in "$TESTS_REPO"/*.fun "$TESTS_REPO"/*.ok; do
    [ -f "$f" ] && cp "$f" "$PROJ_DIR/tests/"
  done

  # Patch Makefile — use HEAD version, fix paths
  cp "$PROJ_DIR/reference/Makefile" "$PROJ_DIR/starter/Makefile"
  for target in starter reference; do
    local mf="$PROJ_DIR/$target/Makefile"
    sed -i '' 's|TEST_DIR=\.\./cs429h_s24_p2__tests|TEST_DIR=./tests|' "$mf"
    # P2 HEAD uses bare "time" — fix to /usr/bin/time
    sed -i '' 's|/bin/time --quiet|/usr/bin/time --quiet|g' "$mf"
    perl -i -pe 's/^(\t-?)time --quiet/${1}\/usr\/bin\/time --quiet/' "$mf"
    # Remove -march=native (won't work under emulation)
    sed -i '' 's| -march=native||' "$mf"
    strip_remote_targets "$mf"
  done

  write_config "$P"
}

# ─── P3: AArch64 emulator ───────────────────────────────────────────────────
extract_p3() {
  local P=3
  local PROJ_DIR="$PROJECTS_DIR/p$P"
  local REPO="$REPOS_DIR/cs429h_s24_p${P}_anthonyw"
  local TESTS_REPO="$REPOS_DIR/cs429h_s24_p${P}__tests"
  echo "=== Extracting P$P ==="

  rm -rf "$PROJ_DIR/starter" "$PROJ_DIR/tests" "$PROJ_DIR/reference"
  mkdir -p "$PROJ_DIR"/{starter,tests,reference}

  # Starter
  git -C "$REPO" archive "$(get_starter_commit $P)" | tar -x -C "$PROJ_DIR/starter"
  clean_artifacts "$PROJ_DIR/starter"
  rm -f "$PROJ_DIR/starter/"*.arm "$PROJ_DIR/starter/"*.ok

  # Reference
  git -C "$REPO" archive HEAD | tar -x -C "$PROJ_DIR/reference"
  clean_artifacts "$PROJ_DIR/reference"
  rm -f "$PROJ_DIR/reference/"*.arm "$PROJ_DIR/reference/"*.ok
  # Remove student test assembly files
  rm -f "$PROJ_DIR/reference/"*.s

  # Fix GCC 11+ compatibility: add missing #include <cstdio> where printf is used
  for src in "$PROJ_DIR/reference/"*.cxx "$PROJ_DIR/reference/"*.h; do
    [ -f "$src" ] || continue
    if grep -q 'printf' "$src" && ! grep -q '#include.*cstdio\|#include.*stdio' "$src"; then
      perl -i -0pe 's/(#include\s*[<"][^>"]+[>"])/\1\n#include <cstdio>/' "$src"
    fi
  done

  # Tests (.arm + .ok files)
  for f in "$TESTS_REPO"/*.arm "$TESTS_REPO"/*.ok; do
    [ -f "$f" ] && cp "$f" "$PROJ_DIR/tests/"
  done

  # Patch Makefile
  cp "$PROJ_DIR/reference/Makefile" "$PROJ_DIR/starter/Makefile"
  for target in starter reference; do
    local mf="$PROJ_DIR/$target/Makefile"
    sed -i '' 's|TEST_DIR=\.\./cs429h_s24_p3__tests|TEST_DIR=./tests|' "$mf"
    sed -i '' 's|/bin/time --quiet|/usr/bin/time --quiet|g' "$mf"
    perl -i -pe 's/^(\t-?)time --quiet/${1}\/usr\/bin\/time --quiet/' "$mf"
    strip_remote_targets "$mf"
  done

  write_config "$P"
}

# ─── P4: Fun→ARM compiler ───────────────────────────────────────────────────
extract_p4() {
  local P=4
  local PROJ_DIR="$PROJECTS_DIR/p$P"
  local REPO="$REPOS_DIR/cs429h_s24_p${P}_anthonyw"
  local TESTS_REPO="$REPOS_DIR/cs429h_s24_p${P}__tests"
  echo "=== Extracting P$P ==="

  rm -rf "$PROJ_DIR/starter" "$PROJ_DIR/tests" "$PROJ_DIR/reference"
  mkdir -p "$PROJ_DIR"/{starter,tests,reference}

  # Starter
  git -C "$REPO" archive "$(get_starter_commit $P)" | tar -x -C "$PROJ_DIR/starter"
  clean_artifacts "$PROJ_DIR/starter"
  rm -f "$PROJ_DIR/starter/"*.fun "$PROJ_DIR/starter/"*.ok "$PROJ_DIR/starter/"*.args "$PROJ_DIR/starter/"*.s "$PROJ_DIR/starter/"*.arm

  # Reference
  git -C "$REPO" archive HEAD | tar -x -C "$PROJ_DIR/reference"
  clean_artifacts "$PROJ_DIR/reference"
  rm -f "$PROJ_DIR/reference/"*.fun "$PROJ_DIR/reference/"*.ok "$PROJ_DIR/reference/"*.args "$PROJ_DIR/reference/"*.s "$PROJ_DIR/reference/"*.arm

  # Tests (.fun + .ok + .args files)
  for f in "$TESTS_REPO"/*.fun "$TESTS_REPO"/*.ok "$TESTS_REPO"/*.args; do
    [ -f "$f" ] && cp "$f" "$PROJ_DIR/tests/"
  done

  # Patch Makefile
  cp "$PROJ_DIR/reference/Makefile" "$PROJ_DIR/starter/Makefile"
  for target in starter reference; do
    local mf="$PROJ_DIR/$target/Makefile"
    sed -i '' 's|TEST_DIR=\.\./cs429h_s24_p4__tests|TEST_DIR=./tests|' "$mf"
    # Replace UT-specific ARM tool paths
    sed -i '' "s|ARM_QEMU=~gheith/public/qemu_5.1.0_old/bin/qemu-aarch64|ARM_QEMU=qemu-aarch64-static|" "$mf"
    sed -i '' "s|ARM_GCC=~gheith/public/gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu/bin/aarch64-none-linux-gnu-gcc|ARM_GCC=aarch64-linux-gnu-gcc|" "$mf"
    sed -i '' 's|/bin/time --quiet|/usr/bin/time --quiet|g' "$mf"
    perl -i -pe 's/^(\t-?)time --quiet/${1}\/usr\/bin\/time --quiet/' "$mf"
    strip_remote_targets "$mf"
  done

  write_config "$P"
}

# ─── P5: Malloc implementation ───────────────────────────────────────────────
extract_p5() {
  local P=5
  local PROJ_DIR="$PROJECTS_DIR/p$P"
  local REPO="$REPOS_DIR/cs429h_s24_p${P}_anthonyw"
  local TESTS_EMBEDDED="$REPO/cs429h_s24_p5__tests"
  echo "=== Extracting P$P ==="

  rm -rf "$PROJ_DIR/starter" "$PROJ_DIR/tests" "$PROJ_DIR/reference"
  mkdir -p "$PROJ_DIR"/{starter,tests,reference}

  # P5 starter: heap/ dir + Makefile + t0.c/t0.ok
  git -C "$REPO" archive "$(get_starter_commit $P)" | tar -x -C "$PROJ_DIR/starter"
  clean_artifacts "$PROJ_DIR/starter"

  # The Makefile is at project root (from the starter commit, which includes the
  # test Makefile). Actually the starter has its own Makefile. But we want the
  # test Makefile (from the embedded __tests dir) since it has the test harness.
  # Copy the test Makefile to root — it already globs *.c in cwd.
  cp "$TESTS_EMBEDDED/Makefile" "$PROJ_DIR/starter/Makefile"

  # Remove starter test files (keep heap/ and README)
  rm -f "$PROJ_DIR/starter/"*.c "$PROJ_DIR/starter/"*.ok

  # Reference solution: heap/ directory contains the malloc implementation
  git -C "$REPO" archive HEAD -- heap/ | tar -x -C "$PROJ_DIR/reference"
  cp "$TESTS_EMBEDDED/Makefile" "$PROJ_DIR/reference/Makefile"
  git -C "$REPO" show HEAD:README > "$PROJ_DIR/reference/README" 2>/dev/null || true

  # Tests: .c + .ok files from the embedded tests dir
  for f in "$TESTS_EMBEDDED"/*.c "$TESTS_EMBEDDED"/*.ok; do
    [ -f "$f" ] && cp "$f" "$PROJ_DIR/tests/"
  done

  # P5 layout: tests go flat alongside source (no TEST_DIR subdirectory).
  # The Makefile globs *.c in cwd. We also need heap/ in cwd.
  # At runtime, workspace = starter/ contents + tests/*.c,*.ok copied flat.

  # Patch Makefile — strip remote targets, ensure /usr/bin/time
  for target in starter reference; do
    local mf="$PROJ_DIR/$target/Makefile"
    strip_remote_targets "$mf"
  done

  # Remove -I ./ flags that reference nonexistent dirs
  # P5 Makefile already uses /usr/bin/time, no patching needed

  write_config "$P"
}

# ─── P6: C-routines (CSP) ───────────────────────────────────────────────────
extract_p6() {
  local P=6
  local PROJ_DIR="$PROJECTS_DIR/p$P"
  local REPO="$REPOS_DIR/cs429h_s24_p${P}_anthonyw"
  local TESTS_EMBEDDED="$REPO/cs429h_s24_p6__tests"
  echo "=== Extracting P$P ==="

  rm -rf "$PROJ_DIR/starter" "$PROJ_DIR/tests" "$PROJ_DIR/reference"
  mkdir -p "$PROJ_DIR"/{starter,tests,reference}

  # Starter
  git -C "$REPO" archive "$(get_starter_commit $P)" | tar -x -C "$PROJ_DIR/starter"
  clean_artifacts "$PROJ_DIR/starter"
  # Remove starter test files (t0-t7.c and .ok files)
  rm -f "$PROJ_DIR/starter/"t*.c "$PROJ_DIR/starter/"t*.ok
  rm -f "$PROJ_DIR/starter/"*.c "$PROJ_DIR/starter/"*.ok

  # Reference
  git -C "$REPO" archive HEAD | tar -x -C "$PROJ_DIR/reference"
  clean_artifacts "$PROJ_DIR/reference"
  rm -f "$PROJ_DIR/reference/"t*.c "$PROJ_DIR/reference/"t*.ok
  rm -f "$PROJ_DIR/reference/"anthonyw.c "$PROJ_DIR/reference/"anthonyw.ok
  # Remove embedded test dir from reference
  rm -rf "$PROJ_DIR/reference/cs429h_s24_p6__tests"

  # Tests (.c + .ok files)
  for f in "$TESTS_EMBEDDED"/*.c "$TESTS_EMBEDDED"/*.ok; do
    [ -f "$f" ] && cp "$f" "$PROJ_DIR/tests/"
  done

  # Patch Makefile — use HEAD version (has TEST_DIR) for both starter and reference
  cp "$PROJ_DIR/reference/Makefile" "$PROJ_DIR/starter/Makefile"
  for target in starter reference; do
    local mf="$PROJ_DIR/$target/Makefile"
    # Patch TEST_DIR
    sed -i '' 's|TEST_DIR = cs429h_s24_p6__tests|TEST_DIR = ./tests|' "$mf"
    strip_remote_targets "$mf"
  done

  write_config "$P"
}

# ─── P7: Pipelined CPU (Verilog) ────────────────────────────────────────────
extract_p7() {
  local P=7
  local PROJ_DIR="$PROJECTS_DIR/p$P"
  local REPO="$REPOS_DIR/cs429h_s24_p${P}_anthonyw"
  local TESTS_EMBEDDED="$REPO/cs429h_s24_p7__tests"
  echo "=== Extracting P$P ==="

  rm -rf "$PROJ_DIR/starter" "$PROJ_DIR/tests" "$PROJ_DIR/reference"
  mkdir -p "$PROJ_DIR"/{starter,tests,reference}

  # Starter
  git -C "$REPO" archive "$(get_starter_commit $P)" | tar -x -C "$PROJ_DIR/starter"
  clean_artifacts "$PROJ_DIR/starter"
  rm -f "$PROJ_DIR/starter/"*.hex "$PROJ_DIR/starter/"*.ok

  # Reference
  git -C "$REPO" archive HEAD | tar -x -C "$PROJ_DIR/reference"
  clean_artifacts "$PROJ_DIR/reference"
  rm -f "$PROJ_DIR/reference/"*.hex "$PROJ_DIR/reference/"*.ok "$PROJ_DIR/reference/"*.asm
  # Remove vvp build artifact
  rm -f "$PROJ_DIR/reference/main"
  # Remove embedded test dir
  rm -rf "$PROJ_DIR/reference/cs429h_s24_p7__tests"
  # Remove aux tools (not needed for testing)
  rm -rf "$PROJ_DIR/reference/aux"

  # Tests (.hex + .ok files, and .v files from test dir)
  for f in "$TESTS_EMBEDDED"/*.hex "$TESTS_EMBEDDED"/*.ok; do
    [ -f "$f" ] && cp "$f" "$PROJ_DIR/tests/"
  done
  # Also copy any .v files from test dir (test bench modules)
  for f in "$TESTS_EMBEDDED"/*.v; do
    [ -f "$f" ] && cp "$f" "$PROJ_DIR/tests/"
  done

  # Patch Makefile
  cp "$PROJ_DIR/reference/Makefile" "$PROJ_DIR/starter/Makefile"
  for target in starter reference; do
    local mf="$PROJ_DIR/$target/Makefile"
    sed -i '' 's|TEST_DIR=\./cs429h_s24_p7__tests|TEST_DIR=./tests|' "$mf"
    strip_remote_targets "$mf"
    # Remove assemble/disassemble/emulate targets
    sed -i '' '/^assemble:/,/^$/d' "$mf"
    sed -i '' '/^disassemble:/,/^$/d' "$mf"
    sed -i '' '/^emulate:/,/^$/d' "$mf"
  done

  write_config "$P"
}

# ─── P8: Enhanced pipelined CPU (Verilog) ────────────────────────────────────
extract_p8() {
  local P=8
  local PROJ_DIR="$PROJECTS_DIR/p$P"
  local REPO="$REPOS_DIR/cs429h_s24_p${P}_anthonyw"
  local TESTS_EMBEDDED="$REPO/cs429h_s24_p8__tests"
  echo "=== Extracting P$P ==="

  rm -rf "$PROJ_DIR/starter" "$PROJ_DIR/tests" "$PROJ_DIR/reference"
  mkdir -p "$PROJ_DIR"/{starter,tests,reference}

  # Starter
  git -C "$REPO" archive "$(get_starter_commit $P)" | tar -x -C "$PROJ_DIR/starter"
  clean_artifacts "$PROJ_DIR/starter"
  rm -f "$PROJ_DIR/starter/"*.hex "$PROJ_DIR/starter/"*.ok "$PROJ_DIR/starter/"*.asm

  # Reference
  git -C "$REPO" archive HEAD | tar -x -C "$PROJ_DIR/reference"
  clean_artifacts "$PROJ_DIR/reference"
  rm -f "$PROJ_DIR/reference/"*.hex "$PROJ_DIR/reference/"*.ok "$PROJ_DIR/reference/"*.asm
  rm -rf "$PROJ_DIR/reference/cs429h_s24_p8__tests"
  rm -rf "$PROJ_DIR/reference/cs429h_s24_p7__tests"
  rm -rf "$PROJ_DIR/reference/aux"

  # Tests (.hex + .ok files, and .v files from test dir)
  for f in "$TESTS_EMBEDDED"/*.hex "$TESTS_EMBEDDED"/*.ok; do
    [ -f "$f" ] && cp "$f" "$PROJ_DIR/tests/"
  done
  for f in "$TESTS_EMBEDDED"/*.v; do
    [ -f "$f" ] && cp "$f" "$PROJ_DIR/tests/"
  done

  # Patch Makefile — the committed Makefile references p7__tests (student error),
  # so we need to match both p7 and p8 patterns
  cp "$PROJ_DIR/reference/Makefile" "$PROJ_DIR/starter/Makefile"
  for target in starter reference; do
    local mf="$PROJ_DIR/$target/Makefile"
    sed -i '' 's|TEST_DIR=\./cs429h_s24_p[78]__tests|TEST_DIR=./tests|' "$mf"
    strip_remote_targets "$mf"
  done

  write_config "$P"
}

# ─── Config writer ───────────────────────────────────────────────────────────
write_config() {
  local P="$1"
  local PROJ_DIR="$PROJECTS_DIR/p$P"
  cat > "$PROJ_DIR/config.json" << EOF
{
  "project": "p$P",
  "docker_image": "$(get_docker_image $P):latest",
  "starter_commit": "$(get_starter_commit $P)",
  "repo": "cs429h_s24_p${P}_anthonyw",
  "test_layout": "$([ "$P" = "5" ] && echo "flat" || echo "tests_dir")"
}
EOF
}

# ─── Main ────────────────────────────────────────────────────────────────────
echo "Preparing all projects from $REPOS_DIR → $PROJECTS_DIR"
echo ""

extract_p1
extract_p2
extract_p3
extract_p4
extract_p5
extract_p6
extract_p7
extract_p8

echo ""
echo "=== Summary ==="
for p in 1 2 3 4 5 6 7 8; do
  dir="$PROJECTS_DIR/p$p"
  starter_files=$(find "$dir/starter" -type f | wc -l | tr -d ' ')
  ref_files=$(find "$dir/reference" -type f | wc -l | tr -d ' ')
  test_files=$(find "$dir/tests" -type f | wc -l | tr -d ' ')
  echo "  p$p: starter=$starter_files files, reference=$ref_files files, tests=$test_files files"
done

echo ""
echo "Done. Projects extracted to $PROJECTS_DIR"
