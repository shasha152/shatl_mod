#include "packet.pb.h"
#include "setting.pb.h"
#include "shatl/funtions/detail/lang.h"
#include "shatl/funtions/detail/world.h"
#include "shatl/funtions/service.h"
#include "shatl/utils/freeze.h"
#include "shatl/utils/log.h"

namespace tl {
namespace func {
class _bool_value_setting : public server_route_caller {

    bool handle() noexcept override {
        auto value = as<pro::REQbool_value>();

        if (value) [[likely]] {
            auto type = value->type();
            if (type < 0 || type >= pro::bool_value_type_MAX + 1)
                return true;

            config::ins().bool_value[type] = value->is_open();
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
            auto type = value->type();
            if (type < 0 || type >= pro::float_value_type_MAX + 1)
                return true;

            auto &fv = config::ins().float_value[type];
            if (type == pro::speed) {
                local_player_speed(value->value(), value->is_open());
            } else {
                fv.value = value->value();
                fv.is_enable = value->is_open();
            }
        }
        return true;
    }

  private:
    void local_player_speed(float value, bool enable) {
        if (enable) {
            utils::g_freeze_worker.set(
                [value]() {
                    auto p = world::local_player();
                    if (p == nullptr) [[unlikely]]
                        return;

                    p->set("moveSpeed", value);
                },
                utils::ft::speed);
        } else {
            utils::g_freeze_worker.clear(utils::ft::speed);
        }
    }
};

TL_Register_Router(_float_value_setting, pro::cmd_player_float_value);

class _max_value_setting : public server_route_caller {

    bool handle() noexcept override {
        auto value = as<pro::REQplayer_max_value>();
        if (value) [[likely]] {
            auto type = value->type();
            if (type < 0 || type >= pro::max_value_type_MAX + 1)
                return true;

            auto &data = config::ins().max_value[type];
            data.value = value->value();
            data.is_enable = value->is_open();
            data.max = value->max();
        }
        return true;
    }
};

TL_Register_Router(_max_value_setting, pro::cmd_player_max_value);

class _set_item_type : public server_route_caller {
    bool handle() noexcept override {
        auto value = as<pro::REQset_item_type>();
        if (!value) [[unlikely]]
            return true;

        auto bag = func::get_local_player_bag();
        if (bag == nullptr) [[unlikely]] {
            LOGW("本地玩家未就绪，忽略设置物品类型");
            return true;
        }

        auto index = value->index();
        if (index < 0 || static_cast<size_t>(index) >= bag->size())
            return true;

        auto item = (*bag)[index];
        if (item == nullptr) [[unlikely]]
            return true;

        if (item->stack == 0)
            item->stack = 1;
        item->change_item(value->type());

        return true;
    }
};

TL_Register_Router(_set_item_type, pro::cmd_set_item_type);

class _get_player_bag : public server_route_caller {
    bool handle(pro::confirm &confirm) noexcept override {
        auto value = as<pro::REQplayer_get_bag>();
        if (!value) [[unlikely]] {
            confirm.set_ok(false);
            confirm.set_message("解析失败");
            return true;
        }

        il2cpp::array<item *> *items = nullptr;
        if (value->me()) {
            items = get_local_player_bag();
            if (items == nullptr) [[unlikely]] {
                LOGW("本地玩家未就绪，背包暂不可用");
                confirm.set_ok(false);
                confirm.set_message("玩家未就绪");
                return true;
            }
            LOGI("get_me_bag:%zu", items->size());
        }

        if (items) {
            pro::RESplayer_get_bag res;
            for (auto item : *items) {
                if (item == nullptr) [[unlikely]]
                    continue;

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
        if (!value) [[unlikely]]
            return true;

        auto &item = value->sitem();
        auto index = value->index();
        LOGI("%s", item.DebugString().c_str());
        if (index < 0 || index > 59)
            return true;

        auto bag = get_local_player_bag();
        if (bag == nullptr) [[unlikely]] {
            LOGW("本地玩家未就绪，忽略修改物品属性");
            return true;
        }
        if (static_cast<size_t>(index) >= bag->size())
            return true;

        auto game_item = (*bag)[index];
        if (game_item == nullptr) [[unlikely]]
            return true;

        if (game_item->maxStack < item.stack())
            game_item->maxStack = 9999;
        game_item->stack = item.stack();
        game_item->scale = item.scale();
        game_item->knockBack = item.knockback();
        game_item->shoot = item.shoot();
        game_item->shootSpeed = item.shootspeed();
        game_item->damage = item.damage();
        game_item->useTime = item.usetime();

        return true;
    }
};

TL_Register_Router(_set_item_pro, pro::cmd_set_item_pro);

class _get_item_names : public server_route_caller {
    bool handle(pro::confirm &confirm) noexcept override {
        pro::RESitem_names res;
        auto localized_texts = lang::get_item_localized_texts();
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

TL_Register_Router(_get_item_names, pro::cmd_get_item_name);

} // namespace func
} // namespace tl