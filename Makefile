# Builds all four variants. Each variant is a single self-contained main.cpp.
#
#   make                 build every variant
#   make v1              build + run variant 1 (v1..v4)
#   make run             build + run all four, one after the other
#   make clean           delete the compiled binaries
#
# Each program reads the config.txt sitting next to it, so the run targets cd
# into the variant directory first.

CXX      ?= g++
CXXFLAGS ?= -std=c++20 -O2 -Wall -Wextra -Wpedantic -pthread

V1 := variant1_immediate_range
V2 := variant2_immediate_divisibility
V3 := variant3_deferred_range
V4 := variant4_deferred_divisibility
VARIANTS := $(V1) $(V2) $(V3) $(V4)
BINS := $(VARIANTS:%=%/prime)

.PHONY: all run clean v1 v2 v3 v4 help
.DEFAULT_GOAL := all

all: $(BINS)

%/prime: %/main.cpp
	@echo "--- compiling $* ---"
	@$(CXX) $(CXXFLAGS) $< -o $@

v1: $(V1)/prime ; @cd $(V1) && ./prime
v2: $(V2)/prime ; @cd $(V2) && ./prime
v3: $(V3)/prime ; @cd $(V3) && ./prime
v4: $(V4)/prime ; @cd $(V4) && ./prime

run: all
	@for v in $(VARIANTS); do \
	    echo "=== $$v ==="; \
	    (cd $$v && ./prime); \
	    echo; \
	done

clean:
	@rm -f $(BINS)

help:
	@grep -E '^#   ' Makefile | sed 's/^#   //'
