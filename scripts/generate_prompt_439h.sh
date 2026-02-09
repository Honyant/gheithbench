#!/usr/bin/env bash
#
# generate_prompt_439h.sh — Generate prompt.txt for each 439H project.
# Usage: ./generate_prompt_439h.sh [project_number]
#   If no project number given, generates for all projects.
#
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT_DIR="$(dirname "$SCRIPT_DIR")"
PROJECTS_DIR="$ROOT_DIR/projects_439h"

get_task_summary() {
  case "$1" in
    1) echo "Implement critical sections using spinlocks for multi-core synchronization in a bare-metal x86 kernel. The critical() template function must ensure mutual exclusion: only one core executes the given work at a time. Handle recursive critical sections correctly." ;;
    2) echo "Implement cooperative multi-threading for an x86 kernel. Create a threading system with thread creation (thread<T>(T t)), blocking promises (Promise<T>), barriers, and context switching. Threads have 8KB stacks and yield cooperatively." ;;
    3) echo "Add preemptive scheduling to the threading system using PIT (Programmable Interval Timer) interrupts every 1ms. Implement blocking locks (BlockingLock), bounded buffers (BoundedBuffer<T>), and timer/sleep support. The PIT handler must be O(1) and run with interrupts disabled." ;;
    4) echo "Implement atomic reference counting (ARC) with StrongPtr<T> and WeakPtr<T> smart pointer templates. Control blocks track strong_count and weak_count with atomic operations. Objects are freed when strong_count reaches 0, control blocks freed when both reach 0." ;;
    5) echo "Implement an ext2 filesystem driver for the kernel. Write an IDE disk driver, block I/O abstraction, and ext2 parser that supports reading files, traversing directories, and following symlinks. The filesystem must be thread-safe." ;;
    6) echo "Implement virtual memory management with x86 paging. Write a physical frame allocator, page directory/table management, page fault handler, and naive_mmap/naive_unmap for memory-mapped regions. Each thread gets its own virtual address space." ;;
    7) echo "Add user-mode process support to the kernel. Implement an ELF loader, system call dispatcher (open, read, write, close, exit, fork, exec, wait, etc.), and privilege-level switching (ring 0 ↔ ring 3). The kernel boots /sbin/init from the ext2 filesystem." ;;
  esac
}

# Filter README content: remove dates, REPORT.txt mentions, "donate/contribute test case" lines
filter_readme() {
  sed -E \
    -e '/^Due:/,/^$/{ /^Due:/d; /^\s*(test|code)\s+[0-9]+\/[0-9]+\/[0-9]+/d; }' \
    -e '/Due date:/d' \
    -e '/^\s*test:?\s+[0-9]+\/[0-9]+\/[0-9]+/d' \
    -e '/^\s*code:?\s+[0-9]+\/[0-9]+\/[0-9]+/d' \
    -e '/REPORT\.txt/Id' \
    -e '/report questions/Id' \
    -e '/[Dd]onate a test case/d' \
    -e '/[Cc]ontribute a test case/d' \
    -e '/- [Dd]onate a test/d' \
    -e '/- [Cc]ontribute a test/d' \
    -e '/Answer the report/Id' \
    | sed -E '/^[[:space:]]*$/{ N; /^\n[[:space:]]*$/d; }'
}

get_instructions() {
  case "$1" in
    1)
      cat << 'INST'
## Instructions
1. Read and understand all provided source files in your workspace.
2. Implement the required functionality by modifying the kernel source files.
3. To compile and test, use the `./exec` wrapper which runs commands in a Linux
   environment with all necessary tools:
   - `./exec 'make'` — compile
   - `./exec 'make -s test'` — run all tests
