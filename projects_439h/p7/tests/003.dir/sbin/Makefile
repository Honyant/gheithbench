
# The list of binaries to build.
# These correspond to binary targets in cargo; each file in
# src/bin creates a binary with the corresponding name.
UTILS = $(basename $(notdir $(wildcard src/bin/*.rs)))

# If you want to install your own version of Rust (via rustup or some other means):
#  - The version being used here is 1.72.1
#  - You'll have to install the standard library source code
#    (if using rustup, run 'rustup component add rust-src')
# If Rust is on your path, this should just work; if not, you can
# add it to the RUST_INSTALL_PATH variable, like below:
# RUST_INSTALL_PATH = ~/.cargo
RUST_INSTALL_PATH = /u/gheith/public/rust-1.72.1
export PATH := ${RUST_INSTALL_PATH}/bin:${PATH}

# Check if the Rust version matches the expected one;
# By default, the lab machines have an older Rust version that doesn't work for this.
CARGO_VERSION = $(shell env PATH="$(RUST_INSTALL_PATH)/bin:$$PATH" cargo --version)
ifneq ($(CARGO_VERSION),cargo 1.72.1 (103a7ff2e 2023-08-15))
$(warning $(shell env printf '\x1b[0;1;31m')Unexpected Rust version $(CARGO_VERSION); things may break!$(shell env printf '\x1b[0m'))
endif

# Find a good place for the target directory by finding the root
# of the current git repository.  Otherwise, it will place it in
# the working directory (inside the test case) and cause mkfs.ext2
# to fail.  This places it in 'cs439h_f23_p5_csid/rust-target'
# (in the root of your project folder).
#
# You may want to clean this out at times, either by removing
# the directory or running 'make clean' within a test case,
# as it can get reasonably large.

MAKEFILE_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

PROJECT_ROOT := $(shell git rev-parse --show-toplevel)
ifeq ($(PROJECT_ROOT),)
$(warning $(shell env printf '\x1b[0;1;33m')Failed to find project root, putting target directory in the makefile's dir (this might break mkfs.ext2)$(shell env printf '\x1b[0m'))
PROJECT_ROOT := $(MAKEFILE_DIR)
endif

RELATIVE_TARGET_DIR = /rust-target
export CARGO_TARGET_DIR := $(PROJECT_ROOT)$(RELATIVE_TARGET_DIR)

# You normally shouldn't do this; it's hack to get unstable features
# on a reasonably accessible Rust version, like those you can get
# from your distribution's package manager.
# (The Rust compiler internally uses unstable features, so for a stable
# compiler to be able to compile itself, it must use this setting.)
export RUSTC_BOOTSTRAP=1

all : $(UTILS)

$(UTILS): Makefile RUST_SOURCE
	cargo build --release --bin $@
	cp -p $${CARGO_TARGET_DIR}/i686-unknown-none/release/$@ $@

clippy: RUST_SOURCE
	# Don't do this; hack to get unstable features on the lab machines' Rust version
	RUSTC_BOOTSTRAP=1 cargo clippy

clean ::
	cargo clean

# RUST_SOURCE is a "phony" target that is always rerun, and thus relies on Cargo's caching
# to avoid rebuilds
.PHONY: clean RUST_SOURCE clippy
