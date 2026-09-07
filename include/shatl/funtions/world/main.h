#pragma once

#include "shatl/funtions/player/data.h"
#include "shatl/funtions/world/data.h"
#include "shatl/il2cpp/il2cpp.h"
#include "shatl/utils/config.h"
#include "shatl/utils/log.h"
#include <deque>
#include <dobby.h>
#include <functional>
#include <mutex>
#include <utility>
#include <vector>

namespace tl {
namespace func {
inline std::vector<std::function<void()>> static_register_function;
inline std::deque<std::function<void()>> once_function;
inline std::mutex once_function_mtx;

template <typename Func>
inline void register_once_function(Func &&func) noexcept {
    auto mfunc = std::function<void()>(std::forward<Func>(func));

    std::lock_guard lock(once_function_mtx);
    once_function.emplace_back(std::move(mfunc));
}

inline void invoke_once_function() noexcept {
    std::unique_lock lock(once_function_mtx);
    if (once_function.empty())
        return;

    auto &func = once_function.front();

    lock.unlock();
    func();
    lock.lock();
    once_function.pop_front();
}

namespace detail {
inline void double_click_minimap_to_teleport(il2cpp::vector2 pos) noexcept {
    auto p = world::instance()->static_call<player *>("get_LocalPlayer");
    p->position = (pos * 16);
}

} // namespace detail

install_hook_name(world_update, void, void *game_time) {
    // LOGI("%ld", register_function.size());
    for (auto &func : static_register_function)
        func();
    invoke_once_function();
    return orig_world_update(game_time);
}

install_hook_name(PingMapAdd, void, ping_map *map, il2cpp::vector2 pos) {
    STATIC_IF_CONFIG_BOOL("world.double_click_minimap_to_teleport") {
        map->call<void>("Clear");
        detail::double_click_minimap_to_teleport(pos);
    }
    orig_PingMapAdd(map, pos);
}

inline void log_player_addr() noexcept {
    auto p = world::instance()->static_call<void *>("get_LocalPlayer");
    LOGI("%p", p);
}

inline void grab_all_world_item() {
    auto p = world::instance()->static_call<player *>("get_LocalPlayer");
    auto items = world_item::world_items();
    if (items != nullptr) {
        for (auto item : *items)
            p->call<void>("PickupItem", item);
    }
}

inline void world_init() noexcept {
    install_hook_world_update(
        world::instance()->get_method("Update", 1)->get());
    install_hook_PingMapAdd(world::instance()
                                ->static_get<ping_map *>("Pings")
                                ->get_method("Add", 1)
                                ->get());
}

struct __register_function {
    template <typename Func> explicit __register_function(Func &&func) {
        static_register_function.emplace_back(std::forward<Func>(func));
    }
};
} // namespace func
} // namespace tl

#define TL_Register_World_Call(fun)                                            \
    static ::tl::func::__register_function reg__##fun { fun }