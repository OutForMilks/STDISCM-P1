#pragma once

#include <vector>
#include <string>
#include <mutex>
#include <atomic>

class ThreadWorker {
    private:
        int id;
        
        inline static std::vector<std::string> output;

        inline static int N_THREADS;
        inline static int DELAY;
        inline static int turn = 1;
        inline static std::mutex mut;
        inline static std::atomic<int> shared_counter = 0;
        inline static int counter = 0;
    
    public:
        ThreadWorker(int i, int x);

        void run(int s, int e);

        static void set_delay(int val){
            DELAY = val;
        }

        static void print_output();
};