#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <sys/types.h>
#include <system_error>
#include <utility>
namespace tl {
namespace net {
class socket {
    int sock = -1;

  public:
    explicit socket(const socket &) = delete;
    socket &operator=(const socket &) = delete;

    socket(socket &&other) noexcept : sock(std::exchange(other.sock, -1)) {}
    socket &operator=(socket &&other) noexcept {
        if (this != &other) {
            sock = std::exchange(other.sock, -1);
        }

        return *this;
    }

    ~socket() noexcept;
    socket() = default;

    static std::pair<std::optional<socket>, std::error_code>
    localhsot(std::uint16_t port) noexcept;

    std::error_code read(void *buffer, std::size_t size) noexcept;
    std::error_code write(void *buffer, std::size_t size) noexcept;

    template <typename Tp, std::size_t Size = std::dynamic_extent>
    std::error_code read(std::span<Tp, Size> buffer) noexcept {
        return read(buffer.data(), buffer.size_bytes());
    }

    template <typename Tp, std::size_t Size = std::dynamic_extent>
    std::error_code write(std::span<Tp, Size> buffer) noexcept {
        return write(buffer.data(), buffer.size_bytes());
    }

    std::error_code listen();
    std::pair<std::optional<socket>, std::error_code> accept();
};
} // namespace net
} // namespace tl