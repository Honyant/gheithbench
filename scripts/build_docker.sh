#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT_DIR="$(dirname "$SCRIPT_DIR")"
DOCKER_DIR="$ROOT_DIR/docker"

# Use --platform linux/amd64 for production (Codex runs on x86).
# For local testing on ARM Mac, omit or set PLATFORM="" to build native.
PLATFORM="${GHEITHBENCH_PLATFORM:---platform linux/amd64}"

echo "=== Building gheithbench-base ==="
docker build $PLATFORM \
  -t gheithbench-base:latest \
  -f "$DOCKER_DIR/base.Dockerfile" \
  "$DOCKER_DIR"

echo "=== Building gheithbench-p4 ==="
docker build $PLATFORM \
  -t gheithbench-p4:latest \
  -f "$DOCKER_DIR/p4.Dockerfile" \
  "$DOCKER_DIR"

echo "=== Building gheithbench-verilog ==="
docker build $PLATFORM \
  -t gheithbench-verilog:latest \
  -f "$DOCKER_DIR/verilog.Dockerfile" \
  "$DOCKER_DIR"

echo "=== Building gheithbench-os ==="
docker build $PLATFORM \
  -t gheithbench-os:latest \
  -f "$DOCKER_DIR/os.Dockerfile" \
  "$DOCKER_DIR"

echo ""
echo "=== Smoke tests ==="
echo -n "base: "
docker run --rm $PLATFORM gheithbench-base:latest \
  bash -c 'gcc --version | head -1 && /usr/bin/time --version 2>&1 | head -1'

echo -n "p4: "
docker run --rm $PLATFORM gheithbench-p4:latest \
  bash -c 'aarch64-linux-gnu-gcc --version | head -1 && qemu-aarch64-static --version | head -1'

echo -n "verilog: "
docker run --rm $PLATFORM gheithbench-verilog:latest \
  bash -c 'iverilog -V 2>&1 | head -1'

echo -n "os: "
docker run --rm $PLATFORM gheithbench-os:latest \
  bash -c 'gcc -m32 -dumpversion && qemu-system-i386 --version | head -1 && mkfs.ext2 -V 2>&1 | head -1'

echo ""
echo "All images built and verified successfully."
