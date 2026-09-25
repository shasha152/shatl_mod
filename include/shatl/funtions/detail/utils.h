#pragma once
#include "shatl/il2cpp/il2cpp.h"
namespace tl {
namespace func {
struct utils {
    inline static il2cpp::_class *main_class = nullptr;
    static void init() noexcept {
        // Terraria.Utils
        main_class = il2cpp::assembly::create("Assembly-CSharp.dll")
                         ->mclass("Terraria", "Utils");
    }

    // Microsoft.Xna.Framework.Vector2 ToScreenPosition(struct
    // Microsoft.Xna.Framework.Vector2 worldPosition);
    static il2cpp::vector2 to_screen_position(il2cpp::vector2 pos) noexcept {
        static auto mtd = main_class->mmethod("ToScreenPosition", 1);
        return mtd->invoke<il2cpp::vector2>(pos);
    }
};
} // namespace func
} // namespace tl