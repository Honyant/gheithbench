FROM gheithbench-base:latest
RUN apt-get update && apt-get install -y \
    gcc-multilib g++-multilib \
    qemu-system-x86 \
    e2fsprogs \
    python3 \
    && rm -rf /var/lib/apt/lists/*
