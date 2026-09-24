#include <iostream>
#include <vector>
#include <thread>
#include <string>
#include <sstream>
#include "ThreadWorker.h"
#include <fstream>
#include <cmath>
#include "../include/config.h"
#include "../include/timestamp.h"

// A thread's slice of the search space. end < start means "nothing to do",
// which is what threads get when there are more threads than numbers.
struct Range {
    int start;
    int end;
};

int main() {

    Config cfg;

    cfg = read_config("config.txt");

    std::vector<ThreadWorker> workers;
    std::vector<std::thread> threads;
    std::vector<std::vector<std::string>> results(cfg.threads);
    
    // Split 1..y into x chunks. Every thread gets y/x numbers, and the first
    // (y % x) threads take one extra each, so the remainder is spread out
    // instead of being dropped. `next` is the first number nobody owns yet, so
    // each chunk starts exactly where the previous one ended - that is what
    // makes gaps impossible.
    const int base  = cfg.upper / cfg.threads;
    const int extra = cfg.upper % cfg.threads;

    std::vector<Range> ranges;
    int next = 1;
    for (int i = 0; i < cfg.threads; i++){
        const int size = base + (i < extra ? 1 : 0);
        ranges.push_back(Range{next, next + size - 1});
        next += size;
    }

    for (int i = 1; i <= cfg.threads; i++){
        workers.push_back(ThreadWorker(i, cfg.threads, cfg.delay_ms));
    }
    auto s = std::chrono::steady_clock::now();
    std::cout << "[" << timestamp() << "] RUN START threads=" << cfg.threads << " limit=" << cfg.upper << std::endl;

    for (int i = 0; i < cfg.threads; i++){
        std::cout << "Thread " << i+1 << " assigned [" << ranges[i].start << ", " << ranges[i].end << "]" << std::endl;
    }

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
    auto e = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(e - s).count();
    std::cout << "[" << timestamp() << "] RUN END elapsed=" << elapsed << std::endl;

    return 0;
}
