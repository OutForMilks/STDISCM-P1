#pragma once   // prevents it from being included twice

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

struct Config {
    int threads = 0;   // x
    int upper = 0;     // y
    int delay_ms = 5;  // optional; slows each step so the interleaving is visible
};

// inline, like timestamp(): this header is included by more than one .cpp per
// variant, and a non-inline definition would be a duplicate symbol at link time.
inline Config read_config(const std::string& path){
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
            if      (key == "x")      c.threads = value;
            else if (key == "y")      c.upper = value;
            else if (key == "delay")  c.delay_ms = value;
        }
    }
    return c;
}