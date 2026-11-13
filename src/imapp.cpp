#include "shatl/imgui/buffer.h"
#include "shatl/imgui/detail/gui.h"
#include "shatl/utils/log.h"

#include <backends/imgui_impl_android.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>
#include <memory>
#include <string>

#if IS_ENABLE_DEBUG_GUI
#include "shatl/funtions/config.h"
#include "shatl/funtions/detail/class/lang.h"
#include "shatl/funtions/detail/class/player.h"
#include "shatl/funtions/detail/class/world.h"
#include "shatl/funtions/detail/map_reveal.h"
#include "shatl/il2cpp/il2cpp.h"
#endif

namespace tl {
namespace im {

inline int width = 0;
inline int height = 0;

void draw_buffer(ImDrawList *draw) noexcept {
    auto &readable = double_draw_buffer::ins().readable_buffer();

    for (auto &line : readable.lines)
        draw->AddLine(line.start, line.end, ImColor(255, 255, 255));
    for (auto &line : readable.rects)
        draw->AddRect(line.start, line.end, ImColor(255, 255, 255));
    for (auto &name : readable.names)
        draw->AddText(name.start, ImColor(255, 255, 255), name.name.c_str());

    double_draw_buffer::ins().swap();
}

class imgui_fram {
  public:
    imgui_fram(int w, int h) {
        width = w;
        height = h;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplAndroid_NewFrame(w, h);
        ImGui::NewFrame();
    }

    ~imgui_fram() {
        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
};

#if IS_ENABLE_DEBUG_GUI
void detail::draw_main_gui(int w, int h) noexcept {

    static bool is_show_version = false;
    static std::string unity_version = "";

    imgui_fram fram(w, h);

    if (ImGui::Begin("Debug")) {
        if (ImGui::Button("local player bag")) {
            auto bag = func::get_local_player_bag();
            if (bag) {
                LOGI("array_size:%zu", bag->size());
                for (auto item : *bag) {
                    LOGI("type:%d,number:%d", item->type, item->stack);
                }
            }
        }
        if (ImGui::Button("item name")) {
            auto arr = func::lang::item_localized_texts();
            if (arr != nullptr) {
                for (auto txt : *arr) {
                    if (txt == nullptr || txt->value == nullptr) [[unlikely]]
                        continue;

                    LOGI("%s:%s", txt->key->to_string().c_str(),
                         txt->value->to_string().c_str());
                }
            }
        }

        if (ImGui::Button("tiandingjian")) {
            auto bag = func::get_local_player_bag();
            (*bag)[0]->stack = 1;
            func::register_once_function(
                [bag]() { (*bag)[0]->change_item(4956); });
        }

        if (ImGui::Button("auto_aim")) {
            func::config::ins().auto_aim.set_is_open(true);
            func::config::ins().auto_aim.set_npc(true);
        }

        if (ImGui::Button("local player")) {
            func::register_once_function([]() {
                auto p = func::world::local_player();
                LOGI("%p", p);
            });
        }

        if (ImGui::Button("speed")) {
            auto &data =
                func::config::ins().float_value[pro::float_value_type::speed];
            data.is_enable = true;
            data.value = 10.f;
        }

        if (ImGui::Button("show version")) {
            func::register_once_function([]() {
                unity_version = il2cpp::application::get_unity_version();
                is_show_version = true;
            });
        }

        if (ImGui::Button("New Boss")) {
            func::register_once_function([]() {
                func::npc::spawn(func::world::local_player()->position, 50, 0);
            });
        }

        if (ImGui::Button("draw e")) {
            auto &conf = func::config::ins().draw;
            conf.set_draw_line(true);
            conf.set_draw_npc(true);
            conf.set_draw_rect(true);
        }

        if (ImGui::Button("map reveal")) {
            static auto map = std::make_shared<func::map_reveal>();
            map->run();
        }

        if (is_show_version) {
            ImGui::Text("unity version:%s", unity_version.c_str());
        }
    }
    ImGui::End();
    draw_buffer(ImGui::GetBackgroundDrawList());
}
#else
void detail::draw_main_gui(int w, int h) noexcept {
    imgui_fram fram(w, h);
    draw_buffer(ImGui::GetBackgroundDrawList());
}
#endif

int detail::get_screen_width() noexcept { return width; }
int detail::get_screen_height() noexcept { return height; }

} // namespace im
} // namespace tl