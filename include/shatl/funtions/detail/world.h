#pragma once

#include "item.h"
#include "player.h"
#include "shatl/funtions/config.h"
#include "shatl/il2cpp/il2cpp.h"
#include <deque>
#include <dobby.h>
#include <functional>
#include <future>


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

inline std::vector<std::function<void()>> static_register_function;
inline std::deque<std::function<void()>> once_function;
inline std::mutex once_function_mtx;

template <typename Func>
std::future<std::invoke_result_t<Func>> register_once_function(Func &&func) {
    using ReturnType = std::invoke_result_t<Func>;

    auto promise = std::make_shared<std::promise<ReturnType>>();
    auto future = promise->get_future();

    auto task = [func = std::forward<Func>(func), promise]() mutable {
        try {
            if constexpr (std::is_void_v<ReturnType>) {
                std::invoke(std::move(func));
                promise->set_value();
            } else {
                promise->set_value(std::invoke(std::move(func)));
            }
        } catch (...) {
            promise->set_exception(std::current_exception());
        }
    };
    {
        std::lock_guard lock(once_function_mtx);
        once_function.emplace_back(std::move(task));
    }

    return future;
}

inline void invoke_once_function() noexcept {
    std::function<void()> func;

    {
        std::lock_guard lock(once_function_mtx);
        if (once_function.empty())
            return;

        func = std::move(once_function.front());
        once_function.pop_front();
    }

    func();
}

namespace detail {
inline void double_click_minimap_to_teleport(il2cpp::vector2 pos) noexcept {
    auto p = world::instance()->static_call<player *>("get_LocalPlayer");
    p->position = (pos * 16);
}

} // namespace detail

install_hook_name(world_update, void, void *game_time) {
    for (auto &func : static_register_function)
        func();

    invoke_once_function();

    return orig_world_update(game_time);
}

install_hook_name(PingMapAdd, void, ping_map *map, il2cpp::vector2 pos) {
    if (config::ins().bool_value[pro::double_click_teleport]) {
        map->call<void>("Clear");
        detail::double_click_minimap_to_teleport(pos);
    }
    orig_PingMapAdd(map, pos);
}

inline void log_player_addr() noexcept {
    auto p = world::instance()->static_call<void *>("get_LocalPlayer");
    LOGI("%p", p);
}


inline il2cpp::array<item *> *get_local_player_bag() {
    auto player_future = register_once_function([]() {
        return world::instance()->static_call<player *>("get_LocalPlayer");
    });
    player_future.wait();
    return player_future.get()->get<il2cpp::array<item *> *>("inventory");
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

#define TL_Register_World_Call(fun)                                            \
    static ::tl::func::__register_function reg__##fun { fun }

} // namespace func
} // namespace tl