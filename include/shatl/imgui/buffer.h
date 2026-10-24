#pragma once

#include "imgui.h"
#include <array>
#include <atomic>
#include <mutex>
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

namespace detail {
struct buffer_uint {
    std::vector<line> lines;
    std::vector<rect> rects;
};
} // namespace detail

class double_draw_buffer {
    std::array<detail::buffer_uint, 2> buffers;
    std::atomic_bool can_swap;
    mutable std::mutex mutex;

    double_draw_buffer() noexcept : can_swap(false) {}

  public:
    static double_draw_buffer &ins() noexcept {
        static double_draw_buffer buffer;
        return buffer;
    }

    void write(const line &l) noexcept {
        if (!can_swap)
            writeble_buffer().lines.push_back(l);
    }
    void write(const rect &l) noexcept {
        if (!can_swap)
            writeble_buffer().rects.push_back(l);
    }

    void finish() noexcept { can_swap = true; }

    void swap() {
        if (can_swap) {
            {
                std::lock_guard lock(mutex);
                readble_buffer().lines.clear();
                readble_buffer().rects.clear();
                std::swap(buffers[0], buffers[1]);
            }
            can_swap = false;
        }
    }

    detail::buffer_uint &readble_buffer() noexcept { return buffers[0]; }
    detail::buffer_uint &writeble_buffer() noexcept { return buffers[1]; }
};

} // namespace im
} // namespace tl