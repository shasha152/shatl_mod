#include "shatl/funtions/detail/class/player.h"
#include "setting.pb.h"
#include "shatl/funtions/config.h"
#include "shatl/funtions/detail/class/localuser_game_state.h"
#include "shatl/funtions/detail/class/world.h"
#include "shatl/il2cpp/il2cpp.h"
#include "shatl/imgui/detail/gui.h"

namespace tl {
namespace func {

void set_life() noexcept {
    static auto &data = config::get<cmax_t>(pro::life);
    if (!data)
        return;

    auto p = world::local_player();
    if (p) {
        p->set("statLife", data.value);
        p->set("statLifeMax", data.max);
    }
}

void set_mana() noexcept {
    static auto &data = config::get<cmax_t>(pro::mana);
    if (!data)
        return;

    auto p = world::local_player();
    if (p) {
        p->set("statMana", data.value);
        p->set("statManaMax", data.max);
    }
}

void set_damage() noexcept {
    static auto &data = config::get<cfloat_t>(pro::damage);
    if (!data)
        return;
    auto p = world::local_player();

    p->set("meleeDamage", data.value);
    p->set("magicDamage", data.value);
    p->set("rangedDamage", data.value);
}

void super_move() noexcept {
    auto p = world::local_player();
    if (!p)
        return;
    const auto &ctrl_fly = config::get<cfloat_t>(pro::control_fly);
    const auto &cursor_fly = config::get<cfloat_t>(pro::cursor_fly);
    if (ctrl_fly || cursor_fly)
        p->velocity = il2cpp::vector2();
    else
        return;

    const float speed = ctrl_fly ? ctrl_fly.value : cursor_fly.value;

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
        const auto wh_c = il2cpp::vector2(im::detail::get_screen_width(),
                                          im::detail::get_screen_height()) /
                          2;
        auto state = local_user_state_xna::instance();
        const auto pos =
            (il2cpp::vector2(state->get<float>("_virtualCursorOverrideX"),
                             state->get<float>("_virtualCursorOverrideY")) -
             wh_c) /
            50;
        p->velocity = pos * speed / 10;
    }
    p->set("maxFallSpeed", 99999.0f);
}

void after_reset_effects(player *player) noexcept {
    if (player != world::local_player())
        return;

    set_life();
    set_mana();
    super_move();
    set_damage();
}

} // namespace func
} // namespace tl