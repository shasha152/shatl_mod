#pragma once

#include "shatl/il2cpp/il2cpp.h"
namespace tl {
namespace func {
struct world : il2cpp::object<world> {
    static world *instance() noexcept {
        static auto main_class = il2cpp::_class::create(
            il2cpp::assembly::create("Assembly-CSharp.dll"), "Terraria",
            "Main");
        return main_class->mfield("instance")->static_value<world *>();
    }
};

struct world_item : il2cpp::object<world_item> {

    static il2cpp::array<world_item *> *world_items() noexcept {
        auto items =
            world::instance()->static_get<il2cpp::array<world_item *> *>(
                "item");
        return items;
    }
};

struct star : il2cpp::object<star> {
    static void star_fall(float posX) noexcept {
        static_call2<void>("Assembly-CSharp.dll", "Terraria", "Star",
                           "SpawnStars", (int)posX);
    }
};

struct ping_map : il2cpp::object<ping_map> {
    // static ping_map* main_ping_map() noexcept {
    //     // world.double_click_minimap_to_teleport
        
    // }
};

} // namespace func
} // namespace tl