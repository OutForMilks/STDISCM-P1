#include "ThreadWorker.h"

#include <chrono>
#include <thread>
#include <iostream>
#include <vector>
#include <cmath>
inline int DELAY = 500;

ThreadWorker::ThreadWorker(int i, int x){
    this->id = i;
    ThreadWorker::N_THREADS = x;
}

// void ThreadWorker::advance_turn(){
//     //turn is id, but id starts with 1 so we need to -1
//     int cur_turn = ThreadWorker::turn - 1;
//     for (int i = 0; i < ThreadWorker::N_THREADS; i++){
//         cur_turn = (cur_turn + 1) % ThreadWorker::N_THREADS;
//         if(!ThreadWorker::finished[cur_turn]){
//            ThreadWorker::turn = cur_turn + 1; 
//            break; 
//         } 
//     }
// }

void ThreadWorker::show_output(){
    for (std::string s : ThreadWorker::output){
        std::cout << s << std::endl;
    }
}

void ThreadWorker::async_print(int s, int e){
    this->start = s;
    this->end = e;
    for (int n = start; n <= end; n++){

        if (n < 2) continue;                // 0 and 1 are not prime
        if (n > 2 and n % 2 == 0) continue; // evens above 2 are composite

        bool is_composite = false;

        for (int p = 3; (p*p <=n) and (!is_composite); p += 2){
            if (n % p == 0){
                is_composite = true;
            }
        }
        if (!is_composite){
            auto now = std::chrono::system_clock::now();
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
            std::cout << ms <<" ms | Thread " << this->id << ": " << n << " is a PRIME" << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(DELAY));
    }
}


void ThreadWorker::sync_prime(int s, int e){
    this->start = s;
    this->end = e;
    for (int n = start; n <= end; n++){

        if (n < 2) continue;                // 0 and 1 are not prime
        if (n > 2 and n % 2 == 0) continue; // evens above 2 are composite

        bool is_composite = false;

        for (int p = 3; (p*p <=n) and (!is_composite); p += 2){
            if (n % p == 0){
                is_composite = true;
            }
        }

        if (!is_composite){
            auto now = std::chrono::system_clock::now();
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
            std::string s_ms = std::to_string(ms);
            std::string s_id = std::to_string(this->id);
            std::string s_n = std::to_string(n);
            std::unique_lock<std::mutex> lock(ThreadWorker::print_mutex);
            ThreadWorker::output.push_back(s_ms + " ms | Thread " + s_id + ": " + s_n + " is a PRIME");
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(DELAY));
    }
    ThreadWorker::shared_counter++;
    if(ThreadWorker::shared_counter == N_THREADS) show_output();
}

// void ThreadWorker::sync_div(int s, int e, int y, std::vector<int> local_primes){

//     this->start = s;
//     this->end = e;
//     this->y = y;

//     int prime_size = static_cast<int>(local_primes.size());
//     for (int n = 0; n < prime_size; n++){
//         if (y % local_primes[n] == 0){
//             auto now = std::chrono::system_clock::now();
//             auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();            

//             std::string s_ms = std::to_string(ms);
//             std::string s_id = std::to_string(this->id);
//             std::unique_lock<std::mutex> lockB(ThreadWorker::print_mutex);

//             ThreadWorker::output.push_back(s_ms + " ms | Thread " + s_id + ": " + std::to_string(primes[n]) + " proves " + std::to_string(y) + " is not a PRIME number");
//         }
//         std::this_thread::sleep_for(std::chrono::milliseconds(DELAY));
//     }

//     ThreadWorker::shared_counter++;
//     if(ThreadWorker::shared_counter == N_THREADS) {
//         show_output();
//         ThreadWorker::shared_counter = 0;
//         ThreadWorker::output.clear();
//     }
// }
