#pragma once

#include "shatl/il2cpp/il2cpp.h"
namespace tl {
namespace func {
struct local_user_game_state : il2cpp::object<local_user_game_state> {
    static local_user_game_state *instance() noexcept {
        static auto main_class = il2cpp::_class::create(
            il2cpp::assembly::create("Assembly-CSharp.dll"), "",
            "LocalUserGameState");
        return main_class->mfield("Instance")
            ->static_value<local_user_game_state *>();
    }
};

struct local_user_state_xna : il2cpp::object<local_user_state_xna> {
    static local_user_state_xna *instance() noexcept {
        static auto main_class = il2cpp::_class::create(
            il2cpp::assembly::create("Assembly-CSharp.dll"), "",
            "LocalUserXNAState");
        return main_class->mfield("Instance")
            ->static_value<local_user_state_xna *>();
    }
};
} // namespace func
} // namespace tl