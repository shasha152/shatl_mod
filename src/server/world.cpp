#include "shatl/funtions/detail/class/world.h"
#include "packet.pb.h"
#include "shatl/funtions/config.h"
#include "shatl/funtions/detail/map_reveal.h"
#include "shatl/funtions/webservice.h"
#include "shatl/utils/log.h"
#include <memory>

namespace tl {
namespace func {

class _grab_world_item : public server_route_caller {
  public:
    bool handle() noexcept override {
        register_once_function([]() {
            auto p = world::local_player();
            if (p == nullptr) [[unlikely]]
                return;

            auto items = world_item::world_items();
            if (items == nullptr)
                return;

            for (auto item : *items) {
                if (item == nullptr) [[unlikely]]
                    continue;

                p->call<void>("PickupItem", item);
            }
        });
        return true;
    }
};

TL_Register_Router(_grab_world_item, pro::cmd_grab_world_item);

class _auto_aim_entity : public server_route_caller {
    bool handle() noexcept override {
        auto value = as<pro::REQauto_aim>();
        if (value) [[likely]] {
            config::ins().auto_aim.CopyFrom(value.value());
        }
        return true;
    }
};

TL_Register_Router(_auto_aim_entity, pro::cmd_auto_aim);

class _draw_object : public server_route_caller {
    bool handle() noexcept override {
        auto value = as<pro::REQdraw_object>();
        if (!value)
            return false;

        config::ins().draw.CopyFrom(value.value());
        return true;
    }
};

TL_Register_Router(_draw_object, pro::cmd_draw_object);

class _map_reveal : public server_route_caller {
    std::shared_ptr<map_reveal> m;

  public:
    _map_reveal() noexcept : m(std::make_shared<map_reveal>()) {}

    bool handle() noexcept override {
        if (m->is_run())
            return false;

        m->run();
        return true;
    }
};

TL_Register_Router(_map_reveal, pro::cmd_map_reveal);

} // namespace func
} // namespace tl