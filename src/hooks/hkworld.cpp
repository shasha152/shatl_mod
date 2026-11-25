#include "shatl/funtions/config.h"
#include "shatl/funtions/detail/class/world.h"
#include "shatl/funtions/detail/register.h"
#include "shatl/funtions/maininit.h"
#include <dobby.h>


namespace tl {
namespace func {
install_hook_name(world_update, void, void *self, void *game_time) {
    world::_local_player =
        world::_main_class->mmethod("get_LocalPlayer")->invoke<player *>();
    for (auto &func : detail::static_register_function)
        func();

    invoke_once_function();

    return orig_world_update(self, game_time);
}

install_hook_name(PingMapAdd, void, ping_map *map, il2cpp::vector2 pos) {
    if (map != nullptr && config::ins().bool_value[pro::double_click_teleport])
        detail::double_click_minimap_to_teleport(pos);

    orig_PingMapAdd(map, pos);
}

inline void world_init() noexcept {
    while (!world::_main_class) {
        world::init();
        std::this_thread::yield();
    }

    while (!world::ready())
        std::this_thread::yield();

    LOGI("游戏已加载");

    auto w = world::instance();
    if (w == nullptr) [[unlikely]] {
        LOGE("取 Main.instance 失败，跳过钩子安装");
        return;
    }
    // DoUpdate(Microsoft.Xna.Framework.GameTime gameTime)
    auto update = w->get_method("DoUpdate", 1);
    if (update != nullptr)
        install_hook_world_update(update->get());
    else
        LOGE("未找到 Main.Update，世界回调不会运行");

    auto pings = w->static_get<ping_map *>("Pings");
    auto ping_add = pings ? pings->get_method("Add", 1) : nullptr;
    if (ping_add != nullptr)
        install_hook_PingMapAdd(ping_add->get());
    else
        LOGE("未找到 Pings.Add，双击传送不可用");
}

TL_Register_Init(PRIORITY_HIGHEST, world_init);
} // namespace func
} // namespace tl