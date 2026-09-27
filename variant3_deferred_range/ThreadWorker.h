#pragma once

#include <vector>
#include <string>
#include <mutex>
#include <atomic>

/**
 * A worker that searches one contiguous slice of the number range for primes.
 *
 * Variant 3: deferred printing, straight range division. Each worker tests
 * every number in its own slice. Instead of printing, it saves each prime to a
 * shared list, and main prints the whole list after every thread has joined.
 */
class ThreadWorker {
    private:
        /** This worker's thread ID, starting at 1. Saved with every prime found. */
        int id;

        /** Primes found by all workers, saved as ready-to-print lines. Guarded by {@link #mut}. */
        inline static std::vector<std::string> output;

        /** Total number of worker threads (x from config.txt). */
        inline static int N_THREADS;
        /** Milliseconds to sleep after each number. */
        inline static int DELAY;
        /** Guards {@link #output} so two threads cannot add to it at the same time. */
        inline static std::mutex mut;
        /** How many workers have finished their slice. */
        inline static std::atomic<int> shared_counter = 0;

    public:
        /**
         * Creates a worker.
         *
         * @param i the thread ID for this worker, starting at 1
         * @param x the total number of worker threads
         */
        ThreadWorker(int i, int x);

        /**
         * Tests every number from s to e and saves each prime to {@link #output}.
         * Nothing is printed here.
         *
         * @param s the first number of this worker's slice (inclusive)
         * @param e the last number of this worker's slice (inclusive);
         *          if e is less than s, the slice is empty and nothing is tested
         */
        void run(int s, int e);

        /**
         * Sets how long every worker sleeps after testing each number.
         *
         * @param val the delay in milliseconds; 0 means no delay
         */
        static void set_delay(int val){
            DELAY = val;
        }

        /**
         * Prints every saved prime, in the order it was saved.
         * Call this only after every worker thread has been joined.
         */
        static void print_output();
};