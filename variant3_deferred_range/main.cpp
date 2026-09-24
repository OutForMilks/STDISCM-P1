// Variant 3 - DEFERRED printing, task division by RANGE.
//
// Same work split as variant 1: the interval [2, y] becomes x contiguous chunks,
// one per thread. The difference is when the output appears. Each thread keeps
// its findings in its own slot of a results vector - one slot per thread, so no
// two threads ever touch the same memory and no lock is needed - and main prints
// everything only after every thread has been joined.
//
// The timestamp is taken at the moment the prime is found, so the recorded times
// still show the threads running concurrently even though the printing is serial.

#include <chrono>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

namespace {

using Clock = std::chrono::steady_clock;
const Clock::time_point PROGRAM_START = Clock::now();

std::string stamp() {
    const double ms = std::chrono::duration<double, std::milli>(Clock::now() - PROGRAM_START).count();
    std::ostringstream os;
    os << std::fixed << std::setprecision(3) << std::setw(10) << ms << " ms";
    return os.str();
}

struct Config {
    int threads = 1;   // x
    int upper = 1;     // y
    int delay_ms = 0;  // optional
};

bool load_config(const std::string& path, Config& cfg, std::string& err) {
    std::ifstream file(path);
    if (!file) {
        err = "cannot open " + path;
        return false;
    }

    // One "key value" pair per line; blank lines and #-comments are skipped.
    std::string line;
    bool saw_x = false, saw_y = false;
    while (std::getline(file, line)) {
        const std::size_t comment = line.find('#');
        if (comment != std::string::npos) line.erase(comment);

        std::istringstream in(line);
        std::string key;
        long long value;
        if (!(in >> key)) continue;
        if (!(in >> value)) {
            err = "expected \"" + key + " <number>\" in " + path;
            return false;
        }

        if (key == "x") { cfg.threads = static_cast<int>(value); saw_x = true; }
        else if (key == "y") { cfg.upper = static_cast<int>(value); saw_y = true; }
        else if (key == "delay") { cfg.delay_ms = static_cast<int>(value); }
        else { err = "unknown key \"" + key + "\" in " + path; return false; }
    }
    if (!saw_x || !saw_y) {
        err = path + " must define both x and y";
        return false;
    }
    if (cfg.threads < 1) { err = "x must be at least 1"; return false; }
    if (cfg.delay_ms < 0) { err = "delay must not be negative"; return false; }
    return true;
}

bool is_prime(int n) {
    if (n < 2) return false;
    if (n % 2 == 0) return n == 2;
    for (int d = 3; d <= n / d; d += 2) {
        if (n % d == 0) return false;
    }
    return true;
}

}  // namespace

int main() {
    Config cfg;
    std::string err;
    if (!load_config("config.txt", cfg, err)) {
        std::cerr << "config error: " << err << '\n';
        return 1;
    }

    std::cout << "Variant 3 - deferred print, range division\n"
              << "threads (x) = " << cfg.threads
              << ", upper bound (y) = " << cfg.upper
              << ", delay = " << cfg.delay_ms << " ms\n"
              << "searching...\n";

    const int total = cfg.upper >= 2 ? cfg.upper - 1 : 0;
    const int base = total / cfg.threads;
    const int extra = total % cfg.threads;

    // One slot per thread: each writes only to results[id - 1].
    std::vector<std::vector<std::string>> results(static_cast<std::size_t>(cfg.threads));
    std::vector<std::thread> threads;
    threads.reserve(static_cast<std::size_t>(cfg.threads));

    int next = 2;
    for (int id = 1; id <= cfg.threads; ++id) {
        const int size = base + (id <= extra ? 1 : 0);
        const int lo = next;
        const int hi = next + size - 1;
        next += size;

        std::vector<std::string>& sink = results[static_cast<std::size_t>(id - 1)];
        threads.emplace_back([id, lo, hi, delay_ms = cfg.delay_ms, &sink] {
            for (int n = lo; n <= hi; ++n) {
                if (is_prime(n)) {
                    sink.push_back(stamp() + " | thread " + std::to_string(id) + ": " +
                                   std::to_string(n) + " is PRIME");
                }
                if (delay_ms > 0) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
                }
            }
        });
    }

    for (std::thread& t : threads) {
        t.join();
    }

    // Every thread has finished, so reading the results needs no synchronisation.
    std::cout << "\n--- all threads finished, printing collected results ---\n";
    for (const std::vector<std::string>& per_thread : results) {
        for (const std::string& line : per_thread) {
            std::cout << line << '\n';
        }
    }
    return 0;
}
