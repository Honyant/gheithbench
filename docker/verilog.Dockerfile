FROM gheithbench-base:latest
RUN apt-get update && apt-get install -y iverilog \
    && rm -rf /var/lib/apt/lists/*
