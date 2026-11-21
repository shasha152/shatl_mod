#include "shatl/funtions/maininit.h"
#include "shatl/funtions/detail/class/lang.h"
#include "shatl/funtions/detail/class/player.h"
#include "shatl/funtions/detail/class/texture2D.h"
#include "shatl/funtions/detail/class/utils.h"
#include "shatl/funtions/detail/class/world.h"
#include <vector>

namespace tl {
namespace func {

inline std::vector<initializer_type> init_funcs;

void initialize() noexcept {
    for (auto f : init_funcs)
        f();
    world_init();
    init_player();
    lang::init();
    utils::init();
    npc::init();
    tex::init();
}

bool push_init_func(initializer_type func) noexcept {
    init_funcs.push_back(func);
    return true;
}
} // namespace func
} // namespace tl
