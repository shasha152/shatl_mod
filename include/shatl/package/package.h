#pragma once

#include "shatl/utils/log.h"
#include <exception>
#include <filesystem>
#include <optional>
#include <string>
#include <system_error>
#include <vector>

namespace tl {

namespace detail {
inline bool is_useable_package(const std::string &path) {
    return std::filesystem::is_regular_file(path + "/pack.json") &&
           std::filesystem::is_regular_file(path + "/icon.png") &&
           std::filesystem::is_directory(path + "/Content");
}

inline constexpr auto default_package_path =
    "/storage/emulated/0/Documents/shatl_assets/";
inline constexpr auto default_config_file = ".enable";
} // namespace detail
class package {
    bool _is_enable;
    bool _has_error;

    std::string _path;

  public:
    explicit package(const std::string &path) noexcept
        : _is_enable(false), _has_error(false), _path(path) {
        check_enable();
    }

    std::string content_path_string() const noexcept {
        return _path + "/Content";
    }
    bool is_enable() const noexcept { return _is_enable; }
    bool has_error() const noexcept { return _has_error; }

  private:
    void check_enable() noexcept {
        _is_enable =
            std::filesystem::exists(_path + "/" + detail::default_config_file);
    }
};

inline std::optional<package> make_package(const std::string &path) noexcept {
    bool is_useable = false;
    try {
        is_useable = detail::is_useable_package(path);
    } catch (std::exception err) {
        LOGI("%s", err.what());
    }

    if (!is_useable)
        return std::nullopt;

    package p(path);
    if (p.has_error())
        return std::nullopt;

    return p;
}

inline std::vector<package> read_packages() noexcept {
    std::vector<package> result;

    std::error_code ec;
    auto has = std::filesystem::exists(detail::default_package_path, ec);
    if (!has)
        return result;
    if (ec) {
        LOGI("%s", ec.message().c_str());
        return result;
    }

    for (auto path :
         std::filesystem::directory_iterator(detail::default_package_path)) {
        if (!path.is_directory())
            continue;

        auto pk = make_package(path.path());
        if (pk)
            result.push_back(pk.value());
    }

    return result;
}
} // namespace tl