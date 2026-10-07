#pragma once

#include <atomic>
#include <thread>
#include "TaskQueue.h"

/**
 * A pool thread that tests chunks of divisors taken from a shared {@link TaskQueue}.
 *
 * Variant 4: deferred printing, divisibility division. Numbers are checked
 * one at a time. For each number, main pushes one chunk of odd divisors per
 * worker into the queue and waits for all of them. A worker that finds a
 * divisor clears {@link #shared_bool}. Workers never save or print anything;
 * main reads the flag and keeps the primes itself.
 *
 * The thread starts in the constructor and keeps taking tasks until the
 * queue is closed. The destructor joins it.
 */
class ThreadWorker {
    private:
        /** Milliseconds to sleep per step. Set from config.txt but not used in this variant. */
        inline static std::size_t DELAY;
        /** Stays true while no worker has found a divisor of the current number. */
        inline static std::atomic<bool> shared_bool = true;

        /** The queue this worker takes tasks from. Must be declared before {@link #t}. */
        TaskQueue& queue;
        /** This worker's thread. Started in the constructor, joined in the destructor. */
        std::thread t;

        /** Takes tasks from {@link #queue} until it is closed and empty. Runs on {@link #t}. */
        void run();

        /**
         * Tests the odd divisors task.start, task.start+2, ... up to task.end
         * against task.n. Stops early once any worker has found a divisor.
         * Always calls {@link TaskQueue#task_done} when finished.
         *
         * @param task the number and the chunk of divisors to test;
         *             if task.end is less than task.start, there is nothing to test
         */
        void doTask(const Task& task);

    public:
        /**
         * Creates a worker and starts its thread.
         *
         * @param queue the shared queue the worker takes tasks from
         */
        ThreadWorker(TaskQueue& queue);

        /** Waits for this worker's thread to finish. Close the queue first, or this waits forever. */
        ~ThreadWorker();

        ThreadWorker(const ThreadWorker&) = delete;             // a worker can't be copied
        ThreadWorker& operator=(const ThreadWorker&) = delete;

        /**
         * Sets the per-step delay.
         *
         * @param val the delay in milliseconds; 0 means no delay
         */
        static void set_delay(std::size_t val){
            DELAY = val;
        }

        /** Sets {@link #shared_bool} back to true. Call before testing the next number. */
        static void reset_bool(){
            shared_bool = true;
        }

        /**
         * Tells whether the current number is prime.
         * Read this only after {@link TaskQueue#wait_all} returns.
         *
         * @return true if no worker found a divisor of the current number
         */
        static bool is_prime(){
            return shared_bool;
        }
};