#include "shatl/funtions/maininit.h"
#include "shatl/funtions/lang/data.h"
#include "shatl/funtions/world/main.h"

void tl::func::initialize() noexcept {
    world_init();
    lang::init();
}