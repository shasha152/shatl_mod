#pragma once

#include "entity.h"
#include "player.pb.h"
#include "shatl/funtions/config.h"
#include "shatl/il2cpp/il2cpp.h"
#include "shatl/utils/log.h"

#include <dobby.h>
#include <string>

namespace tl {
namespace func {

struct player : il2cpp::object<player>, entity_ex {
    bool is_active() noexcept { return get<int>("statLife") > 0; }

    std::string get_name() noexcept {
        return get<il2cpp::string *>("name")->to_string();
    }
};
// System.Double Hurt(Terraria.DataStructures.PlayerDeathReason damageSource,
// System.Int32 Damage, System.Int32 hitDirection, System.Boolean pvp,
// System.Boolean quiet, System.Boolean Crit, System.Int32 cooldownCounter,
// System.Boolean dodgeable); // 0x1281A08
install_hook_name(Hurt, double, void *self, void *p1, int i2, int i3, bool b4,
                  bool b5, bool b6, int i7, bool b8) {
    if (config::ins().max_value[pro::life]) {
        return 0;
    }

    return orig_Hurt(self, p1, i2, i3, b4, b5, b6, i7, b8);
}

install_hook_name(TileCollision, il2cpp::vector2, player *self,
                  il2cpp::vector2 position, il2cpp::vector2 velocity,
                  bool fallThrough, bool ignorePlats) {
    if (config::ins().bool_value[pro::bool_value_type::noclip]) {
        orig_TileCollision(self, position, velocity, true, true);
        return self->velocity;
    }
    return orig_TileCollision(self, position, velocity, fallThrough,
                              ignorePlats);
}

void after_reset_effects() noexcept;

install_hook_name(ResetEffects, void, void *self) {
    orig_ResetEffects(self);
    after_reset_effects();
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

} // namespace func
} // namespace tl