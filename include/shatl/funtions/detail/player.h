#pragma once

#include "entity.h"
#include "player.pb.h"
#include "shatl/funtions/config.h"
#include "shatl/il2cpp/il2cpp.h"

#include <dobby.h>

namespace tl {
namespace func {

struct player : il2cpp::object<player>, entity_ex {
    bool is_active() noexcept { return get<int>("statLife") > 0; }
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

inline void init_player() {
    auto cls = il2cpp::assembly::create("Assembly-CSharp.dll")
                   ->mclass("Terraria", "Player");
    auto hurt = cls ? cls->mmethod("Hurt") : nullptr;
    if (hurt == nullptr) [[unlikely]] {
        LOGE("未找到 Player.Hurt，免疫伤害不可用");
        return;
    }

    install_hook_Hurt(hurt->get());
}

} // namespace func
} // namespace tl