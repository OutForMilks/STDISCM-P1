#pragma once

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

inline std::string timestamp() {
    using namespace std::chrono;

    auto now = system_clock::now();
    std::time_t t = system_clock::to_time_t(now);
    auto us = duration_cast<microseconds>(now.time_since_epoch()) % 1'000'000;

    std::tm local{};
    localtime_r(&t, &local);

    std::ostringstream out;
    out << std::put_time(&local, "%Y-%m-%d %H:%M:%S")
        << '.' << std::setw(6) << std::setfill('0') << us.count();
    return out.str();
}