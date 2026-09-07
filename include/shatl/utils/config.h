#pragma once

#include "shatl/funtions/config.h"

namespace tl {
namespace utils {
inline static func::config g_config{};
}
} // namespace tl

#define STATIC_IF_CONFIG(key, type, value)                                     \
    static type &__value_##type =                                              \
        ::tl::utils::g_config[key].get_ref<type &>();                          \
    if (__value_##type == value)

#define STATIC_IF_CONFIG_BOOL(key) STATIC_IF_CONFIG(key, bool, true)

#define GET_PTR_CONFIG_VALUE(key, type)                                        \
    &::tl::utils::g_config[key].get_ref<type &>()
#define GET_PTR_CONFIG_VALUE_BOOL(key) GET_PTR_CONFIG_VALUE(key, bool)