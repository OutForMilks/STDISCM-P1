#pragma once

#include <vector>
#include <string>
#include <mutex>
#include <atomic>


/**
 * A worker that searches one contiguous slice of the number range for primes.
 *
 * Variant 1: immediate printing, straight range division. Each worker tests
 * every number in its own slice and prints a prime the moment it finds it,
 * tagged with a timestamp and its thread ID.
 */
class ThreadWorker {
    private:
        /** This worker's thread ID, starting at 1. Shown on every printed line. */
        int id;

        /** Milliseconds to sleep after each number, so the interleaving is easier to see. */
        inline static int DELAY;
        /** Guards std::cout so two threads cannot mix their lines together. */
        inline static std::mutex mut;

    public:
        /**
         * Creates a worker.
         *
         * @param i the thread ID for this worker, starting at 1
         */
        ThreadWorker(int i);

        /**
         * Tests every number from s to e and prints each prime immediately.
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
};