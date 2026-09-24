#pragma once

#include "packet.pb.h"

#include "shatl/network/server.h"
#include "shatl/network/socket.h"
#include "shatl/utils/log.h"
#include <cassert>
#include <memory>
#include <optional>
#include <string_view>
#include <unordered_map>

namespace tl {
namespace func {
class server_route_caller {
    const pro::packet *packet = nullptr;

    friend class server_body;

  public:
    template <typename T> std::optional<T> as() noexcept {
        if (packet == nullptr) [[unlikely]]
            return std::nullopt;

        T to_packet;
        if (to_packet.ParseFromArray(packet->data().data(),
                                     packet->data().size()))
            return to_packet;

        return std::nullopt;
    }

    virtual bool handle() { return false; }
    virtual bool handle(pro::confirm &confirm) noexcept { return false; }
};

class server_body {
    net::socket *socket = nullptr;
    pro::packet packet_msg;
    pro::confirm confirm_msg;

  public:
    inline static std::unordered_map<pro::pk_cmd,
                                     std::unique_ptr<server_route_caller>>
        routes;

    void on_accept(net::socket &s) noexcept { socket = &s; }
    void on_data(const pro::packet &packet) noexcept {
        packet_msg.Clear();
        confirm_msg.Clear();

        LOGI("cmd:%d", packet.cmd());
        packet_msg.set_cmd(pro::pk_cmd::cmd_confirm);
        packet_msg.set_seq(packet.seq());

        if (handle_message(packet)) {
        }
        echo_message();
    }

  private:
    void echo_message() noexcept {
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

    bool handle_message(const pro::packet &packet) noexcept {
        auto it = routes.find(packet.cmd());
        if (it == routes.end()) {
            LOGI("cmd:%d not found caller", packet.cmd());
            return false;
        }

        auto &caller = it->second;
        caller->packet = &packet;

        if (caller->handle(confirm_msg))
            return true;

        confirm_msg.set_message("ok");
        confirm_msg.set_ok(true);

        if (caller->handle())
            return true;

        confirm_msg.set_message("error");
        confirm_msg.set_ok(false);

        LOGE("handle error");
        return false;
    }
};

template <typename Caller> struct __register_router {
    explicit __register_router(pro::pk_cmd cmd,
                               std::string_view debug_class_name) noexcept {
        auto [_, inserted] =
            server_body::routes.emplace(cmd, std::make_unique<Caller>());

        LOGI("register router cmd=%d inserted=%d class=%s",
             static_cast<int>(cmd), inserted, debug_class_name.data());
    }
};

#define TL_Register_Router(caller, cmd)                                        \
    static ::tl::func::__register_router<caller> reg_router__##caller {        \
        cmd, #caller                                                           \
    }

using server = net::basic_server<server_body>;
} // namespace func
} // namespace tl