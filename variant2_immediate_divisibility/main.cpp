// Variant 2 - IMMEDIATE printing, task division by DIVISIBILITY.
//
// Here the threads do not own a slice of the search range; they cooperate on one
// candidate at a time. For each n the divisor range [2, sqrt(n)] is split among
// the x threads by stride: thread 0 tests 2, 2+x, 2+2x, ..., thread 1 tests 3,
// 3+x, ... If any thread finds a divisor it raises a shared flag.
//
// The threads are created once and reused for every candidate; a barrier keeps
// them in step. The last thread to reach the barrier knows every slice for n has
// been tested, so it is the one that prints - immediately, before the group moves
// on to n + 1 - and it also clears the flag for the next round.

#include <atomic>
#include <chrono>
#include <cmath>
#include <condition_variable>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <mutex>
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

// Reusable barrier. The last thread to arrive runs `on_last` while still holding
// the lock, then releases the whole group, so `on_last` is the one place where a
// per-candidate decision can be made exactly once and without a race.
class Barrier {
public:
    explicit Barrier(int count) : count_(count) {}

    void arrive_and_wait(const std::function<void()>& on_last) {
        std::unique_lock<std::mutex> lock(mutex_);
        const unsigned long long my_generation = generation_;
        if (++waiting_ == count_) {
            on_last();
            waiting_ = 0;
            ++generation_;
            cv_.notify_all();
        } else {
            cv_.wait(lock, [this, my_generation] { return generation_ != my_generation; });
        }
    }

private:
    std::mutex mutex_;
    std::condition_variable cv_;
    int count_;
    int waiting_ = 0;
    unsigned long long generation_ = 0;
};

// Largest r with r*r <= n, without trusting the rounding of sqrt().
int isqrt(int n) {
    int r = static_cast<int>(std::sqrt(static_cast<double>(n)));
    while (r > 0 && r > n / r) --r;
    while ((r + 1) <= n / (r + 1)) ++r;
    return r;
}

}  // namespace

int main() {
    Config cfg;
    std::string err;
    if (!load_config("config.txt", cfg, err)) {
        std::cerr << "config error: " << err << '\n';
        return 1;
    }

    std::cout << "Variant 2 - immediate print, divisibility division\n"
              << "threads (x) = " << cfg.threads
              << ", upper bound (y) = " << cfg.upper
              << ", delay = " << cfg.delay_ms << " ms\n\n";

    if (cfg.upper < 2) {
        std::cout << "no candidates in range\n";
        return 0;
    }

    Barrier barrier(cfg.threads);
    std::atomic<bool> composite{false};

    std::vector<std::thread> threads;
    threads.reserve(static_cast<std::size_t>(cfg.threads));

    for (int id = 0; id < cfg.threads; ++id) {
        threads.emplace_back([id, &cfg, &barrier, &composite] {
            for (int n = 2; n <= cfg.upper; ++n) {
                // This thread's slice of the divisor range, taken with a stride
                // of x so the work is spread evenly even for small n.
                const int limit = isqrt(n);
                for (int d = 2 + id; d <= limit; d += cfg.threads) {
                    if (n % d == 0) {
                        composite.store(true);
                        break;
                    }
                }
                if (cfg.delay_ms > 0) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(cfg.delay_ms));
                }

                barrier.arrive_and_wait([id, n, &composite] {
                    if (!composite.load()) {
                        std::cout << stamp() << " | thread " << (id + 1) << ": "
                                  << n << " is PRIME\n";
                    }
                    composite.store(false);  // reset before the group moves on
                });
            }
        });
    }

    for (std::thread& t : threads) {
        t.join();
    }

    std::cout << "\nall threads finished\n";
    return 0;
}
