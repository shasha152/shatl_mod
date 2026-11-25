#pragma once

#include "esp.pb.h"
#include "multi_player.pb.h"
#include "player.pb.h"
#include "setting.pb.h"

#include <array>
#include <cstddef>
#include <type_traits>
#include <vector>

#include "shatl/utils/reflection.h"

namespace tl {
namespace func {

struct enable {
    bool is_enable;

    explicit operator bool() const noexcept { return is_enable; }
};

template <typename T> struct max_value : enable {
    using value_type = T;
    T max;
    T value;
};

template <typename T> struct value : enable {
    using value_type = T;
    T value;
};

using cfloat_t = std::vector<value<float>>;
using cmax_t = std::vector<max_value<int>>;
using cbool_t = std::array<bool, pro::bool_value_type_MAX + 1>;
using caim_t = pro::REQauto_aim;
using cdobj_t = pro::REQdraw_object;

struct config {
    cmax_t int_max_value{pro::max_value_type_MAX + 1};
    cfloat_t float_value{pro::float_value_type_MAX + 1};
    cbool_t bool_value;
    caim_t auto_aim;
    cdobj_t draw;

    static config &ins() noexcept {
        static config c{};
        return c;
    }

    template <typename T> static const T &get() noexcept {
        return tl::get<T>(ins());
    }
    template <typename T> static T &ref() noexcept { return tl::get<T>(ins()); }

    template <typename T>
    static typename T::value_type &ref(std::size_t i) noexcept {
        return tl::get<T>(ins())[i];
    }

    template <typename T>
    static const typename T::value_type &get(std::size_t i) noexcept {
        return tl::get<T>(ins())[i];
    }

  private:
    // config() noexcept {
    //     bool_value.fill(false);
    //     auto_aim.set_is_open(false);
    //     draw.set_draw_npc(false);
    //     draw.set_draw_player(false);
    //     draw.set_draw_world_item(false);
    // }
};

// namespace tl
} // namespace func
} // namespace tl