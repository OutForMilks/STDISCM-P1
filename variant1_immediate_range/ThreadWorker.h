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
        // inline static int turn = 1;
        inline static std::mutex mut;
        // inline static std::atomic<int> shared_counter = 0;

        // inline static std::mutex prime_mutex;
    
    public:
        ThreadWorker(int i, int x, int delay);

        void run(int s, int e);
};