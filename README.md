# GheithBench

A benchmark for evaluating LLM coding agents on real university CS assignments from UT Austin's CS 429H (Computer Architecture) and CS 439H (Operating Systems) courses, taught by Professor Gheith.

## Overview

GheithBench measures how well AI coding agents can complete multi-file, systems-level programming assignments — from writing a C interpreter and building a Y86-64 processor in Verilog, to implementing an x86 OS kernel with virtual memory, filesystems, and user-mode processes.

Each benchmark run gives an agent:
- Starter code (the assignment skeleton students receive)
- A prompt describing what to implement
- An `./exec` wrapper that runs build/test commands inside a Docker container with the correct toolchain
- Access to the test suite

The agent writes code, compiles, and iterates until tests pass. Scores are computed automatically.

## Project Structure

```
gheithbench/
├── docker/                  # Dockerfiles for build environments
│   ├── base.Dockerfile      #   Ubuntu 22.04 + GCC/Make
│   ├── os.Dockerfile        #   + QEMU, e2fsprogs (for 439H)
│   ├── p4.Dockerfile        #   + Verilator (for 429H P4)
│   └── verilog.Dockerfile   #   + iverilog (for 429H P5-P9)
├── projects/                # CS 429H assignments (P1-P9)
│   └── p<N>/
│       ├── prompt.txt       #   Agent prompt
│       ├── starter/         #   Starter code given to agent
│       ├── reference/       #   Reference solution (for validation)
│       └── tests/           #   Test cases
├── projects_439h/           # CS 439H assignments (P1-P7)
│   └── p<N>/
│       ├── prompt.txt       #   Agent prompt
│       ├── starter/         #   Starter code
│       ├── reference/       #   Reference solution
│       └── tests/           #   Test cases
├── scripts/
│   ├── run_agent.sh         # Run agent on a single 429H project
│   ├── run_agent_439h.sh    # Run agent on a single 439H project
│   ├── run_benchmark.sh     # Run agent on all 429H projects
│   ├── run_benchmark_439h.sh# Run agent on all 439H projects
│   ├── run_sequential_439h.sh # Sequential mode: agent builds P1→P7 incrementally
│   ├── grade_439h.py        # Parallel reliability grader (runs tests N times)
│   ├── generate_prompt.sh   # Generate 429H prompts from repos
│   ├── generate_prompt_439h.sh # Generate 439H prompts from repos
│   ├── prepare_projects.sh  # Extract 429H project data from git repos
│   ├── prepare_projects_439h.sh # Extract 439H project data from git repos
│   ├── build_docker.sh      # Build Docker images
│   └── score.sh             # Single-run test scorer
└── README.md
```

## CS 429H Projects

| # | Topic | Language |
|---|-------|----------|
| P1 | Port a C++ FUN interpreter to C | C |
| P2 | Extend FUN interpreter (if/else, while, functions) | C |
| P3 | AArch64 (ARM64) instruction set emulator | C |
| P4 | FUN language → AArch64 compiler | C |
| P5 | malloc/free heap allocator | C |
| P6 | Cooperative threading (coroutines, context switch) | C, x86-64 asm |
| P7 | Pipelined 16-bit CPU | Verilog |
| P8 | Pipelined 16-bit CPU (optimize CPI) | Verilog |
| P9 | Pipelined 16-bit CPU (maximize performance) | Verilog |

## CS 439H Projects (x86 OS Kernel)

| # | Topic | Description |
|---|-------|-------------|
| P1 | Spinlocks | Critical sections, multi-core synchronization |
| P2 | Threads | Cooperative multi-threading, promises, barriers |
| P3 | Preemption | PIT interrupts, blocking locks, bounded buffers |
| P4 | Smart pointers | Atomic reference counting (StrongPtr/WeakPtr) |
| P5 | File systems | IDE driver, ext2 parser, symlinks |
| P6 | Virtual memory | Paging, page faults, mmap/munmap |
| P7 | User processes | ELF loader, syscalls, ring 0 ↔ ring 3 |

439H projects build incrementally — each project extends the kernel from the previous one.

## Supported Agents

- **Codex** (OpenAI) — default
- **Claude** (Anthropic)
- **Gemini** (Google)

## Quick Start

### Prerequisites

- Docker
- One of: `codex`, `claude`, or `gemini` CLI installed
- `socat` (Linux, for Docker socket proxying)

### Build Docker Images

```bash
./scripts/build_docker.sh
```

### Run a Single Project

```bash
# 429H (default agent: codex)
./scripts/run_agent.sh 3

# 439H
./scripts/run_agent_439h.sh 2

# With a different agent
AGENT=claude ./scripts/run_agent_439h.sh 5
```

### Run Full Benchmark

```bash
# All 429H projects
./scripts/run_benchmark.sh

# All 439H projects
./scripts/run_benchmark_439h.sh

# 439H sequential mode (agent builds on its own prior work)
./scripts/run_sequential_439h.sh
```

### Environment Variables

| Variable | Default | Description |
|----------|---------|-------------|
| `AGENT` | `codex` | Agent to use: `codex`, `claude`, `gemini` |
| `AGENT_MODEL` | *(agent default)* | Model override |
| `AGENT_REASONING` | `xhigh` | Reasoning effort (codex only) |
| `GHEITHBENCH_PLATFORM` | `--platform linux/amd64` | Docker platform flag |
| `GRADE_ITERATIONS` | `100` | Iterations for reliability grading |

## Grading

### 429H

Tests are run once. Score = passed / total.

### 439H

- **P1, P4**: Single-run scoring (pass/fail per test)
- **P2, P3, P5, P6, P7**: Reliability grading via `grade_439h.py` — each test is run 100 times in parallel. Per-test grades:
  - 0 failures → 100%
  - 1 failure → 50%
  - 2 failures → 25%
  - 3+ failures → 0%
  - Final score = mean of per-test grades

The reliability grading catches flaky concurrency bugs that single-run testing misses.

## How It Works

1. Starter code is copied to a temp workspace
2. A Docker container is started with the workspace mounted
3. An `./exec` script is placed in the workspace — it proxies commands into the container
4. The agent receives the prompt and works in the workspace, using `./exec` to compile and test
5. After the agent finishes, tests are re-run in a fresh container for clean scoring
6. Results (source code, test output, scores) are saved to `results/` or `results_439h/`

## License

For research and evaluation purposes.
