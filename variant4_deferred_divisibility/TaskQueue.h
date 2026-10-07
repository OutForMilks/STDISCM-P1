#pragma once
#include "Task.h"
#include <condition_variable>
#include <mutex>
#include <queue>

/**
 * A thread-safe queue of {@link Task}s shared by main and the worker pool.
 *
 * main pushes tasks and waits for them with {@link #wait_all}. Workers take
 * tasks with {@link #pop} and report each one with {@link #task_done}.
 * Every field is guarded by {@link #mut}.
 */
class TaskQueue {
    private:
        /** Tasks waiting for a worker. */
        std::queue<Task> tasks;
        /** Guards every field in this class. */
        std::mutex mut;

        /** Set by {@link #close}. Once true, no more tasks can be pushed. */
        bool closed = false;
        /** Tasks pushed but not yet reported with {@link #task_done}. */
        std::size_t pending = 0;

        /** Wakes a worker when a task is pushed or the queue is closed. */
        std::condition_variable cv;
        /** Wakes main when {@link #pending} reaches 0. */
        std::condition_variable done_cv;

    public:
        /**
         * Adds a task and wakes one waiting worker.
         *
         * @param task the task to add
         * @throws std::runtime_error if the queue is already closed
         */
        void push(const Task& task);

        /**
         * Waits for a task and takes it off the queue.
         *
         * @param task filled with the next task when this returns true
         * @return true if a task was taken; false if the queue is closed and empty,
         *         which tells the worker to stop
         */
        bool pop(Task& task);

        /** Stops new tasks and wakes every waiting worker so they can exit. */
        void close();

        /** Marks one task as finished. Wakes main when the last one is done. */
        void task_done();

        /** Waits until every pushed task has been reported with {@link #task_done}. */
        void wait_all();
};