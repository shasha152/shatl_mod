#pragma once

#include <array>
#include <atomic>
#include <chrono>
#include <cstddef>
#include <functional>
#include <thread>
#include <utility>
namespace tl {
namespace utils {
template <std::size_t N> class freeze_worker {
    std::array<std::function<void()>, N> funcs;

    std::thread worker;
    std::chrono::milliseconds milliseconds;
    std::atomic_bool is_stop;

  public:
    explicit freeze_worker(std::chrono::milliseconds milliseconds) noexcept
        : milliseconds(milliseconds), is_stop(false) {}

    void run() {
        worker = std::thread([this]() { _work(); });
    }

    template <typename Func> void set(Func &&fn, std::size_t i) noexcept {
        funcs[i] = std::forward<Func>(fn);
    }

    void clear(std::size_t i) noexcept { funcs[i] = nullptr; }
    void stop() noexcept {
        is_stop.store(true, std::memory_order_relaxed);
        if (worker.joinable())
            worker.join();
    }

    ~freeze_worker() noexcept { stop(); }

  private:
    void _work() noexcept {
        while (!is_stop.load(std::memory_order_relaxed)) {
            for (auto &fn : funcs) {
                if (fn)
                    fn();
            }
            std::this_thread::sleep_for(milliseconds);
        }
    }
};

inline static freeze_worker<16> g_freeze_worker(std::chrono::milliseconds(2));

} // namespace utils
} // namespace tl