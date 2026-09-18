#pragma once

#include "config.h"
#include "packet.pb.h"
#include "player/data.h"
#include "player/main.h"
#include "setting.pb.h"
#include "shatl/funtions/item/data.h"
#include "shatl/funtions/lang/data.h"
#include "shatl/il2cpp/il2cpp.h"
#include "shatl/network/server.h"
#include "shatl/network/socket.h"
#include "shatl/utils/log.h"
#include <cassert>
#include <type_traits>
#include <utility>

namespace tl {
namespace func {

template <typename T> struct res_packet {
    T packet;
    bool completed;
    pro::pk_cmd cmd;

    explicit operator bool() const noexcept { return completed; }
};
namespace detail {
template <typename T> inline constexpr bool is_res_packet = false;

template <typename T> inline constexpr bool is_res_packet<res_packet<T>> = true;
} // namespace detail

class server_body {
    net::socket *socket = nullptr;
    pro::packet packet_msg;
    pro::confirm confirm_msg;

  public:
    void on_accept(net::socket &s) noexcept { socket = &s; }
    void on_data(const pro::packet &packet) noexcept {
        assert(s != nullptr);
        LOGI("cmd:%d", packet.cmd());
        packet_msg.set_cmd(pro::pk_cmd::cmd_confirm);
        packet_msg.set_seq(packet.seq());

        if (handle_message(packet)) {
        }
        echo_message();
        LOGI("packet_msg:%s", packet.Utf8DebugString().c_str());
    }

  private:
    void echo_message() {
        packet_msg.set_data(confirm_msg.SerializeAsString());

        auto data = packet_msg.SerializeAsString();
        std::uint32_t length = htonl(data.size());
        auto res = socket->write(&length, 4);
        if (res)
            LOGW("%s", res.message().c_str());
        res = socket->write(data.data(), data.size());
        if (res)
            LOGW("%s", res.message().c_str());
    };

    bool handle_message(const pro::packet &packet) {
        bool res = false;

        switch (packet.cmd()) {
        case pro::cmd_player_max_value:
            res = handle_message_parse_packet<pro::REQplayer_max_value>(packet);
            break;
        case pro::cmd_player_float_value:
            res = handle_message_parse_packet<pro::REQfloat_value>(packet);
            break;
        case pro::cmd_player_bool_value:
            res = handle_message_parse_packet<pro::REQbool_value>(packet);
            break;
        case pro::cmd_player_get_bag:
            res = handle_message_parse_packet<pro::REQplayer_get_bag>(packet);
            break;
        case pro::cmd_verify:
        default:
            break;
        }

        confirm_msg.set_message("ok");
        return true;
    }

    template <typename Packet>
    bool handle_message_parse_packet(const pro::packet &packet) {
        Packet pk;

        if (pk.ParseFromArray(packet.data().data(), packet.data().size())) {
            packet_handler<Packet> handler;
            using handler_result_type =
                std::invoke_result_t<packet_handler<Packet>, const Packet &>;
            if constexpr (std::is_void_v<handler_result_type>) {
                handler(pk);
                confirm_msg.set_ok(true);
                return true;
            }
            if constexpr (detail::is_res_packet<handler_result_type>) {
                auto [res_pak, completed, cmd] = handler(pk);
                confirm_msg.set_ok(completed);
                confirm_msg.set_cmd(cmd);
                confirm_msg.set_data(res_pak.SerializeAsString());

                return true;
            }
        }

        return false;
    }
};

template <> struct packet_handler<pro::REQbool_value> {
    void operator()(const pro::REQbool_value &value) noexcept {
        config::ins().bool_value[value.type()] = value.is_open();
    }
};

template <> struct packet_handler<pro::REQfloat_value> {
    void operator()(const pro::REQfloat_value &value) noexcept {
        auto &data = config::ins().float_value[value.type()];
        data.value = value.value();
        data.is_enable = value.is_open();
    }
};

template <> struct packet_handler<pro::REQplayer_max_value> {
    void operator()(const pro::REQplayer_max_value &value) noexcept {
        auto &data = config::ins().max_value[value.type()];
        data.value = value.value();
        data.is_enable = value.is_open();
        data.max = value.max();
    }
};

template <> struct packet_handler<pro::REQplayer_get_bag> {
    res_packet<pro::RESplayer_get_bag>
    operator()(const pro::REQplayer_get_bag &value) noexcept {
        res_packet<pro::RESplayer_get_bag> resMsg;
        resMsg.cmd = pro::cmd_player_get_bag;

        il2cpp::array<item *> *items = nullptr;
        if (value.me()) {
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
                res_item->set_name(
                    (*lang::get_item_localized_texts())[item->type]
                        ->value->to_string());
            }
            resMsg.packet = std::move(res);
            resMsg.completed = true;
        } else {
            resMsg.completed = false;
        }

        return resMsg;
    }
};

using server = net::basic_server<server_body>;
} // namespace func
} // namespace tl