#include "shatl/funtions/config.h"
#include "shatl/funtions/detail/class/player.h"
#include "shatl/funtions/maininit.h"

#include <dobby.h>

namespace tl {
namespace func {
// System.Double Hurt(Terraria.DataStructures.PlayerDeathReason damageSource,
// System.Int32 Damage, System.Int32 hitDirection, System.Boolean pvp,
// System.Boolean quiet, System.Boolean Crit, System.Int32 cooldownCounter,
// System.Boolean dodgeable); // 0x1281A08
install_hook_name(Hurt, double, void *self, void *p1, int i2, int i3, bool b4,
                  bool b5, bool b6, int i7, bool b8) {
    if (config::get<cmax_t>(pro::life)) {
        return 0;
    }

    return orig_Hurt(self, p1, i2, i3, b4, b5, b6, i7, b8);
}

install_hook_name(TileCollision, il2cpp::vector2, player *self,
                  il2cpp::vector2 position, il2cpp::vector2 velocity,
                  bool fallThrough, bool ignorePlats) {
    if (config::get<cbool_t>(pro::noclip)) {
        orig_TileCollision(self, position, velocity, true, true);
        return self->velocity;
    }
    return orig_TileCollision(self, position, velocity, fallThrough,
                              ignorePlats);
}

install_hook_name(ResetEffects, void, player *self) {
    orig_ResetEffects(self);
    after_reset_effects(self);
}

inline void init_player() {
    auto cls = il2cpp::assembly::create("Assembly-CSharp.dll")
                   ->mclass("Terraria", "Player");
    auto hurt = cls ? cls->mmethod("Hurt") : nullptr;
    if (hurt == nullptr) [[unlikely]] {
        LOGE("未找到 Player.Hurt，免疫伤害不可用");
        return;
    }

    install_hook_Hurt(hurt->get());

    // Microsoft.Xna.Framework.Vector2 TileCollision(struct
    // Microsoft.Xna.Framework.Vector2 position, struct
    // Microsoft.Xna.Framework.Vector2 velocity, System.Boolean fallThrough,
    // System.Boolean ignorePlats); // 0x12B1358
    auto tileCollision = cls->mmethod("TileCollision");
    if (tileCollision == nullptr) [[unlikely]] {
        LOGE("未找到 Player.TileCollision");
        return;
    }
    install_hook_TileCollision(tileCollision->get());

    auto resetEffects = cls->mmethod("ResetEffects");
    if (resetEffects == nullptr) [[unlikely]] {
        LOGE("未找到Player.ResetEffects");
        return;
    }

    install_hook_ResetEffects(resetEffects->get());
}

TL_Register_Init(PRIORITY_LOW, init_player);
} // namespace func
} // namespace tl