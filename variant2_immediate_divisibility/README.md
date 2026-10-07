# Variant 2 — immediate print, divisibility division

The threads do not own a slice of the search range. `main` walks the candidates
**one at a time**, and for each `n` it splits the odd divisors `3, 5, 7, …, √n`
into `x` contiguous chunks, one per thread. Evens are skipped up front, so only
odd divisors need testing. If any thread finds a divisor it clears a shared
atomic flag. Every thread checks the flag as it goes and stops testing its
chunk as soon as the flag is cleared.

A fresh set of threads is started for every candidate and joined before `main`
moves on to `n + 1`. When a thread finishes its chunk and the flag is still
set, it bumps a shared atomic counter. The one that brings it to `x` knows every chunk has
been tested, so it is the one that **prints immediately**, tagged with a
timestamp and its thread id. `main` resets the flag and the counter after the
joins, ready for the next candidate.

For each candidate `main` also prints an `INNER RUN START` / `INNER RUN END`
pair, so you can see the per-candidate cost.

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
delay   0     # read, but not used by this variant
```

## What to look for

Primes come out in **ascending order** here, unlike
[variant 1](../variant1_immediate_range/) — the barrier forces the group through
the candidates one at a time, so the parallelism is inside a single primality
test rather than across the range. The thread id on each line varies, though:
it names whichever thread happened to finish its divisor slice last.

The trade-off is visible in the timing. Creating and joining `x` threads for
every candidate is real overhead, and for small `n` the divisor range is shorter
than the thread count, so most threads get an empty chunk and exit straight
away. [Variant 4](../variant4_deferred_divisibility/) avoids that cost with a
thread pool.
