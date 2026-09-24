# STDISCM Project 1

Multithreaded C++20 project built with CMake.

## Layout

```
Makefile          convenience wrapper around CMake
CMakeLists.txt
include/          headers
src/main.cpp      entry point
```

## Build & run

One command — configures, compiles, and runs:

```bash
make
```

Binary lands in `build/bin/project1`. Re-running `make` only recompiles what changed.

## Targets

| Command | What it does |
| --- | --- |
| `make` / `make run` | configure + build + run (Release) |
| `make build` | configure + build, no run |
| `make debug` | Debug build in `build-debug/`, then run |
| `make tsan` | ThreadSanitizer build in `build-tsan/`, then run — catches data races |
| `make clean` | delete `build/` |
| `make distclean` | delete every build directory |
| `make rebuild` | clean, then build from scratch |
| `make help` | list targets |

Pass arguments to the program with `ARGS`:

```bash
make run ARGS="4 100"
```

Override the defaults if needed:

```bash
make BUILD_TYPE=RelWithDebInfo    # build type
make BUILD_DIR=out                # build directory
make JOBS=4                       # parallel compile jobs
```

## Without make

The wrapper is optional; plain CMake still works.

```bash
cmake -B build
cmake --build build -j
./build/bin/project1
```

## Adding files

Add new `.cpp` files to the `add_executable(...)` list in `CMakeLists.txt`.
Headers in `include/` are already on the include path.
