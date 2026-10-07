#include <chrono>
#include <iostream>
#include <vector>
#include <cmath>
#include <memory>
#include "ThreadWorker.h"
#include "TaskQueue.h"
#include "../include/config.h"
#include "../include/timestamp.h"

/**
 * Variant 4: deferred printing, divisibility division.
 *
 * Reads x and y from config.txt and starts a pool of x worker threads once.
 * Then it checks the numbers 2..y one at a time. For each number, it splits
 * the odd divisors up to its square root into x chunks, pushes one task per
 * chunk into the queue, and waits for all of them. If no worker found a
 * divisor, main saves the number. After every number is done, all saved
 * primes are printed at once. Prints a timestamp when the run starts and
 * when it ends.
 *
 * @return 0 when the run finishes
 */
int main() {

    const Config cfg = read_config("config.txt");
    ThreadWorker::set_delay(cfg.delay_ms);

    auto s = std::chrono::steady_clock::now();
    std::cout << "[" << timestamp() << "] RUN START threads=" << cfg.threads << " limit=" << cfg.upper << std::endl;

    TaskQueue queue;
    std::vector<std::unique_ptr<ThreadWorker>> workers;

    for (std::size_t i = 0; i < cfg.threads; i++){
        workers.push_back(std::make_unique<ThreadWorker>(queue));
    }

    std::vector<std::size_t> primes;

    for (std::size_t n = 1; n <= cfg.upper; n++){
        
        if (n < 2) continue;         
        if (n > 2 and n % 2 == 0) continue;
        
        
        std::size_t limit = (int)std::sqrt(n);
        std::size_t count = (limit >= 3) ? (limit - 1)/2 : 0;

        std::size_t base  = count / cfg.threads;
        std::size_t extra = count % cfg.threads;

        std::size_t start = 3;
        for (std::size_t i = 0; i < cfg.threads; i++){
            const std::size_t size = base + (i < extra ? 1 : 0);
            const std::size_t end = start + (2 * (size-1));
            queue.push(Task{start, end, n});
            start += 2 + (2 * (size-1));
        }

        queue.wait_all();

        if (ThreadWorker::is_prime()){
            primes.push_back(n);
        }

        ThreadWorker::reset_bool();
    }

    queue.close();
    workers.clear();

    std::cout << "List of primes: " << std::endl;
    for (const auto& prime : primes){
        std::cout << prime <<std::endl;
    }

    auto e = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(e - s).count();
    std::cout << "[" << timestamp() << "] RUN END elapsed=" << elapsed << std::endl;
    return 0;
}

