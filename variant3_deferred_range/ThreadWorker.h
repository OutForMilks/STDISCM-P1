#pragma once

#include <vector>
#include <mutex>

/**
 * A worker that searches one contiguous slice of the number range for primes.
 *
 * Variant 3: deferred printing, straight range division. Each worker tests
 * every number in its own slice. Instead of printing, it saves each prime to a
 * shared list, and main prints the whole list after every thread has joined.
 */
class ThreadWorker {
    private:
        /** Primes found by all workers. Owned by main and shared by every worker. Guarded by {@link #mut}. */
        std::vector<std::size_t>& output;

        /** Milliseconds to sleep after each number. */
        inline static std::size_t DELAY;
        /** Guards {@link #output} so two threads cannot add to it at the same time. */
        inline static std::mutex mut;

    public:
        /**
         * Creates a worker that saves its primes into a shared list.
         *
         * @param o the list owned by main that every worker adds its primes to
         */
        ThreadWorker(std::vector<std::size_t>& o);

        /**
         * Tests every number from s to e and adds each prime to {@link #output}.
         * Nothing is printed here; main prints the list after every thread joins.
         *
         * @param s the first number of this worker's slice (inclusive)
         * @param e the last number of this worker's slice (inclusive);
         *          if e is less than s, the slice is empty and nothing is tested
         */
        void run(std::size_t s, std::size_t e);

        /**
         * Sets how long every worker sleeps after testing each number.
         *
         * @param val the delay in milliseconds; 0 means no delay
         */
        static void set_delay(std::size_t val){
            DELAY = val;
        }
};