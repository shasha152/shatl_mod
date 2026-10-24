#include "shatl/funtions/detail/npc.h"
#include "packet.pb.h"
#include "shatl/funtions/detail/world.h"
#include "shatl/funtions/service.h"
#include "shatl/il2cpp/il2cpp.h"
#include "world.pb.h"

namespace tl {
namespace func {
class _new_npc : public server_route_caller {
    bool handle() noexcept override {
        func::register_once_function([msg = as<pro::REQnew_npc>()]() {
            auto player = world::local_player();
            auto pos =
                il2cpp::vector2(player->position.x, player->position.y - 50);
            npc::spawn(pos, msg->type(), msg->target());
        });

        return true;
    }
};

TL_Register_Router(_new_npc, pro::cmd_new_npc);
} // namespace func
} // namespace tl