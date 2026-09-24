# Thin wrapper around CMake. Plain `make` configures, asks which variant you
# want, builds that one, and runs it from its own directory so it picks up its
# own config.txt.
#
#   make                 ask which variant, then build + run it
#   make VARIANT=2       same without the question (1-4, or the folder name)
#   make v1 v2 v3 v4     shorthand for VARIANT=n
#   make build           build every variant, run nothing
#   make debug           ask, then build + run a Debug build in build-debug/
#   make tsan            ask, then build + run under ThreadSanitizer
#   make list            list the variants
#   make clean           delete the build directory
#   make distclean       delete every build directory
#   make help            show this list
#
# The prompt lives here rather than in CMakeLists.txt because IDEs and CI run
# CMake's configure step non-interactively, where a prompt would just hang.

MAKEFLAGS += --no-print-directory

BUILD_DIR  ?= build
BUILD_TYPE ?= Release
JOBS       ?= $(shell nproc 2>/dev/null || echo 4)
CMAKE_ARGS ?=
RUNNER     ?=
VARIANT    ?=

# WSL randomizes mmap too aggressively for ThreadSanitizer, so run it without ASLR.
TSAN_RUNNER := $(shell command -v setarch >/dev/null 2>&1 && echo "setarch -R")

CACHE := $(BUILD_DIR)/CMakeCache.txt
PICK  := scripts/pick-variant.sh

.PHONY: all run build configure clean distclean rebuild debug tsan list help v1 v2 v3 v4
.DEFAULT_GOAL := run

all: run

## run: ask which variant (unless VARIANT is set), then build and run it
run: configure
	@variant=$$(bash $(PICK) "$(VARIANT)") || exit 1; \
	cmake --build $(BUILD_DIR) --config $(BUILD_TYPE) --target $$variant -j $(JOBS) || exit 1; \
	bin="$(CURDIR)/$(BUILD_DIR)/bin/$$variant"; \
	[ -x "$$bin" ] || bin="$(CURDIR)/$(BUILD_DIR)/bin/$(BUILD_TYPE)/$$variant.exe"; \
	echo "--- running $$variant ---"; \
	cd "$$variant" && $(RUNNER) "$$bin"

v1: ; @$(MAKE) run VARIANT=1
v2: ; @$(MAKE) run VARIANT=2
v3: ; @$(MAKE) run VARIANT=3
v4: ; @$(MAKE) run VARIANT=4

## build: compile every variant without running anything
build: configure
	@cmake --build $(BUILD_DIR) --config $(BUILD_TYPE) -j $(JOBS)

## configure: run cmake only when the cache is missing or CMakeLists.txt changed
configure: $(CACHE)

$(CACHE): CMakeLists.txt
	@cmake -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) $(CMAKE_ARGS)

## debug: ask, then build + run with debug info and no optimization
debug:
	@$(MAKE) run BUILD_DIR=build-debug BUILD_TYPE=Debug VARIANT=$(VARIANT)

## tsan: ask, then build + run under ThreadSanitizer to catch data races
tsan:
	@$(MAKE) run BUILD_DIR=build-tsan BUILD_TYPE=Debug VARIANT=$(VARIANT) \
	    CMAKE_ARGS=-DENABLE_SANITIZERS=ON RUNNER="$(TSAN_RUNNER)"

## list: show the variants and their numbers
list:
	@i=1; for v in variant*/; do printf '  %d) %s\n' $$i "$${v%/}"; i=$$((i+1)); done

## clean: remove the build directory
clean:
	@rm -rf $(BUILD_DIR)

## distclean: remove every build directory
distclean:
	@rm -rf build build-debug build-tsan

## rebuild: clean then build everything from scratch
rebuild: clean build

## help: list the available targets
help:
	@grep -E '^#   ' Makefile | sed 's/^#   //'
