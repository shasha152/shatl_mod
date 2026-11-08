#include "shatl/funtions/detail/npc.h"
#include "packet.pb.h"
#include "shatl/funtions/detail/lang.h"
#include "shatl/funtions/detail/world.h"
#include "shatl/funtions/webservice.h"
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

class _get_npc_name : public server_route_caller {
    bool handle(pro::confirm &confirm) noexcept override {
        pro::RESitem_names res;
        auto localized_texts = lang::npc_localized_texts();
        if (localized_texts != nullptr) {
            for (auto text : *localized_texts) {
                if (text == nullptr || text->value == nullptr) [[unlikely]]
                    continue;

                auto name = res.add_names();
                *name = text->value->to_string();
            }
        }

        confirm.set_ok(true);
        confirm.set_data(res.SerializeAsString());

        return true;
    }
};
TL_Register_Router(_get_npc_name, pro::cmd_get_npc_name);
} // namespace func
} // namespace tl