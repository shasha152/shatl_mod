#include "shatl/network/socket.h"
#include <cerrno>
#include <cstddef>
#include <linux/in.h>
#include <optional>
#include <span>
#include <sys/endian.h>
#include <system_error>

#include <netinet/in.h>
#include <sys/errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/unistd.h>
#include <unistd.h>
#include <utility>

namespace tl {
namespace net {

std::pair<std::optional<socket>, std::error_code>
socket::localhsot(std::uint16_t port) noexcept {
    socket msock;
    msock.sock = ::socket(AF_INET, SOCK_STREAM, 0);
    if (msock.sock == -1)
        return std::make_pair(std::nullopt,
                              std::error_code(errno, std::system_category()));

    sockaddr_in sockaddr_in{};
    sockaddr_in.sin_family = AF_INET;
    sockaddr_in.sin_port = htons(port);
    sockaddr_in.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    auto res = ::bind(msock.sock, reinterpret_cast<sockaddr *>(&sockaddr_in),
                      sizeof(sockaddr_in));
    if (res == -1)
        return std::make_pair(std::nullopt,
                              std::error_code(errno, std::system_category()));

    return std::make_pair(std::make_optional(std::move(msock)),
                          std::error_code());
}
socket::~socket() noexcept {
    if (sock != -1)
        close(sock);
}
std::error_code socket::listen() {
    auto res = ::listen(sock, SOMAXCONN);
    if (res == -1)
        return std::error_code(errno, std::system_category());

    return std::error_code();
}

std::pair<std::optional<socket>, std::error_code> socket::accept() {
    sockaddr_in sockaddr_in{};
    socklen_t len = sizeof(sockaddr);

    auto res = ::accept(sock, reinterpret_cast<sockaddr *>(&sockaddr_in), &len);
    if (res == -1)
        return std::make_pair(std::nullopt,
                              std::error_code(errno, std::system_category()));

    socket msock;
    msock.sock = res;
    return std::make_pair(std::make_optional(std::move(msock)),
                          std::error_code());
}

std::error_code socket::read(void *buffer, std::size_t size) noexcept {
    std::span<char> buffers(reinterpret_cast<char *>(buffer), size);
    std::size_t total = 0;

    while (total < size) {
        auto data = buffers.subspan(total);
        auto len = ::read(sock, data.data(), data.size_bytes());
        if (len == -1)
            return std::error_code(errno, std::system_category());
        if (len == 0)
            return std::make_error_code(std::errc::connection_reset);

        total += len;
    }

    return std::error_code();
}
std::error_code socket::write(void *buffer, std::size_t size) noexcept {
    std::span<char> buffers(reinterpret_cast<char *>(buffer), size);
    std::size_t total = 0;

    while (total < size) {
        auto data = buffers.subspan(total);
        auto len = ::write(sock, data.data(), data.size_bytes());
        if (len == -1)
            return std::error_code(errno, std::system_category());
        if (len == 0)
            return std::make_error_code(std::errc::connection_reset);

        total += len;
    }

    return std::error_code();
}
} // namespace net
} // namespace tl