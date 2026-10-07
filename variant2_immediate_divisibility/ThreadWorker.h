#pragma once

#include <mutex>
#include <atomic>

/**
 * A worker that tests one chunk of divisors for the current number.
 *
 * Variant 2: immediate printing, divisibility division. Numbers are checked
 * one at a time. For each number, every worker tests a different chunk of odd
 * divisors. The last worker to finish prints the number right away if no
 * worker found a divisor.
 */
class ThreadWorker {
    private:
        /** This worker's thread ID, starting at 1. Shown on every printed line. */
        std::size_t id;
        /** The number every worker is currently testing. */
        inline static std::size_t n;

        /** Total number of worker threads (x from config.txt). */
        inline static std::size_t N_THREADS;
        /** Milliseconds to sleep per step. Stored but not used yet. */
        inline static std::size_t DELAY;
        /** Guards std::cout so two threads cannot mix their lines together. */
        inline static std::mutex mut;
        /** Stays true while no worker has found a divisor of {@link #n}. */
        inline static std::atomic<bool> shared_bool = true;
        /** How many workers have finished testing {@link #n}. */
        inline static std::atomic<std::size_t> shared_counter = 0;

    public:
        /**
         * Creates a worker.
         *
         * @param i the thread ID for this worker, starting at 1
         * @param x the total number of worker threads
         */
        ThreadWorker(std::size_t i, std::size_t x);

        /**
         * Tests the odd divisors s, s+2, s+4, ... up to e against {@link #n}.
         * If this is the last worker to finish and no divisor was found,
         * it prints {@link #n} as a prime.
         *
         * @param s the first divisor to test (odd, inclusive)
         * @param e the last divisor to test (inclusive);
         *          if e is less than s, this worker has no divisors to test
         */
        void run(std::size_t s, std::size_t e);

        /**
         * Sets the number every worker will test next.
         *
         * @param val the number to test
         */
        static void set_n(std::size_t val){
            n = val;
        }

        /**
         * Sets the per-step delay.
         *
         * @param val the delay in milliseconds; 0 means no delay
         */
        static void set_delay(std::size_t val){
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
};