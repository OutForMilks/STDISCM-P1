#include "ThreadWorker.h"
#include "../include/timestamp.h"

#include <thread>
#include <iostream>

ThreadWorker::ThreadWorker(std::size_t i, std::size_t x){
    this->id = i;
    ThreadWorker::N_THREADS = x;
}

void ThreadWorker::run(std::size_t start, std::size_t end){
    for (std::size_t p = start; p <= end and this->shared_bool; p+=2){
        if (this->n % p == 0) {
            this->shared_bool = false;
        }
        
    }
    if (this->shared_bool and (++shared_counter == this->N_THREADS)){
        std::unique_lock<std::mutex> lock(ThreadWorker::mut);
        std::cout << "[" << timestamp() << "] Thread " << this->id << " : " << this->n << std::endl;
    }
}
