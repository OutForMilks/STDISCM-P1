#pragma once

#include <vector>
#include <string>
#include <mutex>
#include <atomic>


/**
 * A worker that tests one chunk of divisors for the current number.
 *
 * Variant 4: deferred printing, divisibility division. Numbers are checked
 * one at a time. For each number, every worker tests a different chunk of odd
 * divisors. The last worker to finish saves the number to a shared list if no
 * worker found a divisor, and main prints the list once every number is done.
 */
class ThreadWorker {
    private:
        /** This worker's thread ID, starting at 1. Saved with every prime found. */
        int id;
        /** The number every worker is currently testing. */
        inline static int n;

        /** Primes found so far, saved as ready-to-print lines. Guarded by {@link #mut}. */
        inline static std::vector<std::string> output;

        /** Total number of worker threads (x from config.txt). */
        inline static int N_THREADS;
        /** Milliseconds to sleep per step. Stored but not used yet. */
        inline static int DELAY;
        /** Guards {@link #output} so two threads cannot add to it at the same time. */
        inline static std::mutex mut;
        /** Stays true while no worker has found a divisor of {@link #n}. */
        inline static std::atomic<bool> shared_bool = true;
        /** How many workers have finished testing {@link #n}. */
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
         * Tests the odd divisors s, s+2, s+4, ... up to e against {@link #n}.
         * If this is the last worker to finish and no divisor was found,
         * it saves {@link #n} to {@link #output}. Nothing is printed here.
         *
         * @param s the first divisor to test (odd, inclusive)
         * @param e the last divisor to test (inclusive);
         *          if e is less than s, this worker has no divisors to test
         */
        void run(int s, int e);

        /**
         * Sets the number every worker will test next.
         *
         * @param val the number to test
         */
        static void set_n(int val){
            n = val;
        }

        /**
         * Sets the per-step delay.
         *
         * @param val the delay in milliseconds; 0 means no delay
         */
        static void set_delay(int val){
            DELAY = val;
        }

        /** Sets the finished-worker count back to 0. Call before testing the next number. */
        static void reset_count(){
            shared_counter = 0;
        }

        /** Sets {@link #shared_bool} back to true. Call before testing the next number. */
        static void reset_bool(){
            shared_bool = true;
        }

        /**
         * Prints every saved prime, in the order it was saved.
         * Call this only after every number has been tested and every thread joined.
         */
        static void print_output();
};