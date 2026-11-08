#pragma once

#include "ixwebsocket/IXWebSocket.h"
#include "ixwebsocket/IXWebSocketMessageType.h"
#include "ixwebsocket/IXWebSocketServer.h"
#include "shatl/utils/log.h"
#include <cstdint>
#include <functional>
#include <string>
#include <thread>
#include <utility>

#include "packet.pb.h"

namespace tl {
namespace func {
class server_route_caller {
    const pro::packet *packet = nullptr;

    friend class websocket_server;

  public:
    template <typename T> std::optional<T> as() noexcept {
        if (packet == nullptr) [[unlikely]]
            return std::nullopt;

        T to_packet;
        if (to_packet.ParseFromArray(packet->data().data(),
                                     packet->data().size()))
            return to_packet;

        LOGW("解析失败");

        return std::nullopt;
    }

    virtual bool handle() { return false; }
    virtual bool handle(pro::confirm &confirm) noexcept { return false; }
};

class websocket_server {
  public:
    inline static std::unordered_map<
        pro::pk_cmd, std::function<std::unique_ptr<server_route_caller>()>>
        routes;
    explicit websocket_server(std::uint16_t port) noexcept
        : server(port, "127.0.0.1") {
        server.setOnClientMessageCallback(
            [this](std::shared_ptr<ix::ConnectionState> state,
                   ix::WebSocket &conn,
                   const ix::WebSocketMessagePtr &message) {
                if (message == nullptr) {
                    LOGI("message=null");
                    return;
                }

                switch (message->type) {
                case ix::WebSocketMessageType::Open:
                    LOGI("client connected:%s", conn.getUrl().c_str());
                    break;
                case ix::WebSocketMessageType::Close:
                    LOGI("client disconnected");
                    break;
                case ix::WebSocketMessageType::Message:
                    if (message->binary) {
                        onWSMsg(conn, message->str);
                    } else {
                        LOGW("expect message type is bindary");
                    }
                    break;
                case ix::WebSocketMessageType::Error:
                    LOGW("%s", message->errorInfo.reason.c_str());
                    break;
                default:
                    break;
                }
            });
    }
    void run() {
        auto result = server.listen();
        if (!result.first) {
            LOGE("WebSocket listen failed: %s", result.second.c_str());
            return;
        }

        LOGI("WebSocket listen success");
        server.start();
    }
    void stop() {
        LOGI("服务停止");
        server.stop();
    }

    void onWSMsg(ix::WebSocket &conn, const std::string &str) {
        pro::packet packet;
        pro::confirm conf;
        if (packet.ParseFromArray(str.data(), str.size())) {
            handle_message(packet, conf);

            packet.set_cmd(pro::cmd_confirm);
            packet.set_data(conf.SerializeAsString());

        } else {
            LOGW("parse packet failed");
            set_packet_error(packet, "parse packet failed");
        }
        conn.sendBinary(packet.SerializeAsString());
    }

  private:
    void set_packet_error(pro::packet &packet,
                          const std::string &err) noexcept {
        pro::confirm cof;
        cof.set_ok(false);
        cof.set_message(err);

        packet.set_data(cof.SerializeAsString());
        packet.set_cmd(pro::cmd_confirm);
    }

    void handle_message(pro::packet &packet,
                        pro::confirm &confirm_msg) noexcept {

        auto it = routes.find(packet.cmd());
        if (it == routes.end()) {
            LOGI("cmd:%d not found caller", packet.cmd());
            return;
        }

        auto caller = it->second();
        caller->packet = &packet;

        if (caller->handle(confirm_msg))
            return;

        confirm_msg.set_message("ok");
        confirm_msg.set_ok(true);

        if (caller->handle())
            return;

        confirm_msg.set_message("error");
        confirm_msg.set_ok(false);

        LOGE("handle error");
    }
    ix::WebSocketServer server;
    std::thread ws_thr;
    std::atomic<bool> running;
};

template <typename Caller> struct __register_router {
    explicit __register_router(pro::pk_cmd cmd,
                               std::string_view debug_class_name) noexcept {
        auto [_, inserted] = websocket_server::routes.emplace(
            cmd, []() { return std::make_unique<Caller>(); });

        LOGI("register router cmd=%d inserted=%d class=%s",
             static_cast<int>(cmd), inserted, debug_class_name.data());
    }
};
#define TL_Register_Router(caller, cmd)                                        \
    static ::tl::func::__register_router<caller> reg_router__##caller {        \
        cmd, #caller                                                           \
    }
} // namespace func
} // namespace tl