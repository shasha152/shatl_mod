#include "shatl/funtions/config.h"
#include "shatl/funtions/detail/lang.h"
#include "shatl/funtions/detail/player.h"
#include "shatl/funtions/detail/world.h"
#include "shatl/imgui/def.h"
#include "shatl/imgui/detail/gui.h"
#include "shatl/utils/log.h"

#include <backends/imgui_impl_android.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>

namespace tl {
namespace im {

inline int width = 0;
inline int height = 0;

#if IS_ENABLE_DEBUG_GUI
void detail::draw_main_gui(int w, int h) noexcept {
    width = w;
    height = h;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplAndroid_NewFrame(w, h);
    ImGui::NewFrame();

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
            auto arr = func::lang::get_item_localized_texts();
            for (auto txt : *arr) {
                LOGI("%s:%s", txt->key->to_string().c_str(),
                     txt->value->to_string().c_str());
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
    }
    ImGui::End();
    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
#else
void detail::draw_main_gui(int w, int h) noexcept {
    width = w;
    height = h;
}
#endif

int detail::get_screen_width() noexcept { return width; }
int detail::get_screen_height() noexcept { return height; }

} // namespace im
} // namespace tl