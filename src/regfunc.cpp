#include "shatl/funtions/localuser_gamestate/main.h"
#include "shatl/funtions/player/main.h"
#include "shatl/funtions/world/main.h"

namespace tl {
namespace func {
TL_Register_World_Call(set_life);
TL_Register_World_Call(set_mana);
TL_Register_World_Call(set_speed);
TL_Register_World_Call(set_instantkill);
TL_Register_World_Call(set_light_color);
} // namespace func
} // namespace tl
