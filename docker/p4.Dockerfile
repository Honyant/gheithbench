FROM gheithbench-base:latest
RUN apt-get update && apt-get install -y \
    gcc-aarch64-linux-gnu qemu-user-static \
    && rm -rf /var/lib/apt/lists/*
