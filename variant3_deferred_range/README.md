# Variant 3 — deferred print, range division

Same work split as [variant 1](../variant1_immediate_range/): the interval
`[2, y]` becomes `x` contiguous slices, one per thread. What changes is **when the
output appears**.

`main` owns one `std::vector<std::size_t>` of primes and hands every worker a
reference to it. Each thread adds a prime the moment it finds one, holding a
mutex for the `push_back`: `std::vector` is not thread-safe, so two threads
adding at once could lose primes or corrupt the vector. Nothing is printed while
the threads run. `main` prints the whole list only after every thread has been
joined. Once the joins are done there are no other threads left to race with, so
reading the list needs no lock.

Per the spec for deferred printing, each prime is printed as the bare number,
with no timestamp or thread id.

## Run

From the repository root:

```bash
make v3
```

Or by hand, from this directory (the working directory matters — that is how
`config.txt` is found):

```bash
cd variant3_deferred_range
../build/bin/variant3_deferred_range
```

## Config

`config.txt`, one `key value` per line:

```
x       4     # threads
y       50    # search 2..y
delay   0     # optional ms slept per step
```

## What to look for

The list is in **discovery order, not numeric order**. The threads run at the
same time and take turns adding to the shared vector, so primes from different
slices end up mixed together:

```
2
251
3
257
5
...
```

That mixing is the evidence of concurrency here, the same thing
[variant 1](../variant1_immediate_range/) shows through the order of its printed
lines.

Nothing is printed until every thread has finished, so total wall time is set by
the slowest thread — the one holding the top of the range.
