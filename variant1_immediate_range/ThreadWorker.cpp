#include "ThreadWorker.h"

#include <chrono>
#include <thread>
#include <iostream>
#include <vector>
#include <cmath>
#include <primesieve.hpp>
#include "../include/config.h"
#include "../include/timestamp.h"

ThreadWorker::ThreadWorker(int i, int x, int delay){
    this->id = i;
    ThreadWorker::N_THREADS = x;
    this->DELAY = delay;
}

void ThreadWorker::run(int start, int end){


    for (int n = start; n <= end; n++){

        if (n < 2) continue;                // 0 and 1 are not prime
        if (n > 2 and n % 2 == 0) continue; // evens above 2 are composite

        bool is_prime = true;

        for (int p = 3; (p*p <=n) and (is_prime); p += 2){
            if (n % p == 0){
                is_prime = false;
            }
        }
        if (is_prime){
            auto now = std::chrono::system_clock::now();
            std::cout << "[" << timestamp() << "] Thread " << this->id << " : " << n << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(DELAY));
    }
}