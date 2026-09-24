# Variant 4 — deferred print, divisibility division

The work split is identical to [variant 2](../variant2_immediate_divisibility/):
for each candidate `n` the divisor range `[2, √n]` is spread across the `x`
threads by stride, the threads are created once and reused, and a barrier keeps
them in step per candidate.

What changes is the output. Instead of printing, the last thread to reach the
barrier appends the result to a shared vector — the barrier already guarantees
that only one thread runs that code at a time, so the push needs no further
locking — and `main` prints the whole list after every thread has been joined.

Timestamps are taken when each prime was confirmed, so they still show the
concurrent search behind the serialized output.

## Run

From the repository root:

```bash
make v4
```

Or by hand, from this directory (the working directory matters — that is how
`config.txt` is found):

```bash
cd variant4_deferred_divisibility
../build/bin/variant4_deferred_divisibility
```

## Config

`config.txt`, one `key value` per line:

```
x       4     # threads
y       50    # search 2..y
delay   0     # optional ms slept per step
```

## What to look for

This is the one variant where the printed order is fully deterministic and
ascending **and** the timestamps are monotonic — the barrier serializes the
candidates, and the deferred print preserves that order. Only the thread id
column varies between runs, naming whichever thread finished its divisor slice
last for that candidate.

Compared with [variant 3](../variant3_deferred_range/), the same total work is
spread differently: variant 3 lets threads run ahead independently and pays for it
with an unbalanced finish, while this one keeps every thread on the same candidate
and pays per-candidate synchronisation instead.
