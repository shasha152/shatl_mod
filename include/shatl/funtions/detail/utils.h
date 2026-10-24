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
        if (main_class == nullptr) [[unlikely]]
            LOGE("未找到 Terraria.Utils，自动瞄准不可用");
    }

    // Microsoft.Xna.Framework.Vector2 ToScreenPosition(struct
    // Microsoft.Xna.Framework.Vector2 worldPosition);
    static il2cpp::vector2 to_screen_position(il2cpp::vector2 pos) noexcept {
        static auto mtd = main_class ? main_class->mmethod("ToScreenPosition", 1)
                                     : nullptr;
        if (mtd == nullptr) [[unlikely]]
            return pos;

        return mtd->invoke<il2cpp::vector2>(pos);
    }
};
} // namespace func
} // namespace tl