4. You can edit files directly — they are shared with the build environment.
5. The kernel runs in QEMU (x86-64, 4 cores, 128MB RAM). Tests timeout after 1 second.
6. Focus on correctness first. Concurrency bugs are the main challenge.
7. Your primary goal is to pass ALL test cases. Partial solutions are not acceptable — strive to make every single test pass.
8. You may create your own test files (`.cc` and `.ok`) in the `tests/` directory to help debug your implementation.
INST
      ;;
    2|3)
      cat << 'INST'
## Instructions
1. Read and understand all provided source files in your workspace.
2. Implement the required functionality by modifying the kernel source files.
3. To compile and test, use the `./exec` wrapper which runs commands in a Linux
   environment with all necessary tools:
   - `./exec 'make'` — compile
   - `./exec 'make -s test'` — run all tests
4. You can edit files directly — they are shared with the build environment.
5. The kernel runs in QEMU (i386, 4 cores, 128MB RAM). Tests timeout after 10 seconds.
6. This is a freestanding kernel — no libc. Use only what's provided.
7. Focus on correctness first. Concurrency bugs and race conditions are the main challenge.
8. Your primary goal is to pass ALL test cases. Partial solutions are not acceptable — strive to make every single test pass.
9. You may create your own test files (`.cc` and `.ok`) in the `tests/` directory to help debug your implementation.
10. A grading tool is available at `./grade`. After building, run `./exec './grade -n 10'` to run each test 10 times and check for flaky concurrency bugs. Your final grade depends on test reliability — tests must pass consistently, not just once.
INST
      ;;
    4)
      cat << 'INST'
## Instructions
1. Read and understand all provided source files in your workspace.
2. Implement the required functionality by modifying the kernel source files.
3. To compile and test, use the `./exec` wrapper which runs commands in a Linux
   environment with all necessary tools:
   - `./exec 'make'` — compile
   - `./exec 'make -s test'` — run all tests
4. You can edit files directly — they are shared with the build environment.
5. The kernel runs in QEMU (i386, 4 cores, 128MB RAM). Tests timeout after 10 seconds.
6. This is a freestanding kernel — no libc. Use only what's provided.
7. Focus on correctness first. Concurrency bugs and race conditions are the main challenge.
8. Your primary goal is to pass ALL test cases. Partial solutions are not acceptable — strive to make every single test pass.
9. You may create your own test files (`.cc` and `.ok`) in the `tests/` directory to help debug your implementation.
INST
      ;;
    5)
      cat << 'INST'
## Instructions
1. Read and understand all provided source files in your workspace.
2. Implement the required functionality by modifying the kernel source files.
3. To compile and test, use the `./exec` wrapper which runs commands in a Linux
   environment with all necessary tools:
   - `./exec 'make'` — compile
   - `./exec 'make -s test'` — run all tests
4. You can edit files directly — they are shared with the build environment.
5. The kernel runs in QEMU (i386, 4 cores, 128MB RAM). Tests timeout after 10 seconds.
6. Tests mount an ext2 disk image as a second drive. The kernel must read from it.
7. This is a freestanding kernel — no libc. Use only what's provided.
8. Focus on correctness and thread safety.
9. Your primary goal is to pass ALL test cases. Partial solutions are not acceptable — strive to make every single test pass.
10. You may create your own test files (`.cc`, `.ok`, `.dir`, `.block_size`) in the `tests/` directory to help debug your implementation.
11. A grading tool is available at `./grade`. After building, run `./exec './grade -n 10'` to run each test 10 times and check for flaky concurrency bugs. Your final grade depends on test reliability — tests must pass consistently, not just once.
INST
      ;;
    6)
      cat << 'INST'
## Instructions
1. Read and understand all provided source files in your workspace.
2. Implement the required functionality by modifying the kernel source files.
3. To compile and test, use the `./exec` wrapper which runs commands in a Linux
   environment with all necessary tools:
   - `./exec 'make'` — compile
   - `./exec 'make -s test'` — run all tests
