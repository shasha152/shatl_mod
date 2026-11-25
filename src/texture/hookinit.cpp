#include "shatl/funtions/detail/class/texture2D.h"
#include "shatl/funtions/detail/register.h"
#include "shatl/funtions/maininit.h"
#include "shatl/il2cpp/il2cpp.h"
#include "shatl/package/package.h"
#include "shatl/utils/log.h"

#include <filesystem>
#include <optional>
#include <string>
#include <system_error>
#include <unistd.h>
#include <unordered_map>
#include <utility>
#include <vector>

namespace tl {
namespace tex {
struct assets {
    std::unordered_map<std::string, func::texture2D *> data;
    std::vector<std::string> texture_paths;

    func::texture2D *get(const std::string &str) {
        auto it = data.find(str);
        if (it != data.end())
            return it->second;

        auto f = get_file(str);

        if (!f.has_value()) {
            data[str] = nullptr;
            return nullptr;
        }

        auto tex = func::texture2D::create(f.value());
        data[str] = tex;
        return tex;
    }

    bool has_file(const std::string &p) noexcept {
        std::error_code ec;
        bool res = std::filesystem::is_regular_file(p, ec);

        return res && !ec;
    }

    std::optional<std::string> get_file(const std::string &p) noexcept {
        static auto data_path = il2cpp::application::get_data_path();

        std::optional<std::string> result;

        for (const auto &n : texture_paths) {
            auto path = n + "/" + p + ".png";

            if (has_file(path)) {
                result = std::move(path);
                break;
            }
        }
        return result;
    }

    void add_path(const std::string &p) noexcept { texture_paths.push_back(p); }
    void clear_path() noexcept { texture_paths.clear(); }
    bool is_path_empty() const noexcept { return texture_paths.empty(); }
};

inline assets g_assets{};

void load_texture(func::texture_assets::value_type val) noexcept {
    if (val == nullptr) [[unlikely]]
        return;

    auto tex = g_assets.get(val->name->to_string());

    if (tex) {
        if (val->state == 0) {
            val->call<void>("SubmitLoadedContent", tex, nullptr);
        } else {
            val->value = tex;
            val->state = 2;
        }
    }
}

void load_texture(func::texture_assets::array_type arr) noexcept {
    if (arr == nullptr) [[unlikely]]
        return;

    for (auto asset : *arr)
        load_texture(asset);
}

void update_texture_package() noexcept {
    if (g_assets.is_path_empty())
        return;

    func::register_once_function([]() {
        auto load = [](il2cpp::field *f) {
            using ta = func::texture_assets;

            if (f->name() == "Background")
                return;
            if (f->is_static()) {
                if (f->type_name() == "ReLogic.Content.Asset<Microsoft.Xna."
                                      "Framework.Graphics.Texture2D>") {
                    load_texture(f->static_value<ta::value_type>());
                } else if (f->type_name() ==
                           "ReLogic.Content.Asset<Microsoft.Xna.Framework."
                           "Graphics.Texture2D>[]") {
                    load_texture(f->static_value<ta::array_type>());
                }
            }
        };
        func::texture_assets::_main_class->for_each_field(load);
    });
}

void init() noexcept {
    func::texture2D::init();
    func::texture_assets::init();

    LOGI("资源包加载");
    auto packages = read_packages();
    for (const auto &pack : packages) {
        if (pack.is_enable()) {
            LOGI("添加资源包:%s", pack.content_path_string().c_str());
            // g_assets.add_path(pack.content_path_string());
        }
    }

    update_texture_package();
}

TL_Register_Init(PRIORITY_HIGH, init);

} // namespace tex
} // namespace tl