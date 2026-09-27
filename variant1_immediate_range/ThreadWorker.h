#pragma once

#include <vector>
#include <string>
#include <mutex>
#include <atomic>


class ThreadWorker {
    private:
        int id;
        

        inline static int DELAY;
        inline static std::mutex mut;

    public:
        ThreadWorker(int i);

        void run(int s, int e);

        static void set_delay(int val){
            DELAY = val;
        }
};