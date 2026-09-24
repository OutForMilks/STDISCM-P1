#include <iostream>
#include <vector>
#include <thread>
#include <string>
#include "ThreadWorker.h"
#include <fstream>
#include <cmath>



int main() {
    
    std::ifstream file("config.txt");
    if (!file){
        std::cerr << "Failed to open file\n";
        return 1;
    }

    int value;
    std::string word;
    //default values
    int x = 2; //Num of threads
    int y = 100; // Uper bound

    while (file >> word >> value){
        if (word == "x") x = value;
        else if (word == "y") y = value;
    }


    std::vector<ThreadWorker> workers;
    std::vector<std::thread> threads;
    std::vector<std::vector<std::string>> results(x);
    
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
