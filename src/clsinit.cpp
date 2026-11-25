#include "shatl/funtions/detail/class/lang.h"
#include "shatl/funtions/detail/class/npc.h"
#include "shatl/funtions/detail/class/utils.h"
#include "shatl/funtions/maininit.h"

namespace tl {
namespace func {

void init_class() noexcept {
    lang::init();
    utils::init();
    npc::init();
}

TL_Register_Init(PRIORITY_MEDIUM, init_class);
} // namespace func
} // namespace tl