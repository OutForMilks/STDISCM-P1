#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#include "ThreadWorker.h"
#include "../include/config.h"
#include "../include/timestamp.h"

/**
 * One thread's slice of the numbers to search.
 * If end is less than start, the slice is empty and the thread has nothing to do.
 * That happens when there are more threads than numbers.
 */
struct Range {
    /** The first number in the slice (inclusive). */
    std::size_t start;
    /** The last number in the slice (inclusive). */
    std::size_t end;
};

/**
 * Variant 1: immediate printing, straight range division.
 *
 * Reads x and y from config.txt, splits 1..y into x equal slices, and starts
 * one thread per slice. Each thread prints its primes as soon as it finds them.
 * Prints a timestamp when the run starts and when it ends.
 *
 * @return 0 when the run finishes
 */
int main() {

    const Config cfg = read_config("config.txt");
    ThreadWorker::set_delay(cfg.delay_ms);

    std::vector<ThreadWorker> workers;
    std::vector<std::thread> threads;

    const std::size_t base  = cfg.upper / cfg.threads;
    const std::size_t extra = cfg.upper % cfg.threads;

    std::vector<Range> ranges;
    std::size_t next = 1;
    for (std::size_t i = 0; i < cfg.threads; i++){
        const std::size_t size = base + (i < extra ? 1 : 0);
        ranges.push_back(Range{next, next + size - 1});
        next += size;
    }

    for (std::size_t i = 1; i <= cfg.threads; i++){
        workers.push_back(ThreadWorker(i));
    }
    auto s = std::chrono::steady_clock::now();
    std::cout << "[" << timestamp() << "] RUN START threads=" << cfg.threads << " limit=" << cfg.upper << std::endl;

    // for (std::size_t i = 0; i < cfg.threads; i++){
    //     std::cout << "Thread " << i+1 << " assigned [" << ranges[i].start << ", " << ranges[i].end << "]" << std::endl;
    // }

    for (std::size_t i = 0; i < cfg.threads; i++){
        const std::size_t start = ranges[i].start;
        const std::size_t end   = ranges[i].end;
        threads.push_back(std::thread([&workers, i, start, end]{
            workers[i].run(start, end);
        }));
    }

    for (std::size_t i = 0; i < cfg.threads; i++){
        threads[i].join();
    }
    threads.clear();
    auto e = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(e - s).count();
    std::cout << "[" << timestamp() << "] RUN END elapsed=" << elapsed << std::endl;

    return 0;
}
