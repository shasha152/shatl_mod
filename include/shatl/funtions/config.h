#pragma once

#include "multi_player.pb.h"
#include "player.pb.h"
#include "setting.pb.h"

#include <array>
#include <vector>

namespace tl {
namespace func {

struct enable {
    bool is_enable;

    explicit operator bool() const noexcept { return is_enable; }
};

template <typename T> struct max_value : enable {
    T max;
    T value;
};

template <typename T> struct value : enable {
    T value;
};

struct config {
    std::vector<max_value<int>> max_value{pro::max_value_type_MAX + 1};
    std::vector<value<float>> float_value{pro::float_value_type_MAX + 1};
    std::array<bool, pro::float_value_type_MAX + 1> bool_value;
    pro::REQauto_aim auto_aim;

    static config &ins() noexcept {
        static config c{};
        // LOGI("%zu", c.max_value.size());
        return c;
    }

  private:
    config() noexcept {
        bool_value.fill(false);
        auto_aim.set_is_open(false);
    }
};

// namespace tl
} // namespace func
} // namespace tl