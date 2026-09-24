#pragma once

#include <vector>
#include <string>
#include <mutex>
#include <thread>
#include <atomic>
#include <condition_variable>

class ThreadWorker {
    private:
        // int delay = 0;
        int id;
        int start;
        int end;
        int y;
        
        inline static std::vector<std::string> output;

        inline static int N_THREADS;
        inline static int turn = 1;
        inline static std::mutex print_mutex;
        inline static std::atomic<int> shared_counter = 0;

        inline static std::mutex prime_mutex;

        inline static bool isPrime = true;
        inline static int counter = 0;
        // int squared_n;        
    
    public:
        ThreadWorker(int i, int x);

        void async_print(int s, int e);
        //rename to async_prime();
        void sync_prime(int s, int e);

        void async_div();
        void sync_div(int s, int e, int y);
        
        void show_output();

        void advance_turn();
        
        // int getId();
};