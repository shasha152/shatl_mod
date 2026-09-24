#pragma once

#include "shatl/il2cpp/il2cpp.h"

namespace tl {
namespace func {
struct entity {
    int whoAmI;
    il2cpp::vector2 position;
    il2cpp::vector2 velocity;
    il2cpp::vector2 oldPosition;
    il2cpp::vector2 oldVelocity;
    int oldDirection;
    int direction;
    int width;
    int height;
    bool wet;
    bool shimmerWet;
    bool honeyWet;
    char wetCount;
    bool lavaWet;
};
} // namespace func
} // namespace tl