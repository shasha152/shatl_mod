#include "player.pb.h"
#include "shatl/utils/log.h"

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace tl {
namespace func {

struct max_value {
    int max;
    int value;
    bool is_open;

    std::string to_string() {
        return "(" + std::to_string(max) + "," + std::to_string(value) + "," +
               std::to_string(is_open) + ")";
    }
};

struct float_value {
    float value;
    bool is_open;
};

struct config {
    std::vector<max_value> max_value{pro::max_value_type_MAX + 1};
    std::vector<float_value> float_value{pro::float_value_type_MAX + 1};

    static config &ins() noexcept {
        static config c{};
        // LOGI("%zu", c.max_value.size());
        return c;
    }
};
// namespace tl
} // namespace func
} // namespace tl