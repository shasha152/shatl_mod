#include "shatl/funtions/maininit.h"
#include "shatl/funtions/detail/class/lang.h"
#include "shatl/funtions/detail/class/player.h"
#include "shatl/funtions/detail/class/texture2D.h"
#include "shatl/funtions/detail/class/utils.h"
#include "shatl/funtions/detail/class/world.h"

void tl::func::initialize() noexcept {
    world_init();
    init_player();
    lang::init();
    utils::init();
    npc::init();
    tex::init();
}