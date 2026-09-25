#include "shatl/funtions/detail/world.h"
#include "shatl/funtions/config.h"
#include "shatl/funtions/detail/entity.h"
#include "shatl/funtions/detail/localuser_game_state.h"
#include "shatl/funtions/detail/utils.h"
#include "shatl/il2cpp/il2cpp.h"
#include "shatl/utils/log.h"

namespace tl {
namespace func {
void set_light_color() noexcept {
    bool is_open = config::ins().bool_value[pro::light_color];
    if (is_open) {
        auto engine =
            local_user_game_state::instance()
                ->get<il2cpp::object<il2cpp::tag<2>> *>("_legacyEngine");
        auto colors1 =
            engine->get<il2cpp::object<il2cpp::tag<3>> *>("_activeLightMap")
                ->get<il2cpp::array<il2cpp::vector3> *>("_colors");
        auto colors2 =
            engine->get<il2cpp::object<il2cpp::tag<3>> *>("_workingLightMap")
                ->get<il2cpp::array<il2cpp::vector3> *>("_colors");

        (*colors1)[0] = il2cpp::vector3{100.f, 100.f, 100.f};
        (*colors2)[0] = il2cpp::vector3{100.f, 100.f, 100.f};
    }
}

TL_Register_World_Call(set_light_color);

void set_cursor_position(il2cpp::vector2 pos) noexcept {
    auto state = local_user_state_xna::instance();
    if (state) {
        state->set<float>("_virtualCursorOverrideX", pos.x);
        state->set<float>("_virtualCursorOverrideY", pos.y);
    }
}

void aim_target(player *me, entity *e) noexcept {
    if (!e)
        return;

    auto pos = utils::to_screen_position(e->position);
    set_cursor_position(world::to_real_screen(pos));
}

npc *get_target_npc(player *me, il2cpp::array<npc *> *arr,
                    bool is_attack_friendly) noexcept {
    npc *target = nullptr;
    float min_dis = 100000.f;

    for (auto e : *arr) {
        if (e->life <= 0 || (is_attack_friendly == false && e->friendly))
            continue;
        auto dis = me->position.distance(e->position);
        if (min_dis > dis) {
            min_dis = dis;
            target = e;
        }
    }

    return target;
}

player *get_target_player(player *me, il2cpp::array<player *> *arr) noexcept {
    player *target = nullptr;
    float min_dis = 100000.f;

    for (auto e : *arr) {
        if (e->get<int>("statLife") <= 0 || e == me || !e->get<bool>("hostile"))
            continue;
        auto dis = me->position.distance(e->position);
        if (min_dis > dis) {
            min_dis = dis;
            target = e;
        }
    }

    return target;
}

void auto_aim() noexcept {
    auto &cfg = config::ins().auto_aim;
    if (cfg.is_open()) {
        auto p = world::instance()->static_call<player *>("get_LocalPlayer");

        if (cfg.npc()) {
            auto npcs = world::npcs();
            auto target = get_target_npc(p, npcs, cfg.is_attack_friendly());
            if (target)
                aim_target(p, reinterpret_cast<entity *>(target));
        }

        if (cfg.player()) {
            auto players = world::players();
            auto target = get_target_player(p, players);
            if (target)
                aim_target(p, reinterpret_cast<entity *>(target));
        }
    }
}

TL_Register_World_Call(auto_aim);

} // namespace func
} // namespace tl
