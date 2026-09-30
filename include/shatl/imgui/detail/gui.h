#pragma once

#define IS_ENABLE_DEBUG_GUI 1

namespace tl {
namespace im {
namespace detail {
void draw_main_gui(int w, int h) noexcept;
int get_screen_width() noexcept;
int get_screen_height() noexcept;
} // namespace detail
} // namespace im
} // namespace tl