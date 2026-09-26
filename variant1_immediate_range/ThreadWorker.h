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
        inline static std::mutex mut;

    public:
        ThreadWorker(int i, int x);

        void run(int s, int e);

        static void set_delay(int val){
            DELAY = val;
        }
};