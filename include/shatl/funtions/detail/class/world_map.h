#pragma once

#include "shatl/funtions/detail/class/world.h"
#include "shatl/il2cpp/il2cpp.h"
namespace tl {
namespace func {
struct world_map : il2cpp::object<world_map> {
    static world_map *map() noexcept {
        return world::_main_class->mmethod("get_Map")->invoke<world_map *>();
    }
    // UpdateLighting
    void update_lighting(int x, int y, char light) {
        call<void>("UpdateLighting", x, y, light);
    }

    void save() { call<void>("Save", false); }

    int max_width() { return get<int>("MaxWidth"); }
    int max_height() { return get<int>("MaxHeight"); }
};

} // namespace func

} // namespace tl