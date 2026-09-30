#include "shatl/funtions/maininit.h"
#include "shatl/funtions/detail/lang.h"
#include "shatl/funtions/detail/player.h"
#include "shatl/funtions/detail/utils.h"
#include "shatl/funtions/detail/world.h"

void tl::func::initialize() noexcept {
    world_init();
    init_player();
    lang::init();
    utils::init();
}