#pragma once

#include "entity.h"
#include "shatl/il2cpp/il2cpp.h"

#include <string>

namespace tl {
namespace func {

struct player : il2cpp::object<player>, entity_ex {
    bool is_active() noexcept { return get<int>("statLife") > 0; }

    std::string get_name() noexcept {
        return get<il2cpp::string *>("name")->to_string();
    }
};

void after_reset_effects(player *) noexcept;

} // namespace func
} // namespace tl