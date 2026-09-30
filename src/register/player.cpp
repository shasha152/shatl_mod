#include "shatl/funtions/config.h"
#include "shatl/funtions/detail/localuser_game_state.h"
#include "shatl/funtions/detail/world.h"
#include "shatl/il2cpp/il2cpp.h"
#include "shatl/imgui/detail/gui.h"

namespace tl {
namespace func {

inline void set_life() {
    // if (pro::)
    static auto &data = config::ins().max_value[pro::max_value_type::life];

    if (data) {
        auto p = world::local_player();
        if (p) {
            p->set("statLife", data.value);
            p->set("statLifeMax", data.max);
        }
    }
}

inline void set_mana() {
    static auto &data = config::ins().max_value[pro::max_value_type::mana];
    if (data) {
        auto p = world::local_player();
        if (p) {
            p->set("statMana", data.value);
            p->set("statManaMax", data.max);
        }
    }
}

void super_move() {
    auto p = world::local_player();
    if (!p)
        return;
    auto &ctrl_fly =
        config::ins().float_value[pro::float_value_type::control_fly];
    auto &cursor_fly =
        config::ins().float_value[pro::float_value_type::cursor_fly];
    if (ctrl_fly || cursor_fly)
        p->velocity = il2cpp::vector2();
    else
        return;

    float speed = ctrl_fly ? ctrl_fly.value : cursor_fly.value;

    if (ctrl_fly) {
        if (p->get<bool>("controlLeft"))
            p->velocity.x = -speed;
        if (p->get<bool>("controlRight"))
            p->velocity.x = speed;
        if (p->get<bool>("controlUp"))
            p->velocity.y = -speed;
        if (p->get<bool>("controlDown"))
            p->velocity.y = speed;

        if (!p->get<bool>("controlUp") && !p->get<bool>("controlDown"))
            p->velocity.y = -p->get<float>("gravity");
    }

    if (cursor_fly) {
        auto wh_c = il2cpp::vector2(im::detail::get_screen_width(),
                                    im::detail::get_screen_height()) /
                    2;
        auto state = local_user_state_xna::instance();
        auto pos =
            (il2cpp::vector2(state->get<float>("_virtualCursorOverrideX"),
                             state->get<float>("_virtualCursorOverrideY")) -
             wh_c) /
            50;
        p->velocity = pos * speed / 10;
    }
}

TL_Register_World_Call(set_life);
TL_Register_World_Call(set_mana);
TL_Register_World_Call(super_move);

} // namespace func
} // namespace tl