#include <iostream>
#include <vector>
#include <thread>
#include <string>
#include <sstream>
#include "ThreadWorker.h"
#include <fstream>
#include <cmath>

struct Config {
    int threads = 0;   // x
    int upper = 0;     // y
    int delay_ms = 0;  // optional; slows each step so the interleaving is visible
};

Config read_config(const std::string& path){
    Config c;
    std::ifstream file(path);

    if (!file){
        std::cerr << "Could not open " << path << ", using defaults\n";
        return c;
    }

    std::string line;
    while (std::getline(file, line)){
        std::istringstream iss(line);
        std::string key;
        int value;
        if (iss >> key >> value){
            if      (key == "x")    c.threads = value;
            else if (key == "y")    c.upper = value;
        }
    }
    return c;
}

int main() {

    Config cfg;

    cfg = read_config("config.txt");


    std::vector<ThreadWorker> workers;
    std::vector<std::thread> threads;
    std::vector<std::vector<std::string>> results(cfg.threads);
    
    int start = 1;
    int end;

    for (int i = 1; i <= x; i++){
        workers.push_back(ThreadWorker(i, x));
    }

    for (int i = 0; i < x; i++){
        if(y >= x){
            start = (y/x) * (i) + 1; // 2/3 = 0 * 0 = 0 + 1 = 1
            end = (y/x) * (i+1); // 10/3 = 3 * 3 = 9
            
            if((i+1 == x) and (end != y)) end++;
        }
        else{
            start = i+1;
            end = i+1;
        }

        threads.push_back(std::thread([&workers, i, end, start]{
            workers[i].async_print(start, end);
        }));
    }

    for (int i = 0; i < x; i++){
        threads[i].join();
    }
    threads.clear();

    // std::cout << "--- Finished asynchronous print ---" << std::endl;
    // for (int i = 0; i < x; i++){
    //     if(y >= x){
    //         start = (y/x) * (i) + 1; // 2/3 = 0 * 0 = 0 + 1 = 1
    //         end = (y/x) * (i+1); // 10/3 = 3 * 3 = 9
            
    //         if((i+1 == x) and (end != y)) end += (y % x);
    //     }
    //     else{
    //         start = i+1;
    //         end = i+1;
    //     }
    //     std::cout << "id " << i + 1 << " | " << start << " - " << end << std::endl;

    //     threads.push_back(std::thread([&workers, i, end, start]{
    //         workers[i].sync_prime(start, end);
    //     }));
    // }

    // for (int i = 0; i < x; i++){
    //     threads[i].join();
    // }
    // threads.clear();
    // std::cout << "--- Finished end print ---" << std::endl;

    // for (int i = 1; i <= x; i++){
    //     workers.push_back(ThreadWorker(i, y/x));
    // }

    //tag 2 as prime
    // int n_threads;
    // for (int i = 3; i <= y; i++){

    //     int root = static_cast<int>(std::sqrt(y));  
    //     if (root < x) n_threads = root+1;
    //     else n_threads = x;

    //     for (int j = 0; j < n_threads; j++){

    //         if(root > x){
    //             start = (root/x) * (i) + 1; // 2/3 = 0 * 0 = 0 + 1 = 1
    //             end = (root/x) * (i+1); // 10/3 = 3 * 3 = 9
                
    //             if((i+1 == x) and (end != root)) end++;
    //         }
    //         else{
    //             start = j+1;
    //             end = j+1;
    //         }

    //         threads.push_back(std::thread([&workers, j, start, end, i]{
    //             workers[j].sync_div(start, end, i);
    //         }));
    //     }  
    //     for (int j = 0; j < x; j++){
    //         threads[j].join();
    //     }     
    //     threads.clear();
    //     workers.clear();
    // }


    return 0;
}
