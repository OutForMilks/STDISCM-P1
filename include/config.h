#pragma once   // prevents it from being included twice

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <charconv>
#include <cstddef>
#include <cstdlib>
#include <system_error>

inline constexpr std::size_t MAX_THREADS = 1024;
inline constexpr std::size_t MAX_UPPER   = 100'000'000;
inline constexpr std::size_t MAX_DELAY   = 1000;

struct Config {
    std::size_t threads  = 1; // x
    std::size_t upper    = 0; // y
    std::size_t delay_ms = 5; // optional; slows each step so the interleaving is visible
};


inline bool parse_num(const std::string& text, std::size_t min, std::size_t max, std::size_t& out){
    std::size_t val = 0;
    const char* last = text.data() + text.size();
    auto [ptr, ec] = std::from_chars(text.data(), last, val);

    if (ec != std::errc{})          return false;  // not a number, negative, or too big
    if (ptr != last)                return false;  // junk after it, e.g. "12abc"
    if (val < min || val > max) return false;  // outside your allowed range
    out = val;
    return true;
}

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
        std::string val;
        if (iss >> key >> val){
            bool ok = true;                             // unknown keys are ignored
            if      (key == "x")     ok = parse_num(val, 1, MAX_THREADS, c.threads);
            else if (key == "y")     ok = parse_num(val, 0, MAX_UPPER,   c.upper);
            else if (key == "delay") ok = parse_num(val, 0, MAX_DELAY,   c.delay_ms);

            if (!ok){                                   
                std::cerr << "Bad value for " << key << ": " << val << '\n';
                std::exit(1);
            }
        }
    }
    return c;
}