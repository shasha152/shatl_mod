#include "shatl/il2cpp/il2cpp.h"
#include "shatl/utils/log.h"

#include <fstream>
#include <iterator>
#include <nlohmann/json.hpp>
#include <string>
#include <string_view>

namespace tl {
namespace func {
class config {
    inline static nlohmann::json default_config = {
        {"player.lifefrezze", false},
        {"player.manafrezze", false},
        {"player.instantkill", false},
        {"world.double_click_minimap_to_teleport", false},
        {"world.highlight_entire_map", false}};
    inline static std::string config_path;

    inline static std::fstream file;
    inline static nlohmann::json data;

  public:
    inline static void init() noexcept {
        config_path =
            il2cpp::object<il2cpp::tag<1>>::static_call2<il2cpp::string *>(
                "UnityEngine.CoreModule.dll", "UnityEngine", "Application",
                "get_persistentDataPath")
                ->to_string() +
            "/shati_config.json";

        LOGI("private_path:%s", config_path.c_str());

        file.open(config_path);
        if (!file.is_open()) {
            new_file();
            write_default_josn();
        } else {
            std::string sdata(std::istreambuf_iterator<char>(file.rdbuf()),
                              std::istreambuf_iterator<char>());
            data = nlohmann::json::parse(sdata.begin(), sdata.end(), nullptr,
                                         false);
            if (data.is_discarded()) {
                LOGW("josn解析失败");
                write_default_josn();
            }
            file.flush();
        }

        check();
    }

    nlohmann::json &operator[](std::string_view key) {
        if (!data.contains(key))
            LOGE("%s 不存在这个key", key.data());
        return data[key];
    } // namespace func

    void save() noexcept {
        new_file();
        auto sdata = data.dump();
        file.write(sdata.c_str(), sdata.size());
        file.flush();
    }

  private:
    static void new_file() noexcept {
        file.close();
        file.open(config_path,
                  std::ios::out | std::ios::binary | std::ios::trunc);

        if (!file.is_open()) {
            LOGI("文件创建失败: %s", config_path.c_str());
            return;
        }
    }

    static void write_default_josn() {
        data = default_config;
        auto sdata = data.dump();
        file.write(sdata.c_str(), sdata.size());
    }

    static void check() {
        for (auto &[k, _] :
             default_config.get_ref<nlohmann::json::object_t &>()) {
            if (!data.contains(k)) {
                LOGI("config文件出错");
                new_file();
                write_default_josn();
                break;
            }
        }
    }
}; // namespace tl
} // namespace func
} // namespace tl