#include "ThreadWorker.h"

#include <chrono>
#include <thread>
#include <iostream>
#include <vector>
#include <cmath>
#include "../include/timestamp.h"

ThreadWorker::ThreadWorker(int i, int x){
    this->id = i;
    ThreadWorker::N_THREADS = x;
}

void ThreadWorker::print_output(){

    for (std::string s : ThreadWorker::output){
        std::cout << s << std::endl;
    }
    
}

void ThreadWorker::run(int start, int end){
    for (int p = start; p <= end; p+=2){
        if (this->n % p == 0) {
            this->shared_bool = false;
        }
    }

    if (this->shared_bool and (++shared_counter == this->N_THREADS)){
        std::unique_lock<std::mutex> lock(ThreadWorker::mut);
        std::string s_id = std::to_string(this->id);
        std::string s_n = std::to_string(n);
        ThreadWorker::output.push_back("[" + timestamp() + "] Thread " + s_id + " | " + s_n + " is  PRIME.");
    }
}