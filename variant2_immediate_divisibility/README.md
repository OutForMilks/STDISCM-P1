# Variant 2 — immediate print, divisibility division

The threads do not own a slice of the search range. They work on **one candidate
at a time**, splitting the divisor range `[2, √n]` among themselves by stride:
thread 0 tests 2, 2+x, 2+2x, …, thread 1 tests 3, 3+x, … If any thread finds a
divisor it raises a shared flag.

The threads are created once and reused for every candidate; a barrier keeps them
in step. The last thread to reach the barrier knows every slice for `n` has been
tested, so it is the one that **prints immediately** — before the group moves on
to `n + 1` — and it also clears the flag for the next round. Doing the decision
inside the barrier's release means it happens exactly once per candidate, with no
extra locking.

## Run

From the repository root:

```bash
make v2
```

Or by hand, from this directory (the working directory matters — that is how
`config.txt` is found):

```bash
cd variant2_immediate_divisibility
../build/bin/variant2_immediate_divisibility
```

## Config

`config.txt`, one `key value` per line:

```
x       4     # threads
y       50    # search 2..y
delay   0     # optional ms slept per step; raise it to watch the interleaving
```

## What to look for

Primes come out in **ascending order** here, unlike
[variant 1](../variant1_immediate_range/) — the barrier forces the group through
the candidates one at a time, so the parallelism is inside a single primality
test rather than across the range. The thread id on each line varies, though:
it names whichever thread happened to finish its divisor slice last.

The trade-off is visible in the timing. Per candidate there is real
synchronisation overhead, and for small `n` the divisor range is shorter than the
thread count, so most threads have nothing to test and simply wait at the
barrier.
