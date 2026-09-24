#include "shatl/funtions/detail/world.h"
#include "packet.pb.h"
#include "shatl/funtions/server.h"

namespace tl {
namespace func {

class _grab_world_item : public server_route_caller {
  public:
    bool handle() noexcept override {
        register_once_function([]() {
            auto p =
                world::instance()->static_call<player *>("get_LocalPlayer");
            auto items = world_item::world_items();
            if (items != nullptr) {
                for (auto item : *items)
                    p->call<void>("PickupItem", item);
            }
        });
        return true;
    }
};

TL_Register_Router(_grab_world_item, pro::cmd_grab_world_item);

} // namespace func
} // namespace tl