#pragma once

#include "entity.h"
#include "item.h"
#include "npc.h"
#include "player.h"
#include "shatl/funtions/config.h"
#include "shatl/funtions/detail/player.h"
#include "shatl/il2cpp/il2cpp.h"
#include "shatl/imgui/detail/gui.h"
#include <deque>
#include <dobby.h>
#include <functional>
#include <future>

namespace tl {
namespace func {
struct world : il2cpp::object<world> {
    inline static il2cpp::_class *_main_class = nullptr;
    inline static player *_local_player = nullptr;

    static void init() noexcept {
        _main_class = il2cpp::_class::create(
            il2cpp::assembly::create("Assembly-CSharp.dll"), "Terraria",
            "Main");
        if (_main_class == nullptr) [[unlikely]]
            LOGE("未找到 Terraria.Main，游戏版本不匹配？");
    }
    static world *instance() noexcept {
        if (_main_class == nullptr) [[unlikely]]
            return nullptr;

        auto f = _main_class->mfield("instance");
        return f ? f->static_value<world *>() : nullptr;
    }

    static il2cpp::array<npc *> *npcs() noexcept {
        if (_main_class == nullptr) [[unlikely]]
            return nullptr;

        auto f = _main_class->mfield("npc");
        return f ? f->static_value<il2cpp::array<npc *> *>() : nullptr;
    }

    static il2cpp::array<player *> *players() noexcept {
        if (_main_class == nullptr) [[unlikely]]
            return nullptr;

        auto f = _main_class->mfield("player");
        return f ? f->static_value<il2cpp::array<player *> *>() : nullptr;
    }

    static il2cpp::vector2 to_real_screen(il2cpp::vector2 pos) noexcept {
        if (_main_class == nullptr) [[unlikely]]
            return pos;

        auto screen_width = _main_class->mmethod("get_screenWidth");
        auto screen_height = _main_class->mmethod("get_screenHeight");
        if (screen_width == nullptr || screen_height == nullptr) [[unlikely]]
            return pos;

        float scale_x =
            (float)im::detail::get_screen_width() / screen_width->invoke<int>();
        float scale_y = (float)im::detail::get_screen_height() /
                        screen_height->invoke<int>();

        return pos * il2cpp::vector2(scale_x, scale_y);
    }

    static player *local_player() noexcept {
        if (_main_class == nullptr) [[unlikely]]
            return nullptr;
        return _local_player;
    }
};

struct world_item : il2cpp::object<world_item> {

    static il2cpp::array<world_item *> *world_items() noexcept {
        auto w = world::instance();
        if (w == nullptr) [[unlikely]]
            return nullptr;

        return w->static_get<il2cpp::array<world_item *> *>("item");
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
    invoke_once_function();
}

namespace detail {
inline void double_click_minimap_to_teleport(il2cpp::vector2 pos) noexcept {
    auto p = world::local_player();
    if (p == nullptr) [[unlikely]]
        return;

    p->position = (pos * 16);
}

} // namespace detail

install_hook_name(world_update, void, void *game_time) {
    world::_local_player =
        world::_main_class->mmethod("get_LocalPlayer")->invoke<player *>();
    for (auto &func : static_register_function)
        func();

    invoke_once_function();

    return orig_world_update(game_time);
}

install_hook_name(PingMapAdd, void, ping_map *map, il2cpp::vector2 pos) {
    if (map != nullptr && config::ins().bool_value[pro::double_click_teleport])
        detail::double_click_minimap_to_teleport(pos);

    orig_PingMapAdd(map, pos);
}

inline void log_player_addr() noexcept { LOGI("%p", world::local_player()); }

inline il2cpp::array<item *> *get_local_player_bag() {
    auto p = world::local_player();
    if (p == nullptr) [[unlikely]]
        return nullptr;

    return p->get<il2cpp::array<item *> *>("inventory");
}

inline void world_init() noexcept {
    world::init();

    auto w = world::instance();
    if (w == nullptr) [[unlikely]] {
        LOGE("取 Main.instance 失败，跳过钩子安装");
        return;
    }

    auto update = w->get_method("Update", 1);
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

struct __register_function {
    template <typename Func> explicit __register_function(Func &&func) {
        static_register_function.emplace_back(std::forward<Func>(func));
    }
};

#define TL_Register_World_Call(fun)                                            \
    static ::tl::func::__register_function reg__##fun { fun }

} // namespace func
} // namespace tl