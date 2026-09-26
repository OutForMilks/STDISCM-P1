# Variant 4 — deferred print, divisibility division

Same work split as [variant 2](../variant2_immediate_divisibility/): the threads
do not own a slice of the search range. `main` walks the candidates **one at a
time**, and for each `n` it splits the odd divisors `3, 5, 7, …, √n` into `x`
contiguous chunks, one per thread. Evens are skipped up front, so only odd
divisors need testing. If any thread finds a divisor it clears a shared flag.

A fresh set of threads is started for every candidate and joined before `main`
moves on to `n + 1`. Each thread bumps a shared atomic counter when it is done;
the one that brings it to `x` knows every chunk has been tested, so it is the one
that records the verdict. What changes from variant 2 is **when the output
appears**: instead of printing, that thread appends the line to a shared results
vector under a mutex, and `main` prints the whole vector only after the last
candidate's threads have been joined.

The timestamp is taken **at the moment the prime is confirmed**, not at print
time, so the recorded times still show when each result was actually found.

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

Primes come out in **ascending order**, like variant 2 — the candidates are
settled one at a time, so the parallelism is inside a single primality test
rather than across the range. The thread id on each line names whichever thread
happened to finish its chunk last.

Nothing is printed until every candidate has been tested, so the output arrives
as one block just before `RUN END`. Compare the total `elapsed` with variant 2:
the search itself is identical, so the gap is the cost of printing while the
threads run versus printing once afterwards.

For small `n` the divisor range is shorter than the thread count, so most threads
get an empty chunk and exit straight away — the cost of creating and joining `x`
threads per candidate dominates the actual divisibility testing.
