#pragma once

#include "imgui.h"
#include <array>
#include <atomic>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

namespace tl {
namespace im {
struct line {
    ImVec2 start;
    ImVec2 end;
};
struct rect {
    ImVec2 start;
    ImVec2 end;
};

struct name {
    ImVec2 start;
    std::string name;
};

namespace detail {
struct buffer_uint {
    std::vector<line> lines;
    std::vector<rect> rects;
    std::vector<name> names;
};
} // namespace detail

class double_draw_buffer {
    std::array<detail::buffer_uint, 2> buffers;
    std::atomic_bool can_swap;

    double_draw_buffer() noexcept : can_swap(false) {}

  public:
    static double_draw_buffer &ins() noexcept {
        static double_draw_buffer buffer;
        return buffer;
    }

    void write(const line &l) {
        if (!can_swap.load(std::memory_order_acquire))
            writeble_buffer().lines.push_back(l);
    }
    void write(const rect &l) {
        if (!can_swap.load(std::memory_order_acquire))
            writeble_buffer().rects.push_back(l);
    }

    void write(const name &l) {
        if (!can_swap.load(std::memory_order_acquire))
            writeble_buffer().names.push_back(l);
    }

    void finish() noexcept { can_swap.store(true, std::memory_order_release); }

    void swap() {
        if (!can_swap.load(std::memory_order_acquire))
            return;

        // std::lock_guard lock(mutex);
        readable_buffer().lines.clear();
        readable_buffer().rects.clear();
        readable_buffer().names.clear();
        std::swap(buffers[0], buffers[1]);

        can_swap.store(false, std::memory_order_release);
    }

    detail::buffer_uint &readable_buffer() noexcept { return buffers[0]; }

  private:
    detail::buffer_uint &writeble_buffer() noexcept { return buffers[1]; }
};

} // namespace im
} // namespace tl