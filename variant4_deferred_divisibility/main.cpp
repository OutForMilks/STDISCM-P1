#include <chrono>
#include <iostream>
#include <thread>
#include <vector>
#include <cmath>
#include "ThreadWorker.h"
#include "../include/config.h"
#include "../include/timestamp.h"

// A thread's slice of the search space. end < start means "nothing to do",
// which is what threads get when there are more threads than numbers.
struct Range {
    int start;
    int end;
};

int main() {

    const Config cfg = read_config("config.txt");
    ThreadWorker::set_delay(cfg.delay_ms);

    std::vector<ThreadWorker> workers;
    std::vector<std::thread> threads;

    for (int i = 1; i <= cfg.threads; i++){
        workers.push_back(ThreadWorker(i, cfg.threads));
    }
    
    auto s = std::chrono::steady_clock::now();
    std::cout << "[" << timestamp() << "] RUN START threads=" << cfg.threads << " limit=" << cfg.upper << std::endl;
        for (int n = 1; n <= cfg.upper; n++){

            
            if (n < 2) continue;         
            if (n > 2 and n % 2 == 0) continue;

            ThreadWorker::set_n(n);
            int limit = (int)std::sqrt(n);
            int count = (limit-1)/2;

            int base  = count / cfg.threads;
            int extra = count % cfg.threads;

            std::vector<Range> ranges;
            int next = 2;
            int start = 3;
            for (int i = 0; i < cfg.threads; i++){
                const int size = base + (i < extra ? 1 : 0);
                ranges.push_back(Range{start, start + (next * (size-1))});
                start += next + (next * (size-1));
            }
            
            for (int i = 0; i < cfg.threads; i++){
                std::cout << "Thread " << i+1 << " assigned [" << ranges[i].start << ", " << ranges[i].end << "]" << std::endl;
            }

            auto s_i = std::chrono::steady_clock::now();
            std::cout << "[" << timestamp() << "] INNER RUN START threads=" << cfg.threads << " num=" << n << std::endl;

            for (int i = 0; i < cfg.threads; i++){
                const int start = ranges[i].start;
                const int end   = ranges[i].end;
                threads.push_back(std::thread([&workers, i, start, end]{
                    workers[i].run(start, end);
                }));
            }

            for (int i = 0; i < cfg.threads; i++){
                threads[i].join();
            }
            threads.clear();
            ThreadWorker::reset_count();
            ThreadWorker::reset_bool();
            auto e_i = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(e_i - s_i).count();
            std::cout << "[" << timestamp() << "] INNER RUN END elapsed=" << elapsed << std::endl;
        }
    ThreadWorker::print_output();
    auto e = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(e - s).count();
    std::cout << "[" << timestamp() << "] RUN END elapsed=" << elapsed << std::endl;
    return 0;
}

