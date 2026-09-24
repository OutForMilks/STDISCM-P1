# Variant 1 — immediate print, range division

Each of the `x` threads gets a contiguous slice of `[2, y]` and tests every number
in its slice on its own. When a thread finds a prime it **prints it immediately**,
tagged with a timestamp and its thread id, so the output order is the real
interleaving of the threads rather than numeric order.

`std::cout` must be held under a mutex for the duration of one line: each `<<` is
a separate call, so without the lock two threads can wedge their output into the
middle of each other's line.

## Run

From the repository root:

```bash
make v1
```

Or by hand, from this directory (the working directory matters — that is how
`config.txt` is found):

```bash
cd variant1_immediate_range
../build/bin/variant1_immediate_range
```

## Config

`config.txt`, one `key value` per line:

```
x       4     # threads
y       50    # search 2..y
delay   0     # optional ms slept per step; raise it to watch the interleaving
```

## What to look for

Timestamps climb monotonically but thread ids and numbers jump around: thread 3
can report 29 before thread 2 reports 17. Raising `delay` spreads the run out and
makes the interleaving obvious.

Threads finish at different times even with equal slice sizes — primality testing
gets more expensive as the numbers grow, so the thread holding the top of the
range does the most work. That imbalance is the point of contrast with
[variant 2](../variant2_immediate_divisibility/), which splits each candidate's
divisor range instead.
