# STDISCM Project 1 — Threaded Prime Search

Four variants of the same problem: find every prime in `[2, y]` using `x` threads.
They differ along two axes, giving the 2×2 matrix below.

|                    | print **immediately** | print **deferred** |
| ------------------ | --------------------- | ------------------ |
| divide by **range**        | [variant1_immediate_range/](variant1_immediate_range/) | [variant3_deferred_range/](variant3_deferred_range/) |
| divide by **divisibility** | [variant2_immediate_divisibility/](variant2_immediate_divisibility/) | [variant4_deferred_divisibility/](variant4_deferred_divisibility/) |

**Range division** gives each thread a contiguous slice of `[1, y]` to test on its
own. **Divisibility division** puts all threads on one candidate at a time and
splits its odd divisors `3, 5, 7, …, √n` among them (even candidates above 2 are
skipped up front, so even divisors never need testing).

**Immediate** printing means the thread that settles a number prints it right
then, tagged with a timestamp and its thread id, so the output order is the real
interleaving. **Deferred** printing means the primes are collected and printed by
the main thread only after all the work is done. Deferred output is the bare
numbers, with no per-prime timestamp or thread id.

Every variant prints a timestamped `RUN START` line before the work begins and a
`RUN END` line with the elapsed time when it finishes.

## Layout

```
Makefile                        CMake wrapper; plain `make` asks which variant to run
CMakeLists.txt                  one executable per variant
scripts/pick-variant.sh         the "which variant?" prompt
include/                        pieces shared by all variants (config, timestamp)
variant1_immediate_range/       main.cpp + ThreadWorker.{h,cpp} + config.txt + README.md
variant2_immediate_divisibility/
variant3_deferred_range/
variant4_deferred_divisibility/ also Task.h + TaskQueue.{h,cpp} (thread pool)
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

## Without the wrapper

```bash
cmake -B build
cmake --build build --target run2    # builds variant 2 and runs it (run1..run4)
```

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
| `delay` | optional, milliseconds slept per step (default `5`, max `1000`) — stretches the run so the interleaving is easy to read. Only variants 1 and 3 use it. |
