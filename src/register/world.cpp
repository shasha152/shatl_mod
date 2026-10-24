#include "shatl/funtions/detail/world.h"
#include "shatl/detail/counter.inc.h"
#include "shatl/funtions/config.h"
#include "shatl/funtions/detail/entity.h"
#include "shatl/funtions/detail/localuser_game_state.h"
#include "shatl/funtions/detail/npc.h"
#include "shatl/funtions/detail/player.h"
#include "shatl/funtions/detail/utils.h"
#include "shatl/il2cpp/il2cpp.h"
#include "shatl/utils/log.h"
#include <utility>

namespace tl {
namespace func {
void set_light_color() noexcept {
    bool is_open = config::ins().bool_value[pro::light_color];
    if (!is_open)
        return;

    using object_ptr = il2cpp::object<il2cpp::tag<TL_AUTO_TAG_ID>> *;

    auto state = local_user_game_state::instance();
    if (state == nullptr) [[unlikely]]
        return;

    auto engine = state->get<object_ptr>("_legacyEngine");
    if (engine == nullptr) [[unlikely]]
        return;

    auto active = engine->get<object_ptr>("_activeLightMap");
    auto working = engine->get<object_ptr>("_workingLightMap");
    if (active == nullptr || working == nullptr) [[unlikely]]
        return;

    auto colors1 = active->get<il2cpp::array<il2cpp::vector3> *>("_colors");
    auto colors2 = working->get<il2cpp::array<il2cpp::vector3> *>("_colors");
    if (colors1 == nullptr || colors2 == nullptr) [[unlikely]]
        return;
    if (colors1->size() == 0 || colors2->size() == 0) [[unlikely]]
        return;

    (*colors1)[0] = il2cpp::vector3{100.f, 100.f, 100.f};
    (*colors2)[0] = il2cpp::vector3{100.f, 100.f, 100.f};
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
    if (!e || !me)
        return;

    auto pos = utils::to_screen_position(
        e->position + il2cpp::vector2(e->width, e->height) / 2);
    set_cursor_position(world::to_real_screen(pos));
}

template <typename Entity> struct is_use_entity_traits {};

template <> struct is_use_entity_traits<npc> {
    bool operator()(npc *obj, bool is_attack_friendly) noexcept {
        return obj->life <= 0 || (is_attack_friendly == false && obj->friendly);
    }
};

template <> struct is_use_entity_traits<player> {
    bool operator()(player *obj, player *me) noexcept {
        return obj->get<int>("statLife") <= 0 || obj == me ||
               !obj->get<bool>("hostile");
    }
};

template <typename Entity, typename... Args>
Entity *get_target_entity(player *me, il2cpp::array<Entity *> *arr,
                          Args &&...args) noexcept {
    is_use_entity_traits<Entity> entity_traits{};

    Entity *target = nullptr;
    float min_dis = 100000.f;

    for (auto e : *arr) {
        if (e == nullptr)
            continue;
        if (entity_traits(e, std::forward<Args>(args)...))
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
        auto p = world::local_player();
        if (p == nullptr) [[unlikely]]
            return;

        if (cfg.npc()) {
            auto npcs = world::npcs();
            auto target =
                npcs ? get_target_entity(p, npcs, cfg.is_attack_friendly())
                     : nullptr;
            if (target)
                aim_target(p, reinterpret_cast<entity *>(target));
        }

        if (cfg.player()) {
            auto players = world::players();
            auto target = players ? get_target_entity(p, players, p) : nullptr;
            if (target)
                aim_target(p, reinterpret_cast<entity *>(target));
        }
    }
}

TL_Register_World_Call(auto_aim);

} // namespace func
} // namespace tl
