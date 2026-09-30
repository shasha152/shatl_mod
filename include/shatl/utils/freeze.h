#pragma once

#include "shatl/utils/log.h"
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
    struct safe_function {
        std::function<void()> function{nullptr};
        std::atomic_bool has_value{false};

        explicit operator bool() const noexcept {
            return has_value.load(std::memory_order_acquire);
        }
        template <typename Func> void set(Func &&fn) noexcept {
            function = std::forward<Func>(fn);
            has_value.store(true, std::memory_order_release);
        }

        void clear() { has_value.store(false, std::memory_order_release); }

        void operator()() { function(); }
    };

    std::array<safe_function, N> funcs;

    std::thread worker;
    std::chrono::nanoseconds interval;
    std::atomic_bool is_stop;

  public:
    explicit freeze_worker(std::chrono::nanoseconds interval) noexcept
        : interval(interval), is_stop(false) {}

    void run() {
        is_stop.store(false, std::memory_order_relaxed);
        worker = std::thread([this]() { _work(); });
    }

    template <typename Func> void set(Func &&fn, std::size_t i) noexcept {
        funcs[i].set(std::forward<Func>(fn));
    }

    void clear(std::size_t i) noexcept { funcs[i].clear(); }
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
            std::this_thread::sleep_for(interval);
        }
    }
};

namespace ft {
inline constexpr int speed = 0;
}

inline freeze_worker<16> g_freeze_worker(std::chrono::nanoseconds(1));

} // namespace utils
} // namespace tl