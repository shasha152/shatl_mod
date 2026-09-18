#pragma once

#include "packet.pb.h"
#include "shatl/utils/log.h"
#include "socket.h"
#include <cstdint>
#include <utility>
#include <vector>

namespace tl {
namespace net {

// namespace detail

template <typename Server> class basic_server {
    Server server;
    pro::packet packet;
    socket listen_socket;
    std::vector<char> buffer;

  public:
    using server_type = Server;

    template <typename... Args>
    basic_server(std::uint16_t port, Args &&...args) noexcept
        : server(std::forward<Args>(args)...) {
        auto [s, e] = socket::localhsot(port);
        if (e)
            LOGW("%s", e.message().c_str());

        listen_socket = std::move(s.value());

        e = listen_socket.listen();
        if (e)
            LOGW("%s", e.message().c_str());
    }
    void run() noexcept {
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