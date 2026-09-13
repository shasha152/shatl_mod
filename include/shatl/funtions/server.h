#pragma once

#include "packet.pb.h"
#include "shatl/network/server.h"
#include "shatl/network/socket.h"
#include "shatl/utils/log.h"
#include <cassert>

namespace tl {
namespace func {
class server_body {
    net::socket *socket = nullptr;

  public:
    void on_accept(net::socket &s) noexcept { socket = &s; }
    void on_data(const pro::packet &packet) noexcept {
        assert(s != nullptr);
        LOGI("cmd:%d", packet.cmd());
    }
};

using server = net::basic_server<server_body>;
} // namespace func
} // namespace tl