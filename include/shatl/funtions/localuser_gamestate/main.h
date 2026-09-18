#pragma once

#include "../config.h"
#include "data.h"

namespace tl {
namespace func {
inline void set_light_color() noexcept {
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
} // namespace func
} // namespace tl