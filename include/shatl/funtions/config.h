#include "player.pb.h"

#include <unordered_map>
#include <utility>

namespace tl {
namespace func {
struct config {
    std::unordered_map<int, pro::player_max_value> max_value{
        pro::max_value_type_MAX};
    std::unordered_map<int, pro::player_float_value> float_value{
        pro::float_value_type_MAX};
};
// namespace tl
} // namespace func
} // namespace tl