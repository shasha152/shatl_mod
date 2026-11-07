#pragma once

#include "packet.pb.h"
#include "shatl/utils/log.h"
#include "socket.h"
#include "websocket.h"
#include <cstdint>
#include <thread>
#include <utility>
#include <vector>

namespace tl {
namespace net {

template <typename Server> class websocket_server {
  public:
    struct CMDConnData {
        bool login;
    };
    using WSServer = websocket::WSServer<websocket_server, CMDConnData>;
    using WSConn = WSServer::Connection;

    void run() {
        if (!wsserver.init("0.0.0.0", 1234)) {
            std::cout << "wsserver init failed: " << wsserver.getLastError()
                      << std::endl;
            return;
        }

        running = true;
        ws_thr = std::thread([this]() {
            while (running.load(std::memory_order_relaxed)) {
                wsserver.poll(this);
                std::this_thread::yield();
            }
        });

        std::cout << "Server running..." << std::endl;
        ws_thr.join();
        std::cout << "Server stopped..." << std::endl;
    }

    void stop() { running = false; }

    bool onWSConnect(WSConn &conn, const char *request_uri, const char *host,
                     const char *origin, const char *protocol,
                     const char *extensions, char *resp_protocol,
                     uint32_t resp_protocol_size, char *resp_extensions,
                     uint32_t resp_extensions_size) {
        struct sockaddr_in addr;
        conn.getPeername(addr);
        std::cout << "ws connection from: " << inet_ntoa(addr.sin_addr) << ":"
                  << ntohs(addr.sin_port) << std::endl;
        std::cout << "request_uri: " << request_uri << std::endl;
        std::cout << "host: " << host << std::endl;
        if (origin)
            LOGI("origin:%s", origin);

        if (protocol)
            LOGI("protocol:%s", protocol);

        if (extensions)
            LOGI("extensions:%s", extensions);

        return true;
    }

    // called when a websocket connection is closed
    // status_code 1005 means no status code in the close msg
    // status_code 1006 means not a clean close(tcp connection closed without a
    // close msg)
    void onWSClose(WSConn &conn, uint16_t status_code, const char *reason) {
        std::cout << "ws close, status_code: " << status_code
                  << ", reason: " << reason << std::endl;
    }

    // onWSMsg is used if RecvSegment == false(by default), called when a whole
    // msg is received
    void onWSMsg(WSConn &conn, uint8_t opcode, const uint8_t *payload,
                 uint32_t pl_len) {
        if (opcode == websocket::OPCODE_PING) {
            conn.send(websocket::OPCODE_PONG, payload, pl_len);
            return;
        }
        if (opcode != websocket::OPCODE_TEXT) {
            conn.close(1003, "not text msg");
            return;
        }
    }

    void onWSSegment(WSConn &conn, uint8_t opcode, const uint8_t *payload,
                     uint32_t pl_len, uint32_t pl_start_idx, bool fin) {
        std::cout << "error: onWSSegment should not be called" << std::endl;
    }

  private:
    WSServer wsserver;
    Server server;
    std::thread ws_thr;
    std::atomic<bool> running;
};

template <typename Server> class basic_server {
    Server server;
    pro::packet packet;
    socket listen_socket;
    std::vector<char> buffer;
    bool is_ready = false;

  public:
    using server_type = Server;

    template <typename... Args>
    basic_server(std::uint16_t port, Args &&...args) noexcept
        : server(std::forward<Args>(args)...) {
        auto [s, e] = socket::localhost(port);
        if (e) {
            LOGE("监听 127.0.0.1:%u 失败: %s", port, e.message().c_str());
            return;
        }

        listen_socket = std::move(s.value());

        e = listen_socket.listen();
        if (e) {
            LOGE("listen 失败: %s", e.message().c_str());
            return;
        }

        is_ready = true;
    }
    void run() noexcept {
        if (!is_ready) [[unlikely]] {
            LOGE("服务未就绪，跳过启动");
            return;
        }

        while (true) {
            auto [s, e] = listen_socket.accept();
            if (e) {
                LOGW("%s", e.message().c_str());
                break;
            }
            LOGI("建立连接");
            server.on_accept(s.value());

            while (true) {
                if (read_packet(s.value()))
                    server.on_data(packet);
                else
                    break;
            }
        }
        LOGI("关闭服务器");
    }

  private:
    bool read_packet(socket &s) {
        std::int32_t network_size = 0;
        auto e = s.read(&network_size, 4);
        if (e) {
            LOGW("%s", e.message().c_str());
            return false;
        }
        const std::int32_t size = ntohl(network_size);
        buffer.resize(size);

        e = s.read(buffer.data(), buffer.size());
        if (e) {
            LOGW("%s", e.message().c_str());
            return false;
        }

        return packet.ParseFromArray(buffer.data(), buffer.size());
    }
};
} // namespace net
} // namespace tl