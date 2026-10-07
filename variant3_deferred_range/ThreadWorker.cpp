#include "ThreadWorker.h"

#include <chrono>
#include <thread>

ThreadWorker::ThreadWorker(std::vector<std::size_t>& o)
    : output(o) {}


void ThreadWorker::run(std::size_t start, std::size_t end){

    for (std::size_t n = start; n <= end; n++){

        if (n < 2) continue;                // 0 and 1 are not prime
        if (n > 2 and n % 2 == 0) continue; // evens above 2 are composite

        bool is_prime = true;

        for (std::size_t p = 3; (p*p <=n) and (is_prime); p += 2){
            if (n % p == 0){
                is_prime = false;
            }
        }

        if (is_prime){
            std::unique_lock<std::mutex> lock(ThreadWorker::mut);
            ThreadWorker::output.push_back(n);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(DELAY));
    }
}

