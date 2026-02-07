#!/usr/bin/env bash
#
# generate_prompt.sh — Generate prompt.txt for each project.
# Usage: ./generate_prompt.sh [project_number]
#   If no project number given, generates for all projects.
#
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT_DIR="$(dirname "$SCRIPT_DIR")"
PROJECTS_DIR="$ROOT_DIR/projects"

get_task_summary() {
  case "$1" in
    1) echo "Port the provided C++ interpreter for a simple language (FUN) to C. The C++ reference implementation is provided — you must rewrite it in C while preserving identical behavior." ;;
    2) echo "Extend the FUN language interpreter with new features: comparison operators, if/else, while loops, function expressions, and function calls. Performance matters — the fastest correct solution wins." ;;
    3) echo "Implement an emulator for a subset of the AArch64 (ARM64) instruction set. Your emulator reads ELF binaries and executes them, producing output identical to real hardware." ;;
    4) echo "Implement a compiler from the FUN language to AArch64 assembly. Your compiler reads .fun source files and outputs .s assembly that is then cross-compiled and run under QEMU." ;;
    5) echo "Implement malloc() and free() in C. Your allocator must manage a heap using sbrk(), handle allocations of varying sizes, and correctly free memory for reuse." ;;
    6) echo "Implement cooperative threading (c-routines / coroutines) using x86-64 assembly for context switching. You need to implement stack allocation, context switching, and a simple scheduler." ;;
    7) echo "Complete a pipelined implementation of a simple 16-bit CPU in Verilog. The ISA includes sub, movl, movh, jz, jnz, js, jns, and ld instructions. Your design is graded on correctness and CPI." ;;
    8) echo "Complete a pipelined implementation of a simple 16-bit CPU in Verilog. The ISA includes sub, movl, movh, jz, jnz, js, jns, and ld instructions. Verify correctness and do anything to minimize cycles per instruction (without modifying the memory, clock, regs, or cycle counter). Performance is scored relative to hidden but optimized baselines: solutions in the top quartile score 90-100, median performance scores 75, and slow solutions score lower. Incorrect solutions score 0. It would be a good idea to first get a correct implementation, then optimize it for performance. A branch predictor may be helpful for performance." ;;
    9) echo "Complete a pipelined implementation of a simple 16-bit CPU in Verilog. The ISA includes sub, movl, movh, jz, jnz, js, jns, and ld instructions. Do whatever it takes to minimize cycles per instruction. Use check_performance.py and cycles_reference.json to see how your cycle counts compare to optimized baselines, and verify that these scripts work early on. Top quartile (score 90-100) requires excellent pipelining and hazard handling. A branch predictor is necessary." ;;
  esac
}

get_instructions() {
  case "$1" in
    1|2|3|5)
      cat << 'INST'
## Instructions
1. Read and understand all provided source files in your workspace.
2. Implement the required functionality by modifying the source files.
3. To compile and test, use the `./exec` wrapper which runs commands in a Linux
   environment with all necessary tools:
   - `./exec 'make'` — compile
   - `./exec 'make -s test'` — run all tests
4. You can edit files directly — they are shared with the build environment.
5. Focus on correctness first, then optimization.
INST
      ;;
    4)
      cat << 'INST'
## Instructions
1. Read and understand all provided source files in your workspace.
2. Implement the required functionality by modifying the source files.
3. To compile and test, use the `./exec` wrapper which runs commands in a Linux
   environment with all necessary tools:
   - `./exec 'make'` — compile
   - `./exec 'make -s test'` — run all tests
4. You can edit files directly — they are shared with the build environment.
5. `aarch64-linux-gnu-gcc` and `qemu-aarch64-static` are available in the build environment.
6. Focus on correctness first, then optimization.
INST
      ;;
    6)
      cat << 'INST'
## Instructions
1. Read and understand all provided source files in your workspace.
2. Implement the required functionality by modifying the source files.
3. To compile and test, use the `./exec` wrapper which runs commands in a Linux
   environment with all necessary tools:
   - `./exec 'make'` — compile
   - `./exec 'make -s test'` — run all tests
4. You can edit files directly — they are shared with the build environment.
5. This runs in a Linux x86-64 environment. Use GNU assembler (GAS) AT&T syntax for assembly.
6. Focus on correctness first, then optimization.
INST
      ;;
    7|8)
      cat << 'INST'
## Instructions
1. Read and understand all provided source files in your workspace.
2. Implement the required functionality by modifying the source files.
3. To compile and test, use the `./exec` wrapper which runs commands in a Linux
   environment with all necessary tools:
   - `./exec 'make'` — compile
   - `./exec 'make -s test'` — run all tests
   - `./exec './cpu'` — run the CPU binary
4. You can edit files directly — they are shared with the build environment.
5. `iverilog` is available in the build environment.
6. Focus on correctness first, then optimization.
INST
      ;;
    9)
      cat << 'INST'
## Instructions
1. Read and understand all provided source files in your workspace.
2. Implement the required functionality by modifying the source files.
3. To compile and test, use the `./exec` wrapper which runs commands in a Linux
   environment with all necessary tools:
   - `./exec 'make'` — compile
   - `./exec 'make -s test'` — run all tests
   - `./exec './cpu'` — run the CPU binary
   - `./exec 'python3 check_performance.py'` — check your cycle performance
4. You can edit files directly — they are shared with the build environment.
5. `iverilog` is available in the build environment.
6. Use check_performance.py after running tests to see how your cycles compare.
7. cycles_reference.json contains min/q1/median/q3/avg for each test from optimized implementations.
8. Focus on correctness first, then optimize for performance.
INST
      ;;
  esac
}

generate_one() {
  local P="$1"
  local PROJ_DIR="$PROJECTS_DIR/p$P"
  local PROMPT_FILE="$PROJ_DIR/prompt.txt"

  echo "Generating prompt for P$P ..."

  local TASK_SUMMARY
  TASK_SUMMARY=$(get_task_summary "$P")

  # Read the README
  local README_CONTENT=""
  if [ -f "$PROJ_DIR/starter/README" ]; then
    README_CONTENT=$(cat "$PROJ_DIR/starter/README")
  fi

  # List workspace files (starter)
  local FILE_LIST
  FILE_LIST=$(cd "$PROJ_DIR/starter" && find . -type f | sort | sed 's|^\./||')

  # Count tests
  local TEST_COUNT
  TEST_COUNT=$(find "$PROJ_DIR/tests" -name "*.ok" -type f | wc -l | tr -d ' ')

  local INSTRUCTIONS
  INSTRUCTIONS=$(get_instructions "$P")

  cat > "$PROMPT_FILE" << PROMPT_EOF
# CS429H Project $P

## Task
$TASK_SUMMARY

## Assignment Specification
$README_CONTENT

## Workspace Files
The following files are in your workspace:
\`\`\`
$FILE_LIST
\`\`\`

There are $TEST_COUNT tests available. Tests are located in the \`tests/\` directory (or flat in the workspace for P5).

$INSTRUCTIONS

## Constraints
- Do NOT modify the Makefile, README, or any test files (.ok files).
- Do NOT modify the REPORT.txt file.
- You may create new source files if needed, but prefer modifying existing ones.
- The code must compile without warnings (\`-Wall -Werror\` is enabled).
PROMPT_EOF

  echo "  → $PROMPT_FILE ($TEST_COUNT tests)"
}

# Main
if [ $# -ge 1 ]; then
  generate_one "$1"
else
  for p in 1 2 3 4 5 6 7 8 9; do
    generate_one "$p"
  done
fi
