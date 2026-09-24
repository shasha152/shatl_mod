#include "shatl/funtions/config.h"
#include "shatl/funtions/detail/world.h"

namespace tl {
namespace func {

inline void set_life() {
    // if (pro::)
    static auto &data = config::ins().max_value[pro::max_value_type::life];

    if (data) {

        auto p = world::instance()->static_call<player *>("get_LocalPlayer");
        if (p) {
            p->set("statLife", data.value);
            p->set("statLifeMax", data.max);
        }
    }
}

inline void set_mana() {
    static auto &data = config::ins().max_value[pro::max_value_type::mana];

    if (data) {

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
    static auto &data = config::ins().float_value[pro::float_value_type::speed];

    if (data) {
        auto p = world::instance()->static_call<player *>("get_LocalPlayer");
        if (p) {
            p->set("moveSpeed", data.value);
        }
    }
}

TL_Register_World_Call(set_life);
TL_Register_World_Call(set_mana);
TL_Register_World_Call(set_speed);
TL_Register_World_Call(set_instantkill);

} // namespace func
} // namespace tl