#pragma once

#include "imgui.h"
#include <array>
#include <mutex>
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
    float width;
    float height;
};

namespace detail {
struct buffer_uint {
    std::vector<line> lines;
    std::vector<rect> rects;
};
} // namespace detail

class double_draw_buffer {
    std::array<detail::buffer_uint, 2> buffers;
    mutable std::mutex mutex;

    double_draw_buffer() noexcept {}

  public:
    static double_draw_buffer &ins() noexcept {
        static double_draw_buffer buffer;
        return buffer;
    }

    void write(const line &l) noexcept { std::lock_guard lock(mutex); }
    void write(const rect &l) noexcept {}
};

} // namespace im
} // namespace tl