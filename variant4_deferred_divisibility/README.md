# Variant 4 — deferred print, divisibility division

Same work split as [variant 2](../variant2_immediate_divisibility/): the threads
do not own a slice of the search range. `main` walks the candidates **one at a
time**, and for each `n` it splits the odd divisors `3, 5, 7, …, √n` into `x`
contiguous chunks, one per thread. Evens are skipped up front, so only odd
divisors need testing. If any thread finds a divisor it clears a shared flag.

Unlike variant 2, the threads are **not** recreated per candidate. They form a
**thread pool**: `x` workers are started once and stay alive for the whole run.

| File | Role |
| --- | --- |
| `Task.h` | one job: the number `n` and a chunk `start..end` of odd divisors |
| `TaskQueue.{h,cpp}` | the shared job queue; `main` pushes jobs and waits on them |
| `ThreadWorker.{h,cpp}` | a pool thread; takes jobs until the queue is closed |

For each candidate:

1. `main` resets the shared "is prime" flag and pushes one task per chunk.
2. Idle workers wake up, take a task each, and test their chunk. A worker that
   finds a divisor clears the flag, and every worker stops early once it is clear.
3. Each worker calls `task_done()`. `main` sleeps in `wait_all()` until all of
   that candidate's tasks are done, which replaces `join()` from variant 2.
4. `main` reads the flag and, if it is still set, adds `n` to its own list.

Only `main` ever touches the list of primes, so it needs no lock. After the last
candidate, `main` closes the queue (the workers exit their loop), destroys the
workers (each destructor joins its thread), and prints the whole list.

What changes from variant 2 is **when the output appears**: nothing is printed
until every candidate has been tested. Per the spec for deferred printing, each
prime is printed as the bare number, with no timestamp or thread id.

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
delay   0     # read, but not used by this variant
```

## What to look for

Primes come out in **ascending order**, like variant 2. The candidates are
settled one at a time, so the parallelism is inside a single primality test
rather than across the range.

Nothing is printed until every candidate has been tested, so the output arrives
as one block just before `RUN END`.

Compare the total `elapsed` with variant 2. Two things differ: variant 2 prints
while it runs, and it creates and joins `x` threads for every candidate. Here
the threads are created once, so the per-candidate cost is only a queue push and
a wait. For small `n` most chunks are empty, so in variant 2 that thread setup
cost dominates the actual divisibility testing.
