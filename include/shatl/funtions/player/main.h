#pragma once

#include "../config.h"
#include "data.h"
#include "shatl/funtions/world/data.h"

namespace tl {
namespace func {

inline void set_life() {
    // if (pro::)
    static auto &data = config::ins().max_value[pro::max_value_type::life];

    if (data.is_open) {

        auto p = world::instance()->static_call<player *>("get_LocalPlayer");
        if (p) {
            p->set("statLife", data.value);
            p->set("statLifeMax", data.max);
        }
    }
}

inline void set_mana() {
    static auto &data = config::ins().max_value[pro::max_value_type::mana];

    if (data.is_open) {

        auto p = world::instance()->static_call<player *>("get_LocalPlayer");
        if (p) {
            p->set("statMana", data.value);
            p->set("statManaMax", data.max);
        }
    }
}

inline void set_instantkill() {
    // STATIC_IF_CONFIG_BOOL("player.instantkill") {
    //     auto p = world::instance()->static_call<player *>("get_LocalPlayer");
    //     if (p) {
    //         p->set("meleeDamage", 99999.0f);
    //         p->set("magicDamage", 99999.0f);
    //         p->set("rangedDamage", 99999.0f);
    //         p->set("minionDamage", 99999.0f);
    //     }
    // }
}

inline void set_speed() {
    static auto &data = config::ins().max_value[pro::max_value_type::life];

    if (data.is_open) {

        auto p = world::instance()->static_call<player *>("get_LocalPlayer");
        if (p) {
            p->set("moveSpeed", data.value);
        }
    }
}

// PickupItem

} // namespace func
} // namespace tl