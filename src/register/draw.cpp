#include "imgui.h"
#include "shatl/funtions/config.h"
#include "shatl/funtions/detail/entity.h"
#include "shatl/funtions/detail/npc.h"
#include "shatl/funtions/detail/player.h"
#include "shatl/funtions/detail/utils.h"
#include "shatl/funtions/detail/world.h"
#include "shatl/il2cpp/il2cpp.h"
#include "shatl/imgui/buffer.h"
#include "shatl/utils/log.h"
namespace tl {
namespace func {

void draw_entity(entity *e, bool draw_line, bool draw_rect) {
    auto left_top = e->position;
    auto screen_left_top =
        world::to_real_screen(utils::to_screen_position(left_top));

    if (draw_line) {
        auto center_top = left_top + il2cpp::vector2(e->width, 0) / 2;
        auto screen_center_top =
            world::to_real_screen(utils::to_screen_position(center_top));

        im::double_draw_buffer::ins().write(
            im::line{ImVec2(im::detail::get_screen_width() / 2, 10),
                     ImVec2(screen_center_top.x, screen_center_top.y)});
    }

    if (draw_rect) {
        auto right_bottom = left_top + il2cpp::vector2(e->width, e->height);
        auto screen_right_bottom =
            world::to_real_screen(utils::to_screen_position(right_bottom));
        im::double_draw_buffer::ins().write(
            im::rect{ImVec2(screen_left_top.x, screen_left_top.y),
                     ImVec2(screen_right_bottom.x, screen_right_bottom.y)});
    }
}

template <typename Entity>
void draw_arr_entity(il2cpp::array<Entity *> *arr) noexcept {
    if (!arr) [[unlikely]]
        return;

    for (auto e : *arr) {
        if (e && e->is_active())
            draw_entity(reinterpret_cast<entity *>(e),
                        config::ins().draw.draw_line(),
                        config::ins().draw.draw_rect());
    }
}

void main_draw() {
    if (config::ins().draw.draw_npc())
        draw_arr_entity(world::npcs());
    if (config::ins().draw.draw_player())
        draw_arr_entity(world::players());

    im::double_draw_buffer::ins().finish();
}

TL_Register_World_Call(main_draw);

} // namespace func
} // namespace tl