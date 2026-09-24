# Variant 3 — deferred print, range division

Same work split as [variant 1](../variant1_immediate_range/): the interval
`[2, y]` becomes `x` contiguous slices, one per thread. What changes is **when the
output appears**.

Each thread appends its findings to its own slot of a results vector — one slot
per thread, so no two threads ever touch the same memory and no lock is needed —
and `main` prints everything only after every thread has been joined. Once the
joins are done there are no other threads left to race with, so reading the
results needs no synchronisation either.

The timestamp is taken **at the moment the prime is found**, not at print time, so
the recorded times still show the threads running concurrently even though the
printing is strictly serial.

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

Output is grouped by thread, and **the timestamps are not monotonic** down the
page: thread 2's first find is stamped earlier than thread 1's last one, because
the threads were running at the same time. That non-monotonic column is the whole
point — it is the evidence of concurrency that immediate printing shows through
line ordering instead.

Nothing is printed until every thread has finished, so total wall time is set by
the slowest thread — the one holding the top of the range.
