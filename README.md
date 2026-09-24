# STDISCM Project 1 — Threaded Prime Search

Four variants of the same problem: find every prime in `[2, y]` using `x` threads.
They differ along two axes, giving the 2×2 matrix below.

|                    | print **immediately** | print **deferred** |
| ------------------ | --------------------- | ------------------ |
| divide by **range**        | [variant1_immediate_range/](variant1_immediate_range/) | [variant3_deferred_range/](variant3_deferred_range/) |
| divide by **divisibility** | [variant2_immediate_divisibility/](variant2_immediate_divisibility/) | [variant4_deferred_divisibility/](variant4_deferred_divisibility/) |

**Range division** gives each thread a contiguous slice of `[2, y]` to test on its
own. **Divisibility division** puts all threads on one candidate at a time and
splits the divisor range `[2, √n]` among them.

**Immediate** printing means the thread that settles a number prints it right
then, so the output order is the real interleaving. **Deferred** printing means
findings are collected and printed by the main thread after every worker has been
joined — the timestamps are still taken at discovery time, so they show the
concurrency behind the serialized output.

## Layout

```
Makefile                        CMake wrapper; plain `make` asks which variant to run
CMakeLists.txt                  one executable per variant
scripts/pick-variant.sh         the "which variant?" prompt
include/                        pieces shared by all variants (config, timestamp)
variant1_immediate_range/       main.cpp + config.txt + README.md
variant2_immediate_divisibility/
variant3_deferred_range/
variant4_deferred_divisibility/
```

Each variant owns its `config.txt`, so the four can be configured independently.

## Build & run

```bash
make
```

That configures CMake, asks which variant you want, builds only that one, and
runs it from its own directory:

```
Which variant do you want to run?
  1) variant1_immediate_range           immediate range
  2) variant2_immediate_divisibility    immediate divisibility
  3) variant3_deferred_range            deferred range
  4) variant4_deferred_divisibility     deferred divisibility
choice [1-4]:
```

Skip the question by naming the variant up front:

```bash
make VARIANT=2      # or: make v2
```

## Targets

| Command | What it does |
| --- | --- |
| `make` / `make run` | ask, then build + run the chosen variant (Release) |
| `make VARIANT=n` / `make vn` | same, without the question |
| `make build` | build all four, run nothing |
| `make debug` | ask, then Debug build in `build-debug/` and run |
| `make tsan` | ask, then ThreadSanitizer build in `build-tsan/` and run — catches data races |
| `make list` | list the variants and their numbers |
| `make clean` | delete `build/` |
| `make distclean` | delete every build directory |
| `make help` | show this list |

## Without the wrapper

Plain CMake works too; the prompt is the only thing it lacks.

```bash
cmake -B build
cmake --build build --target run2    # builds variant 2 and runs it (run1..run4)
```

The prompt lives in the `Makefile` rather than in `CMakeLists.txt` on purpose:
IDEs and CI run CMake's configure step non-interactively, and a question asked
there would hang them. `run1`..`run4` set the working directory to the variant's
own folder, which is what makes its `config.txt` resolve.

To run a binary by hand, `cd` into the variant first for the same reason:

```bash
cd variant2_immediate_divisibility && ../build/bin/variant2_immediate_divisibility
```

## Configuration

Every variant reads the `config.txt` next to it — one `key value` per line:

| Key | Meaning |
| --- | --- |
| `x` | number of threads |
| `y` | upper bound; primes are searched in `2..y` |
| `delay` | optional, milliseconds slept per step (default `0`) — stretches the run so the interleaving is easy to read |

## Adding files to a variant

Just drop the `.cpp` in the variant's folder. `CMakeLists.txt` globs each
variant's sources with `CONFIGURE_DEPENDS`, so no edit is needed; headers in
`include/` and in the variant's own folder are already on the include path.
Files with `copy` in the name (`main copy.cpp`) are skipped, since those scratch
copies usually hold a second `main()`.

## Slides

`slides.pdf` is not in the repo — drop the exported deck in at the top level.
