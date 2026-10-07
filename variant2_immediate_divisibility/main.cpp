#include <chrono>
#include <iostream>
#include <thread>
#include <vector>
#include <cmath>
#include "ThreadWorker.h"
#include "../include/config.h"
#include "../include/timestamp.h"

/**
 * One thread's chunk of odd divisors to test for the current number.
 * If end is less than start, the chunk is empty and the thread has nothing to do.
 * That happens when the number has fewer divisors to test than there are threads.
 */
struct Range {
    /** The first divisor in the chunk (odd, inclusive). */
    std::size_t start;
    /** The last divisor in the chunk (inclusive). */
    std::size_t end;
};

/**
 * Variant 2: immediate printing, divisibility division.
 *
 * Reads x and y from config.txt, then checks the numbers 2..y one at a time.
 * For each number, it splits the odd divisors up to its square root into x
 * chunks and starts one thread per chunk. A prime is printed as soon as all
 * threads for that number finish. Prints a timestamp when the run starts and
 * when it ends.
 *
 * @return 0 when the run finishes
 */
int main() {

    const Config cfg = read_config("config.txt");
    ThreadWorker::set_delay(cfg.delay_ms);

    std::vector<ThreadWorker> workers;
    std::vector<std::thread> threads;

    for (std::size_t i = 1; i <= cfg.threads; i++){
        workers.push_back(ThreadWorker(i, cfg.threads));
    }
    
    auto s = std::chrono::steady_clock::now();
    std::cout << "[" << timestamp() << "] RUN START threads=" << cfg.threads << " limit=" << cfg.upper << std::endl;
        for (std::size_t n = 1; n <= cfg.upper; n++){

            
            if (n < 2) continue;         
            if (n > 2 and n % 2 == 0) continue;

            ThreadWorker::set_n(n);
            std::size_t limit = (std::size_t)std::sqrt(n);
            std::size_t count = (limit >= 3) ? (limit - 1)/2 : 0;

            std::size_t base  = count / cfg.threads;
            std::size_t extra = count % cfg.threads;

            std::vector<Range> ranges;
            std::size_t next = 2;
            std::size_t start = 3;
            for (std::size_t i = 0; i < cfg.threads; i++){
                const std::size_t size = base + (i < extra ? 1 : 0);
                ranges.push_back(Range{start, start + (next * (size-1))});
                start += next + (next * (size-1));
            }
            
            // for (std::size_t i = 0; i < cfg.threads; i++){
            //     std::cout << "Thread " << i+1 << " assigned [" << ranges[i].start << ", " << ranges[i].end << "]" << std::endl;
            // }

            auto s_i = std::chrono::steady_clock::now();
            std::cout << "[" << timestamp() << "] INNER RUN START threads=" << cfg.threads << " num=" << n << std::endl;

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
            ThreadWorker::reset_count();
            ThreadWorker::reset_bool();
            auto e_i = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(e_i - s_i).count();
            std::cout << "[" << timestamp() << "] INNER RUN END elapsed=" << elapsed << std::endl;
        }
    auto e = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(e - s).count();
    std::cout << "[" << timestamp() << "] RUN END elapsed=" << elapsed << std::endl;
    return 0;
}

