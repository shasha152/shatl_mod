#include "shatl/funtions/maininit.h"
#include "shatl/funtions/detail/class/lang.h"
#include "shatl/funtions/detail/class/player.h"
#include "shatl/funtions/detail/class/texture2D.h"
#include "shatl/funtions/detail/class/utils.h"
#include "shatl/funtions/detail/class/world.h"
#include "shatl/utils/log.h"
#include <algorithm>
#include <utility>
#include <vector>

namespace tl {
namespace func {

inline std::vector<std::pair<int, initializer_type>> init_funcs;

void initialize() noexcept {
    auto highest_count =
        std::count_if(init_funcs.begin(), init_funcs.end(),
                      [](const auto &v) { return v.first == 0; });
    if (highest_count != 1)
        LOGW("initializer highest count != 1,count=%ld", highest_count);
    std::sort(
        init_funcs.begin(), init_funcs.end(),
        [](const auto &v1, const auto &v2) { return v1.first < v2.first; });
    LOGI("初始化函数number:%lu", init_funcs.size());

    for (auto [_, f] : init_funcs)
        f();
}

bool push_init_func(int prio, initializer_type func) noexcept {
    init_funcs.emplace_back(prio, func);
    return true;
}
} // namespace func
} // namespace tl