4. You can edit files directly — they are shared with the build environment.
5. The kernel runs in QEMU (i386, 4 cores, 128MB RAM). Tests timeout after 40 seconds.
6. Tests mount an ext2 disk image as a second drive.
7. This is a freestanding kernel — no libc. Use only what's provided.
8. Focus on correctness. Virtual memory bugs can be very subtle.
9. Your primary goal is to pass ALL test cases. Partial solutions are not acceptable — strive to make every single test pass.
10. You may create your own test files (`.cc`, `.ok`, `.dir`, `.block_size`) in the `tests/` directory to help debug your implementation.
11. A grading tool is available at `./grade`. After building, run `./exec './grade -n 10'` to run each test 10 times and check for flaky concurrency bugs. Your final grade depends on test reliability — tests must pass consistently, not just once.
INST
      ;;
    7)
      cat << 'INST'
## Instructions
1. Read and understand all provided source files in your workspace.
2. Implement the required functionality by modifying the kernel source files.
3. To compile and test, use the `./exec` wrapper which runs commands in a Linux
   environment with all necessary tools:
   - `./exec 'make'` — compile
   - `./exec 'make -s test'` — run all tests
4. You can edit files directly — they are shared with the build environment.
5. The kernel runs in QEMU (i386, 4 cores, 128MB RAM). Tests timeout after 10 seconds.
6. Tests provide an ext2 disk image containing /sbin/init (a user-mode ELF binary).
7. The kernel builds a single kernel.img used for all tests. Each test provides a different filesystem.
8. This is a freestanding kernel — no libc. Use only what's provided.
9. Focus on correctness. System call handling and privilege switching are the main challenges.
10. Your primary goal is to pass ALL test cases. Partial solutions are not acceptable — strive to make every single test pass.
11. You may create your own test files (`.ok` and `.dir` with `/sbin/init`) in the `tests/` directory to help debug your implementation.
12. A grading tool is available at `./grade`. After building, run `./exec './grade -n 10'` to run each test 10 times and check for flaky concurrency bugs. Your final grade depends on test reliability — tests must pass consistently, not just once.
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

  # Read and filter the README (remove dates, REPORT.txt mentions, donate test case)
  local README_CONTENT=""
  if [ -f "$PROJ_DIR/starter/README" ]; then
    README_CONTENT=$(cat "$PROJ_DIR/starter/README" | filter_readme)
  fi

  # List workspace files (starter), excluding REPORT.txt
  local FILE_LIST
  FILE_LIST=$(cd "$PROJ_DIR/starter" && find . -type f | sort | sed 's|^\./||' | grep -v '^REPORT\.txt$')

  # Count tests
  local TEST_COUNT
  TEST_COUNT=$(find "$PROJ_DIR/tests" -name "*.ok" -type f | wc -l | tr -d ' ')

  local INSTRUCTIONS
  INSTRUCTIONS=$(get_instructions "$P")

  cat > "$PROMPT_FILE" << PROMPT_EOF
# CS439H Project $P

## Task
$TASK_SUMMARY

## Assignment Specification
$README_CONTENT

## Workspace Files
The following files are in your workspace:
\`\`\`
$FILE_LIST
\`\`\`

There are $TEST_COUNT tests available. Tests are located in the \`tests/\` directory.

$INSTRUCTIONS

## Constraints
- Do NOT modify the Makefile or kernel/Makefile.
- Do NOT modify the README.
- Do NOT modify test files (.ok files, .dir directories).
- Do NOT modify kernel infrastructure files unless the README says you can:
  - Leave alone: kernel/machine.h, kernel/machine.S, kernel/script.ld
- You may create new source files in the kernel/ directory if needed.
- The code must compile without warnings (\`-Wall -Werror\` is enabled).
PROMPT_EOF

  echo "  → $PROMPT_FILE ($TEST_COUNT tests)"
}

# Main
if [ $# -ge 1 ]; then
  generate_one "$1"
else
  for p in 1 2 3 4 5 6 7; do
    generate_one "$p"
  done
fi
