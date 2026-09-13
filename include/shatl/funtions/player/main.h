#pragma once

#include "data.h"
#include "shatl/funtions/world/main.h"
// #include "shatl/utils/config.h"


namespace tl {
namespace func {

inline void set_life() {
    // if (pro::)

    // STATIC_IF_CONFIG_BOOL("player.lifefrezze")  {
    //     auto p = world::instance()->static_call<player *>("get_LocalPlayer");
    //     if (p)
    //         p->set("statLife", 500);
    // }
}

inline void set_mana() {
    // STATIC_IF_CONFIG_BOOL("player.manafrezze") {
    //     auto p = world::instance()->static_call<player *>("get_LocalPlayer");
    //     if (p)
    //         p->set("statMana", 500);
    // }
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

// PickupItem

} // namespace func
} // namespace tl