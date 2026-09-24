# Thin wrapper around CMake so the common case is one command.
#
#   make            configure + build + run
#   make build      configure + build only
#   make debug      same as `make run` but a Debug build in build-debug/
#   make tsan       ThreadSanitizer build + run (GCC/Clang)
#   make clean      delete the build directory
#   make distclean  delete every build directory (release, debug, tsan)
#   make rebuild    clean + build from scratch
#
# Pass program arguments with ARGS:   make run ARGS="4 100"

# Silence the recursive-make directory chatter from the generated build system.
MAKEFLAGS += --no-print-directory

BUILD_DIR  ?= build
BUILD_TYPE ?= Release
TARGET     ?= project1
JOBS       ?= $(shell nproc 2>/dev/null || echo 4)
CMAKE_ARGS ?=
ARGS       ?=
RUNNER     ?=

# WSL randomizes mmap too aggressively for ThreadSanitizer, so run it without ASLR.
TSAN_RUNNER := $(shell command -v setarch >/dev/null 2>&1 && echo "setarch -R")

CACHE := $(BUILD_DIR)/CMakeCache.txt

# Multi-config generators (MSVC) nest the binary under a config folder.
BIN = $(firstword $(wildcard $(BUILD_DIR)/bin/$(TARGET) \
                             $(BUILD_DIR)/bin/$(TARGET).exe \
                             $(BUILD_DIR)/bin/$(BUILD_TYPE)/$(TARGET).exe))

.PHONY: all run build configure clean distclean rebuild debug tsan help
.DEFAULT_GOAL := run

all: run

## run: build then execute the binary
run: build
	@echo "--- running $(TARGET) ---"
	@$(RUNNER) $(BIN) $(ARGS)

## build: configure if needed, then compile
build: configure
	@cmake --build $(BUILD_DIR) --config $(BUILD_TYPE) -j $(JOBS)

## configure: run cmake only when the cache is missing or CMakeLists.txt changed
configure: $(CACHE)

$(CACHE): CMakeLists.txt
	@cmake -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) $(CMAKE_ARGS)

## debug: build + run with debug info and no optimization
debug:
	@$(MAKE) run BUILD_DIR=build-debug BUILD_TYPE=Debug

## tsan: build + run under ThreadSanitizer to catch data races
tsan:
	@$(MAKE) run BUILD_DIR=build-tsan BUILD_TYPE=Debug CMAKE_ARGS=-DENABLE_SANITIZERS=ON RUNNER="$(TSAN_RUNNER)"

## clean: remove the build directory
clean:
	@rm -rf $(BUILD_DIR)

## distclean: remove every build directory (release, debug, tsan)
distclean:
	@rm -rf build build-debug build-tsan

## rebuild: clean then build from scratch
rebuild: clean build

## help: list the available targets
help:
	@grep -E '^## ' $(MAKEFILE_LIST) | sed 's/^## /  make /'
