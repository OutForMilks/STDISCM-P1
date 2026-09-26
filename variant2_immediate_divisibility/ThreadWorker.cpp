#include "ThreadWorker.h"
#include "../include/timestamp.h"

#include <thread>
#include <iostream>
#include <vector>

ThreadWorker::ThreadWorker(int i, int x){
    this->id = i;
    ThreadWorker::N_THREADS = x;
}

void ThreadWorker::run(int start, int end){
    for (int p = start; p <= end; p+=2){
        if (this->n % p == 0) {
            this->shared_bool = false;
        }
        
    }
    if (this->shared_bool and (++shared_counter == this->N_THREADS)){
        std::unique_lock<std::mutex> lock(ThreadWorker::mut);
        std::cout << "[" << timestamp() << "] Thread " << this->id << " | " << this->n << " is " << " PRIME." << std::endl;
    }
}
