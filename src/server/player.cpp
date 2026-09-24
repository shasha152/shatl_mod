#include "packet.pb.h"
#include "shatl/funtions/detail/lang.h"
#include "shatl/funtions/detail/world.h"
#include "shatl/funtions/server.h"
#include "shatl/utils/log.h"

namespace tl {
namespace func {
class _bool_value_setting : public server_route_caller {
  public:
    bool handle() noexcept override {
        auto value = as<pro::REQbool_value>();

        if (value) [[likely]] {
            config::ins().bool_value[value->type()] = value->is_open();
        }
        return true;
    }
};

TL_Register_Router(_bool_value_setting, pro::cmd_player_bool_value);

class _float_value_setting : public server_route_caller {
  public:
    bool handle() noexcept override {
        auto value = as<pro::REQfloat_value>();

        if (value) [[likely]] {
            auto &fv = config::ins().float_value[value->type()];
            fv.value = value->value();
            fv.is_enable = value->is_open();
        }
        return true;
    }
};

TL_Register_Router(_float_value_setting, pro::cmd_player_float_value);

class _max_value_setting : public server_route_caller {
  public:
    bool handle() noexcept override {
        LOGI("handler");
        auto value = as<pro::REQplayer_max_value>();
        if (value) [[likely]] {
            auto &data = config::ins().max_value[value->type()];
            data.value = value->value();
            data.is_enable = value->is_open();
            data.max = value->max();
        }
        return true;
    }
};

TL_Register_Router(_max_value_setting, pro::cmd_player_max_value);

class _set_item_type : public server_route_caller {
  public:
    bool handle() noexcept override {
        auto value = as<pro::REQset_item_type>();
        auto bag = func::get_local_player_bag();

        if (bag) {
            auto item = (*bag)[value->index()];
            if (item->stack == 0)
                item->stack = 1;
            item->change_item(value->type());
        }

        return true;
    }
};

TL_Register_Router(_set_item_type, pro::cmd_set_item_type);

class _get_player_bag : public server_route_caller {
    bool handle(pro::confirm &confirm) noexcept override {
        auto value = as<pro::REQplayer_get_bag>();

        il2cpp::array<item *> *items = nullptr;
        if (value->me()) {
            items = get_local_player_bag();
            LOGI("get_me_bag:%zu", items->size());
        }

        if (items) {
            pro::RESplayer_get_bag res;
            for (auto item : *items) {
                auto res_item = res.add_items();
                res_item->set_stack(item->stack);
                res_item->set_type(item->type);
                res_item->set_maxstack(item->maxStack);
                res_item->set_crit(item->crit);
                res_item->set_defense(item->defense);
                res_item->set_damage(item->damage);
                res_item->set_knockback(item->knockBack);
                res_item->set_shoot(item->shoot);
                res_item->set_shootspeed(item->shootSpeed);
                res_item->set_usetime(item->useTime);
                res_item->set_scale(item->scale);
            }

            confirm.set_ok(true);
            confirm.set_data(res.SerializeAsString());
        } else {
            confirm.set_ok(false);
        }

        return true;
    }
};

TL_Register_Router(_get_player_bag, pro::cmd_player_get_bag);

class _set_item_pro : public server_route_caller {
    bool handle() noexcept override {
        auto value = as<pro::REQset_item_property>();

        auto &item = value->sitem();
        auto index = value->index();
        LOGI("%s", item.DebugString().c_str());
        if (index > 59 || index < 0)
            return true;

        auto game_item = (*get_local_player_bag())[index];
        if (game_item->maxStack < item.stack())
            game_item->maxStack = 9999;
        game_item->stack = item.stack();
        game_item->crit = item.crit();
        game_item->scale = item.scale();
        game_item->knockBack = item.knockback();
        game_item->shoot = item.shoot();
        game_item->shootSpeed = item.shootspeed();
        game_item->damage = item.damage();
        game_item->defense = item.defense();
        game_item->useTime = item.usetime();

        return true;
    }
};

TL_Register_Router(_set_item_pro, pro::cmd_set_item_pro);

class _get_item_names : public server_route_caller {
    bool handle(pro::confirm &confirm) noexcept override {
        pro::RESitem_names res;
        auto localized_texts = lang::get_item_localized_texts();
        if (localized_texts) {
            for (auto text : *localized_texts) {
                auto name = res.add_names();
                *name = text->value->to_string();
            }
        }

        confirm.set_ok(true);
        confirm.set_data(res.SerializeAsString());

        return true;
    }
};

TL_Register_Router(_get_item_names, pro::cmd_get_item_name);

} // namespace func
} // namespace tl