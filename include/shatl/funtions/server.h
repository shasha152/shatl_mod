#pragma once

#include "config.h"
#include "packet.pb.h"
#include "shatl/network/server.h"
#include "shatl/network/socket.h"
#include "shatl/utils/log.h"
#include <cassert>
#include <utility>

namespace tl {
namespace func {
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
        LOGI("发送:%zu", data.size());
        std::uint32_t length = htonl(data.size());
        auto res = socket->write(&length, 4);
        if (res)
            LOGW("%s", res.message().c_str());
        res = socket->write(data.data(), data.size());
        if (res)
            LOGW("%s", res.message().c_str());
    };

    bool handle_message(const pro::packet &packet) {
        switch (packet.cmd()) {
        case pro::cmd_player_max_value: {
            pro::player_max_value _;
            if (_.ParseFromArray(packet.data().data(), packet.data().size())) {
                handle_value(_);
            } else
                LOGW("解析失败");

            break;
        }
        case pro::cmd_player_float_value: {
            pro::player_float_value _;
            if (_.ParseFromArray(packet.data().data(), packet.data().size())) {
                handle_value(_);
            } else
                LOGW("解析失败");

            break;
        }
        case pro::cmd_confirm:
        case pro::cmd_verify:
        default:
            break;
        }

        confirm_msg.set_ok(true);
        confirm_msg.set_message("ok");
        return true;
    }

    bool handle_value(const pro::player_float_value &value) {
        auto &data = config::ins().float_value[value.type()];
        data.value = value.value();
        data.is_open = value.is_open();

        return true;
    }

    bool handle_value(const pro::player_max_value &value) {
        auto &data = config::ins().max_value[value.type()];
        data.value = value.value();
        data.max = value.max();
        data.is_open = value.is_open();

        return true;
    }
};

using server = net::basic_server<server_body>;
} // namespace func
} // namespace tl