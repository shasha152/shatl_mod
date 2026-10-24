#pragma once

#include "shatl/il2cpp/il2cpp.h"
namespace tl {
namespace func {
struct local_user_game_state : il2cpp::object<local_user_game_state> {
    static local_user_game_state *instance() noexcept {
        static auto main_class = il2cpp::_class::create(
            il2cpp::assembly::create("Assembly-CSharp.dll"), "",
            "LocalUserGameState");
        if (main_class == nullptr) [[unlikely]]
            return nullptr;

        auto f = main_class->mfield("Instance");
        return f ? f->static_value<local_user_game_state *>() : nullptr;
    }
};

struct local_user_state_xna : il2cpp::object<local_user_state_xna> {
    static local_user_state_xna *instance() noexcept {
        static auto main_class = il2cpp::_class::create(
            il2cpp::assembly::create("Assembly-CSharp.dll"), "",
            "LocalUserXNAState");
        if (main_class == nullptr) [[unlikely]]
            return nullptr;

        auto f = main_class->mfield("Instance");
        return f ? f->static_value<local_user_state_xna *>() : nullptr;
    }

    
};
} // namespace func
} // namespace tl